#include <vector>
#include <list>
#include <McCadMcVoid.hxx>
#include <McCadCSGTool.hxx>
#include <McCadGTOOL.hxx>
#include <McCadCSGGeom_HSequenceOfCell.hxx>
#include <McCadCSGTool_Decomposer.hxx>
#include <McCadTDS_HSequenceOfExtSolid.hxx>
#include <McCadCSGTool_Extender.hxx>
#include <McCadCSGTool_CellBuilder.hxx>
#include <McCadCSGGeom_Cell.hxx>
#include <McCadCSGGeom_Surface.hxx>
#include <McCadCSGAdapt_SolidAnalyser.hxx>
#include <McCadCSGAdapt_ShapeAnalyser.hxx>
#include <McCadTDS_ExtSolid.hxx>
#include <McCadCSGAdapt_FaceAnalyser.hxx>
#include <McCadCSGBuild_SolidFromCSG.hxx>

#include <McCadCSGGeom_Plane.hxx>
#include <McCadCSGGeom_Cylinder.hxx>
#include <McCadCSGGeom_Cone.hxx>
#include <McCadCSGGeom_Sphere.hxx>
#include <McCadCSGGeom_Torus.hxx>
#include <McCadCSGGeom_GQ.hxx>
#include <McCadCSGGeom_SurfaceOfRevolution.hxx>

#include <McCadCSGGeom_DataMapOfIntegerSurface.hxx>
#include <McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface.hxx>

#include <McCadDiscDs_DiscFace.hxx>
#include <McCadDiscDs_HSequenceOfDiscFace.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_Orientation.hxx>

#include <ShapeFix_Solid.hxx>

#include <TColStd_HSequenceOfReal.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColgp_HSequenceOfPnt.hxx>

#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom_Plane.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>

#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepAlgoAPI_Common.hxx>

#include <BRepPrim_Sphere.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTools.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>

#include <BRepTools_TrsfModification.hxx>
#include <BRepTools_Modifier.hxx>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <STEPControl_Writer.hxx>

#include <ShapeAnalysis_Surface.hxx>
#include <gp_Pnt2d.hxx>

#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

using namespace std;

Standard_Real McCadMcVoid::VolumeOfShape(const TopoDS_Shape& theShape)
{
	GProp_GProps GP;
	BRepGProp::VolumeProperties(theShape, GP);
	return GP.Mass();
}


Standard_Real McCadMcVoid::AreaOfShape(const TopoDS_Shape& theShape)
{
	GProp_GProps GP;
	BRepGProp::SurfaceProperties(theShape, GP);
	return GP.Mass();
}


gp_Pnt McCadMcVoid::ApproxBaryCenter(const TopoDS_Shape& theShape)
{
	Standard_Integer i;
	TopExp_Explorer ex;
	gp_XYZ xyz(0, 0, 0);
	for (ex.Init(theShape, TopAbs_VERTEX), i = 1; ex.More(); ex.Next(), i++)
		xyz += BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())).XYZ();
	xyz /= i;
	return gp_Pnt(xyz);
}


TopoDS_Shape McCadMcVoid::RelScale(const TopoDS_Shape& theShape, const gp_Pnt& thePnt, const Standard_Real theFact)
{
	gp_Trsf T;
	//  gp_Pnt  thePnt = ApproxBaryCenter(S);
	T.SetScale(thePnt, theFact); // we scale up or down relative to barycenter depending on theFact;
	Handle(BRepTools_TrsfModification) TrsfMod = new BRepTools_TrsfModification(T);
	BRepTools_Modifier Md;
	Md.Init(theShape);
	Md.Perform(TrsfMod);
	if (Md.IsDone())
	{
		return Md.ModifiedShape(theShape);
	}
	cout << "Can not scale/modify shape !! " << endl;
	return theShape;
}


TopoDS_Shape McCadMcVoid::Scale(const TopoDS_Shape& theShape, const Standard_Real theFact)
{
	gp_Trsf T;
	gp_Pnt thePnt(0, 0, 0);
	T.SetScale(thePnt, theFact); // we scale up or down relative to barycenter depending on theFact;
	Handle(BRepTools_TrsfModification) TrsfMod = new BRepTools_TrsfModification(T);
	BRepTools_Modifier Md;
	Md.Init(theShape);
	Md.Perform(TrsfMod);
	if (Md.IsDone())
	{
		return Md.ModifiedShape(theShape);
	}
	cout << "Can not scale/modify shape !! " << endl;
	return theShape;
}


Handle(McCadCSGGeom_Surface) McCadMcVoid::MakeMcCadSurf(GeomAdaptor_Surface& theAdapSurface)
{
	Handle(McCadCSGGeom_Surface) aCSGSurf;

	switch (theAdapSurface.GetType())
	{
		case GeomAbs_Plane:
		aCSGSurf = new McCadCSGGeom_Plane;
		break;
		case GeomAbs_Cylinder:
		aCSGSurf = new McCadCSGGeom_Cylinder;
		break;
		case GeomAbs_Cone:
		aCSGSurf = new McCadCSGGeom_Cone;
		break;
		case GeomAbs_Sphere:
		aCSGSurf = new McCadCSGGeom_Sphere;
		break;
		case GeomAbs_Torus:
		aCSGSurf = new McCadCSGGeom_Torus;
		break;
		case GeomAbs_BezierSurface:
		{
			cout << "_#_McCadMcVoid.cxx :: Surface is not analytic (BezierSurface) " << endl;
			break;
		}
		case GeomAbs_BSplineSurface:
		{
			cout << "_#_McCadMcVoid.cxx :: Surface is not analytic (BSplineSurface) " << endl;
			aCSGSurf = new McCadCSGGeom_GQ;
			break;
		}
		case GeomAbs_SurfaceOfRevolution:
		{
			aCSGSurf = new McCadCSGGeom_SurfaceOfRevolution;
			//cout << "_#_McCadMcVoid.cxx :: Surface is not analytic (SurfaceOfRevolution) " << endl;
			break;
		}
		case GeomAbs_SurfaceOfExtrusion:
		{
			cout << "_#_McCadMcVoid.cxx :: Surface is not analytic (SurfaceOfExtrusion)  " << endl;
			break;
		}
		case GeomAbs_OffsetSurface:
		{
			cout << "_#_McCadMcVoid.cxx :: Surface is not analytic (OffsetSurface) " << endl;
			break;
		}
		case GeomAbs_OtherSurface:
		{
			cout << "Surface is not analytic " << endl;
			break;
		}
	}

	aCSGSurf->SetCasSurf(theAdapSurface);

	return aCSGSurf;
}


Standard_Boolean McCadMcVoid::ApproxGeomEqual(Handle(McCadCSGGeom_Surface)& S1,Handle(McCadCSGGeom_Surface)& S2)
{
	// only planar surface are treated

	Handle(Geom_Plane) Gpln1 = Handle_Geom_Plane::DownCast(S1->CasSurf());
	gp_Pln aPln = Gpln1->Pln();
	gp_Ax3 pos = aPln.Position();
	gp_Dir dir = pos.Direction();
	gp_Pnt loc = pos.Location ();
	Standard_Real sA,sB,sC,sD;
	aPln.Coefficients(sA,sB,sC,sD);
	Standard_Real D1 = dir.X()*loc.X() + dir.Y()*loc.Y() + dir.Z()*loc.Z();

	Handle(Geom_Plane) Gpln = Handle_Geom_Plane::DownCast(S2->CasSurf());
	gp_Pln other = Gpln->Pln();

	gp_Ax3 otherPos = other.Position();
	gp_Dir otherDir = otherPos.Direction();
	gp_Pnt otherLoc = otherPos.Location();
	Standard_Real D2 = otherDir.X()*otherLoc.X() + otherDir.Y()*otherLoc.Y() + otherDir.Z()*otherLoc.Z();
	if (dir.IsParallel(otherDir,0.1))
	{
		if (fabs(D1 - D2) <= 1.0) // this gives the resolution of the cut planes: 25cm
		{
			return Standard_True;
		}
	}
	// if(dir.Angle(otherDir) < 0.034906585) // about 2 deg.
	//  return Standard_True;
	return Standard_False;
}


Standard_Boolean McCadMcVoid::ApproxGeomEqualFaces(TopoDS_Face& F1,	TopoDS_Face& F2, Standard_Real theCFRes)
{
	// only planar face are treated

	TopLoc_Location l;
	Handle(Geom_Surface) gS1 = BRep_Tool::Surface(F1,l);
	GeomAdaptor_Surface adSurf1(gS1);
	if(adSurf1.GetType() != GeomAbs_Plane)
		return Standard_False;

	Handle(Geom_Plane) Gpln1 = Handle_Geom_Plane::DownCast(gS1);
	gp_Pln aPln = Gpln1->Pln();
	gp_Ax3 pos = aPln.Position();
	gp_Dir dir = pos.Direction();
	//gp_Pnt loc = pos.Location ();
	//  Standard_Real D1 = dir.X()*10.e10 + dir.Y()*10.e10 + dir.Z()*loc.Z();

	Handle(Geom_Surface) gS2 = BRep_Tool::Surface(F2,l);
	GeomAdaptor_Surface adSurf2(gS2);
	if(adSurf2.GetType() != GeomAbs_Plane)
			return Standard_False;

	Handle(Geom_Plane) Gpln = Handle_Geom_Plane::DownCast(gS2);
	gp_Pln other = Gpln->Pln();
	gp_Ax3 otherPos = other.Position();
	gp_Dir otherDir = otherPos.Direction();
	//gp_Pnt otherLoc = otherPos.Location();
	//  Standard_Real D2 = otherDir.X()*otherLoc.X() + otherDir.Y()*otherLoc.Y() + otherDir.Z()*otherLoc.Z() ;
	//gp_Pnt pinf(10.e6,10.e6,10.e6);
	gp_Pnt pinf(0,0,0);
	if (dir.IsParallel(otherDir,0.0001))
	{
		 //cout << " Distance  =  " << fabs(aPln.Distance(pinf) - other.Distance(pinf)) << endl;
		/* Standard_Real A,B,C,D;
		 aPln.Coefficients(A,B,C,D);
		 cout  << "me.   A =  " << A << " B =  " <<  B << " C =  "  << C <<" D =  "  << D << endl;
		 other.Coefficients(A,B,C,D);
		 cout  << "ot.   A =  " << A << " B =  " <<  B << " C =  "  << C <<" D =  "  << D << endl;*/
//not reliable for infinit point!!!
// infp leads to major differences in output between GUI and non-GUI version.
	//	cout << " Dist : " << fabs(aPln.Distance(pinf) - other.Distance(pinf)) << " - ";

		if (fabs(aPln.Distance(pinf) - other.Distance(pinf)) <= theCFRes)
		{
		//	cout << "REJECTED because of small distance : " << fabs(aPln.Distance(pinf) - other.Distance(pinf));
			return Standard_True;
		}
	}

	return Standard_False;
}


Handle(McCadDiscDs_HSequenceOfDiscFace) McCadMcVoid::DeleteApproxRedundant(Handle(McCadDiscDs_HSequenceOfDiscFace)& theVCutFaceSeq, Standard_Real theCFRes)
{
	/**/
	/**/

	/////////////////////////////////////////////////////////////////////////////////////////////////
	if (theVCutFaceSeq->Length() < 1)
		return theVCutFaceSeq;


	// Sort by Area
	for(Standard_Integer i=1 ; i<theVCutFaceSeq->Length(); i++)
	{
		for(Standard_Integer j=i+1; j<=theVCutFaceSeq->Length(); j++)
		{
			TopoDS_Face F1,F2;
			F1 = TopoDS::Face(theVCutFaceSeq->Value(j).GetFace());
			F2 = TopoDS::Face(theVCutFaceSeq->Value(i).GetFace());
			if(AreaOfShape(F1) > AreaOfShape(F2))
				theVCutFaceSeq->Exchange(i,j);
		}
	}


	// only planar faces are treated
	/*cout << "=================================================================" << endl;
	cout << "                    Cut Face Canditdates                             " <<endl;
	cout << "=================================================================" << endl;

	for (int i=1; i<= theVCutFaceSeq->Length(); i++) // print all coefficients of plane-equations to screen
	{
		TopoDS_Face F1 = TopoDS::Face(theVCutFaceSeq->Value(i).GetFace());
		TopLoc_Location loc;
		Handle(Geom_Surface) gS1 = BRep_Tool::Surface(F1,loc);
		GeomAdaptor_Surface As1(gS1);
		gp_Pln aPln = As1.Plane();
		Standard_Real A,B,C,D;
		aPln.Coefficients(A,B,C,D);
		cout << i << ".   A = " << A << "  B = " << B << "  C = " << C <<"  D = " << D << endl;
	}*/
	/////////////////////////////////////////////////////////////////////////////////////////////////


	vector<McCadDiscDs_DiscFace> theVCutFace, tmpVCutFace;
	vector<McCadDiscDs_DiscFace>::iterator fit;
	for (int i=1; i<= theVCutFaceSeq->Length(); i++)//put all vcutfaces in above vectors
	{
		tmpVCutFace.push_back(theVCutFaceSeq->Value(i));
		theVCutFace.push_back(theVCutFaceSeq->Value(i));
	}

	int io = 0;
	for (fit = tmpVCutFace.begin(); fit!= tmpVCutFace.end(); fit++)//For all temp voxel faces delete 'double faces'
	{
		io++;
		TopoDS_Face F1 = TopoDS::Face((*fit).GetFace());
		if(F1.IsNull())
			continue;

		vector<McCadDiscDs_DiscFace>::iterator fit2;
		int ii = io;
		for (fit2 = fit+1; fit2!= tmpVCutFace.end(); fit2++)
		{
			ii++;
		//	cout << "\n(" << io << "," << ii << ") : ";

			TopoDS_Face F2 = TopoDS::Face((*fit2).GetFace());

			if(F2.IsNull())
			{
				continue;
			}
			if (ApproxGeomEqualFaces(F1, F2, 1.e-3))
			{
				McCadDiscDs_DiscFace vnull; // create empts DiscFace
				//if(AreaOfShape(F2)<AreaOfShape(F1))
					*fit2 = vnull;
				//else
				//	*fit = vnull;
			}
		}
	}

	theVCutFace.erase(theVCutFace.begin(),theVCutFace.end());
	int idel = 0;
	for (fit = tmpVCutFace.begin(); fit!= tmpVCutFace.end(); fit++)
	{
		idel++;
		TopoDS_Face F1 = TopoDS::Face((*fit).GetFace());
		if(!F1.IsNull())
			theVCutFace.push_back(*fit);
	}

/*	cout << "=================================================================" << endl;
	cout << "                      Cut Face Data                              " <<endl;
	cout << "=================================================================" << endl;

	int iii=0;
	for (fit = theVCutFace.begin(); fit!= theVCutFace.end(); fit++)//For all cut faces print plane equation and area
	{
		iii++;
		TopoDS_Face F1 = TopoDS::Face((*fit).GetFace());
		Standard_Real area = AreaOfShape(F1);
		TopLoc_Location loc;
		Handle(Geom_Surface) gS1 = BRep_Tool::Surface(F1,loc);
		GeomAdaptor_Surface As1(gS1);
		gp_Pln aPln = As1.Plane();
		Standard_Real A,B,C,D;
		aPln.Coefficients(A,B,C,D);
		if(fabs(A)<1.e-7) A = 0.0;	//MYMOD 230108 all 4
		if(fabs(B)<1.e-7) B = 0.0;
		if(fabs(C)<1.e-7) C = 0.0;
		if(fabs(D)<1.e-7) D = 0.0;
		cout << iii << ".   A = " << A << "  B = " << B << "  C = " << C <<"  D = " << D << "    Area = " << area << endl;

	}*/
	//cout << "====================================================================================== " << endl;
//	cout << "====================================================================================== " << endl;
	////////////////////////////////////////////////////////////////////////////////////////////
	//sort here
/*	int ii = 0;
	//  tmpVCutFace.erase(tmpVCutFace.begin(),tmpVCutFace.end());
	if(theVCutFace.size() > 1)
	{
		for (fit = theVCutFace.begin() + 1; fit!= theVCutFace.end(); fit++) //arrange faces by size (decreasing)
		{
			TopoDS_Face F1 = TopoDS::Face((*fit).GetFace());
			Standard_Real areaF1 = AreaOfShape(F1);
			vector<McCadDiscDs_DiscFace>::iterator fit2;
			for (fit2 = fit - 1; fit2 >= theVCutFace.begin(); fit2--)
			{
				TopoDS_Face F2 = TopoDS::Face((*fit2).GetFace());
				Standard_Real areaF2 = AreaOfShape(F2);
				if (areaF1 > areaF2)
				{
					iter_swap(fit2 + 1,fit2);
				}
				else
					break;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
	ii = 0;
	for (fit = theVCutFace.begin(); fit!= theVCutFace.end(); fit++)
	{
		ii++;
		TopoDS_Face F1 = TopoDS::Face((*fit).GetFace());
		Standard_Real area = AreaOfShape(F1);
		TopLoc_Location loc;
		Handle(Geom_Surface) gS1 = BRep_Tool::Surface(F1,loc);
		GeomAdaptor_Surface As1(gS1);
		gp_Pln aPln = As1.Plane();
		Standard_Real A,B,C,D;
		aPln.Coefficients(A,B,C,D);
		if(fabs(A)<1.e-7) A = 0.0;//MYMOD 230108 all 4
		if(fabs(B)<1.e-7) B = 0.0;
		if(fabs(C)<1.e-7) C = 0.0;
		if(fabs(D)<1.e-7) D = 0.0;
		//cout << ii << ".   A = " << A << "  B = " << B << "  C = " << C <<"  D = " << D << "     Area = " << area << endl;
	}*/
	//cout << "================================================================= " << endl;

	theVCutFaceSeq->Clear();

	for (fit = theVCutFace.begin(); fit!= theVCutFace.end(); fit++)
	{
		TopoDS_Face F1 = TopoDS::Face((*fit).GetFace());
		if(!F1.IsNull()) theVCutFaceSeq->Append(*fit);
	}

	return theVCutFaceSeq;
}


Handle(TopTools_HSequenceOfShape) McCadMcVoid::DeleteOne(Handle(TopTools_HSequenceOfShape)& theFaceSeq)
{

	if (theFaceSeq->Length() < 2 )
		return theFaceSeq;

	TopoDS_Face F1 = TopoDS::Face(theFaceSeq->Value(1));
	TopAbs_Orientation orient1 = F1.Orientation();
	TopLoc_Location loc;
	Handle(Geom_Surface) gS1 = BRep_Tool::Surface(F1,loc);
	GeomAdaptor_Surface As1(gS1);
	TopoDS_Face F2 = TopoDS::Face(theFaceSeq->Value(2));
	TopAbs_Orientation orient2 = F2.Orientation();
	Handle(Geom_Surface) gS2 = BRep_Tool::Surface(F2,loc);
	GeomAdaptor_Surface As2(gS2);

	if (As1.GetType() != As2.GetType() )
		return theFaceSeq;

	Handle(McCadCSGGeom_Surface) S1 = MakeMcCadSurf(As1);
	S1->SetCasSurf(As1);

	Handle(McCadCSGGeom_Surface) S2 = MakeMcCadSurf(As2);
	S2->SetCasSurf(As2);

	////////////////////////////////////////////////////////////////////////////////////
	// a plane must be correctly orriented to be compared ....
	if(As1.GetType() == GeomAbs_Plane)
	{
		gp_Pln aP1 = As1.Plane();
		gp_Ax3 locAxis = aP1.Position();
		gp_Dir locDir = locAxis.Direction();
		gp_Ax3 glAxis (locAxis.Location(),gp_Vec(fabs(locDir.X()),fabs(locDir.Y()),fabs(locDir.Z())));
		gp_Dir glDir = glAxis.Direction();
		Standard_Real A,B,C,D;
		if (locAxis.Direct())
		{
			A = locDir.X();
			B = locDir.Y();
			C = locDir.Z();
		}
		else
		{
			A = -locDir.X();
			B = -locDir.Y();
			C = -locDir.Z();
		}
		const gp_Pnt& P = locAxis.Location();
		D =-(A * P.X() + B * P.Y() + C * P.Z());

		if ((D > 0 && (locDir.X() < 0 || locDir.Y() < 0 || locDir.Z() < 0 )) ||	(locDir.X() <= 0 && locDir.Y() <= 0 && locDir.Z() <= 0 ))
		{
			gp_Ax3 glAxis (locAxis.Location(),gp_Vec(-1*locDir.X(),-1*locDir.Y(),-1*locDir.Z()));
			Handle(Geom_Plane) tmpPl = new Geom_Plane(glAxis);
			GeomAdaptor_Surface Ast(tmpPl);
			S1->SetCasSurf(Ast);
			if(orient1 == TopAbs_FORWARD)
				F1.Orientation(TopAbs_REVERSED);
			else if (orient1 == TopAbs_REVERSED)
				F1.Orientation(TopAbs_FORWARD);
			//else;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////
	if(As2.GetType() == GeomAbs_Plane)
	{
		gp_Pln aP1 = As2.Plane();
		gp_Ax3 locAxis = aP1.Position();
		gp_Dir locDir = locAxis.Direction();
		gp_Ax3 glAxis (locAxis.Location(),gp_Vec(fabs(locDir.X()),fabs(locDir.Y()),fabs(locDir.Z())));
		gp_Dir glDir = glAxis.Direction();
		Standard_Real A,B,C,D;
		if (locAxis.Direct())
		{
			A = locDir.X();
			B = locDir.Y();
			C = locDir.Z();
		}
		else
		{
			A = -locDir.X();
			B = -locDir.Y();
			C = -locDir.Z();
		}
		const gp_Pnt& P = locAxis.Location();
		D =-(A * P.X() + B * P.Y() + C * P.Z());

		if ((D > 0 && (locDir.X() < 0 || locDir.Y() < 0 || locDir.Z() < 0 )) ||	(locDir.X() <= 0 && locDir.Y() <= 0 && locDir.Z() <= 0 ))
		{
			gp_Ax3 glAxis (locAxis.Location(),gp_Vec(-1*locDir.X(),-1*locDir.Y(),-1*locDir.Z()));
			Handle(Geom_Plane) tmpPl = new Geom_Plane(glAxis);
			GeomAdaptor_Surface Ast(tmpPl);
			S2->SetCasSurf(Ast);
			if(orient2 == TopAbs_FORWARD)
				F2.Orientation(TopAbs_REVERSED);
			else if (orient2 == TopAbs_REVERSED)
				F2.Orientation(TopAbs_FORWARD);
			//else;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////


	if (S1->IsEqual(S2))
	{
		if (F1.Orientation()!= F2.Orientation())
		{
			theFaceSeq->Remove(2);
		}
	}

	return theFaceSeq;
}


Handle(TopTools_HSequenceOfShape) McCadMcVoid::ShellOrder(Handle(TopTools_HSequenceOfShape)& theFaceSeq,Handle(TopTools_HSequenceOfShape)& theSolidSeq)
{

	if (theFaceSeq->Length() < 2) return theFaceSeq;
	Handle(TColStd_HSequenceOfInteger) signChanges = new TColStd_HSequenceOfInteger();
	for (Standard_Integer i = 1; i<= theFaceSeq->Length(); i++)
	{
		Standard_Integer signChgs = 0;
		TopoDS_Face F1 = TopoDS::Face(theFaceSeq->Value(i));
		TopLoc_Location loc;
		Handle(Geom_Surface) S1 = BRep_Tool::Surface(F1,loc);
		GeomAdaptor_Surface AS1(S1);
		for (Standard_Integer j = 1; j<= theSolidSeq->Length(); j++)
		{
			Standard_Integer NbNegPnt=0, NbPosPnt=0;
			TopoDS_Solid S1 = TopoDS::Solid(theSolidSeq->Value(j));
			for (TopExp_Explorer exV(S1,TopAbs_VERTEX); exV.More(); exV.Next())
			{
				gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(exV.Current()));
				Standard_Real aVal = McCadGTOOL::Evaluate(AS1,aPnt);
				if(aVal > 1.e-03 && aVal < 1.e+100) NbPosPnt++;
				if (aVal < -1.e-03 && aVal > -1.e+100 ) NbNegPnt++;
			}
			if (NbPosPnt > 0 && NbNegPnt > 0 ) // changes sign on the solid
			signChgs++;
		}
		signChanges->Append(signChgs);
	}
	// now we do the shell ordering according to sign changes!!

	for (Standard_Integer j = 2; j<= signChanges->Length(); j++)
	{
		Standard_Integer k;
		Standard_Integer sign = signChanges->Value(j);
		for (k = j-1; k>=1; k--)
		{
			if (sign < signChanges->Value(k))
			{
				signChanges->SetValue(k+1,signChanges->Value(k));
				theFaceSeq->Exchange(k+1,k);
			}
			else
			break;
		}
		signChanges->SetValue(k+1,sign);
	}
	cout << "========================================================= " << endl;
	cout << "These are the sorted sign changing sequences " << endl;
	for (Standard_Integer j = 1; j<= signChanges->Length(); j++)
	cout << j << "           " << signChanges->Value(j) << endl;
	cout << "========================================================= " << endl;
	return theFaceSeq;
}


Standard_Boolean McCadMcVoid::SignChanging(TopoDS_Solid& baseSolid,	const McCadDiscDs_DiscFace& theVFace)
{
	TopoDS_Face F1 = TopoDS::Face(theVFace.GetFace());
	TopLoc_Location loc;
	Handle(Geom_Surface) gS1 = BRep_Tool::Surface(F1,loc);
	GeomAdaptor_Surface As1(gS1);
	Handle(McCadCSGGeom_Surface) S1 = MakeMcCadSurf(As1);
	S1->SetCasSurf(As1);

	for (TopExp_Explorer exF(baseSolid,TopAbs_FACE); exF.More(); exF.Next()) // if theVFace is equal to solid's bounding face, return false
	{
		TopoDS_Face F2 = TopoDS::Face(exF.Current());
		Handle(Geom_Surface) gS2 = BRep_Tool::Surface(F2,loc);
		GeomAdaptor_Surface As2(gS2);
		if (As1.GetType() == As2.GetType())
		{
			Handle(McCadCSGGeom_Surface) S2 = MakeMcCadSurf(As2);
			S2->SetCasSurf(As2);
			if (S1->IsEqual(S2)) //BB-Interface
				return Standard_False;
		}
	}

	Standard_Boolean inFound = Standard_False;

	Standard_Real MaxTol = 1.e-7;
	BRepClass3d_SolidClassifier bsc3d (baseSolid);
	Handle(TColgp_HSequenceOfPnt) tmpPntSeq = theVFace.GetPoints();

	//cout << "Number of Sample Points : " << tmpPntSeq->Length() << endl;

	for (int j=1; j<= tmpPntSeq->Length(); j++)
	{
		gp_Pnt aPnt = tmpPntSeq->Value(j);
		bsc3d.Perform(aPnt,MaxTol);
		if (bsc3d.State() == TopAbs_IN)
		{
			inFound = Standard_True;
			break;
		}
	}

	//cout << "SIGN CHANGING ????????????? > " << inFound << endl;

	if(inFound)
		return Standard_True;
	else
		return Standard_False;
}


TopoDS_Shape McCadMcVoid::MakeHalfSpace(TopoDS_Shape& aSolid)
{
	gp_Pnt lPnt(-1.e5, -1.e5, -1.e5);
	gp_Pnt uPnt(1.e5, 1.e5, 1.e5);

	return MakeHalfSpace(aSolid, lPnt, uPnt);
}


TopoDS_Shape McCadMcVoid::MakeHalfSpace(TopoDS_Shape& aSolid, gp_Pnt& theLPnt, gp_Pnt& theUPnt)
{
	TopoDS_Face aFace = TopoDS::Face(aSolid);
	TopoDS_Shape HSol;
	bool foundIN = false;
	TopLoc_Location loc;
	//TopAbs_Orientation orient = aFace.Orientation();

	Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(aFace,loc);
	GeomAdaptor_Surface AS(theFaceSurface);

	/*STEPControl_Writer wrt;
	wrt.Transfer(aSolid, STEPControl_AsIs);
	wrt.Write("makeHalfSpace.stp");*/

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	gp_Pnt thePnt = theFaceSurface->Value(1,1);
	gp_Pnt tempPnt;
//MYMOD 170108
	for (Standard_Integer j=1; j<= 3 ; j++) // Find a point that lies on the solid-side of the face
	{
		for (Standard_Integer k=-1; k<=1; k+=2)
		{
			tempPnt = thePnt;
			tempPnt.SetCoord(j, tempPnt.Coord(j)+k);
			if(PointState(aSolid, tempPnt)==TopAbs_IN)
			{
				foundIN = true;
				break;
			}
		}
		if (foundIN == true)
			break;
	}

	if(!foundIN)
		cout << "_#_McCadMcVoid.cxx :: Could not determine inside point for half space construction!" << endl;


	/////////////////////////////////////////////////////////////////////////////////////
	// find the eight projection points on the surface find the two points that are
	// the farest away from the box
	Handle(TColStd_HSequenceOfReal) uVals = new TColStd_HSequenceOfReal;
	Handle(TColStd_HSequenceOfReal) vVals = new TColStd_HSequenceOfReal;

	if(theLPnt.IsEqual(theUPnt, 1.0))
	{
		cout << "same point\n";
		gp_Pnt lPnt(-1.e5, -1.e5, -1.e5);
		gp_Pnt uPnt(1.e5, 1.e5, 1.e5);
		theLPnt = lPnt;
		theUPnt = uPnt;
	}

	TopoDS_Shape theBox = BRepPrimAPI_MakeBox(theLPnt,theUPnt).Shape();
	TopExp_Explorer ex;

	gp_Pnt2d the2dPnt;
	Standard_Real uMin, uMax, vMin, vMax;
	ShapeAnalysis_Surface sAs(theFaceSurface);

	for(ex.Init(theBox, TopAbs_VERTEX); ex.More(); ex.Next())
	{
		//get point
		TopoDS_Vertex theVertex = TopoDS::Vertex(ex.Current());
		gp_Pnt thePnt = BRep_Tool::Pnt(theVertex);

		//project on surface
		GeomAPI_ProjectPointOnSurf pntProj(thePnt, theFaceSurface);
		for(int j=1; j<=pntProj.NbPoints(); j++)
		{
			the2dPnt = sAs.ValueOfUV(pntProj.Point(j), 1e-3);
			uVals->Append(the2dPnt.X());
			vVals->Append(the2dPnt.Y());
		}
	}

	// minimize and maximize
	uMin = uVals->Value(1);
	uMax = uVals->Value(1);
	for(int i=2; i<=uVals->Length(); i++)
	{
		Standard_Real theVal = uVals->Value(i);

		if(uMin > theVal)
			uMin = theVal;
		if(uMax < theVal)
			uMax = theVal;
	}

	vMin = vVals->Value(1);
	vMax = vVals->Value(1);
	for(int i=2; i<=vVals->Length(); i++)
	{
		Standard_Real theVal = vVals->Value(i);
		if(vMin > theVal)
			vMin = theVal;
		if(vMax < theVal)
			vMax = theVal;
	}

	if(AS.GetType() == GeomAbs_Cylinder)
	{
		uMin = 0;
        uMax = 2.*M_PI;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef OCC650
	TopoDS_Face F1 = BRepBuilderAPI_MakeFace(theFaceSurface, uMin, uMax, vMin, vMax);
#else
	TopoDS_Face F1 = BRepBuilderAPI_MakeFace(theFaceSurface, uMin, uMax, vMin, vMax, 1.e-7);
#endif
	// if cylinder, don't make half space but build solid!!!
	if(AS.GetType() == GeomAbs_Cylinder)
	{
		BRepBuilderAPI_Sewing tailor;
		tailor.Add(F1);

		TopExp_Explorer ex;
		for(ex.Init(F1, TopAbs_EDGE); ex.More(); ex.Next())
		{
			TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
			BRepBuilderAPI_MakeWire wireMaker(anEdge);
			TopoDS_Wire aWire = wireMaker.Wire();

			if(!aWire.Closed())
				continue;

			BRepBuilderAPI_MakeFace faceMaker(aWire, Standard_True);

			if(faceMaker.IsDone())
			{
				TopoDS_Face face = faceMaker.Face();
				tailor.Add(face);
			}
		}

		TopoDS_Shape sewedShape;
		try
		{
			tailor.Perform();
			sewedShape = tailor.SewedShape();
		}
		catch(...)
		{
			cout << "sewing failed\n";
			return aSolid;
		}

		TopoDS_Shell theShell = TopoDS::Shell(sewedShape);
		BRepBuilderAPI_MakeSolid solMaker(theShell);

		TopoDS_Solid retSol;

		if(solMaker.IsDone())
		{
			retSol = solMaker.Solid();
			return retSol;
		}
		else
			return aSolid;
	}

	if (F1.IsNull())
		cout << "_#_McCadMcVoid.cxx :: Infinite face for half space construction could not be constructed!" << endl;

	try
	{
		BRepPrimAPI_MakeHalfSpace tmpHlfSpc(F1,tempPnt);//Standard_Construction_Error occures !!!!!

		if(tmpHlfSpc.IsDone())
		{

			/*STEPControl_Writer wrt;
			wrt.Transfer(F1, STEPControl_AsIs);
			wrt.Transfer(theBox, STEPControl_AsIs);
			wrt.Transfer(BRepBuilderAPI_MakeVertex(tempPnt).Vertex(), STEPControl_AsIs);
			wrt.Write("face_and_box.stp");*/

			HSol = tmpHlfSpc.Solid();
			/*TopoDS_Shape common = BRepAlgoAPI_Common(HSol, theBox).Shape();
			return common;*/
		}
		else
		{
			cout << "Half Space construction failed\n";
			return aSolid;
		}
	}
	catch(...)
	{
		cout << "_#_McCadMcVoid.cxx :: Half space could not be constructed!!!" << endl;
	}

//MYMOD #
	return HSol;
}


TopoDS_Shape McCadMcVoid::MakeTrimmHalf(TopoDS_Shape& aSolid, TopoDS_Shape& aFace)
{
	// we make a trimmed halfspace in the size of aSolid.
	Bnd_Box theBB;
	BRepBndLib::Add(aSolid, theBB);
	theBB.SetGap(0.0);
	Standard_Real Xmin, Ymin, Zmin, Xmax, Ymax, Zmax;
	theBB.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
	gp_Pnt P1(Xmin, Ymin, Zmin);
	gp_Pnt P2(Xmax, Ymax, Zmax);

	TopoDS_Shape Box = BRepPrimAPI_MakeBox(P1,P2).Shape();
	TopoDS_Shape fShape = MakeHalfSpace(aFace);
	TopoDS_Shape aShape;
	try
	{
		BRepAlgoAPI_Cut aCommonOp(fShape,Box);

		if(aCommonOp.IsDone())
		{
			aShape = aCommonOp.Shape();
		}
		else
		{
			cout << "_#_McCadMcVoid.cxx :: Boolean Operation on a trimmed halfspace failed !!!" << endl;
		}

	}
	catch(...)
	{
		cout << "_#_McCadMcVoid.cxx :: Boolean Operation on a trimmed halfspace failed !!!" << endl;
	}

	return aShape;
}


TopAbs_State McCadMcVoid::PointState(const TopoDS_Shape& aShape,const gp_Pnt& aPnt)
{
	// This algorithm checks the state of a point relative to a basic semi-algebraic set.
	// The point should lie within the bounding box of the basic semi-algebraic set.
	// If this is not the case the method may fail due to evaluation in a connected component
	// of the set. Check first if the point is not outside of the b-Box.
	// for non basic sets use the classifier of cascade.

	TopExp_Explorer ex;
//	TopoDS_Solid aSol = TopoDS::Solid(aShape);
	bool onState=false;
	for (ex.Init(aShape, TopAbs_FACE); ex.More(); ex.Next())
	{
		TopoDS_Face theFace = TopoDS::Face(ex.Current());
		TopLoc_Location loc;
		Handle(Geom_Surface) S1 = BRep_Tool::Surface(theFace,loc);
		GeomAdaptor_Surface As1(S1);
		////////////////////////////////////////////////////////////////////////////////////
		// a plane must be correctly orriented
		if(As1.GetType() == GeomAbs_Plane)
		{
			gp_Pln aP1 = As1.Plane();
			gp_Ax3 locAxis = aP1.Position();
			gp_Dir locDir = locAxis.Direction();
			/*gp_Ax3 glAxis (locAxis.Location(),gp_Vec(fabs(locDir.X()),fabs(locDir.Y()),fabs(locDir.Z())));
			gp_Dir glDir = glAxis.Direction();*/
			Standard_Real A,B,C,D;
			if (locAxis.Direct())
			{
				A = locDir.X();
				B = locDir.Y();
				C = locDir.Z();
			}
			else
			{
				A = -locDir.X();
				B = -locDir.Y();
				C = -locDir.Z();
			}
			const gp_Pnt& P = locAxis.Location();
			D =-(A * P.X() + B * P.Y() + C * P.Z());

			if ((D > 0 && (locDir.X() < 0 || locDir.Y() < 0 || locDir.Z() < 0 )) ||
					(locDir.X() <= 0 && locDir.Y() <= 0 && locDir.Z() <= 0 ))
			{
				gp_Ax3 glAxis (locAxis.Location(),gp_Vec(-1*locDir.X(),-1*locDir.Y(),-1*locDir.Z()));
				Handle(Geom_Plane) tmpPl = new Geom_Plane(glAxis);
				As1.Load(tmpPl);

				if(theFace.Orientation() == TopAbs_FORWARD)
					theFace.Orientation(TopAbs_REVERSED);
				else if ( theFace.Orientation() == TopAbs_REVERSED)
					theFace.Orientation(TopAbs_FORWARD);
				else;
			}
		}

		Standard_Real aVal = McCadGTOOL::Evaluate(As1,aPnt);
	//	cout << aVal << endl;

		if (aVal >= -1.e-03 && aVal <= 1.e-3 )
			onState= true;
		else if(aVal > 1.e-03 && aVal < 1.e+100)
			aVal=1;
		else if (aVal < -1.e-03 && aVal > -1.e+100 )
			aVal=-1;
		else
			return TopAbs_OUT; // NULL

		if ((theFace.Orientation() == TopAbs_FORWARD) && aVal==1)
		{
			return TopAbs_OUT;
		}
		if ((theFace.Orientation() == TopAbs_REVERSED) && aVal==-1)
		{
			return TopAbs_OUT;
		}

		/*if(Abs(aVal)==1 && !onState) // double check!!!
		{
			BRepClass3d_SolidClassifier asc3d (aShape);
			asc3d.Perform(aPnt, 1.0e-7);
			if(asc3d.State() == TopAbs_OUT)
				return TopAbs_OUT;
		}*/
	}

	if (onState)
		return TopAbs_ON;
	else
	{
		return TopAbs_IN;
	}
}
