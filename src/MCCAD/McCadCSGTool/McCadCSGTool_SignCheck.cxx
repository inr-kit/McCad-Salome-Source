#include <McCadCSGTool_SignCheck.ixx>

#include <McCadTDS_HSequenceOfExtFace.hxx>
#include <McCadGTOOL.hxx>
#include <McCadCSGTool.hxx>

#include <TColStd_HSequenceOfReal.hxx>
#include <TColgp_HSequenceOfPnt.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pln.hxx>
#include <gp_Trsf.hxx>
#include <gp_Pln.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_TFace.hxx>

#include <BRepTools_TrsfModification.hxx>
#include <BRepTools_Modifier.hxx>

#include <BRepTools.hxx>

#include <Bnd_Box2d.hxx>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TCollection_AsciiString.hxx>

#include <TopoDS_Vertex.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

#include <McCadCSGGeom_Surface.hxx>
#include <McCadCSGGeom_Plane.hxx>
#include <McCadCSGGeom_Cylinder.hxx>
#include <McCadCSGGeom_Cone.hxx>
#include <McCadCSGGeom_Sphere.hxx>
#include <McCadCSGGeom_Torus.hxx>
#include <McCadCSGTool_Extender.hxx>
#include <McCadCSGTool_FaceFuser.hxx>
#include <McCadCSGGeom_SurfaceOfRevolution.hxx>
#include <McCadMessenger_Singleton.hxx>

McCadCSGTool_SignCheck::McCadCSGTool_SignCheck()
{
	mySignIsConstant = Standard_False;
        myIsDone = Standard_False;
}


McCadCSGTool_SignCheck::McCadCSGTool_SignCheck(const TopoDS_Shape& theSolid)
{
	mySignIsConstant = Standard_False;
        myIsDone = Standard_False;
	Init(theSolid);
}


void McCadCSGTool_SignCheck::Init(const TopoDS_Shape& theSolid)
{
	NotDone();

	mySolid = theSolid;
	myExtFaceIndex = 0;
	myAllExtFace = new McCadTDS_HSequenceOfExtFace();

        //append sample points of the solid
	mySamplePnt = new TColgp_HSequenceOfPnt;
        mySamplePnt->Append(McCadCSGTool::SamplePoints(mySolid));

	Classify();
	Done();
}


Standard_Boolean McCadCSGTool_SignCheck::IsDone() const
{
	return myIsDone;
}


void McCadCSGTool_SignCheck::Done()
{
	myIsDone = Standard_True;
}


void McCadCSGTool_SignCheck::NotDone()
{
	myIsDone = Standard_False;
}


Handle(McCadTDS_ExtFace) McCadCSGTool_SignCheck::GetExtFace()
{
	if(myAllExtFace->Length()== 0 )
		return 0;

	myExtFaceIndex++;
	//myExtFace = Rank(myAllExtFace);
	myExtFace = myAllExtFace->Value(myExtFaceIndex);

	AdjustFaceUV();

	return myExtFace;
}


Standard_Integer McCadCSGTool_SignCheck::NbExtFace() const
{
	return myAllExtFace->Length() - myExtFaceIndex;
}


void McCadCSGTool_SignCheck::Classify() {
    McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
    Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt;
    thePntSeq->Append(mySamplePnt); //All Sample Points of current Solid

    if (!thePntSeq->Length()) {
            msgr->Message("_#_McCadCSGTool_SignCheck.cxx :: No sample points available",
                          McCadMessenger_ErrorMsg);
            NotDone();
            return;
    }
    Handle(McCadTDS_HSequenceOfExtFace) aResultantSeq = new McCadTDS_HSequenceOfExtFace;
    Handle(McCadTDS_HSequenceOfExtFace) aSignFaceSeq = new McCadTDS_HSequenceOfExtFace;

    // fuse nonlinear FACES BEFORE GENERATING RESULTANTS
    Handle(TopTools_HSequenceOfShape) faceSeq = new TopTools_HSequenceOfShape;
    Handle(TopTools_HSequenceOfShape) planeSeq = new TopTools_HSequenceOfShape;

    // sort out planar faces, delete redundant planes
    TopLoc_Location loc;
    //For all Faces of current solid
    for (TopExp_Explorer ex(mySolid, TopAbs_FACE); ex.More(); ex.Next()) {
        TopoDS_Face exFace = TopoDS::Face(ex.Current());

        const Handle(Geom_Surface)& exFaceSurface = BRep_Tool::Surface(exFace,loc);
        GeomAdaptor_Surface adaptorSurface(exFaceSurface);

        // remove planar faces that ly on the same surface;; only the surface is important for sign constancy
        // no need for fusing for planar faces!!!
        //////////////////////////////
        if(adaptorSurface.GetType() == GeomAbs_Plane)
        {
            Standard_Boolean sameSurfaceFound(Standard_False);

            gp_Pln thePln = adaptorSurface.Plane();
            Standard_Real sA, sB, sC, sD;
            thePln.Coefficients(sA, sB, sC, sD);

            for(Standard_Integer i=1; i<= planeSeq->Length(); i++) {
                TopoDS_Face otherFace = TopoDS::Face(planeSeq->Value(i));
                const Handle(Geom_Surface)& otherSurface = BRep_Tool::Surface(otherFace,loc);
                GeomAdaptor_Surface otherAdaptorSurface(otherSurface);

                gp_Pln otherPln = otherAdaptorSurface.Plane();
                Standard_Real oA, oB, oC, oD;
                otherPln.Coefficients(oA, oB, oC, oD);

                Standard_Real localTolerance = 1.e-5;
                if( ( Abs( oA - sA ) < localTolerance &&
                      Abs( oB - sB ) < localTolerance &&
                      Abs( oC - sC ) < localTolerance &&
                      Abs( oD - sD ) < localTolerance ) ||
                    ( Abs( oA + sA ) < localTolerance &&
                      Abs( oB + sB ) < localTolerance &&
                      Abs( oC + sC ) < localTolerance &&
                      Abs( oD + sD ) < localTolerance ) )
                {
                    sameSurfaceFound = Standard_True;
                    break;
                }
            }
            if(!sameSurfaceFound)
                planeSeq->Append(exFace);
        }
        else
            faceSeq->Append(exFace);
    }

    // fuse non planar faces
    McCadCSGTool_FaceFuser faceFuser(faceSeq);
    faceSeq->Clear();
    faceSeq->Append(faceFuser.GetFused());
    faceSeq->Append(planeSeq);

    // test for all faces if extension of boundary support set is neccessary
    for(int i=1; i<=faceSeq->Length(); i++) {
        TopoDS_Face theFace = TopoDS::Face(faceSeq->Value(i));
        Handle(McCadTDS_ExtFace) extFace = new McCadTDS_ExtFace(theFace);
        TopLoc_Location L;
        BRepAdaptor_Surface BS(theFace, Standard_True);
        TopAbs_Orientation orient = theFace.Orientation();
        Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(theFace,L);
        GeomAdaptor_Surface theAdaptFaceSurface = BS.Surface();

        // Evaluate Sample Points for current face
        for (Standard_Integer i = 1; i <= thePntSeq->Length(); i++) {
            Standard_Real aVal = McCadGTOOL::Evaluate(theAdaptFaceSurface, thePntSeq->Value(i)); // get result of Face Equation for current Sample Point

            if (aVal >  1.e-6)
                extFace->AppendPosPnt(thePntSeq->Value(i));
            if (aVal < -1.e-6)
                extFace->AppendNegPnt(thePntSeq->Value(i));
        }

        if( extFace->GetNegPoints()->Length() < 1 &&
            extFace->GetPosPoints()->Length() < 1 )
            msgr->Message("_#_McCadCSGTool_SignCheck :: no positive and no negative points found\n");

        // define orientation of cut face
        Standard_Integer NbPosPnt = extFace->NbPosPnt();
        Standard_Integer NbNegPnt = extFace->NbNegPnt();
        if ((orient != TopAbs_FORWARD) && (orient != TopAbs_REVERSED)) {
            msgr->Message("_#_McCadCSGTool_SignCheck.cxx :: Warning: Face orientation heuristically.",
                          McCadMessenger_WarningMsg);
            if (NbPosPnt > NbNegPnt) {
                    theFace.Orientation(TopAbs_REVERSED);
                    extFace->SetFace(theFace);
            }
            else if (NbPosPnt < NbNegPnt) {
                    theFace.Orientation(TopAbs_FORWARD);
                    extFace->SetFace(theFace);
            }
            else {
                theFace.Orientation(TopAbs_FORWARD);
                extFace->SetFace(theFace);
                msgr->Message("_#_McCadCSGTool_SignCheck : Face orientation heuristically.", McCadMessenger_WarningMsg);
            }
        }

        // determine if cut face is extending the boundary support set
        if (orient == TopAbs_FORWARD && NbPosPnt > 0)
            extFace->SetHaveExt(Standard_True);
        else if (orient == TopAbs_REVERSED && NbNegPnt > 0)
            extFace->SetHaveExt(Standard_True);
        else {
            extFace->SetHaveExt(Standard_False);
        }

        if (extFace->HaveExt())
                aSignFaceSeq->Append(extFace);
    }

    // compute resultants for non-linear cut faces
    for(int i=1; i<=aSignFaceSeq->Length(); i++) {
        TopoDS_Face aF = (aSignFaceSeq->Value(i))->GetFace();
        BRepAdaptor_Surface aBS(aF, Standard_True);
        GeomAdaptor_Surface aAS = aBS.Surface();

        if (aAS.GetType() == GeomAbs_Plane) // append only planar faces to myAllExtFace
            myAllExtFace->Append(aSignFaceSeq->Value(i));
        else // compute resultants for non linear faces
            myAllExtFace->Append(SingChangingResultants(aSignFaceSeq->Value(i)));
    }

    // sort cut faces by area content
    myExtFace = Rank(myAllExtFace);

    if (myAllExtFace->Length() > 0) {
        mySignIsConstant = Standard_True;
        return;
    }
    else {
        mySignIsConstant = Standard_False;
        return;
    }
}


Handle(McCadTDS_HSequenceOfExtFace) McCadCSGTool_SignCheck::SingChangingResultants(const Handle(McCadTDS_ExtFace)& theExtFace) const
{
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    Handle(McCadTDS_ExtFace) extFace = theExtFace;
    Handle(McCadTDS_HSequenceOfExtFace) extResFaceSeq = new McCadTDS_HSequenceOfExtFace();
    /////////////////////////////////////////////////////////////////////////////////////////////////
    // It is preferable to use adapted pseudo resultants called by McCadCSGTool::Resultants(extFace->GetFace()).
    // But they work if their support is on outer side. If the support starts in the inner side and end
    // with a contact with the outer side they fail to be sign changing.....
    // Therefore we try first if we can get valid adapted pseudo resultants if this fails we compute
    // the Boxed resultants.
    ///////////////////////////////////////////////////////////////////////////////////////////////

    Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt;
    thePntSeq->Append(mySamplePnt);
    Standard_Boolean successful = Standard_False;
    Standard_Boolean isTorus = Standard_False;
    Handle(TopTools_HSequenceOfShape) theResultants = new TopTools_HSequenceOfShape;

    // trigger for torus to Boxed computation: for the torus boxed resultatns are used always.
    TopoDS_Face aF = extFace->GetFace();
    BRepAdaptor_Surface aBS(aF,Standard_True);
    GeomAdaptor_Surface aAS = aBS.Surface();
    if (aAS.GetType() == GeomAbs_Torus)
            isTorus = Standard_True;

    // add face sample points to make life easy
    theResultants = McCadCSGTool::BoxResultants(aF);
    for(TopExp_Explorer ex(mySolid,TopAbs_VERTEX); ex.More(); ex.Next())
            thePntSeq->Append(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));

    for (Standard_Integer j=1; j <=theResultants->Length(); j++) {
        if(theResultants->Value(j).IsNull()) {
            msgr->Message("_#_McCadCSGTool_SignCheck.cxx :: NULL Resultant skipped!!!\n",
                          McCadMessenger_WarningMsg);
            continue;
        }

        TopoDS_Face theFace = TopoDS::Face(theResultants->Value(j));

        Handle(McCadTDS_ExtFace) extFace = new McCadTDS_ExtFace(theFace);
        BRepAdaptor_Surface BS(theFace,Standard_True);
        GeomAdaptor_Surface theAdaptFaceSurface = BS.Surface();

        for (Standard_Integer i=1; i <= thePntSeq->Length(); i++) {
            Standard_Real aVal = McCadGTOOL::Evaluate(theAdaptFaceSurface, thePntSeq->Value(i));

            if(aVal >= 0.1 )
                extFace->AppendPosPnt(thePntSeq->Value(i));
            else if (aVal < -0.1 )
                extFace->AppendNegPnt(thePntSeq->Value(i));
        }

        Standard_Integer NbPosPnt = extFace->NbPosPnt();
        Standard_Integer NbNegPnt = extFace->NbNegPnt();

        if (NbPosPnt > 0 && NbNegPnt > 0 ) {
            extFace->SetHaveExt(Standard_True);
            extResFaceSeq->Append(extFace);
            successful = Standard_True;
        }
        else {
            extFace->SetHaveExt(Standard_False);
        }
    }
    if (successful)
        return extResFaceSeq;
    else // not sign changing ...
        extResFaceSeq->Clear();

    // non-toroidal surfaces
    if (!isTorus) {
        theResultants = McCadCSGTool::Resultants(extFace->GetFace());
        for (Standard_Integer j=1; j <=theResultants->Length(); j++) {
            TopoDS_Face theFace = TopoDS::Face(theResultants->Value(j));
            Handle(McCadTDS_ExtFace) extFace = new McCadTDS_ExtFace(theFace);
            BRepAdaptor_Surface BS(theFace,Standard_True);
            GeomAdaptor_Surface theAdaptFaceSurface = BS.Surface();

            for (Standard_Integer i=1; i <= thePntSeq->Length(); i++) {
                Standard_Real aVal = McCadGTOOL::Evaluate(theAdaptFaceSurface, thePntSeq->Value(i));

                if(aVal > 0.1 )
                    extFace->AppendPosPnt(thePntSeq->Value(i));
                else if (aVal < -0.1 )
                    extFace->AppendNegPnt(thePntSeq->Value(i));
            }

            Standard_Integer NbPosPnt = extFace->NbPosPnt();
            Standard_Integer NbNegPnt = extFace->NbNegPnt();

            if (NbPosPnt > 0 && NbNegPnt > 0 ) {
                extFace->SetHaveExt(Standard_True);
                extResFaceSeq->Append(extFace);
                successful = Standard_True;
            }
            else
                extFace->SetHaveExt(Standard_False);
        }
    }

    if (successful) {
        cout << " Pseudo Resultants used. Number of resultants  =   "<< extResFaceSeq->Length() << endl;
        return extResFaceSeq;
    }

    return extResFaceSeq;
}

	////////////////////////////////////////////////////////////////////
	// if the abouve fail we use more sample points.
/*	thePntSeq->Clear();
	thePntSeq->Append(McCadCSGTool::AllSamplePoints(mySolid));
	for(TopExp_Explorer ex(mySolid,TopAbs_VERTEX); ex.More(); ex.Next())
	{
		thePntSeq->Append(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));
	}
	////////////////////////////////////////////////////////////////////
	theResultants = McCadCSGTool::AppBoxResultants(extFace->GetFace());
	cout << "Extended Box  Resultants ............... being calculated ..." << endl;
	successful = Standard_False;

	for (Standard_Integer j=1; j <=theResultants->Length(); j++)
	{
		TopoDS_Face theFace = TopoDS::Face(theResultants->Value(j));

		Handle(McCadTDS_ExtFace) extFace = new McCadTDS_ExtFace(theFace);

		BRepAdaptor_Surface BS(theFace,Standard_True);

		GeomAdaptor_Surface theAdaptFaceSurface = BS.Surface();

		//TopAbs_Orientation orient = theFace.Orientation();

		//  GeomAbs_SurfaceType surfType =  theAdaptFaceSurface.GetType();

		cout << "............................................................" << endl;

		for (Standard_Integer i=1; i <= thePntSeq->Length(); i++)
		{

			Standard_Real aVal = McCadGTOOL::Evaluate(theAdaptFaceSurface, thePntSeq->Value(i));

			//	  cout  << "Resultant:   aVal = " << aVal << endl;

			if(aVal > 1.5 )
				extFace->AppendPosPnt(thePntSeq->Value(i));
			else if (aVal < -1.5 )
				extFace->AppendNegPnt(thePntSeq->Value(i));
		}

		Standard_Integer NbPosPnt = extFace->NbPosPnt();
		Standard_Integer NbNegPnt = extFace->NbNegPnt();

		if (NbPosPnt > 0 && NbNegPnt > 0 )
		extFace->SetHaveExt(Standard_True);
		else
		extFace->SetHaveExt(Standard_False);

		if(extFace->HaveExt())
		{

			if (NbPosPnt == 0)
			{
				cout << "Warning:: Can not compute Sample points around face !! " << endl;

			}
			else if (NbNegPnt == 0)
			{
				cout << "Warning:: Can not compute Sample points around face !! " << endl;
			}
			successful = Standard_True;
			extResFaceSeq->Append(extFace);
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (successful)
	{
		cout << " Boxed Resultants with samples used.  Number of resultants  =   "<< extResFaceSeq->Length() << endl;
		return extResFaceSeq;
	}
	else
	{
		cout << "Warning:: Boxed Resultants with samples can not be computed!!! "<< extResFaceSeq->Length() << endl;
		cout << "Warning:: Resultant computation failed due to failing sample points !!! " << endl;
		extResFaceSeq->Clear();
		return extResFaceSeq;
        }*/


Handle(McCadTDS_ExtFace) McCadCSGTool_SignCheck::Rank(Handle(McCadTDS_HSequenceOfExtFace)& aExtFaceSeq)
{
        McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
	Handle(McCadTDS_HSequenceOfExtFace) theExtFaceSeq = aExtFaceSeq;
	//1. the ranking is performed through the number of faces supported.
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// face area based ranking base on feature assumption.
	// big faces determine the feature of the shape
	Handle(TColStd_HSequenceOfReal) areaSeq = new TColStd_HSequenceOfReal();
	Standard_Real area;

	for (Standard_Integer i=1; i <= theExtFaceSeq->Length(); i++)
	{
		TopoDS_Face exFace = (theExtFaceSeq->Value(i))->GetFace();
		GProp_GProps GP;
		BRepGProp::SurfaceProperties(exFace,GP);
		area= GP.Mass();
		if(area <= 1.0e-7)
		{
			// small face heuristics in the hope that they do not affect the computation!!
			theExtFaceSeq->Remove(i);
                        TCollection_AsciiString msg("_#_McCadCSGTool_SignCheck.cxx :: Small area face removed; Area : ");

                        msg += area;
                        msgr->Message(msg.ToCString());
		}
		else
			areaSeq->Append(area);
//		cout << i << ". Area of Face =  " << area <<endl;
	}

	//sort here
	if(areaSeq->Length() >1)
	{
		for (Standard_Integer j = 2; j<= areaSeq->Length(); j++)
		{
			Standard_Integer k;
			Standard_Real tmpArea = areaSeq->Value(j);
			for (k = j-1; k>=1; k--)
			{
				if (tmpArea > areaSeq->Value(k))
				{
					areaSeq->SetValue(k+1,areaSeq->Value(k));
					theExtFaceSeq->Exchange(k+1,k);
				}
				else
					break;
			}
			areaSeq->SetValue(k+1,tmpArea);
		}
	}

	if (theExtFaceSeq->Length() > 0)
		return theExtFaceSeq->Value(1);
	else
		return 0;
}


void McCadCSGTool_SignCheck::AdjustFaceUV()
{
	//The underlying surface of an extFace is here always planar.
	//The extended Face will be used for linear halfspace construction. Therefore  umin,umax,vmin, and vmax should be
	// adjusted. we set this values to 1e7, i.e., 100 meters.

	TopoDS_Face theFace = myExtFace->GetFace();
	Standard_Real prec = BRep_Tool::Tolerance(theFace);
	//cout << "Old Tolerance of Face =    " << prec <<endl;
	TopLoc_Location loc;
	TopAbs_Orientation orient = theFace.Orientation();
	Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(theFace,loc);
	// BRepAdaptor_Surface BS(theFace,Standard_True);
	GeomAdaptor_Surface theAdaptFaceSurface(theFaceSurface);
	if (theAdaptFaceSurface.GetType() != GeomAbs_Plane)
	{
		//we must have a plane surface here!!
		cout << "_#_McCadCSGTool_SignCheck.cxx :: Face adaptation for halfspace construction failed!!"<< endl;
		return;
	}

	gp_Pln aPln = theAdaptFaceSurface.Plane();

	///////////////////////////////////////////////////////////////////////////////////////////////////
	Standard_Real A, B, C, D;
	aPln.Coefficients(A, B, C, D);
	//cout << " A =  " << A << " B =  " << B << " C =  " << C <<" D =  " << D	<< endl;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	Handle(Geom_Plane) aPlane = new Geom_Plane(aPln);
	Standard_Real umin=-1.e5, umax=1.e5, vmin=-1.e5, vmax=1.e5;
#ifdef OCC650
	TopoDS_Face adjFace = BRepBuilderAPI_MakeFace(aPlane, umin, umax, vmin,	vmax);
#else
	TopoDS_Face adjFace = BRepBuilderAPI_MakeFace(aPlane, umin, umax, vmin,	vmax, 1.e-7);
#endif
	//Standard_Real prec1 = BRep_Tool::Tolerance(adjFace);

	const Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*)&adjFace.TShape());
	// if (prec < prec1) TF->Tolerance(prec);
	TF->Tolerance(prec);
	//cout << "New Tolerance of Face =    " << BRep_Tool::Tolerance(adjFace)	<< endl;
	adjFace.Orientation(orient);
	myExtFace->SetFace(adjFace);

	//cout << "Face adaptation for halfspace construction successful.\n"<< endl;
}


Standard_Boolean McCadCSGTool_SignCheck::SignStateChanges() const
{
	return mySignIsConstant;
}

