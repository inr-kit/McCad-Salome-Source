#include <McCadViewTool_HideSelected.ixx>
#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListOfInteractive.hxx>

McCadViewTool_HideSelected::McCadViewTool_HideSelected(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	 myDoc = theDoc;
	 myView = theView;
	 myState = theState;
	 myUndoState = theUndoState;
	 myRedoState = theRedoState;
	 myID = McCadTool_HideSelected;
}

void McCadViewTool_HideSelected::Destroy()
{
 return;
}

Standard_Boolean McCadViewTool_HideSelected::IsNull()
{
	return Standard_False;
}

void McCadViewTool_HideSelected::Execute()
{
	Handle(AIS_InteractiveContext) theIC = myDoc->GetContext();

	for(theIC->InitCurrent(); theIC->MoreCurrent(); theIC->NextCurrent())
//qiu		theIC->Erase(theIC->Current(),Standard_False,Standard_True);
        theIC->Erase(theIC->Current(),Standard_False);

	theIC->UpdateCurrentViewer();

	Done();
}

void McCadViewTool_HideSelected::UnExecute()
{
}

void McCadViewTool_HideSelected::Suspend()
{
  return;
}

void McCadViewTool_HideSelected::Resume()
{

  return;
}

void McCadViewTool_HideSelected::MousePressEvent(const McCadWin_MouseEvent& e)
{
   return;
}

void McCadViewTool_HideSelected::MouseReleaseEvent(const McCadWin_MouseEvent& e)
{
  return;
}

void McCadViewTool_HideSelected::MouseLeaveEvent()
{
  return;
}

void McCadViewTool_HideSelected::MouseEnterEvent()
{
 return;
}

