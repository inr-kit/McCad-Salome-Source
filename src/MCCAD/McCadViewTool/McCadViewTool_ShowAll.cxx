#include <McCadViewTool_ShowAll.ixx>
#include <V3d_View.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>


McCadViewTool_ShowAll::McCadViewTool_ShowAll(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	 myDoc = theDoc;
	 myView = theView;
	 myState = theState;
	 myUndoState = theUndoState;
	 myRedoState = theRedoState;
	 myID = McCadTool_ShowAll;
}

void McCadViewTool_ShowAll::Destroy()
{
 return;
}

Standard_Boolean McCadViewTool_ShowAll::IsNull()
{
	return Standard_False;
}

void McCadViewTool_ShowAll::Execute()
{
	Handle(AIS_InteractiveContext) theIC = myDoc->GetContext();

	AIS_ListOfInteractive ioList;
//qiu 	theIC->ObjectsInCollector(ioList);
    //qiu replace it with a function with same function specifications
    theIC->ErasedObjects(ioList);

	AIS_ListIteratorOfListOfInteractive it(ioList);

	for(; it.More(); it.Next())
		theIC->Display(it.Value(), Standard_False);

	theIC->UpdateCurrentViewer();
	Done();
}

void McCadViewTool_ShowAll::UnExecute()
{
}

void McCadViewTool_ShowAll::Suspend()
{
  return;
}

void McCadViewTool_ShowAll::Resume()
{

  return;
}

void McCadViewTool_ShowAll::MousePressEvent(const McCadWin_MouseEvent& e)
{
   return;
}

void McCadViewTool_ShowAll::MouseReleaseEvent(const McCadWin_MouseEvent& e)
{
  return;
}

void McCadViewTool_ShowAll::MouseLeaveEvent()
{
  return;
}

void McCadViewTool_ShowAll::MouseEnterEvent()
{
 return;
}

