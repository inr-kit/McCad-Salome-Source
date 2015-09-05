#include <McCadViewTool_ZoomOut.ixx>
#include <V3d_View.hxx>

McCadViewTool_ZoomOut::McCadViewTool_ZoomOut(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_ZoomOut;
}

 void McCadViewTool_ZoomOut::Destroy()
{
	 return;
}

 Standard_Boolean McCadViewTool_ZoomOut::IsNull()
 {
	 return Standard_False;
 }

 void McCadViewTool_ZoomOut::Execute()
{
	 Handle(V3d_View) aView = myView->View();
	 aView->SetZoom(0.833);
	 Done();
}

 void McCadViewTool_ZoomOut::UnExecute()
{
	 return;
}

 void McCadViewTool_ZoomOut::Suspend()
{
	 return;
}

 void McCadViewTool_ZoomOut::Resume()
{
	 return;
}

 void McCadViewTool_ZoomOut::MousePressEvent(const McCadWin_MouseEvent& e)
{
	 return;
}

 void McCadViewTool_ZoomOut::MouseReleaseEvent(const McCadWin_MouseEvent& e)
{
	 return;
}

 void McCadViewTool_ZoomOut::MouseLeaveEvent()
{
	 return;
}

 void McCadViewTool_ZoomOut::MouseEnterEvent()
{
	 return;
}

