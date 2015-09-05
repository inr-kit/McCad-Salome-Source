Handle(TopTools_HSequenceOfShape)  LinApprox(Handle(TopTools_HSequenceOfShape) failedHSolSeq, const TopoDS_Solid& theSolid)
{

  ////////////////////////////////////////////////////////////////////
  Handle(TopTools_HSequenceOfShape) linHSpaces = new  TopTools_HSequenceOfShape();
 
  Bnd_Box2d  BB2;
  Bnd_Box BB3;
  Standard_Real UMin,UMax,VMin,VMax;
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax; 
  for (TopExp_Explorer ex(theSolid,TopAbs_VERTEX); ex.More(); ex.Next())
    {
      BRepBndLib::AddClose(ex.Current(),BB3);
      // BB3.Add(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current()))); 
      // BB3.SetGap(0.0);
    }
  BB3.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  TopoDS_Shape locBox = BRepPrimAPI_MakeBox(gp_Pnt(aXmin, aYmin, aZmin),  gp_Pnt(aXmax, aYmax, aZmax)).Shape();
  BB2.Add(gp_Pnt2d(aXmin, aYmin));
  BB2.Add(gp_Pnt2d(aXmin, aZmin));
  BB2.Add(gp_Pnt2d(aYmin, aZmin));
  BB2.Add(gp_Pnt2d(aXmax, aYmax));
  BB2.Add(gp_Pnt2d(aXmax, aZmax));
  BB2.Add(gp_Pnt2d(aYmax, aZmax));
  BB2.SetGap(0.0);
  BB2.Get(UMin,VMin,UMax,VMax); 
  int iz=0;
  for (int k =1; k<= failedHSolSeq->Length(); k++)
    {
      for (TopExp_Explorer ex(failedHSolSeq->Value(k),TopAbs_FACE); ex.More(); ex.Next()) 
	{
	  TopLoc_Location L;
	  const TopoDS_Face& iFace = TopoDS::Face(ex.Current()); 
	  const Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*)&iFace.TShape());
	  TF->Tolerance(1.e-03);
	  ////////////////////////////////////////////////////////////////////////////////////////
      TopoDS_Face fF = BRepBuilderAPI_MakeFace(BRep_Tool::Surface(iFace,L),UMin,UMax,VMin,VMax);
      fF.Orientation(iFace.Orientation());
      BRepTools::Update(fF);
      BRepTools::UVBounds(fF,UMin,UMax,VMin,VMax);
      cout << "Face Bounds  for        " <<  ++iz << endl;
      cout << UMin << " " << UMax <<  " " << VMin << " " << VMax << endl; 
      ////////////////////////////////////////////////////////////////////////////////
      BRep_Builder B; 
      TopoDS_Shell Sh; 
      B.MakeShell(Sh);
      TopoDS_Solid Solid;   
      B.Add(Sh,fF); 
      if (!Sh.Free ()) Sh.Free(Standard_True); 
      B.MakeSolid(Solid); 
      B.Add(Solid,Sh); 
      
      
      TopoDS_Shape tmpSh;
      try
	{

	  BRepAlgoAPI_Common COp(locBox,Solid); 
	  if(COp.IsDone())
	    {
	      
	      tmpSh  =  COp.Shape();
	    }
	  else
	    {
	      cout << "LinApprox BOP: Boolean Operation on a halfspace failed !!!" << endl;
	      continue; 
	    }
	  
	}
      catch(Standard_Failure) 
	{
	  cout << "LinApprox Catch: Boolean Operation on a halfspace failed !!!" << endl;
	  Standard_Failure::Caught()->Print(cout); cout << endl; 
	  continue;
	}
      

      ////////////////////////////////////////////////////////////////////////////////
      //////////////////////////// Triangulation /////////////////////////////////////
      for (TopExp_Explorer exF(tmpSh,TopAbs_FACE); exF.More(); exF.Next()) 
	{
	  TopLoc_Location loc;
	  TopoDS_Face lF = TopoDS::Face(exF.Current());
	  BRepTools::Update(lF);
	  BRepTools::UVBounds(lF,UMin,UMax,VMin,VMax);
	  cout << "Face Bounds  for        " <<  ++iz << endl;
	  cout << UMin << " " << UMax <<  " " << VMin << " " << VMax << endl; 

	  Handle(Geom_Surface) theSurf =  BRep_Tool::Surface(lF,loc);
	  Handle(Poly_Triangulation) mesh; 
	  BRepAdaptor_Surface BS(lF,Standard_True);
	  gp_Trsf T = BS.Trsf();
      
	  Standard_Real  aDeflection  = MAX2( fabs(UMax)-fabs(UMin), fabs(VMax)-fabs(VMin))/1.0;
	  BRepMesh::Mesh(lF, aDeflection);
	  mesh = BRep_Tool::Triangulation(lF,loc); 

	  if (mesh.IsNull()) cout << "Face triangulation failed !!" << endl;
	  else 
	    {
	      Standard_Integer nNodes = mesh->NbNodes();
	      TColgp_Array1OfPnt meshPnts(1,nNodes);
	      meshPnts = mesh->Nodes();
	      Standard_Integer nbTriangles = mesh->NbTriangles(); 
	      cout << " Number of Nodes  = "  << nNodes <<  " Number of Triangles  = "  << nbTriangles << endl; 
	      Standard_Integer n1, n2, n3; 
	      const Poly_Array1OfTriangle& Triangles = mesh->Triangles(); 
	      for (int i = 1; i <= nbTriangles; i++) 
		{ 
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
		  BRep_Builder fB; 
		  TopoDS_Shell fSh; 
		  fB.MakeShell(fSh);
		  TopoDS_Solid fSolid;   
		  fB.Add(fSh,F1); 
		  if (!fSh.Free ()) fSh.Free(Standard_True); 
		  fB.MakeSolid(fSolid); 
		  fB.Add(fSolid,fSh); 
		  linHSpaces->Append(fSolid);
		}
	    }
	}
	}
    }
  return linHSpaces;
}
