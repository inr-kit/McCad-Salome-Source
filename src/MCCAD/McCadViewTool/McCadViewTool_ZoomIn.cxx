#include <McCadViewTool_ZoomIn.ixx>
#include <V3d_View.hxx>

McCadViewTool_ZoomIn::McCadViewTool_ZoomIn(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_ZoomIn;
}

 void McCadViewTool_ZoomIn::Destroy()
{
	 return;
}

 Standard_Boolean McCadViewTool_ZoomIn::IsNull()
 {
	 return Standard_False;
 }

 void McCadViewTool_ZoomIn::Execute()
{
	 Handle(V3d_View) aView = myView->View();
	 aView->SetZoom(1.2);
	 Done();
}

 void McCadViewTool_ZoomIn::UnExecute()
{
	 return;
}

 void McCadViewTool_ZoomIn::Suspend()
{
	 return;
}

 void McCadViewTool_ZoomIn::Resume()
{
	 return;
}

 void McCadViewTool_ZoomIn::MousePressEvent(const McCadWin_MouseEvent& e)
{
	 return;
}

 void McCadViewTool_ZoomIn::MouseReleaseEvent(const McCadWin_MouseEvent& e)
{
	 return;
}

 void McCadViewTool_ZoomIn::MouseLeaveEvent()
{
	 return;
}

 void McCadViewTool_ZoomIn::MouseEnterEvent()
{
	 return;
}

