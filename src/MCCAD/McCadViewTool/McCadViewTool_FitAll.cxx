#include <McCadViewTool_FitAll.ixx>
#include <V3d_View.hxx>

McCadViewTool_FitAll::McCadViewTool_FitAll(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
 myDoc = theDoc;
 myView = theView;
 myState = theState;
 myUndoState = theUndoState;
 myRedoState = theRedoState;
 myID = McCadTool_FitAll;
}

void McCadViewTool_FitAll::Destroy() 
{
 return;
}

Standard_Boolean McCadViewTool_FitAll::IsNull()
{
	return Standard_False;
}

void McCadViewTool_FitAll::Execute() 
{
  Handle(V3d_View) aView = myView->View();
  aView->FitAll();
  aView->Redraw();
  Done();
}

void McCadViewTool_FitAll::UnExecute() 
{
  return;
}

void McCadViewTool_FitAll::Suspend() 
{
  return;
}

void McCadViewTool_FitAll::Resume() 
{
  return;

}

void McCadViewTool_FitAll::MousePressEvent(const McCadWin_MouseEvent& e) 
{
   return;
}

void McCadViewTool_FitAll::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
  return;
}

void McCadViewTool_FitAll::MouseLeaveEvent() 
{
  return;
}

void McCadViewTool_FitAll::MouseEnterEvent() 
{
 return;
}

