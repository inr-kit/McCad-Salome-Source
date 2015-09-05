#include <McCadViewTool_Delete.ixx>
#include <V3d_View.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <QMcCad_Application.h>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <TDocStd_Document.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <AIS_InteractiveObject.hxx>
#include <TDF_Tool.hxx>
#include <TDF_LabelSequence.hxx>
#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <XCAFDoc_ShapeTool.hxx>

McCadViewTool_Delete::McCadViewTool_Delete(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
 myDoc = theDoc;
 myView = theView;
 myState = theState;
 myUndoState = theUndoState;
 myRedoState = theRedoState;
 myID = McCadTool_ModelErase;
}

void McCadViewTool_Delete::Destroy()
{
 return;
}

Standard_Boolean McCadViewTool_Delete::IsNull()
{
	return Standard_False;
}

void McCadViewTool_Delete::Execute()
{
    Handle(AIS_InteractiveContext) theContext = myDoc->GetContext();
//	Standard_Boolean haveSelection = Standard_False;

	Handle(TDocStd_Document) tDoc = myDoc->GetTDoc();
	Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(tDoc->Main());

	/*TDF_LabelSequence labSeq;

	for (theContext->InitCurrent(); theContext->MoreCurrent(); theContext->NextCurrent() )
 	{
		//theContext->Erase(theContext->Current(),Standard_False,Standard_False);
		Handle(TPrsStd_AISPresentation) curPres = Handle(TPrsStd_AISPresentation::DownCast(theContext->Current()->GetOwner()));

		labSeq.Append(curPres->Label());
 	 	haveSelection = Standard_True;
 	}
	if(!haveSelection)
	{
		QMcCad_Application::GetAppMainWin()->Mcout("No objects selected - nothing to delete");
		return;
	}

	tDoc->NewCommand();
	{
		for(Standard_Integer i=1; i<=labSeq.Length(); i++)
			labSeq.Value(i).ForgetAllAttributes();

		theContext->UpdateCurrentViewer();
		QMcCad_Application::GetAppMainWin()->GetTreeWidget()->UpdateDocument(QMcCad_Application::GetAppMainWin()->GetEditor()->ID());
	} tDoc->CommitCommand();*/

    Handle(TopTools_HSequenceOfShape) shpSeq = new TopTools_HSequenceOfShape;
    for (theContext->InitCurrent(); theContext->MoreCurrent(); theContext->NextCurrent() )
    {
        Handle(AIS_InteractiveObject) curIO = theContext->Current();
        Handle(AIS_Shape) aisShp = Handle(AIS_Shape)::DownCast(curIO);
        TopoDS_Shape theShp = aisShp->Shape();
        shpSeq->Append(theShp);
//qiu        theContext->Erase(curIO, 0, 0);
        theContext->Erase(curIO, 0);
    }
    theContext->UpdateCurrentViewer();

    Standard_Integer editorID = QMcCad_Application::GetAppMainWin()->GetEditor()->ID();
    QList<TCollection_AsciiString> listDeletedLabel;

    // find and remove all shapes in shpSeq from theTDoc
    for(Standard_Integer i=1; i<=shpSeq->Length(); i++)
    {
        TDF_Label shpLab = sTool->FindShape(shpSeq->Value(i),1);

        TCollection_AsciiString labEntry;
        TDF_Tool::Entry(shpLab, labEntry);

        labEntry.Prepend("_");
        labEntry.Prepend(editorID);
        listDeletedLabel.append(labEntry);

        if(!shpLab.IsNull())
        {
            shpLab.ForgetAllAttributes();
        }
    }

    Standard_Integer EditorID = QMcCad_Application::GetAppMainWin()->GetEditor()->ID();
    QMcCad_Application::GetAppMainWin()->GetTreeWidget()->UpdateDocument(editorID,listDeletedLabel);

	/*AIS_ListOfInteractive ioList, tmpList;
	theContext->DisplayedObjects(ioList, 0);
	theContext->ObjectsInCollector(tmpList);
	ioList.Append(tmpList);
	AIS_ListIteratorOfListOfInteractive it(tmpList);

	for(; it.More(); it.Next())
	{

	}*/

    Done();
}

void McCadViewTool_Delete::UnExecute()
{
	if(myListOfLastDeleted.Extent()<1)
		return;

	Handle(AIS_InteractiveContext) theContext = myDoc->GetContext();
	AIS_ListIteratorOfListOfInteractive it(myListOfLastDeleted);
	for(; it.More(); it.Next())
		theContext->Display(it.Value(), Standard_True);
}

void McCadViewTool_Delete::Suspend()
{
  return;
}

void McCadViewTool_Delete::Resume()
{
  return;

}

void McCadViewTool_Delete::MousePressEvent(const McCadWin_MouseEvent& e)
{
   return;
}

void McCadViewTool_Delete::MouseReleaseEvent(const McCadWin_MouseEvent& e)
{
  return;
}

void McCadViewTool_Delete::MouseLeaveEvent()
{
  return;
}

void McCadViewTool_Delete::MouseEnterEvent()
{
 return;
}

