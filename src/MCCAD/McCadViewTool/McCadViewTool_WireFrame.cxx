#include <McCadViewTool_WireFrame.ixx>
#include <V3d_View.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <AIS_Shape.hxx>
#include <QMcCad_Application.h>
#include <QMcCad_Editor.h>

McCadViewTool_WireFrame::McCadViewTool_WireFrame(const Handle(McCadCom_CasDocument)& theDoc, const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
  myDoc = theDoc;
  myView = theView;
  myState = theState;
  myUndoState = theUndoState;
  myRedoState = theRedoState;
  myID = McCadTool_FlatShade;
}

 void McCadViewTool_WireFrame::Destroy() 
{
}

 
Standard_Boolean McCadViewTool_WireFrame::IsNull()
{
 	return Standard_False;
} 
 
void McCadViewTool_WireFrame::Execute()
{	
     Handle(AIS_InteractiveContext) theContext = myDoc->GetContext();
     Standard_Boolean haveSelection = Standard_False;

     for (theContext->InitCurrent(); theContext->MoreCurrent(); theContext->NextCurrent() )
     {
         theContext->SetDisplayMode(theContext->Current(), 0, Standard_False);
         haveSelection = Standard_True;
     }

     if(!haveSelection)
     {
         AIS_ListOfInteractive aList;
         theContext->DisplayedObjects( aList );
         AIS_ListIteratorOfListOfInteractive aListIterator;

         for ( aListIterator.Initialize( aList ); aListIterator.More(); aListIterator.Next() )
         {
             theContext->SetDisplayMode(aListIterator.Value(), 0, Standard_False);
         }
     }

     //theContext->OpenLocalContext();
     //theContext->ActivateStandardMode(TopAbs_SOLID);

     theContext->UpdateCurrentViewer();
     Done();
}

 void McCadViewTool_WireFrame::UnExecute() 
{
}

 void McCadViewTool_WireFrame::Suspend() 
{
}

 void McCadViewTool_WireFrame::Resume() 
{
}

 void McCadViewTool_WireFrame::MousePressEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_WireFrame::MouseReleaseEvent(const McCadWin_MouseEvent& e) 
{
}

 void McCadViewTool_WireFrame::MouseLeaveEvent() 
{
}

 void McCadViewTool_WireFrame::MouseEnterEvent() 
{
}

