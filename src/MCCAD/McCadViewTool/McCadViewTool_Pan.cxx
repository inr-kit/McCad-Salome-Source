#include <McCadViewTool_Pan.ixx>
#include <V3d_View.hxx>

McCadViewTool_Pan::McCadViewTool_Pan(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_Pan;
	myXmin = 0;
	myXmax = 0;
	myYmin = 0;
	myYmax = 0;
}

 void McCadViewTool_Pan::Destroy()
{
}


Standard_Boolean McCadViewTool_Pan::IsNull()
{
	return Standard_False;
}

 void McCadViewTool_Pan::Execute()
{
	 Suspend();
}

 void McCadViewTool_Pan::UnExecute()
{
}

 void McCadViewTool_Pan::Suspend()
{
}

 void McCadViewTool_Pan::Resume()
{
}

 void McCadViewTool_Pan::MousePressEvent(const QMouseEvent& e)
{
	 QPoint aPoint = e.pos();

 	 myXmin = aPoint.x();
  	 myYmin = aPoint.y();
  	 myXmax = aPoint.x();
  	 myYmax = aPoint.y();    
}

 void McCadViewTool_Pan::MouseReleaseEvent(const QMouseEvent& e)
{
   	 Done();
}

 void McCadViewTool_Pan::MouseMoveEvent(const QMouseEvent& e)
{
	 QPoint aPoint = e.pos();

     Handle(V3d_View) aView = myView->View();
     aView->Pan(aPoint.x() - myXmax, myYmax - aPoint.y());

}

 void McCadViewTool_Pan::MouseLeaveEvent()
{
}

 void McCadViewTool_Pan::MouseEnterEvent()
{
}

