#include <QMcCad_Viewer.h>

//qiu #include <Graphic3d_GraphicDevice.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_InteractiveContext.hxx>

#include <QMcCad_Editor.h>
#include <QMcCad_View.h>

//qiu #include <Graphic3d_GraphicDevice.hxx>
#include <V3d_Viewer.hxx>
#include <Visual3d_Layer.hxx>

QMcCad_Viewer::QMcCad_Viewer(QMcCad_Editor* theEditor)
{
	myEditor = theEditor;
	myIsConfigured = false;

	Standard_ExtString aName, cName;
	TCollection_ExtendedString a3DName("Visu3D");
	TCollection_ExtendedString collectorName("Collector");
	aName=a3DName.ToExtString();
	cName=collectorName.ToExtString();
	Standard_CString aDomain("McCad");
	myViewer = MakeCasViewer(getenv("DISPLAY"), aName, aDomain, 1000.0,	V3d_XposYnegZpos, Standard_True, Standard_True); // McCadAEV_Viewer...
	myCollector = MakeCasViewer(getenv("DISPLAY"), cName, aDomain, 1000.0,	V3d_XposYnegZpos, Standard_True, Standard_True);
	myViewer->Init();
	myViewer->SetDefaultLights();
	myViewer->SetLightOn();
	/*myViewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);
	Standard_Real theW, theH;
	cout << theW << "  "  << theH << endl;
	myView->View()->Size(theW, theH);
	myViewer->SetRectangularGridGraphicValues(theW + 200, theH + 200, 0);*/
//	myViewer->SetDefaultVisualization(V3d_WIREFRAME);
    myContext = new AIS_InteractiveContext(myViewer, myCollector);
	myContext->SetHilightColor(Quantity_NOC_HOTPINK);
	//myContext->SelectionColor(Quantity_NOC_PURPLE);
    myContext->SelectionColor(Quantity_NOC_YELLOW);

   //make output look a little more smooth
	//no good performance for more complex models
        //in a later version the user should be able to set resolution as high/low as is neccessary for the system in use

    myContext->SetDeviationAngle(myContext->DeviationAngle() / 4.5);
    myContext->SetDeviationCoefficient(myContext->DeviationCoefficient() / 4.5);
    // Lei lu 25.11.2013
    //myContext->CloseAllContexts();
    //myContext->OpenLocalContext();
    //myContext->ActivateStandardMode(TopAbs_FACE);

    //myContext->SetHLRDeviationCoefficient(myContext->HLRDeviationCoefficient() / 4.5);         *
    //myContext->SetHLRAngle(myContext->HLRAngle() / 4.5);
}


QMcCad_Viewer::~QMcCad_Viewer()
{
	//  delete myViewer;
	// delete myView;
}



QMcCad_ActionMaster* QMcCad_Viewer::GetActionMaster()
{
	return myEditor->GetActionMaster();
}


QMcCad_View* QMcCad_Viewer::GetView()
{
	return myView;
}


void QMcCad_Viewer::SetView(QMcCad_View* theView)
{
	myView = theView;
}


void QMcCad_Viewer::Update()
{

}


void QMcCad_Viewer::Reconfigure()
{
	Handle_McCadCom_CasDocument theDoc = myEditor->GetDocument();
	if (myView == NULL)
	{
		Mcout("No view found!!!");
		myIsConfigured = false;
	}
	else
	{
		theDoc->Attach(myView);
		SetView(myView);
		myIsConfigured = true;
	}
}


QMcCad_Editor* QMcCad_Viewer::GetEditor()
{
	return myEditor;
}


void QMcCad_Viewer::Mcout(const QString & aMessage)
{
	myEditor->Mcout(aMessage);
}
