#include <McCadViewTool_Dump.ixx>
#include <V3d_View.hxx>

McCadViewTool_Dump::McCadViewTool_Dump(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
  myDoc = theDoc;
  myView = theView;
  myState = theState;
  myUndoState = theUndoState;
  myRedoState = theRedoState;
  myID = McCadTool_Hardcopy;
}

 void McCadViewTool_Dump::Destroy() 
{
 return;
}

Standard_Boolean McCadViewTool_Dump::IsNull()
{
 	return Standard_False;
}
 
 void McCadViewTool_Dump::Execute() 
{
  Handle(V3d_View) aView = myView->View(); 
  aView->Redraw();
  Done();
}

 void McCadViewTool_Dump::UnExecute() 
{
 return;
}

 void McCadViewTool_Dump::Suspend() 
{
 return;
}

 void McCadViewTool_Dump::Resume() 
{
 return;
}

 void McCadViewTool_Dump::MousePressEvent(const McCadWin_MouseEvent& e) 
{
 return;
}

 void McCadViewTool_Dump::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
 return;
}

 void McCadViewTool_Dump::MouseLeaveEvent() 
{
return;
 
}

 void McCadViewTool_Dump::MouseEnterEvent() 
{
 return;
}

