#include <McCadCSGAdapt_ShapeAnalyser.ixx>
#include <BRep_Tool.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopLoc_Location.hxx>

#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <ShapeCustom.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>

//==================================================================================
// Function:  McCadCSGAdapt_ShapeAnalyser()
// Description :
// Author       : H. Tsige-Tamirat 
// Date         : Fri Jan  2 19:06:57 CET 2001
//==================================================================================

McCadCSGAdapt_ShapeAnalyser::McCadCSGAdapt_ShapeAnalyser()
{
 
}
//==================================================================================
// Function: McCadCSGAdapt_ShapeAnalyser(const TopoDS_Shape& theShape)
// Description :
// Author       : H. Tsige-Tamirat 
// Date         : Fri Jan  2 19:06:57 CET 2001
//==================================================================================

McCadCSGAdapt_ShapeAnalyser::McCadCSGAdapt_ShapeAnalyser(const TopoDS_Shape& theShape)
{

  myShape = theShape;

}

void McCadCSGAdapt_ShapeAnalyser::Init(const TopoDS_Shape& theShape) 
{
  myShape = theShape;

}

void McCadCSGAdapt_ShapeAnalyser::SetTolerance(const Standard_Real theTol) 
{

  if (myShape.IsNull() || theTol <= 0) return;
  ShapeFix_ShapeTolerance ShapeTol;
  ShapeTol.SetTolerance(myShape,theTol);

}

Standard_Real McCadCSGAdapt_ShapeAnalyser::GetTolerance() const
{
  ShapeAnalysis_ShapeTolerance ShapeTolAnalysis;
  return ShapeTolAnalysis.Tolerance(myShape,0,TopAbs_SHAPE); 

}

void McCadCSGAdapt_ShapeAnalyser::PrintContent(Standard_OStream& theStream) const
{
 
  Standard_Integer NbSharedSolids,NbSolidsWithVoids, NbSolids;
  Standard_Integer NbSharedShells, NbShells,  NbFaces, NbWires, NbEdges, NbVertices;
  TopExp_Explorer exp; 
  TopTools_MapOfShape mapsh; 
  NbSharedSolids= 0;
  NbSolidsWithVoids=0;
  NbSolids = 0;
  for (exp.Init (myShape,TopAbs_SOLID); exp.More(); exp.Next()) { 
    TopoDS_Solid theSolid = TopoDS::Solid (exp.Current()); 
    theSolid.Location(TopLoc_Location()); 
    mapsh.Add(theSolid); 
    Standard_Integer nbs = 0; 
    for (TopExp_Explorer shex(theSolid,TopAbs_SHELL); shex.More(); shex.Next()) 
      nbs ++; 
    if (nbs > 1) NbSolidsWithVoids++; 
    NbSolids++;   
  } 
  NbSharedSolids = mapsh.Extent(); 

  NbShells=0;
  NbFaces = 0; 
  NbWires = 0; 
  NbEdges = 0;  
  NbVertices = 0;
  
  mapsh.Clear(); 

  for (exp.Init (myShape,TopAbs_SHELL); exp.More(); exp.Next()) { 
    NbShells++; 
    TopoDS_Shell theShell = TopoDS::Shell(exp.Current()); 
    theShell.Location(TopLoc_Location()); 
    mapsh.Add(theShell); 
    for (TopExp_Explorer shel (theShell,TopAbs_FACE); shel.More(); shel.Next()) 
    TopoDS_Face theFace = TopoDS::Face(shel.Current());  
    NbFaces++; 
  } 
 NbSharedShells = mapsh.Extent(); 

 theStream << "====== Shape Content Analysis =====================" <<'\n';
 theStream << "Shared Solids: " <<  NbSharedSolids  <<'\n';
 theStream << "Solids: " << NbSolids <<'\n';
 theStream << "Solids with voids: " << NbSolidsWithVoids <<'\n';
 theStream << "========================================" <<'\n';
 theStream << "Shared Shells: " << NbSharedShells <<'\n';
 theStream << "Shells: " << NbShells <<'\n';
 theStream << "Total Number of Faces: " <<  NbFaces <<'\n';
 theStream << "========================================" <<'\n';
}

void McCadCSGAdapt_ShapeAnalyser::PrintInfo(Standard_OStream& theStream) const
{
  // print here avrage  tolerance for face, edge and vertices.
  ShapeAnalysis_ShapeTolerance ShapeTolAnalysis;
  Standard_Real AvTol = ShapeTolAnalysis.Tolerance(myShape,0,TopAbs_SHAPE); 
  Standard_Real MinTol = ShapeTolAnalysis.Tolerance(myShape,-1,TopAbs_SHAPE); 
  Standard_Real MaxTol = ShapeTolAnalysis.Tolerance(myShape,1,TopAbs_SHAPE); 
  theStream << "======  Tolerance =====================" <<'\n';
  theStream << "Avrage  global Tolerance: " << AvTol <<'\n';
  theStream << "Minimal global Tolerance: " << MinTol <<'\n';
  theStream << "Maximal global Tolerance: " << MaxTol <<'\n';
  theStream << "========================================" <<'\n';
}
void McCadCSGAdapt_ShapeAnalyser::ScaleShape(const Standard_Real theFactor)
{

 TopoDS_Shape theScaledShape = ShapeCustom::ScaleShape(myShape, theFactor);
 myShape = theScaledShape;

}

TopoDS_Shape McCadCSGAdapt_ShapeAnalyser::Shape() const
{
  return myShape;
}
