#include <McCadCSGAdapt_ShellAnalyser.ixx>
#include <ShapeAnalysis_Shell.hxx>
#include <Handle_ShapeFix_Shell.hxx>
#include <ShapeFix_Shell.hxx>

McCadCSGAdapt_ShellAnalyser::McCadCSGAdapt_ShellAnalyser()
{
  isFixed = Standard_False;
}

McCadCSGAdapt_ShellAnalyser::McCadCSGAdapt_ShellAnalyser(const TopoDS_Shell& theShell)
{
  Init(theShell);
}

void McCadCSGAdapt_ShellAnalyser::Init(const TopoDS_Shell& theShell) 
{
  myShell = theShell;
  isFixed = Standard_False;
}

Standard_Boolean McCadCSGAdapt_ShellAnalyser::CheckShell() const
{
  ShapeAnalysis_Shell ShellAnal;
  ShellAnal.CheckOrientedShells(myShell,Standard_True);
    if(ShellAnal.HasBadEdges() || ShellAnal.HasFreeEdges()) return Standard_False;
    else return Standard_True;
}

TopoDS_Shell McCadCSGAdapt_ShellAnalyser::FixedShell()
{
  if(isFixed)
    return myShell;
  else 
    {
      Fix();
      return FixedShell();
    }  
}

void McCadCSGAdapt_ShellAnalyser::Fix() 
{

  isFixed = Standard_True;
  Handle(ShapeFix_Shell) theShellFix = new ShapeFix_Shell;
  theShellFix->Init(myShell);
  theShellFix->Perform();
  TopoDS_Shell theShell = theShellFix->Shell();
  myShell = theShell;
}

