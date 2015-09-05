#include <McCadViewTool_FrontProject.ixx>
#include <V3d_View.hxx>

McCadViewTool_FrontProject::McCadViewTool_FrontProject(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_FrontProject;
}

 void McCadViewTool_FrontProject::Destroy() 
{
	 return;
}
 
Standard_Boolean McCadViewTool_FrontProject::IsNull()
{
	return Standard_False;
} 

 void McCadViewTool_FrontProject::Execute() 
{
	 Handle(V3d_View) aView = myView->View();
	 aView->SetProj(V3d_Xpos);
	 aView->FitAll();
	 Done();
}

 void McCadViewTool_FrontProject::UnExecute() 
{
	 return;
}

 void McCadViewTool_FrontProject::Suspend() 
{
	 return;
}

 void McCadViewTool_FrontProject::Resume() 
{
	 return;
}

 void McCadViewTool_FrontProject::MousePressEvent(const McCadWin_MouseEvent& e) 
{
	 return;
}

 void McCadViewTool_FrontProject::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
	 return;
}

 void McCadViewTool_FrontProject::MouseLeaveEvent() 
{
	 return;
}

 void McCadViewTool_FrontProject::MouseEnterEvent() 
{
	 return;
}

