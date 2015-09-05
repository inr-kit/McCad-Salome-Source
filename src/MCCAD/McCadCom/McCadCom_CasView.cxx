//qiu change the order of the inlude files
#include <AIS_InteractiveContext.hxx>
#include <Visual3d_Layer.hxx>
#include <McCadCom_CasView.ixx>

McCadCom_CasView::McCadCom_CasView()
{

	myXmin = 0;
	myYmin = 0;
	myXmax = 0;
	myYmax = 0;
	myTaskMaster = new McCadTool_TaskMaster(this);
	myIsInitialized = Standard_False;
}


McCadCom_CasView::McCadCom_CasView(const Handle(McCadCom_CasDocument)& theDocument)
{
	SetDocument(theDocument);
	myXmin = 0;
	myYmin = 0;
	myXmax = 0;
	myYmax = 0;
	myTaskMaster = new McCadTool_TaskMaster(this);
	myIsInitialized = Standard_False;
}

//qiu comment out because the Window in old McCad is not used any more
/*qiu 
void McCadCom_CasView::Init(const Handle(Xw_Window)& theWin)
{
	Handle(McCadCom_CasDocument) aDocument = Handle_McCadCom_CasDocument::DownCast(myDocument);
	Handle(AIS_InteractiveContext) aContext = aDocument->GetContext();
	myView = aContext->CurrentViewer()->CreateView();
	if ( !myView.IsNull() )
	{
		myIsInitialized = Standard_True;
		myView->SetWindow(theWin);
		if ( !theWin->IsMapped() )
			theWin->Map();

		myView->SetBackgroundColor(aDocument->GetBgColor());
		myView->MustBeResized();
		myView->SetTransparency(1);
		//myView->TriedronDisplay( Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.1, V3d_WIREFRAME );
	}
	else
		cout << "Warning:: Can not create CAS view !!!" << endl;
}
*/


Standard_Boolean McCadCom_CasView::IsInit() const
{
	return myIsInitialized;
}


void McCadCom_CasView::SetTaskMaster(const Handle(McCadTool_TaskMaster)& theTaskMaster)
{
	myTaskMaster = theTaskMaster;
}


Handle(McCadTool_TaskMaster) McCadCom_CasView::GetTaskMaster() const
{
	return myTaskMaster;
}


void McCadCom_CasView::SetDefaultTask(const Handle(McCadTool_Task)& theTask)
{
	myTaskMaster->SetDefaultTask(theTask);
}


Handle(McCadTool_Task) McCadCom_CasView::GetDefaultTask() const
{
        return myTaskMaster->GetDefaultTask();
}


void McCadCom_CasView::SetCurrentTask(const Handle(McCadTool_Task)& theTask)
{
	myTaskMaster->SetCurrentTask(theTask);
}


Handle(McCadTool_Task) McCadCom_CasView::GetCurrentTask() const
{
	return myTaskMaster->GetCurrentTask();
}


void McCadCom_CasView::Execute()
{
	myTaskMaster->Execute();
}


void McCadCom_CasView::UnExecute()
{
	myTaskMaster->UnExecute();
}


void McCadCom_CasView::Suspend()
{
	myTaskMaster->Suspend();
}

void McCadCom_CasView::Resume()
{
	myTaskMaster->Resume();
}


void McCadCom_CasView::SetState(const McCadTool_State theState)
{
	myTaskMaster->SetState(theState);
}


McCadTool_State McCadCom_CasView::State() const
{
        return myTaskMaster->State();
}


void McCadCom_CasView::MousePressEvent(const QMouseEvent& e)
{
	myTaskMaster->MousePressEvent(e);
}


void McCadCom_CasView::MouseReleaseEvent(const QMouseEvent& e)
{
	myTaskMaster->MouseReleaseEvent(e);
}


void McCadCom_CasView::MouseMoveEvent(const QMouseEvent& e)
{
	myTaskMaster->MouseMoveEvent(e);
}


void McCadCom_CasView::MouseLeaveEvent()
{
	myTaskMaster->MouseLeaveEvent();
}


void McCadCom_CasView::MouseEnterEvent()
{
	myTaskMaster->MouseEnterEvent();
}


Handle(V3d_View) McCadCom_CasView::View() const
{
	return myView;
}


void McCadCom_CasView::Redraw() const
{
	myView->Redraw();
}


void McCadCom_CasView::Reset() const
{
	myView->Reset();
}


void McCadCom_CasView::FitAll() const
{
	myView->FitAll();
	myView->ZFitAll();
	myView->Redraw();
}


void McCadCom_CasView::FitAll(const Standard_Integer Xmin,
		const Standard_Integer Ymin, const Standard_Integer Xmax,
		const Standard_Integer Ymax)
{
	myXmin = Xmin;
	myYmin = Ymin;
	myXmax = Xmax;
	myYmax = Ymax;
	myView->WindowFitAll(myXmin, myYmin, myXmax, myYmax);
}


void McCadCom_CasView::Back() const
{
	myView->SetProj(V3d_Xneg);
}


void McCadCom_CasView::Front() const
{
	myView->SetProj(V3d_Xpos);
}


void McCadCom_CasView::Top() const
{
	myView->SetProj(V3d_Zpos);
}


void McCadCom_CasView::Bottom() const
{
	myView->SetProj(V3d_Zneg);
}


void McCadCom_CasView::Left() const
{
	myView->SetProj(V3d_Ypos);
}


void McCadCom_CasView::Right() const
{
	myView->SetProj(V3d_Yneg);
}


void McCadCom_CasView::Iso() const
{
	myView->SetProj(V3d_XposYnegZpos);
}


void McCadCom_CasView::Pan(const Standard_Integer Dx, const Standard_Integer Dy)
{
	myView->Pan(Dx, Dy);
}


void McCadCom_CasView::Rotate(const Standard_Real Ax, const Standard_Real Ay,
		const Standard_Real Az)
{
	myView->StartRotation(Ax, Ay);
}


void McCadCom_CasView::Rotate(const Standard_Real Ax, const Standard_Real Ay,
		const Standard_Real Az, const Standard_Integer X,
		const Standard_Integer Y, const Standard_Integer Z)
{
}


void McCadCom_CasView::WindowFitAll(const Standard_Integer Xmin,
		const Standard_Integer Ymin, const Standard_Integer Xmax,
		const Standard_Integer Ymax)
{
	myXmin = Xmin;
	myYmin = Ymin;
	myXmax = Xmax;
	myYmax = Ymax;
	myView->WindowFitAll(myXmin, myYmin, myXmax, myYmax);
}


void McCadCom_CasView::Resize()
{
	myView->MustBeResized();
}


void McCadCom_CasView::Size(Standard_Integer& theWidth,
		Standard_Integer& theHeight)
{
}


void McCadCom_CasView::Zoom()
{
	myView->Zoom(myXmin, myYmin, myXmax, myYmax);
}


void McCadCom_CasView::SetBackgroundColor(const Quantity_Color& theColor)
{
	myView->SetBackgroundColor(theColor);
}


void McCadCom_CasView::SetBackgroundColor(const Standard_Real R,
		const Standard_Real G, const Standard_Real B)
{
}


Quantity_Color McCadCom_CasView::BackgroundColor() const
{
    Quantity_Color aColor(Quantity_NOC_WHITE);
    return aColor;
}


void McCadCom_CasView::SetShadingModel(const V3d_TypeOfShadingModel Model)
{
}


void McCadCom_CasView::SetVisualization(const V3d_TypeOfVisualization Mode)
{
}


void McCadCom_CasView::SetHiddenLineOn()
{
}


void McCadCom_CasView::SetHiddenLineOff()
{
}

void McCadCom_CasView::SetAntialiasingOn()
{
}


void McCadCom_CasView::SetAntialiasingOff()
{
}


void McCadCom_CasView::Dump(const Standard_CString aFile,
		const Aspect_FormatOfSheetPaper aFormat)
{
	myView->Redraw();
	Standard_Boolean dumped = myView->Dump(aFile);
	if (dumped)
		cout << "Window content has been printed to the File: " << aFile
				<< endl;
	else
		cout << "Printing failed !!" << endl;
}


void McCadCom_CasView::SetMinMaxPoints(const Standard_Integer Xmin,
		const Standard_Integer Ymin, const Standard_Integer Xmax,
		const Standard_Integer Ymax)
{
	myXmin = Xmin;
	myYmin = Ymin;
	myXmax = Xmax;
	myYmax = Ymax;
}


void McCadCom_CasView::SetMinPoints(const Standard_Integer Xmin,
		const Standard_Integer Ymin)
{
	myXmin = Xmin;
	myYmin = Ymin;
}


void McCadCom_CasView::SetMaxPoints(const Standard_Integer Xmax,
		const Standard_Integer Ymax)
{
	myXmax = Xmax;
	myYmax = Ymax;
}


void McCadCom_CasView::GetMinMaxPoints(Standard_Integer& Xmin,
		Standard_Integer& Ymin, Standard_Integer& Xmax, Standard_Integer& Ymax)
{
	Xmin = myXmin;
	Ymin = myYmin;
	Xmax = myXmax;
	Ymax = myYmax;
}


void McCadCom_CasView::GetMinPoints(Standard_Integer& Xmin,
		Standard_Integer& Ymin)
{
	Xmin = myXmin;
	Ymin = myYmin;
}


void McCadCom_CasView::GetMaxPoints(Standard_Integer& Xmax,
		Standard_Integer& Ymax)
{
	Xmax = myXmax;
	Ymax = myYmax;
}
