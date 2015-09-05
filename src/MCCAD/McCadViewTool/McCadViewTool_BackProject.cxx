#include <McCadViewTool_BackProject.ixx>
#include <V3d_View.hxx>
#include <gp_Ax3.hxx>

McCadViewTool_BackProject::McCadViewTool_BackProject(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_BackProject;
}

void McCadViewTool_BackProject::Destroy()
{
	return;
}

void McCadViewTool_BackProject::Execute()
{
	Handle(V3d_View) aView = myView->View();
	aView->SetProj(V3d_Xneg);
	aView->FitAll();
	/*gp_Dir privDir(-1,0,0);
	gp_Pnt ori(0,0,0);
	gp_Ax3 privAx(ori, privDir);
	myView->View()->Viewer()->SetPrivilegedPlane(privAx);*/
	Done();
}

Standard_Boolean McCadViewTool_BackProject::IsNull()
{
	return Standard_False;
}

void McCadViewTool_BackProject::UnExecute()
{
	return;
}

void McCadViewTool_BackProject::Suspend()
{
	return;
}

void McCadViewTool_BackProject::Resume()
{
	return;
}

void McCadViewTool_BackProject::MousePressEvent(const McCadWin_MouseEvent& e)
{
	return;
}

void McCadViewTool_BackProject::MouseReleaseEvent(const McCadWin_MouseEvent& e)
{
	return;
}

void McCadViewTool_BackProject::MouseLeaveEvent()
{
	return;
}

void McCadViewTool_BackProject::MouseEnterEvent()
{
	return;
}

