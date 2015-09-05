#include <QMcCad_Editor.h>
#include <OSD_Environment.hxx>
#include <QMcCad_VGParamDialog.h>

#include <STEPCAFControl_Reader.hxx>
#include <TDF_Label.hxx>
#include <Handle_XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_LabelSequence.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFPrs_Driver.hxx>
#include <TPrsStd_DriverTable.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_ChildIterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <XCAFDoc.hxx>
#include <Visual3d_Layer.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFPrs.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <AIS_LocalContext.hxx>
#include <QMcCadGeomeTree_TreeWidget.hxx>
#include <XCAFPrs_AISObject.hxx>
#include <McCadXCAF_TDocShapeLabelIterator.hxx>
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ListOfExtendedString.hxx>
#include <AIS_TexturedShape.hxx>
#include <McCadExDllStep_Step.hxx>

#include <Visual3d_ViewManager.hxx>


//*******************************************************************
TCollection_AsciiString ToAsciiString(const QString& theStr)
{
	char aStr[theStr.length()];
	QByteArray ba = theStr.toAscii();
	strcpy(aStr, ba.data());
	TCollection_AsciiString anAsciiStr(aStr);
	return anAsciiStr;
}

TCollection_AsciiString Filter(TCollection_AsciiString& aName)
{
	TCollection_AsciiString aFileName = aName;
	aFileName.LeftAdjust();
	Standard_Integer i = aFileName.SearchFromEnd(".");
	TCollection_AsciiString theFilter = aFileName.Split(i-1);
	return theFilter;
}
//******************************************************************

//CTOR
QMcCad_Editor::QMcCad_Editor(QWidget* theParent, const char* theName, const Standard_Integer& theID) :	QMainWindow(theParent)
{
	myID = theID;
	myIsLoaded = Standard_False;

	setFocusPolicy(Qt::ClickFocus);

  //Make sure no triangles are visual when Antialiasing applied
	OSD_Environment theEnv("CALL_OPENGL_ANTIALIASING_MODE","1");
	theEnv.Build();

  //QT-window options
	setMinimumSize(50, 50);
	resize(700,500);
	InitViewer();
	InitDocument();
	InitTDoc();
	myDoc->SetContext(myViewer->GetContext());

  //viewer
	QMcCad_View* theView = new QMcCad_View(this, myViewer, myDoc);
	QMcCadGeomeTree_TreeWidget* theTreeWidget = (QMcCad_Application::GetAppMainWin())->GetTreeWidget();

	myViewer->SetView(theView);
	if (!myViewer->IsConfigured())
		myViewer->Reconfigure();
	setCentralWidget(theView);
	setAttribute(Qt::WA_DeleteOnClose);


  //Signals/Slots
	connect(theView, SIGNAL(SigSelectionChanged()), theTreeWidget, SLOT(SlotVisuSelectionChanged()));
	connect(this, SIGNAL(keyPressed(QKeyEvent*)), theView, SLOT(keyPressEvent(QKeyEvent*)));
	connect(this, SIGNAL(keyReleased(QKeyEvent*)), theView, SLOT(keyReleaseEvent(QKeyEvent*)));
	connect(theView, SIGNAL(viewModified()), this, SLOT(SlotViewModified()));

	if(myID > 1)
	{
		TCollection_AsciiString aMessage("Editor No. ");
		aMessage += TCollection_AsciiString(myID);
		aMessage += " opened.";
		Mcout(aMessage.ToCString());
	}

    m_pMatManager = new MaterialManager();
}


void QMcCad_Editor::GradientBackground() // doesn't work :(
{
	/*cout << "1\n";
	Handle(V3d_View) V3dView = myViewer->GetView()->View();
	Handle_Visual3d_ViewManager viewManager = myViewer->GetViewer()->Viewer();

	Quantity_Color aTopColor (0, 0, 0, Quantity_TOC_RGB);
	Quantity_Color aBottomColor (1, 1, 1, Quantity_TOC_RGB);
	cout << "2\n";
//	if (aLayer.IsNull())
	//{
		Standard_Boolean aSizeDependant = Standard_True; //each window to have particular mapped layer?
		static Handle(Visual3d_Layer) aLayer = new Visual3d_Layer(viewManager, Aspect_TOL_UNDERLAY, aSizeDependant);
	//}

	int aWidth = myViewer->GetView()->width(), aHeight = myViewer->GetView()->height(); //e.g. QWidget::width() and height() for Qt-based apps
	aLayer->Clear(); //make sure we draw on a clean layer
	aLayer->Begin();
	aLayer->SetViewport(aWidth, aHeight);

	cout << aWidth << "  " << aHeight << endl;
	cout << "3\n";
	//now draw a polygon using top and bottom colors
	//remember that default boundary is [-1,1;-1,1] and origin is in the left bottom corner

	//check position for the middle color - if transition should be non-uniform then
	//additional points should be inserted and techiques changes - 2 polygons instead of 1
	aLayer->BeginPolygon();
	aLayer->SetColor (aTopColor);
	aLayer->AddVertex (-1,1);
	aLayer->AddVertex (1,1);
	cout << "4\n";
	aLayer->SetColor (aBottomColor);
	aLayer->AddVertex (1,-1);
	aLayer->AddVertex (-1,-1);
	aLayer->ClosePrimitive();
	aLayer->End();
	cout << "5\n";*/
}


QMcCad_Editor::~QMcCad_Editor() // DTOR
{
	delete myViewer;

    delete m_pMatManager;  // Add by Lei 23/10/2013
    m_pMatManager = NULL; 
}


Standard_Integer QMcCad_Editor::ID() //Get ID Number
{
	return myID;
}


void QMcCad_Editor::updateProgress(int a) //Update Progress Bar
{
/*	if(!myProgressBar->isVisible())
		myProgressBar->show();
	myProgressBar->setValue(a);
	if(a == 100)
	{
		myProgressBar->hide();
		myProgressBar->setValue(0);
		//myViewer->GetView()->View()->Redraw();
	}*/
}

QMcCad_ActionMaster* QMcCad_Editor::GetActionMaster()
{
	return QMcCad_Application::GetAppMainWin()->GetActionMaster();
}


void QMcCad_Editor::Mcout(const QString & aMessage, McCadMessenger_MessageType msgType)
{
	//QString theEdMess = "QMcCad_Editor: ";
	//theEdMess = theEdMess + aMessage;
        QMcCad_Application::GetAppMainWin()->Mcout(aMessage.toUtf8().constData(), msgType);
}


Handle(McCadCom_CasDocument) QMcCad_Editor::GetDocument()
{
	return myDoc;
}


QMcCad_View* QMcCad_Editor::GetView()
{
	if (myViewer != NULL)
		return myViewer->GetView();
	else
                Mcout("Missing Viewer", McCadMessenger_ErrorMsg);
	return NULL;
}


QMcCad_Viewer* QMcCad_Editor::GetViewer()
{
	if (myViewer != NULL)
		return myViewer;
	else
                Mcout("Missing Viewer", McCadMessenger_ErrorMsg);
	return NULL;
}

TDF_LabelSequence* QMcCad_Editor::GetLabelSequence()
{
	return &myLabelSequence;
}

//Traverses the document tree and prints it's structure
void Traverse( TDF_Label label , TCollection_AsciiString thePrefix )
{
	TCollection_AsciiString plusPrefix(" | - ");
	TCollection_AsciiString aString;
	TDF_Tool::Entry(label, aString);
	cout << aString.ToCString();

	Handle(TDataStd_Name) theName = new TDataStd_Name;
	Standard_GUID theNameGUID = TDataStd_Name::GetID();
	Standard_Boolean HasAttribute = label.FindAttribute(theNameGUID, theName);
	if(HasAttribute)
	{
		cout << " :  ";
		cout << "\"" << theName->Get() << "\"";
	}

	cout << endl;
	for( TDF_ChildIterator it( label, Standard_False ); it.More(); it.Next() )
	{
		cout << thePrefix.ToCString();
		Traverse( it.Value() , plusPrefix+thePrefix );
	}
}


// Open/Load/Import a file to TDoc
void QMcCad_Editor::Import(const QString& theFileName)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	TCollection_AsciiString aName = ToAsciiString(theFileName);
	TCollection_AsciiString aFileFilter = Filter(aName);

	OSD_OpenMode aMode = OSD_ReadOnly;
	OSD_Protection aProtect(OSD_RW, OSD_RW, OSD_RW, OSD_RW);
	OSD_Path aPath(aName);
	OSD_File aFile(aPath);
	aFile.Append(aMode, aProtect);

	if (!(aFile.Exists() && aFile.IsOpen()))
	{
        Mcout("Can not open the file: ", McCadMessenger_ErrorMsg);
		Mcout("theFileName");
		return;
	}

	McCadGUI_GuiFactoryPtr theGuiFactory = McCadGUI_GuiFactory::Instance();
	theGuiFactory->SetType(McCadGUI_Base);

	McCadEXPlug_ExchangePlugin *theReader;

	if (QMcCad_Application::GetAppMainWin()->CheckExtension(aFileFilter))
	{
        theReader = QMcCad_Application::GetAppMainWin()->GetPlugin(aFileFilter);
        theReader->SetFilename(aName);
	}
	else
	{
        Mcout("Can't read file, no reader available for the this file: ", McCadMessenger_ErrorMsg);
		Mcout(theFileName);
		QApplication::restoreOverrideCursor();
		return;
	}

	///////////////////////////////////////////////////////////////////
	//read data and store file name
//MYMOD

	//IMPORT file

    Handle(TDocStd_Document) theTDoc = myDoc->GetTDoc();
    QApplication::restoreOverrideCursor();
    if(!theReader->ImportToDocument(theTDoc))
    {
        QString theError(" could not be loaded, or import has aborted.");
        theError.prepend(theFileName);
        Mcout(theError, McCadMessenger_ErrorMsg);
    }

    //ctrl stays locked after ctrl+i; here we release it manualy
    QKeyEvent* releaseCtrl = new QKeyEvent(QEvent::KeyRelease, Qt::Key_Control, Qt::NoModifier);
    keyReleaseEvent(releaseCtrl);

    QApplication::restoreOverrideCursor();
}


bool QMcCad_Editor::Export(const TCollection_AsciiString& theFileName)
{
	Handle(TDocStd_Document) theTDoc = myDoc->GetTDoc();
	mySaveName = theFileName;
	TCollection_AsciiString aFileFilter = Filter(mySaveName);

	Handle(AIS_InteractiveContext) theContext = myDoc->GetContext();

	OSD_OpenMode aMode = OSD_ReadOnly;
	OSD_Protection aProtect(OSD_RW, OSD_RW, OSD_RW, OSD_RW);
	OSD_Path aPath(mySaveName);
	OSD_File aFile(aPath);
	aFile.Append(aMode, aProtect);

	if (!(aFile.Exists() && aFile.IsOpen()))
	{
        Mcout("Can not open the file: ", McCadMessenger_ErrorMsg);
		Mcout(theFileName.ToCString());
		return false;
	}

	McCadGUI_GuiFactoryPtr theGuiFactory = McCadGUI_GuiFactory::Instance();
	theGuiFactory->SetType(McCadGUI_Base);

	//The Writer
	////////////////////////////////////////
	McCadEXPlug_ExchangePlugin *theWriter;

	if (QMcCad_Application::GetAppMainWin()->CheckExtension(aFileFilter))
	{
		theWriter = QMcCad_Application::GetAppMainWin()->GetPlugin(aFileFilter);
		theWriter->SetFilename(mySaveName);
	}
	else
	{
		TCollection_AsciiString theMessage("Can't write file, no writer available for this file: ");
		theMessage += theFileName;
		Mcout(theMessage.ToCString());

		return false;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);

	//Prepare TDoc
	///////////////////////////////////////////
	Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());

	McCadXCAF_TDocShapeLabelIterator it(theTDoc);
	for( ; it.More(); it.Next())
	{
		TDF_Label curL = it.Current();

		if(sTool->IsAssembly(curL)) //assemblies are not shown, only substructures
			continue;

		Handle(TPrsStd_AISPresentation) prs;

		if ( curL.FindAttribute ( TPrsStd_AISPresentation::GetID(), prs ) ) //remove all deleted objects permanently from TDoc
		{
			Handle(AIS_InteractiveObject) anAISObject = prs->GetAIS();

			if(!theContext->IsDisplayed(anAISObject) /*&& !theContext->IsInCollector(anAISObject)*/)
				curL.ForgetAllAttributes(1);
		}
	}

	//Export
    if(!theWriter->ExportFromDocument(theTDoc))
	{
        Mcout("Could not export Document!", McCadMessenger_ErrorMsg);
        return false;
	}

	QApplication::restoreOverrideCursor();
	myDoc->SetDocName(theFileName);
	myDoc->SetIsModified(Standard_False);
	Mcout("File saved...");

	return true;
}


void QMcCad_Editor::UpdateView()
{
    Handle(AIS_InteractiveContext) theContext = myDoc->GetContext();

    //theContext->CloseLocalContext();

	Handle(TDocStd_Document) theTDoc = myDoc->GetTDoc();
	TCollection_AsciiString dummy;
	Traverse(theTDoc->Main(), dummy);

	//Init Visualization Drivers
	/*Handle(XCAFPrs_Driver) aPrsDriver = new XCAFPrs_Driver();
	TPrsStd_DriverTable::Get();
	TPrsStd_DriverTable::Get()->InitStandardDrivers();
	TPrsStd_DriverTable::Get()->AddDriver(XCAFPrs_Driver::GetID(), aPrsDriver);*/

	//print document structure
	//TCollection_AsciiString dummy;
	//Traverse(theTDoc->Main(), dummy);

	//Update Visualization
	Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
	Handle(XCAFDoc_ColorTool) cTool = XCAFDoc_DocumentTool::ColorTool(theTDoc->Main());

	McCadGUI_ProgressDialogPtr theProgress = new McCadGUI_ProgressDialog(QMcCad_Application::GetAppMainWin());
	Handle(Message_ProgressIndicator) thePI = Handle(Message_ProgressIndicator)::DownCast(theProgress);
	theProgress->SetLabelText("Updating View.");
	theProgress->SetCancelButtonText("Cancel");
	theProgress->SetTotalSteps(100);
	theProgress->SetProgress(0);
	TDF_LabelSequence labSeq;
	sTool->GetFreeShapes(labSeq);
	thePI->SetStep(100.5/labSeq.Length());

	srand ( time(NULL) );    

    Standard_Real r,g,b;
    //r = (rand() % 255) / 256.0;
    //g = (rand() % 255) / 256.0;
    //b = (rand() % 255) / 256.0;
    TCollection_AsciiString theName;
    TCollection_AsciiString labelName;
    Standard_GUID theNameGUID = TDataStd_Name::GetID();
    Handle(TDataStd_Name) tmpName;

    McCadXCAF_TDocShapeLabelIterator it(theTDoc);
    TCollection_AsciiString strGroupName("");
    Quantity_Color theCol;          // the display color for each group
    for( ; it.More(); it.Next())    //iterate through all toplevel shapes and children
	{
		thePI->Increment();
		if(thePI->UserBreak())
			return;

		TDF_Label curL = it.Current();
		Handle(TNaming_NamedShape) nmdShp;

		if(!curL.FindAttribute(TNaming_NamedShape::GetID(), nmdShp))
			continue;

		// skip already registered labels
		Handle(TDataStd_BooleanList) bList;
		if(curL.FindAttribute(TDataStd_BooleanList::GetID(), bList) && !myIsLoaded)
			continue;

		if(!myIsLoaded)
		{
			bList = new TDataStd_BooleanList;
			curL.AddAttribute(bList);
		}

		// visualize ...
		Handle(AIS_Shape) aisShp = new AIS_Shape(nmdShp->Get());
//		Handle(AIS_TexturedShape) aisShp = new AIS_TexturedShape(nmdShp->Get());
/*		TCollection_AsciiString tFileName("/home/grosse/Testarea/TexturedShapes/myface.bmp");
		aisShp->SetTextureFileName(tFileName);
		aisShp->SetTextureMapOn();
		aisShp->SetTextureRepeat(true,2, 1);
        aisShp->SetTextureScale(0.1,0.1);
        aisShp->SetDisplayMode(3);*/

        TDF_Tool::Entry(curL, labelName);
        if(curL.FindAttribute(theNameGUID, tmpName))
            theName = tmpName->Get();
        else
            theName = labelName;

        // theName.Remove(theName.SearchFromEnd(" "));
        if (!theName.IsEqual(strGroupName)) // @color Lei Lu 10/10/2013
        {
            r = ((rand() % 236)+10) / 256.0;
            g = ((rand() % 236)+10) / 256.0;
            b = ((rand() % 236)+10) / 256.0;
            strGroupName = theName;
            theCol.SetValues(r,g,b, Quantity_TOC_RGB);
        }
//        if(cTool->GetColor(curL, XCAFDoc_ColorGen ,cLab))
//        {
//            cTool->GetColor(curL, XCAFDoc_ColorGen, theCol);
//        }
//        else
//        {
//            // random color
//            //theCol.SetValues(0.12,0.77,1,Quantity_TOC_RGB);
//            /* Standard_Real r,g,b;*/
//            //r = (rand() % 255) / 256.0;
//            //g = (rand() % 255) / 256.0;
//            //b = (rand() % 255) / 256.0;

//        }

//		cTool->GetColor(curL, theCol);

        aisShp->SetColor(theCol);
        aisShp->SetMaterial(Graphic3d_NOM_PLASTIC);
        aisShp->SetDisplayMode(AIS_Shaded);
        aisShp->SetWidth(2.);

        theContext->Display(aisShp, Standard_False);
//		theContext->SetMaterial(aisShp, Graphic3d_NOM_PLASTIC);
//		theContext->UpdateCurrentViewer();
//		aisShp->UpdateAttributes();

		//Handle(TPrsStd_AISPresentation) prs;

		/*if ( ! curL.FindAttribute ( TPrsStd_AISPresentation::GetID(), prs ) && ! sTool->IsAssembly(curL))
		{
			prs = TPrsStd_AISPresentation::Set(curL,XCAFPrs_Driver::GetID());
			prs->SetMode(1);*/

			//prs->Display(Standard_True);
			//prs->SetMaterial(Graphic3d_NOM_PLASTIC);
			//theContext->Display(prs->GetAIS(), 0);
			//theContext->HilightWithColor(prs->GetAIS(),Quantity_NOC_BLACK,0);
		//}
	}
	myIsLoaded = Standard_False;
	theContext->UpdateCurrentViewer();
	GetView()->FitAll();
}


/* *
 * save and export functions
 * */
bool QMcCad_Editor::Save()
{
	TCollection_ExtendedString saveName(myDoc->GetDocName()); //get name

	if(saveName.SearchFromEnd(".xml") < 0 || saveName.SearchFromEnd(".xml") > (saveName.Length()-3)) //check extension
		saveName += TCollection_ExtendedString(".xml");

	return Save(saveName);
}


bool QMcCad_Editor::Save(const TCollection_AsciiString& saveName) // save function
{
	TCollection_ExtendedString outString;
	Handle(TDocStd_Application) theXCAFApp = QMcCad_Application::GetAppMainWin()->GetXCAFApp();
	QApplication::setOverrideCursor(Qt::WaitCursor);
#ifdef OCC650
	CDF_StoreStatus theStat;
#else
	PCDM_StoreStatus theStat;
#endif
	theStat = theXCAFApp->SaveAs(myDoc->GetTDoc(), saveName, outString);
	QApplication::restoreOverrideCursor();

#ifdef OCC650
	if(theStat==CDF_SS_DriverFailure)
                Mcout("PCDM_SS_DriverFailure", McCadMessenger_ErrorMsg);
	if(theStat==CDF_SS_WriteFailure)
                Mcout("PCDM_SS_WriteFailure", McCadMessenger_ErrorMsg);
	if(theStat==CDF_SS_Failure)
                Mcout("PCDM_SS_Failure", McCadMessenger_ErrorMsg);
#else
	if(theStat==PCDM_SS_DriverFailure)
                Mcout("PCDM_SS_DriverFailure", McCadMessenger_ErrorMsg);
	if(theStat==PCDM_SS_WriteFailure)
                Mcout("PCDM_SS_WriteFailure", McCadMessenger_ErrorMsg);
	if(theStat==PCDM_SS_Failure)
                Mcout("PCDM_SS_Failure", McCadMessenger_ErrorMsg);
#endif


	cout << "SAVE FILE AS : " << saveName.ToCString() << endl;
	TCollection_AsciiString statMess(outString);
	cout << "StatusMessage: " << statMess.ToCString() << endl;

	myDoc->SetDocName(saveName);
	myDoc->SetIsModified(Standard_False);
	UpdateWindowTitle();
	setWindowModified(false);
	return true;
}


/* *
 * set functions
 * */

void QMcCad_Editor::SetDocument(Handle(McCadCom_CasDocument) theDoc)
{
	myDoc = theDoc;
}


void QMcCad_Editor::SetView(QMcCad_View* theView)
{
	if (myViewer != NULL)
		return myViewer->SetView(theView);
	else
                Mcout(QString("Missing Viewer"), McCadMessenger_ErrorMsg);
}


void QMcCad_Editor::SetViewer(QMcCad_Viewer* theViewer)
{
	myViewer = theViewer;
}


void QMcCad_Editor::SetID(Standard_Integer theID) // Set ID number
{
	myID = theID;
}



/* *
 * Init functions
 * */

void QMcCad_Editor::InitDocument()
{
	//if (myViewer != NULL)
	//{
		//Handle(AIS_InteractiveContext) theContext = myViewer->GetContext();
		//myDoc = new McCadCom_CasDocument(theContext);
		//myDoc->NewDocument(myID);
	//}
	//else
	//	Mcout(QString("No viewer found. Editor could not be created."));
	myDoc = new McCadCom_CasDocument(myID);
	QColor theBgColor = QMcCad_Application::GetAppMainWin()->GetBgColor();
	Quantity_Color bgCol(theBgColor.red()/255., theBgColor.green()/255., theBgColor.blue()/255., Quantity_TOC_RGB);
	myDoc->SetBgColor(bgCol);
}


void QMcCad_Editor::InitDocument(Handle(McCadCom_CasDocument) theDoc)
{
	if(myViewer != NULL)
	{
		Handle(AIS_InteractiveContext) theContext = myViewer->GetContext();
		theDoc->SetContext(theContext);
		myDoc = theDoc;
	}
	else
        Mcout(QString("Can not create a Document"), McCadMessenger_ErrorMsg);
}


void QMcCad_Editor::SetIsLoaded(Standard_Boolean status)
{
	myIsLoaded = status;
}


void QMcCad_Editor::InitTDoc()
{
	Handle(TDocStd_Document) theTDoc; // = new TDocStd_Document("XmlXCAF");
	/*if(!QMcCad_Application::GetAppMainWin()->NewDocument(theTDoc));
		return;*/ // crashes ...
	Handle(TDocStd_Application) theXCAFApp = QMcCad_Application::GetAppMainWin()->GetXCAFApp();
	theXCAFApp->NewDocument("XmlXCAF", theTDoc); //initialize Document
	//theXCAFApp->NewDocument("McCad_Document", theTDoc);

	if(!XCAFDoc_DocumentTool::IsXCAFDocument(theTDoc))
	{
		cout << "Couldn't initialize Document. Not a XmlXCAF Doc!!!\n";
                Mcout("Couldn't initialize Document. Not a XmlXCAF Doc!!!\n", McCadMessenger_ErrorMsg);
		return;
	}

	//Undo
	theTDoc->SetUndoLimit(50);

	//XDE Viewer // DOESN'T WORK FOR ME ... WHY ???
/*	Handle(TPrsStd_AISViewer) aTViewer;
	TDF_Label rootLabel = theTDoc->GetData()->Root();

	if (!TPrsStd_AISViewer::Find (rootLabel, aTViewer))
	{
		Handle(AIS_InteractiveContext) theIC = myViewer->GetContext();
		TPrsStd_AISViewer::New(rootLabel, theIC);
	}
	else
		Mcout("TPrsStd_AISViewer already attached to the root label!");
*/
    //Set label for MCards  21.11.2013, Lei Lu
    /*Handle(TDataStd_Name) theMCardName;
	Handle(TDF_Data) theData = theTDoc->GetData();
	TDF_Label theLab;
	TCollection_AsciiString mCardLab("0:1:1:1:6");

	TDF_Tool::Label(theData, mCardLab, theLab, Standard_True); //create new Label for MCards

	if(theLab.IsNull())
        Mcout("MCard Label could not be set.", McCadMessenger_ErrorMsg);
	else
        theMCardName->Set(theLab, "MCards");*/

    GetDocument()->SetTDoc(theTDoc);
}


void QMcCad_Editor::InitViewer()
{
	myViewer = new QMcCad_Viewer(this);
}


void QMcCad_Editor::Hardcopy(const QString& theFileName)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	if (myViewer != NULL)
	{
		TCollection_AsciiString aFileName = ToAsciiString(theFileName);
		myViewer->GetView()->Dump(aFileName.ToCString(), Aspect_FOSP_A4);
		Mcout("Picture written into file:");
		Mcout(theFileName);
	}
	else
                Mcout("Missing Viewer", McCadMessenger_ErrorMsg);
	QApplication::restoreOverrideCursor();
}


void QMcCad_Editor::MakeRotationMoviePictures()
{
	/*Standard_Integer i(0), j(0);

	for(j=1; j<= 200; j++)
	{
		TCollection_AsciiString nm("pic");
		myViewer->GetView()->View()->SetZoom(1.01);
		myViewer->GetView()->View()->Redraw();
		TCollection_AsciiString num;
		if(j<10)
			num += "0";
		if(j<100)
			num += "0";

		num += j;
		nm += num;
		nm += ".bmp";
		myViewer->GetView()->Dump(nm.ToCString(), Aspect_FOSP_A4);
	}

	for(i=j; i<=j+360; i++)
	{
		TCollection_AsciiString nm("pic");
		// perform rotation
		Standard_Real ang(1*PI/180.);
		myViewer->GetView()->View()->Rotate(V3d_Z, ang, 858.35, -47.075, 49.875);
		myViewer->GetView()->View()->Redraw();

		// dump view
		TCollection_AsciiString num;
		if(i<10)
			num += "0";
		if(i<100)
			num += "0";

		num += i;
		nm += num;
		nm += ".bmp";
		myViewer->GetView()->Dump(nm.ToCString(), Aspect_FOSP_A4);
	}*/
}


void QMcCad_Editor::Print(QPrinter* thePrinter)
{
	/*
	 static QString filter = "Images Files (*.bmp *.gif *.xwd)";
	 QFileDialog fd (this,"McCad Printer",filter);
	 int ret = fd.exec();

	 // update the desktop after the dialog is closed
	 // QApplication::processEvents();
	 QString file (ret == QDialog::Accepted ? fd.selectedFile() : QString::null);
	 if ( !file.isNull() )
	 {
	 QApplication::setOverrideCursor(Qt::WaitCursor);
	 if ( !QFileInfo( file ).extension().length() )
	 file += QString( ".bmp" );

	 }
	 */
	// bool res = GetView()->dump((Standard_CString)file.latin1());
	/*
	 bool res = false;
	 QApplication::restoreOverrideCursor();
	 if ( !res )
	 {
	 // QMessageBox::information ( QApplication::mainWidget(),tr("TIT_ERROR"), tr("INF_ERROR"), tr("BTN_OK"),
	 QString::null, QString::null, 0, 0 );
	 // QApplication::processEvents();
	 // }
	 }
	 */
	cout << "NOT YET IMPLEMENTED\n";
}


void QMcCad_Editor::closeEvent(QCloseEvent *e)
{
	QString closedMessage("Editor No. ");
	TCollection_AsciiString id(myID);
	id += " closed.";
	closedMessage.append(id.ToCString());

	if (myDoc->IsSetModified() )
	{
		TCollection_AsciiString theName = myDoc->GetDocName();

		theName.Prepend("\"");
		theName+=TCollection_AsciiString("\"");

		QString addText = tr(" has been modified!\nSave changes?");
		QString saveText = theName.ToCString() + addText;

		switch (QMessageBox::warning(this, tr("Save Changes"), saveText, tr("Yes"), tr("No"), tr("Cancel") ))
		{
		case 0:
			if(Save())
			{
				Mcout(closedMessage);
				e->accept();
				emit destroyed(myID);
			}
			else
				e->ignore();
			break;
		case 1:
			Mcout(closedMessage);
			e->accept();
			emit destroyed(myID);
			break;
		default:
			e->ignore();
			break;
		}
	}
	else
	{
		Mcout(closedMessage);
		emit destroyed(myID);
		e->accept();
	}
}


QString QMcCad_Editor::GetQFileName()
{
	TCollection_AsciiString aName(myDoc->GetDocName());
	QString qName(aName.ToCString());
	return qName;
}

void QMcCad_Editor::SetCurrentFilter(QString theFilter)
{
	myCurrentFilter = theFilter;
}


QString QMcCad_Editor::GetCurrentFilter()
{
	return myCurrentFilter;
}


void QMcCad_Editor::UpdateWindowTitle()
{
	TCollection_AsciiString docName = myDoc->GetDocName();
	if(docName.Search("/")>0)
		docName.Remove(1,docName.SearchFromEnd("/"));
	/*if(docName.Search(".")>0)
		docName.Remove(docName.SearchFromEnd("."), docName.Length()-docName.SearchFromEnd(".")+1);*/
	QString theTitle(docName.ToCString() + QString("[*]"));
	setWindowTitle(theTitle);
}


void QMcCad_Editor::keyPressEvent(QKeyEvent* e)
{
	emit keyPressed(e);
}

void QMcCad_Editor::keyReleaseEvent(QKeyEvent* e)
{
	emit keyReleased(e);
}

void QMcCad_Editor::SlotViewModified()
{
	myDoc->SetIsModified(Standard_True);
	setWindowModified(true);
}

MaterialManager* QMcCad_Editor::GetMatManager()
{
    return m_pMatManager;
}


/** ********************************************************************
* @brief
* @param
* @return
*
* @date 21/11/2013
* @author  Lei Lu
***********************************************************************/
Standard_Boolean QMcCad_Editor::AddTreeItem(TCollection_AsciiString theLabel, QTreeWidgetItem *& pItem)
{
    if(!m_IDLabelItemMap.contains(theLabel))
    {
        m_IDLabelItemMap.insert(theLabel,pItem);
    }
    else
    {
        return Standard_False;
    }

    if(!m_ItemSolidMap.contains(pItem))
    {
        m_ItemSolidMap.insert(pItem,theLabel);
    }
    else
    {
        return Standard_False;
    }
}



/** ********************************************************************
* @brief
* @param
* @return
*
* @date 21/11/2013
* @author  Lei Lu
***********************************************************************/
void QMcCad_Editor::DeleteSelectedItem(TCollection_AsciiString theLabel)
{
    QTreeWidgetItem *pItem = m_IDLabelItemMap[theLabel];
    m_IDLabelItemMap.remove(theLabel);
    m_ItemSolidMap.remove(pItem);
    delete pItem;
    pItem = NULL;
}


/** ********************************************************************
* @brief
* @param
* @return
*
* @date 21/11/2013
* @author  Lei Lu
***********************************************************************/
TCollection_AsciiString QMcCad_Editor::GetSolidFromItem(QTreeWidgetItem *& pItem)
{
    if(m_ItemSolidMap.contains(pItem))
    {
        return m_ItemSolidMap[pItem];
    }
    else
    {
        return "";
    }
}



/** ********************************************************************
* @brief
* @param
* @return
*
* @date 21/11/2013
* @author  Lei Lu
***********************************************************************/
QTreeWidgetItem * QMcCad_Editor::GetTreeItem(TCollection_AsciiString theLabel)
{
    if(m_IDLabelItemMap.contains(theLabel))
    {
        QTreeWidgetItem * pItem = NULL;
        pItem = m_IDLabelItemMap[theLabel];
        return pItem;
    }
}



/** ********************************************************************
* @brief
* @param
* @return
*
* @date 21/11/2013
* @author  Lei Lu
***********************************************************************/
void QMcCad_Editor::DeleteTreeItem()
{
    QMap<TCollection_AsciiString, QTreeWidgetItem*>::const_iterator iter;
    for(iter = m_IDLabelItemMap.constBegin(); iter != m_IDLabelItemMap.constEnd(); ++iter)
    {
        delete iter.value();
    }

    m_ItemSolidMap.clear();
}



