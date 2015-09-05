#include <QtGui/QtGui>
// #include <QtAssistant/QAssistantClient>

#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#ifndef _OSD_Directory_HeaderFile
#include <OSD_Directory.hxx>
#endif

#include <QMcCad_Application.h>
#include <QMcCad_ActionFactory.h>
#include <QMcCad_ActionMaster.h>
#include <QMcCad_Editor.h>
#include <QMcCad_View.h>

#include <ui_QMcCad_TransparencyDialog.h>
#include <QMcCadGeomeTree_TreeWidget.hxx>

#include <McCadXCAF_Application.hxx>

#include <XCAFDoc_DocumentTool.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <XCAFPrs_Driver.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <TDF_Tool.hxx>
#include <McCadConfig.h>

#include <McCadXCAF_TDocShapeLabelIterator.hxx>

//extern QString theProgName;
// QAssistantClient* QMcCad_Application::myAssistant = NULL;
QMcCad_Application* QMcCad_Application::myApp = NULL;


TCollection_AsciiString ToAStr(const QString& theStr)
{
	char aStr[theStr.length()];
	QByteArray ba = theStr.toAscii();
	strcpy(aStr, ba.data());
	TCollection_AsciiString anAsciiStr(aStr);
	return anAsciiStr;
}

QMcCad_Application::QMcCad_Application(QWidget *parent, const char *name) :	QMainWindow(parent)
{
	if(name);

	myXCAFApp = McCadXCAF_Application::GetApplication(); //create XCAF application to use XDE and OCAF from OpenCascade

	myApp = this;
	myPrinter = new QPrinter( QPrinter::HighResolution );
	myAcM = new QMcCad_ActionMaster();

	myWS = new QWorkspace(this);
	setCentralWidget(myWS);

	connect(myWS, SIGNAL(windowActivated(QWidget *)),this, SLOT(SlotUpdateMenus()));

	OSD_Directory theResourceDir = ResourceDir();
	OSD_Path theResourcePath;
	theResourceDir.Path(theResourcePath);
	TCollection_AsciiString aPathName;
	theResourcePath.SystemName(aPathName);
	QString theDocumentDir(aPathName.ToCString());
	theDocumentDir = theDocumentDir+"/doc";

	// if (!myAssistant)
	//	myAssistant = new QAssistantClient( QDir( theDocumentDir).absolutePath(), this );
	CreateActions();
	CreateMenus();
	CreateToolBars();
	CreateStatusBar();
	CreateLogWindow();
	CreateTreeView();
	ReadSettings();
	SlotUpdateMenus();
	myEditorIDCnt = 0;

	if(myWorkingDirectory.isEmpty())
		myWorkingDirectory = ".";

	setWindowTitle(tr("McCad"));
	setWindowIcon(QIcon(":images/icon.png"));
	Mcout("McCad_Ready:");
	showMaximized();

	SlotFileNew();

        McCadMessenger_ObserverInterface *qmessenger = new QMcCadMessenger_MainWindowMessage;
        m_messenger = McCadMessenger_Singleton::Instance();
        m_messenger->Register(qmessenger);

        Mcout("Please notice: Warnings and error messages are not yet fully available via the GUI. Please run McCad from inside a shell to have all available debug messages printed out.", McCadMessenger_WarningMsg);
}


Handle(TDocStd_Application) QMcCad_Application::GetXCAFApp()
{
	return myXCAFApp;
}


QWorkspace* QMcCad_Application::GetWorkSpace()
{
	return myWS;
}


QMcCad_Editor* QMcCad_Application::GetEditor()
{
	if (myWS != NULL)
	{
		return (QMcCad_Editor*)myWS->activeWindow();
	}
	else
	{
		return NULL;
	}
}


QColor QMcCad_Application::GetBgColor()
{
	return myBgCol;
}

QMcCad_Editor* QMcCad_Application::GetEditor(int theID)
{
	return myIDEditorMap[theID];
}


QMcCad_View* QMcCad_Application::GetView()
{
	QMcCad_Editor* theEditor = GetEditor();
	if (theEditor !=NULL)
	{
		return theEditor->GetView();
	}
	return NULL;
}


Handle(McCadCom_CasDocument) QMcCad_Application::GetDocument()
{
	QMcCad_Editor* theEditor = GetEditor();
	if (theEditor !=NULL)
	{
		return theEditor->GetDocument();
	}
	return NULL;
}


Standard_Boolean QMcCad_Application::NewDocument(Handle(TDocStd_Document)& theTDoc)
{
	myXCAFApp->NewDocument("XmlXCAF", theTDoc);

	if(!XCAFDoc_DocumentTool::IsXCAFDocument(theTDoc))
	{
                Mcout("Couldn't initialize Document. Not a XmlXCAF Doc!!!\n", McCadMessenger_ErrorMsg);
		return Standard_False;
	}

	return Standard_True;
}


void QMcCad_Application::SetCurrentFile(const QString &fileName)
{
	myCurrentFileName = fileName;
	//setWindowModified(false);

	QString winName = "Untitled";
	if (!myCurrentFileName.isEmpty())
	{
		winName = StrippedName(myCurrentFileName);
		myRecentFiles.removeAll(myCurrentFileName);
		myRecentFiles.prepend(myCurrentFileName);
		UpdateRecentFileItems();
	}
	//setWindowTitle(tr("%1[*] - %2").arg(winName).arg(tr("McCad")));
}


void QMcCad_Application::SetBgColor(const QColor theCol)
{
	myBgCol = theCol;
}


QString QMcCad_Application::GetCurrentFile()
{
	return myCurrentFileName;
}


QString QMcCad_Application::StrippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}


void QMcCad_Application::UpdateRecentFileItems()
{
	QMutableStringListIterator iter(myRecentFiles);
	while (iter.hasNext())
	{
		if (!QFile::exists(iter.next()))
			iter.remove();
	}

	for (int j = 0; j < MaxRecentFiles; ++j)
	{
		if (j < myRecentFiles.count())
		{
			QString text = tr("&%1 %2").arg(j + 1).arg(StrippedName(myRecentFiles[j]));
			myRecentFileActions[j]->setText(text);
			myRecentFileActions[j]->setData(myRecentFiles[j]);
			myRecentFileActions[j]->setVisible(true);
		}
		else
		{
			myRecentFileActions[j]->setVisible(false);
		}
	}
}


void QMcCad_Application::OpenRecentFile()
{
	QAction *theAction = qobject_cast<QAction *>(sender());
	if (theAction)
	{
		QString theName(theAction->data().toString());
		SlotFileOpen(theName);
		/*QMcCad_Editor* theEditor = GetEditor();
		if (theEditor != NULL)
		{
			theEditor->Import(theAction->data().toString());
		}
		else
		{
			SlotFileNew();
			theEditor = GetEditor();
			if (theEditor !=NULL)
			{
				theEditor->Import(theAction->data().toString());
			}
			else
				statusBar()->showMessage("Loading aborted", 2000);
		}*/
	}
}


/*void QMcCad_Application::Mcout(const QString & aMessage, McCadGUI_MessageType msgType)
{
	if(msgType == McCadGUI_DefaultMsg)
		myOut->Append(aMessage);
	else if(msgType == McCadGUI_WarningMsg)
		myOut->Warning(aMessage);
	else if(msgType == McCadGUI_ErrorMsg)
		myOut->Error(aMessage);
	else if(msgType == McCadGUI_HeadingMsg)
		myOut->Heading(aMessage);
	else
		myOut->Append(aMessage);

}*/

void QMcCad_Application::Mcout(const std::string& message, McCadMessenger_MessageType msgType)
{
        QString qMessage(message.c_str());
        if(msgType == McCadMessenger_DefaultMsg)
                myOut->Append(qMessage);
        else if(msgType == McCadMessenger_WarningMsg)
                myOut->Warning(qMessage);
        else if(msgType == McCadMessenger_ErrorMsg)
                myOut->Error(qMessage);
        else if(msgType == McCadMessenger_HeadingMsg)
                myOut->Heading(qMessage);
        else
                myOut->Append(qMessage);

}





void QMcCad_Application::WriteSettings()
{
	// prepare recent files for saving
	QString recentFiles;
	for(int i=0; i<myRecentFiles.count(); i++)
	{
		recentFiles += myRecentFiles[i];
		if(i < myRecentFiles.count()-1)
			recentFiles += ";;";
	}

	QString strSettingName("McCad_");
	strSettingName = strSettingName + QString::number(MCCAD_VERSION_MAJOR) + QString(".") + QString::number(MCCAD_VERSION_MINOR);

	// save settings
        QSettings settings("Karlsruher_Institut_fuer_Technologie", strSettingName);
	settings.beginGroup("MainWindow");
		/*settings.setValue("pos", pos());
		settings.setValue("size", size());
		settings.setValue("state", saveState());*/
		settings.setValue("backgroundColor", myBgCol);
		settings.setValue("recentFiles", recentFiles);
		settings.setValue("workingDirectory", myWorkingDirectory);
	settings.endGroup();
}


void QMcCad_Application::ReadSettings()
{
	QString strSettingName("McCad_");
	strSettingName = strSettingName + QString::number(MCCAD_VERSION_MAJOR) + QString(".") + QString::number(MCCAD_VERSION_MINOR);

        QSettings settings("Karlsruher_Institut_fuer_Technologie", strSettingName);
	settings.beginGroup("MainWindow");
		/*move(settings.value("pos", QPoint(200, 200)).toPoint());
		resize(settings.value("size", QSize(400, 400)).toSize());
		restoreState(settings.value("state", QByteArray()).toByteArray());*/
		myWorkingDirectory = settings.value("workingDirectory").toString();
		QString recentFiles = settings.value("recentFiles").toString();
		myBgCol = settings.value("backgroundColor").value<QColor>();
	settings.endGroup();

	// manage background color
	if(!myBgCol.isValid())
		myBgCol = QColor(50,50,50);

	// update recent files ...
	TCollection_AsciiString files = ToAStr(recentFiles);
	while(true)
	{
		if(files.Search(";;") < 1)
		{
			myRecentFiles.append(files.ToCString());
			break;
		}

		TCollection_AsciiString tmpStr = files;
		files = tmpStr.Split(tmpStr.Search(";;")+1);
		tmpStr.Remove(tmpStr.Length()-1,2);
		//cout << tmpStr.ToCString() << endl;
		myRecentFiles.append(tmpStr.ToCString());
	}

	UpdateRecentFileItems();
}


/*####################################################################################
 *
 *  SLOTS SECTION
 *
 * ###################################################################################
 */

void QMcCad_Application::SlotPrint()
{
	/*myPrinter->setFullPage(TRUE);
	QMcCad_Editor* theEditor = GetEditor();
	if (theEditor != NULL)
		theEditor->Print(myPrinter);
	else
	{
		Mcout("Printing aborted ... ");
	}*/
	Mcout("McCad: Printing not yet implemented...");
	/*Mcout("Taking Pictures !!!");
	GetEditor()->MakeRotationMoviePictures();*/
}


void QMcCad_Application::SetOpenFile(const TCollection_AsciiString& inName)
{
	if(inName.IsEqual("."))
	{
		myWorkingDirectory = ".";
		return;
	}

	QMcCad_Editor* theEditor = GetEditor();
	theEditor->Import(inName.ToCString());
	theEditor->setWindowModified(true);
	theEditor->UpdateView();
	GetTreeWidget()->LoadDocument(theEditor->ID());
}


void QMcCad_Application::SlotImport()
{
	QString theFilter = "All Formats (*.stp *.step *.igs *.iges *.rle *.brep *.mcn *.d *.tri);;CAD Files (*.stp *.step *.igs *.iges *.rle *.brep);;MCNP (*.mcn);;Tripoli (*.d *.tri)";

	cout << "Import : " << myWorkingDirectory.toStdString() << endl;

	QFileDialog theDialog(this,tr("Import Geometry"), myWorkingDirectory, theFilter);
	QStringList theFiles;
	theDialog.setFileMode(QFileDialog::ExistingFiles);
	theDialog.setLabelText(QFileDialog::Accept,"Import");

	if(theDialog.exec())
		theFiles = theDialog.selectedFiles();
	QString theFileName;// = theDialog.getOpenFileName(this, tr("Open CAD/MCNP File"), myWorkingDirectory, theFilter);

	theDialog.hide();

	//Cancel Open
	if(theFiles.count() < 1)
		return;

	QApplication::setOverrideCursor(Qt::WaitCursor);
	//Get current file filter
	TCollection_AsciiString FF = ToAStr(theFileName);
	FF.Remove(1,FF.SearchFromEnd(".")-1);
	FF.Prepend("*");
	QString theSelectedFileFilter(FF.ToCString());

	QMcCad_Editor* theEditor = GetEditor();    

	for(int f=0; f<theFiles.size(); f++)//multiple files
	{
		theFileName = theFiles.at(f);
		if (theEditor != NULL ) // import function
		{
			Handle(McCadCom_CasDocument) theDoc = theEditor->GetDocument();
            McCadXCAF_TDocShapeLabelIterator slIter(theDoc->GetTDoc());
            Standard_Integer iNumBeforeImp = slIter.NumberOfShapes();

            theEditor->Import(theFileName);

            // use the file name as the material group's name.
            TCollection_AsciiString strGroupName = ToAStr(theFileName);
            strGroupName.Remove(1,strGroupName.SearchFromEnd("/"));
            strGroupName.Remove(strGroupName.SearchFromEnd("."),strGroupName.Length()-strGroupName.SearchFromEnd(".")+1);

            Standard_Integer iNumAfterImp;
            myTreeView->UpdateTreeWidget(theEditor->ID(),iNumBeforeImp,iNumAfterImp,strGroupName);

            QString qstrMatGroup(strGroupName.ToCString());
            theEditor->GetMatManager()->GenSolidGroupMap(iNumBeforeImp+1,iNumAfterImp,qstrMatGroup);// Set solid id and material group map

			theDoc->SetIsModified(Standard_True);
            theEditor->setWindowModified(true);
		}
		else
		{
			SlotFileNew();
			theEditor = GetEditor();
			Handle(McCadCom_CasDocument) theDoc = theEditor->GetDocument();

			if (theEditor != NULL)
				theEditor->Import(theFileName);
            else
			{
				statusBar()->showMessage("Loading aborted", 200);
				QApplication::restoreOverrideCursor();
				return;
			}
		}
	}

	theEditor->SetCurrentFilter(theSelectedFileFilter);





    // update tree view
	/////////////////////////
	theEditor->UpdateView();
    //myTreeView->LoadDocument(theEditor->ID());

	//end import section
	/////////////////////////
	QApplication::restoreOverrideCursor();
	myWorkingDirectory = QFileInfo(theFileName).absolutePath(); //save path for future

	QString theMessage("File(s) :   ");
	for(int i=0; i<theFiles.size(); i++)
	{
		theMessage.append(StrippedName(theFiles.at(i)));
		if(i<theFiles.size()-1)
			theMessage.append(", ");
	}
	theMessage.append("   imported.");
        Mcout(theMessage.toUtf8().constData());
}


void QMcCad_Application::SlotExport()
{
	QMcCad_Editor* ed = GetEditor();
	if(ed==NULL)
		return;

	QString theFilter = "MCNP (*.mcn);;Tripoli (*.d *.tri);;STEP (*.stp *.step);;IGES (*.igs *.iges);;BREP (*.rle *.brep)";
	QFileDialog theDialog(this,tr("Export Geometry"), myWorkingDirectory, theFilter);
	theDialog.setLabelText(QFileDialog::Accept,"Export");

	QStringList theFiles;

	if(theDialog.exec())
		theFiles = theDialog.selectedFiles();
	else
		return;

	QString theFileName = theFiles.at(0);
	QString selectedFilter = theDialog.selectedFilter();

	//check file name ending
	TCollection_AsciiString filter = ToAStr(theFileName);
	if(filter.SearchFromEnd(".")>0)
		filter.Remove(1,filter.SearchFromEnd(".")-1);
	else
	{
		if(selectedFilter == "MCNP (*.mcn)")
			theFileName += ".mcn";
		if(selectedFilter == "STEP (*.stp *.step)")
			theFileName += ".stp";
		if(selectedFilter == "IGES (*.igs *.iges)")
			theFileName += ".igs";
		if(selectedFilter == "BREP (*.rle *.brep)")
			theFileName += ".brep";
		if(selectedFilter == "Tripoli (*.d *.tri)")
			theFileName += ".d";
	}

	if(!ed->Export(ToAStr(theFileName)))
		return;

	QString theMessage("Exported to : ");
	theMessage.append(theFileName);
        Mcout(theMessage.toUtf8().constData());
}


void QMcCad_Application::SlotFileNew() // create new editor ...
{
	bool isFirst = false;

	QMcCad_Editor* curEd = GetEditor();

	if(curEd==NULL)
		isFirst=true;
	else if(curEd->isMaximized())
		curEd->showNormal();

	QMcCad_Editor* ed = new QMcCad_Editor(myWS,0, ++myEditorIDCnt);
	ed->setWindowIcon(QIcon(":images/document.png"));

	//Set context menu options
	ed->setContextMenuPolicy(Qt::ActionsContextMenu);
	ed->addAction(myActionBackgroundColor);
	ed->addAction(myActionShapeColor);
	ed->addAction(myActionShapeTransparency);
//	ed->addAction(myActionHideSelected);
//	ed->addAction(myActionShowAll);


	//ed->addAction(myActionShapeMaterial);

	TCollection_ExtendedString anErrorMessage;

	ed->UpdateWindowTitle();

	myWS->addWindow(ed);
	myCurrentFileName.clear();

	connect(ed, SIGNAL(destroyed(int)),  this, SLOT(SlotEditorClosed(int)));
	connect(ed, SIGNAL(saveAs()), this, SLOT(SlotFileSaveAs()));

	ed->show();
	ed->resize(600, 400);

	if(isFirst) // first editor? -> show maximized
		ed->showMaximized();

	//Register new editor
	myIDEditorMap[myEditorIDCnt]=ed;

	myTreeView->AddEditor(myEditorIDCnt);

	myAcM->SetCurrentAction(McCadTool_Axis); //show axis
	//ed->GradientBackground();
}


void QMcCad_Application::SlotFileSave()
{
	QMcCad_Editor* ed = GetEditor();
	if(ed == NULL)
		return;

	if(!ed->GetDocument()->IsNew())
		ed->Save();
	else
		SlotFileSaveAs();
}


void QMcCad_Application::SlotFileSaveAs()
{
	QString fileFilters("*.xml");
	Handle(TColStd_HSequenceOfAsciiString) Filters = Formats();

	QString theFileName = QFileDialog::getSaveFileName(this, tr("Save As"),	".", fileFilters );

	if ( !theFileName.isEmpty() )
	{
		if(!theFileName.endsWith(".xml"))
			theFileName.append(".xml");

		QMcCad_Editor* ed = GetEditor();

		SetCurrentFile(theFileName);

		if (ed != NULL)
		{
			if(ed->Save(ToAStr(theFileName)))
			{
				myTreeView->UpdateEditorName(ed->ID());
				QString theMessage("\nDocument saved as : ");
				theMessage.append(StrippedName(theFileName));
                                Mcout(theMessage.toUtf8().constData());
			}
		}
		else
                        Mcout("No document available - nothing to save.", McCadMessenger_ErrorMsg);
	}
	else
		Mcout("Saving aborted");
}


void QMcCad_Application::SlotFileOpen(QString& fileName)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	TCollection_AsciiString anAStr = ToAStr(fileName);
	TCollection_ExtendedString loadFile(anAStr);

	if(GetEditor()==NULL)
		SlotFileNew();

	QMcCad_Editor* ed = GetEditor();

	Handle_TDocStd_Document theTDoc = ed->GetDocument()->GetTDoc();

	myXCAFApp->Open(loadFile,theTDoc);
	ed->SetIsLoaded(Standard_True); // workaround
	ed->GetDocument()->SetTDoc(theTDoc);

	//link V3d_Viewer to TPrsStd_Viewer in Document
	/*TDF_Label rootLabel = theTDoc->GetData()->Root();
	Handle(V3d_Viewer) theViewer = ed->GetViewer()->GetViewer();
	TPrsStd_AISViewer::New(rootLabel, theViewer);

	//delete all PrsStd_AISPresentations attached to the Labels
	Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
	TDF_LabelSequence freeShapes;
	sTool->GetFreeShapes(freeShapes);

	Handle(TPrsStd_AISPresentation) prs;
	for(Standard_Integer i=1; i<=freeShapes.Length(); i++)
	{
		TDF_Label curL = freeShapes.Value(i);
		if(sTool->IsAssembly(curL))
		{
			TDF_LabelSequence subShapes;
			sTool->GetSubShapes(curL, subShapes);
			for(TDF_ChildIterator it( curL, Standard_False ); it.More(); it.Next())
				if ( it.Value().FindAttribute( TPrsStd_AISPresentation::GetID(), prs ) )
					it.Value().ForgetAttribute(TPrsStd_AISPresentation::GetID());
		}
		else
		{
			if ( curL.FindAttribute ( TPrsStd_AISPresentation::GetID(), prs ) )
				curL.ForgetAttribute(TPrsStd_AISPresentation::GetID());
		}
	}*/

	//Visualize
	ed->UpdateView();

	//Update TreeView
	myTreeView->LoadDocument(myEditorIDCnt);

	//update names
	ed->GetDocument()->SetDocName(anAStr);
	ed->UpdateWindowTitle();
	myTreeView->UpdateEditorName(ed->ID());

		QApplication::restoreOverrideCursor();
}


void QMcCad_Application::SlotFileOpen()
{
	QString ff("*.xml");
	QStringList theFiles;

	QFileDialog theFileDialog(this, Qt::Dialog);
	theFileDialog.setWindowTitle("Open McCad Document");
	theFileDialog.setFilter(ff);
	theFileDialog.setDirectory(myWorkingDirectory);
	if(theFileDialog.exec())
		theFiles = theFileDialog.selectedFiles();
	else
		return;

	QString theFileName = theFiles.at(0);

	if(theFileName == "") // open dialog has been cancelled
		return;

	SlotFileOpen(theFileName);
}


void QMcCad_Application::SlotFileClose()
{
	QMcCad_Editor* theEditor = GetEditor();
	if (theEditor !=NULL)
	{
		theEditor->close();
	}
}


void QMcCad_Application::SlotUpdateMainTitle()
{
/*	QMcCad_Editor* theEditor = GetEditor();
	if(theEditor != NULL)
		SetCurrentFile(theEditor->GetQFileName());*/
}


void QMcCad_Application::SlotEditorClosed(int theID)
{
	//cout << "editor " << theID << " closed\n";
//	myCADPropWidget->EditorClosed(theID);
	myTreeView->RemoveEditor(theID);
	QMcCad_Editor* theEditor;
	theEditor = GetEditor();
	if(theEditor == NULL) // last editor closed?...
		setWindowTitle("McCad");
	else //set title active editor...
		SetCurrentFile(theEditor->GetQFileName());

	statusBar()->showMessage("Editor closed", 2000);
}


void QMcCad_Application::closeEvent(QCloseEvent *e)
{
	QWidgetList windows = myWS->windowList();

	WriteSettings();

	if (windows.count() )
	{
		for (int i = 0; i < int(windows.count()); ++i)
		{
			QWidget *window = windows.at(i);
			if ( !window->close() )
			{
				e->ignore();
				return;
			}
		}
	}
}


void QMcCad_Application::SlotAbout()
{
	QString strMessage("<h2>McCad Version ");
	strMessage = strMessage + QString::number(MCCAD_VERSION_MAJOR) + QString(".") + QString::number(MCCAD_VERSION_MINOR) + QString(".") + QString::number(MCCAD_VERSION_PATCH);
    strMessage = strMessage + QString("</h2>   <p>Copyright &copy; 2014 Karlsruhe Institut of Technology (KIT) <p> <b>McCad is a CAD interface for  Monte Carlo transport codes.</b>");
	QMessageBox::about( this, tr("About McCad"), strMessage);
}


void QMcCad_Application::SlotQuit()
{
	WriteSettings();
	this->close();
}



QMcCadGeomeTree_TreeWidget* QMcCad_Application::GetTreeWidget()
{
	return myTreeView;
}


void QMcCad_Application::contextMenuEvent(QContextMenuEvent *event)
{
	/*
	 QPopupMenu contextMenu(this);
	 cutAct->addTo(&contextMenu);
	 copyAct->addTo(&contextMenu);
	 pasteAct->addTo(&contextMenu);
	 */
	//myContextMenu->exec(event->globalPos());
}



void QMcCad_Application::SlotHelp()
{
	OSD_Directory theResourceDir = ResourceDir();
	OSD_Path theResourcePath;
	theResourceDir.Path(theResourcePath);
	TCollection_AsciiString aPathName;
	theResourcePath.SystemName(aPathName);

	QString theDocumentDir(aPathName.ToCString());
	theDocumentDir = theDocumentDir+"/doc";
	/*
	 if (myAssistant==NULL)
	 {
	 myAssistant = new QAssistantClient(QDir(theDocumentDir).absPath(), this);
	 QStringList argList;
	 argList << "-profile";
	 argList << QDir::convertSeparators(theDocumentDir + "/McCaddoc.adp");
	 myAssistant->setArguments(argList);
	 }
	 myAssistant->openAssistant();
	 */
	/*myAssistant->showPage(QString("index.html"));
	if (myAssistant->isOpen())
	{
		cout << "Can not open the assistant!! " << qPrintable(theDocumentDir) << endl;
	}
	else
	{
		cout << "Assistant is open!! " << qPrintable(theDocumentDir) << endl;
	}*/
}

void QMcCad_Application::SlotUpdateMenus()
{
	bool haveEditor = (GetEditor() != 0);

	for (int i = 0; i < myEditActionList.size(); ++i)
		myEditActionList[i]->setEnabled(false);
	for (int i = 0; i < 2; ++i)
		myEditActionList[i]->setEnabled(haveEditor);

	for (int i = 0; i < myViewActionList.size(); ++i)
		myViewActionList[i]->setEnabled(haveEditor);

	for (int i = 0; i < myAssistActionList.size(); ++i)
		myAssistActionList[i]->setEnabled(false);

	for (int i = 4; i < myDesignActionList.size(); ++i)
		myDesignActionList[i]->setEnabled(false);
	for (int i = 0; i < 4; ++i)
		myDesignActionList[i]->setEnabled(haveEditor);

	myModifyActionList[0]->setEnabled(haveEditor);

	for (int i = 0; i < myModifyActionList.size(); ++i)
		myModifyActionList[i]->setEnabled(haveEditor);

	for (int i = 0; i < myWindowActionList.size(); ++i)
		//	myWindowActionList[i]->setEnabled(false);
	myWindowActionList[1]->setEnabled(haveEditor);

	myFileActionList[0]->setEnabled(true); //always active
	myFileActionList[1]->setEnabled(true); // "
	for(int i = 2; i< myFileActionList.size(); i++)
		myFileActionList[i]->setEnabled(haveEditor);
}


void QMcCad_Application::SlotHardcopy()
{
	QString theFile = QFileDialog::getSaveFileName(this, "Choose a filename for saving hardcopy", ".",	"Image Files (*.bmp *.gif *.xwd)");

	QFileInfo fi(theFile);
	QString ext = fi.suffix();

	if (!theFile.isNull() )
	{
		if ( !ext.length() )
			theFile += QString(".bmp");
		QMcCad_Editor* theEditor = GetEditor();
		if (theEditor !=NULL)
		{
			theEditor->Hardcopy(theFile);
		}
		else
                        Mcout("No active Editor available for hardcopy!!", McCadMessenger_ErrorMsg);
	}
}



/* ######################################################################
 *
 * CREATE SECTION
 *
 * ######################################################################
 */



void QMcCad_Application::CreateTreeView()
{
	QDockWidget* theDockWid = new QDockWidget(this);
	myTreeView = new QMcCadGeomeTree_TreeWidget(theDockWid);
	myTreeView->setContextMenuPolicy(Qt::ActionsContextMenu);
	//myTreeView->addAction(myActionBackgroundColor);
	//myTreeView->addAction(myActionShapeColor);
	//myTreeView->addAction(myActionShapeTransparency);
    /*myTreeView->addAction(myActionHideSelected);
	myTreeView->addAction(myActionRedisplaySelected);
    myTreeView->addAction(myActionDisplaySelectedOnly);
    myTreeView->addAction(myActionShowAll);*/
    //myTreeView->addAction(myActionAddMaterialGroup);    // Lei
    //myTreeView->addAction(myActionAddToMaterialGroup);  // Lei
	//myTreeView->addAction(myActionModelErase);
	theDockWid->setFeatures(QDockWidget::DockWidgetMovable);
	theDockWid->setWidget(myTreeView);
	theDockWid->setMinimumSize(150,350);
	theDockWid->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
	addDockWidget(Qt::LeftDockWidgetArea, theDockWid);
}


void QMcCad_Application::CreateLogWindow()
{
	QDockWidget *dock = new QDockWidget(this);
	dock->setAllowedAreas(Qt::BottomDockWidgetArea);
	dock->setGeometry(x(), y(), 1/200*width(), 1/200*height());
	myOut = new QMcCad_LogWindow(dock);
	dock->setWidget(myOut);
	addDockWidget(Qt::BottomDockWidgetArea, dock);
}


void QMcCad_Application::CreateStatusBar()
{
	statusBar()->showMessage("Ready");
}

void QMcCad_Application::CreateMenus()
{
//FILE
	myFileMenu = menuBar()->addMenu(tr("&File"));
	myFileMenu->addAction(myActionFileNew);
	myFileMenu->addAction(myActionFileOpen);
	myFileMenu->addAction(myActionFileClose);
	myFileMenu->addAction(myActionFileSave);
	myFileMenu->addAction(myActionFileSaveAs);

	myFileMenu->addSeparator();
	myFileMenu->addAction(myActionImportGeom);
	myFileMenu->addAction(myActionExportGeom);

	myFileMenu->addSeparator();
	myFileMenu->addAction(myActionHardCopy);
	myFileMenu->addAction(myActionPrint);

	myFileMenu->addSeparator();
	for (int i = 0; i < MaxRecentFiles; ++i)
		myFileMenu->addAction(myRecentFileActions[i]);
	myFileMenu->addSeparator();

	myFileMenu->addAction(myActionQuit);

//EDIT
	myEditMenu = menuBar()->addMenu(tr("&Edit"));
	myEditMenu->addAction(myActionEditUndo);
	myEditMenu->addAction(myActionEditRedo);
	myEditMenu->addAction(myActionEditCut);
	myEditMenu->addAction(myActionEditCopy);
	myEditMenu->addAction(myActionEditPaste);
	myEditMenu->addAction(myActionSelect);
	myEditMenu->addAction(myActionSelectAll);
	myEditMenu->addAction(myActionDeselectAll);

//VIEW
	myViewMenu = menuBar()->addMenu(tr("&View"));
	myViewMenu->addAction(myActionRedraw);
	myZoomSubMenu = myViewMenu->addMenu(tr("&Zoom"));
	myZoomSubMenu->addAction(myActionZoomWindow);
	//myZoomSubMenu->addAction(myActionZoomDynamic);
	//myZoomSubMenu->addAction(myActionZoomScale);
	myZoomSubMenu->addAction(myActionZoomIn);
	myZoomSubMenu->addAction(myActionZoomOut);
	myZoomSubMenu->addAction(myActionFitAll);
	myViewMenu->addAction(myActionPan);
	myViewMenu->addAction(myActionRotate);
	myProjectionSubMenu = myViewMenu->addMenu(tr("&Projection"));
	myProjectionSubMenu->addAction(myActionBackProject);
	myProjectionSubMenu->addAction(myActionFrontProject);
	myProjectionSubMenu->addAction(myActionTopProject);
	myProjectionSubMenu->addAction(myActionBottomProject);
	myProjectionSubMenu->addAction(myActionLeftProject);
	myProjectionSubMenu->addAction(myActionRightProject);
	myViewMenu->addSeparator();
	myShadeSubMenu = myViewMenu->addMenu(tr("&Mode"));
	myShadeSubMenu->addAction(myActionWireFrame);
	myShadeSubMenu->addAction(myActionFlatShade);
	myShadeSubMenu->addAction(myActionSetHiddenLine);
	myViewMenu->addAction(myActionAntialiasing);
	myViewMenu->addAction(myActionBackgroundColor);
	myViewMenu->addAction(myActionShapeColor);
	myViewMenu->addAction(myActionShapeTransparency);
	// myInsertMenu = new QPopupMenu(this);

//ASSIST
	myAssistMenu = menuBar()->addMenu(tr("&Assist"));
	myAssistMenu->addAction(myActionMeasureDistance);
	myAssistMenu->addAction(myActionMeasureAngle);
	myAssistMenu->addAction(myActionMeasureArea);
	myAssistMenu->addAction(myActionMeasureVolume);
	myAssistMenu->addAction(myActionPropertyCurve);
	myAssistMenu->addAction(myActionPropertySurface);

//DESIGN
	myDesignMenu = menuBar()->addMenu(tr("&Design"));
	myDesignMenu->addAction(myActionMakeBox);
	myDesignMenu->addAction(myActionMakeCone);
	myDesignMenu->addAction(myActionMakeCylinder);
	myDesignMenu->addAction(myActionMakeHalfSpace);
	myDesignMenu->addAction(myActionMakeSphere);
	myDesignMenu->addAction(myActionMakeTorus);
	myDesignMenu->addAction(myActionMakeWedge);

//TOOLS
	myModifyMenu = menuBar()->addMenu(tr("&Tools"));
	myModifyMenu->addAction(myActionModelErase);
	myModifyMenu->addAction(myActionModelCopy);
	myModifyMenu->addAction(myActionModelMirror);
	myModifyMenu->addAction(myActionModelMove);
	myModifyMenu->addAction(myActionModelRotate);
	myModifyMenu->addAction(myActionModelScale);
	//myModifyMenu->addAction(myActionModelStrech);
	myBooleanSubMenu = myModifyMenu->addMenu(tr("&Boolean"));
	myBooleanSubMenu->addAction(myActionSolidUnion);
	myBooleanSubMenu->addAction(myActionSolidIntersection);
	myBooleanSubMenu->addAction(myActionSolidDifference);
        myModifyMenu->addSeparator();
        myModifyMenu->addAction(myActionReadValueList);

//WINDOW
	myWindowMenu = menuBar()->addMenu(tr("&Window"));
	myWindowMenu->addAction(myActionSetGrid);
	myWindowMenu->addAction(myActionSetAxis);

	menuBar()->addSeparator();

//HELP
	myHelpMenu = menuBar()->addMenu(tr("&Help"));
	myHelpMenu->addAction(myActionAbout);
	myHelpMenu->addAction(myActionHelp);
}


void QMcCad_Application::CreateToolBars()
{
	myFileToolBar = addToolBar(tr("File"));
	myFileToolBar->addAction(myActionFileNew);
	myFileToolBar->addAction(myActionFileOpen);
	myFileToolBar->addAction(myActionFileSave);
	myFileToolBar->addAction(myActionFileSaveAs);
	myFileToolBar->addAction(myActionFileClose);


	//  myFileToolBar->addAction(myActionFileSaveAs);
	myFileToolBar->addAction(myActionPrint);

	myViewToolBar = addToolBar(tr("View"));
	myViewToolBar->addAction(myActionRedraw);
	myViewToolBar->addAction(myActionFitAll);
	myViewToolBar->addAction(myActionZoomWindow);
	//myViewToolBar->addAction(myActionZoomDynamic);
	//myViewToolBar->addAction(myActionZoomScale);
	myViewToolBar->addAction(myActionZoomIn);
	myViewToolBar->addAction(myActionZoomOut);
	myViewToolBar->addAction(myActionPan);
	myViewToolBar->addAction(myActionRotate);
	myViewToolBar->addAction(myActionBackProject);
	myViewToolBar->addAction(myActionFrontProject);
	myViewToolBar->addAction(myActionTopProject);
	myViewToolBar->addAction(myActionBottomProject);
	myViewToolBar->addAction(myActionLeftProject);
	myViewToolBar->addAction(myActionRightProject);
	myViewToolBar->addAction(myActionWireFrame);
	myViewToolBar->addAction(myActionFlatShade);
	myViewToolBar->addSeparator();
	myViewToolBar->addAction(myActionHideSelected);
	myViewToolBar->addAction(myActionRedisplaySelected);
	myViewToolBar->addAction(myActionDisplaySelectedOnly);
	myViewToolBar->addAction(myActionShowAll);
}


void QMcCad_Application::CreateActions()
{
	QMcCad_ActionFactory myActionFactory(this);

	// file menu
	myActionFileNew = myActionFactory.CreateAction(McCadTool_FileNew, this);
	myActionFileOpen = myActionFactory.CreateAction(McCadTool_FileOpen, this);
	myActionFileClose = myActionFactory.CreateAction(McCadTool_FileClose, this);
	myActionFileSave = myActionFactory.CreateAction(McCadTool_FileSave, this);
	myActionFileSaveAs = myActionFactory.CreateAction(McCadTool_FileSaveAs,	this);
	myActionImportGeom = myActionFactory.CreateAction(McCadTool_ImportGeom, this);
	myActionExportGeom = myActionFactory.CreateAction(McCadTool_ExportGeom, this);
	myActionHardCopy = myActionFactory.CreateAction(McCadTool_Hardcopy, this);
	myActionPrint = myActionFactory.CreateAction(McCadTool_Print, this);
	myActionQuit = new QAction(tr("E&xit"), this);
	//myActionQuit->setShortcut(tr("Ctrl+Q"));
	myActionQuit->setStatusTip(tr("Exit the application"));

	connect(myActionQuit, SIGNAL(triggered()), this, SLOT(SlotQuit()));

	for (int i = 0; i < MaxRecentFiles; ++i)
	{
		myRecentFileActions[i] = new QAction(this);
		myRecentFileActions[i]->setVisible(false);
		connect(myRecentFileActions[i], SIGNAL(triggered()), this,	SLOT(OpenRecentFile()));
	}

	myFileActionList.append(myActionFileNew);
	myFileActionList.append(myActionFileOpen);
	myFileActionList.append(myActionFileClose);
	myFileActionList.append(myActionFileSave);
	myFileActionList.append(myActionFileSaveAs);
	myFileActionList.append(myActionImportGeom);
	myFileActionList.append(myActionExportGeom);
	myFileActionList.append(myActionHardCopy);
	myFileActionList.append(myActionPrint);

	//edit Menu
	myActionEditUndo = myActionFactory.CreateAction(McCadTool_EditUndo, myAcM);
	myActionEditRedo = myActionFactory.CreateAction(McCadTool_EditRedo, myAcM);
	myActionEditCut = myActionFactory.CreateAction(McCadTool_EditCut, myAcM);
	myActionEditCopy = myActionFactory.CreateAction(McCadTool_EditCopy, myAcM);
	myActionEditPaste = myActionFactory.CreateAction(McCadTool_EditPaste, myAcM);
	myActionSelect = myActionFactory.CreateAction(McCadTool_Select, myAcM);
	myActionSelectAll = myActionFactory.CreateAction(McCadTool_SelectAll, myAcM);
	myActionDeselectAll = myActionFactory.CreateAction(McCadTool_DeselectAll, myAcM);

	myEditActionList.append(myActionEditUndo);
	myEditActionList.append(myActionEditRedo);
	myEditActionList.append(myActionEditCut);
	myEditActionList.append(myActionEditCopy);
	myEditActionList.append(myActionEditPaste);
	myEditActionList.append(myActionSelect);
	myEditActionList.append(myActionSelectAll);
	myEditActionList.append(myActionDeselectAll);

	//view Menu

	myActionRedraw = myActionFactory.CreateAction(McCadTool_Redraw, myAcM);
	myActionZoomWindow = myActionFactory.CreateAction(McCadTool_ZoomWindow, myAcM);
	myActionDisplaySelectedOnly = myActionFactory.CreateAction(McCadTool_DispSelOnly, myAcM);
	//myActionZoomDynamic = myActionFactory.CreateAction(McCadTool_ZoomDynamic, myAcM);
	//myActionZoomScale = myActionFactory.CreateAction(McCadTool_ZoomScale, myAcM);
	myActionZoomIn = myActionFactory.CreateAction(McCadTool_ZoomIn, myAcM);
	myActionZoomOut = myActionFactory.CreateAction(McCadTool_ZoomOut, myAcM);
	myActionFitAll = myActionFactory.CreateAction(McCadTool_FitAll, myAcM);
	myActionPan = myActionFactory.CreateAction(McCadTool_Pan, myAcM);
	myActionRotate = myActionFactory.CreateAction(McCadTool_Rotate, myAcM);
	myActionBackProject = myActionFactory.CreateAction(McCadTool_BackProject, myAcM);
	myActionFrontProject = myActionFactory.CreateAction(McCadTool_FrontProject,	myAcM);
	myActionTopProject = myActionFactory.CreateAction(McCadTool_TopProject,	myAcM);
	myActionBottomProject = myActionFactory.CreateAction(McCadTool_BottomProject, myAcM);
	myActionLeftProject = myActionFactory.CreateAction(McCadTool_LeftProject, myAcM);
	myActionRightProject = myActionFactory.CreateAction(McCadTool_RightProject,	myAcM);
	myActionWireFrame = myActionFactory.CreateAction(McCadTool_WireFrame, myAcM);
	myActionFlatShade = myActionFactory.CreateAction(McCadTool_FlatShade, myAcM);
	myActionSetHiddenLine = myActionFactory.CreateAction(	McCadTool_SetHiddenLine, myAcM);
	myActionBackgroundColor = myActionFactory.CreateAction(	McCadTool_BackgroundColor, myAcM);
	myActionShapeColor = myActionFactory.CreateAction(McCadTool_ShapeColor,	myAcM);
	myActionShapeMaterial = myActionFactory.CreateAction(	McCadTool_ShapeMaterial, myAcM);
	myActionAntialiasing = myActionFactory.CreateAction(McCadTool_Antialiasing,	myAcM);
	myActionShapeTransparency = myActionFactory.CreateAction(McCadTool_ShapeTransparency, myAcM);
	myActionShowAll = myActionFactory.CreateAction(McCadTool_ShowAll, myAcM);
	myActionHideSelected = myActionFactory.CreateAction(McCadTool_HideSelected, myAcM);
	myActionRedisplaySelected = myActionFactory.CreateAction(McCadTool_RedispSelected, myAcM);

	myViewActionList.append(myActionRedraw);
	myViewActionList.append(myActionZoomWindow);
	//myViewActionList.append(myActionZoomDynamic);
	//myViewActionList.append(myActionZoomScale);
	myViewActionList.append(myActionZoomIn);
	myViewActionList.append(myActionZoomOut);
	myViewActionList.append(myActionFitAll);
	myViewActionList.append(myActionPan);
	myViewActionList.append(myActionRotate);
	myViewActionList.append(myActionBackProject);
	myViewActionList.append(myActionFrontProject);
	myViewActionList.append(myActionTopProject);
	myViewActionList.append(myActionBottomProject);
	myViewActionList.append(myActionLeftProject);
	myViewActionList.append(myActionRightProject);
	myViewActionList.append(myActionWireFrame);
	myViewActionList.append(myActionFlatShade);
	myViewActionList.append(myActionSetHiddenLine);
	myViewActionList.append(myActionAntialiasing);
	myViewActionList.append(myActionBackgroundColor);
	myViewActionList.append(myActionShapeColor);
	myViewActionList.append(myActionShapeMaterial);
	myViewActionList.append(myActionShapeTransparency);
	myViewActionList.append(myActionShowAll);
	myViewActionList.append(myActionHideSelected);
	myViewActionList.append(myActionRedisplaySelected);
	myViewActionList.append(myActionDisplaySelectedOnly);

	// assis menu
	myActionMeasureDistance = myActionFactory.CreateAction(	McCadTool_MeasureDistance, myAcM);
	myActionMeasureAngle = myActionFactory.CreateAction(McCadTool_MeasureAngle,	myAcM);
	myActionMeasureArea = myActionFactory.CreateAction(McCadTool_MeasureArea,myAcM);
	myActionMeasureVolume = myActionFactory.CreateAction(McCadTool_MeasureVolume, myAcM);
	myActionPropertyCurve = myActionFactory.CreateAction(McCadTool_PropertyCurve, myAcM);
	myActionPropertySurface = myActionFactory.CreateAction(McCadTool_PropertySurface, myAcM);

	myAssistActionList.append(myActionMeasureDistance);
	myAssistActionList.append(myActionMeasureAngle);
	myAssistActionList.append(myActionMeasureArea);
	myAssistActionList.append(myActionMeasureVolume);
	myAssistActionList.append(myActionPropertyCurve);
	myAssistActionList.append(myActionPropertySurface);

	// DesignMenu;
	myActionMakeBox = myActionFactory.CreateAction(McCadTool_MakeBox, myAcM);
	myActionMakeCone = myActionFactory.CreateAction(McCadTool_MakeCone, myAcM);
	myActionMakeCylinder = myActionFactory.CreateAction(McCadTool_MakeCylinder,	myAcM);
	myActionMakeSphere = myActionFactory.CreateAction(McCadTool_MakeSphere,	myAcM);
	myActionMakeHalfSpace = myActionFactory.CreateAction(McCadTool_MakeHalfSpace, myAcM);
	myActionMakeTorus = myActionFactory.CreateAction(McCadTool_MakeTorus, myAcM);
	myActionMakeWedge = myActionFactory.CreateAction(McCadTool_MakeWedge, myAcM);

	myDesignActionList.append(myActionMakeBox);
	myDesignActionList.append(myActionMakeCone);
	myDesignActionList.append(myActionMakeCylinder);
	myDesignActionList.append(myActionMakeSphere);
	myDesignActionList.append(myActionMakeHalfSpace);
	myDesignActionList.append(myActionMakeTorus);
	myDesignActionList.append(myActionMakeWedge);


	//ModifyMenu;
	myActionModelErase = myActionFactory.CreateAction(McCadTool_ModelErase,	myAcM);
	myActionModelCopy = myActionFactory.CreateAction(McCadTool_ModelCopy, myAcM);
	myActionModelMirror = myActionFactory.CreateAction(McCadTool_ModelMirror, myAcM);
	myActionModelMove = myActionFactory.CreateAction(McCadTool_ModelMove, myAcM);
	myActionModelRotate = myActionFactory.CreateAction(McCadTool_ModelRotate, myAcM);
	myActionModelScale = myActionFactory.CreateAction(McCadTool_ModelScale, myAcM);
	//myActionModelStrech = myActionFactory.CreateAction(McCadTool_ModelStrech,myAcM);
	myActionSolidUnion = myActionFactory.CreateAction(McCadTool_SolidUnion,	myAcM);
	myActionSolidIntersection = myActionFactory.CreateAction(	McCadTool_SolidIntersection, myAcM);
	myActionSolidDifference = myActionFactory.CreateAction(	McCadTool_SolidDifference, myAcM);
        myActionReadValueList = myActionFactory.CreateAction(McCadTool_ReadValueList, myAcM);

	myModifyActionList.append(myActionModelErase);
	myModifyActionList.append(myActionModelCopy);
	myModifyActionList.append(myActionModelMirror);
	myModifyActionList.append(myActionModelMove);
	myModifyActionList.append(myActionModelRotate);
	myModifyActionList.append(myActionModelScale);
	//myModifyActionList.append(myActionModelStrech);
	myModifyActionList.append(myActionSolidUnion);
	myModifyActionList.append(myActionSolidIntersection);
	myModifyActionList.append(myActionSolidDifference);
        myModifyActionList.append(myActionReadValueList);

	//WindowMenu
	myActionSetGrid = myActionFactory.CreateAction( McCadTool_ClippingPlane, myAcM);
	myActionSetAxis = myActionFactory.CreateAction( McCadTool_Axis, myAcM);

	myWindowActionList.append(myActionSetGrid);
	myWindowActionList.append(myActionSetAxis);

	//HelpMenu
	myActionAbout = new QAction(tr("&About"),this);
	myActionAbout->setStatusTip(tr("Show the application's About box"));
	connect(myActionAbout, SIGNAL(triggered()), this, SLOT(SlotAbout()));

	myActionHelp = new QAction(tr("Manua&l"), this);
	myActionHelp->setStatusTip(tr("Online Manual"));
	connect(myActionHelp, SIGNAL(triggered()), this, SLOT(SlotHelp()));
    // Lei 15/10
    //myActionAddMaterialGroup = myActionFactory.CreateAction(McCadTool_AddMaterialGroup, myAcM);
    //myActionAddToMaterialGroup = myActionFactory.CreateAction(McCadTool_AddToMaterialGroup, myAcM);
	//myActionClippingPlane = myActionFactory.CreateAction(McCadTool_ClippingPlane, myAcM);
}


