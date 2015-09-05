#include <McCadCSGAdapt_FaceAnalyser.ixx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

#include <ShapeAnalysis_Wire.hxx>
#include <ShapeFix.hxx>
#include <Handle_ShapeBuild_ReShape.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeCustom.hxx>
#include <ShapeAnalysis_CheckSmallFace.hxx>
#include <ShapeFix_FixSmallFace.hxx>

McCadCSGAdapt_FaceAnalyser::McCadCSGAdapt_FaceAnalyser()
{

}

McCadCSGAdapt_FaceAnalyser::McCadCSGAdapt_FaceAnalyser(const TopoDS_Shell& theShell)
{
  Init(theShell);
}

void McCadCSGAdapt_FaceAnalyser::Init(const TopoDS_Shell& theShell) 
{
  myPrecision = 1.e-02;
  myShell =theShell; 
  isFixed = Standard_False; 
  haveSmallFaces = HaveSmallFaces();
  //  haveSmallEdges = HaveSmallEdges();
 haveSmallEdges =  Standard_False;
 haveIndirect = Standard_True; 
}

void McCadCSGAdapt_FaceAnalyser::SetPrecision(const Standard_Real thePrecision) 
{
  myPrecision = thePrecision;
}

Standard_Real McCadCSGAdapt_FaceAnalyser::GetPrecision() const
{
  return myPrecision;
}

TopoDS_Shell McCadCSGAdapt_FaceAnalyser::FixedShell()
{
   if(isFixed)
     return myShell;
   else 
     {
       Fix();
       return FixedShell();
     }  
}

Standard_Boolean McCadCSGAdapt_FaceAnalyser::HaveSmallFaces() const
{
  if(myShell.IsNull()) return Standard_False;

  ShapeAnalysis_CheckSmallFace FaceAnal;
  Standard_Integer NbSmallfaces = 0; 
  for(TopExp_Explorer theExp(myShell,TopAbs_FACE); theExp.More(); theExp.Next()) 
    {
      TopoDS_Face theFace = TopoDS::Face(theExp.Current());
      TopoDS_Edge E1,E2; 
      if(FaceAnal.CheckSpotFace(theFace,myPrecision) || FaceAnal.CheckStripFace(theFace,E1,E2,myPrecision))
        NbSmallfaces++; 
    }
     
  if(NbSmallfaces) 
    return  Standard_True;
  else return Standard_False;

}

void McCadCSGAdapt_FaceAnalyser::DeleteSmallFaces() 
{
  ShapeFix_FixSmallFace FaceFix;
  FaceFix.Init(myShell);
  TopoDS_Shell theShell = TopoDS::Shell(FaceFix.RemoveSmallFaces());
  myShell = theShell;
  haveSmallFaces = Standard_False;

}

Standard_Boolean McCadCSGAdapt_FaceAnalyser::HaveSmallEdges() const
{
  Standard_Boolean tmpBoolean=Standard_False;

  if(myShell.IsNull()) return tmpBoolean;

  for(TopExp_Explorer theExp(myShell,TopAbs_FACE); theExp.More(); theExp.Next()) 
    {
      TopoDS_Face theFace = TopoDS::Face(theExp.Current());
      
      TopoDS_Wire theWire = TopoDS::Wire(theFace);

      ShapeAnalysis_Wire WireAnal(theWire,theFace,myPrecision);
      
      tmpBoolean = WireAnal.CheckSmall(myPrecision);
    }
     
   return tmpBoolean;
  
}

void McCadCSGAdapt_FaceAnalyser::DeleteSmallEdges() 
{
  Handle(ShapeBuild_ReShape) theContext = new ShapeBuild_ReShape();
  TopoDS_Shell theShell = TopoDS::Shell(ShapeFix::RemoveSmallEdges(myShell,myPrecision,theContext));
  myShell = theShell;
  haveSmallEdges = Standard_False;  

}

Standard_Boolean McCadCSGAdapt_FaceAnalyser::HaveInDirectFaces() const
{
  return haveIndirect;
}

void McCadCSGAdapt_FaceAnalyser::DoDirectFaces() 
{
  TopoDS_Shape theShape = ShapeCustom::DirectFaces(myShell);
  myShell = TopoDS::Shell(theShape);
  haveIndirect = Standard_False; 

}

void McCadCSGAdapt_FaceAnalyser::Fix() 
{
  isFixed = Standard_True;

  if(haveSmallEdges) 
    DeleteSmallEdges();
  if(haveSmallFaces) 
    DeleteSmallFaces();
  if(haveIndirect)
    DoDirectFaces();
    
}

