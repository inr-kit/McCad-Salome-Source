#include <McCadViewTool_Axis.ixx>
#include <V3d_View.hxx>

McCadViewTool_Axis::McCadViewTool_Axis(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	 myDoc = theDoc;
	 myView = theView;
	 myState = theState;
	 myUndoState = theUndoState;
	 myRedoState = theRedoState;
	 myID = McCadTool_Axis;
}

void McCadViewTool_Axis::Destroy()
{
 return;
}

Standard_Boolean McCadViewTool_Axis::IsNull()
{
	return Standard_False;
}

void McCadViewTool_Axis::Execute()
{
  Handle(V3d_View) aView = myView->View();

//#ifdef OCC_PATCHED
		aView->TriedronDisplay( Aspect_TOTP_LEFT_LOWER, Quantity_NOC_YELLOW, 0.1, V3d_ZBUFFER );
/*#else
		aView->TriedronDisplay( Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1, V3d_WIREFRAME );
#endif*/

  aView->Redraw();
  Done();
}

void McCadViewTool_Axis::UnExecute()
{
	  Handle(V3d_View) aView = myView->View();
	  aView->TriedronErase();
	  aView->Redraw();
	  Done();
}

void McCadViewTool_Axis::Suspend()
{
  return;
}

void McCadViewTool_Axis::Resume()
{

  return;
}

void McCadViewTool_Axis::MousePressEvent(const McCadWin_MouseEvent& e)
{
   return;
}

void McCadViewTool_Axis::MouseReleaseEvent(const McCadWin_MouseEvent& e)
{
  return;
}

void McCadViewTool_Axis::MouseLeaveEvent()
{
  return;
}

void McCadViewTool_Axis::MouseEnterEvent()
{
 return;
}

