#include <McCadViewTool_TopProject.ixx>
#include <V3d_View.hxx>

McCadViewTool_TopProject::McCadViewTool_TopProject(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_FrontProject;
}

 void McCadViewTool_TopProject::Destroy() 
{
	 return;
}

 Standard_Boolean McCadViewTool_TopProject::IsNull()
 {
 	return Standard_False;
 } 

 
 void McCadViewTool_TopProject::Execute() 
{
	 Handle(V3d_View) aView = myView->View();
	 aView->SetProj(V3d_Zpos);
	 aView->FitAll();
	 Done();
}

 void McCadViewTool_TopProject::UnExecute() 
{
	 return;
}

 void McCadViewTool_TopProject::Suspend() 
{
	 return;
}

 void McCadViewTool_TopProject::Resume() 
{
	 return;
}

 void McCadViewTool_TopProject::MousePressEvent(const McCadWin_MouseEvent& e) 
{
	 return;
}

 void McCadViewTool_TopProject::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
	 return;
}

 void McCadViewTool_TopProject::MouseLeaveEvent() 
{
	 return;
}

 void McCadViewTool_TopProject::MouseEnterEvent() 
{
	 return;
}

