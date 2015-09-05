#include <McCadViewTool_ReadValueList.ixx>
#include <Aspect_ColorScale.hxx>
#include <QMcCad_Application.h>
#include <Quantity_Color.hxx>
#include <V3d_View.hxx>
#include <QMcCad_ReadListDialog.hxx>
#include <McCadXCAF_TDocShapeLabelIterator.hxx>
#include <AIS_InteractiveContext.hxx>

#include <TDataStd_Name.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDF_Label.hxx>

#include <QMcCad_Editor.h>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

McCadViewTool_ReadValueList::McCadViewTool_ReadValueList(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	 myDoc = theDoc;
	 myView = theView;
	 myState = theState;
	 myUndoState = theUndoState;
	 myRedoState = theRedoState;
	 myID = McCadTool_ReadValueList;
}

void McCadViewTool_ReadValueList::Destroy()
{
 return;
}

Standard_Boolean McCadViewTool_ReadValueList::IsNull()
{
	return Standard_False;
}

void McCadViewTool_ReadValueList::Execute()
{
  	Handle(V3d_View) aView = myView->View();
        Handle_AIS_InteractiveContext theIC = myDoc->GetContext();

    // Read File and process Data
        QMcCad_ReadListDialog* readDialog = new QMcCad_ReadListDialog;
        readDialog->exec();

        if(!readDialog->Done())
            return;

        list<int> cellNumberList;
        list<double> valueList;

        cellNumberList = readDialog->GetCellNumberList();
        valueList = readDialog->GetValueList();
        int nbColors(12);
        nbColors = readDialog->GetNbColors();

    // initialize ColorSacle
        aView->ColorScaleDisplay();
        Handle(Aspect_ColorScale) colorScale = aView->ColorScale();
        colorScale->SetXPosition(0.9);
        colorScale->SetYPosition(0.1);
        colorScale->SetSize(0.1, 0.8);
        colorScale->SetNumberOfIntervals(nbColors);
        colorScale->SetMin(readDialog->GetLowerBound());
        colorScale->SetMax(readDialog->GetUpperBound());

    // Assign Colors to Solids
        list<int>::iterator cellIt = cellNumberList.begin();
        list<double>::iterator valueIt = valueList.begin();

        Handle(AIS_InteractiveContext) theContext = myDoc->GetContext();
        Handle(TDocStd_Document) theTDoc = QMcCad_Application::GetAppMainWin()->GetEditor()->GetDocument()->GetTDoc();
        Handle(XCAFDoc_ColorTool) cTool = XCAFDoc_DocumentTool::ColorTool(theTDoc->Main());
        Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());


        for(; cellIt != cellNumberList.end(); cellIt++)
        {
            Quantity_Color curColor;
            colorScale->FindColor(*valueIt, curColor);

            TCollection_AsciiString cellName(*cellIt);

            // find matching names
            McCadXCAF_TDocShapeLabelIterator it(myDoc->GetTDoc());
            for(; it.More(); it.Next())
            {
                TDF_Label curLab = it.Current();
                Handle_TDataStd_Name theName = new TDataStd_Name;
                if(curLab.FindAttribute(TDataStd_Name::GetID(),theName))
                {
                    TCollection_AsciiString curName(theName->Get());
                    if(curName == cellName)
                    {
                        // assign color to solid associated with this label
                        TDF_Label cLab = cTool->AddColor(curColor);
                        cTool->SetColor(curLab, cLab, XCAFDoc_ColorGen);

                        // find interactive Object coresponding to Shape
                        AIS_ListOfInteractive ioList;
                        AIS_ListOfInteractive tmpList;

//qiu                        theIC->ObjectsInCollector(tmpList);
                        theIC->ErasedObjects(tmpList);
                        theIC->DisplayedObjects(ioList, 0);
                        ioList.Append(tmpList);

                        Handle(TNaming_NamedShape) nShp;
                        if(! curLab.FindAttribute(TNaming_NamedShape::GetID(), nShp))
                            continue;

                        TopoDS_Shape refShp = nShp->Get();

                        for(AIS_ListIteratorOfListOfInteractive it(ioList); it.More(); it.Next())
                        {
                            if(theIC->IsSelected(it.Value()))
                                continue;

                            Handle(AIS_Shape) aisShp = Handle(AIS_Shape)::DownCast(it.Value());
                            TopoDS_Shape curShp = aisShp->Shape();

                            if(curShp.IsEqual(refShp))
                                it.Value()->SetColor(curColor);
                        }

                    }
                }
            }

            valueIt++;
        }



	aView->Redraw();
	Done();
}

void McCadViewTool_ReadValueList::UnExecute()
{
	  Handle(V3d_View) aView = myView->View();
          aView->ColorScaleErase();
	  aView->Redraw();
	  Done();
}

void McCadViewTool_ReadValueList::Suspend()
{
  return;
}

void McCadViewTool_ReadValueList::Resume()
{

  return;
}

void McCadViewTool_ReadValueList::MousePressEvent(const McCadWin_MouseEvent& e)
{
   return;
}

void McCadViewTool_ReadValueList::MouseReleaseEvent(const McCadWin_MouseEvent& e)
{
  return;
}

void McCadViewTool_ReadValueList::MouseLeaveEvent()
{
  return;
}

void McCadViewTool_ReadValueList::MouseEnterEvent()
{
 return;
}

// Private Functions
///////////////////////////

bool McCadViewTool_ReadValueList::ParseFile()
{
return false;
}
