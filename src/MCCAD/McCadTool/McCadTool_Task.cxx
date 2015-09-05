#include <McCadTool_Task.ixx>

void McCadTool_Task::Destroy()
{

}

void McCadTool_Task::Initialize(const Handle(McCadCom_CasDocument)& theDoc,
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
	myID = McCadTool_NoID;
}


void McCadTool_Task::Execute()
{
	Start();
	Done();
}

Standard_Boolean McCadTool_Task::IsNull() 
{
	return Standard_True;		
}

void McCadTool_Task::UnExecute()
{
	Start();
	Done();
}


void McCadTool_Task::Suspend()
{
	Waiting();
}


void McCadTool_Task::Resume()
{
	Start();
}


void McCadTool_Task::SetState(const McCadTool_State theState)
{
	myState = theState;
}


void McCadTool_Task::Start()
{
	SetState(McCadTool_Start);
}


void McCadTool_Task::Running()
{
	SetState(McCadTool_Running);
}


void McCadTool_Task::Waiting()
{
	SetState(McCadTool_Waiting);
}


void McCadTool_Task::Done()
{
	SetState(McCadTool_Done);
}


McCadTool_State McCadTool_Task::State() const
{
	return myState;
}


void McCadTool_Task::MousePressEvent(const QMouseEvent& e)
{
}

void McCadTool_Task::MouseReleaseEvent(const QMouseEvent& e)
{
}


void McCadTool_Task::MouseMoveEvent(const QMouseEvent& e)
{
}


void McCadTool_Task::MouseLeaveEvent()
{
}

void McCadTool_Task::MouseEnterEvent()
{
}


void McCadTool_Task::SetCanUndo(const Standard_Boolean theState)
{
	myUndoState = theState;
}

void McCadTool_Task::SetCanRedo(const Standard_Boolean theState)
{
	myRedoState = theState;
}


Standard_Boolean McCadTool_Task::CanUndo() const
{
	if (myUndoState)
		return Standard_True;
	else
		return Standard_False;
}


Standard_Boolean McCadTool_Task::CanRedo() const
{
	if (myRedoState)
		return Standard_True;
	else
		return Standard_False;
}


void McCadTool_Task::SetID(const McCadTool_TaskID theID)
{
	myID = theID;
}


McCadTool_TaskID McCadTool_Task::GetID() const
{
	return myID;
}


Standard_Boolean McCadTool_Task::NeedDlg() const
{
	return myNeedDlg;
}


void McCadTool_Task::SetNeedDlg(const Standard_Boolean theState)
{
	myNeedDlg = theState;
}


Standard_Boolean McCadTool_Task::NeedRect() const
{
	return myNeedRect;
}


void McCadTool_Task::SetNeedRect(const Standard_Boolean theState)
{
	myNeedRect = theState;
}


Standard_Boolean McCadTool_Task::NeedSelect() const
{
	return myNeedSelect;
}


void McCadTool_Task::SetNeedSelect(const Standard_Boolean theState)
{
	myNeedSelect = theState;
}
