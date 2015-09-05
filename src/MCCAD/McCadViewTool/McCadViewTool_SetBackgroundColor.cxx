#include <McCadViewTool_SetBackgroundColor.ixx>
#include <V3d_View.hxx>
#include <Handle_V3d_View.hxx>
#include <qcolor.h>
#include <qcolordialog.h>
#include <AIS_DisplayMode.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <Visual3d_Layer.hxx>
#include <QMcCad_Application.h>

McCadViewTool_SetBackgroundColor::McCadViewTool_SetBackgroundColor(const Handle(McCadCom_CasDocument)& theDoc,const Handle(McCadCom_CasView)& theView,const McCadTool_State theState,const Standard_Boolean theUndoState,const Standard_Boolean theRedoState)
{
	myDoc = theDoc;
	 myView = theView;
	 myState = theState;
	 myUndoState = Standard_True;
	 myRedoState = theRedoState;
	 myID = McCadTool_BackgroundColor;
	 myPreviousColor = myView->View()->BackgroundColor();
}


Standard_Boolean McCadViewTool_SetBackgroundColor::IsNull()
{
	return Standard_False;
}

 void McCadViewTool_SetBackgroundColor::Destroy()
{
}

 void McCadViewTool_SetBackgroundColor::Execute()
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
		QMcCad_Application::GetAppMainWin()->SetBgColor(aRetColor);
		R1 = aRetColor.red()/255.;
		G1 = aRetColor.green()/255.;
		B1 = aRetColor.blue()/255.;
		aView->SetBackgroundColor(Quantity_TOC_RGB, R1, G1, B1);
	}

	aView->Redraw();
	 Done();
}

 void McCadViewTool_SetBackgroundColor::UnExecute()
{
	 if(myView==NULL)
	 {
		cout << "NULL VIEW\n";
		return;
	 }

	 cout << "UNDO\n";
	 myView->View()->SetBackgroundColor(myPreviousColor);
	 cout << "done\n";
}

 void McCadViewTool_SetBackgroundColor::Suspend()
{
}

 void McCadViewTool_SetBackgroundColor::Resume()
{
}

 void McCadViewTool_SetBackgroundColor::MousePressEvent(const McCadWin_MouseEvent& e)
{
}

 void McCadViewTool_SetBackgroundColor::MouseReleaseEvent(const McCadWin_MouseEvent& e)
{
}

 void McCadViewTool_SetBackgroundColor::MouseLeaveEvent()
{
}

 void McCadViewTool_SetBackgroundColor::MouseEnterEvent()
{
}

