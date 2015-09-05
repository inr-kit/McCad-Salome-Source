#include <QMcCad_View.h>

#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QX11Info>

#include <GL/glx.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xmu/StdCmap.h>
#include <X11/Xlib.h>
#include <Xw_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>

////////////////////////////////////
#include <Geom_Axis2Placement.hxx>
#include <AIS_Trihedron.hxx>
#include <gp_Pnt.hxx>
///////////////////////////////////////
#include <McCadTool_Task.hxx>
#include <McCadCom_Document.hxx>
#include <McCadCom_CasDocument.hxx>

#include <QMcCad_Viewer.h>
#include <QMcCad_ActionMaster.h>

#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#include <QtGui/QInputEvent>
#include <QtGui/QColorDialog>
#include <QtGui/QPlastiqueStyle>
#include <V3d_View.hxx>
#include <QMcCad_Application.h>
#include <QtCore/QReadWriteLock>
#include <qrubberband.h>
#include <Visual3d_Layer.hxx>
#include <qstatusbar.h>
#include <gp_Pln.hxx>
#include <gp_Lin.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <V3d_Plane.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <QMcCad_ClippingPlaneDialog.h>
#include <Visual3d_Layer.hxx>
#include <Visual3d_View.hxx>


QMcCad_View::QMcCad_View(QWidget* theParent, QMcCad_Viewer* theViewer,Handle(McCadCom_CasDocument) theDoc)
:QGLWidget((QWidget*) theParent), McCadCom_CasView(theDoc)
{
	myAxisSet = Standard_False;
	QApplication::syncX();
	//  setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
	setMouseTracking(true);
	myRubberBand = new QRubberBand(QRubberBand::Rectangle, this);
	// setMouseTracking( true );
	setAutoFillBackground( false );
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_PaintOnScreen);

	// setAutoBufferSwap( false );
	// setAttribute( Qt::WA_OpaquePaintEvent );
	setWindowIcon(QPixmap(":images/mccad.png"));
	myViewer = theViewer;
	myXmin = 0;
	myYmin = 0;
	myXmax = 0;
	myYmax = 0;
	myCPA = 0.0;
	myCPB = 0.0;
	myCPC = 0.0;
	myCPD = 0.0;
	myCPVisState = Standard_False;
	myIsInit = Standard_False;
	mySelectionMode=Standard_False;
	myRectangleMode=Standard_False;
	myShiftSelect = Standard_False;
	myControlDown = Standard_False;

	/////////////////////////////////////////////////////
	myAcM = myViewer->GetActionMaster();

	connect(myAcM, SIGNAL(ActionStarted()),this, SLOT(SlotInitTask()));
	connect(this, SIGNAL(InitFinished()),this, SLOT(PreExecute()));
}

QMcCad_View::~QMcCad_View()
{
}

QSize QMcCad_View::minimumSizeHint() const
{
	return QSize(250, 250);
}


QSize QMcCad_View::sizeHint() const
{
	return QSize(400, 400);
}


void QMcCad_View::initializeGL()
{

	int windowHandle = (int) winId();
	short hi;
	short lo;
	lo = (short) windowHandle;
	hi = (short) (windowHandle >> 16);

	Handle(AIS_InteractiveContext) theContext = myViewer->GetContext();
	Handle(Xw_Window) theXWindow = new Xw_Window(Handle(Graphic3d_GraphicDevice)::DownCast(theContext->CurrentViewer()->Device()),
			(int) hi,(int) lo,Xw_WQ_SAMEQUALITY,Quantity_NOC_BLACK);
	Init(theXWindow);
	FitAll();
	myIsInit = Standard_True;
}


void QMcCad_View::paintGL()
{
	Redraw();
}


void QMcCad_View::resizeGL(int width, int height)
{
	//int w=width;
	//int h=height;
	Resize();
}


void QMcCad_View::SlotInitTask()
{
	Handle(McCadTool_Task) aTask = GetCurrentTask();
	myCurrentTID = aTask->GetID();

	if(aTask->IsNull())
		return;

	if (aTask->NeedRect())
	{
		aTask->Suspend();
		myRectangleMode = Standard_True;
	}
	if (aTask->NeedSelect())
	{
		aTask->Suspend();
		mySelectionMode = Standard_True;
	}

	emit InitFinished();
}


void QMcCad_View::PreExecute()
{
	Handle(McCadTool_Task) aTask = GetCurrentTask();
	if (aTask->GetID() == McCadTool_NoID)
		return;

	if (aTask->State() != McCadTool_Start)
		aTask->Resume();
    else
        Execute();
}


void QMcCad_View::Execute()
{
	Handle(McCadTool_Task) aTask = GetCurrentTask();

	aTask->Execute();

	if(myCurrentTID == McCadTool_Axis) // show/hide Axis
	{
		if(myAxisSet)
		{
			aTask->UnExecute();
			myAxisSet = Standard_False;
		}
		else
			myAxisSet = Standard_True;
	}

	//Reset Current Task after current Task is done;
	Handle(McCadTool_Task) aT = new McCadTool_Task;
	aT->Initialize();

    if(aTask->State() == McCadTool_Done)
	{
		SetCurrentTask(aT);
		myCurrentTID = McCadTool_NoID;
		mySelectionMode = Standard_False;
		myRectangleMode = Standard_False;
	}
}


void QMcCad_View::Select()
{
    Handle(AIS_InteractiveContext) theContext = myViewer->GetContext();
    theContext->Select();
	mySelectionMode=Standard_False;
	emit InitFinished();
}


void QMcCad_View::ShiftSelect()
{
	Handle(AIS_InteractiveContext) theContext = myViewer->GetContext();
	theContext->ShiftSelect();
	mySelectionMode=Standard_False;
	emit InitFinished();
}


void QMcCad_View::DrawRectangle()
{
	/*QPainter aPainter(this);
	aPainter.setPen(Qt::white);
	//aPainter.setBrush(Qt::NoBrush);
	//QRect aRectangle;
	//aRectangle.setRect(myXmin, myYmin, abs(myXmax-myXmin), abs(myYmax-myYmin));
	//aPainter.drawRect(aRectangle);
	aPainter.drawRect(myXmin, myYmin, abs(myXmax-myXmin), abs(myYmax-myYmin));

	myRectangleMode=Standard_False;
	emit InitFinished();*/
}


void QMcCad_View::mousePressEvent(QMouseEvent* e)
{
    QPoint aPoint = e->pos();
	myXmin = aPoint.x();
	myYmin = aPoint.y();
	myXmax = aPoint.x();
	myYmax = aPoint.y();

	if(e->button() == Qt::LeftButton)
	{
		myLMBpushed = Standard_True;
		MousePressEvent(*e);

		if(myCurrentTID == McCadTool_NoID) // Selection mode
		{
            mySelectionMode = Standard_True;

            myRubberBand->setGeometry(QRect(aPoint, QSize()));
            myRubberBand->setStyle((QStyle*) new QPlastiqueStyle() );
            myRubberBand->show();
		}

		if(myCurrentTID == McCadTool_Rotate)
			View()->StartRotation(aPoint.x(), aPoint.y() );
	}
        else if(e->button()==Qt::MidButton)
	{
		myMMBpushed = Standard_True;
		View()->StartRotation(aPoint.x(), aPoint.y() );
	}
}


void QMcCad_View::mouseReleaseEvent(QMouseEvent* e)
{
	QPoint aPoint = e->pos();

	Handle(AIS_InteractiveContext) theContext = myViewer->GetContext();

	MouseReleaseEvent(*e);

	if(mySelectionMode)
	{
		if (e->button() == Qt::LeftButton)
		{
			myXmax = aPoint.x();
			myYmax = aPoint.y();

			if (myControlDown)
				ShiftSelect();
			else
				Select();
		}
		emit SigSelectionChanged(); //Update TreeWidget
	}

	if (myRectangleMode)
	{
		if (e->button() == Qt::LeftButton)
		{
			myXmax = aPoint.x();
			myYmax = aPoint.y();
			if(myCurrentTID == McCadTool_NoID)
			{
				theContext->Select(myXmin, myYmin, myXmax, myYmax, View());
				emit SigSelectionChanged();
			}

			myRubberBand->hide();
		}
	}

	Execute();

	mySelectionMode = Standard_False;
	myRectangleMode = Standard_False;
	myCurrentTID = McCadTool_NoID;
	myLMBpushed = Standard_False;
	myMMBpushed = Standard_False;
}


Standard_Boolean QMcCad_View::ConvertToPlane(const Standard_Integer Xs, const Standard_Integer Ys, Standard_Real& X, Standard_Real& Y, Standard_Real& Z)
{
	Standard_Real Xv(0.), Yv(0.), Zv(0.);
	Standard_Real Vx(0.), Vy(0.), Vz(0.);
	gp_Pln aPlane(myView->Viewer()->PrivilegedPlane());
	//myView->Viewer()->DisplayPrivilegedPlane(Standard_True, 100);

	//Standard_Real A,B,C,D;
	//(myView->ActivePlane())->Plane(A,B,C,D);
	//cout << A << " " << B << " " << C << " " << D << endl;

	// The + 1 overcomes a fault in OCC, in "OpenGl_togl_unproject_raster.c",
	// which transforms the Y axis ordinate. The function uses the height of the
	// window, not the Y maximum which is (height - 1).
	myView->Convert( Xs, Ys + 1, Xv, Yv, Zv );

	X = Xv; Y = Yv; Z = Zv;

	return Standard_True;

	myView->Proj( Vx, Vy, Vz );
//	cout << "PROJ : " << Vx << "  "  << Vy << "  "  << Vz << endl;
	gp_Lin aLine(gp_Pnt(Xv, Yv, Zv), gp_Dir(Vx, Vy, Vz));
	IntAna_IntConicQuad theIntersection( aLine, aPlane, Precision::Angular() );
	if (theIntersection.IsDone())
	{
		if (!theIntersection.IsParallel())
		{
			if (theIntersection.NbPoints() > 0)
			{
				gp_Pnt theSolution(theIntersection.Point(1));
				X = theSolution.X();
				Y = theSolution.Y();
				Z = theSolution.Z();

				return Standard_True;
			}
		}
	}

	return Standard_False;
}


void QMcCad_View::mouseMoveEvent(QMouseEvent* e)
{
	QPoint aPoint = e->pos();
	Standard_Integer x,y;

	MouseMoveEvent(*e);

	//Hilight object under the cursor
	x=aPoint.x();
	y=aPoint.y();

	if(myCurrentTID != McCadTool_Rotate && !myMMBpushed)
	{
		myViewer->GetContext()->MoveTo(x, y, myView);

		// visualize cursor position on plane
		Standard_Real theX, theY, theZ;
		ConvertToPlane( x,	y,	theX, theY, theZ );


		//myView->ConvertToGrid( x, y, theX , theY , theZ );
		//cout << theX << "  "  << theY << "  " << theZ << endl;
		Standard_Real projX, projY, projZ;
		myView->Proj(projX, projY, projZ);
                if( (projX == 0 && projY == 0) ||
                    (projX == 0 && projZ == 0) ||
                    (projY == 0 && projZ == 0) ) // projection
		{
			QString aString;
			TCollection_AsciiString numStr;
			if(projX != 0)
				numStr = "0";
			else
				numStr = theX;
			aString.append(numStr.ToCString());
			aString.append(" , ");
			if(projY != 0)
				numStr = "0";
			else
				numStr = theY;
			aString.append(numStr.ToCString());
			aString.append(" , ");
			if(projZ != 0)
				numStr = "0";
			else
				numStr = theZ;
			aString.append(numStr.ToCString());
			QMcCad_Application::GetAppMainWin()->statusBar()->showMessage(aString);
		}
		else
			QMcCad_Application::GetAppMainWin()->statusBar()->showMessage("Ready");
	}

	//handle mouse clicks
	if(myMMBpushed)
		View()->Rotation(x, y);

	if(myLMBpushed)
	{
		if(myControlDown)
                {
                    myCurrentTID = McCadTool_Pan;
                    mySelectionMode = false;
                }

		switch(myCurrentTID)
		{
                    case McCadTool_Rotate:
                            View()->Rotation(x, y);
                            break;
                    case McCadTool_Pan:
                            View()->Pan(x - myXmax, myYmax - y );
                            myXmax = x;
                            myYmax = y;
                            break;
                    default:
                            myRubberBand->setGeometry(QRect(QPoint(myXmin, myYmin), e->pos()).normalized());
                            Redraw();
                            myRectangleMode = Standard_True;
                            break;
		}
	}
}


void QMcCad_View::LeaveEvent(QEvent*)
{
	MouseLeaveEvent() ;
}


void QMcCad_View::EnterEvent(QEvent*)
{
	MouseEnterEvent() ;
}


void QMcCad_View::FocusInEvent(QFocusEvent*)
{
}


void QMcCad_View::FocusOutEvent(QFocusEvent*)
{
}


void QMcCad_View::wheelEvent(QWheelEvent* e) //ZOOM
{
	int numSteps = e->delta();

	if(numSteps > 0)
		myAcM->SlotZoomIn();
	else if(numSteps < 0)
		myAcM->SlotZoomOut();
}


void QMcCad_View::keyPressEvent(QKeyEvent* e)
{
	if(e->key() == Qt::Key_Control)
		myControlDown = Standard_True;

	if(e->key() == Qt::Key_Delete)
	{
		Handle(AIS_InteractiveContext) theContext = myViewer->GetContext();
		if(theContext->NbCurrents() > 0)
			emit viewModified();
		myAcM->SlotErase();
	}
	if(e->key() == Qt::Key_Shift)
		myShiftSelect = Standard_True;
	else
		e->ignore();
}


void QMcCad_View::keyReleaseEvent(QKeyEvent* e)
{
	if(e->key()==Qt::Key_Control)
		myControlDown = Standard_False;
	if(e->key() == Qt::Key_Shift)
		myShiftSelect = Standard_False;
	else
		e->ignore();
}

void QMcCad_View::ClippingPlaneDialog()
{
	QMcCad_ClippingPlaneDialog cpDial(this/*QMcCad_Application::GetAppMainWin()*/);
	cpDial.SetInitialValues(myCPA, myCPB, myCPC, myCPD, myCPVisState);
	cpDial.exec();
}

void QMcCad_View::SetClippingPlaneParameters(Standard_Real a, Standard_Real b, Standard_Real c, Standard_Real d, Standard_Boolean state)
{
	myCPA = a;
	myCPB = b;
	myCPC = c;
	myCPD = d;
	myCPVisState = state;
}
