#include <McCadCSGTool.hxx>
#include <McCadGTOOL.hxx>

#include <StdFail_NotDone.hxx>

#include <gp.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cone.hxx>
#include <gp_Torus.hxx>

#include <TColStd_MapOfInteger.hxx>
#include <TColgp_HSequenceOfPnt.hxx>

#include <Geom_BSplineCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <gp_Cone.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <GeomInt_IntSS.hxx>
#include <Geom_HSequenceOfSurface.hxx>

#include <TopoDS.hxx>
#include <TopAbs.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <BRepTools_Modifier.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>

#include <BRepAdaptor_Curve.hxx>

#include <BRepCheck.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>

#include <BRepTools_WireExplorer.hxx>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <BRepLProp_SLProps.hxx>

#include <BRepBndLib.hxx>
#include <BndLib_AddSurface.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_SeqOfBox.hxx>
#include <BRepMesh.hxx>
#include <BRepMesh_FastDiscret.hxx>

#include <BRepClass_FaceClassifier.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Connect.hxx>
#include <ElSLib.hxx>
#include <CSLib.hxx>
#include <CSLib_DerivativeStatus.hxx>
#include <TCollection_AsciiString.hxx>

#include <McCadMessenger_Singleton.hxx>

#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadMathTool.hxx"

#include <omp.h>

#include <STEPControl_Writer.hxx>


//MYTEST
/*#include <McCadMcRead_SurfaceFactory.hxx>
#include <McCadCSGAdapt_SolidAnalyser.hxx>

#include <McCadCSGGeom_Surface.hxx>
#include <McCadCSGGeom_Cone.hxx>

#include <Geom_ConicalSurface.hxx>
#include <McCadCSGGeom_SurfaceType.hxx>*/
//#include <STEPControl_Writer.hxx>
//MYTEST


#define MAX2(X, Y)	(  Abs(X) > Abs(Y)? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)	( MAX2 ( MAX2(X,Y) , Z) )

#define MIN2(X, Y)	(  Abs(X) < Abs(Y)? Abs(X) : Abs(Y) )
#define MIN3(X, Y, Z)	( MIN2 ( MIN2(X,Y) , Z) )

gp_Dir Normal(const TopoDS_Face& F, const gp_Pnt& P) {
	Standard_Real U, V;
	gp_Pnt pt;
	BRepAdaptor_Surface AS(F, Standard_True);

	switch (AS.GetType())
	{
		case GeomAbs_Plane:
			ElSLib::Parameters(AS.Plane(), P, U, V);
			break;

		case GeomAbs_Cylinder:
			ElSLib::Parameters(AS.Cylinder(), P, U, V);
			break;

		case GeomAbs_Cone:
			ElSLib::Parameters(AS.Cone(), P, U, V);
			break;

		case GeomAbs_Torus:
			ElSLib::Parameters(AS.Torus(), P, U, V);
			break;

		default:
		{
			return gp_Dir(1., 0., 0.);
		}
	}

	gp_Vec D1U, D1V;
	AS.D1(U, V, pt, D1U, D1V);
	gp_Dir N;
	CSLib_DerivativeStatus St;
	CSLib::Normal(D1U, D1V, Precision::Confusion(), St, N);
	if (F.Orientation() == TopAbs_FORWARD)
		N.Reverse();
	return N;
}

Standard_Real AreaOfShape(const TopoDS_Shape& aShape) {
	GProp_GProps GP;
	BRepGProp::SurfaceProperties(aShape, GP);
	return GP.Mass();

}

Standard_Boolean IsTriangleValid(const gp_Pnt& P1, const gp_Pnt& P2,
		const gp_Pnt& P3) {
	gp_Vec V1(P1, P2);
	gp_Vec V2(P2, P3);
	gp_Vec V3(P3, P1);

	if ((V1.SquareMagnitude() > 1.e-10) && (V2.SquareMagnitude() > 1.e-10)
			&& (V3.SquareMagnitude() > 1.e-10)) {
		V1.Cross(V2); // V1 = Normal
		if (V1.SquareMagnitude() > 1.e-10)
			return Standard_True;
		else
			return Standard_False;
	} else
		return Standard_False;
}

Handle(TopTools_HSequenceOfShape) McCadCSGTool::Resultants(const TopoDS_Face& theFace)
{
	TopLoc_Location loc;
	TopAbs_Orientation orient = theFace.Orientation();

	Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(theFace,loc);

	GeomAdaptor_Surface adaptedFaceSurface(theFaceSurface);

	Handle(TopTools_HSequenceOfShape) seqResultant = new TopTools_HSequenceOfShape();

	//////////////////////////////////////////////////////////////////////////////////////////////
	// new adapted resultant computation: in local coordinates.
	//- Note that in the case of  the cone, resultants alone are not separating. we have to
	//  include also partial drivatives.
	//////////////////////////////////////////////////////////////////////////////////////////////

	Standard_Real UMin,UMax, VMin, VMax;
	BRepTools::UVBounds(theFace,UMin,UMax, VMin, VMax);

        // reject closed surfaces
        if( (adaptedFaceSurface.GetType() == GeomAbs_Cylinder || adaptedFaceSurface.GetType() == GeomAbs_Cone) &&
                Abs(Abs(UMax-UMin)-2.*M_PI) < 1e-7 )
		return seqResultant;
        if( adaptedFaceSurface.GetType() == GeomAbs_Sphere &&
            ( Abs(Abs(UMax-UMin)-2.*M_PI) < 1.e-7  ||
              Abs(Abs(VMax-VMin)-M_PI)    < 1.e-7  ) )
             return seqResultant;
        if( adaptedFaceSurface.GetType() == GeomAbs_Torus &&
            ( Abs(Abs(UMax-UMin)-2.*M_PI) < 1.e-7  ||
              Abs(Abs(VMax-VMin)-2.*M_PI) < 1.e-7  )  )
            return seqResultant;

	BRepAdaptor_Surface BSF(theFace,Standard_True);
	gp_Trsf T = BSF.Trsf();

	if ( fabs(UMax - UMin)<= gp::Resolution() || fabs(VMax - VMin)<= gp::Resolution() )
		return seqResultant;

        // get extremal points
	gp_Pnt P1 = (theFaceSurface->Value(UMin,VMin)).Transformed(T);
	gp_Pnt P2 = (theFaceSurface->Value(UMin,VMax)).Transformed(T);
	gp_Pnt P3 = (theFaceSurface->Value(UMax,VMin)).Transformed(T);
	gp_Pnt P4 = (theFaceSurface->Value(UMax,VMax)).Transformed(T);

	// we compute an orientation point which lies on the right side of the face using bbox.
	Bnd_Box theBB;
	BRepBndLib::Add(theFace,theBB);
	theBB.SetGap(5.0);
	Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
	theBB.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Plane1 = ( P1, P2, p3 ),
	//  v1 = p1 - p2, v2 = p1 - p4 v3 = v1 ^ v2
	gp_Vec v1(P1, P2), v2(P1, P3);
	gp_Vec v3 = v1 ^ v2;
	v3.Normalize();
	gp_Dir D1(v3);
	Handle(Geom_Plane) Pl1 = new Geom_Plane(P1,D1);

	gp_Vec v4(P3, P1), v5(P3, P4);
	gp_Vec v6 = v4 ^ v5;
	v6.Normalize();
	gp_Dir D2(v6);
	Handle(Geom_Plane) Pl2 = new Geom_Plane(P3,D2);

	ElSLib::Parameters(Pl1->Pln(),P1,UMin,VMin);
        ElSLib::Parameters(Pl2->Pln(),P4,UMax,VMax);

	if (v3.IsParallel(v6,1.0e-4))
	{
#ifdef OCC650
		TopoDS_Face F1 = BRepBuilderAPI_MakeFace(Pl1, UMin,UMax, VMin, VMax);
#else
		TopoDS_Face F1 = BRepBuilderAPI_MakeFace(Pl1, UMin,UMax, VMin, VMax, 1.e-7);
#endif
		F1.Orientation(orient); // this orieantation will not be used !!!
		seqResultant->Append(F1);
	}
	else
	{
#ifdef OCC650
		TopoDS_Face F1 = BRepBuilderAPI_MakeFace(Pl1, UMin,UMax, VMin, VMax);
#else
		TopoDS_Face F1 = BRepBuilderAPI_MakeFace(Pl1, UMin,UMax, VMin, VMax, 1.e-7);
#endif
		F1.Orientation(orient); // this orieantation will not  be used !!!
		seqResultant->Append(F1);

#ifdef OCC650
		F1 = BRepBuilderAPI_MakeFace(Pl2, UMin,UMax, VMin, VMax);
#else
		F1 = BRepBuilderAPI_MakeFace(Pl2, UMin,UMax, VMin, VMax, 1.e-7);
#endif
		F1.Orientation(orient); // this orieantation will not  be used !!!
		seqResultant->Append(F1);
	}
//	cout << "=================================================== " << endl;
//	cout << " Number of Resultants computed  =  " << seqResultant->Length() << "\n" << endl;

	return seqResultant;
}


Handle(TopTools_HSequenceOfShape) McCadCSGTool::BoxResultants(const TopoDS_Face& theFace)
{
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    TopLoc_Location loc;
    Handle_Geom_Surface theSurface = BRep_Tool::Surface(theFace,loc);
    GeomAdaptor_Surface adaptedFaceSurface(theSurface);
    Handle_TopTools_HSequenceOfShape resultantSeq = new TopTools_HSequenceOfShape();
    Standard_Real uMin,uMax, vMin, vMax;
    BRepTools::UVBounds(theFace,uMin,uMax, vMin, vMax);
    BRepAdaptor_Surface brepSurface(theFace,Standard_True);
    gp_Trsf surfaceTransformation = brepSurface.Trsf();

    // return planes for closed curved faces
    if((adaptedFaceSurface.GetType() == GeomAbs_Cylinder && Abs(Abs(uMax-uMin)-2*M_PI) < 1e-7  ) ||
       (adaptedFaceSurface.GetType() == GeomAbs_Torus    && ( Abs(Abs(vMax-vMin)-2*M_PI) < 1e-7  ||
        Abs(Abs(uMax-uMin)-2*M_PI) < 1e-7)                                                     ) ||
       (adaptedFaceSurface.GetType() == GeomAbs_Sphere    && ( Abs(Abs(vMax-vMin)-2*M_PI) < 1e-7 ||
        Abs(Abs(uMax-uMin)-2*M_PI) < 1e-7)                                                     ) ||
       (adaptedFaceSurface.GetType() == GeomAbs_Cone && Abs(Abs(uMax-uMin)-2*M_PI) < 1e-7))
    {
        TopExp_Explorer edgeIterator(theFace, TopAbs_EDGE);
        for(; edgeIterator.More(); edgeIterator.Next()) {
            TopoDS_Edge curEdge = TopoDS::Edge(edgeIterator.Current());

            Bnd_Box tstBox;
            BRepBndLib::Add(curEdge, tstBox);

            if(tstBox.IsThin(1e-7))
                continue;

            Standard_Real curveFirst, curveLast;
            const Handle_Geom_Curve& curve = BRep_Tool::Curve(curEdge,loc, curveFirst, curveLast);

            BRepAdaptor_Curve baCurve;
            baCurve.Initialize(curEdge);

            if(curve->IsClosed()) {
                TopoDS_Wire wire = BRepBuilderAPI_MakeWire(curEdge);
                TopoDS_Face planarFace = BRepBuilderAPI_MakeFace(wire ,Standard_True);
                resultantSeq->Append(planarFace);
            }
        }
        return resultantSeq;
    }

    // Calculate resultants from corner vertices of face
    //////////////////////////////////////////////////////

    // not closed but U and V parameters differ too little
    if ( fabs(uMax - uMin) <= gp::Resolution() || fabs(vMax - vMin) <= gp::Resolution() )
        return resultantSeq;

    // define corner points of face
    gp_Pnt p1 = (theSurface->Value(uMin,vMin)).Transformed(surfaceTransformation);
    gp_Pnt p2 = (theSurface->Value(uMin,vMax)).Transformed(surfaceTransformation);
    gp_Pnt p3 = (theSurface->Value(uMax,vMin)).Transformed(surfaceTransformation);
    gp_Pnt p4 = (theSurface->Value(uMax,vMax)).Transformed(surfaceTransformation);

    // calculate vertices for planes
    gp_Vec vec1(p1, p2), vec2(p1, p3);
    gp_Vec vec3 = vec1 ^ vec2;
    vec3.Normalize();
    gp_Vec vec4(p3, p1), vec5(p3, p4);
    gp_Vec vec6 = vec4 ^ vec5;
    vec6.Normalize();
    TopoDS_Vertex vert1 = BRepBuilderAPI_MakeVertex(p1);
    TopoDS_Vertex vert2 = BRepBuilderAPI_MakeVertex(p2);
    TopoDS_Vertex vert3 = BRepBuilderAPI_MakeVertex(p3);
    TopoDS_Vertex vert4 = BRepBuilderAPI_MakeVertex(p4);

    //in der Ebene
    if (vec3.IsParallel(vec6,1.0e-4)) {
        TopoDS_Wire wire1 = BRepBuilderAPI_MakePolygon(vert1,vert3,vert4,vert2,Standard_True); // ordered traversal important..
        TopoDS_Face planarFace1 = BRepBuilderAPI_MakeFace(wire1,Standard_True);
        if(planarFace1.IsNull())
            msgr->Message("_#_McCadCSGTool::BoxResultants : Null face computed - 0\n", McCadMessenger_WarningMsg);
        else
            resultantSeq->Append(planarFace1);
    }
    else {
        TopoDS_Wire wire1 = BRepBuilderAPI_MakePolygon(vert1,vert3,vert4,Standard_True);
        TopoDS_Face planarFace1 = BRepBuilderAPI_MakeFace(wire1,Standard_True);
        if(planarFace1.IsNull())
            msgr->Message("_#_McCadCSGTool::BoxResultants : Null face computed  - 1\n", McCadMessenger_WarningMsg);
        else
            resultantSeq->Append(planarFace1);
        TopoDS_Wire wire2 = BRepBuilderAPI_MakePolygon(vert1,vert4,vert2,Standard_True);
        TopoDS_Face planarFace2 = BRepBuilderAPI_MakeFace(wire2,Standard_True);
        if(planarFace2.IsNull())
            msgr->Message("_#_McCadCSGTool::BoxResultants : Null face computed  - 1\n", McCadMessenger_WarningMsg);
        else
            resultantSeq->Append(planarFace2);
    }

    //calculate planes for all non-degenerated, non-linear edges
    /////////////////////////////////////////////////////////////
    BRepAdaptor_Curve adaptorCurve;
    for (TopExp_Explorer exd(theFace,TopAbs_EDGE); exd.More(); exd.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(exd.Current());

        // skip degenerated edges
        if(BRep_Tool::Degenerated (edge))
                continue;

        // skip linear lines
        adaptorCurve.Initialize(edge);
        if (adaptorCurve.GetType() == GeomAbs_Line)
            continue;

        // determine first and last parameter for curve
        Standard_Real firstParameter = (!(Precision::IsNegativeInfinite(adaptorCurve.FirstParameter()))) ? adaptorCurve.FirstParameter() : -1.0;
        Standard_Real lastParameter = (!(Precision::IsPositiveInfinite(adaptorCurve.LastParameter()))) ? adaptorCurve.LastParameter() : 1.0;
        if(adaptorCurve.IsClosed())
            firstParameter = MAX2(lastParameter,firstParameter)/2;
        if (lastParameter-firstParameter < .5 ) // why?
            continue;

        // set correct orientation
        Standard_Real oFirst, oLast;
        if (edge.Orientation() == TopAbs_REVERSED) {
            oFirst = lastParameter;
            oLast  = firstParameter;
        }
        else {
            oFirst = firstParameter;
            oLast  = lastParameter;
        }

        gp_Pnt pf = adaptorCurve.Value(oFirst);
        gp_Pnt pl = adaptorCurve.Value(oLast);
        gp_Pnt pm = adaptorCurve.Value(oFirst + (oLast-oFirst)/2);

        gp_Vec ccv1(pf, pl);
        gp_Lin lin(pf,gp_Dir(ccv1));
        Standard_Real dismPnt = lin.Distance(pm);

        // shift middle vertex if it has small distance to the connecting line between first and last point
        if (dismPnt < 1.e-5 ) {
            gp_Dir normalOfFace = Normal(theFace,pf);
            gp_Vec normalVec(normalOfFace);
            /** Using vTan is the worst thing you can do
                gp_Vec vTan = ccv1 ^ vN;
                vTan.Normalize();
                vTan *= 100.;
                we have along linear edges also associated linear faces therefore we make this face big
                to use the non linear cut faces first....*/
            normalVec.Normalize();
            normalVec *= 100000.;
            pm.Translate(normalVec);
            vert1 = BRepBuilderAPI_MakeVertex(pf);
            vert2 = BRepBuilderAPI_MakeVertex(pm);
            vert3 = BRepBuilderAPI_MakeVertex(pl);
        }
        else {
            vert1 = BRepBuilderAPI_MakeVertex(pf);
            vert2 = BRepBuilderAPI_MakeVertex(pm);
            vert3 = BRepBuilderAPI_MakeVertex(pl);
        }
        TopoDS_Wire w1 = BRepBuilderAPI_MakePolygon(vert1,vert2,vert3,Standard_True);
        TopoDS_Face F1 = BRepBuilderAPI_MakeFace(w1,Standard_True);
        if(F1.IsNull())
            msgr->Message("_#_McCadCSGTool::BoxResultants : Null face computed - 2\n", McCadMessenger_WarningMsg);
        else
            resultantSeq->Append(F1);
    }

    return resultantSeq;
}



/*Handle(TopTools_HSequenceOfShape) McCadCSGTool::AppBoxResultants(const TopoDS_Face& theFace)
{
	TopLoc_Location loc;
	Bnd_Box theBB;
	TopoDS_Shape supBox;
	TopExp_Explorer ex;

	Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(theFace,loc);
	Handle(TopTools_HSequenceOfShape) seqResultant = new TopTools_HSequenceOfShape();

	// BRepAdaptor_Surface surface with bounds
	cout << "Starting Bounding Box computation !!!!" << endl;

	BRepAdaptor_Surface BS(theFace,Standard_True);
	// Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(theFace,loc);
	GeomAdaptor_Surface theAFaceSurface = BS.Surface();
	if (theAFaceSurface.GetType() == GeomAbs_Plane)
	{
                //cout << "A plane Face " << endl;
		return seqResultant;
	}
	// BndLib_AddSurface::Add(theAFaceSurface,Precision::Confusion(),theBB);
	BRepBndLib::Add(theFace,theBB);
	theBB.SetGap(0.0);
	Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
	theBB.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

        // what should be done in this cases is not yet clear!!!
        // enlarge Box added on Thu Sep 29 14:42:03 CES 2005
	if(theBB.IsVoid())
	{
		cout << "Void  Box computed!!  " << endl;
		return seqResultant;
	}
	if(theBB.IsWhole())
	{
		cout << "Whole Box computed!! " << endl;
		return seqResultant;
	}
	if(theBB.IsThin(BRep_Tool::Tolerance(theFace)))
	{
		cout << "Thin  Box computed!! " << endl;
		return seqResultant;
	}
	if(theBB.IsXThin(BRep_Tool::Tolerance(theFace)))
	{
		cout << "X Thin  Box computed!! " << endl;
		aXmin = aXmin - 0.5;
		aXmax = aXmax + 0.5;
		// return seqResultant;
	}
	if(theBB.IsYThin(BRep_Tool::Tolerance(theFace)))
	{
		cout << "Y Thin  Box computed!! " << endl;
		aYmin = aYmin - 0.5;
		aYmax = aYmax + 0.5;
		// return seqResultant;
	}
	if(theBB.IsZThin(BRep_Tool::Tolerance(theFace)))
	{
		cout << "Z Thin  Box computed!! " << endl;
		cout << "Will increase box size!!" << endl;
		aZmin = aZmin - 0.5;
		aZmax = aZmax + 0.5;
		//
		//return seqResultant;
	}

	supBox = BRepPrimAPI_MakeBox(gp_Pnt(aXmin, aYmin, aZmin), gp_Pnt(aXmax, aYmax, aZmax)).Shape();

	TopoDS_Solid solBox = TopoDS::Solid(supBox);

//	cout << "Bounding Box computation successful !!!!" << endl;

	for (ex.Init(solBox,TopAbs_FACE); ex.More(); ex.Next())
	{
		TopoDS_Face aBFace = TopoDS::Face(ex.Current());
		Handle(Geom_Surface) aBSurface = BRep_Tool::Surface(aBFace,loc);
		// if (aBSurface.GetType() == GeomAbs_Plane)  return seqResultant;
		Standard_Real A=0, B=0, C=0, D=0;
		gp_Pln aPlane = GeomAdaptor_Surface(aBSurface).Plane();

		aPlane.Coefficients(A, B, C, D);

		Standard_Real epsi;
		epsi =.001*D;
		if (fabs(A) > 1e-5)
			epsi = fabs(aXmax - aXmin)/1000.;
		if (fabs(B) > 1e-5)
			epsi = fabs(aYmax - aYmin)/1000.;
		if (fabs(C) > 1e-5)
			epsi = fabs(aZmax - aZmin)/1000.;
		// epsi is set 1/5 of the box size.
		// peturbation: since the intersection of the undelying surfaces is
		// transversal, the resultants are peturbation invariant.

		Handle(Geom_Plane) aPosPetSurface = new Geom_Plane( A, B, C, D+epsi);
		Handle(Geom_Plane) aNegPetSurface = new Geom_Plane( A, B, C, D-epsi);

		GeomInt_IntSS theIntPos(aPosPetSurface,theFaceSurface,Precision::Confusion(), Standard_True, Standard_False,Standard_False);

		if(!theIntPos.IsDone())
		{
			cout << "_#_McCadCSGTool.cxx :: Positive Int failed! " << endl;
			return seqResultant;
		}

		GeomInt_IntSS theIntNeg(aNegPetSurface,theFaceSurface,Precision::Confusion(),Standard_True, Standard_False,Standard_False);

		if(!theIntNeg.IsDone())
		{
			cout << "_#_McCadCSGTool.cxx :: Negative Int failed! " << endl;
			return seqResultant;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		cout << "Number of Lines: " <<theIntPos.NbLines() << "   " << theIntNeg.NbLines() << endl;
		Standard_Real sA,sB,sC,sD;
		aPlane.Coefficients(sA,sB,sC,sD);
//		cout << " resultants    A = " << sA << "  B = " << sB << "  C = " << sC << "  " << " D=  " << sD << endl;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (theIntPos.NbLines() && theIntNeg.NbLines())
			seqResultant->Append(aBFace);
	}
//	cout << "Resultant computation Successful !! " << endl;
	cout << "Number of Resultant = "<< seqResultant->Length() << endl;

	return seqResultant;
}*/


//
Handle(TopTools_HSequenceOfShape) McCadCSGTool::Partials(const TopoDS_Face& aFace)
{
	TopoDS_Face theFace = aFace;
	TopLoc_Location loc;	
	Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(theFace,loc);
	GeomAdaptor_Surface adaptedFaceSurface(theFaceSurface);
	Handle(TopTools_HSequenceOfShape) seqResultant = new TopTools_HSequenceOfShape();

	//////////////////////////////////////////////////////////////////////////////////////////////
	// new adapted resultant computation: in local coordinates.
	//- Note that in the case of the cone, resultants alone are not separating. we have to
	//  include also partial derivatives.
	//////////////////////////////////////////////////////////////////////////////////////////////
	Standard_Real UMin,UMax, VMin, VMax;
	BRepTools::UVBounds(theFace,UMin,UMax, VMin, VMax);
	BRepAdaptor_Surface BSF(theFace,Standard_True);
    gp_Trsf T = BSF.Trsf(); //the surfaces coordinate system

	if(adaptedFaceSurface.GetType() == GeomAbs_Plane)
    {
        return seqResultant;
    }

    TopAbs_Orientation orient = theFace.Orientation();
    if (orient == TopAbs_FORWARD)
    {
       return seqResultant;
    }

    if ( fabs(UMax - UMin)<= gp::Resolution() || fabs(VMax - VMin)<= gp::Resolution() )
        return seqResultant;

        // trac#23
        gp_Pnt P1 = (theFaceSurface->Value(UMin,VMin)).Transformed(T); //get extremal points in the surfaces coord sys.
        gp_Pnt P2 = (theFaceSurface->Value(UMin,VMax)).Transformed(T);
        gp_Pnt P3 = (theFaceSurface->Value(UMax,VMin)).Transformed(T);
        gp_Pnt P4 = (theFaceSurface->Value(UMax,VMax)).Transformed(T);

        //cout << "___U/V-Min/-Max: " << UMin << " " << UMax << " " << VMin << " " << VMax << endl;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // we compute an orientation point which lies on the right side of the face using bbox.
        Bnd_Box theBB;
        BRepBndLib::Add(theFace,theBB);
        theBB.SetGap(100.0);
        Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
        theBB.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
        gp_Pnt rescuePnt(Xmin+(Xmin+Xmax)/2., Ymin+(Ymin+Ymax)/2., Zmin+(Zmin+Zmax)/2.);

        //////////////////////////////
        // handle closed faces
        if((adaptedFaceSurface.GetType() == GeomAbs_Cylinder || adaptedFaceSurface.GetType() == GeomAbs_Cone )
                &&	Abs((UMax-UMin)-2*M_PI) < 1e-7 ) // closed cylinder doesn't need resultant
        {
                return seqResultant;
        }

      /*  if(adaptedFaceSurface.GetType() == GeomAbs_Cone && Abs((UMax-UMin)-2*M_PI) < 1.0e-7 )
        {

            gp_Pln aPln(1, 1, 1, 10);
            TopoDS_Face F1 = BRepBuilderAPI_MakeFace(aPln, 1,2, 1, 2).Face();

            //F1.Orientation(InOrient);
            seqResultant->Append(F1);

            return seqResultant;
        }*/

        if(adaptedFaceSurface.GetType() == GeomAbs_Sphere) // u-closed && v-closed sphere
        {
                if(Abs(Abs(UMax - UMin)-2*M_PI) < 1.e-5  /*&&  Abs(Abs(VMax - VMin)-PI) < 1.0e-5*/)
                {
                        return seqResultant;
                }
        }
        //MYTEST Torus special : Tue Jan 19 13:57:40 CET 2010
/*	if(adaptedFaceSurface.GetType() == GeomAbs_Torus)
        {
                if(orient == TopAbs_REVERSED) // reversed torus
                {
                        // check if u-closed
                        if(Abs(Abs(UMax - UMin) - 2*PI) > 1.e-3)
                        {// not u-closed -> need for splitting plane
                                cout << "NOT U-CLOSED\n";
                                if(Abs(Abs(UMax-UMin)) - PI > 1e-3)
                                        cout << "CCCCCCCCCCCCCCCCC cut neccessary for torus CCCCCCCCCCCCCCCCCCCCCCc\n";
                                // create face
                                // not important right now !!!
                                //TODO : implement missing planar faces
                        }


                        // check if v-closed
                        if(Abs(Abs(VMax - VMin) - 2*PI) > 1.e-3)
                        {// not v-closed -> need for splitting plane
                        //	cout << "NOT V-CLOSED\n";
                                TCollection_AsciiString sDesc("0 Z ");
                                Standard_Real radius = sqrt(P1.X()*P1.X() + P1.Y()*P1.Y());
                                sDesc += P1.Z();
                                sDesc += TCollection_AsciiString(" ");
                                sDesc += radius;
                                sDesc += TCollection_AsciiString(" ");
                                radius = sqrt(P4.X()*P4.X() + P4.Y()*P4.Y());
                                sDesc += P4.Z();
                                sDesc += TCollection_AsciiString(" ");
                                sDesc += radius;

                                cout << sDesc.ToCString() << endl;

                                McCadMcRead_SurfaceFactory sfFact;
                                sfFact.MakeSurface(sDesc);

                                if(!sfFact.IsDone())
                                {
                                        cout << "_#_McCadCSGTool.cxx :: Failed to make Axissymmetric Surface By Points!\n";
                                        return seqResultant;
                                }

                                TopoDS_Face aFace;

                                Handle(McCadCSGGeom_Surface) theSurf = sfFact.GetSurface();
                                if(theSurf->GetType() == McCadCSGGeom_Conical)
                                {
                                        Handle(McCadCSGGeom_Cone) mcCone = Handle(McCadCSGGeom_Cone)::DownCast(theSurf);
                                        Handle(Geom_ConicalSurface) coneSurf = Handle(Geom_ConicalSurface)::DownCast(mcCone->CasSurf());
                                        gp_Cone aCone = coneSurf->Cone();
                                        aFace = BRepBuilderAPI_MakeFace(aCone).Face();

                                        aFace.Orientation(TopAbs_REVERSED);
                                        seqResultant->Append(aFace);

                                }
                                else if(theSurf->GetType() == McCadCSGGeom_Cylindrical)
                                {
                                        //TODO implement cylindrical
                                }
                                else
                                {
                                        cout << "_#_McCadCSGTool.cxx :: Partial has uncommon surface type!!!\n";
                                }

                        }
                }
        }*/
        //#MYTEST Torus special



        if(adaptedFaceSurface.GetType() == GeomAbs_Torus || adaptedFaceSurface.GetType() == GeomAbs_SurfaceOfRevolution)
        {

                /*if(Abs(UMin) < 1e-7)
                        VMin = 0;*/

                if(Abs(Abs(VMax - VMin)-2.*M_PI) < 1e-5) // v-closed torus doesn't need resultants
                        return seqResultant;
                else if(Abs(Abs(UMax - UMin)-2*M_PI) < 0.001 || Abs(Abs(UMax - UMin)-M_PI) < 0.001) // reset Pnt3 & Pnt4
                {
                        P3 = (theFaceSurface->Value(UMax/3.0, VMin)).Transformed(T);
                        P4 = (theFaceSurface->Value(UMax/3.0, VMax)).Transformed(T);
                }

                // else construct special rescue point for toroidal surfaces
                Standard_Real uDiffHalf = (UMax-UMin)/2.0;

                gp_Pnt pnt1 = (theFaceSurface->Value(uDiffHalf, VMin));
                gp_Pnt pnt2 = (theFaceSurface->Value(uDiffHalf, (VMax-VMin)/2.0));

                if(orient == TopAbs_FORWARD)
                        rescuePnt = gp_Pnt((pnt1.X()+pnt2.X())/2.0, (pnt1.Y()+pnt2.Y())/2.0, (pnt1.Z()+pnt2.Z())/2.0);
                else
                        rescuePnt = gp_Pnt(1e9, 1e9, 1e9);
        }

        Handle(TColgp_HSequenceOfPnt) oPntSeq = new TColgp_HSequenceOfPnt();	//SamplePoints created by triangulation
        TopoDS_Shape supBox = BRepPrimAPI_MakeBox(gp_Pnt(Xmin, Ymin, Zmin), gp_Pnt(Xmax, Ymax, Zmax)).Shape();

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //computing sample point on the faces of the bounding box
        for (TopExp_Explorer exF(supBox,TopAbs_FACE); exF.More(); exF.Next())
        {
                TopoDS_Face aFace = TopoDS::Face(exF.Current());
                Handle(Poly_Triangulation) mesh;
                mesh = BRep_Tool::Triangulation(aFace, loc);
                if (mesh.IsNull())
                {
                        Standard_Real aDeflection = MAX3( Xmax-Xmin , Ymax-Ymin , Zmax-Zmin)/4.0;
                        BRepMesh::Mesh(aFace, aDeflection);
                        mesh = BRep_Tool::Triangulation(aFace, loc);
                }

                if (mesh.IsNull())
                {
                        cout << "_#_McCadCSGTool.cxx :: Face triangulation failed !!" << endl;
                        cout << "                       Use now incremental method !!" << endl;
                        Standard_Real dx = fabs(Xmax - Xmin)/2.;
                        Standard_Real dy = fabs(Ymax - Ymin)/2.;
                        Standard_Real dz = fabs(Zmax - Zmin)/2.;
                        oPntSeq->Append(gp_Pnt(Xmin, Ymin, Zmin));
                        oPntSeq->Append(gp_Pnt(Xmin, Ymin+dy, Zmin+dz));
                        oPntSeq->Append(gp_Pnt(Xmax, Ymin+dy, Zmin+dz));
                        oPntSeq->Append(gp_Pnt(Xmin+dx, Ymin, Zmin+dz));
                        oPntSeq->Append(gp_Pnt(Xmin+dx, Ymax, Zmin+dz));
                        oPntSeq->Append(gp_Pnt(Xmin+dx, Ymin+dy, Zmin));
                        oPntSeq->Append(gp_Pnt(Xmin+dx, Ymin+dy, Zmax));
                        oPntSeq->Append(gp_Pnt(Xmax, Ymax, Zmax));
                }
                else
                {
                        Standard_Integer nNodes = mesh->NbNodes();
                        TColgp_Array1OfPnt meshPnts(1,nNodes);
                        meshPnts = mesh->Nodes();
                        for(Standard_Integer i=meshPnts.Lower(); i<=meshPnts.Upper(); i++)
                        {
                                gp_Pnt aPnt = (meshPnts(i)).Transformed(T);
                                oPntSeq->Append(aPnt);
                        }
                }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////

        Standard_Integer inIndex(0);
        Standard_Real inVal(0.0);

        if(orient == TopAbs_FORWARD)
        {//For all potential Sample Points find the point with the biggest value OUT of the Surface
                for (Standard_Integer il=1; il<= oPntSeq->Length(); il++)
                {
                        Standard_Real orientVal = McCadGTOOL::Evaluate(adaptedFaceSurface,oPntSeq->Value(il));

                        if(orientVal < -.1 )
                        {
                                if (Abs(orientVal) > inVal) // we maximize
                                {
                                        inIndex = il;
                                        inVal = Abs(orientVal);
                                }
                        }
                }
        }
        else if(orient == TopAbs_REVERSED)
        {
                for (Standard_Integer il=1; il<= oPntSeq->Length(); il++) //For all potential Sample Points find the point with the biggest value OUT of the Surface
                {
                        Standard_Real orientVal = McCadGTOOL::Evaluate(adaptedFaceSurface,oPntSeq->Value(il));

                        if(orientVal > 0.1 )
                        {
                                if (Abs(orientVal) > inVal) // we maximize
                                {
                                        inIndex = il;
                                        inVal = Abs(orientVal);
                                        //cout << orientVal << endl;
                                }
                        }
                }
        }


        gp_Pnt InOrientPnt;//, OutOrientPnt;
        if(inIndex == 0)
        {
                cout << "Important Warning: Inward/Outward orientation point assumed ( " << rescuePnt.X() << " , " << rescuePnt.Y() << " , " << rescuePnt.Z() << " ) "<< endl; //(0,0,0) !!" << endl;
                InOrientPnt = rescuePnt;
        }
        else
        {
                InOrientPnt = oPntSeq->Value(inIndex);
        }
//cout<< "3\n";

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // compute new partials
        gp_Vec v1(P1, P2), v2(P1, P3);
        gp_Vec v3 = v1 ^ v2;
        v3.Normalize();
        gp_Dir D1(v3);
        Handle(Geom_Plane) Pl1 = new Geom_Plane(P1,D1);

        gp_Vec v4(P3, P1), v5(P3, P4);
        gp_Vec v6 = v4 ^ v5;
        v6.Normalize();
        gp_Dir D2(v6);
        Handle(Geom_Plane) Pl2 = new Geom_Plane(P3,D2);
//cout << "4\n";
        ////////////////////////////////////////////////////
        Handle(Geom_HSequenceOfSurface) surfSeq = new Geom_HSequenceOfSurface;

        if (v3.IsParallel(v6,1.0e-4)) // trac#23 always parallel the way the extremal points are defined
        {
            surfSeq->Append(Pl1);
        }
        else
	{
            surfSeq->Append(Pl1);
            surfSeq->Append(Pl2);
            std::cout << "not parallel\n";
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	for (Standard_Integer si = 1; si <= surfSeq->Length(); si++)
	{
		GeomAdaptor_Surface parSurface(surfSeq->Value(si));
		/////////////////////////////////////////////////////////////////////
		// we compute here the orientation of the parSurface relative to orientPnt
		// which certainly lies on the Face.
		TopAbs_Orientation InOrient;

		Standard_Real orientVal = McCadGTOOL::Evaluate(parSurface,InOrientPnt);

		if(orientVal < 0.0 )
		{
			InOrient = TopAbs_FORWARD;
		}
		else if (orientVal > 0.0 )
		{
			InOrient = TopAbs_REVERSED;
		}
		else
		{
			cout << "Important Warning: FORWARD orientation assumed for Partial!!" << endl;
			InOrient = TopAbs_FORWARD;
		}

		Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt();

		//only in this case we need additional points. In the other cases the partials are always supported.
		if (adaptedFaceSurface.GetType() == GeomAbs_Torus || adaptedFaceSurface.GetType() == GeomAbs_Cone )
		{
			Handle(Poly_Triangulation) mesh;
			mesh = BRep_Tool::Triangulation(theFace, loc);
			BRepAdaptor_Surface BS(theFace,Standard_True);
			gp_Trsf T = BS.Trsf();
			Standard_Real aDeflection=0.0;
			if (mesh.IsNull())
			{
				//cout << "_#_McCadCSGTool.cxx :: A Face has no triangulation !!" << endl;
				Bnd_Box B;
				BRepBndLib::AddClose(theFace, B);
				B.SetGap(0.0);
				Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
				B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
				//cout << aXmin << "  " << aYmin << "  " << aZmin <<"  " << aXmax<< "  " << aYmax<< "  " << aZmax << endl;
				//cout << "Box computed  !!" << endl;

				aDeflection = MAX3(aXmax-aXmin , aYmax-aYmin , aZmax-aZmin)/1.0;

				BRepMesh::Mesh(theFace, aDeflection);
			}

			mesh = BRep_Tool::Triangulation(theFace, loc);

			if (mesh.IsNull())
				cout << "_#_McCadCSGTool.cxx :: Face triangulation failed !!" << endl;
			else
			{
				Standard_Integer nNodes = mesh->NbNodes();

				Poly_Connect conn(mesh);
				TColgp_Array1OfPnt meshPnts(1,nNodes);
				meshPnts = mesh->Nodes();

			//	cout << " Number of total node Points =  " << nNodes <<endl;
				for(Standard_Integer i=meshPnts.Lower(); i<=meshPnts.Upper(); i++)
				{
					gp_Pnt aPnt = (meshPnts(i)).Transformed(T);
					Standard_Real aVal = McCadGTOOL::Evaluate(parSurface,aPnt);

					if (InOrient == TopAbs_FORWARD)
					{
						if(aVal < 0.0 ); // point is supported
						else thePntSeq->Append(aPnt);
					}
					else
					{
						if(aVal > 0.0 ); // point is supported
						else thePntSeq->Append(aPnt);
					}
				}
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////
		// the thePntSeq contains the unsupported points
		// we extremize and compute the max distance and move the partial by this distance away from the surface!
		gp_Pln parPlane = parSurface.Plane();
		gp_Ax3 axis = parPlane.Position();

		Standard_Boolean shouldPush = Standard_False;
		Standard_Real dist = 0 , maxDist = 0;

		if ( thePntSeq->Length() > 0 ) // otherwise all points are supported, we are done here;
		{
			shouldPush = Standard_True;
			gp_Pln aPln = parSurface.Plane();

			Standard_Integer index=0;
			for (int i=1; i<= thePntSeq->Length(); i++)
			{
				dist = aPln.Distance(thePntSeq->Value(i));
				//cout << dist << " ";
				if ( dist >= maxDist)
				{
					maxDist = dist;
					index = i;
				}
			}

			//cout << maxDist << " " << endl;
			axis.SetLocation(thePntSeq->Value(index));
		}
		gp_Pln aPlane(axis);

		if(shouldPush) // we have to correct by push away from node;
		{
			cout << "\nPUSHING!!!\n"	;
			Standard_Real A,B,C,D;
			aPlane.Coefficients(A,B,C,D);

			if (InOrient == TopAbs_FORWARD)
			{
				//  cout << "The orientation of the partial is forward." << endl;
				gp_Pln tmpPln(A,B,C,D - maxDist);
				aPlane = tmpPln;
			}
			else
			{
				//  cout << "The orientation of the partial is reversed." << endl;
				gp_Pln tmpPln(A,B,C,D + maxDist);
				aPlane = tmpPln;
			}
		}
		/*
		 else // push partial  away from the boundary by standard purturbation of 1.0;
		 {
		 Standard_Real A,B,C,D;
		 aPlane.Coefficients(A,B,C,D);
		 if (parOrient == TopAbs_FORWARD)
		 {
		 cout << "The orientation of the partial is forward." << endl;
		 gp_Pln tmpPln(A,B,C,D - 10.0);
		 aPlane = tmpPln;
		 }
		 else
		 {
		 cout << "The orientation of the partial is reversed." << endl;
		 gp_Pln tmpPln(A,B,C,D + 10.0);
		 aPlane = tmpPln;
		 }
		 }
		 */

		 Standard_Real A,B,C,D;
		 aPlane.Coefficients(A,B,C,D);
		 //	cout << "ADDING PLANE :: " << A << " " << B << " " << C << " " << D << endl;

		//////////////////
		// this is new addition to have bounded partials
		if(si==1)
		{
			ElSLib::Parameters(Pl1->Pln(), P1, UMin,VMin); // !!!! HERE
			ElSLib::Parameters(Pl1->Pln(), P4, UMax,VMax);
		}
		else
		{
			ElSLib::Parameters(Pl2->Pln(), P1, UMin,VMin); // !!!! HERE
			ElSLib::Parameters(Pl2->Pln(), P4, UMax,VMax);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////
		TopoDS_Face F1 = BRepBuilderAPI_MakeFace(aPlane, UMin,UMax, VMin, VMax).Face();
	//	cout << "U/V-Min/-Max: " << UMin << " " << UMax << " " << VMin << " " << VMax << endl;

		F1.Orientation(InOrient); // this orientation will be used !!!
		seqResultant->Append(F1);
	}
	//cout << " Number of Partials computed  =  " << seqResultant->Length() << endl ;
	// cout << "=================================================== " << endl;
	return seqResultant;
}


Standard_Boolean McCadCSGTool::AddAuxSurf(const TopoDS_Face& aFace,
                                          Handle(TopTools_HSequenceOfShape) & seqResultant)
{
    TopLoc_Location loc;
    Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(aFace,loc);
    GeomAdaptor_Surface adaptedFaceSurface(theFaceSurface);

    if( adaptedFaceSurface.GetType() == GeomAbs_Cylinder
        || adaptedFaceSurface.GetType() == GeomAbs_Cone
        || adaptedFaceSurface.GetType() == GeomAbs_Sphere)
    {
        return GenAuxFaceOfCylinder(seqResultant,aFace);
    }

    else if( adaptedFaceSurface.GetType() == GeomAbs_Torus
        /*|| adaptedFaceSurface.GetType() == GeomAbs_SurfaceOfRevolution*/)
    {
        return GenAuxFaceOfTorus(seqResultant,aFace);
    }
    else
    {
        return Standard_False;
    }
}


Standard_Boolean McCadCSGTool::GenAuxFaceOfCylinder( Handle(TopTools_HSequenceOfShape) & seqResultant,
                                                     const TopoDS_Face& theFace)
{
    TopAbs_Orientation orient = theFace.Orientation();
    if (orient == TopAbs_FORWARD)
    {
       return Standard_False;
    }

    TopLoc_Location loc;
    Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(theFace,loc);
    GeomAdaptor_Surface adaptedFaceSurface(theFaceSurface);

    Standard_Real UMin,UMax, VMin, VMax;
    BRepTools::UVBounds(theFace,UMin,UMax, VMin, VMax);
    BRepAdaptor_Surface BSF(theFace,Standard_True);
    gp_Trsf T = BSF.Trsf(); //the surfaces coordinate system

    if ( fabs(UMax - UMin)<= gp::Resolution() || fabs(VMax - VMin)<= gp::Resolution())
    {
//qiu        return seqResultant;
		  return seqResultant==NULL? Standard_False: Standard_True;
    }

    // trac#23
    gp_Pnt P1 = (theFaceSurface->Value(UMin,VMin)).Transformed(T); //get extremal points in the surfaces coord sys.
    gp_Pnt P2 = (theFaceSurface->Value(UMin,VMax)).Transformed(T);
    gp_Pnt P3 = (theFaceSurface->Value(UMax,VMin)).Transformed(T);
    gp_Pnt P4 = (theFaceSurface->Value(UMax,VMax)).Transformed(T);

    gp_Pnt pnt_mid = (theFaceSurface->Value(UMin+(UMax-UMin)/2.0,VMin+(VMax-VMin)/2.0)).Transformed(T);

    gp_Vec v1, v2, v3;

    // handle closed faces
    if( adaptedFaceSurface.GetType() == GeomAbs_Cylinder
        || adaptedFaceSurface.GetType() == GeomAbs_Cone ) // closed cylinder doesn't need resultant
    {
        if (Abs((UMax-UMin)-2*M_PI) < 1e-7)
        {
//qiu        return seqResultant;
		  return seqResultant==NULL? Standard_False: Standard_True;
        }

        v1 = gp_Vec::gp_Vec(P2,P1);
        v2 = gp_Vec::gp_Vec(P3,P1);
        v3 = v2 ^ v1;
    }
    else if(adaptedFaceSurface.GetType() == GeomAbs_Sphere) // u-closed && v-closed sphere
    {
        if(Abs(Abs(UMax - UMin)-2*M_PI) < 1.e-5)
        {
//qiu        return seqResultant;
		  return seqResultant==NULL? Standard_False: Standard_True;
        }

        P3 = (theFaceSurface->Value(UMin,VMin+((VMax - VMin)/2.0)) ).Transformed(T);
        P4 = (theFaceSurface->Value(UMax,VMin+((VMax - VMin)/2.0)) ).Transformed(T);

        v1 = gp_Vec::gp_Vec(P2,P1);
        v2 = gp_Vec::gp_Vec(P3,P4);
        v3 = v2 ^ v1;
    }

   // gp_Vec v1(P1, P2), v2(P1, P3);
   // gp_Vec v3 = v2 ^ v1;

    v3.Normalize();
    gp_Dir plane_dir(v3);

    //gp_Pnt pnt(P1.X()+v3.X()*0.0001,P1.Y()+v3.Y()*0.0001,P1.Z()+v3.Z()*0.0001);
    Handle(Geom_Plane) plane = new Geom_Plane(P1,plane_dir);
    TopoDS_Face face = BRepBuilderAPI_MakeFace(plane->Pln());

    Standard_Real orientVal = McCadGTOOL::Evaluate(plane->Pln(),pnt_mid);
    TopAbs_Orientation ori_plane;

    if(orientVal < 0.0 )
    {
        ori_plane = TopAbs_FORWARD;
    }
    else if (orientVal > 0.0 )
    {
        ori_plane = TopAbs_REVERSED;
    }

    face.Orientation(ori_plane); // this orientation will be used !!!
    seqResultant->Append(face);
}



Standard_Boolean McCadCSGTool::GenAuxFaceOfTorus(Handle(TopTools_HSequenceOfShape) & seqResultant,
                                                 const TopoDS_Face& theFace)
{
    TopLoc_Location loc;
    Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(theFace,loc);
    GeomAdaptor_Surface adaptedFaceSurface(theFaceSurface);

    Standard_Real UMin,UMax, VMin, VMax;
    BRepTools::UVBounds(theFace,UMin,UMax, VMin, VMax);
    BRepAdaptor_Surface BSF(theFace,Standard_True);
    gp_Trsf T = BSF.Trsf(); //the surfaces coordinate system

    Standard_Real orientVal;
    TopoDS_Face face; 

    McCadMathTool::ZeroValue(UMin,1.e-10);
    McCadMathTool::ZeroValue(UMax,1.e-10);
    McCadMathTool::ZeroValue(VMin,1.e-10);
    McCadMathTool::ZeroValue(VMin,1.e-10);

    TopAbs_Orientation orient = theFace.Orientation();    
    if (orient == TopAbs_FORWARD)
    {
        if ( fabs(UMax - UMin)<= gp::Resolution())// || fabs(VMax - VMin)<= gp::Resolution() )
            return Standard_False;

        gp_Pnt P1 = (theFaceSurface->Value(UMin,VMin)).Transformed(T); //get extremal points in the surfaces coord sys.
        gp_Pnt P2 = (theFaceSurface->Value(UMin,VMax)).Transformed(T);
        gp_Pnt P3 = (theFaceSurface->Value(UMax,VMin)).Transformed(T);
        gp_Pnt P4 = (theFaceSurface->Value(UMax,VMax)).Transformed(T);

        // handle closed faces
        if(Abs((UMax-UMin)-2*M_PI) < 1e-7 ) // closed cylinder doesn't need resultant
        {
            return Standard_False;
        }

        gp_Pnt PA = (theFaceSurface->Value(UMin, M_PI)).Transformed(T);
        gp_Pnt PB = (theFaceSurface->Value(UMax, M_PI)).Transformed(T);

        gp_Torus torus = adaptedFaceSurface.Torus();    // Get the geometry of torus
        gp_Ax3 Axis = torus.Position();                 // The coordinator of torus
        gp_Dir Dir = Axis.Direction();                  // The direction of torus axis

        gp_Vec vecA(Dir);
        gp_Vec vecB(PA, PB);

        gp_Vec vecPln =  vecA ^ vecB;
        vecPln.Normalize();
        gp_Dir dirPln(vecPln);

        //gp_Pnt PC(PA.X()+vecPln.X()*0.0001,PA.Y()+vecPln.Y()*0.0001,PA.Z()+vecPln.Z()*0.0001);
        Handle(Geom_Plane) plane = new Geom_Plane(PA,dirPln);

        face = BRepBuilderAPI_MakeFace(plane->Pln());

        gp_Pnt pnt_mid = (theFaceSurface->Value(UMin+(UMax-UMin)/2.0,VMin+(VMax-VMin)/2.0)).Transformed(T);
        orientVal = McCadGTOOL::Evaluate(plane->Pln(),pnt_mid);
    }
    else
    {
        gp_Pnt PA = (theFaceSurface->Value(0, VMin)).Transformed(T);
        gp_Pnt PB = (theFaceSurface->Value(0, VMax)).Transformed(T);

        gp_Vec vecA(PA,PB);

        gp_Torus torus = adaptedFaceSurface.Torus();    // Get the geometry of torus
        gp_Ax3 axis = torus.Position();                 // The coordinator of torus
        gp_Pnt center = axis.Location();                // The center of torus

        gp_Dir dir = axis.Direction();                  // The direction of torus axis
        gp_Vec vecB(dir); 

        Standard_Real radius = torus.MajorRadius();

        Standard_Real semiAngle = vecA.Angle(vecB);
        gp_Pnt pnt_mid = (theFaceSurface->Value(UMin+(UMax-UMin)/2.0,VMin+(VMax-VMin)/2.0)).Transformed(T);

        if (Abs(semiAngle - M_PI/2.0)<McCadConvertConfig::GetTolerence())
        {
            gp_Pln plane(center,dir);
            face = BRepBuilderAPI_MakeFace(plane);
            orientVal = McCadGTOOL::Evaluate(plane,pnt_mid);

        }
        else if (Abs(semiAngle - 0.0) < McCadConvertConfig::GetTolerence()
                 || Abs(semiAngle - M_PI) < McCadConvertConfig::GetTolerence())
        {            
            gp_Cylinder cyln(axis,radius);
            face = BRepBuilderAPI_MakeFace(cyln);
            orientVal = McCadGTOOL::Evaluate(cyln,pnt_mid);
        }
        else
        {            
            if (semiAngle > M_PI/2.0)
            {
                semiAngle = M_PI - semiAngle;

                vecA.SetX(-1.0*vecA.X());
                vecA.SetY(-1.0*vecA.Y());
                vecA.SetZ(-1.0*vecA.Z());
            }

            gp_Pnt pnt_cnt = (theFaceSurface->Value(0.0,0.0)).Transformed(T);
            gp_Vec vecC(center,pnt_cnt);

            Standard_Real angle = vecA.Angle(vecC);        

            if(angle > M_PI/2.0)
            {
                gp_Dir new_dir(-1.0*dir.X(), -1.0*dir.Y(), -1.0*dir.Z());
                axis.SetDirection(new_dir);
            }

            gp_Cone cone;     

            cone.SetSemiAngle(semiAngle);
            cone.SetLocation(center);
            cone.SetPosition(axis);
            cone.SetRadius(radius);

            face = BRepBuilderAPI_MakeFace(cone);
            orientVal = McCadGTOOL::Evaluate(cone,pnt_mid);
        }
    }

    TopAbs_Orientation ori,ori_new;
    ori = face.Orientation();

    if(orientVal < 0.0 )
    {
        ori_new = TopAbs_FORWARD;
    }
    else if (orientVal > 0.0 )
    {
        ori_new = TopAbs_REVERSED;
    }

    face.Orientation(ori_new); // this orientation will be used !!!
    seqResultant->Append(face);

    return Standard_True;

}

//    gp_Pnt P5;
//    gp_Pnt P6;

//    //if(Abs(Abs(UMax - UMin)-2*M_PI) < 0.001 || Abs(Abs(UMax - UMin)-M_PI) < 0.001) // reset Pnt3 & Pnt4
//    {
//       P5 = (theFaceSurface->Value(UMin, VMin+(VMax-VMin)/2.0)).Transformed(T);
//       P6 = (theFaceSurface->Value(UMax, VMin+(VMax-VMin)/2.0)).Transformed(T);
//    }

//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    // compute new partials
//    gp_Vec v1(P1, P2), v2(P1, P5);
//    gp_Vec v3 = v2 ^ v1  ;
//    v3.Normalize();
//    gp_Dir D1(v3);
//    Handle(Geom_Plane) Pl1 = new Geom_Plane(P1,D1);

//    gp_Vec v4(P3, P4), v5(P3, P6);
//    gp_Vec v6 = v4 ^ v5;
//    v6.Normalize();
//    gp_Dir D2(v6);
//    Handle(Geom_Plane) Pl2 = new Geom_Plane(P3,D2);

//    TopoDS_Face glF1 = BRepBuilderAPI_MakeFace(Pl1->Pln());
//    TopoDS_Face glF2 = BRepBuilderAPI_MakeFace(Pl2->Pln());

    //seqResultant->Append(glF1);
    //seqResultant->Append(glF2);








/*** COMPUTE SAMPLE POINTS ***/

Handle(TColgp_HSequenceOfPnt) McCadCSGTool::SamplePoints(const TopoDS_Shape& theShape) {
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt();
    Standard_Real MaxTol = 1.e-7;
    TopoDS_Solid tmpSol;

    try{ // return if no solid
        tmpSol = TopoDS::Solid(theShape);
    }
    catch(...){
        msgr->Message("McCadCSGTool::SamplePoints :: Error : input shape is not a solid\n", McCadMessenger_DefaultMsg);
        return thePntSeq;
    }

    BRepClass3d_SolidClassifier bsc3d (tmpSol); //Provides an algorithm to classify a point in a solid (IN/OUT/ON)
    ///////////////////////////////////////////////////////////////////////////////////////////
    Standard_Integer NbV;
    TopTools_IndexedDataMapOfShapeListOfShape aMVE;
    TopExp::MapShapesAndAncestors(tmpSol, TopAbs_VERTEX, TopAbs_EDGE, aMVE);//aMVE containes a map of all Vertices of tmpSol and appended
    NbV = aMVE.Extent();//number of Vertices								//to each a list of all edges that consist them
    TopTools_ListOfShape aLEdge;
    TopTools_MapOfShape aMEdge;
    Handle(TColgp_HSequenceOfPnt) tmpPntSeq = new TColgp_HSequenceOfPnt();

    for (int k=1; k<=NbV; k++) 	// For all Vertices calculate sample points (1/10th along an edge)
    {
        const TopoDS_Vertex& aV = TopoDS::Vertex(aMVE.FindKey(k)); //current vertex: aV
        gp_Pnt pv = BRep_Tool::Pnt(aV);//point coordinates of current vertex

        Standard_Real Tol = BRep_Tool::Tolerance(aV);
        aLEdge = aMVE.FindFromKey(aV);
        TopTools_ListIteratorOfListOfShape anIt(aLEdge);

        //For all edges that contain current vertex append non-degenerated edges to aMEdge
        for(; anIt.More(); anIt.Next()) {
            TopoDS_Shape aS = anIt.Value();
            aS.Orientation(TopAbs_FORWARD);
            if(!BRep_Tool::Degenerated (TopoDS::Edge(aS))) //check if current edge is degenerated
                aMEdge.Add(aS); //list of non-degenerated edges
        }

        //For all non-degenerated edges calculate potential sample points
        TopTools_MapIteratorOfMapOfShape it(aMEdge);
        BRepAdaptor_Curve c;
        for (; it.More(); it.Next()) {
            const TopoDS_Edge& E = TopoDS::Edge(it.Key());
            c.Initialize(E); // create path along the current edge
            Standard_Real f = (!(Precision::IsNegativeInfinite(c.FirstParameter()))) ? c.FirstParameter() : -1.0;
            Standard_Real l = (!(Precision::IsPositiveInfinite(c.LastParameter()))) ? c.LastParameter() : 1.0;
            gp_Pnt pf = c.Value(f);//starting point of edge
            Standard_Real D = (l - f)/10;
            if(D < MaxTol)
                D = MaxTol*10; //MaxTol=1e-7, i.e. D > 1e-7

            gp_Pnt pp;
            if (fabs(pf.Distance(pv)) <= Tol )
                pp = c.Value(f+D);
            else  //should never occure since the position of the vertex and the paths starting point should be the same within the tolerance
                pp = c.Value(l-D);
            tmpPntSeq->Append(pp);
        }

        // calulate barycenter
        gp_XYZ xyz(0,0,0);
        for (int j=1; j<=tmpPntSeq->Length(); j++)
            xyz += (tmpPntSeq->Value(j)).XYZ();
        xyz /= tmpPntSeq->Length();
        gp_Pnt bary(xyz);

        // prepend the vertex
        tmpPntSeq->Prepend(pv);

        //test if Bary is inside the Solid else perturb
        bsc3d.Perform(bary,MaxTol);
        if (bsc3d.State() == TopAbs_IN) {
            tmpPntSeq->Append(bary);
        }
        else {
            // perturbation coordinatewise
            Handle(TColgp_HSequenceOfPnt) aPntSeq = new TColgp_HSequenceOfPnt;
            for (int j=1; j<= tmpPntSeq->Length(); j++) {
                gp_Pnt aPnt = tmpPntSeq->Value(j);
                gp_Pnt aPref;

                if(j == 1)
                    aPref = bary; // we perturb the vertex relative to the bary center
                else
                    aPref = pv; 		// the other coord. are perturbed relative to the vertex.

                //X-pertubation
                Standard_Boolean done = Standard_False;
                for ( int kx=-1; kx <=1; kx++) {
                    if(done)
                        break;

                    gp_Pnt aPntx = aPnt;
                    Standard_Real dx = fabs(aPref.X() - aPntx.X())/2;

                    if (dx <= MaxTol*1000)
                        dx= MaxTol*1000;

                    Standard_Real rx = aPntx.Coord(1);
                    aPntx.SetCoord(1,rx+dx*kx);
                    bsc3d.Perform(aPntx,MaxTol*100);

                    if (bsc3d.State() == TopAbs_IN) {
                        aPntSeq->Append(aPntx);
                        done = Standard_True;
                        break;
                    }

                    //Y-pertubation
                    for ( int ky=-1; ky <=1; ky++) {
                        if(done)
                            break;

                        gp_Pnt aPnty = aPntx;
                        Standard_Real dy = fabs(aPref.Y() - aPnty.Y())/2;

                        if (dy <= MaxTol*1000)
                            dy= MaxTol*1000;

                        Standard_Real ry = aPnty.Coord(2);
                        aPnty.SetCoord(2,ry+dy*ky);
                        bsc3d.Perform(aPnty,MaxTol*100);
                        if (bsc3d.State() == TopAbs_IN) {
                            aPntSeq->Append(aPnty);
                            done = Standard_True;
                            break;
                        }

                        //Z-pertubation
                        for ( int kz=-1; kz <=1; kz++) {
                            gp_Pnt aPntz = aPnty;
                            Standard_Real dz = fabs(aPref.Z() - aPntz.Z())/2;

                            if (dz <= MaxTol*1000)
                                dz= MaxTol*1000;

                            Standard_Real rz = aPntz.Coord(3);
                            aPntz.SetCoord(3,rz+dz*kz);
                            bsc3d.Perform(aPntz,MaxTol*100);

                            if (bsc3d.State() == TopAbs_IN) {
                                aPntSeq->Append(aPntz);
                                done = Standard_True;
                                break;
                            }
                        }
                    }
                }
            }
            tmpPntSeq->Append(aPntSeq);
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        aMEdge.Clear();
        thePntSeq->Append(tmpPntSeq);
        tmpPntSeq->Clear();
    }
   /* TCollection_AsciiString message("number of sample points computed : ");
    message += thePntSeq->Length();
    msgr->Message(message.ToCString()); */
    return thePntSeq;
}


Handle(TColgp_HSequenceOfPnt) McCadCSGTool::AllSamplePoints(const TopoDS_Shape& theShape)
{
	Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt();

        thePntSeq->Append(SamplePoints(theShape));
    Standard_Integer MAXNUMPNT=5, MINNUMPNT=3;


        for (TopExp_Explorer ex(theShape,TopAbs_FACE); ex.More(); ex.Next())
	{
		TopoDS_Face aF = TopoDS::Face(ex.Current());
        thePntSeq->Append(FaceSamplePoints(aF,MAXNUMPNT, MINNUMPNT));

	}
	// filter redundant poits
	for (int j=2; j<= thePntSeq->Length(); j++)
	{
		gp_XYZ p1 = (thePntSeq->Value(j)).XYZ();
		for (int jk=j-1; jk >= 1; jk--)
		{
			gp_XYZ p2 = (thePntSeq->Value(jk)).XYZ();
			if(p1.IsEqual(p2,1.e-03)) {thePntSeq->Remove(jk); j=jk;}
		}
	}
	//////////////////////////////////////////////////////////////////////
    cout << "Number of All Sample Points:----------------------------------------------  " << thePntSeq->Length() << endl;
	return thePntSeq;
}


Handle(TColgp_HSequenceOfPnt) McCadCSGTool::FaceSamplePoints(const TopoDS_Face& theFace,const Standard_Integer MAXNUMPNT,const Standard_Integer MINNUMPNT)
{
	Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt();    

	TopExp_Explorer ex;
	BRepClass_FaceClassifier bsc3d;
	BRepAdaptor_Curve2d c;
	int vCount=0;
	if (MAXNUMPNT <= 0 || MINNUMPNT <= 0 )
	{
		cout << "Error Negative Number of Sample Points asked for:  " << thePntSeq->Length() << endl;
		return thePntSeq;
	}
	Standard_Real xmLen = 50, ymLen = 50; // this is the resolution 5 cm;
	Standard_Real amLen = 3.14/(2*MAXNUMPNT);
	BRepAdaptor_Surface BS(theFace,Standard_True);
	GeomAdaptor_Surface theASurface = BS.Surface();

	for (TopExp_Explorer exV(theFace,TopAbs_VERTEX); exV.More(); exV.Next()) //count vertices
		vCount++;	


    for (TopExp_Explorer ex(theFace,TopAbs_EDGE); ex.More(); ex.Next()) // For all edges, spread sample points along edges
	{
		c.Initialize(TopoDS::Edge(ex.Current()),theFace);
		Standard_Real f = (!(Precision::IsNegativeInfinite(c.FirstParameter()))) ? c.FirstParameter() : -1.0;
		Standard_Real l = (!(Precision::IsPositiveInfinite(c.LastParameter()))) ? c.LastParameter() : 1.0;
		gp_Pnt2d pf = c.Value(f);
		gp_Pnt2d pl = c.Value(l);
		gp_Pnt p3f;
		theASurface.D0(pf.X(),pf.Y(),p3f);
		gp_Pnt p3l;
		theASurface.D0(pl.X(),pl.Y(),p3l);
		Standard_Real Len = p3f.Distance(p3l);

		//  cout << "Len =  " << Len  << endl;
		Standard_Integer NUMPNT;

		if(int(Len/xmLen) < MINNUMPNT)
			NUMPNT = MINNUMPNT;
		else if(int(Len/xmLen) > MAXNUMPNT)
			NUMPNT = MAXNUMPNT;
		else
			NUMPNT = int(Len/xmLen);

		for(int t=0; t<=NUMPNT;t++)
		{
			Standard_Real a = Standard_Real(t)/Standard_Real(NUMPNT);
			Standard_Real par = (1-a)*f + a*l;
			gp_Pnt2d p2 = c.Value(par);
			Standard_Real U = p2.X();
			Standard_Real V = p2.Y();
			gp_Pnt p1;
			theASurface.D0(U,V,p1);
			bsc3d.Perform(theFace,p2,1.0e-07);
			if (bsc3d.State() == TopAbs_IN || bsc3d.State() == TopAbs_ON )
			{
				thePntSeq->Append(p1);
			}

		}
	}

	//////////////////////////////////////////////////////////
	Standard_Real U1, U2, V1, V2;
	BRepTools::UVBounds(theFace, U1, U2, V1, V2);

	if (theASurface.GetType() == GeomAbs_Cone ||theASurface.GetType() == GeomAbs_Cylinder)
	{
		xmLen = amLen;
	}
	if (theASurface.GetType() == GeomAbs_Sphere ||theASurface.GetType() == GeomAbs_Torus)
	{
		xmLen = amLen;
		ymLen = amLen;
	}

	Standard_Real du=1, dv=1;

	Standard_Real XLen=U2-U1;

	Standard_Real YLen=V2-V1;

	Standard_Integer XNUMPNT;
	if(int(XLen/xmLen) < MINNUMPNT)
		XNUMPNT = MINNUMPNT;
	else if(int(XLen/xmLen) > MAXNUMPNT)
		XNUMPNT = MAXNUMPNT;
	else
		XNUMPNT = int(XLen/xmLen);

	Standard_Integer YNUMPNT;

	if(int(YLen/ymLen) < MINNUMPNT)
		YNUMPNT = MINNUMPNT;
	else if(int(YLen/ymLen) > MAXNUMPNT)
		YNUMPNT = MAXNUMPNT;
	else
		YNUMPNT = int(YLen/ymLen);

	Handle(TColgp_HSequenceOfPnt) tmpPntSeq = new TColgp_HSequenceOfPnt();
	Standard_Integer repeat = 0;

	do{ //spread Sample Points all over the Face
		/*if(repeat > 0 && repeat <= 3)
			cout << "*** Face Sample loop repeated ** " << endl;*/
		if (repeat > 3 )
		{
			cout << "The model could be under-sampled !!! " << endl;
			cout << "Number of sample points is :    " << tmpPntSeq->Length() << endl;
			break;
		}
		du = XLen/Standard_Real(XNUMPNT);
		dv = YLen/Standard_Real(YNUMPNT);
		// cout << "The deltas are :  " << du  << "   " << dv << endl;
		// cout << "The No of deltas are :  " << XNUMPNT << "   " << YNUMPNT << endl;
		tmpPntSeq->Clear();

		for (int j=0; j<=YNUMPNT; j++)
		{
			Standard_Real V = V1+dv*j;

			for (int i=0; i<=XNUMPNT; i++)
			{
	//			cout << " J = " << j << "   I = " << i << endl;
				gp_Pnt p1;
				Standard_Real U = U1+du*i;
				theASurface.D0(U,V,p1);
				gp_Pnt2d p2(U,V);
				bsc3d.Perform(theFace,p2,1.0e-07);
				if (bsc3d.State() == TopAbs_IN || bsc3d.State() == TopAbs_ON )
				{
					tmpPntSeq->Append(p1);
				}
			}
		}

		XNUMPNT=XNUMPNT*2;
		YNUMPNT=YNUMPNT*2;
		repeat++;
	}while(tmpPntSeq->Length() < MINNUMPNT*MINNUMPNT);

	thePntSeq->Append(tmpPntSeq);
	/////////////////////////////////////////////////////////////////////////
	// filter redundant points
	for (int j=2; j<= thePntSeq->Length(); j++)
	{
		gp_XYZ p1 = (thePntSeq->Value(j)).XYZ();
		for (int jk=j-1; jk >= 1; jk--)
		{
			gp_XYZ p2 = (thePntSeq->Value(jk)).XYZ();
			if(p1.IsEqual(p2,1.e-03))
			{
				thePntSeq->Remove(jk);
			    j=jk;
			}
		}
	}
	//////////////////////////////////////////////////////////
//	cout << "Number of Face Sample Points:  " << thePntSeq->Length() << endl;
	return thePntSeq;

}


Handle(TopTools_HSequenceOfShape) McCadCSGTool::DeleteRedLinFace(Handle(TopTools_HSequenceOfShape)& aFaceSeq)
{
	if (aFaceSeq->Length() > 1 )
	{
		for(int i=2; i<= aFaceSeq->Length(); i++)
		{
			TopLoc_Location l;
			Handle(Geom_Surface) S;
			TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(i));
			S = BRep_Tool::Surface(F,l);
			GeomAdaptor_Surface AS(S);

                        if(AS.GetType() != GeomAbs_Plane)
                        {
                            aFaceSeq->Remove(i);
                            i--;
                            continue;
                        }

			gp_Pln aPln = AS.Plane();

			gp_Ax3 pos = aPln.Position();
			gp_Dir dir = pos.Direction();
			gp_Pnt loc = pos.Location ();
			Standard_Real sA,sB,sC,sD;
			aPln.Coefficients(sA,sB,sC,sD);
			Standard_Real D1 = dir.X()* loc.X() + dir.Y()* loc.Y() + dir.Z()*loc.Z();


			for (int j=i-1; j >= 1; j--)
			{
				TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(j));
				S = BRep_Tool::Surface(F,l);
				GeomAdaptor_Surface AS2(S);

                                if(AS2.GetType() != GeomAbs_Plane)
                                {
                                    aFaceSeq->Remove(j);
                                    continue;
                                }

				gp_Pln other = AS2.Plane();
				gp_Ax3 otherPos = other.Position();
				gp_Dir otherDir = otherPos.Direction();
				gp_Pnt otherLoc = otherPos.Location();

				Standard_Real D2 = otherDir.X()* otherLoc.X() + otherDir.Y()* otherLoc.Y() + otherDir.Z()* otherLoc.Z();

				if (dir.IsEqual(otherDir, 1.0e-03) && fabs(D1 - D2) < 1.0e-03)
				{
					aFaceSeq->Remove(j);
					i=j;
				}
			}
		}
		return aFaceSeq;
	}
        else
            return aFaceSeq;
}


Handle(TopTools_HSequenceOfShape) McCadCSGTool::DeleteRedCylFace(Handle(TopTools_HSequenceOfShape)& aFaceSeq)
{
	if (aFaceSeq->Length() > 1 )
	{
		for(int i=2; i<= aFaceSeq->Length(); i++)
		{
			TopLoc_Location l;
			Handle(Geom_Surface) S;
			TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(i));
			S = BRep_Tool::Surface(F);
			GeomAdaptor_Surface AS(S);
                        if(AS.GetType() != GeomAbs_Cylinder)
                        {
                            aFaceSeq->Remove(i);
                            i--;
                            continue;
                        }
			gp_Cylinder C1 = AS.Cylinder();
			gp_Ax3 ax1 = C1.Position();
			gp_Dir dir1 = ax1.Direction();
			gp_Pnt loc1 = ax1.Location();
			Standard_Real rad1 = C1.Radius();

			for (int j=i-1; j >= 1; j--)
			{
				TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(j));
				S = BRep_Tool::Surface(F);
				GeomAdaptor_Surface AS2(S);
                                if(AS2.GetType() != GeomAbs_Cylinder)
                                {
                                    aFaceSeq->Remove(j);
                                    continue;
                                }
                                gp_Cylinder C2 = AS2.Cylinder();
				gp_Ax3 ax2 = C2.Position();
				gp_Dir dir2 = ax2.Direction();
				gp_Pnt loc2 = (ax2.Location()).XYZ();
				Standard_Real rad2 = C2.Radius();
			//	Standard_Real Precision = 1.e-05;

				if (dir1.IsEqual(dir2, 1.e-03) &&
					loc1.IsEqual(loc2,1.e-03)  &&
					(fabs(rad1 - rad2 )<= 1.e-03) )
				{
					aFaceSeq->Remove(j);
					i=j;
				}
			}
		}
		return aFaceSeq;
	}
	else return aFaceSeq;

}

Handle(TopTools_HSequenceOfShape) McCadCSGTool::DeleteRedConFace(Handle(TopTools_HSequenceOfShape)& aFaceSeq)
{
	if (aFaceSeq->Length() > 1 )
	{
		for(int i=2; i<= aFaceSeq->Length(); i++)
		{
			TopLoc_Location l;
			Handle(Geom_Surface) S;
			TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(i));
			S = BRep_Tool::Surface(F);
			GeomAdaptor_Surface AS(S);
                        if(AS.GetType() != GeomAbs_Cone)
                        {
                            aFaceSeq->Remove(i);
                            i--;
                            continue;
                        }
			gp_Cone C1 = AS.Cone();
			gp_Ax3 ax1 = C1.Position();
			gp_Dir dir1 = ax1.Direction();
			gp_XYZ loc1 = (ax1.Location()).XYZ();
			Standard_Real rad1 = C1.RefRadius();
			Standard_Real ang1 = C1.SemiAngle();
			for (int j=i-1; j >= 1; j--)
			{
				TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(j));
				S = BRep_Tool::Surface(F);
				GeomAdaptor_Surface AS2(S);
                                if(AS2.GetType() != GeomAbs_Cone)
                                {
                                    aFaceSeq->Remove(j);
                                    continue;
                                }
				gp_Cone C2 = AS2.Cone();
				gp_Ax3 ax2 = C2.Position();
				gp_Dir dir2 = ax2.Direction();
				gp_XYZ loc2 = (ax2.Location()).XYZ();
				Standard_Real rad2 = C2.RefRadius();
				Standard_Real ang2 = C2.SemiAngle();
				Standard_Real Precision = 1.e-05;

				if((dir1.IsEqual(dir2,1.e-03)) && (loc1.IsEqual(loc2,1.e-03))
						&& (fabs(rad1 -rad2 )<= Precision) && (fabs(ang1 - ang2)<= Precision))
				{
					aFaceSeq->Remove(j);
					i=j;
				}
			}
		}
		return aFaceSeq;
	}
	else return aFaceSeq;
}

Handle(TopTools_HSequenceOfShape) McCadCSGTool::DeleteRedSphFace(Handle(TopTools_HSequenceOfShape)& aFaceSeq)
{
	if (aFaceSeq->Length() > 1 )
	{
		for(int i=2; i<= aFaceSeq->Length(); i++)
		{
			TopLoc_Location l;
			Handle(Geom_Surface) S;
			TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(i));
			S = BRep_Tool::Surface(F);
			GeomAdaptor_Surface AS(S);
                        if(AS.GetType() != GeomAbs_Sphere)
                        {
                            aFaceSeq->Remove(i);
                            i--;
                            continue;
                        }
			gp_Sphere C1 = AS.Sphere();
			gp_Ax3 ax1 = C1.Position();
			gp_Dir dir1 = ax1.Direction();
			gp_XYZ loc1 = (ax1.Location()).XYZ();
			Standard_Real rad1 = C1.Radius();

			for (int j=i-1; j >= 1; j--)
			{
				TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(j));
				S = BRep_Tool::Surface(F);
				GeomAdaptor_Surface AS2(S);
                                if(AS2.GetType() != GeomAbs_Sphere)
                                {
                                    aFaceSeq->Remove(j);
                                    continue;
                                }
				gp_Sphere C2 = AS2.Sphere();
				gp_Ax3 ax2 = C2.Position();
				gp_Dir dir2 = ax2.Direction();
				gp_XYZ loc2 = (ax2.Location()).XYZ();
				Standard_Real rad2 = C2.Radius();
				Standard_Real Precision = 1.e-05;

				if((dir1.IsEqual(dir2,1.e-03)) && (loc1.IsEqual(loc2,1.e-03)) && (fabs(rad1 -rad2 )<= Precision) )
				{
					aFaceSeq->Remove(j);
					i=j;
				}
			}
		}
		return aFaceSeq;
	}
	else return aFaceSeq;

}

Handle(TopTools_HSequenceOfShape) McCadCSGTool::DeleteRedTorFace(Handle(TopTools_HSequenceOfShape)& aFaceSeq)
{
	if (aFaceSeq->Length() > 1 )
	{
		for(int i=2; i<= aFaceSeq->Length(); i++)
		{
			TopLoc_Location l;
			Handle(Geom_Surface) S;
			TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(i));
			S = BRep_Tool::Surface(F);
			GeomAdaptor_Surface AS(S);
                        if(AS.GetType() != GeomAbs_Torus)
                        {
                            aFaceSeq->Remove(i);
                            i--;
                            continue;
                        }
			gp_Torus C1 = AS.Torus();
			gp_Ax3 ax1 = C1.Position();
			gp_Dir dir1 = ax1.Direction();
			gp_XYZ loc1 = (ax1.Location()).XYZ();
			Standard_Real arad1 = C1.MajorRadius();
			Standard_Real irad1 = C1.MinorRadius();

			for (int j=i-1; j >= 1; j--)
			{
				TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(j));
				S = BRep_Tool::Surface(F);
				GeomAdaptor_Surface AS2(S);
                                if(AS2.GetType() != GeomAbs_Torus)
                                {
                                    aFaceSeq->Remove(j);
                                    continue;
                                }
				gp_Torus C2 = AS2.Torus();
				gp_Ax3 ax2 = C2.Position();
				gp_Dir dir2 = ax2.Direction();
				gp_XYZ loc2 = (ax2.Location()).XYZ();
				Standard_Real arad2 = C2.MajorRadius();
				Standard_Real irad2 = C2.MinorRadius();
				Standard_Real Precision = 1.e-05;

				if((dir1.IsEqual(dir2,1.e-03)) && (loc1.IsEqual(loc2,1.e-03)) &&
						(fabs(arad1 -arad2 )<= Precision) && (fabs(irad1 -irad2 )<= Precision) )
				{
					aFaceSeq->Remove(j);
					i=j;
				}
			}
		}
		return aFaceSeq;
	}
	else return aFaceSeq;

}

Handle(TopTools_HSequenceOfShape) McCadCSGTool::DeleteRedSORFace(Handle(TopTools_HSequenceOfShape)& aFaceSeq)
{
	if (aFaceSeq->Length() > 1 )
	{
		for(int i=2; i<= aFaceSeq->Length(); i++)
		{
			TopLoc_Location l;
			Handle(Geom_Surface) S;
			TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(i));
			S = BRep_Tool::Surface(F);
			GeomAdaptor_Surface AS(S);
			gp_Torus C1 = AS.Torus();
			gp_Ax3 ax1 = C1.Position();
			gp_Dir dir1 = ax1.Direction();
			gp_XYZ loc1 = (ax1.Location()).XYZ();
			Standard_Real arad1 = C1.MajorRadius();
			Standard_Real irad1 = C1.MinorRadius();

			for (int j=i-1; j >= 1; j--)
			{
				TopoDS_Face F = TopoDS::Face(aFaceSeq->Value(j));
				S = BRep_Tool::Surface(F);
				GeomAdaptor_Surface AS2(S);
				gp_Torus C2 = AS2.Torus();
				gp_Ax3 ax2 = C2.Position();
				gp_Dir dir2 = ax2.Direction();
				gp_XYZ loc2 = (ax2.Location()).XYZ();
				Standard_Real arad2 = C2.MajorRadius();
				Standard_Real irad2 = C2.MinorRadius();
				Standard_Real Precision = 1.e-05;

				if((dir1.IsEqual(dir2,1.e-03)) && (loc1.IsEqual(loc2,1.e-03)) &&
						(fabs(arad1 -arad2 )<= Precision) && (fabs(irad1 -irad2 )<= Precision) )
				{
					aFaceSeq->Remove(j);
					i=j;
				}
			}
		}
		return aFaceSeq;
	}
	else
		return aFaceSeq;

}

/*
TopoDS_Solid McCadCSGTool::LinSolid(const TopoDS_Solid& aSolid)
{
	// this code has a major problem not specificaly known.
	// classfication of infinity fails, resulting solid not always valid!!!
	// don't use it at the moment.
	TopoDS_Solid theSolid = aSolid;
	cout << " Linearizing small faces of the solid! "<< endl;
	cout << " ===================================== "<< endl;
	int icorr=0;
	int fc = 0;
	for (TopExp_Explorer ex(theSolid, TopAbs_FACE); ex.More(); ex.Next()) {
		fc++;
	}
	cout << " Total number of Face = "<< fc << endl;
	fc=0;
	for (TopExp_Explorer ex(theSolid, TopAbs_FACE); ex.More(); ex.Next())
	{
		fc++;
		cout << " Processing FaceBox number :  "<< fc << endl;
		TopoDS_Face theFace = TopoDS::Face(ex.Current());

		BRepAdaptor_Surface BS(theFace, Standard_True);
		GeomAdaptor_Surface theASurface = BS.Surface();

		if (theASurface.GetType() == GeomAbs_Plane)
			continue;
		if (theASurface.GetType() != GeomAbs_Cone && theASurface.GetType()
				!= GeomAbs_Cylinder &&theASurface.GetType() != GeomAbs_Sphere
				&& theASurface.GetType() != GeomAbs_Torus) {
			cout << "Can not treat non analytic surface "<< endl;
			return theSolid;
		}
		Standard_Real area = AreaOfShape(theFace);
		cout << "area fo face = "<< area << endl;
		if (area >= 50000)
			continue;
		cout << " .......... Processing FaceBox number :  "<< fc << endl;

		////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////
		TopTools_IndexedDataMapOfShapeListOfShape allMEF;
		TopExp::MapShapesAndAncestors(theSolid, TopAbs_EDGE, TopAbs_FACE,
				allMEF);
		TopTools_ListOfShape allLFace;
		//Standard_Boolean onlyLin=Standard_True;
		////////////////////////////////////////////////////////////////////////////////////
		TopLoc_Location loc;
		Handle(Geom_Surface) aS = BRep_Tool::Surface(theFace,loc);
		//	  Handle(Geom_ElementarySurface) aEs =  Handle_Geom_ElementarySurface::DownCast(aS);

		//int ivc = 0;

		gp_Vec Nvec(0,0,0), Lvec(0,0,0);
		Standard_Real U1, U2, V1, V2;
		//Standard_Real Prec = gp::Resolution();
		BRepTools::UVBounds(theFace, U1, U2, V1, V2);
		if( U2 - U1 > .25*PI ) continue; // here we have a surface > 1/4 perimeter, we let it live.
		// cout << "U1 = " << U1 << "   U2 = " << U2 << "   V1 = " << V1 << "   V2 = " << V2 << endl;
		gp_Pnt p1,p2,p3,p4;
		theASurface.D0(U1,V1,p1);
		theASurface.D0(U1,V2,p2);
		theASurface.D0(U2,V1,p3);
		theASurface.D0(U2,V2,p4);
		///////////////////////////////////////////////////////
		gp_Pnt pu,po;
		theASurface.D0((U1+U2)/2,V1,pu);
		theASurface.D0((U1+U2)/2,V2,po);
		///////////////////////////////////////////////////////
		// compute extremal to derermine the shift.
		gp_Pnt pex;

		theASurface.D0((U1+U2)/2,(V1+V2)/2,pex);
		gp_Vec v1(p1, p2), v2(p1, p3),v4(p1, p4);
		gp_Vec v3 = v1 ^ v2;
		gp_Vec v5 = v1 ^ v4;
		v3.Normalize();v5.Normalize();
		gp_Dir D1(v3);
		gp_Dir D2(v5);
		gp_Pln aPln1(p1,D1),aPln2(p1,D2);

		Standard_Real exD1 = aPln1.Distance(pex);
		Standard_Real exD2 = aPln2.Distance(pex);
		Standard_Real exD;
		if (exD1 > exD2) exD = 2*exD1;
		else exD = 2*exD2;
		cout << "Extrem Distance = " << exD << endl;
		if (exD < 1.) exD = 1.0;
		/////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////
		Standard_Integer NbV;
		TopTools_IndexedDataMapOfShapeListOfShape aMVE;
		TopExp::MapShapesAndAncestors(theFace, TopAbs_VERTEX, TopAbs_EDGE, aMVE);
		NbV = aMVE.Extent();
		TopTools_ListOfShape aLEdge;
		TopTools_MapOfShape aMEdge;
		///////////////////////////////////////////////////////////////////////////////////
		Handle(TopTools_HSequenceOfShape) aVSeq = new TopTools_HSequenceOfShape;
		cout << " Nb of V = " << NbV << endl;
		for (int k=1; k<=NbV; k++)
		{
			TopoDS_Vertex aV=TopoDS::Vertex(aMVE.FindKey(k));
			if (aMEdge.Add(aV))
			{
				aVSeq->Append(aV);
				aLEdge = aMVE.FindFromKey(aV);
				TopTools_ListIteratorOfListOfShape anIt(aLEdge);
		//		Standard_Boolean reject = Standard_False;
				int itt=0;
				for(; anIt.More(); anIt.Next())
				{
					itt++;
					TopoDS_Shape aS=anIt.Value();
					aS.Orientation(TopAbs_FORWARD);
					for (TopExp_Explorer exd(aS,TopAbs_VERTEX); exd.More(); exd.Next())
					{
						if (!aMEdge.Add(exd.Current()));
						else
						{
							aVSeq->Append(exd.Current());
						}
					}
				}
			}
		}

		cout << "Number of a  Vertex is : " << aVSeq->Length() << endl;
		///////////////////////////////////////////////////////////////////////////////////
		// for (Standard_Integer ji=1; ji<= aVSeq->Length(); ji++)
		//  {
		//   PrintPnt(BRep_Tool::Pnt(TopoDS::Vertex(aVSeq->Value(ji))));
		//  }
		///////////////////////////////////////////////////////////////////////////////////

		Handle(TopTools_HSequenceOfShape) aWSeq = new TopTools_HSequenceOfShape;
		Handle(TopTools_HSequenceOfShape) aESeq = new TopTools_HSequenceOfShape;

		if(aVSeq->Length() < 3 || aVSeq->Length() > 4 ) continue;
		if(aVSeq->Length() == 3 || aVSeq->Length() == 4 )
		{
			TopoDS_Wire w1 = BRepBuilderAPI_MakePolygon(TopoDS::Vertex(aVSeq->Value(1)),
					TopoDS::Vertex(aVSeq->Value(2)),
					TopoDS::Vertex(aVSeq->Value(3)),Standard_True).Wire();
			if(w1.Checked()) cout << "w1 Checked ................." << endl;
			if(w1.Closed())cout << "w1 Closed ................." << endl;
			if(w1.Orientable()) cout << "w1 Orientable ................." << endl;
			w1.Orientation(TopAbs_FORWARD);
			aWSeq->Append(w1);
		}
		if( aVSeq->Length() == 4 )
		{
			TopoDS_Wire w2 = BRepBuilderAPI_MakePolygon(TopoDS::Vertex(aVSeq->Value(2)),
					TopoDS::Vertex(aVSeq->Value(3)),
					TopoDS::Vertex(aVSeq->Value(4)),Standard_True).Wire();
			if(w2.Checked()) cout << "w2 Checked ................." << endl;
			if(w2.Closed())cout << "w2 Closed ................." << endl;
			if(w2.Orientable()) cout << "w2 Orientable ................." << endl;
			w2.Orientation(TopAbs_FORWARD);
			aWSeq->Append(w2);
		}

		aVSeq->Clear();
		///////////////////////////////////////////////////////////////////////////////////
		BRepAdaptor_Curve c;
		Handle(TopTools_HSequenceOfShape) aFaceSeq = new TopTools_HSequenceOfShape;

		TopTools_MapOfShape aMapFace;
		TopTools_MapOfShape exMapFace;
		///////////////////////////////////////////////////////////////////////////////
		for (TopExp_Explorer exw(theSolid,TopAbs_FACE); exw.More(); exw.Next())
		aMapFace.Add(TopoDS::Face(exw.Current()));
		cout << "beging Number of Faces in the map : " << aMapFace.Extent() << endl;
		////////////////////////////////////////////////////////////////////////////////
		int ied = 0;
		Standard_Boolean replaceFailed = Standard_False;
		for (TopExp_Explorer exd(theFace,TopAbs_EDGE); exd.More(); exd.Next())
		{

			replaceFailed = Standard_False;
			aVSeq->Clear();
			TopoDS_Edge E = TopoDS::Edge(exd.Current());
			for (TopExp_Explorer exv(E,TopAbs_VERTEX); exv.More(); exv.Next()) aVSeq->Append(exv.Current());
			c.Initialize(E);
			if (c.GetType() == GeomAbs_Line) {cout << "GeomAbs_Line" << endl; continue;}
			if (c.GetType() == GeomAbs_Circle) cout << "GeomAbs_Circle" << endl;
			if (c.GetType() == GeomAbs_Ellipse) cout << "GeomAbs_Ellipse" << endl;
			if (c.GetType() == GeomAbs_Hyperbola) cout << "GeomAbs_Hyperbola" << endl;
			if (c.GetType() == GeomAbs_Parabola) cout << "GeomAbs_Parabola" << endl;
			if (c.GetType() == GeomAbs_BezierCurve) cout << "GeomAbs_Bezier" << endl;
			if (c.GetType() == GeomAbs_BSplineCurve) cout << "GeomAbs_BSpline" << endl;
			if (c.GetType() == GeomAbs_OtherCurve) cout << "GeomAbs_Other" << endl;

			Standard_Real f = (!(Precision::IsNegativeInfinite(c.FirstParameter()))) ? c.FirstParameter() : -1.0;
			Standard_Real l = (!(Precision::IsPositiveInfinite(c.LastParameter()))) ? c.LastParameter() : 1.0;

			gp_Pnt pf = c.Value(f);
			gp_Pnt pl = c.Value(l);
			gp_Pnt pm = c.Value((l-f)/2);
			gp_Vec ccv1(pf, pl), ccv2(pm, pl);
			if (ccv1.IsParallel(ccv2,1.e-5))
			{
				continue;
			}
			ied++;
			cout << ied << "  start ================================================================= " << ied << endl;
			//    TopoDS_Edge lE = BRepBuilderAPI_MakeEdge(pf,pl).Edge();
			///////////////////////////////////////////////////////
                        Standard_Integer NbeE;
			TopTools_IndexedDataMapOfShapeListOfShape aMEF;
			TopExp::MapShapesAndAncestors(theSolid,TopAbs_EDGE,TopAbs_FACE, aMEF);
			NbeE = aMEF.Extent();
			TopTools_ListOfShape aLFace;

			TopTools_MapOfShape exMapEdge;
			///////////////////////////////////////////////////////////////////////////////////
			//  cout << " Nb of F = " << NbeE << endl;
			for (int k=1; k<=NbeE; k++)
			{
				TopoDS_Edge aFE =TopoDS::Edge(aMEF.FindKey(k));
				TopoDS_Edge tmpE = aFE;
				tmpE.Orientation(E.Orientation());
				if(!exMapEdge.Add(aFE)) {continue;}

				// if(aFE.Orientation() != E.Orientation() && E == tmpE)
				if(E == tmpE)
				{

					cout << ied << "  Edge is Found!!!! " << endl;
					aLFace = aMEF.FindFromKey(aMEF.FindKey(k));
					TopTools_ListIteratorOfListOfShape anIt(aLFace);
					for(; anIt.More(); anIt.Next())
					{
						TopoDS_Face hF= TopoDS::Face(anIt.Value());
						if(hF!=theFace)
						{
							BRepAdaptor_Surface BAS(hF,Standard_True);
							GeomAdaptor_Surface aaS = BAS.Surface();
							if (aaS.GetType() == GeomAbs_Plane)
							{
								exMapFace.Add(hF);
								cout << "Linear Face is Found!!!! " << endl;
								TopLoc_Location lo;
								Handle (Geom_Surface) aSur = BRep_Tool::Surface(hF,lo);
								TopoDS_Wire wo;

								int iwc = 0;
								for (TopExp_Explorer exw(hF,TopAbs_WIRE); exw.More(); exw.Next())
								{
									iwc++;
									wo = TopoDS::Wire(exw.Current());
								}
								if(iwc > 1)
								{
									replaceFailed = Standard_True;
									cout << " >>>>>>>> ReplaceFailed  Number of wires is :  " << iwc << endl;
									break;
								}
								TopoDS_Wire wh;
								//  wh = TopoDS::Wire(wo.EmptyCopied());

								BRepBuilderAPI_MakeWire BWh(wh);

								/////////////////////////////////////////////////////////
								for (TopExp_Explorer exh(hF,TopAbs_EDGE); exh.More(); exh.Next())
								{
									TopoDS_Edge hE = TopoDS::Edge(exh.Current());
									hE.Orientation(E.Orientation());
									TopoDS_Vertex VF,VL;
									for (TopExp_Explorer exv(TopoDS::Edge(exh.Current()),TopAbs_VERTEX);
											exv.More(); exv.Next())
									{

										TopAbs_Orientation vto = exv.Current().Orientation();
										if (vto == TopAbs_FORWARD) {
											VF = TopoDS::Vertex(exv.Current());
										}
										else if (vto == TopAbs_REVERSED) {
											VL = TopoDS::Vertex(exv.Current());
										}
										else cout << " Unknown Vertex Orientation !! " << endl;
									}
									if (hE == E)
									{
										cout << "Local  Edge is Found!!!! " << endl;
										cout << " Making New Edge ............................ " << endl;
										gp_Pnt P1 = BRep_Tool::Pnt(VF);
										gp_Pnt P2 = BRep_Tool::Pnt(VL);
										Standard_Real l = P1.Distance(P2);
										if (l <= gp::Resolution())
										{
											replaceFailed = Standard_True;
											cout << " Making New Edge Failed: Same Points ..... " << endl;
											break;
										}
										else
										{
											TopoDS_Edge tE= BRepBuilderAPI_MakeEdge(VF,VL).Edge();
											BWh.Add(tE);
										}
										cout << " New Edge Added " << endl;
									}
									else
									{
										TopoDS_Edge ahE = TopoDS::Edge(exh.Current());
										BWh.Add(ahE);
									}
								}

								wh = BWh.Wire();
								wh.Orientation(wo.Orientation());
								//////////////////////////////////////////////////////////////////////////////////
								cout << "============  face begin    ===========" << endl;
								// TopoDS_Shape S = hF.EmptyCopied();
								// S.Orientation (TopAbs_FORWARD );
								BRep_Builder BF;
								// BF.Add(S,wh);
								TopoDS_Face anF = BRepBuilderAPI_MakeFace(aSur,wh).Face();
								//  TopoDS_Face anF = TopoDS::Face(S);
								anF.Orientation(hF.Orientation());
								BF.UpdateFace(anF,BRep_Tool::Tolerance(hF));
								cout << "============   face end   ===========" << endl;
								//////////////////////////////////////////////////////////////////////////////
								BRepCheck_Analyzer BA(wh,Standard_True);
								BA.Init(wh,Standard_False);
								Handle(BRepCheck_Result) RES = BA.Result(wh);
								BRepCheck_ListOfStatus StatusList;
								StatusList = RES->Status();
								BRepCheck_ListIteratorOfListOfStatus iter;
								cout << "============   Wire     ===========" << endl;
								for (iter.Initialize(StatusList); iter.More(); iter.Next())
								{
									BRepCheck::Print(iter.Value(),cout);
								}
								cout << "===================================" << endl;
								if(BA.IsValid())
								{
									cout << "Wire BRepCheck_Analyzer done !!" << endl;
								}
								else
								{
									replaceFailed = Standard_True;
									cout << "Wire BRepCheck_Analyzer failed !!" << endl;
									break;
								}
								//////////////////////////////////////////////////////////////////////////////////
								BA.Init(anF,Standard_False);
								RES = BA.Result(anF);
								StatusList = RES->Status();
								cout << "============   Face   ===========" << endl;
								for (iter.Initialize(StatusList); iter.More(); iter.Next())
								{
									BRepCheck::Print(iter.Value(),cout);
									// if(iter.Value() !=BRepCheck_NoError)
									// theShapeSequence->Append(wo);
								}
								cout << "===================================" << endl;
								if(BA.IsValid())
								{

									cout << "FACE BRepCheck_Analyzer done !!" << endl;
								}
								else
								{
									replaceFailed = Standard_True;
									cout << "FACE BRepCheck_Analyzer failed !!" << endl;
									break;
								}
								////////////////////////////////////////////////////////////////////////////////////
								aMapFace.Add(anF);
								// theShapeSequence->Append(wh);
								// if(ied==1 )
								// theShapeSequence->Append(anF);
								///////////////////////////////////////////////////////////////////
							}
							else
							{
								/// non planar case
								cout << "Non Linear Face is Found!!!! " << endl;
								/////////////////////////////////////////////////////////////////////
								TopoDS_Wire wo;
								int iwc = 0;
								for (TopExp_Explorer exw(theFace,TopAbs_WIRE); exw.More(); exw.Next())
								{
									iwc++;
									wo = TopoDS::Wire(exw.Current());
								}
								if(iwc > 1)
								{
									replaceFailed = Standard_True;
									cout << " >>>>>>>> ReplaceFailed  Number of wires is :  " << iwc << endl;
									break;
								}
								/////////////////////////////////////////////////////////////////////
								TopoDS_Wire wh;
								BRepBuilderAPI_MakeWire BWh(wh);

								/////////////////////////////////////////////////////////

								TopoDS_Vertex VF,VL;
								for (TopExp_Explorer exv(E,TopAbs_VERTEX);
										exv.More(); exv.Next())
								{

									TopAbs_Orientation vto = exv.Current().Orientation();
									if (vto == TopAbs_FORWARD) {
										VF = TopoDS::Vertex(exv.Current());
									}
									else if (vto == TopAbs_REVERSED) {
										VL = TopoDS::Vertex(exv.Current());
									}
									else cout << " Unknown Vertex Orientation !! " << endl;
								}
								cout << "Local  Edge is Found!!!! " << endl;
								cout << " Making New Edge ............................ " << endl;
								gp_Pnt P1 = BRep_Tool::Pnt(VF);
								gp_Pnt P2 = BRep_Tool::Pnt(VL);
								Standard_Real l = P1.Distance(P2);
								if (l <= gp::Resolution())
								{
									replaceFailed = Standard_True;
									cout << " Making New Edge Failed: Same Points ..... " << endl;
									break;
								}
								else
								{
									TopoDS_Edge tE= BRepBuilderAPI_MakeEdge(VL,VF).Edge();
									tE.Orientation(E.Orientation());
									BWh.Add(tE);
								}
								cout << " New Edge Added " << endl;
								BWh.Add(E);
								wh = BWh.Wire();
								//	  wh.Orientation(wo.Orientation());
								if (wh.Orientation() == TopAbs_FORWARD) {
									cout << " TopAbs_FORWARD Wire Orientation !! " << endl;
								}
								else if (wh.Orientation() == TopAbs_REVERSED) {
									cout << " TopAbs_REVERSED Wire Orientation !! " << endl;
								}
								else cout << " Unknown Wire Orientation !! " << endl;
								//////////////////////////////////////////////////////////////////////////////////
								cout << "============  face begin    ===========" << endl;
								BRep_Builder BF;
								TopoDS_Face anF = BRepBuilderAPI_MakeFace(wh).Face();
								if(anF.IsNull())
								{
									replaceFailed = Standard_True;
									cout << " Making New Edge Failed: Space Curve ..... " << endl;
									break;
								}

								// anF.Orientation(wo.Orientation());
								////////////////////////////////////////////////////////////////
								BRepAdaptor_Surface lBAS(anF,Standard_True);
								gp_Pln aPln = lBAS.Plane();
								gp_Ax3 pos = aPln.Position();
								const gp_Pnt& loc = pos.Location ();
								const gp_Dir& dir = pos.Direction();
								Standard_Real D = (dir.X() * (pex.X() - loc.X()) +
										dir.Y() * (pex.Y() - loc.Y()) +
										dir.Z() * (pex.Z() - loc.Z()));
								if (D < 0)
								anF.Orientation(TopAbs_FORWARD);
								else if (D>0)
								anF.Orientation(TopAbs_REVERSED);
								else
								{
									replaceFailed = Standard_True;
									cout << " Orientation Value : " << D << endl;
									cout << " Unknown Orientation !!" << endl;
									break;

								}
								/////////////////////////////////////////////////////////////////
								BF.UpdateFace(anF,BRep_Tool::Tolerance(hF));
								cout << "============   face end   ===========" << endl;
								//////////////////////////////////////////////////////////////////////////////
								BRepCheck_Analyzer BA(wh,Standard_True);
								BA.Init(wh,Standard_False);
								Handle(BRepCheck_Result) RES = BA.Result(wh);
								BRepCheck_ListOfStatus StatusList;
								StatusList = RES->Status();
								BRepCheck_ListIteratorOfListOfStatus iter;
								cout << "============   Wire     ===========" << endl;
								for (iter.Initialize(StatusList); iter.More(); iter.Next())
								{
									BRepCheck::Print(iter.Value(),cout);
								}
								cout << "===================================" << endl;
								if(BA.IsValid())
								{
									cout << "Wire BRepCheck_Analyzer done !!" << endl;
								}
								else
								{
									replaceFailed = Standard_True;
									cout << "Wire BRepCheck_Analyzer failed !!" << endl;
									break;
								}
								//////////////////////////////////////////////////////////////////////////////////
								BA.Init(anF,Standard_False);
								RES = BA.Result(anF);
								StatusList = RES->Status();
								cout << "============   Face   ===========" << endl;
								for (iter.Initialize(StatusList); iter.More(); iter.Next())
								{
									BRepCheck::Print(iter.Value(),cout);
								}
								cout << "===================================" << endl;
								if(BA.IsValid())
								{

									cout << "FACE BRepCheck_Analyzer done !!" << endl;
								}
								else
								{
									replaceFailed = Standard_True;
									cout << "FACE BRepCheck_Analyzer failed !!" << endl;
									break;
								}
								////////////////////////////////////////////////////////////////////////////////////
								aMapFace.Add(anF);
								aMapFace.Add(hF);
								//  theShapeSequence->Append(anF);
								// theShapeSequence->Append(hF);
								///////////////////////////////////////////////////////////////////
							}
						}

						if(replaceFailed) break;
					}
				}
				if(replaceFailed) break;
			}
			cout << ied << "  end ================================================================= " << ied << endl;
			if(replaceFailed) break;
		}
		if(replaceFailed)
		{
			cout << "Replacement of Faces : " << --fc << "  failed!!" << endl;
			continue;
		}

		cout << "before removing Number of Faces : " << aMapFace.Extent() << endl;
		aMapFace.Remove(theFace);
		//////////////////////////////////////////////////////////////////////
		TopTools_MapIteratorOfMapOfShape anItw(exMapFace);
		for(; anItw.More(); anItw.Next())
		{
			if(aMapFace.Remove(anItw.Key()))
			cout << "Removing a faces........ " << endl;
		}
		//////////////////////////////////////////////////////////////////////

		cout << "after removing Number of Faces : " << aMapFace.Extent() << endl;
		// theShapeSequence->Append( theFace);
		///////////////////////////////////////////////////////////////////////////////////
		cout << "Number of wires is : " << aWSeq->Length() << endl;
		////////////////////////////////////////////////////////////////////////////////////
                TopAbs_Orientation orientOftheFace = TopAbs_FORWARD;
		for (TopExp_Explorer exw(theFace,TopAbs_WIRE); exw.More(); exw.Next())
			orientOftheFace = exw.Current().Orientation();

		for(int j=1; j<= aWSeq->Length(); j++)
		{
			TopoDS_Wire aW = TopoDS::Wire(aWSeq->Value(j));
			TopoDS_Face aF = BRepBuilderAPI_MakeFace(aW,Standard_False).Face();
			if(aF.IsNull()) cout << "Null Face " << endl;
			BRep_Builder BF;
			BF.UpdateFace(aF, BRep_Tool::Tolerance(theFace));
			aF.Orientation(orientOftheFace);
			aMapFace.Add(aF);
		}
		/////////////////////////////////////////////////////////////////
		cout << " Total number of Face = " << aMapFace.Extent() << endl;
		//////////////////////////////////////////////////////////////////
		Standard_Real stol = 1.e-6;
		Redo:
		BRepBuilderAPI_Sewing Sew(stol);
		Sew.SetMinTolerance(1.e-3);
		Sew.SetLocalTolerancesMode(Standard_True);
		TopoDS_Shell shell;
		Sew.Load(shell);
		TopoDS_Solid solid;

		if (aMapFace.Extent() > 0 )
		{
			TopTools_MapIteratorOfMapOfShape anIt(aMapFace);
			for(; anIt.More(); anIt.Next())
			{
				TopoDS_Face tmpFace = TopoDS::Face(anIt.Key());
				Sew.Add(tmpFace);
				// cout << "Sew faces........ " << endl;
			}
		}
		else {cout << "Warning: No faces available!!! " << endl; continue;}
		Sew.Perform();
		if (Sew.NbFreeEdges() > 0 )
		{
			if (stol < 1e-3)
			{
				stol = stol*10;
				cout << "Sewing tolerance corrrection : " << stol << endl;
				goto Redo;
			}
			else
			{
				cout << "Face replace failed due to Sew error!!" << endl;
				continue;
			}
		}
		Sew.Dump();
		BRep_Builder B;
		B.MakeSolid(solid);
		Standard_Boolean failedShell = Standard_False;
		TopoDS_Shape sShape = Sew.SewedShape();
		//  cout << "............       " ;
		// TopAbs::Print(sShape.ShapeType(),cout);cout << "          ................." << endl;
		for(TopExp_Explorer exw(sShape,TopAbs_SHELL); exw.More(); exw.Next())
		{
			TopoDS_Shell sh = TopoDS::Shell(exw.Current());
			cout << "...................         Add Shell      ........ " << endl;
			BRepCheck_Analyzer BA(sh,Standard_True);
			BA.Init(sh,Standard_True);
			Handle(BRepCheck_Result) RES = BA.Result(sh);
			BRepCheck_ListOfStatus StatusList;
			StatusList = RES->Status();
			BRepCheck_ListIteratorOfListOfStatus iter;
			for (iter.Initialize(StatusList); iter.More(); iter.Next())
			{
				BRepCheck::Print(iter.Value(),cout);

			}
			if(BA.IsValid())
			{
				cout << "Shell BRepCheck_Analyzer done !!" << endl;
				B.Add(solid,sh);
			}
			else
			{
				cout << "Shell BRepCheck_Analyzer failed !!" << endl;
				failedShell = Standard_True;
			}

		}
		if(failedShell)
		{
			cout << "Shell building failed no repair is performed ............       ";
			continue;
		}
		if(solid.IsNull()) cout << " Null Solid failed  ............       ";
		else cout << " Non Null Solid  ............       ";
		cout << "............       ";
		TopAbs::Print(solid.ShapeType(),cout);cout << "          ................." << endl;
		///////////////////////////////////////////////////////////////////////////
		try
		{
			BRepClass3d_SolidClassifier bsc3d(solid);
			// Standard_Real t = Precision::Confusion();
			bsc3d.PerformInfinitePoint(stol);
			if (bsc3d.State() == TopAbs_IN)
			{
				cout << ">>>>>>>>>>>>>>>>>>>>>>> Infinity in Solid !! <<<<<<<<<<<<<<<<<<<<<< " << endl;
			}
			else
			{
				icorr++;
				theSolid = solid;
				cout << "Solid build up finished! " << endl;

			}
		}

		catch(Standard_Failure) {
			cout << "Warning: SolidFromShell: Exception: " << endl;
		}
	}
	cout << "  Total number of corrected faces is : "<< icorr << endl;
	cout << "Lin ends here ++++++++++++++++++++++++++++++++++++++++++++++++++"
			<< endl;
	cout << endl;
	return theSolid;
}
*/
