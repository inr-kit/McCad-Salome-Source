#include <McCadViewTool_Transparency.ixx>

McCadViewTool_Transparency::McCadViewTool_Transparency(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myID = McCadTool_ShapeTransparency;
}

Standard_Boolean McCadViewTool_Transparency::IsNull()
{
	return Standard_False;
}

 void McCadViewTool_Transparency::Destroy() 
{
}

 void McCadViewTool_Transparency::Execute() 
{
}

 void McCadViewTool_Transparency::UnExecute() 
{
}

 void McCadViewTool_Transparency::Suspend() 
{
}

 void McCadViewTool_Transparency::Resume() 
{
}

 void McCadViewTool_Transparency::MousePressEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_Transparency::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_Transparency::MouseLeaveEvent() 
{
}

 void McCadViewTool_Transparency::MouseEnterEvent() 
{
}

