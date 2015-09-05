#include <McCadViewTool_Zoom.ixx>
#include <V3d_View.hxx>

McCadViewTool_Zoom::McCadViewTool_Zoom(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_ZoomWindow;	
	
	myXmin = 0;
	myXmax = 0;
	myYmin = 1024;
	myYmax = 768;
}

 void McCadViewTool_Zoom::Destroy() 
{
}
 
Standard_Boolean McCadViewTool_Zoom::IsNull()
{
 	 return Standard_False;
}


void McCadViewTool_Zoom::Execute() 
{
	 Handle(V3d_View) aView = myView->View();
 	 aView->WindowFit(myXmin, myYmin, myXmax , myYmax);
 	 Done();
}

 void McCadViewTool_Zoom::UnExecute() 
{
}

 void McCadViewTool_Zoom::Suspend() 
{
	 SetState(McCadTool_Waiting);
}

 void McCadViewTool_Zoom::Resume() 
{
}

 void McCadViewTool_Zoom::MousePressEvent(const QMouseEvent& e) 
{
	 QPoint aPoint = e.pos();
	 
	 myXmin = aPoint.x();
 	 myYmin = aPoint.y();
 	 myXmax = aPoint.x();
 	 myYmax = aPoint.y();
}

 void McCadViewTool_Zoom::MouseReleaseEvent(const QMouseEvent& e) 
{
	 QPoint aPoint = e.pos();
	 	 
	 myXmax = aPoint.x();
	 myYmax = aPoint.y();
	 
	 //Execute();
}

 void McCadViewTool_Zoom::MouseLeaveEvent() 
{
}

 void McCadViewTool_Zoom::MouseEnterEvent() 
{
}
 
Standard_Boolean McCadViewTool_Zoom::NeedRect() const
{
	return Standard_True;
}

