#include <McCadViewTool_SetAntialiasing.ixx>
#include <V3d_View.hxx>

McCadViewTool_SetAntialiasing::McCadViewTool_SetAntialiasing(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_Antialiasing;
}

 void McCadViewTool_SetAntialiasing::Destroy() 
{
}

Standard_Boolean McCadViewTool_SetAntialiasing::IsNull()
{
 	return Standard_False;
}
 
 void McCadViewTool_SetAntialiasing::Execute() 
{
	 Handle(V3d_View) aView = myView->View();
	 
	 if(aView->Antialiasing())
	 	 aView->SetAntialiasingOff();
	 else
		 aView->SetAntialiasingOn();

	 aView->Redraw();
	 Done();
}

 void McCadViewTool_SetAntialiasing::UnExecute() 
{
}

 void McCadViewTool_SetAntialiasing::Suspend() 
{
}

 void McCadViewTool_SetAntialiasing::Resume() 
{
}

 void McCadViewTool_SetAntialiasing::MousePressEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_SetAntialiasing::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_SetAntialiasing::MouseLeaveEvent() 
{
}

 void McCadViewTool_SetAntialiasing::MouseEnterEvent() 
{
}

