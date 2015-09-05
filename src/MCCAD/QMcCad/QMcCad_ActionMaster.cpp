#include <QMcCad_ActionMaster.h>

#include <McCadCom_CasDocument.hxx>
#include <McCadCom_CasView.hxx>

#include <QMcCad_Application.h>
#include <McCadViewTool_BackProject.hxx>
#include <McCadViewTool_BottomProject.hxx>
#include <McCadViewTool_FrontProject.hxx>
#include <McCadViewTool_TopProject.hxx>
#include <McCadViewTool_LeftProject.hxx>
#include <McCadViewTool_RightProject.hxx>
#include <McCadViewTool_Redraw.hxx>
#include <McCadViewTool_ZoomOut.hxx>
#include <McCadViewTool_ZoomIn.hxx>
#include <McCadViewTool_FitAll.hxx>
#include <McCadViewTool_Rotate.hxx>
#include <McCadViewTool_Pan.hxx>
#include <McCadViewTool_SetShadingMode.hxx>
#include <McCadViewTool_WireFrame.hxx>
#include <McCadViewTool_Delete.hxx>
#include <McCadViewTool_SetBackgroundColor.hxx>
#include <McCadViewTool_SetShapeColor.hxx>
#include <McCadViewTool_Zoom.hxx>
#include <McCadViewTool_Axis.hxx>
#include <McCadViewTool_Transparency.hxx>
#include <McCadViewTool_ShowAll.hxx>
#include <McCadViewTool_HideSelected.hxx>
#include <McCadViewTool_RedisplaySelected.hxx>
#include <McCadViewTool_DisplaySelectedOnly.hxx>
#include <McCadViewTool_ReadValueList.hxx>
#include <QMcCadGeomeTree_TreeWidget.hxx>

#include <McCadDesign_Tool.hxx>
#include <McCadDesign_MakeBox.hxx>
#include <McCadDesign_MakeCylinder.hxx>
#include <McCadDesign_MakeCone.hxx>
#include <McCadDesign_MakeSphere.hxx>
#include <McCadDesign_CopyTo.hxx>
#include <McCadDesign_Rotate.hxx>
#include <McCadDesign_MoveTo.hxx>
#include <McCadDesign_Scale.hxx>

//#include <McCadViewTool_Grid.hxx>
//#include <McCadViewTool_Axo.hxx>
#include <McCadViewTool_SetAntialiasing.hxx>
#include <McCadViewTool_SetHiddenLine.hxx>

#include <QMcCad_TransparencyDialog.h>
//#include <QMcCad_ClippingPlaneDialog.h>


#include <QMcCad_Editor.h>
#include <QMcCad_View.h>



QMcCad_ActionMaster::QMcCad_ActionMaster()
{
	McCadTool_TaskHistory newTaskHistory(20);
	myTaskHistory = newTaskHistory;
}

QMcCad_ActionMaster::~QMcCad_ActionMaster()
{

}

void QMcCad_ActionMaster::Mcout(const QString & aMessage)
{
	QString theEdMess = "QMcCad: ";
	theEdMess = theEdMess + aMessage;
        QMcCad_Application::GetAppMainWin()->Mcout(theEdMess.toUtf8().constData());
}


Handle(McCadTool_Task) QMcCad_ActionMaster::GetCurrentAction()
{
	QMcCad_View* aView = QMcCad_Application::GetAppMainWin()->GetView();

	if (aView !=NULL)
		return aView->GetCurrentTask();
	else
		return NULL;
}

void QMcCad_ActionMaster::SetCurrentAction(McCadTool_TaskID theID)
{
	Handle(McCadTool_Task) aT = new McCadTool_Task;
	aT->Initialize();
	Handle(McCadCom_CasDocument) theDoc = QMcCad_Application::GetAppMainWin()->GetDocument();
	QMcCad_View* theView = QMcCad_Application::GetAppMainWin()->GetView();


	if (theView == NULL || theDoc == NULL)
	{
		QMcCad_Application::GetAppMainWin()->Mcout("Missing application document or view");
		return;
	}
	switch (theID)
	{
		case McCadTool_NoID:
			break;
		/*handled else where*/
		case McCadTool_FileNew:
			break;
		case McCadTool_FileOpen:
			break;
		case McCadTool_FileClose:
			break;
		case McCadTool_FileSave:
			break;
		case McCadTool_FileSaveAs:
			break;
		case McCadTool_Hardcopy:
			break;
		case McCadTool_ImportBREP:
			break;
		case McCadTool_ImportIGES:
			break;
		case McCadTool_ImportSTEP:
			break;
		case McCadTool_ImportMCNP:
			break;
		case McCadTool_ExportBREP:
			break;
		case McCadTool_ExportIGES:
			break;
		case McCadTool_ExportSTEP:
			break;
		case McCadTool_ExportMCNP:
			break;
		/*------------------------------*/
		case McCadTool_Printer:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_PrinterSetup:
			break;
		case McCadTool_PrintPreview:
			break;
		case McCadTool_Print:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_ResentFiles:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_EditUndo:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_EditRedo:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_EditCut:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_EditCopy:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_EditPaste:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_Select:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_SelectAll:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_DeselectAll:
			Mcout("Not yet implemented!");
			break;

		/*view tools*/
		case McCadTool_Redraw:
			aT = new McCadViewTool_Redraw(theDoc, theView);
			break;
		case McCadTool_ZoomWindow:
			aT = new McCadViewTool_Zoom(theDoc, theView);
			break;
		case McCadTool_ZoomDynamic:
			Mcout("Not yet implemented!");
			break;
		/*case McCadTool_ZoomScale:
			break;*/
		case McCadTool_FitAll:
			aT = new McCadViewTool_FitAll(theDoc, theView);
			break;
		case McCadTool_ZoomIn:
			aT = new McCadViewTool_ZoomIn(theDoc, theView);
			break;
		case McCadTool_ZoomOut:
			aT = new McCadViewTool_ZoomOut(theDoc, theView);
			break;
		case McCadTool_Pan:
			aT = new McCadViewTool_Pan(theDoc, theView);
			break;
		case McCadTool_Rotate:
                        aT = new McCadViewTool_Rotate(theDoc, theView);
                        break;
                case McCadTool_ReadValueList:
                        aT = new McCadViewTool_ReadValueList(theDoc, theView);
			break;
		case McCadTool_FrontProject:
			aT = new McCadViewTool_FrontProject(theDoc,theView);
			break;
		case McCadTool_TopProject:
			aT = new McCadViewTool_TopProject(theDoc,theView);
			break;
		case McCadTool_BottomProject:
			aT = new McCadViewTool_BottomProject(theDoc,theView);
			break;
		case McCadTool_LeftProject:
			aT = new McCadViewTool_LeftProject(theDoc, theView);
			break;
		case McCadTool_RightProject:
			aT = new McCadViewTool_RightProject(theDoc, theView);
			break;
		case McCadTool_WireFrame:
			aT = new McCadViewTool_WireFrame(theDoc, theView);
			break;
		case McCadTool_FlatShade:
			aT = new McCadViewTool_SetShadingMode(theDoc, theView);
			break;
		case McCadTool_MeasureDistance:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_MeasureAngle:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_MeasureArea:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_MeasureVolume:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_PropertyCurve:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_PropertySurface:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_MakeBox:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_MakeCone:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_MakeCylinder:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_MakeHalfSpace:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_MakeSphere:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_MakeTorus:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_MakeWedge:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_ModelErase:
			aT = new McCadViewTool_Delete(theDoc, theView);
			break;
		case McCadTool_ModelCopy:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_ModelMirror:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_ModelMove:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_ModelRotate:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_ModelScale:
			//Mcout("Not yet implemented!");
			break;
		case McCadTool_ModelStrech:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_SolidUnion:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_SolidIntersection:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_SolidDifference:
			Mcout("Not yet implemented!");
			break;
		case McCadTool_BackgroundColor:
			aT = new McCadViewTool_SetBackgroundColor(theDoc, theView);
			break;
		case McCadTool_ShapeColor:
			aT = new McCadViewTool_SetShapeColor(theDoc, theView);
			break;
		case McCadTool_ShapeMaterial:
			break;
		case McCadTool_ShapeTransparency:
			aT = new McCadViewTool_Transparency(theDoc, theView);
			break;
		case McCadTool_Antialiasing:
			aT = new McCadViewTool_SetAntialiasing(theDoc, theView);
			break;
		case McCadTool_SetHiddenLine:
			aT = new McCadViewTool_SetHiddenLine(theDoc, theView);
			break;
		case McCadTool_Axis:
			aT = new McCadViewTool_Axis(theDoc, theView);
			break;
		case McCadTool_BackProject:
			aT = new McCadViewTool_BackProject(theDoc,theView);
			break;
		case McCadTool_ShowAll:
			aT = new McCadViewTool_ShowAll(theDoc, theView);
			break;
		case McCadTool_HideSelected:
			aT = new McCadViewTool_HideSelected(theDoc, theView);
			break;
		case McCadTool_RedispSelected:
			aT = new McCadViewTool_RedisplaySelected(theDoc, theView);
			break;
		case McCadTool_DispSelOnly:
			aT = new McCadViewTool_DisplaySelectedOnly(theDoc, theView);
			break;
		case McCadTool_ClippingPlane:
			theView->ClippingPlaneDialog();
			return;
			break;
		default:
			Mcout("QMcCad_ActionMaster :: no such action found.");
			break;
	}

	if (!aT->IsNull())
	{
		theView->SetCurrentTask(aT);
		myTaskHistory.Log(aT);
        emit ActionStarted();
	}
}

void QMcCad_ActionMaster::SlotUndo()
{
	QMcCad_Editor* theEditor = QMcCad_Application::GetAppMainWin()->GetEditor();
	Handle(TDocStd_Document) theTDoc = theEditor->GetDocument()->GetTDoc();

	if(!theTDoc->Undo())
		//cout << "could not UNDO command\n";

	theEditor->GetDocument()->GetContext()->UpdateCurrentViewer();

	if(myTaskHistory.CanUndo())
		myTaskHistory.Undo();
}
void QMcCad_ActionMaster::SlotRedo()
{
	QMcCad_Application::GetAppMainWin()->GetEditor()->GetDocument()->GetTDoc()->Redo();
	//SetCurrentAction(McCadTool_EditRedo);
}
void QMcCad_ActionMaster::SlotCut()
{
	SetCurrentAction(McCadTool_EditCut);
}
void QMcCad_ActionMaster::SlotCopy()
{
	SetCurrentAction(McCadTool_EditCopy);
}
void QMcCad_ActionMaster::SlotPast()
{
	SetCurrentAction(McCadTool_EditPaste);
}
void QMcCad_ActionMaster::SlotSelect()
{
	SetCurrentAction(McCadTool_Select);
}
void QMcCad_ActionMaster::SlotSelectAll()
{
	SetCurrentAction(McCadTool_SelectAll);
}
void QMcCad_ActionMaster::SlotDeselectAll()
{
	SetCurrentAction(McCadTool_DeselectAll);
}

// view ............................................................
void QMcCad_ActionMaster::SlotRedraw()
{
	SetCurrentAction(McCadTool_Redraw);
}

void QMcCad_ActionMaster::SlotZooomWindow()
{
	SetCurrentAction(McCadTool_ZoomWindow);
}
void QMcCad_ActionMaster::SlotZoomDynamic()
{
	SetCurrentAction(McCadTool_ZoomDynamic);
}
void QMcCad_ActionMaster::SlotDispSelOnly()
{
	SetCurrentAction(McCadTool_DispSelOnly);
}
void QMcCad_ActionMaster::SlotRedisplaySelected()
{
	SetCurrentAction(McCadTool_RedispSelected);
}
void QMcCad_ActionMaster::SlotHideSelected()
{
	SetCurrentAction(McCadTool_HideSelected);
}
void QMcCad_ActionMaster::SlotShowAll()
{
	SetCurrentAction(McCadTool_ShowAll);
}
/*void QMcCad_ActionMaster::SlotZoomScale()
{
	SetCurrentAction(McCadTool_ZoomScale);
}*/
void QMcCad_ActionMaster::SlotFitAll()
{
	SetCurrentAction(McCadTool_FitAll);
}
void QMcCad_ActionMaster::SlotZoomIn()
{
	SetCurrentAction(McCadTool_ZoomIn);
}
void QMcCad_ActionMaster::SlotZoomOut()
{
	SetCurrentAction(McCadTool_ZoomOut);
}
void QMcCad_ActionMaster::SlotPan()
{
	SetCurrentAction(McCadTool_Pan);
}
void QMcCad_ActionMaster::SlotRotate()
{
        SetCurrentAction(McCadTool_Rotate);
}
void QMcCad_ActionMaster::SlotBackProject()
{
	SetCurrentAction(McCadTool_BackProject);
}
void QMcCad_ActionMaster::SlotFrontProject()
{
	SetCurrentAction(McCadTool_FrontProject);
}
void QMcCad_ActionMaster::SlotTopProject()
{
	SetCurrentAction(McCadTool_TopProject);
}
void QMcCad_ActionMaster::SlotBottomProject()
{
	SetCurrentAction(McCadTool_BottomProject);
}
void QMcCad_ActionMaster::SlotLeftProject()
{
	SetCurrentAction(McCadTool_LeftProject);
}
void QMcCad_ActionMaster::SlotRightProject()
{
	SetCurrentAction(McCadTool_RightProject);
}
void QMcCad_ActionMaster::SlotWireFrame()
{
	SetCurrentAction(McCadTool_WireFrame);
}
void QMcCad_ActionMaster::SlotShade()
{
	SetCurrentAction(McCadTool_FlatShade);
}
// assist..............................................................
void QMcCad_ActionMaster::SlotDistance()
{
	SetCurrentAction(McCadTool_MeasureDistance);
}
void QMcCad_ActionMaster::SlotAngle()
{
	SetCurrentAction(McCadTool_MeasureAngle);
}
void QMcCad_ActionMaster::SlotArea()
{
	SetCurrentAction(McCadTool_MeasureArea);
}
void QMcCad_ActionMaster::SlotVolume()
{
	SetCurrentAction(McCadTool_MeasureVolume);
}
void QMcCad_ActionMaster::SlotCurveProperty()
{
	SetCurrentAction(McCadTool_PropertyCurve);
}
void QMcCad_ActionMaster::SlotSurfaceProperty()
{
	SetCurrentAction(McCadTool_PropertySurface);
}

// model ...............................................................
void QMcCad_ActionMaster::SlotMakeBox()
{
	//SetCurrentAction(McCadTool_MakeBox);
	McCadDesign_MakeBox boxMaker(QMcCad_Application::GetAppMainWin());
	boxMaker.SetCurrentEditor(QMcCad_Application::GetAppMainWin()->GetEditor());
	boxMaker.exec();
}
void QMcCad_ActionMaster::SlotMakeCone()
{
	//SetCurrentAction(McCadTool_MakeCone);
	McCadDesign_MakeCone coneMaker(QMcCad_Application::GetAppMainWin());
	coneMaker.SetCurrentEditor(QMcCad_Application::GetAppMainWin()->GetEditor());
	coneMaker.exec();
}
void QMcCad_ActionMaster::SlotMakeCylinder()
{
	//SetCurrentAction(McCadTool_MakeCylinder);
	McCadDesign_MakeCylinder cylinderMaker(QMcCad_Application::GetAppMainWin());
	cylinderMaker.SetCurrentEditor(QMcCad_Application::GetAppMainWin()->GetEditor());
	cylinderMaker.exec();
}
void QMcCad_ActionMaster::SlotMakeHalfSpace()
{
	SetCurrentAction(McCadTool_MakeHalfSpace);
}
void QMcCad_ActionMaster::SlotMakeSphere()
{
	//SetCurrentAction(McCadTool_MakeSphere);
	McCadDesign_MakeSphere sphereMaker(QMcCad_Application::GetAppMainWin());
	sphereMaker.SetCurrentEditor(QMcCad_Application::GetAppMainWin()->GetEditor());
	sphereMaker.exec();
}
void QMcCad_ActionMaster::SlotMakeTorus()
{
	SetCurrentAction(McCadTool_MakeTorus);
}
void QMcCad_ActionMaster::SlotMakeWedge()
{
	SetCurrentAction(McCadTool_MakeWedge);
}

// modify  ...............................................................
void QMcCad_ActionMaster::SlotErase()
{
	SetCurrentAction(McCadTool_ModelErase);
}
void QMcCad_ActionMaster::SlotModelCopy()
{
	McCadDesign_CopyTo copyMaker(QMcCad_Application::GetAppMainWin());
	copyMaker.SetCurrentEditor(QMcCad_Application::GetAppMainWin()->GetEditor());
	copyMaker.exec();
	//SetCurrentAction(McCadTool_ModelCopy);
}
void QMcCad_ActionMaster::SlotMirror()
{
	SetCurrentAction(McCadTool_ModelMirror);
}
void QMcCad_ActionMaster::SlotMove()
{
	McCadDesign_MoveTo moveMaker(QMcCad_Application::GetAppMainWin());
	moveMaker.SetCurrentEditor(QMcCad_Application::GetAppMainWin()->GetEditor());
	moveMaker.exec();
	//SetCurrentAction(McCadTool_ModelMove);
}
void QMcCad_ActionMaster::SlotModelRotate()
{
	McCadDesign_Rotate rotationMaker(QMcCad_Application::GetAppMainWin());
	rotationMaker.SetCurrentEditor(QMcCad_Application::GetAppMainWin()->GetEditor());
	rotationMaker.exec();
	//SetCurrentAction(McCadTool_ModelRotate);
}
void QMcCad_ActionMaster::SlotScale()
{
	//SetCurrentAction(McCadTool_ModelScale);
	McCadDesign_Scale scaleMaker(QMcCad_Application::GetAppMainWin());
	scaleMaker.SetCurrentEditor(QMcCad_Application::GetAppMainWin()->GetEditor());
	scaleMaker.exec();
}

void QMcCad_ActionMaster::SlotReadValueList()
{
    SetCurrentAction(McCadTool_ReadValueList);
}

void QMcCad_ActionMaster::SlotStrech()
{
	SetCurrentAction(McCadTool_ModelStrech);
}
void QMcCad_ActionMaster::SlotUnion()
{
	SetCurrentAction(McCadTool_SolidUnion);
}
void QMcCad_ActionMaster::SlotIntersection()
{
	SetCurrentAction(McCadTool_SolidIntersection);
}
void QMcCad_ActionMaster::SlotDifference()
{
	SetCurrentAction(McCadTool_SolidDifference);
}

//window
void QMcCad_ActionMaster::SlotBackgroundColor()
{
	SetCurrentAction(McCadTool_BackgroundColor);
}
void QMcCad_ActionMaster::SlotShapeColor()
{
	SetCurrentAction(McCadTool_ShapeColor);
}
void QMcCad_ActionMaster::SlotShapeMaterial()
{
	SetCurrentAction(McCadTool_ShapeMaterial);
}
void QMcCad_ActionMaster::SlotShapeTransparency()
{
	//SetCurrentAction(McCadTool_ShapeTransparency);
	QMcCad_TransparencyDialog transDial(QMcCad_Application::GetAppMainWin());
	transDial.exec();
}
void QMcCad_ActionMaster::SlotAntialiasing()
{
	SetCurrentAction(McCadTool_Antialiasing);
}
void QMcCad_ActionMaster::SlotSetHiddenLine()
{
	SetCurrentAction(McCadTool_SetHiddenLine);
}
void QMcCad_ActionMaster::SlotSetAxis()
{
	SetCurrentAction(McCadTool_Axis);
}
void QMcCad_ActionMaster::SlotSetGrid()
{
	SetCurrentAction(McCadTool_Grid);
}

// QMcCadGeomeTree related stuff
void QMcCad_ActionMaster::SlotAddMaterialGroup()
{
    (QMcCad_Application::GetAppMainWin()->GetTreeWidget())->SlotAddMaterialGroup();
}

void QMcCad_ActionMaster::SlotAddToMaterialGroup()
{
	(QMcCad_Application::GetAppMainWin()->GetTreeWidget())->SlotAddToMaterialGroup();
}

/*void QMcCad_ActionMaster::SlotOnClickRButton() // Lei
{
    (QMcCad_Application::GetAppMainWin()->GetTreeWidget())->SlotAddMaterialGroup();
}*/

void QMcCad_ActionMaster::SlotClippingPlane()
{
	SetCurrentAction(McCadTool_ClippingPlane);
	//QMcCad_ClippingPlaneDialog cpDial(QMcCad_Application::GetAppMainWin());
	//cpDial.exec();
}
