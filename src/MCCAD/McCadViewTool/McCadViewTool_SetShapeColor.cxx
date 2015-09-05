#include <McCadViewTool_SetShapeColor.ixx>
#include <V3d_View.hxx>
#include <Handle_V3d_View.hxx>
#include <qcolor.h>
#include <qcolordialog.h>
#include <AIS_DisplayMode.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <TDocStd_Document.hxx>
#include <QMcCad_Application.h>
#include <XCAFDoc_DocumentTool.hxx>
#include <QMcCad_Editor.h>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>


McCadViewTool_SetShapeColor::McCadViewTool_SetShapeColor(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	 myView = theView;
	 myState = theState;
	 myUndoState = theUndoState;
	 myRedoState = theRedoState;
	 myID = McCadTool_ShapeColor;
}


Standard_Boolean McCadViewTool_SetShapeColor::IsNull()
{
	return Standard_False;
}

 void McCadViewTool_SetShapeColor::Destroy()
{
}

 void McCadViewTool_SetShapeColor::Execute()
{
	QColor aColor;
	Standard_Real R1;
	Standard_Real G1;
	Standard_Real B1;
	Handle(V3d_View) aView = myView->View();
	Handle(AIS_InteractiveContext) theContext = myDoc->GetContext();

	aView->BackgroundColor(Quantity_TOC_RGB, R1, G1, B1);
	aColor.setRgb(int(R1*255), int(G1*255), int(B1*255));
	QColorDialog* aColorDialog;

	QColor aRetColor = aColorDialog->getColor(aColor);
	if (aRetColor.isValid() )
	{
		Handle(TDocStd_Document) theTDoc = QMcCad_Application::GetAppMainWin()->GetEditor()->GetDocument()->GetTDoc();
		Handle(XCAFDoc_ColorTool) cTool = XCAFDoc_DocumentTool::ColorTool(theTDoc->Main());
		Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());

		Quantity_Color color(aRetColor.red() / 255., aRetColor.green() / 255., aRetColor.blue() / 255., Quantity_TOC_RGB);
		/*TDF_Label cLab = cTool->AddColor(color);
		TCollection_AsciiString newCLab;
		TDF_Tool::Entry(cLab,newCLab);*/

		AIS_ListOfInteractive ioList;
		for(theContext->InitCurrent(); theContext->MoreCurrent(); theContext->NextCurrent())
			ioList.Append(theContext->Current());

		AIS_ListIteratorOfListOfInteractive it(ioList);

		for ( ; it.More(); it.Next() )
		{
			Handle(AIS_InteractiveObject) io = it.Value();//theContext->Current();
			io->SetColor(color);

			Handle(AIS_Shape) aisShp = Handle(AIS_Shape)::DownCast(io);

			if(aisShp == NULL)
				continue;

			TopoDS_Shape theShp = aisShp->Shape();
			TDF_Label curL = sTool->FindShape(theShp,1);

			//TopoDS_Shape aShape = Handle(AIS_Shape)::DownCast(io)->Shape();
			//Handle(TPrsStd_AISPresentation) prs = Handle(TPrsStd_AISPresentation::DownCast(io->GetOwner()));
			//theContext->Clear(prs->GetAIS(),0); //delete current presentation, do not update viewer
			TDF_Label cLab = cTool->AddColor(color);
			cTool->SetColor(curL, cLab, XCAFDoc_ColorGen);
			//cTool->SetColor(prs->Label(), newCLab, XCAFDoc_ColorGen);
			//TPrsStd_AISViewer::Update(theTDoc->GetData()->Root());
			//prs->Update();
			//theContext->Display(prs->GetAIS(),0);
		}
		theContext->UpdateCurrentViewer();
	}

	aView->Redraw();
	Done();
}

 void McCadViewTool_SetShapeColor::UnExecute()
{
}

 void McCadViewTool_SetShapeColor::Suspend()
{
}

 void McCadViewTool_SetShapeColor::Resume()
{
}

 void McCadViewTool_SetShapeColor::MousePressEvent(const McCadWin_MouseEvent& e)
{
}

 void McCadViewTool_SetShapeColor::MouseReleaseEvent(const McCadWin_MouseEvent& e)
{
}

 void McCadViewTool_SetShapeColor::MouseLeaveEvent()
{
}

 void McCadViewTool_SetShapeColor::MouseEnterEvent()
{
}

