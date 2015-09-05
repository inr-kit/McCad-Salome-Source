#include <McCadViewTool_SetHiddenLine.ixx>
#include <V3d_View.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_InteractiveObject.hxx>

McCadViewTool_SetHiddenLine::McCadViewTool_SetHiddenLine(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	myView = theView;
	myState = theState;
	myUndoState = theUndoState;
	myRedoState = theRedoState;
	myID = McCadTool_SetHiddenLine;
}

 void McCadViewTool_SetHiddenLine::Destroy() 
{
}

 Standard_Boolean McCadViewTool_SetHiddenLine::IsNull()
 {
	 return Standard_False;
 }
 
 void McCadViewTool_SetHiddenLine::Execute() 
{
//	 Handle(AIS_InteractiveContext) theContext = myDoc->GetContext();
//	 Handle(V3d_View) aView = myView->View();
//	 /*if(theContext->DrawHiddenLine())
//	 {
//		 theContext->DisableDrawHiddenLine();
//		 cout << "Setting Hidden Line OFF \n";
//	 }
//	 else
//	 {
//		 theContext->EnableDrawHiddenLine();
//		 cout << "Setting Hidden Line ON \n";
//	 }	*/
		 
//	 aView->SetComputedMode(!aView->ComputedMode());
//	 aView->Redraw();
//	 Done();

     Handle(AIS_InteractiveContext) theContext = myDoc->GetContext();
     Standard_Boolean haveSelection = Standard_False;

     //if(myView->View()->ComputedMode())
     myView->View()->SetComputedMode(Standard_False);

     for (theContext->InitCurrent(); theContext->MoreCurrent(); theContext->NextCurrent() )
     {
         theContext->SetDisplayMode(theContext->Current(), 2, Standard_False);
         haveSelection = Standard_True;
     }

     if(!haveSelection)
     {
         AIS_ListOfInteractive aList;
         theContext->DisplayedObjects( aList );
         AIS_ListIteratorOfListOfInteractive aListIterator;

         for ( aListIterator.Initialize( aList ); aListIterator.More(); aListIterator.Next() )
             theContext->SetDisplayMode(aListIterator.Value(), 2, Standard_False);
     }

     theContext->UpdateCurrentViewer();
     Done();



}

 void McCadViewTool_SetHiddenLine::UnExecute() 
{
}

 void McCadViewTool_SetHiddenLine::Suspend() 
{
}

 void McCadViewTool_SetHiddenLine::Resume() 
{
}

 void McCadViewTool_SetHiddenLine::MousePressEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_SetHiddenLine::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_SetHiddenLine::MouseLeaveEvent() 
{
}

 void McCadViewTool_SetHiddenLine::MouseEnterEvent() 
{
}

