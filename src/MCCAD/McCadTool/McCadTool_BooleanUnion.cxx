#include <McCadTool_BooleanUnion.hxx>

void McCadTool_Task::Destroy()
{

}

void McCadTool_BooleanUnion::Initialize(const Handle(McCadCom_CasDocument)& theDoc,
		const Handle(McCadCom_CasView)& theView,
		const Standard_Boolean theNeedSelect,
		const Standard_Boolean theNeedDlg,
		const Standard_Boolean theNeedRect,
		const McCadTool_State theState,
		const Standard_Boolean theUndoState,
		const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myNeedSelect = theNeedSelect;
	myNeedDlg = theNeedDlg;
	myNeedRect = theNeedRect;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_SolidUnion;
}


void McCadTool_BooleanUnion::Execute()
{
	//Start();

	Handle(AIS_InteractiveContext) theIC = myDoc->GetContext();

	for(theIC->InitCurrent(); theIC->MoreCurrent(); theIC->NextCurrent())
		theIC->Erase(theIC->Current(),0,1);

	theIC->UpdateCurrentViewer();


	Done();
}

Standard_Boolean McCadTool_BooleanUnion::IsNull()
{
	return Standard_True;		
}

void McCadTool_BooleanUnion::UnExecute()
{
	Start();
	Done();
}


void McCadTool_BooleanUnion::Suspend()
{
	Waiting();
}


void McCadTool_BooleanUnion::Resume()
{
	Start();
}


void McCadTool_BooleanUnion::SetState(const McCadTool_State theState)
{
	myState = theState;
}


void McCadTool_BooleanUnion::Start()
{
	SetState(McCadTool_Start);
}


void McCadTool_BooleanUnion::Running()
{
	SetState(McCadTool_Running);
}


void McCadTool_BooleanUnion::Waiting()
{
	SetState(McCadTool_Waiting);
}


void McCadTool_BooleanUnion::Done()
{
	SetState(McCadTool_Done);
}


McCadTool_State McCadTool_BooleanUnion::State() const
{
	return myState;
}


void McCadTool_BooleanUnion::MousePressEvent(const QMouseEvent& e)
{
}

void McCadTool_BooleanUnion::MouseReleaseEvent(const QMouseEvent& e)
{
}


void McCadTool_BooleanUnion::MouseMoveEvent(const QMouseEvent& e)
{
}


void McCadTool_BooleanUnion::MouseLeaveEvent()
{
}

void McCadTool_BooleanUnion::MouseEnterEvent()
{
}


void McCadTool_BooleanUnion::SetCanUndo(const Standard_Boolean theState)
{
	myUndoState = theState;
}

void McCadTool_BooleanUnion::SetCanRedo(const Standard_Boolean theState)
{
	myRedoState = theState;
}


Standard_Boolean McCadTool_BooleanUnion::CanUndo() const
{
	if (myUndoState)
		return Standard_True;
	else
		return Standard_False;
}


Standard_Boolean McCadTool_BooleanUnion::CanRedo() const
{
	if (myRedoState)
		return Standard_True;
	else
		return Standard_False;
}


void McCadTool_BooleanUnion::SetID(const McCadTool_BooleanUnionID theID)
{
	myID = theID;
}


McCadTool_BooleanUnionID McCadTool_BooleanUnion::GetID() const
{
	return myID;
}


Standard_Boolean McCadTool_BooleanUnion::NeedDlg() const
{
	return myNeedDlg;
}


void McCadTool_BooleanUnion::SetNeedDlg(const Standard_Boolean theState)
{
	myNeedDlg = theState;
}


Standard_Boolean McCadTool_BooleanUnion::NeedRect() const
{
	return myNeedRect;
}


void McCadTool_BooleanUnion::SetNeedRect(const Standard_Boolean theState)
{
	myNeedRect = theState;
}


Standard_Boolean McCadTool_BooleanUnion::NeedSelect() const
{
	return myNeedSelect;
}


void McCadTool_BooleanUnion::SetNeedSelect(const Standard_Boolean theState)
{
	myNeedSelect = theState;
}
