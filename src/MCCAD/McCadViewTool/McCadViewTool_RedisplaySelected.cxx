#include <McCadViewTool_RedisplaySelected.ixx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <QMcCad_Application.h>
#include <QMcCadGeomeTree_TreeWidget.hxx>

McCadViewTool_RedisplaySelected::McCadViewTool_RedisplaySelected(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_RedispSelected;
}

Standard_Boolean McCadViewTool_RedisplaySelected::IsNull()
{
	return Standard_False;
}

 void McCadViewTool_RedisplaySelected::Destroy()
{
}

 void McCadViewTool_RedisplaySelected::Execute()
{
	 AIS_ListOfInteractive ioList;
	 QMcCad_Application::GetAppMainWin()->GetTreeWidget()->GetSelected(ioList);

	 Handle(AIS_InteractiveContext) theIC = myDoc->GetContext();

	 AIS_ListIteratorOfListOfInteractive it(ioList);

	 for(; it.More(); it.Next())
	 	theIC->Display(it.Value(), Standard_False);

	 theIC->UpdateCurrentViewer();

   
     Done();
}

 void McCadViewTool_RedisplaySelected::UnExecute()
{
}

 void McCadViewTool_RedisplaySelected::Suspend()
{
}

 void McCadViewTool_RedisplaySelected::Resume()
{
}

void McCadViewTool_RedisplaySelected::MousePressEvent(const QMouseEvent& e)
{
    
}

void McCadViewTool_RedisplaySelected::MouseReleaseEvent(const QMouseEvent& e)
{
     
}


 void McCadViewTool_RedisplaySelected::MouseLeaveEvent()
{
}

 void McCadViewTool_RedisplaySelected::MouseEnterEvent()
{
}

