#include <McCadViewTool_RightProject.ixx>
#include <V3d_View.hxx>

McCadViewTool_RightProject::McCadViewTool_RightProject(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_RightProject;
}

 void McCadViewTool_RightProject::Destroy() 
{
	 return;
}
 
 Standard_Boolean McCadViewTool_RightProject::IsNull()
 {
 	return Standard_False;
 } 

 void McCadViewTool_RightProject::Execute() 
{
	 Handle(V3d_View) aView = myView->View();
	 aView->SetProj(V3d_Yneg);
	 aView->FitAll();
	 Done();
}

 void McCadViewTool_RightProject::UnExecute() 
{
	 return;
}

 void McCadViewTool_RightProject::Suspend() 
{
	 return;
}

 void McCadViewTool_RightProject::Resume() 
{
	 return;
}

 void McCadViewTool_RightProject::MousePressEvent(const McCadWin_MouseEvent& e) 
{
	 return;
}

 void McCadViewTool_RightProject::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
	 return;
}

 void McCadViewTool_RightProject::MouseLeaveEvent() 
{
	 return;
}

 void McCadViewTool_RightProject::MouseEnterEvent() 
{
	 return;
}

