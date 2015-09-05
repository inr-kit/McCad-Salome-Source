/*
 * McCadCSGTool_FaceFuser.cxx
 *
 *  Created on: Dec 15, 2010
 *      Author: grosse
 */

//#include <STEPControl_Writer.hxx>

#include <McCadCSGTool_FaceFuser.hxx>
#include <McCadMessenger_Singleton.hxx>
#include <Geom_Curve.hxx>
#include <BRep_Tool.hxx>


McCadCSGTool_FaceFuser::McCadCSGTool_FaceFuser()
{
	Init();
}


McCadCSGTool_FaceFuser::McCadCSGTool_FaceFuser(Handle_TopTools_HSequenceOfShape& faceSeq,
                                               Standard_Boolean skipForwardOriented )
{
	Init();
	myUseReverseOrientedOnly = skipForwardOriented;
	AddShapes(faceSeq);
	Fuse();
}


McCadCSGTool_FaceFuser::McCadCSGTool_FaceFuser(TopoDS_Face& f1, TopoDS_Face& f2)
{
	Init();
	myOriginalFaces->Append(f1);
	myOriginalFaces->Append(f2);
	Fuse();
}


void McCadCSGTool_FaceFuser::Init()
{
	myCurrentSurfaceType = GeomAbs_OtherSurface;
	myIsDone = Standard_False;
        myFusionFailed = Standard_False;
	myUseReverseOrientedOnly = Standard_False;

	myFusedFaces = new TopTools_HSequenceOfShape;
	myOriginalFaces = new TopTools_HSequenceOfShape;

}


Standard_Boolean McCadCSGTool_FaceFuser::IsDone()
{
	return myIsDone;
}


Standard_Boolean McCadCSGTool_FaceFuser::FusionFailed()
{
        return myFusionFailed;
}


Handle_TopTools_HSequenceOfShape McCadCSGTool_FaceFuser::GetFused()
{
        if(!IsDone())
		Fuse();
        if(FusionFailed())
	{
                McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
                msgr->Message("_#_McCadCSGTool_FaceFuser :: Lateral fusing of faces has failed!!!\n",
                              McCadMessenger_WarningMsg);
	}

	return myFusedFaces;
}

Handle_TopTools_HSequenceOfShape McCadCSGTool_FaceFuser::GetOriginal()
{
	return myOriginalFaces;
}


void McCadCSGTool_FaceFuser::AddShapes(Handle_TopTools_HSequenceOfShape& theShapes)
{
	for(Standard_Integer i=1; i<=theShapes->Length(); i++)
	{
		TopoDS_Shape curShape = theShapes->Value(i);
		for(TopExp_Explorer ex(curShape, TopAbs_FACE); ex.More(); ex.Next())
		{
			TopoDS_Face curFace = TopoDS::Face(ex.Current());
			myOriginalFaces->Append(curFace);
		}
	}
}

void McCadCSGTool_FaceFuser::SkipForwardOriented(Standard_Boolean& state)
{
	myUseReverseOrientedOnly = state;
}



Standard_Boolean McCadCSGTool_FaceFuser::IsLateral(const TopoDS_Face& f1, const TopoDS_Face& f2)
{
    TopLoc_Location loc;
    const Handle(Geom_Surface)& surface1 = BRep_Tool::Surface(f1,loc);
    GeomAdaptor_Surface adaptedSurface1(surface1);
    const Handle(Geom_Surface)& surface2 = BRep_Tool::Surface(f2,loc);
    GeomAdaptor_Surface adaptedSurface2(surface2);

    // surfaces not the same surface type? skip
    if(adaptedSurface1.GetType() != adaptedSurface2.GetType())
            return Standard_False;

    TopAbs_Orientation orient1 = f1.Orientation();
    TopAbs_Orientation orient2 = f2.Orientation();

    // surfaces not the same orientation? skip
    if(orient1 != orient2)
            return Standard_False;

    Handle(McCadCSGGeom_Surface) mccadSurface1;
    Handle(McCadCSGGeom_Surface) mccadSurface2;

    // check which kind of surface type we have
    if(adaptedSurface1.GetType() == GeomAbs_Plane)
    {
            myCurrentSurfaceType = GeomAbs_Plane;
            mccadSurface1 = new McCadCSGGeom_Plane;
            mccadSurface2 = new McCadCSGGeom_Plane;
    }
    else if(adaptedSurface1.GetType() == GeomAbs_Cylinder)
    {
            myCurrentSurfaceType = GeomAbs_Cylinder;
            mccadSurface1 = new McCadCSGGeom_Cylinder;
            mccadSurface2 = new McCadCSGGeom_Cylinder;
    }
    else if(adaptedSurface1.GetType() == GeomAbs_Cone)
    {
            myCurrentSurfaceType = GeomAbs_Cone;
            mccadSurface1 = new McCadCSGGeom_Cone;
            mccadSurface2 = new McCadCSGGeom_Cone;
    }
    else if(adaptedSurface1.GetType() == GeomAbs_Sphere)
    {
            myCurrentSurfaceType = GeomAbs_Sphere;
            mccadSurface1 = new McCadCSGGeom_Sphere;
            mccadSurface2 = new McCadCSGGeom_Sphere;
    }
    else if(adaptedSurface1.GetType() == GeomAbs_Torus)
    {
            myCurrentSurfaceType = GeomAbs_Torus;
            mccadSurface1 = new McCadCSGGeom_Torus;
            mccadSurface2 = new McCadCSGGeom_Torus;
    }
    else
            myCurrentSurfaceType = GeomAbs_OtherSurface;


    if(myCurrentSurfaceType == GeomAbs_OtherSurface)
            return Standard_False;

    mccadSurface1->SetCasSurf(adaptedSurface1);
    mccadSurface2->SetCasSurf(adaptedSurface2);

    if(!mccadSurface1->IsEqual(mccadSurface2))
            return Standard_False;


    // check if the faces have a shared edge
    for(TopExp_Explorer ex1(f1,TopAbs_EDGE); ex1.More(); ex1.Next())
    {
            TopoDS_Edge e1 = TopoDS::Edge(ex1.Current());
            for(TopExp_Explorer ex2(f2,TopAbs_EDGE); ex2.More(); ex2.Next())
            {
                    TopoDS_Edge e2 = TopoDS::Edge(ex2.Current());
                    if(e1.IsSame(e2))  // fails see trac#21
                            return Standard_True;
            }
    }

    return Standard_False;

}

Standard_Boolean McCadCSGTool_FaceFuser::FaceBoundaryOnUVIsoLines(const TopoDS_Face& theFace)
{
	// this does only make sense for cones and cylinders!!
	TopLoc_Location loc;
	const Handle(Geom_Surface)& surface = BRep_Tool::Surface(theFace,loc);
	GeomAdaptor_Surface adaptedSurface(surface);

	Standard_Boolean onBounds(Standard_False);
	Standard_Boolean firstSwitch(Standard_True);

	gp_Ax1 mainAx;
	if(adaptedSurface.GetType() == GeomAbs_Cylinder)
		mainAx = adaptedSurface.Cylinder().Axis();
	else if(adaptedSurface.GetType() == GeomAbs_Cone)
		mainAx = adaptedSurface.Cone().Axis();
	else if(adaptedSurface.GetType() == GeomAbs_Sphere)
		;
	else if(adaptedSurface.GetType() == GeomAbs_Torus)
		mainAx = adaptedSurface.Torus().Axis();

	for(TopExp_Explorer ex1(theFace, TopAbs_EDGE); ex1.More(); ex1.Next())
	{
		Standard_Real first, mid, last;
		TopoDS_Edge e1 = TopoDS::Edge(ex1.Current());

		Handle(Geom_Curve) theCurve = BRep_Tool::Curve(e1, first, last);

		GeomAbs_CurveType crvType = GeomAdaptor_Curve(theCurve).GetType();

		// check if curve lies on a plane ;;
		if( crvType == GeomAbs_Line )
			continue;
		if( crvType != GeomAbs_Circle )
		{
			return Standard_False;
		}


		if(firstSwitch)
		{
			firstSwitch = Standard_False;
			onBounds = Standard_True;
		}

		mid = (first+last)/2.0;

		gp_Pnt p1, p2, p3;
		theCurve->D0(first, p1);
		theCurve->D0(mid, p2);
		theCurve->D0(last, p3);

		gp_Vec v1(p2,p1), v2(p2,p3), v1xv2;
		v1xv2 = v1.Crossed(v2);

		gp_Dir dir(v1xv2);
		gp_Ax1 ax(p2, dir);

		if(!ax.IsParallel(mainAx, 0.01))
			onBounds = Standard_False;
	}

	return onBounds;
}


// Perform fusion operations
/////////////////////////////
void McCadCSGTool_FaceFuser::Fuse()
{
        McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
	Standard_Boolean unsupportedSurfaceType(Standard_False);
        myFusionFailed = false;

	myFusedFaces->Clear();
	myFusedFaces->Append(myOriginalFaces);

	for(Standard_Integer i=1; i<=myFusedFaces->Length(); i++)
	{
		TopoDS_Face f1 = TopoDS::Face(myFusedFaces->Value(i));

		// Check surface area
		GProp_GProps GP1;
		BRepGProp::SurfaceProperties(f1, GP1);
		if(GP1.Mass() < 0.1)
			continue;

		if(myUseReverseOrientedOnly == Standard_True && f1.Orientation() != TopAbs_REVERSED)
		{
			myFusedFaces->Remove(i);
			i--;
			continue;
		}

		for(Standard_Integer j=myFusedFaces->Length(); j>i; j--)
		{
			TopoDS_Face f2 = TopoDS::Face(myFusedFaces->Value(j));

			// Check surface area
			GProp_GProps GP2;
			BRepGProp::SurfaceProperties(f2, GP2);
			if(GP2.Mass() < 0.1)
				continue;

			if(myUseReverseOrientedOnly == Standard_True && f2.Orientation() != TopAbs_REVERSED)
			{
				myFusedFaces->Remove(j);
				continue;
			}

			if(IsLateral(f1, f2))
			{
				Handle_TopTools_HSequenceOfShape curFaces = new TopTools_HSequenceOfShape;
				curFaces->Append(f1);
				curFaces->Append(f2);

				// Try to use the OpenCascade Boolean Operation Fuse Method
				if(!OCCFuse(curFaces))
				{
					switch(myCurrentSurfaceType)
					{
						case GeomAbs_Plane:
							PlaneFuse(curFaces);
							break;
						case GeomAbs_Cylinder:
							CylinderFuse(curFaces);
							break;
						case GeomAbs_Cone:
							CylinderFuse(curFaces);
							break;
						case GeomAbs_Sphere:
							SphereFuse(curFaces);
							break;
						case GeomAbs_Torus:
							SphereFuse(curFaces);
							break;
						default:
                                                        msgr->Message("_#_McCadCSGTool_FaceFuser :: Trying to fuse faces of unsupported surface type!",
                                                                      McCadMessenger_WarningMsg);
							myFusedFaces->Remove(j);
							myFusedFaces->Remove(i);
							i--;
							unsupportedSurfaceType = Standard_True;
							break;
					}

					// break j-loop
					if(unsupportedSurfaceType)
					{
						unsupportedSurfaceType = Standard_False;
						break;
					}

					if(curFaces->Length() > 1)
                                                myFusionFailed = Standard_True;
					else
					{
						myFusedFaces->Remove(j);
						myFusedFaces->Remove(i);
						myFusedFaces->Append(curFaces);
						i--;
						break; // j-loop
					}
				}
				else // OCC fuse operation was successful
				{
					myFusedFaces->Remove(j);
					myFusedFaces->Remove(i);
					myFusedFaces->Append(curFaces);
					i--;
					break; // j-loop
				}
			}
		}
	}
        myIsDone = true;
}

// Try OCC fuse operation
//////////////
Standard_Boolean McCadCSGTool_FaceFuser::OCCFuse(Handle_TopTools_HSequenceOfShape& faces)
{
    McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();

	if(faces->Length() != 2)
		return Standard_False;

	TopoDS_Face f1 = TopoDS::Face(faces->Value(1));
	TopoDS_Face f2 = TopoDS::Face(faces->Value(2));

	TopoDS_Shape fusedShape;

	try
	{
		// use older BRepAlgo_Fuse instead of BRepAlgoAPI_Fuse; it delivers better results...
		BRepAlgo_Fuse occFuser(f1, f2);

		if(!occFuser.IsDone())
		{
                        msgr->Message("_#_McCadCSGTool_FaceFuser :: Boolean Fuse Operation failed for two faces!",
                                      McCadMessenger_WarningMsg);
			return Standard_False;
		}

		fusedShape = occFuser.Shape();
	}
	catch(...)
	{
                msgr->Message("_#_McCadCSGTool_FaceFuser :: Boolean Fuse Operation crashed!!!\n",
                              McCadMessenger_WarningMsg);
		return Standard_False;
	}

	Handle_TopTools_HSequenceOfShape fuseResults = new TopTools_HSequenceOfShape;

	for(TopExp_Explorer ex(fusedShape, TopAbs_FACE); ex.More(); ex.Next())
	{
		TopoDS_Face theNewFace = TopoDS::Face(ex.Current());
		fuseResults->Append(theNewFace);
	}

	if(fuseResults->Length() != 1)
		return Standard_False;

	TopoDS_Face newFace = TopoDS::Face(fuseResults->Value(1));

	// check validity of Result
	//////////////////////////////////////
	BRepCheck_Analyzer faceAnalyzer(newFace, Standard_True);
	if(!faceAnalyzer.IsValid())
		return Standard_False;

	// adjust orientation if neccessary -- in some cases this is needed (even for fusion of planar faces)!!!
	faces->Clear();
	if(newFace.Orientation() != f1.Orientation())
		newFace.Orientation(f1.Orientation());
	faces->Append(newFace);

	return Standard_True;
}


// surface specific fuse

void McCadCSGTool_FaceFuser::CylinderFuse(Handle_TopTools_HSequenceOfShape& faces)
{
	TopoDS_Face f1 = TopoDS::Face(faces->Value(1));
	TopoDS_Face f2 = TopoDS::Face(faces->Value(2));

	if(FaceBoundaryOnUVIsoLines(f1) && FaceBoundaryOnUVIsoLines(f2))
	{
            Standard_Real UMin1(0.0), UMin2(0.0), UMin(0.0), UMax1(0.0), UMax2(0.0), UMax(0.0),
                          VMin1(0.0), VMin2(0.0), VMin(0.0), VMax1(0.0), VMax2(0.0), VMax(0.0);
		BRepTools::UVBounds(f1,UMin1,UMax1, VMin1, VMax1);
		BRepTools::UVBounds(f2,UMin2,UMax2, VMin2, VMax2);

		// Nullify small dimensions
		if(Abs(UMin1) < 1.e-10)
			UMin1 = 0.0;
		if(Abs(UMin2) < 1.e-10)
			UMin2 = 0.0;
		if(Abs(UMax1) < 1.e-10)
			UMax1 = 0.0;
		if(Abs(UMax2) < 1.e-10)
			UMax2 = 0.0;
		if(Abs(VMin1) < 1.e-10)
			VMin1 = 0.0;
		if(Abs(VMin2) < 1.e-10)
			VMin2 = 0.0;
		if(Abs(VMax1) < 1.e-10)
			VMax1 = 0.0;
		if(Abs(VMax2) < 1.e-10)
			VMax2 = 0.0;

		// figure out the boundary
        if(Abs(UMax1-UMin1+UMax2-UMin2 - 2*M_PI) < 1e-5)
		{
			UMin = 0;
            UMax = 2*M_PI;
		}
		else
		{
                        if( Abs(UMin1-UMax2) < 1.0e-5 || ( UMin1 == 0 && Abs(UMax2-2*M_PI) < 1e-7 ) )
			{
				UMin = UMin2;
				UMax = UMax1;
			}
                        else if( Abs(UMin2-UMax1) < 1.0e-5 || ( UMin2 == 0 && Abs(UMax1-2*M_PI) < 1e-7 ) )
			{
				UMin = UMin1;
				UMax = UMax2;
			}

			if(UMax < UMin)
                UMax += 2*M_PI;
		}

		if (VMin1 <= VMin2)
			VMin = VMin1;
		else
			VMin = VMin2;

		if (VMax1 >= VMax2)
			VMax = VMax1;
		else
			VMax = VMax2;

		// make new face
		TopLoc_Location loc;
		Handle(Geom_Surface) surface = BRep_Tool::Surface(f1,loc);
#ifdef OCC650
		TopoDS_Face theNewFace  = BRepBuilderAPI_MakeFace(surface, UMin,UMax, VMin, VMax).Face();
#else
		TopoDS_Face theNewFace  = BRepBuilderAPI_MakeFace(surface, UMin,UMax, VMin, VMax, 1.e-7).Face();
#endif
		theNewFace.Orientation(f1.Orientation());

		faces->Clear();
		faces->Append(theNewFace);
	}
        else // Todo : how to fuse to lateral cylindrical faces, that have skewed ends?
	{

	}
}

void McCadCSGTool_FaceFuser::ConeFuse(Handle_TopTools_HSequenceOfShape& faces)
{
	cout << "Cone Fuse -- NOT IMPLEMENTED!!!!!!!!!!!\n";
}

void McCadCSGTool_FaceFuser::SphereFuse(Handle_TopTools_HSequenceOfShape& faces)
{
	if(faces->Length() != 2)
		return;

	TopoDS_Face f1 = TopoDS::Face(faces->Value(1));
	TopoDS_Face f2 = TopoDS::Face(faces->Value(2));

        Standard_Real UMin1(0.0), UMin2(0.0), UMin(0.0), UMax1(0.0), UMax2(0.0), UMax(0.0),
                VMin1(0.0), VMin2(0.0), VMin(0.0), VMax1(0.0), VMax2(0.0), VMax(0.0);
	BRepTools::UVBounds(f1,UMin1,UMax1, VMin1, VMax1);
	BRepTools::UVBounds(f2,UMin2,UMax2, VMin2, VMax2);

	// Nullify small dimensions
	if(Abs(UMin1) < 1.e-10)
		UMin1 = 0.0;
	if(Abs(UMin2) < 1.e-10)
		UMin2 = 0.0;
	if(Abs(UMax1) < 1.e-10)
		UMax1 = 0.0;
	if(Abs(UMax2) < 1.e-10)
		UMax2 = 0.0;
	if(Abs(VMin1) < 1.e-10)
		VMin1 = 0.0;
	if(Abs(VMin2) < 1.e-10)
		VMin2 = 0.0;
	if(Abs(VMax1) < 1.e-10)
		VMax1 = 0.0;
	if(Abs(VMax2) < 1.e-10)
		VMax2 = 0.0;

	if(Abs(UMin1 - UMin2) < 1e-7 && Abs(UMax1-UMax2) < 1e-7 )
	{
        if(Abs((VMax2-VMin1)-2*M_PI)<1e-7 || Abs((VMax1-VMin2)-2*M_PI)<1e-7)
		{
			VMin = 0;
            VMax = 2*M_PI;
		}
		UMin = UMin1;
		UMax = UMax1;
	}
	if(Abs(VMin1 - VMin2) < 1e-7 && Abs(VMax1 - VMax2) < 1e-7)
	{
        if(Abs((UMax2-UMin1)-2*M_PI)<1e-7 || Abs((UMax1-UMin2)-2*M_PI)<1e-7)
		{
			UMin = 0;
            UMax = 2*M_PI;
		}
		VMin=VMin1;
		VMax=VMax1;
	}

	TopLoc_Location loc;
	Handle(Geom_Surface) surface = BRep_Tool::Surface(f1,loc);
#ifdef OCC650
	TopoDS_Face theNewFace  = BRepBuilderAPI_MakeFace(surface, UMin,UMax, VMin, VMax).Face();
#else
	TopoDS_Face theNewFace  = BRepBuilderAPI_MakeFace(surface, UMin,UMax, VMin, VMax, 1.e-7).Face();
#endif
	theNewFace.Orientation(f1.Orientation());

	faces->Clear();
	faces->Append(theNewFace);
}

void McCadCSGTool_FaceFuser::TorusFuse(Handle_TopTools_HSequenceOfShape& faces)
{
	cout << "Torus Fuse\n ";

	if(faces->Length() != 2)
			return;

		TopoDS_Face f1 = TopoDS::Face(faces->Value(1));
		TopoDS_Face f2 = TopoDS::Face(faces->Value(2));

                Standard_Real UMin1(0.0), UMin2(0.0), UMin(0.0), UMax1(0.0), UMax2(0.0), UMax(0.0),
                              VMin1(0.0), VMin2(0.0), VMin(0.0), VMax1(0.0), VMax2(0.0), VMax(0.0);
		BRepTools::UVBounds(f1,UMin1,UMax1, VMin1, VMax1);
		BRepTools::UVBounds(f2,UMin2,UMax2, VMin2, VMax2);

		// Nullify small dimensions
		if(Abs(UMin1) < 1.e-10)
			UMin1 = 0.0;
		if(Abs(UMin2) < 1.e-10)
			UMin2 = 0.0;
		if(Abs(UMax1) < 1.e-10)
			UMax1 = 0.0;
		if(Abs(UMax2) < 1.e-10)
			UMax2 = 0.0;
		if(Abs(VMin1) < 1.e-10)
			VMin1 = 0.0;
		if(Abs(VMin2) < 1.e-10)
			VMin2 = 0.0;
		if(Abs(VMax1) < 1.e-10)
			VMax1 = 0.0;
		if(Abs(VMax2) < 1.e-10)
			VMax2 = 0.0;

		//cout << UMin1 << "  " << UMax1 << "  -  " << VMin1 << "  " << VMax1 << endl;
		//cout << UMin2 << "  " << UMax2 << "  -  " << VMin2 << "  " << VMax2 << endl;

		if(Abs(UMin1 - UMin2) < 1e-7 && Abs(UMax1-UMax2) < 1e-7 )
		{
            if(Abs((VMax2-VMin1)-2*M_PI)<1e-7 || Abs((VMax1-VMin2)-2*M_PI)<1e-7)
			{
				VMin = 0;
                VMax = 2*M_PI;
			}
			UMin = UMin1;
			UMax = UMax1;
		}
		if(Abs(VMin1 - VMin2) < 1e-7 && Abs(VMax1 - VMax2) < 1e-7)
		{
            if(Abs((UMax2-UMin1)-2*M_PI)<1e-7 || Abs((UMax1-UMin2)-2*M_PI)<1e-7)
			{
				UMin = 0;
                UMax = 2*M_PI;
			}
			VMin=VMin1;
			VMax=VMax1;
		}

		TopLoc_Location loc;
		Handle(Geom_Surface) surface = BRep_Tool::Surface(f1,loc);
#ifdef OCC650
		TopoDS_Face theNewFace  = BRepBuilderAPI_MakeFace(surface, UMin,UMax, VMin, VMax).Face();
#else
		TopoDS_Face theNewFace  = BRepBuilderAPI_MakeFace(surface, UMin,UMax, VMin, VMax, 1.e-7).Face();
#endif
		theNewFace.Orientation(f1.Orientation());

/*		STEPControl_Writer wrt;
		wrt.Transfer(f1, STEPControl_AsIs);
		wrt.Transfer(f2, STEPControl_AsIs);
		wrt.Transfer(theNewFace, STEPControl_AsIs);
		wrt.Write("torus.stp");*/

		faces->Clear();
		faces->Append(theNewFace);
}

void McCadCSGTool_FaceFuser::PlaneFuse(Handle_TopTools_HSequenceOfShape& faces)
{
// fusing of planes is not necessary for the algorithm
//    the fusing is ment for higher order surfaces
}

