#include <McCadViewTool_LeftProject.ixx>
#include <V3d_View.hxx>

McCadViewTool_LeftProject::McCadViewTool_LeftProject(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_LeftProject;
}

 void McCadViewTool_LeftProject::Destroy() 
{
	 return;
}

 Standard_Boolean McCadViewTool_LeftProject::IsNull()
 {
 	return Standard_False;
 } 
 
 void McCadViewTool_LeftProject::Execute() 
{
	 Handle(V3d_View) aView = myView->View();
	 aView->SetProj(V3d_Ypos);
	 aView->FitAll();
	 Done();
}

 void McCadViewTool_LeftProject::UnExecute() 
{
	 return;
}

 void McCadViewTool_LeftProject::Suspend() 
{
	 return;
}

 void McCadViewTool_LeftProject::Resume() 
{
	 return;
}

 void McCadViewTool_LeftProject::MousePressEvent(const McCadWin_MouseEvent& e) 
{
	 return;
}

 void McCadViewTool_LeftProject::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
	 return;
}

 void McCadViewTool_LeftProject::MouseLeaveEvent() 
{
	 return;
}

 void McCadViewTool_LeftProject::MouseEnterEvent() 
{
	 return;
}

