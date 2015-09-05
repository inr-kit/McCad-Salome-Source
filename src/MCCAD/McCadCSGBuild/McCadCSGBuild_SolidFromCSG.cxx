#include <McCadCSGBuild_SolidFromCSG.ixx>
#include <McCadCSGGeom_Surface.hxx>
#include <McCadCSGGeom_Plane.hxx>
#include <McCadCSGGeom_Cylinder.hxx>
#include <McCadCSGGeom_Cone.hxx>
#include <McCadCSGGeom_Sphere.hxx>
#include <McCadCSGGeom_Torus.hxx>
#include <McCadGTOOL.hxx>
#include <McCadCSGGeom_DataMapOfIntegerSurface.hxx>
#include <McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface.hxx>
#include <McCadCSGGeom_SurfaceOfRevolution.hxx>

#include <gp.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>

#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cone.hxx>
#include <gp_Torus.hxx>

#include <TColgp_HSequenceOfPnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ElementarySurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Surface.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS_Iterator.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgo_Common.hxx>
#include <BRepAlgo.hxx>
#include <BRepAlgo_BooleanOperations.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgo_Section.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
//qiu #include <BOPTools_DSFiller.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TFace.hxx>
#include <BRep_TVertex.hxx>
#include <BRep_TEdge.hxx>
#include <BRepLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <BRepCheck_Shell.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Face.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepAlgo_EdgeConnector.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

#include <BRepBndLib.hxx>
#include <BndLib_AddSurface.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>

#include <BRepMesh.hxx>

#include <BRepTools_TrsfModification.hxx>
#include <BRepTools_Modifier.hxx>

//qiu #include <BOPTools_Checker.hxx>
//qiu #include <BOPTools_Tools3D.hxx>
//qiu #include <BOPTools_SSIntersectionAttribute.hxx>
//qiu #include <BOP_Refiner.hxx>
//qiu #include <BOP_CorrectTolerances.hxx>
//qiu #include <BOPTools_DSFiller.hxx>

//qiu #include <IntTools_Context.hxx>
#include <GeomAPI_IntCS.hxx>
#include <TColGeom_HSequenceOfSurface.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Triangulation.hxx>

#include <ShapeFix_Shape.hxx>
#include <ShapeFix_Shell.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Wireframe.hxx>
#include <ShapeFix_FixSmallFace.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <BRepTools.hxx>
#include <STEPControl_Writer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <TopExp_Explorer.hxx>

#include <Geom_Ellipse.hxx>

#include <McCadMcVoid.hxx>
#include <ShapeAnalysis_Shell.hxx>

#include <McCadMessenger_Singleton.hxx>


#define MAX2(X, Y)	(  Abs(X) > Abs(Y) ? Abs(X) : Abs(Y) )
//qiu
const Standard_Boolean bRunParallel = true;
const Standard_Real  aFuzzyValue = 1.0e-5;


Standard_Real McCadCSGBuild_SolidFromCSG::VolumeOfShape(const TopoDS_Shape& theShape)
{
	GProp_GProps GP;
	BRepGProp::VolumeProperties(theShape, GP);
	return GP.Mass();
}

/*Handle(TopTools_HSequenceOfShape) LinApprox(Handle(TopTools_HSequenceOfShape) failedHSolSeq, const TopoDS_Solid& theSolid)
{
	////////////////////////////////////////////////////////////////////
	Handle(TopTools_HSequenceOfShape) linHSpaces = new TopTools_HSequenceOfShape();
	Handle(TopTools_HSequenceOfShape) HFaces = new TopTools_HSequenceOfShape();
	Bnd_Box2d BB2;
	Bnd_Box BB3;
	Standard_Real UMin,UMax,VMin,VMax;
	Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
	for (TopExp_Explorer ex(theSolid,TopAbs_FACE); ex.More(); ex.Next())
	{
		BRepBndLib::AddClose(ex.Current(),BB3);
		// BB3.Add(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));
		// BB3.SetGap(0.0);
	}
	BB3.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
	TopoDS_Shape locBox = BRepPrimAPI_MakeBox(gp_Pnt(aXmin, aYmin, aZmin), gp_Pnt(aXmax, aYmax, aZmax)).Shape();
	BB2.Add(gp_Pnt2d(aXmin, aYmin));
	BB2.Add(gp_Pnt2d(aXmin, aZmin));
	BB2.Add(gp_Pnt2d(aYmin, aZmin));
	BB2.Add(gp_Pnt2d(aXmax, aYmax));
	BB2.Add(gp_Pnt2d(aXmax, aZmax));
	BB2.Add(gp_Pnt2d(aYmax, aZmax));
	BB2.SetGap(0.0);
	BB2.Get(UMin,VMin,UMax,VMax);
	int iz=0;

	Standard_Real tolDegen(1.e-7);

	for (int k =1; k<= failedHSolSeq->Length(); k++)
	{
		for (TopExp_Explorer ex(failedHSolSeq->Value(k),TopAbs_FACE); ex.More(); ex.Next())
		{
			TopLoc_Location L;
			const TopoDS_Face& iFace = TopoDS::Face(ex.Current());
			const Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*)&iFace.TShape());
			TF->Tolerance(1.e-03);
			TopAbs::Print(iFace.Orientation(),cout); cout << endl;
			////////////////////////////////////////////////////////////////////////////////////////
#ifdef OCC650
			TopoDS_Face fF = BRepBuilderAPI_MakeFace(BRep_Tool::Surface(iFace,L),UMin,UMax,VMin,VMax);
#else
			TopoDS_Face fF = BRepBuilderAPI_MakeFace(BRep_Tool::Surface(iFace,L),UMin,UMax,VMin,VMax, tolDegen);
#endif
			fF.Orientation(iFace.Orientation());
			BRepTools::Update(fF);
			BRepTools::UVBounds(fF,UMin,UMax,VMin,VMax);
			cout << "Face Bounds  for        " << ++iz << endl;
			cout << UMin << " " << UMax << " " << VMin << " " << VMax << endl;
			////////////////////////////////////////////////////////////////////////////////
			BRep_Builder B;
			TopoDS_Shell Sh;
			B.MakeShell(Sh);
			TopoDS_Solid Solid;
			B.Add(Sh,fF);
			if (!Sh.Free ()) Sh.Free(Standard_True);
			B.MakeSolid(Solid);
			B.Add(Solid,Sh);

			//////////////////////////// Triangulation /////////////////////////////////////
			for (TopExp_Explorer exF(Solid,TopAbs_FACE); exF.More(); exF.Next())
			{
				TopLoc_Location loc;
				TopoDS_Face lF = TopoDS::Face(exF.Current());
				BRepTools::Update(lF);
				BRepTools::UVBounds(lF,UMin,UMax,VMin,VMax);
				cout << "Face Bounds  for        " << ++iz << endl;
				cout << UMin << " " << UMax << " " << VMin << " " << VMax << endl;

				Handle(Geom_Surface) theSurf = BRep_Tool::Surface(lF,loc);
				Handle(Poly_Triangulation) mesh;
				BRepAdaptor_Surface BS(lF,Standard_True);
				gp_Trsf T = BS.Trsf();

				Standard_Real aDeflection = MAX2( fabs(UMax)-fabs(UMin), fabs(VMax)-fabs(VMin))/4.0;
				BRepMesh::Mesh(lF, aDeflection);
				mesh = BRep_Tool::Triangulation(lF,loc);

				if (mesh.IsNull()) cout << "Face triangulation failed !!" << endl;
				else
				{
					Standard_Integer nNodes = mesh->NbNodes();
					TColgp_Array1OfPnt meshPnts(1,nNodes);
					meshPnts = mesh->Nodes();
					Standard_Integer nbTriangles = mesh->NbTriangles();
					cout << " Number of Nodes  = " << nNodes << " Number of Triangles  = " << nbTriangles << endl;
					Standard_Integer n1, n2, n3;
					const Poly_Array1OfTriangle& Triangles = mesh->Triangles();
					BRep_Builder fB;
					TopoDS_Shell fSh;
					fB.MakeShell(fSh);
					TopoDS_Solid fSolid;
					int messer;
					if (nbTriangles >4) messer = int(nbTriangles/4);
					else messer = 1;
					cout << messer << endl;
					for (int i = 1; i <= nbTriangles; i=i+messer)
					{
						cout << i << endl;
						Triangles(i).Get(n1, n2, n3);
						gp_Pnt P1 = (meshPnts(n1)).Transformed(T);
						gp_Pnt P2 = (meshPnts(n2)).Transformed(T);
						gp_Pnt P3 = (meshPnts(n3)).Transformed(T);
						gp_Vec v1(P1, P2), v2(P1, P3);
						gp_Vec v3 = v1 ^ v2;
						v3.Normalize();
						gp_Dir D1(v3);
						gp_Pln Plane1(P1,D1);
						TopoDS_Face F1 = BRepBuilderAPI_MakeFace(Plane1);
						F1.Orientation(iFace.Orientation());
						fB.Add(fSh,F1);
					}
					fSh.Orientation(iFace.Orientation());
					if (!fSh.Free ()) fSh.Free(Standard_True);
					fB.MakeSolid(fSolid);
					fB.Add(fSolid,fSh);
					linHSpaces->Append(fSolid);

				}
			}
		}
	}
	return linHSpaces;
}*/


TopoDS_Solid RepairOpenShell(TopoDS_Shape& theSolid)
{
	//  BRepBuilderAPI_Sewing Sew(1.e-04, Standard_False, Standard_False);
	BRepBuilderAPI_Sewing Sew(1.e-04);
	TopoDS_Solid sSolid;

	/*Sew.Load(sSolid);

	if (Sew.IsDegenerated(theSolid))
		cout << "The Shape is degenerated !!" << endl;*/

	for (TopExp_Explorer aExp(theSolid, TopAbs_FACE); aExp.More(); aExp.Next())
		Sew.Add(TopoDS::Face(aExp.Current()));
	Sew.Perform();
	//
	// Extract free edges.
	//
	//  Sew.Dump();

	TopoDS_Shape sShape = Sew.SewedShape();

	//TopAbs::Print(sShape.ShapeType(), cout);
	//cout << endl;

	Standard_Integer nbedge = Sew.NbFreeEdges();
	cout << "Number of free edge found = "<< nbedge << endl;
	TopTools_ListOfShape freeEdges;
	Handle(TopTools_HSequenceOfShape) aFaceSeq = new TopTools_HSequenceOfShape;
	TopoDS_Edge anEdge;
	// we need the following for orientaion calculation!!
	////////////////////////////////////////////////////////////////////////////////
	//  TopoDS_Edge aBoundedEdge = TopoDS::Edge(Sew.MultipleEdge(nbMultEdge));  // ContigousEdge(nbContEdge));
	Handle(TColgp_HSequenceOfPnt) aPntSeq = new TColgp_HSequenceOfPnt;

	//////////////////////////////////////////////////////////////////////////////

	BRepBuilderAPI_MakeWire wireMaker;

	for (Standard_Integer iedge = 1; iedge <= nbedge; iedge++)
	{
		anEdge = TopoDS::Edge(Sew.FreeEdge(iedge));
		wireMaker.Add(anEdge);
	}

	TopoDS_Wire theWire = wireMaker.Wire();
	BRepBuilderAPI_MakeFace faceMaker(theWire);

	if(!faceMaker.IsDone())
	{
		cout << "FACE CONSTRUCTION FAILED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
	}

	TopoDS_Face theFace = faceMaker.Face();

	Sew.Add(theFace);
	Sew.Perform();
	TopoDS_Shape sewed = Sew.SewedShape();

	cout << "make shell\n";
	TopoDS_Shell aShell = TopoDS::Shell(sewed);
	cout << ".. done\n";
	cout << "make solid\n";
	BRepBuilderAPI_MakeSolid solidMaker(aShell);
	TopoDS_Solid finalSolid = solidMaker.Solid();
	cout << "... done\n";

	return finalSolid;

	/*BRepBuilderAPI_Sewing sewTool;
	sewTool.Add(theSolid);
	sewTool.Add(theFace);
	sewTool.Perform();
	TopoDS_Shape sewed = sewTool.SewedShape();*/
	/*STEPControl_Writer stpWrtr;
	stpWrtr.Transfer(sewed,STEPControl_AsIs);
	stpWrtr.Write("finalSolid.stp");

	Handle(ShapeFix_Shape) genericFix = new ShapeFix_Shape;
	genericFix->Init(sewed);
	genericFix->SetPrecision (1.e-03);
	genericFix->SetMaxTolerance(0.0);
	genericFix->FixSolidTool()->CreateOpenSolidMode() = Standard_True;
	genericFix->Perform();
	sewed = genericFix->Shape();*/




/*	BRepAlgoAPI_Fuse fuser(theSolid, theFace);

	if(!fuser.IsDone())
		cout << "Solid Maker failed\n\n\n";

	TopoDS_Shape returnSolid = fuser.Shape();*/



	/*for (Standard_Integer iedge = 1; iedge <= nbedge; iedge++)
	{
		anEdge = TopoDS::Edge(Sew.FreeEdge(iedge));
		if (!BRep_Tool::Degenerated(anEdge) )
		{
			freeEdges.Append(anEdge);
			for (TopExp_Explorer aExv(anEdge, TopAbs_VERTEX); aExv.More(); aExv.Next())
			{
				gp_Pnt theP = BRep_Tool::Pnt(TopoDS::Vertex(aExv.Current()));
				aPntSeq->Append(theP);
			}
		}
	}

	//cout << "Point found!! " << endl;

	Handle(BRepAlgo_EdgeConnector) edgeCon = new BRepAlgo_EdgeConnector;
	TopTools_ListOfShape wires;
	edgeCon->AddStart(freeEdges);
	wires = edgeCon->MakeBlock();

	TopTools_ListIteratorOfListOfShape it(wires);
	if (edgeCon->IsDone())
	{
		cout << "CONNECTED EDGES : " << endl;
		Standard_Integer p(0);
		for (; it.More(); it.Next())
		{
			const TopoDS_Shape& aWire = it.Value();
			TopAbs_Orientation orient = aWire.Orientation();


                        BRepBuilderAPI_FindPlane pFind(aWire, 1.e-02);
			Handle(Geom_Plane) thePlane;

			TopoDS_Face aFace;
			if (pFind.Found())
			{
				thePlane = pFind.Plane();
				gp_Pln aPln = thePlane->Pln();

				GeomAdaptor_Surface theASurf(thePlane);
				////////////////////////////////////////////////////////////////////////
				// calculate the orietation of the face
				cout << " Computing orientation ............. " << endl;
				for (int j =1; j<=aPntSeq->Length(); j++)
				{
					Standard_Real aVal = McCadGTOOL::Evaluate(theASurf,
							aPntSeq->Value(j));
					if (aVal > 1.e-04 || aVal < -1.e-04)
					{
						if (aVal > 1.e-04)
							orient = TopAbs_REVERSED;
						if (aVal < 1.e-04)
							orient = TopAbs_FORWARD;
						cout << " Computing orientation successful!! " << endl;
						break;
					}
				}
				if (orient == TopAbs_FORWARD)
					cout << " Computed orientation is TopAbs_FORWARD!! "
							<< endl;
				else if (orient == TopAbs_REVERSED)
					cout << "Computed orientation is  TopAbs_REVERSED!! "
							<< endl;
				else
					cout << " Computed orientation is TopAbs_Unknown!! "
							<< endl;

				///////////////////////////////////////////////////////////////////////
				aFace = BRepBuilderAPI_MakeFace(aPln, TopoDS::Wire(aWire));
				aFace.Orientation(orient);
				aFaceSeq->Append(aFace);
			}
			else
			{
				cout << "_#_McCadCSGBuild_SolidFromCSG.cxx :: Can not find a plane for a wire. " << endl;
				continue;
			}
		}
	}
	else
	{
		cout << "_#_McCadCSGBuild_SolidFromCSG.cxx :: Can not make wires " << endl;
		// return theSolid;
	}*/

	//cout << "Number of new faces =  " << aFaceSeq->Length() << endl;

	/*
	 TopoDS_Face  F;

	 for(TopExp_Explorer aExp(theSolid,TopAbs_FACE); aExp.More(); aExp.Next())
	 {
	 F = TopoDS::Face(aExp.Current());
	 aFaceSeq->Append(F);
	 }

	 BRepBuilderAPI_Sewing Sew1(1.e-04);
	 TopoDS_Solid S;
	 Sew1.Load(S);
	 for(int i=1; i<= aFaceSeq->Length(); i++) Sew1.Add(TopoDS::Face(aFaceSeq->Value(i)));
	 Sew1.Perform();
	 TopoDS_Shape theShape = Sew1.SewedShape();
	 if(theShape.IsNull())
	 {
	 cout << "Warning: Repair of solid failed!! " << endl;
	 return theSolid;
	 }

	 cout << " Repair of open shell successful !! " << endl ;
	 cout << " Model may be approximated  !! " << endl;

	 return TopoDS::Solid(theShape);
	 }
	 */

	/*TopoDS_Shell sh;
	TopoDS_Face F;
	BRep_Builder B;
	B.MakeShell(sh);

	for (TopExp_Explorer aExp(sShape, TopAbs_FACE); aExp.More(); aExp.Next())
	{
		F = TopoDS::Face(aExp.Current());
		Handle(ShapeFix_Face) faceFix = new ShapeFix_Face;
		faceFix->Init(F);
		faceFix->SetPrecision(1.e-03);
		faceFix->SetMaxTolerance(1.e-03);
		faceFix->Perform();
		F = TopoDS::Face(faceFix->Result());
		B.Add(sh, F);
	}
	if (aFaceSeq->Length() > 0)
	{
		for (int i=1; i<= aFaceSeq->Length(); i++)
		{
			TopoDS_Face theFace = TopoDS::Face(aFaceSeq->Value(i));
			const Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*)&theFace.TShape());
			TF->Tolerance(1.e-03);
			B.Add(sh, theFace);
		}
	}
	if (!sh.Free())
		sh.Free(Standard_True);
	TopoDS_Solid solid;
	B.MakeSolid(solid);
	B.Add(solid, sh);
	try
	{
		BRepClass3d_SolidClassifier bsc3d (solid);
		Standard_Real t = Precision::Confusion();
		bsc3d.PerformInfinitePoint(t);

		if (bsc3d.State() == TopAbs_IN)
		{
			cout << "_#_McCadCSGBuild_SolidFromCSG.cxx :: Inifinity in Solid !! " << endl;
			if (!sh.Free ()) sh.Free(Standard_True);
			TopoDS_Solid soli2;
			B.MakeSolid (soli2);
			sh.Reverse();
			B.Add (soli2,sh);
			solid = soli2;
		}
	}
	catch(Standard_Failure)
	{
		cout << "_#_McCadCSGBuild_SolidFromCSG.cxx :: Warning: SolidFromShell: Exception: " << endl;
	}

	cout << " Repair of open shell may be successful !! " << endl;
	cout << " Model may be approximated  !! " << endl;

	return solid;*/
}

static gp_Pnt ApproxBaryCenter(const TopoDS_Shape& S)
{
	Standard_Integer i;
	TopExp_Explorer ex;
	gp_XYZ xyz(0, 0, 0);
	for (ex.Init(S, TopAbs_VERTEX), i = 1; ex.More(); ex.Next(), i++)
		xyz += BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())).XYZ();
	xyz /= i;
	return gp_Pnt(xyz);
}

TopoDS_Shape Scale(const TopoDS_Shape& S, Standard_Real theFact)
{
	gp_Trsf T;
	gp_Pnt thePnt = ApproxBaryCenter(S);
	T.SetScale(thePnt, theFact); // we scale up or down relative to barycenter depending on theFact;
	Handle(BRepTools_TrsfModification) TrsfMod = new BRepTools_TrsfModification(T);
	BRepTools_Modifier Md;
	Md.Init(S);
	Md.Perform(TrsfMod);
	if (Md.IsDone())
	{
		return Md.ModifiedShape(S);
	}
	else
	{
		cout << "_#_McCadCSGBuild_SolidFromCSG.cxx :: Can not scale/modify shape !! " << endl;
		return S;
	}
}

Handle(TopTools_HSequenceOfShape) SortByRadius(Handle(TopTools_HSequenceOfShape) HSolSeq)
{
	Standard_Real currentRad = 0, oldRad = 0;
	if (HSolSeq->IsEmpty()) return HSolSeq;
	if (HSolSeq->Length() <= 1) return HSolSeq;

	for (int i=1; i<= HSolSeq->Length(); i++)
	{
		TopExp_Explorer exp(HSolSeq->Value(i),TopAbs_FACE);
		TopLoc_Location l;
		Handle(Geom_Surface) S;
		TopoDS_Face F;
		for (; exp.More(); exp.Next()) F = TopoDS::Face(exp.Current()); // we have only one face since this is a halfspace.
		TopAbs_Orientation orient = F.Orientation();
		if (orient == TopAbs_FORWARD);
			//cout << " TopAbs_FORWARD at Sorting !!!" << endl;
		else if (orient == TopAbs_REVERSED);
			//cout << " TopAbs_REVERSED at Sorting !!! " << endl;
		else
			cout << " TopAbs_Unknown at Sorting !!! " << endl;
		S = BRep_Tool::Surface(F, l);
		GeomAdaptor_Surface AS(S);
		switch (AS.GetType())
		{
			case GeomAbs_Cylinder:
			{
				gp_Cylinder C = AS.Cylinder();
				currentRad = C.Radius();
				break;
			}
			case GeomAbs_Cone:
			{
				gp_Cone C = AS.Cone();
				currentRad = C.RefRadius() / tan(C.SemiAngle()); // heuristic
				break;
			}
			case GeomAbs_Sphere:
			{
				gp_Sphere C = AS.Sphere();
				currentRad = C.Radius();
				break;
			}
			case GeomAbs_Torus:
			{
				gp_Torus C = AS.Torus();
				// currentRad = C.MajorRadius();
				currentRad = C.MinorRadius();
				break;
			}
			default: break;
		}
		if (i==1)
		{
			oldRad = currentRad;
			//cout << "Current Radius = " << currentRad << endl;
			continue;
		}
		else
		{
			//cout << "Current Radius = " << currentRad << endl;
			for (int k = i - 1; k>=1; k--)
			{
				if(currentRad < oldRad)
				{
					//cout << "Exchanging ................" << endl;
					HSolSeq->Exchange(k,k+1);
				}
				else
				break;
			}
		}
		oldRad = currentRad;
	}

	//cout << "Radius based sorting performed !! " << endl;

	return HSolSeq;
}


McCadCSGBuild_SolidFromCSG::McCadCSGBuild_SolidFromCSG()
{
}

McCadCSGBuild_SolidFromCSG::McCadCSGBuild_SolidFromCSG(const Handle(McCadCSGGeom_Cell)& theCell)
{
	Init(theCell);
}


void McCadCSGBuild_SolidFromCSG::Init(const Handle(McCadCSGGeom_Cell)& theCell)
{
	NotDone();
	myCell = theCell;
	myType = McCadCSGBuild_Other;
	mySolids = new TopTools_HSequenceOfShape();
	MakeSolids();
	Done();
}


Standard_Boolean McCadCSGBuild_SolidFromCSG::IsDone() const
{
	return myIsDone;
}


void McCadCSGBuild_SolidFromCSG::Done()
{
	myIsDone = Standard_True;
}


void McCadCSGBuild_SolidFromCSG::NotDone()
{
	myIsDone = Standard_False;
}


void McCadCSGBuild_SolidFromCSG::MakeSolids()
{
        McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
	// currently the csg exp is supposed to be in disjunctive normal form.
	Handle(TColStd_HSequenceOfAsciiString) csgExp = myCell->GetCSG();
	McCadCSGGeom_DataMapOfIntegerSurface surfMap;

	McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface surfIter = myCell->GetSurface();

	for (; surfIter.More(); surfIter.Next())//for all surface numbers fill surface map
		surfMap.Bind(surfIter.Key(), surfIter.Value());

	if (csgExp->Length() == 0 || surfMap.Extent() == 0)
	{
                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Empty CSG Expression or number of surfaces is zero !! ",
                              McCadMessenger_ErrorMsg);
		return;
	}

	for(Standard_Integer pl=1; pl<=csgExp->Length(); pl++)
		cout << csgExp->Value(pl).ToCString() << " ";
	cout << endl;

	cout << "Cell processing started  "<< endl;
	cout << "======================== "<< endl;

	for (int i=1; i<= csgExp->Length(); i++) // For All CSG-Terms build solids
	{
		Handle(TopTools_HSequenceOfShape) HSolSeq = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) BoxSeq = new TopTools_HSequenceOfShape();
		Handle(TColGeom_HSequenceOfSurface) geomSurfSeq = new TColGeom_HSequenceOfSurface();

		while (csgExp->Value(i) != TCollection_AsciiString(" ) ")) //half space computation for one solid
		{
			if ((csgExp->Value(i)).IsIntegerValue()) // if surface number in csg expression
			{
		//## build TopoDS_Face
				Standard_Integer surfNo = (csgExp->Value(i)).IntegerValue();
				Standard_Integer key = abs(surfNo);
				Handle(McCadCSGGeom_Surface) aCSGSurf = surfMap(key);
				Handle(Geom_Surface) theSurf = aCSGSurf->CasSurf();
				geomSurfSeq->Append(theSurf);
				GeomAdaptor_Surface theAdaptSurf(theSurf);
Standard_Real tolDegen(1.e-07);
#ifdef OCC650
				TopoDS_Face theFace = BRepBuilderAPI_MakeFace(theSurf);
#else
				TopoDS_Face theFace = BRepBuilderAPI_MakeFace(theSurf, tolDegen);
#endif
			//## find point on solid side of the face
				gp_Ax3 pos;

				if(! theAdaptSurf.GetType() == GeomAbs_SurfaceOfRevolution && ! theAdaptSurf.GetType()==GeomAbs_BSplineSurface)
				{
					Handle(Geom_ElementarySurface) ES = Handle(Geom_ElementarySurface)::DownCast(theSurf);
					pos = ES->Position();
				}
				else
				{
					pos = aCSGSurf->GetAxis();
				}

//				gp_Ax3 stdAx3(gp::Origin(), gp::DZ());
//				gp_Dir theDirect = pos.Direction();
				gp_Trsf T;
				T.SetTransformation(pos);

				// TopAbs_Orientation orient;
				Standard_Real U=1.0, V=1.0;
				gp_Pnt thePnt = theSurf->Value(U, V);

				Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt;

				for (int j=1; j <=3; j++)
				{
					// perturbation coordinatewise
					Standard_Real r = thePnt.Coord(j);
					gp_Pnt tmpPnt = thePnt;
					tmpPnt.SetCoord(j, r+0.1);
					thePntSeq->Append(tmpPnt);
					tmpPnt.SetCoord(j, r-0.1);
					thePntSeq->Append(tmpPnt);
				}

				gp_Pnt samPnt(0,0,0);
				Handle(McCadCSGGeom_SurfaceOfRevolution) surfOfRev = NULL;
				Handle(TColStd_HSequenceOfReal) surfPar = NULL;
				if(theAdaptSurf.GetType()==GeomAbs_SurfaceOfRevolution) // recalculate aVal
				{
                                        surfOfRev = Handle(McCadCSGGeom_SurfaceOfRevolution)::DownCast( aCSGSurf );
					surfOfRev->GetParameter(surfPar);
				}

				if (surfNo < 0)//Forward oriented surface
				{
					for (int j=1; j<=thePntSeq->Length(); j++)
					{
						Standard_Real aVal = McCadGTOOL::Evaluate(theAdaptSurf, thePntSeq->Value(j));

						if(theAdaptSurf.GetType()==GeomAbs_SurfaceOfRevolution) // recalculate aVal
						{
							gp_Dir theDir = surfOfRev->GetAxis().Direction();
							gp_Dir xDir(1,0,0);
							gp_Dir yDir(0,1,0);
							gp_Dir zDir(0,0,1);

							Standard_Real Xb(0),Yb(0),Zb(0), Q(0), X(0), Y(0), Z(0),
										  A(surfPar->Value(4)),
										  B(surfPar->Value(5)),
										  C(surfPar->Value(6));

							if(theDir.IsEqual(xDir, 1e-3))
							{
								Xb = surfPar->Value(1);
								Yb = surfPar->Value(2);
								Zb = surfPar->Value(3);
								X = thePntSeq->Value(j).X();
								Y = thePntSeq->Value(j).Y();
								Z = thePntSeq->Value(j).Z();
							}
							if(theDir.IsEqual(yDir, 1e-3))
							{
								Xb = surfPar->Value(2);
								Yb = surfPar->Value(1);
								Zb = surfPar->Value(3);
								X = thePntSeq->Value(j).Y();
								Y = thePntSeq->Value(j).X();
								Z = thePntSeq->Value(j).Z();
							}
							if(theDir.IsEqual(zDir, 1e-3))
							{
								Xb = surfPar->Value(3);
								Yb = surfPar->Value(2);
								Zb = surfPar->Value(1);
								X = thePntSeq->Value(j).Z();
								Y = thePntSeq->Value(j).Y();
								Z = thePntSeq->Value(j).X();
							}

							Q = Sqrt((Y-Yb) * (Y-Yb) + (Z-Zb) * (Z-Zb)) - A;

							aVal = (X-Xb) * (X-Xb) / (B * B) + Q * Q / (C * C) - 1;
						}

						if (aVal < -1.e-04)
						{
							samPnt = thePntSeq->Value(j);
							break;
						}
					}
				}
				else//reverse oriented surface
				{
					for (int j=1; j<=thePntSeq->Length(); j++)
					{
						Standard_Real aVal = McCadGTOOL::Evaluate(theAdaptSurf, thePntSeq->Value(j));

						if(theAdaptSurf.GetType()==GeomAbs_SurfaceOfRevolution) // recalculate aVal
						{
							gp_Dir theDir = surfOfRev->GetAxis().Direction();
							gp_Dir xDir(1,0,0);
							gp_Dir yDir(0,1,0);
							gp_Dir zDir(0,0,1);

							Standard_Real Xb(0),Yb(0),Zb(0), Q(0), X(0), Y(0), Z(0),
										  A(surfPar->Value(4)),
										  B(surfPar->Value(5)),
										  C(surfPar->Value(6));

							if(theDir.IsEqual(xDir, 1e-3))
							{
								Xb = surfPar->Value(1);
								Yb = surfPar->Value(2);
								Zb = surfPar->Value(3);
								X = thePntSeq->Value(j).X();
								Y = thePntSeq->Value(j).Y();
								Z = thePntSeq->Value(j).Z();
							}
							if(theDir.IsEqual(yDir, 1e-3))
							{
								Xb = surfPar->Value(2);
								Yb = surfPar->Value(1);
								Zb = surfPar->Value(3);
								X = thePntSeq->Value(j).Y();
								Y = thePntSeq->Value(j).X();
								Z = thePntSeq->Value(j).Z();
							}
							if(theDir.IsEqual(zDir, 1e-3))
							{
								Xb = surfPar->Value(3);
								Yb = surfPar->Value(2);
								Zb = surfPar->Value(1);
								X = thePntSeq->Value(j).Z();
								Y = thePntSeq->Value(j).Y();
								Z = thePntSeq->Value(j).X();
							}

							Q = Sqrt((Y-Yb) * (Y-Yb) + (Z-Zb) * (Z-Zb)) - A;

							aVal = (X-Xb) * (X-Xb) / (B * B) + Q * Q / (C * C) - 1;

						}

						if (aVal > 1.e-04)
						{
							samPnt = thePntSeq->Value(j);
							break;
						}
					}
					//}
				}

			//## build half space
				TopoDS_Shape HSol = BRepPrimAPI_MakeHalfSpace(theFace,samPnt).Solid();

				if (HSol.IsNull())
				{
                                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: A NULL half space: maybe wrong orientation !?!",
                                                      McCadMessenger_ErrorMsg);
					return;
				}
				else
				{
					HSolSeq->Append(HSol);
				}
			}
			i++;
		}

		//## sort half spaces by type
		if (HSolSeq->Length() == 0)
		{
                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Halfspace solid computation failed !! ",
                                      McCadMessenger_ErrorMsg);
			return;
		}
		///////////////////////////////////sort halfspaces////////////////////////////////////////////////
		//      TopTools_ListOfShape LPlan,LCyl,LCon,LSphere,LTor;
		Handle(TopTools_HSequenceOfShape) LPlan = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) LCyl = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) LCon = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) LSphere = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) LTor = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) Other = new TopTools_HSequenceOfShape();

		for (int j = 1; j<= HSolSeq->Length(); j++) //For all Half Spaces: collect by type
		{
			TopExp_Explorer exp(HSolSeq->Value(j), TopAbs_FACE);
			TopLoc_Location l;
			Handle(Geom_Surface) S;
			TopoDS_Face F;
			for (; exp.More(); exp.Next())
				F = TopoDS::Face(exp.Current()); // we have only one face since this is a halfspace.

			S = BRep_Tool::Surface(F, l);
			if (!S.IsNull())
			{
				GeomAdaptor_Surface AS(S);
				switch (AS.GetType())
				{
					case GeomAbs_Plane:
					{
						//cout << "plane\n";
						LPlan->Append(HSolSeq->Value(j));
						break;
					}
					case GeomAbs_Cylinder:
					{
						//cout << "cylinder\n";
						LCyl->Append(HSolSeq->Value(j));
						break;
					}
					case GeomAbs_Cone:
					{
						//cout << "cone\n";
						LCon->Append(HSolSeq->Value(j));
						break;
					}
					case GeomAbs_Sphere:
					{
						//cout << "sphere\n";
						LSphere->Append(HSolSeq->Value(j));
						break;
					}
					case GeomAbs_Torus:
					{
						//cout << "torus\n";
						LTor->Append(HSolSeq->Value(j));
						break;
					}
					case GeomAbs_SurfaceOfRevolution:
					{
						//cout << "other\n";
						Other->Append(HSolSeq->Value(j));
						break;
					}
					default: break;
				}
			}
			else
			{
                                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Nonanalytic surface !!! ",
                                              McCadMessenger_WarningMsg);
			}
		}

		LTor = SortByRadius(LTor);
		LSphere= SortByRadius(LSphere);
		LCon =SortByRadius(LCon);
		LCyl = SortByRadius(LCyl);
		HSolSeq->Clear();

		HSolSeq->Append(LTor);
		HSolSeq->Append(LSphere);
		HSolSeq->Append(LCyl);
		HSolSeq->Append(LCon);
		HSolSeq->Append(Other);

		Handle(TopTools_HSequenceOfShape) linHSolSeq = new TopTools_HSequenceOfShape();
		linHSolSeq->Append(LPlan);

		LPlan->Clear();
		LTor->Clear();
		LSphere->Clear();
		LCon->Clear();
		LCyl->Clear();
		Other->Clear();
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//qiu		IntTools_Context ctx;
		TopoDS_Shape tmpS1, tmpS2;
		TopoDS_Solid tmpSol, newSol, oldSol1;
		TopoDS_Shape BigShape;

	//## compute box for common operations
		BigShape = BRepPrimAPI_MakeBox(gp_Pnt(-1.e5,-1.e5,-1.e5),gp_Pnt(1.e5,1.e5,1.e5)).Shape();
		TopoDS_Solid Big = TopoDS::Solid(BigShape);
		//TopoDS_Solid testBIG = Big;


		TopoDS_Solid polSol1;
		///////////////////////////////////////////////////////////////////////////////////////////////////////////

		Handle(TopTools_HSequenceOfShape) polyHSolids = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) failedHSolSeq = new TopTools_HSequenceOfShape();

	//## processing linear half-spaces first
		if (linHSolSeq->Length() > 0) //only linear half spaces are treated
		{
			cout << "processing linear halfspaces : " << linHSolSeq->Length() << endl;
			for (int k=1; k<=linHSolSeq->Length() ; k++) // make huge finit solids which represent the half spaces
			{
				Standard_Boolean bOp= Standard_False;
				tmpS1 = TopoDS::Solid(linHSolSeq->Value(k));

				try
				{
//qiu 					BOPTools_DSFiller pDF;
//qiu                     BOPTools_DSFiller pDF;
//qiu                             pDF.SetShapes(tmpS1, Big);
//qiu 					pDF.Perform();

//qiu					BRepAlgoAPI_Common aCommonOp(tmpS1,Big, pDF);
                    BRepAlgoAPI_Common aCommonOp(tmpS1,Big);

					if(aCommonOp.IsDone())
					{
						bOp= Standard_True;
						tmpS2 = aCommonOp.Shape();
					}
					else
					{
						bOp= Standard_False;
                                                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Boolean Operation on a halfspace failed !!! 2",
                                                              McCadMessenger_WarningMsg);
					}
				}
				catch(...)
				{
					bOp= Standard_False;
                                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Boolean Operation on a halfspace failed !!! 3",
                                                      McCadMessenger_WarningMsg);
				}

				if (bOp)
				{
					polyHSolids->Append(tmpS2);
				}
				else
				{
                                    TCollection_AsciiString msg;
                                        msg += k;
                                        msg += "_#_McCadCSGBuild_SolidFromCSG.cxx ::  Linear halfspace appended to fail sequence!! ";
                                        msgr->Message(msg.ToCString(), McCadMessenger_WarningMsg);
					failedHSolSeq->Append(linHSolSeq->Value(k));
				}
			}


			cout << "Number of linear boxed halfspaces =   "<< polyHSolids->Length() << endl;
			if (polyHSolids->Length() == 1)
			{
				for ( TopExp_Explorer Ex(polyHSolids->Value(1), TopAbs_SOLID); Ex.More(); Ex.Next() )
				{
					polSol1 = TopoDS::Solid( Ex.Current());
				}
			}
			else
			{
		//## perform common operation on generated boxed half spaces (LINEAR)
				for ( TopExp_Explorer Ex(polyHSolids->Value(1), TopAbs_SOLID); Ex.More(); Ex.Next() )
				{
					polSol1 = TopoDS::Solid( Ex.Current());
				}

				for (int k=2; k<=polyHSolids->Length() ; k++)
				{
					/////////////////////////////////////////////////////////////////////////////////////
					int itol = 0;
					Standard_Boolean bOp= Standard_False;

					//oldSol1 = TopoDS::Solid(polyHSolids->Value(k));

					for ( TopExp_Explorer Ex(polyHSolids->Value(k), TopAbs_SOLID); Ex.More(); Ex.Next() )
					{
						oldSol1 = TopoDS::Solid( Ex.Current());
					}

					Standard_Boolean need2repeat = Standard_True;
					while(need2repeat)
					{
						try
						{
//qiu							BOPTools_DSFiller pDF;
//qiu							pDF.SetShapes(oldSol1, polSol1);
//qiu							pDF.Perform();

							BRepAlgoAPI_Common aCommonOp(oldSol1,polSol1);

							if(aCommonOp.IsDone())
							{
								bOp= Standard_True;
								tmpS1 = aCommonOp.Shape();
							}
							else
							{
								bOp= Standard_False;
                                                                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Boolean Operation on a halfspace failed !!! 4",
                                                                              McCadMessenger_WarningMsg);
							}

						}
						catch(...)
						{
							bOp= Standard_False;
                                                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Boolean Operation on a halfspace failed !!! 5",
                                                                      McCadMessenger_WarningMsg);
						}

						const TopoDS_Shape& aShape = tmpS1;

					//Test if shape is valid
						BRepCheck_Analyzer BA(aShape, Standard_False);

						int sCount=0;
						for (TopExp_Explorer ex(aShape, TopAbs_SOLID); ex.More(); ex.Next())
						{
							sCount++;
							const TopoDS_Solid& solid = TopoDS::Solid(ex.Current());
							if (solid.IsNull())
							{
                                                                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: BOP for solid failed: NULL SOLID computed !!",
                                                                              McCadMessenger_WarningMsg);
								bOp = Standard_False;
								break;
							}
							for (TopExp_Explorer exs(solid, TopAbs_SHELL); exs.More(); exs.Next())
							{
								const TopoDS_Shell& sh = TopoDS::Shell(exs.Current());
								BA.Init(sh, Standard_False);
								Handle(BRepCheck_Result) RES = BA.Result(sh);
								BRepCheck_ListOfStatus StatusList;
								StatusList = RES->Status();
								BRepCheck_ListIteratorOfListOfStatus iter;

								for (iter.Initialize(StatusList); iter.More(); iter.Next())
								{
									//BRepCheck::Print(iter.Value(), cout);
									if (iter.Value() == BRepCheck_UnorientableShape)
									{
										Handle(ShapeFix_Shell) genericFix = new ShapeFix_Shell(sh);
										genericFix->SetPrecision(1.e-03);
										genericFix->SetMaxTolerance(0.0);
										genericFix->Perform();
										BA.Init(sh, Standard_False);
									}
								}

								if (!BA.IsValid())
								{
                                                                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: BRepCheck_Analyzer for shell failed !!",
                                                                                      McCadMessenger_WarningMsg);
									bOp = Standard_False;
									break;
								}
							}
						}
						BA.Init(aShape, Standard_False);
						if (!BA.IsValid())
						{
                                                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: BRepCheck_Analyzer for solid failed !! - 0",
                                                                      McCadMessenger_WarningMsg);
							bOp = Standard_False;
						}
						if (sCount==0)
						{
							bOp = Standard_False; // if check is run on compound containing no solid
                                                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Compound containing no solid computed !",
                                                                      McCadMessenger_WarningMsg);
						}

						if (tmpS1.IsNull())
						{
							bOp = Standard_False;
                                                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Null solid computed !!! ",
                                                                      McCadMessenger_WarningMsg);
						}

						////////////////////////////////////////////////
						// try Tolerance  correction
						itol++;
						if (!bOp && itol <= 4)
						{
//qiu							Standard_Real locTol = 1.e-07*(pow(10, itol));
							Standard_Real locTol = 1.e-07*(pow(10.0, itol));
                                                        TCollection_AsciiString msg(itol);
                                                        msg += "_#_McCadCSGBuild_SolidFromCSG.cxx :: .  Tolerance correction due to failed BOP!!";
                                                        msgr->Message(msg.ToCString(), McCadMessenger_WarningMsg);

							/////////////////////////////////////////////////////////////////
							ShapeFix_ShapeTolerance setter;
							Standard_Boolean sett;
							//	  sett = setter.LimitTolerance(oldSol1,locTol,0.0,TopAbs_SOLID);
							sett = setter.LimitTolerance(polSol1, locTol, 0.0, TopAbs_SOLID);
						}
						else if (!bOp && itol > 4)
						{
							itol = 0;
                                                        TCollection_AsciiString msg( "_#_McCadCSGBuild_SolidFromCSG.cxx :: Tolerance correction failed: nonlinear halfspaces number ....   ");
                                                        msg += k;
                                                        msgr->Message(msg.ToCString(), McCadMessenger_WarningMsg);

							failedHSolSeq->Append(polyHSolids->Value(k));
							//continue;
							break; // while loop and continue
						}
						else
						{
						//	cout << "Processing linear halfspaces number ....   "	<< k << " .... finished. " << endl;
							need2repeat = Standard_False;
							for ( TopExp_Explorer Ex(aShape, TopAbs_SOLID); Ex.More(); Ex.Next() )
							{
								polSol1 = TopoDS::Solid( Ex.Current());
							}
						}
					}
				}
			}
		}
		else
		{
			polSol1 = Big;
		}
		//////////////////////////////////////////////////////////////////////

//	BRepTools::Write(polSol1, "polSol1.rle");

		BRepCheck_Analyzer BA3(polSol1, Standard_False);
		if (BA3.IsValid())
			; // mySolids->Append(polSol1);
		else
                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: ***** Invalid linear  Solid computed **** ",
                                      McCadMessenger_ErrorMsg);

		//Big = polSol1;

                int itol=0, ideg = 1;
		//Standard_Boolean openF = Standard_False;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//## perform boolean common operation for nonlinear half spaces
		cout << " Number of nonlinear halfspaces =   " << HSolSeq->Length()	<< endl;
		if (HSolSeq->Length() > 0)//for all nonlinear halfspaces
		{
			for (int k=1; k<=HSolSeq->Length() ; k++)
			{
				itol = 0;
				Standard_Boolean bOp= Standard_False;

				oldSol1 = TopoDS::Solid(HSolSeq->Value(k));
				cout << "Processing nonlinear halfspaces number .........   "	<< k << endl;

				//perform boolean common until success or till tolerance correction is too high.
				Standard_Boolean need2repeat = Standard_True;
				while(need2repeat)
				{
					try
					{
//qiu						BOPTools_DSFiller pDF;
//qiu						pDF.SetShapes(oldSol1, polSol1);
//qiu						pDF.Perform();

//qiu						BRepAlgoAPI_Common aCommonOp(oldSol1,polSol1, pDF);
                        BRepAlgoAPI_Common aCommonOp(oldSol1,polSol1);

						if(aCommonOp.IsDone())
						{
							bOp= Standard_True;
							tmpS1 = aCommonOp.Shape();
						}
						else
						{
							bOp= Standard_False;
                                                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Boolean Operation on a halfspace failed !!! 6",
                                                                      McCadMessenger_WarningMsg);
						}
					}
					catch(...)
					{
						bOp = Standard_False;
						/*STEPControl_Writer flWrt;
						flWrt.Transfer(polSol1, STEPControl_AsIs);
						flWrt.Write("Failed.stp");*/
                                                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Boolean Operation on a halfspace failed !!! 7",
                                                              McCadMessenger_WarningMsg);
						//exit(-1);
						break;
					}

					const TopoDS_Shape& aShape = tmpS1;
					BRepCheck_Analyzer BA(aShape, Standard_False);
					int sCount=0;

					for (TopExp_Explorer ex(aShape, TopAbs_SOLID); ex.More(); ex.Next()) //perform shape tests
					{
						sCount++;
						const TopoDS_Solid& solid = TopoDS::Solid(ex.Current());
						if (solid.IsNull())
						{
                                                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: BOP for solid failed: NULL SOLID computed !!",
                                                                      McCadMessenger_WarningMsg);
							bOp = Standard_False;
							break;
						}
						for (TopExp_Explorer exs(solid, TopAbs_SHELL); exs.More(); exs.Next())
						{
							const TopoDS_Shell& sh = TopoDS::Shell(exs.Current());
							BA.Init(sh, Standard_False);
							Handle(BRepCheck_Result) RES = BA.Result(sh);
							BRepCheck_ListOfStatus StatusList;
							StatusList = RES->Status();
							BRepCheck_ListIteratorOfListOfStatus iter;

							if (!BA.IsValid())
							{
                                                                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: BRepCheck_Analyzer for shell failed !!",
                                                                              McCadMessenger_WarningMsg);
								bOp = Standard_False;
								break;
							}
						}
						///////////////////////////////////////////////////////////////////////////////////////////
						for (TopExp_Explorer exf(solid, TopAbs_FACE); exf.More(); exf.Next())
						{
							const TopoDS_Face& aF = TopoDS::Face(exf.Current());
							BA.Init(aF, Standard_False);
							Handle(BRepCheck_Result) RES = BA.Result(aF);
							BRepCheck_ListOfStatus StatusList;
							StatusList = RES->Status();

							if (BA.IsValid())
								;
							else
							{
                                                                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: BRepCheck_Analyzer for face failed !!",
                                                                              McCadMessenger_WarningMsg);
								bOp = Standard_False;
								break;
							}
						}
						///////////////////////////////////////////////////////////////////////////////////////////
					}

					BA.Init(aShape, Standard_False);

					if (!BA.IsValid())
					{
                                                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: BRepCheck_Analyzer for solid failed !! - 1" ,
                                                               McCadMessenger_WarningMsg);
						bOp = Standard_False;
					}

					if (sCount==0)
					{
						bOp = Standard_False; // if check is run on compound containing no solid
                                                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Compound containing no solid computed !!",
                                                              McCadMessenger_WarningMsg);
					}

					if (tmpS1.IsNull())
					{
						bOp = Standard_False;
                                                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx ::  Null solid computed !!! " ,
                                                              McCadMessenger_WarningMsg);
					}

					////////////////////////////////////////////////
					// try Tolerance  correction
					itol++;
					if (!bOp && itol <= 4)
					{
//qiu						Standard_Real locTol = 1.e-07*(pow(10, itol));
						Standard_Real locTol = 1.e-07*(pow(10.0, itol));
						cout << itol <<" _ " << ideg<< "   Tolerance correction due to failed BOP!!!"	<< endl;

						/////////////////////////////////////////////////////////////////
						ShapeFix_ShapeTolerance setter;
						Standard_Boolean sett;
						//	  sett = setter.LimitTolerance(oldSol1,locTol,0.0,TopAbs_SOLID);
						sett = setter.LimitTolerance(polSol1, locTol, 0.0,	TopAbs_SOLID);
					}
					else if (!bOp && itol > 4)
					{
						itol = 0;
						cout<< "Tolerance correction failed: nonlinear halfspaces number ....   "	<< k << endl;
						failedHSolSeq->Append(HSolSeq->Value(k));
						break; // ... while loop and continue
					}
					else
					{
						need2repeat = Standard_False;
						for ( TopExp_Explorer Ex(aShape, TopAbs_SOLID); Ex.More(); Ex.Next() )
						{
							polSol1 = TopoDS::Solid( Ex.Current());
						}
					}
				}
				/*STEPControl_Writer aWrtr;
				aWrtr.Transfer(polSol1, STEPControl_AsIs);
				TCollection_AsciiString newTest("newTest_");
				//newTest += TCollection_AsciiString(itol);
				newTest += ".stp";
				aWrtr.Write(newTest.ToCString());*/
			}
		}
		//cout << "...done\n\n";
		//Big = polSol1;

                itol=0, ideg = 1;
		HSolSeq->Clear();
		HSolSeq->Append(failedHSolSeq);

	//##reprocess failed solids
		if(HSolSeq->Length() > 0)
		{
			cout << "\n\nReprocessing failed Half Spaces\n";
			cout << "number of failed HS : " << HSolSeq->Length() << "\n+++++++++++++++++++++++++++++++++++++++++++\n";

		//## shrink Big to reasonable size
			Bnd_Box aidBox;
			BRepBndLib::Add(polSol1, aidBox);
			Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
			aidBox.SetGap(5);
			aidBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
			cout << "aidBox :  " << aXmin << " " << aYmin << " " << aZmin << "  " << aXmax << " " << aYmax << " " << aZmax << endl;
			TopoDS_Shape aidShp = BRepPrimAPI_MakeBox(gp_Pnt(aXmin, aYmin, aZmin), gp_Pnt(aXmax, aYmax, aZmax)).Shape();
			Big = TopoDS::Solid(aidShp);

		//## process all failed solids
			for (int k=1; k<=HSolSeq->Length(); k++)
			{
				ShapeFix_ShapeTolerance setter;
				Standard_Real scaleFactor=1.0;
				Standard_Boolean bopFailed = Standard_True, sett = Standard_False;
				Standard_Real aTol(1.e-5);

				sett = setter.LimitTolerance(Big, aTol, 0.0, TopAbs_SOLID);

				if(!sett)
					cout << "McCadCSGBuild_SolidFromCSG :: could not reset tolerance!\n";

			//## compute boxed half space
				while(bopFailed)
				{
					cout << "scaleFactor : " << scaleFactor << endl;
					TopoDS_Shape tmpShp = Scale(Big,scaleFactor);

					try
					{
//qiu						BOPTools_DSFiller pDF;
//qiu						pDF.SetShapes(tmpShp, HSolSeq->Value(k));
//qiu						pDF.Perform();

//qiu						BRepAlgoAPI_Common outerCommonOp(tmpShp,HSolSeq->Value(k), pDF);
                        BRepAlgoAPI_Common outerCommonOp(tmpShp,HSolSeq->Value(k));

						if(outerCommonOp.IsDone())
						{
							tmpS2 = outerCommonOp.Shape();

							/*STEPControl_Writer aWrtr;
							aWrtr.Transfer(tmpS2, STEPControl_AsIs);
							aWrtr.Write("tmpS2.stp");*/

							if(tmpS2.IsNull())
							{
								cout << "\n## NULL Shape computed\n\n";
								continue;
							}

							Standard_Integer shCnt(0);
							for ( TopExp_Explorer Ex(tmpS2, TopAbs_SOLID); Ex.More(); Ex.Next() )
							{
								tmpS2 = Ex.Current();
								shCnt++;
							}

							if(shCnt == 1)
							{
								if(tmpS2.IsNull())
									cout << "NULL SOLID COMPUTED\n\n";
								else
									bopFailed = Standard_False;
							}
							else if(shCnt > 1)
							{
								cout << "More than one Solid resulted!!!\n";
								bopFailed = Standard_True;
							}
							else
							{
								bopFailed = Standard_True;
							}
						}
						else
						{
							bopFailed = Standard_True;
						}
					}
					catch(Standard_Failure)
					{
						bopFailed = Standard_True;
                                                msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Catch: Boolean Operation on a halfspace failed !!!",
                                                              McCadMessenger_WarningMsg);
						Standard_Failure::Caught()->Print(cout); cout << endl;
					}
					scaleFactor+=0.1;
				}


			//## compute common of polSol1 and tmpS2
				try
				{
//qiu					BOPTools_DSFiller pDF;
//qiu					pDF.SetShapes(tmpS2, polSol1);
//qiu					pDF.Perform();

//qiu					BRepAlgoAPI_Common theCOp(tmpS2, polSol1, pDF);
                    BRepAlgoAPI_Common theCOp(tmpS2, polSol1);

					if(theCOp.IsDone())
					{
						tmpS1 = theCOp.Shape();

						if(tmpS1.IsNull())
						{
							cout << "\n## NULL Shape computed\n\n";
							continue;
						}

						Standard_Integer shCnt(0);
						for ( TopExp_Explorer Ex(tmpS1, TopAbs_SOLID); Ex.More(); Ex.Next() )
						{
							newSol = TopoDS::Solid(Ex.Current());
							shCnt++;
						}

						if(shCnt == 1)
						{
							if(newSol.IsNull())
							{
								cout << "NULL SOLID COMPUTED\n\n";
								break;
							}

							Standard_Real oldVol, newVol, tmpVol;
//qiu							oldVol = McCadMcVoid::VolumeOfShape(polSol1);
//qiu							newVol = McCadMcVoid::VolumeOfShape(newSol);
							oldVol = VolumeOfShape(polSol1);
							newVol = VolumeOfShape(newSol);

							if(oldVol < newVol)
							{
								cout << "COMMON HAS BIGGER VOLUME THAN ORIGINAL SOLID!!!\n";
								tmpVol = oldVol;
								oldVol = newVol;
								newVol = tmpVol;
							}

							if(Abs(Abs( oldVol / newVol) -1) < aTol)
							{
								cout << "Nearly identical VOLUMES              !!!\n";
								cout.precision(15);
								cout << "Volume 1 :  " << oldVol << endl;
								cout << "Volume 2 :  " << newVol << endl;
								cout << "Common operation is considered failed !!!\n";

								// extract verties from polSol1 and build BoundingBox

								cout << "Trying to double cut!!!\n";
								TopoDS_Shape cShp1 = Big;
								BRepAlgoAPI_Cut firstCut(cShp1, tmpS2);

								if(firstCut.IsDone())
								{
									TopoDS_Shape tmpShp = firstCut.Shape();

									ShapeFix_ShapeTolerance setter;
									Standard_Boolean sett;
									sett = setter.LimitTolerance(polSol1, 1e-3, 0.0, TopAbs_SOLID);
									if(!sett)
											cout << "McCadCSGBuild_SolidFromCSG :: could not reset tolerance! 0 \n";
									sett = setter.LimitTolerance(tmpShp, 1e-3, 0.0, TopAbs_SOLID);
									if(!sett)
											cout << "McCadCSGBuild_SolidFromCSG :: could not reset tolerance! 1 \n";

									BRepAlgoAPI_Cut secondCut(polSol1, tmpShp);

									if(secondCut.IsDone())
									{
										tmpShp = secondCut.Shape();

//qiu										oldVol = McCadMcVoid::VolumeOfShape(polSol1);
//qiu										newVol = McCadMcVoid::VolumeOfShape(tmpShp);
                                        oldVol = VolumeOfShape(polSol1);
										newVol = VolumeOfShape(tmpShp);

										if(Abs( oldVol / newVol)-1 < aTol)
										{
											cout << "STILL THE SAME VOLUME !!!\n";
											cout.precision(15);
											cout << "Volume 1 :  " << oldVol << endl;
											cout << "Volume 2 :  " << newVol << endl;
											cout << Abs(oldVol / newVol)-1 << endl;
										}

										BRepCheck_Analyzer BA(tmpShp, Standard_True);
										if(!BA.IsValid())
										{
											 cout <<  "Second cut failed !!!"  << endl;

											 ShapeAnalysis_Shell sas;
											 sas.LoadShells(tmpShp);
											 sas.CheckOrientedShells(tmpShp, Standard_True);

											 if(sas.HasFreeEdges())
											 {
												 cout << "Building missing face!!!\n";
												 newSol = RepairOpenShell(tmpShp);
											 }
										}
										else
										{
											cout << "????\n";
											newSol = TopoDS::Solid(tmpShp);
										}

									}
									else
										cout << "SECOND CUT FAILED\n";

								}
								else
									cout << "CUTTING FAILED\n";
							}

							polSol1 = newSol;
						}
						else if(shCnt > 1)
						{
							cout << "More than one Solid resulted!!!\n";
						}
						else
						{
							cout << "NULL Solid computed\n";
						}
					}
					else
					{
						cout << "Common Operation failed!!\n";
					}
				}
				catch(Standard_Failure)
				{
                                        msgr->Message("_#_McCadCSGBuild_SolidFromCSG.cxx :: Catch: Boolean Operation on a halfspace failed !!!",
                                                      McCadMessenger_WarningMsg);
					Standard_Failure::Caught()->Print(cout); cout << endl;
				}

			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (polSol1.IsNull())
                        msgr->Message("***** Null solid computed **** ", McCadMessenger_ErrorMsg);

		mySolids->Append(polSol1);
		cout << "======================== "<< endl;
		cout << "Cell processing ended "<< endl;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
}


Handle(TopTools_HSequenceOfShape) McCadCSGBuild_SolidFromCSG::Solids() const
{
	return mySolids;
}


TopoDS_Solid McCadCSGBuild_SolidFromCSG::Fused() const
{
	TopoDS_Shape tmpSol, oldSol, newSol;

	if (mySolids->Length() == 1)
		return TopoDS::Solid(mySolids->Value(1));

	else
	{

		for (int k=2; k<=mySolids->Length() ; k++)
		{
			if (k==2)
				oldSol = TopoDS::Solid(mySolids->Value(k-1));

			newSol = TopoDS::Solid(mySolids->Value(k));

			tmpSol = BRepAlgoAPI_Fuse(oldSol, newSol);

			oldSol = tmpSol;

		}
	}
	cout << "McCadCSGBuild_SolidFromCSG::Fused() " << endl;

	return TopoDS::Solid(tmpSol);
}


Standard_Integer McCadCSGBuild_SolidFromCSG::NbSolids() const
{
	return mySolids->Length();
}


McCadCSGBuild_SolidType McCadCSGBuild_SolidFromCSG::Type() const
{

	return myType;

}

//    BOPTools_Tools3D::RemoveSims(tmpS2,ctx);
/*
 BRepCheck_Analyzer BA5(tmpS2,Standard_True);
 if(BA5.IsValid()) ;
 else
 {
 cout << k <<  ".1 After BOP:  Invalid Solid computed, fix follows.  "  << endl;
 Handle(ShapeFix_Shape) genericFix = new ShapeFix_Shape;
 genericFix->Init(tmpS2);
 genericFix->SetPrecision (1.e-03);
 genericFix->SetMaxTolerance(0.0);
 genericFix->Perform();
 tmpS2 =TopoDS::Solid(genericFix->Shape());
 }
 BA5.Init(tmpS2);
 if(BA5.IsValid()) ;
 else
 {
 cout << k <<  ".2 After BOP:  Invalid Solid computed, fix follows  "  << endl ;
 Handle(ShapeFix_Wireframe) wFix = new ShapeFix_Wireframe(tmpS2);
 wFix->SetPrecision(1.e-03);
 wFix->SetMaxTolerance(0.0);
 wFix->FixSmallEdges();
 wFix->FixWireGaps();
 tmpS2 =TopoDS::Solid(wFix->Shape());
 }
 BA5.Init(tmpS2);
 if(BA5.IsValid()) ;
 else  cout << k <<  ". After BOP:  Invalid Solid computed, fix failed  "  << endl ;
 */
/*

 BRep_Builder BB; Standard_Real locTol = 1.e-03;
 for ( TopExp_Explorer ex(tmpS2,TopAbs_FACE); ex.More(); ex.Next())
 {
 const TopoDS_Face& iFace = TopoDS::Face(ex.Current());
 const Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*)&iFace.TShape());
 TF->Tolerance(locTol);
 TopExp_Explorer anExpW (iFace, TopAbs_WIRE);
 for (; anExpW.More(); anExpW.Next()) {
 const TopoDS_Shape& aW=anExpW.Current();
 TopExp_Explorer anExp(aW, TopAbs_EDGE);
 for (; anExp.More(); anExp.Next())   {
 const TopoDS_Edge& aE=TopoDS::Edge(anExp.Current());
 const Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&aE.TShape());
 TE->Tolerance(locTol);
 TopExp_Explorer anExpV(aE, TopAbs_VERTEX);
 for (; anExpV.More(); anExpV.Next()) {
 const TopoDS_Vertex& aVx=TopoDS::Vertex(anExpV.Current());
 const Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*)&aVx.TShape());
 TV->Tolerance(locTol);
 }
 }
 }
 }


 Handle(ShapeFix_FixSmallFace) fixer = new  ShapeFix_FixSmallFace;
 fixer->Init(oldSol1);
 fixer->SetPrecision(1.e-03);
 fixer->SetMaxTolerance(1.e-03);
 fixer->Perform();
 oldSol1 = TopoDS::Solid(fixer->Shape());


 if (!sh.Free ()) {cout << "Free Shell !! " << endl; sh.Free(Standard_True);}

 TopoDS_Solid solid;
 BRep_Builder B;
 B.MakeSolid(solid);
 B.Add(solid,sh);
 try {
 BRepClass3d_SolidClassifier bsc3d (solid);
 Standard_Real t = Precision::Confusion();
 bsc3d.PerformInfinitePoint(t);

 if (bsc3d.State() == TopAbs_IN) {
 if (!sh.Free ()) sh.Free(Standard_True);
 TopoDS_Solid soli2;
 B.MakeSolid (soli2);
 sh.Reverse();
 B.Add (soli2,sh);
 solid = soli2;
 }



 Standard_Real UMin=0,UMax=0,VMin=0,VMax=0;
 for ( TopExp_Explorer ex(polSol1,TopAbs_FACE); ex.More(); ex.Next())
 {
 TopLoc_Location L;
 TopoDS_Face F = TopoDS::Face(ex.Current());
 BRepTools::Update(F);
 Standard_Real tUMin=0,tUMax=0,tVMin=0,tVMax=0;
 BRepTools::UVBounds(F,tUMin,tUMax,tVMin,tVMax);
 BRepAdaptor_Surface BS(F,Standard_True);
 GeomAdaptor_Surface adSurf= BS.Surface();
 if (adSurf.GetType() == GeomAbs_Plane)
 {
 cout << tUMin << " " << tUMax <<  " " << tVMin << " " << tVMax << endl;
 //	  if ((fabs(fabs(tUMax) - fabs(tUMin)) >= fabs(fabs(UMax) - fabs(UMin))))
 // if ((fabs(fabs(tVMax) - fabs(tVMin)) >= fabs(fabs(VMax) - fabs(VMin))) )


 if (UMin > tUMin)  UMin = tUMin;
 if (UMax < tUMax)  UMax = tUMax;
 if (VMin > tVMin)  VMin = tVMin;
 if (VMax < tVMax)  VMax = tVMax;

 }
 }


 ////////////////////////////////////////////////////////////////////
 TopoDS_Face HFace;
 for (TopExp_Explorer ex(failedHSolSeq->Value(k),TopAbs_FACE); ex.More(); ex.Next())
 HFace = TopoDS::Face(ex.Current());

 BRepAlgo_Section Sec(HFace,polSol1,Standard_False);


 Sec.ComputePCurveOn1(Standard_True);
 // Sec.ComputePCurveOn2(Standard_True);
 Sec.Approximation(Standard_False);
 Sec.Build();
 TopoDS_Shape secShape = Sec.Shape(); // we expect hier a wire
 int iz=0;
 TopoDS_Wire iWire;
 BRep_Builder fB;
 fB.MakeWire(iWire);

 Handle(BRepAlgo_EdgeConnector) Con = new BRepAlgo_EdgeConnector;

 for (TopExp_Explorer ex(secShape,TopAbs_EDGE); ex.More(); ex.Next())
 {
 TopoDS_Edge anEdge = TopoDS::Edge(ex.Current());
 if (!BRep_Tool::Degenerated(anEdge))
 {
 fB.Add(iWire,anEdge);
 cout << ++iz << endl;
 }
 }
 iWire = BRepAlgo::ConcatenateWire(iWire, GeomAbs_C1, 1e-03);
 if(Con->IsWire(iWire)) cout << " Wire seems to be ok!! " << endl ;
 else cout << " Wire seems to be not ok!! " << endl;

 TopAbs::Print(secShape.ShapeType(),cout); cout << endl;
 ////////////////////////////////////////////////////////////////
 TopoDS_Shell fSh;
 fB.MakeShell(fSh);
 TopoDS_Solid fSolid;
 iz=0;
 for (TopExp_Explorer ex(failedHSolSeq->Value(k),TopAbs_FACE); ex.More(); ex.Next())
 {
 TopLoc_Location L;
 const TopoDS_Face& iFace = TopoDS::Face(ex.Current());
 //
 //	  TopoDS_Face fF = BRepBuilderAPI_MakeFace(BRep_Tool::Surface(iFace,L),iWire);
 // TopoDS_Face fF = BRepBuilderAPI_MakeFace(iWire);
 TopoDS_Face fF = BRepBuilderAPI_MakeFace(iFace,iWire);
 const Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*)&fF.TShape());
 TF->Tolerance(1.0);
 // fF.Orientation(TopAbs_INTERNAL);
 fF.Orientation(iFace.Orientation());
 fB.Add(fSh,fF);
 Standard_Real tUMin=0,tUMax=0,tVMin=0,tVMax=0;
 BRepTools::Update(fF);
 BRepTools::UVBounds(fF,tUMin,tUMax,tVMin,tVMax);
 BRepAdaptor_Surface BS(fF,Standard_True);
 GeomAdaptor_Surface adSurf= BS.Surface();
 cout << tUMin << " " << tUMax <<  " " << tVMin << " " << tVMax << endl;
 cout << ++iz << endl;
 }

 cout << " New  halfspace face computed !! " << endl;

 for (TopExp_Explorer ex(Big,TopAbs_FACE); ex.More(); ex.Next())
 {
 const TopoDS_Face& iFace = TopoDS::Face(ex.Current());
 fB.Add(fSh,iFace);
 cout << ++iz << endl;
 }

 if (!fSh.Free ()) fSh.Free(Standard_True); //  fSh.Reverse();

 fSh.Reverse();

 fSh.Compose(TopAbs_FORWARD);

 if (!fSh.Free ()) fSh.Free(Standard_True);
 fB.MakeSolid(fSolid);
 fB.Add(fSolid,fSh);

 BRepCheck_Analyzer BAF(fSolid,Standard_True);
 if(BAF.IsValid())
 {
 cout << " New  halfspace solid computed !! " << endl;
 }
 else
 {
 cout << " New  halfspace solid is invalid !! " << endl;
 fSolid =  BRepBuilderAPI_MakeSolid(fSh).Solid();
 Handle(BRepCheck_Shell)  res = new BRepCheck_Shell(fSh);
 BRepCheck::Print(res->Closed(),cout);
 BRepCheck::Print(res->Orientation(),cout);
 if(res->IsUnorientable()) cout << " Shell Is Unorientable !! " << endl;
 cout << " Shell Check finished !! " << endl;
 }


 BRepCheck_Analyzer BA(polSol1,Standard_False);
 if(BA.IsValid())
 {
 mySolids->Append(polSol1);
 cout << "Cell processing ended  "<< endl;
 cout << "======================== "<< endl;
 }
 else
 {
 cout << " Invalid Solid computed: fix follows !! " << endl;
 TopoDS_Shell tmpSh;
 for ( TopExp_Explorer ex(polSol1,TopAbs_SHELL); ex.More(); ex.Next())
 {
 tmpSh = TopoDS::Shell(ex.Current());

 Handle(ShapeFix_Shell) genericFix = new ShapeFix_Shell;
 genericFix->Init(tmpSh);
 genericFix->SetPrecision (1.e-03);
 genericFix->SetMaxTolerance(1.e-03);
 genericFix->Perform();
 tmpSh =TopoDS::Shell(genericFix->Shape());

 if(BA.IsValid(tmpSh))
 {
 cout << " Shell is  valid!! " << endl ;
 TopoDS_Solid aSolid = BRepBuilderAPI_MakeSolid(tmpSh).Solid();
 mySolids->Append(aSolid);
 }
 else
 {
 cout << " Shell is  not valid!! " << endl ;

 TopoDS_Solid aSolid = BRepBuilderAPI_MakeSolid(tmpSh).Solid();
 mySolids->Append(aSolid);
 Handle(BRepCheck_Shell)  res = new BRepCheck_Shell(tmpSh);
 BRepCheck::Print(res->Closed(),cout);
 BRepCheck::Print(res->Orientation(),cout);
 if(res->IsUnorientable()) cout << " Shell Is Unorientable !! " << endl;
 }
 for ( TopExp_Explorer ex(tmpSh,TopAbs_FACE); ex.More(); ex.Next())
 {
 const TopoDS_Face& iFace = TopoDS::Face(ex.Current());

 if(BA.IsValid(iFace))cout << " Face is valid!! " << endl;
 else {
 cout << " Face is not valid!! " << endl;
 Handle(BRepCheck_Face)  res = new BRepCheck_Face(iFace);
 res->GeometricControls(Standard_True);
 BRepCheck::Print(res->IntersectWires(),cout);
 BRepCheck::Print(res->ClassifyWires(),cout);
 BRepCheck::Print(res->OrientationOfWires(),cout);
 if(res->IsUnorientable()) cout << " Face Is Unorientable !! " << endl;
 }
 TopExp_Explorer anExpW (iFace, TopAbs_WIRE);
 for (; anExpW.More(); anExpW.Next()) {
 const TopoDS_Shape& aW=anExpW.Current();
 if(BA.IsValid(aW))  cout << " Wire is valid!! " << endl;
 else cout << " Wire is not valid!! " << endl;
 TopExp_Explorer anExp(aW, TopAbs_EDGE);
 for (; anExp.More(); anExp.Next())   {
 const TopoDS_Edge& aE=TopoDS::Edge(anExp.Current());
 if(BA.IsValid(aE))  cout << " Ege is  valid!! " << endl;
 else cout << " Ege is  not  valid!! " << endl;
 TopExp_Explorer anExpV(aE, TopAbs_VERTEX);
 for (; anExpV.More(); anExpV.Next()) {
 const TopoDS_Vertex& aVx=TopoDS::Vertex(anExpV.Current());
 if(BA.IsValid(aVx))  cout << " Verice is  valid!! " << endl;
 else cout << " Verice is  not valid!! " << endl;

 }
 }
 }
 }
 }
 cout << "Cell processing ended "<< endl;
 cout << "======================== "<< endl;
 }



 */
