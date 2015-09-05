#include <McCadViewTool_Redraw.ixx>
#include <V3d_View.hxx>

McCadViewTool_Redraw::McCadViewTool_Redraw(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
  myDoc = theDoc;
  myView = theView;
  myState = theState;
  myUndoState = theUndoState;
  myRedoState = theRedoState;
  myID = McCadTool_Redraw;
}

void McCadViewTool_Redraw::Destroy() 
{
  return;
}

Standard_Boolean McCadViewTool_Redraw::IsNull()
{
	return Standard_False;
}

void McCadViewTool_Redraw::Execute() 
{
  Handle(V3d_View) aView = myView->View();
  aView->Redraw();
  Done();
}

void McCadViewTool_Redraw::UnExecute() 
{
  return;
}

void McCadViewTool_Redraw::Suspend() 
{
  return;
}

void McCadViewTool_Redraw::Resume() 
{
  return;
}

void McCadViewTool_Redraw::MousePressEvent(const McCadWin_MouseEvent& e) 
{
  return;
}

void McCadViewTool_Redraw::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{ 
 return;
 
}

void McCadViewTool_Redraw::MouseLeaveEvent() 
{
  return;
}

void McCadViewTool_Redraw::MouseEnterEvent() 
{
  return;
}

