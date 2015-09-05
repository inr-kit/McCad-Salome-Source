#include <McCadViewTool_Reset.ixx>

McCadViewTool_Reset::McCadViewTool_Reset(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
}

Standard_Boolean McCadViewTool_Reset::IsNull()
{
	return Standard_False;
}

 void McCadViewTool_Reset::Destroy() 
{
}

 void McCadViewTool_Reset::Execute() 
{
	 Done();
}

 void McCadViewTool_Reset::UnExecute() 
{
}

 void McCadViewTool_Reset::Suspend() 
{
}

 void McCadViewTool_Reset::Resume() 
{
}

 void McCadViewTool_Reset::MousePressEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_Reset::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_Reset::MouseLeaveEvent() 
{
}

 void McCadViewTool_Reset::MouseEnterEvent() 
{
}

