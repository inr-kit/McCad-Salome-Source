#include <McCadViewTool_BottomProject.ixx>
#include <V3d_View.hxx>

McCadViewTool_BottomProject::McCadViewTool_BottomProject(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
  myDoc = theDoc;
  myView = theView;
  myState = theState;
  myUndoState = theUndoState;
  myRedoState = theRedoState;
  myID = McCadTool_BottomProject;
}

void McCadViewTool_BottomProject::Destroy() 
{
  return;
}

Standard_Boolean McCadViewTool_BottomProject::IsNull()
{
	return Standard_False;
}

void McCadViewTool_BottomProject::Execute() 
{
	Handle(V3d_View) aView = myView->View();
  	aView->SetProj( V3d_Zneg );
	aView->FitAll();
	Done();
}

void McCadViewTool_BottomProject::UnExecute() 
{
  return;
}

void McCadViewTool_BottomProject::Suspend() 
{
  return;
}

void McCadViewTool_BottomProject::Resume() 
{
  return;
}

void McCadViewTool_BottomProject::MousePressEvent(const McCadWin_MouseEvent& e) 
{
  return;
}

void McCadViewTool_BottomProject::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{ 
 return;
 
}

void McCadViewTool_BottomProject::MouseLeaveEvent() 
{
  return;
}

void McCadViewTool_BottomProject::MouseEnterEvent() 
{
  return;
}

