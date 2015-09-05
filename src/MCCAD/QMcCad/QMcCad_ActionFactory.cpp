#include <QMcCad_ActionFactory.h>

QMcCad_ActionFactory::QMcCad_ActionFactory(QWidget* theWin)
{
	myWin = theWin;
}

QMcCad_ActionFactory::~QMcCad_ActionFactory()
{

}

QAction* QMcCad_ActionFactory::CreateAction(McCadTool_TaskID theID,	QObject* aMaster)
{
	if (aMaster==NULL)
	{
		std::cerr << "Missing Action Master for    " << theID << endl;
		return NULL;
	}

	if (myWin==NULL)
	{
		std::cerr << "Missing Application Window for   " << theID << endl;
		return NULL;
	}

	QAction* action = NULL;

	switch (theID)
	{
		case McCadTool_NoID:
			std::cerr << "No Tool Id " << endl;
			break;

		case McCadTool_FileNew:
			action = new QAction(QIcon(":images/new.png"), tr("&New"), myWin);
			action->setShortcut(tr("Ctrl+N"));
			action->setStatusTip(tr("Creates new document"));
			connect(action, SIGNAL(triggered()), myWin, SLOT(SlotFileNew()));
			break;

		case McCadTool_FileOpen:
			action = new QAction(QIcon(":images/open.png"), tr("&Open"), myWin);
			action->setShortcut(tr("Ctrl+O"));
			action->setStatusTip(tr("Opens file"));
			connect(action, SIGNAL(triggered()), myWin, SLOT(SlotFileOpen()));
			break;

		case McCadTool_FileOpenNew:
			action = new QAction(QIcon(":images/openNew.png"), tr("&Open New"), myWin);
			action->setShortcut(tr("Ctrl+M"));
			action->setStatusTip(tr("Opens file in new document"));
			connect(action, SIGNAL(triggered()), myWin, SLOT(SlotFileOpenNew()));
			break;

		case McCadTool_FileClose:
			action = new QAction(QIcon(":images/close.png"), tr("&Close"), myWin);
			action->setShortcut(tr("Ctrl+Q"));
			action->setStatusTip(tr("Closes file"));
			connect(action, SIGNAL(triggered()), myWin, SLOT(SlotFileClose()));
			break;


		case McCadTool_ImportGeom:
			action = new QAction( tr("&Import"), myWin);
			action->setShortcut(tr("Ctrl+I"));
			action->setStatusTip(tr("Imports Geometry"));
			connect(action, SIGNAL(triggered()), myWin, SLOT(SlotImport()));
			break;


		case McCadTool_ExportGeom:
			action = new QAction(tr("&Export"), myWin);
			action->setShortcut(tr("Ctrl+E"));
			action->setStatusTip(tr("Exports Geometry Tree"));
			connect(action, SIGNAL(triggered()), myWin, SLOT(SlotExport()));
			break;


		case McCadTool_FileSave:
			action = new QAction( QIcon(":images/save.png"), tr("&Save"), myWin);
			action->setShortcut(tr("Ctrl+S"));
			action->setStatusTip(tr("Saves file"));
			connect(action, SIGNAL(triggered()), myWin, SLOT(SlotFileSave()));
			break;

		case McCadTool_FileSaveAs:
			action = new QAction( QIcon(":images/saveas.png"), tr("Save &as"), myWin);
			action->setShortcut(tr("Ctrl+A"));
			action->setStatusTip(tr("Save as ..."));
			connect(action, SIGNAL(triggered()), myWin, SLOT(SlotFileSaveAs()));
			break;

		case McCadTool_Hardcopy:
			action = new QAction(tr("&Hardcopy"), myWin);
			action->setShortcut(tr("Ctrl+H"));
			action->setStatusTip(tr("Window Hardcopy"));
			connect(action, SIGNAL(triggered()), myWin, SLOT(SlotHardcopy()));
			break;

		case McCadTool_Print:
			action = new QAction(QIcon(":images/print.png"), tr("&Print"), myWin);
			action->setShortcut(tr("Ctrl+P"));
			action->setStatusTip(tr("Prints the document"));
			connect(action, SIGNAL(triggered()), myWin, SLOT(SlotPrint()));
			break;

		case McCadTool_EditUndo:
			action = new QAction( QIcon(":images/undo.png"), tr("&Undo"), myWin);
			action->setShortcut(tr("Ctrl+U"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotUndo()));
			break;

		case McCadTool_EditRedo:
			action = new QAction( QIcon(":images/redo.png"), tr("&Redo"), myWin);
			action->setShortcut(tr("Ctrl+R"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotRedo()));
			break;

		case McCadTool_EditCut:
			action = new QAction( QIcon(":images/cut.png"), tr("&Cut"), myWin);
			action->setShortcut(tr("Ctrl+X"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotCut()));
			break;

		case McCadTool_EditCopy:
			action = new QAction( QIcon(":images/copy.png"), tr("&Copy"), myWin);
			action->setShortcut(tr("Ctrl+C"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotCopy()));
			break;

		case McCadTool_EditPaste:
			action = new QAction( QIcon(":images/past.png"), tr("&Past"), myWin);
			action->setShortcut(tr("Ctrl+V"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotPast()));
			break;

		case McCadTool_Select:
			action = new QAction( QIcon(":images/select.png"), tr("&Select"), myWin);
			action->setShortcut(tr("Ctrl+W"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotSelect()));
			break;

		case McCadTool_SelectAll:
			action = new QAction( tr("&SelectAll"), myWin);
			action->setShortcut(tr("Ctrl+Z"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotSelectAll()));
			break;

		case McCadTool_DeselectAll:
			action = new QAction( tr("&DeselectAll"), myWin);
			action->setShortcut(tr("Ctrl+Y"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotDeselectAll()));
			break;

		case McCadTool_Redraw:
			action = new QAction( QIcon(":images/draw.png"), tr("Redraw"), myWin);
			//action->setShortcut(tr("Alt+D"));
			action->setStatusTip(tr("Redraw"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotRedraw()));
			break;

		case McCadTool_ZoomWindow:
			action = new QAction(QIcon(":images/wzoom.png"), tr("&WindowZoom"), myWin);
			action->setShortcut(tr("Alt+W"));
			action->setStatusTip(tr("Window Zoom"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotZooomWindow()));
			break;

		case McCadTool_ZoomDynamic:
			action = new QAction(QIcon(":images/dzoom.png"), tr("&DynamicZoom"), myWin);
			action->setShortcut(tr("Alt+Y"));
			action->setStatusTip(tr("Dynamic Zoom"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotZoomDynamic()));
			break;

		/*case McCadTool_ZoomScale:
			action = new QAction(QIcon(":images/szoom.png"), tr("&ScaleZoom"), myWin);
			action->setShortcut(tr("Alt+S"));
			action->setStatusTip(tr("Scale Zoom"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotZoomScale()));
			break;*/

		case McCadTool_FitAll:
			action = new QAction(QIcon(":images/fitall.png"), tr("&FitAll"), myWin);
			action->setShortcut(tr("Ctrl+F"));
			action->setStatusTip(tr("Fit All"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotFitAll()));
			break;

		case McCadTool_ZoomIn:
			action = new QAction(QIcon(":images/izoom.png"), tr("&ZoomIn"), myWin);
			action->setShortcut(tr("Alt+I"));
			action->setStatusTip(tr("Zoom In"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotZoomIn()));
			break;

		case McCadTool_ZoomOut:
			action = new QAction( QIcon(":images/ozoom.png"), tr("&ZoomOut"), myWin);
			action->setShortcut(tr("Alt+O"));
			action->setStatusTip(tr("Zoom Out"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotZoomOut()));
			break;

		case McCadTool_Pan:
			action = new QAction( QIcon(":images/panview.png"), tr("&Pan"), myWin);
			action->setShortcut(tr("Alt+P"));
			action->setStatusTip(tr("Pan"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotPan()));
			break;

                case McCadTool_Rotate:
			action = new QAction( QIcon(":images/rotate.png"), tr("&Rotate"), myWin);
			action->setShortcut(tr("Alt+R"));
			action->setStatusTip(tr("Rotate"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotRotate()));
			break;

		case McCadTool_BackProject:
			action = new QAction(QIcon(":images/backview.png"), tr("&Back"), myWin);
			action->setStatusTip(tr("Back Projection"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotBackProject()));
			break;

		case McCadTool_FrontProject:
			action = new QAction(QIcon(":images/frontview.png"),tr("&Front"), myWin);
			action->setStatusTip(tr("Fron Projection"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotFrontProject()));
			break;

		case McCadTool_TopProject:
			action = new QAction(QIcon(":images/topview.png"),tr("&Top"), myWin);
			action->setStatusTip(tr("Top Projection"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotTopProject()));
			break;

		case McCadTool_BottomProject:
			action = new QAction(QIcon(":images/bottomview.png"),tr("&Bottom"), myWin);
			action->setStatusTip(tr("Bottom Projection"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotBottomProject()));
			break;

		case McCadTool_LeftProject:
			action = new QAction(QIcon(":images/leftview.png"),tr("&Left"), myWin);
			action->setStatusTip(tr("Left Projection"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotLeftProject()));
			break;

		case McCadTool_RightProject:
			action = new QAction(QIcon(":images/rightview.png"),tr("&Right"), myWin);
			action->setStatusTip(tr("Right Projection"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotRightProject()));
			break;

		case McCadTool_WireFrame:
			action = new QAction(QIcon(":images/wireframe.png"),tr("&WireFrame"), myWin);
			action->setStatusTip(tr("Wireframe"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotWireFrame()));
			break;

		case McCadTool_FlatShade:
			action = new QAction(QIcon(":images/shading.png"),tr("&Shade"), myWin);
			action->setStatusTip(tr("Shading"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotShade()));
			break;

			// AssistActions;

		case McCadTool_MeasureDistance:
			action = new QAction(tr("&Distance"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotDistance()));
			break;

		case McCadTool_MeasureAngle:
			action = new QAction(tr("&Angle"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotAngle()));
			break;

		case McCadTool_MeasureArea:
			action = new QAction(tr("&Area"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotArea()));
			break;

		case McCadTool_MeasureVolume:
			action = new QAction(tr("&Volume"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotVolume()));
			break;

		case McCadTool_PropertyCurve:
			action = new QAction(tr("&CurveProperty"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotCurveProperty()));
			break;

		case McCadTool_PropertySurface:
			action = new QAction(tr("&SurfaceProperty"), myWin);
			connect(action, SIGNAL(triggered()), aMaster,
					SLOT(SlotSurfaceProperty()));
			break;

		case McCadTool_MakeBox:
			action = new QAction(tr("&Box"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotMakeBox()));
			break;

		case McCadTool_MakeCone:
			action = new QAction(tr("&Cone"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotMakeCone()));
			break;

		case McCadTool_MakeCylinder:
			action = new QAction(tr("&Cylinder"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotMakeCylinder()));
			break;

		case McCadTool_MakeHalfSpace:
			action = new QAction(tr("&HalfSpace"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotMakeHalfSpace()));
			break;

		case McCadTool_MakeSphere:
			action = new QAction(tr("&Sphere"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotMakeSphere()));
			break;

		case McCadTool_MakeTorus:
			action = new QAction(tr("&Torus"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotMakeTorus()));
			break;

		case McCadTool_MakeWedge:
			action = new QAction(tr("&Wedge"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotMakeWedge()));
			break;

			//modify

		case McCadTool_ModelErase:
			action = new QAction(tr("&Erase"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotErase()));
			break;

		case McCadTool_ModelCopy:
			action = new QAction(tr("&Copy"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotModelCopy()));
			break;

		case McCadTool_ModelMirror:
			action = new QAction(tr("&Mirror"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotMirror()));
			break;

		case McCadTool_ModelMove:
			action = new QAction(tr("&Move"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotMove()));
			break;

		case McCadTool_ModelRotate:
			action = new QAction(tr("&Rotate"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotModelRotate()));
			break;

		case McCadTool_ModelScale:
			action = new QAction(tr("&Scale"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotScale()));
			break;

		case McCadTool_ModelStrech:
			//action = new QAction(tr("&Strech"), myWin);
			//connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotStrech()));
			break;

		case McCadTool_SolidUnion:
			action = new QAction(tr("&Union"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotUnion()));
			break;

		case McCadTool_SolidIntersection:
			action = new QAction(tr("&Intersection"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotIntersection()));
			break;

		case McCadTool_SolidDifference:
			action = new QAction(tr("&Difference"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotDifference()));
			break;

		case McCadTool_BackgroundColor:
			action = new QAction( QIcon(":images/color.png"), tr("&BackgroundColor"), myWin);
			action->setShortcut(tr("Alt+B"));
			action->setStatusTip(tr("Background Color"));
			connect(action, SIGNAL(triggered()), aMaster,	SLOT(SlotBackgroundColor()));
			break;

		case McCadTool_ShapeColor:
			action = new QAction( QIcon(":images/scolor.png"), tr("&ShapeColor"), myWin);
			action->setShortcut(tr("Alt+C"));
			action->setStatusTip(tr("Shape Color"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotShapeColor()));
			break;

		case McCadTool_ShapeMaterial:
			action = new QAction( QIcon(":images/material.png"), tr("&ShapeMaterial"), myWin);
			action->setShortcut(tr("Alt+M"));
			action->setStatusTip(tr("Shape Material"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotShapeMaterial()));
			break;

		case McCadTool_ShapeTransparency:
			action = new QAction( QIcon(":images/transparency.png"), tr("&Transparency"), myWin);
			//action->setShortcut(tr("Alt+T"));
			action->setStatusTip(tr("Shape Transparency"));
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotShapeTransparency()));
			break;

                case McCadTool_ReadValueList:
                        action = new QAction(tr("Read Value List"), myWin);
                        connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotReadValueList()));
                        break;

		case McCadTool_Antialiasing:
			action = new QAction(tr("&Antialiasing"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotAntialiasing()));
			break;

		case McCadTool_SetHiddenLine:
			action = new QAction(tr("&HiddenLine"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotSetHiddenLine()));
			break;


		case McCadTool_Grid:
			action = new QAction(tr("Set/Unset Grid"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotSetGrid()));
			break;

		case McCadTool_Axis:
			action = new QAction(tr("Set/Unset Axis"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotSetAxis()));
			break;

		case McCadTool_DispSelOnly:
			action = new QAction(QIcon(":images/viewselectedonly.png"),tr("Display Selected Only"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotDispSelOnly()));
			break;

		case McCadTool_ShowAll:
			action = new QAction(QIcon(":images/viewall.png"),tr("Show All"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotShowAll()));
			break;

		case McCadTool_HideSelected:
			action = new QAction(QIcon(":images/selected2ghost.png"),tr("Hide Selected"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotHideSelected()));
			break;

		case McCadTool_RedispSelected:
			action = new QAction(QIcon(":images/ghost2displayed.png"),tr("Redisplay Selected"), myWin);
			connect(action, SIGNAL(triggered()), aMaster, SLOT(SlotRedisplaySelected()));
			break;

         /*case McCadTool_AddMaterialGroup:
            action = new QAction(tr("Edit"), myWin);
            connect(action,SIGNAL(triggered()), aMaster, SLOT(SlotAddMaterialGroup()));
            break; // Lei

        case McCadTool_AddToMaterialGroup:
            action = new QAction(tr("Add solids to material group"), myWin);
           connect(action,SIGNAL(triggered()), aMaster, SLOT(SlotAddToMaterialGroup()));
            connect(action,SIGNAL(triggered()), aMaster, SLOT(SlotEditMaterialCards()));
         */
break;

		case McCadTool_ClippingPlane:
			action = new QAction(tr("Define new Clipping Plane"), myWin);
			action->setShortcut(tr("Ctrl+L"));
			connect(action,SIGNAL(triggered()), aMaster, SLOT(SlotClippingPlane()));
			break;

		default:
			std::cerr << "No Tool  " << theID << "  found!!" << endl;
			break;
	}

	return action;
}

