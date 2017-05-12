#include <McCadTool_TaskMaster.ixx>

McCadTool_TaskMaster::McCadTool_TaskMaster()
{
	myView = NULL;
	myDefaultTask = new McCadTool_Task;
	myCurrentTask = new McCadTool_Task;
	myTaskHistory = new McCadTool_TaskHistory(10);

}

McCadTool_TaskMaster::McCadTool_TaskMaster(const Handle(McCadCom_CasView)& theView)
{
	myView = theView;
	myDefaultTask = new McCadTool_Task;
	myCurrentTask = new McCadTool_Task;
	myTaskHistory = new McCadTool_TaskHistory(10);
}

void McCadTool_TaskMaster::Destroy()
{
}

void McCadTool_TaskMaster::SetView(const Handle(McCadCom_CasView)& theView)
{
    myView = theView;
}

Handle(McCadCom_CasView) McCadTool_TaskMaster::GetView() const
{
	return myView;
}

void McCadTool_TaskMaster::SetDefaultTask(const Handle(McCadTool_Task)& theTask)
{
	myDefaultTask = theTask;
}

Handle(McCadTool_Task) McCadTool_TaskMaster::GetDefaultTask() const
{
	return myDefaultTask;

}

void McCadTool_TaskMaster::SetCurrentTask(const Handle(McCadTool_Task)& theTask)
{
	myCurrentTask = theTask;
}

Handle(McCadTool_Task) McCadTool_TaskMaster::GetCurrentTask() const
{
	return myCurrentTask;
}

void McCadTool_TaskMaster::SetTaskHistory(const Handle(McCadTool_TaskHistory)& theTaskHist)
{
	myTaskHistory = theTaskHist;
}

Handle(McCadTool_TaskHistory) McCadTool_TaskMaster::GetTaskHistory() const
{
	return myTaskHistory;
}

void McCadTool_TaskMaster::Execute()
{
	if (myView == NULL)
	{
		cout << "Error: No view for the Task!" << endl;
		return;
	}
	if (myCurrentTask == NULL)
	{
		cout << "Error: No Task for Execution!" << endl;
		return;
	}

	if (State() != McCadTool_Done) // current task is not executed!
	{
		myCurrentTask->Execute();
		myTaskHistory->Log(myCurrentTask);
	}
	else // current command is executed use History!
	{
		if (myTaskHistory->CanRedo())
			myTaskHistory->Redo();
	}
}

void McCadTool_TaskMaster::UnExecute()
{
	if (myView == NULL)
	{
		cout << "Error: No view for the Task" << endl;
		return;
	}
	if (myTaskHistory->CanUndo())
		myTaskHistory->Undo();
	else
		return;
}

void McCadTool_TaskMaster::Suspend()
{

	if (myView != NULL && myCurrentTask != NULL)
		myCurrentTask->Suspend();
	else
		cout << "Error: No view for the Task" << endl;

}

void McCadTool_TaskMaster::Resume()
{
	if (myView != NULL && myCurrentTask != NULL)
		myCurrentTask->Resume();
	else
		cout << "Error: No view for the Task" << endl;
}

void McCadTool_TaskMaster::SetState(const McCadTool_State theState)
{
	if (myView != NULL && myCurrentTask != NULL)
		myCurrentTask->SetState(theState);
	else
		cout << "Error: No view for the Task" << endl;
}

McCadTool_State McCadTool_TaskMaster::State() const
{
	McCadTool_State theState= McCadTool_None;
	if (myView != NULL && myCurrentTask != NULL)
		theState = myCurrentTask->State();
	else
		cout << "Error: No view for the Task" << endl;
	return theState;
}

void McCadTool_TaskMaster::MousePressEvent(const QMouseEvent& e)
{
	if (myView != NULL)
		return myCurrentTask->MousePressEvent(e);
	else
		cout << "Error: No view for the Task" << endl;
}

void McCadTool_TaskMaster::MouseReleaseEvent(const QMouseEvent& e)
{
	if (myView != NULL)
		return myCurrentTask->MouseReleaseEvent(e);
	else
		cout << "Error: No view for the Task" << endl;
}

void McCadTool_TaskMaster::MouseMoveEvent(const QMouseEvent& e)
{
	if (myView != NULL)
		return myCurrentTask->MouseMoveEvent(e);
	else
		cout << "Error: No view for the Task" << endl;
}

void McCadTool_TaskMaster::MouseLeaveEvent()
{
	if (myView != NULL)
		return myCurrentTask->MouseLeaveEvent();
	else
		cout << "Error: No view for the Task" << endl;
}

void McCadTool_TaskMaster::MouseEnterEvent()
{
	if (myView != NULL)
		return myCurrentTask->MouseEnterEvent();
	else
		cout << "Error: No view for the Task" << endl;
}

