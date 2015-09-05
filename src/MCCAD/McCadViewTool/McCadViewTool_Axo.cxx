#include <McCadViewTool_Axo.ixx>

McCadViewTool_Axo::McCadViewTool_Axo(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myID = McCadTool_Axo;
}

Standard_Boolean McCadViewTool_Axo::IsNull()
{
	return Standard_False;
}

 void McCadViewTool_Axo::Destroy() 
{
}

 void McCadViewTool_Axo::Execute() 
{
}

 void McCadViewTool_Axo::UnExecute() 
{
}

 void McCadViewTool_Axo::Suspend() 
{
}

 void McCadViewTool_Axo::Resume() 
{
}

 void McCadViewTool_Axo::MousePressEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_Axo::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_Axo::MouseLeaveEvent() 
{
}

 void McCadViewTool_Axo::MouseEnterEvent() 
{
}

