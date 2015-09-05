#include <McCadViewTool_Rotate.ixx>
#include <V3d_View.hxx>

McCadViewTool_Rotate::McCadViewTool_Rotate(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_Rotate;
}

 void McCadViewTool_Rotate::Destroy() 
{
	 return;
}

Standard_Boolean McCadViewTool_Rotate::IsNull()
{
	return Standard_False;
}
 
 void McCadViewTool_Rotate::Execute() 
{
     /*Done();*/
}

 void McCadViewTool_Rotate::UnExecute() 
{
	 return;
}

 void McCadViewTool_Rotate::Suspend() 
{
	 return;
}

 void McCadViewTool_Rotate::Resume() 
{
	 return;
}

 void McCadViewTool_Rotate::MouseMoveEvent(const QMouseEvent& e) 
 {
 	 return;
 }
 
 void McCadViewTool_Rotate::MousePressEvent(const QMouseEvent& e) 
{
	 return;
}

 void McCadViewTool_Rotate::MouseReleaseEvent(const QMouseEvent& e) 
{
	 return;
}

 void McCadViewTool_Rotate::MouseLeaveEvent() 
{
	 return;
}

 void McCadViewTool_Rotate::MouseEnterEvent() 
{
	 return;
}

