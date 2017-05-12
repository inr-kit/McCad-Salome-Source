#include <McCadTool_TaskHistory.ixx>
#include <McCadTool_ListIteratorOfListOfTask.hxx>

McCadTool_TaskHistory::McCadTool_TaskHistory(const Standard_Integer theSize)
{
  mySize = theSize;
}

void McCadTool_TaskHistory::Destroy() 
{

}

void McCadTool_TaskHistory::SetSize(const Standard_Integer theSize) 
{
 mySize = theSize;
}

Standard_Integer McCadTool_TaskHistory::GetSize() const
{
  return mySize;
}

void McCadTool_TaskHistory::Log(const Handle(McCadTool_Task)& theTask) 
{
  if (theTask->CanUndo()) 
  {
      if( myUndoList.Extent() >= mySize) 
      {
		  // delete the oldest task;
		  myUndoList.RemoveFirst();
		  myUndoList.Append(theTask);
      }
      else 
      {
    	  myUndoList.Append(theTask);
      }
  }
  if(theTask->CanRedo())
  {
      if( myRedoList.Extent() >= mySize) 
      {
    	  myRedoList.RemoveFirst();
    	  myRedoList.Append(theTask);
      }
      else 
      {
    	  myRedoList.Append(theTask);
      }
  }
}

Standard_Boolean McCadTool_TaskHistory::CanUndo() const
{
  return myUndoList.Extent() > 0;
}

void McCadTool_TaskHistory::Undo() 
{
  if( myUndoList.IsEmpty())
	  return;

  Handle(McCadTool_Task)  theTask = myUndoList.Last();
  theTask->UnExecute();

  McCadTool_ListIteratorOfListOfTask  iter;
  Standard_Integer cnt(0);
  for(iter.Initialize(myUndoList); iter.More(); iter.Next())
  {
	  cout << ".\n";
	  if(cnt++ == myUndoList.Extent())
		  myUndoList.Remove(iter);
  }

  if(theTask->CanRedo())
  {
	  if( myRedoList.Extent() >= mySize)
	  {
		  myRedoList.RemoveFirst();
		  myRedoList.Append(theTask);
	  }
	  else
	  {
		  myRedoList.Append(theTask);
	  }
  }
}

void McCadTool_TaskHistory::Undo(const Standard_Integer theIndex) 
{
  Handle(McCadTool_Task) theTask;

  Standard_Integer i=0;
  Standard_Boolean found = Standard_False;

  McCadTool_ListIteratorOfListOfTask  iter;
  for (iter.Initialize(myUndoList); iter.More(); iter.Next())
   {
     i++;
     if(i == theIndex)
     {
		 theTask = iter.Value();
		 found = Standard_True;
     }
   }

  if(found)
  {
      theTask->UnExecute();
      
      McCadTool_ListIteratorOfListOfTask  iter;
      for (iter.Initialize(myUndoList); iter.More(); iter.Next());
      myUndoList.Remove(iter);
      if(theTask->CanRedo())
      {
		  if( myRedoList.Extent() >= mySize)
		  {
			  myRedoList.RemoveFirst();
			  myRedoList.Append(theTask);
		  }
		  else
		  {
			  myRedoList.Append(theTask);
		  }
		}
    }
}

Standard_Boolean McCadTool_TaskHistory::CanRedo() const
{
   return myRedoList.Extent() > 0;
}

void McCadTool_TaskHistory::Redo() 
{

  if( myRedoList.IsEmpty())
	  return;

  Handle(McCadTool_Task) theTask = myRedoList.Last();
  theTask->Execute();

  McCadTool_ListIteratorOfListOfTask  iter;
  for (iter.Initialize(myUndoList); iter.More(); iter.Next());
  myRedoList.Remove(iter);

  if(theTask->CanUndo())
  {
	  if( myUndoList.Extent() >= mySize)
	  {
		  myUndoList.RemoveFirst();
		  myUndoList.Append(theTask);
	  }
      else 
      {
    	  myUndoList.Append(theTask);
      }
  }
}

void McCadTool_TaskHistory::Redo(const Standard_Integer theIndex) 
{
  Handle(McCadTool_Task) theTask;
  Standard_Integer i=0;
  Standard_Boolean found = Standard_False;

  McCadTool_ListIteratorOfListOfTask  iter;
  for (iter.Initialize(myRedoList); iter.More(); iter.Next())
   {
     i++;
     if(i == theIndex)
       { 
	 theTask = iter.Value();
	 found = Standard_True;
       } 
     
   }

  if(found)
    {
      theTask->Execute();
      McCadTool_ListIteratorOfListOfTask  iter;
      for (iter.Initialize(myUndoList); iter.More(); iter.Next());
      myRedoList.Remove(iter);
      if(theTask->CanUndo())
	{
	  if( myUndoList.Extent() >= mySize) 
	    {
	      myUndoList.RemoveFirst();
	      myUndoList.Append(theTask);
	    }
	  else 
	    {
	      myUndoList.Append(theTask);
	    }
	}
    }

}

void McCadTool_TaskHistory::Clear() 
{
  ClearUndoList();
  ClearRedoList();
}

void McCadTool_TaskHistory::ClearUndoList() 
{
  myUndoList.Clear();
}

void McCadTool_TaskHistory::ClearRedoList() 
{
  myRedoList.Clear();
}

