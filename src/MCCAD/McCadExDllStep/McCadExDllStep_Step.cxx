#ifndef  _OSD_File_HeaderFile
#include <OSD_File.hxx>
#endif
#ifndef  _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#ifndef  _STEPControl_Reader_HeaderFile
#include <STEPControl_Reader.hxx>
#endif
#ifndef  _STEPControl_Writer_HeaderFile
#include <STEPControl_Writer.hxx>
#endif
#include <map>
#include <string>

#include <Message_PrinterOStream.hxx>

#include <STEPControl_StepModelType.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>

#include <McCadGUI_BaseDialogPtr.hxx>
#include <McCadGUI_BaseDialog.hxx>
#include <McCadGUI_ProgressDialogPtr.hxx>
#include <McCadGUI_ProgressDialog.hxx>
#include <McCadGUI_InputDialogPtr.hxx>
#include <McCadGUI_InputDialog.hxx>
#include <McCadGUI_GuiFactoryPtr.hxx>
#include <McCadGUI_GuiFactory.hxx>

#include <McCadEXPlug_PluginMaker.hxx>

#include <McCadExDllStep_Step.ixx>

#include <OSD_Protection.hxx>
#include <TDF_Label.hxx>

#include <Message_ProgressIndicator.hxx>
//qiu remove gui, might be dangerous! #include <QMcCad_Application.h>
#include <McCadXCAF_Application.hxx> //qiu
#include <Quantity_Color.hxx> //qiu 
#include <XSControl_WorkSession.hxx>
#include <Transfer_TransientProcess.hxx>
#include <McCadXCAF_TDocShapeLabelIterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Name.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <TDocStd_Application.hxx>

#include <TopOpeBRep_ShapeIntersector.hxx>

#include <BRepTools.hxx>

using namespace std;

//qiu we don't need "extern"
///*qiu extern */map<string, McCadEXPlug_PluginMaker, less<string> > PluginMakerMap;

McCadExDllStep_Step::McCadExDllStep_Step()
{
	myExtensions = new TColStd_HSequenceOfAsciiString;
	myExtensions->Append(TCollection_AsciiString(".step"));
	myExtensions->Append(TCollection_AsciiString(".stp"));
	myFormat = "step";
	myExportEnabled = Standard_True;
	myImportEnabled = Standard_True;
	myFileName = " ";
	mySize = 0;
	myType = 0;
/*	cout << "========================"<< endl;
	cout << "McCadExDllStep_Step()"<< endl;
	cout << "========================"<< endl;*/

}
//Standard_Integer McCadExDllStep_Step::GetModelType() const
//{
//  return myType;
// }

// void McCadExDllStep_Step::SetModelType(const Standard_Integer theType)
// {
//  myType = theType;
// }

Standard_Boolean McCadExDllStep_Step::Export(const Handle(TopTools_HSequenceOfShape)& theEntitySequence)
{
	McCadGUI_GuiFactoryPtr theGuiFactory = theGuiFactory->Instance();
	theGuiFactory->SetType(McCadGUI_Base);
	McCadGUI_BaseDialogPtr theMessage = theGuiFactory->MakeDialog();
	McCadGUI_BaseDialogPtr theError = theGuiFactory->MakeDialog();
        //theGuiFactory->SetType(McCadGUI_Progress);
        McCadGUI_ProgressDialogPtr theProgress = new McCadGUI_ProgressDialog;

        myProgress = theProgress;

	if (theEntitySequence->Length() == 0)
	{
		theError->Print(TCollection_AsciiString("The model is empty, can not be exported."));
		theError->Print(TCollection_AsciiString("Error: exporting empty model."));
		cout << "The model is empty, can not be exported." << endl;;
		cout << "Error: exporting empty model." << endl;
		delete theError;
		delete theMessage;
		return Standard_False;
	}
	STEPControl_StepModelType myModelType = STEPControl_AsIs;

	switch(myType)
	{
		case 0:
			myModelType = STEPControl_AsIs;
			break;
		case 1:
			myModelType = STEPControl_ManifoldSolidBrep;
			break;
		case 2:
			myModelType =STEPControl_BrepWithVoids;
			break;
		case 3:
			myModelType =STEPControl_FacetedBrep;
			break;
		case 4:
			myModelType = STEPControl_FacetedBrepAndBrepWithVoids;
			break;
		case 5:
			myModelType = STEPControl_ShellBasedSurfaceModel;
			break;
		case 6:
			myModelType = STEPControl_GeometricCurveSet;
			break;
		case 7:
			myModelType = STEPControl_Hybrid;
			break;
		default:
			myModelType = STEPControl_AsIs;
			break;
	}

	if(myModelType == STEPControl_FacetedBrep)
		if (!IsFacetedBrep(theEntitySequence))
		{
			theError->Print(TCollection_AsciiString("The Model is not a faceted Brep."));
			cout << "The Model is not a faceted Brep." << endl;
			delete theMessage;
			delete theError;
			delete theProgress;
			return Standard_False;
		}

	if (!myFileName.IsEmpty())
	{
		OSD_Path thePath(myFileName);
		OSD_File theFile(thePath);
		theFile.Build(OSD_ReadWrite , OSD_Protection());
		if (theFile.Exists())
		{
			Standard_Integer theSequenceLength = theEntitySequence->Length();
			myProgress->SetLabelText("Preparing Step File.");
			myProgress->SetCancelButtonText("Cancel Export!");
			myProgress->SetTotalSteps(100);
			myProgress->SetProgress(0);

                        STEPControl_Writer aWriter;
                        IFSelect_ReturnStatus aReturnStatus = IFSelect_RetVoid;                        

			for (Standard_Integer i=1; i<=theSequenceLength; i++)
			{
				myProgress->SetProgress(i*100/theSequenceLength);
				TopoDS_Shape myShape = theEntitySequence->Value(i);
				if ( !myShape.IsNull() )
				{
                                        aReturnStatus = aWriter.Transfer(myShape, myModelType);
				}
			}
                        if(aReturnStatus == IFSelect_RetDone )
			{
				myProgress->SetLabelText("Exporting Step File.");
				myProgress->SetCancelButtonText("Cancel Export!");
				myProgress->SetTotalSteps(100);
				myProgress->SetProgress(0);

                                aReturnStatus = aWriter.Write(myFileName.ToCString());
                                if (aReturnStatus == IFSelect_RetDone)
				{
					myProgress->SetProgress(100);
					theMessage->Print(myFileName + TCollection_AsciiString(" has been Exported."));
					cout << myFileName.ToCString() << " has been Exported." << endl;
					delete theMessage;
					delete theError;
					delete theProgress;
					return Standard_True;
				}
				else {
					theError->Print(TCollection_AsciiString("Can not Export to the file:") + myFileName );
					theError->Print(TCollection_AsciiString("Error from the Step Writer."));
					cout << "Can not Export to the file:" << myFileName.ToCString() << endl;
					cout <<"Error from the Step Writer." << endl;
					delete theMessage;
					delete theError;
					delete theProgress;
					return Standard_True;
				}
			}
			else
			{
				theError->Print(TCollection_AsciiString("Can not Export to the file:") + myFileName );
				theError->Print(TCollection_AsciiString("Error from the Step Translator."));
				cout << "Can not Export to the file:" << myFileName.ToCString() << endl;
				cout <<"Error from the Step Writer." << endl;
				delete theError;
				delete theMessage;
				delete theProgress;
				return Standard_False;
			}
		}
		else
		{
			theError->Print(myFileName + TCollection_AsciiString(" does not exist."));
			cout << myFileName.ToCString() << " does not exist." << endl;
			delete theMessage;
			delete theError;
			delete theProgress;
			return Standard_False;
		}
		theFile.Close();
	}
	else
	{
		theError->Print(TCollection_AsciiString("Select file first."));
		cout << " Select file first." << endl;
		delete theMessage;
		delete theError;
		delete theProgress;
		return Standard_False;
	}
}


Handle(TopTools_HSequenceOfShape) McCadExDllStep_Step::Import()
{
	Handle(TopTools_HSequenceOfShape) theEntitySequence = new TopTools_HSequenceOfShape;
/*	McCadGUI_GuiFactoryPtr theGuiFactory = theGuiFactory->Instance();
	McCadGUI_BaseDialogPtr theMessage = theGuiFactory->MakeDialog();
	McCadGUI_BaseDialogPtr theError = theGuiFactory->MakeDialog();
	McCadGUI_BaseDialogPtr theProgress = theGuiFactory->MakeDialog();
	myProgress = theProgress;*/

	if (!myFileName.IsEmpty())
	{
		OSD_Path thePath(myFileName);
		OSD_File theFile(thePath);
		if (theFile.Exists())
		{
			/*myProgress->SetLabelText("Reading Step file.");
			myProgress->SetCancelButtonText("Cancel Reading!");
			myProgress->SetTotalSteps(100);
			myProgress->SetProgress(0);*/

			STEPControl_Reader myReader;
			IFSelect_ReturnStatus myReturnStatus = myReader.ReadFile(myFileName.ToCString());

			//myProgress->SetProgress(100);

			if ( myReturnStatus == IFSelect_RetDone)
			{
				/*Standard_CString StepLogFile = "_Step.log";
				Standard_Boolean doAppend = Standard_True;
				Message_PrinterOStream transferMessages(StepLogFile, doAppend);*/
				//Obsolete in OCC63 : Interface_TraceFile::SetDefault(2,StepLogFile,Standard_True);

				Standard_Boolean failsonly = Standard_False;
				myReader.PrintCheckLoad(failsonly, IFSelect_ItemsByEntity);
				// Root transfers
				/*myProgress->SetProgress(0);
				myProgress->SetLabelText("Translating Step file.");
				myProgress->SetCancelButtonText("Cancel Translation!");
				myProgress->SetTotalSteps(100);*/

				Standard_Integer theNumberOfRoots = myReader.NbRootsForTransfer();

				myReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);

				if (theNumberOfRoots != 0)
				{
					TopoDS_Shape theShape;
//MYMOD 050208
					for ( Standard_Integer i = 1; i <= 	theNumberOfRoots; i++ )
					{
						//myProgress->SetProgress(i*100/theNumberOfRoots);
						Standard_Boolean ok = myReader.TransferRoot(i);

						if(ok)
						{
							theShape = myReader.Shape(i);
							theEntitySequence->Append(theShape);
						}
						else
							cout << "_#_McCadExDllStep_Step.cxx :: a shape could not be read!" << endl;
					}
//MYMOD #

					/*theMessage->Print(myFileName + TCollection_AsciiString(" has been Imported."));
					delete theMessage;
					delete theError;
					delete theProgress;*/
					return theEntitySequence;
				}
			}
			else
			{
				/*theError->Print(TCollection_AsciiString("Can not Import the file:") + myFileName );
				theError->Print(TCollection_AsciiString("Error from the Step Reader"));
				delete theError;
				delete theMessage;*/
				return theEntitySequence;
			}
			theFile.Close();
		}
		else
		{
			/*theError->Print(myFileName + TCollection_AsciiString(" does not exist."));
			delete theMessage;
			delete theError;
			delete theProgress;*/
			return theEntitySequence;
		}
	}
	else
	{
		/*theError->Print(TCollection_AsciiString("Select file first."));
		delete theMessage;
		delete theError;
		delete theProgress;*/
		return theEntitySequence;
    }

    return theEntitySequence;
}


TCollection_AsciiString McCadExDllStep_Step::GetFilename() const
{
	return myFileName;
}


void McCadExDllStep_Step::SetFilename(const TCollection_AsciiString& theFileName)
{
	myFileName = theFileName;
}


Standard_Integer McCadExDllStep_Step::GetSize() const
{
	return mySize;
}


Handle(TColStd_HSequenceOfAsciiString) McCadExDllStep_Step::GetExtensions() const
{
	return myExtensions;
}


TCollection_AsciiString McCadExDllStep_Step::GetExtension() const
{
	return myExtensions->Value(1);
}


TCollection_AsciiString McCadExDllStep_Step::GetFormat() const
{
	return myFormat;
}


Standard_Boolean McCadExDllStep_Step::ExportEnabled() const
{
	return myExportEnabled;
}


Standard_Boolean McCadExDllStep_Step::ImportEnabled() const
{
	return myImportEnabled;
}


Standard_Boolean McCadExDllStep_Step::IsFacetedBrep(const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{
	Standard_Boolean OneErrorFound = Standard_False;
	for (Standard_Integer i=1;i<=aHSequenceOfShape->Length();i++)
	{
		TopoDS_Shape aShape= aHSequenceOfShape->Value(i);

		TopExp_Explorer Ex(aShape,TopAbs_FACE);
		while (Ex.More() && !OneErrorFound)
		{
			// Get the      Geom_Surface outside the TopoDS_Face
			Handle(Geom_Surface) aSurface = BRep_Tool::Surface(TopoDS::Face(Ex.Current()));
			// check if it is a plane.
			if (!aSurface->IsKind(STANDARD_TYPE(Geom_Plane)))
			OneErrorFound=Standard_True;
			Ex.Next();
		}
		TopExp_Explorer Ex2(aShape,TopAbs_EDGE);
		while (Ex2.More() && !OneErrorFound)
		{
			// Get the      Geom_Curve outside the TopoDS_Face
			Standard_Real FirstDummy,LastDummy;
			Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(Ex2.Current()),FirstDummy,LastDummy);
			// check if it is a line.
			if (!aCurve->IsKind(STANDARD_TYPE(Geom_Line)))
			OneErrorFound=Standard_True;
			Ex2.Next();
		}
	}
	return !OneErrorFound;
}


Standard_Boolean McCadExDllStep_Step::ImportToDocument(Handle(TDocStd_Document)& theTDoc)
{
    STEPCAFControl_Reader theReader;

//qiu remove gui, might be dangerous!	McCadGUI_ProgressDialogPtr testD =  new McCadGUI_ProgressDialog(QMcCad_Application::GetAppMainWin());
	McCadGUI_ProgressDialogPtr testD =  new McCadGUI_ProgressDialog();//qiu
	TCollection_AsciiString theFile = myFileName;
	if(theFile.Search("/")>0)
		theFile.Remove(1,theFile.SearchFromEnd("/"));
	TCollection_AsciiString labelText = TCollection_AsciiString("Importing file \"") + theFile +TCollection_AsciiString("\"");
	testD->SetLabelText(labelText);
	Handle(Message_ProgressIndicator) theProgress = Handle(Message_ProgressIndicator)::DownCast(testD);

	theReader.SetNameMode(Standard_True);
	theReader.SetColorMode(Standard_True);
        //theReader.SetMatMode(Standard_True);

	theReader.Reader().WS()->MapReader()->SetProgress(theProgress);
	theProgress->NewScope(10,"Load File");
        theReader.ReadFile(myFileName.ToCString());
	theProgress->EndScope();

	Standard_Boolean aStatus=Standard_False;

	theReader.Reader().WS()->MapReader()->SetProgress(theProgress);
	theProgress->NewScope(90,"Transfer Data");


	Handle(TDocStd_Document) tstDoc;

//qiu remove gui, might be dangerous!	QMcCad_Application::GetAppMainWin()->GetXCAFApp()->NewDocument("XmlXCAF", tstDoc);
	McCadXCAF_Application::GetApplication()->NewDocument("XmlXCAF", tstDoc);//qiu find from QMcCad_Application
	aStatus = theReader.Transfer(tstDoc);

	Handle(XCAFDoc_ShapeTool) _sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
	Handle(XCAFDoc_ColorTool) _cTool = XCAFDoc_DocumentTool::ColorTool(theTDoc->Main());
	Handle(XCAFDoc_ColorTool) cTool = XCAFDoc_DocumentTool::ColorTool(tstDoc->Main());

	McCadXCAF_TDocShapeLabelIterator it(tstDoc);

	for(; it.More(); it.Next())
	{
		TDF_Label curLab = it.Current();
		Handle(TNaming_NamedShape) nmdShp;

		if(!curLab.FindAttribute(TNaming_NamedShape::GetID(), nmdShp))
			continue;

		TopoDS_Shape curShp = nmdShp->Get();
		TDF_Label newLab = _sTool->AddShape(curShp, Standard_False, Standard_False);
		Handle(TDataStd_Name) theName;
		TCollection_AsciiString newName;
		if(!curLab.FindAttribute(TDataStd_Name::GetID(),theName))
		{
			TCollection_AsciiString labelName;
			TDF_Tool::Entry(curLab, labelName);
			newName = labelName.Split(labelName.SearchFromEnd(":"));
			newName.Prepend("Solid ");
		}
		else
		{
			newName = TCollection_AsciiString(theName->Get());

			if(newName.Length() == 0)
			{
				TCollection_AsciiString labelName;
				TDF_Tool::Entry(curLab, labelName);
				newName = labelName.Split(labelName.SearchFromEnd(":"));
				newName.Prepend("Solid ");
			}
		}

        TDataStd_Name::Set(newLab, newName);

		// for some reasons we couldn't find colorgen... we can use colors from surfaces though...
        // Quantity_Color newCol;
        Quantity_Color newCol (0, 0, 0, Quantity_TOC_RGB);
		TDF_Label cLab;
        if(cTool->GetColor(curShp, XCAFDoc_ColorGen ,newCol))
		{
			TDF_Label _cLab = _cTool->AddColor(newCol);
			_cTool->SetColor(curShp, _cLab, XCAFDoc_ColorGen);
		}
		else if(cTool->GetColor(curShp, XCAFDoc_ColorSurf ,newCol))
		{
			TDF_Label _cLab = _cTool->AddColor(newCol);
			_cTool->SetColor(curShp, _cLab, XCAFDoc_ColorGen);
		}
		else if(cTool->GetColor(curShp, XCAFDoc_ColorCurv ,newCol))
		{
			TDF_Label _cLab = _cTool->AddColor(newCol);
			_cTool->SetColor(curShp, _cLab, XCAFDoc_ColorGen);
        }
		//newLab.AddAttribute(theName);
    }
    //aStatus = theReader.Transfer(theTDoc);
    theProgress->EndScope();

    return aStatus;
}

Standard_Boolean McCadExDllStep_Step::ExportFromDocument(const Handle(TDocStd_Document)& theTDoc)
{
	STEPCAFControl_Writer theWriter;
    Standard_Boolean T = Standard_True;
	theWriter.SetNameMode(T);
	theWriter.SetColorMode(T);
	theWriter.SetMaterialMode(T);

//qiu remove gui, might be dangerous!	McCadGUI_ProgressDialogPtr testD =  new McCadGUI_ProgressDialog(QMcCad_Application::GetAppMainWin());
	McCadGUI_ProgressDialogPtr testD =  new McCadGUI_ProgressDialog(/*qiu*/); 
	TCollection_AsciiString theFileN = myFileName;
	if(theFileN.Search("/")>0)
		theFileN.Remove(1,theFileN.SearchFromEnd("/"));
	TCollection_AsciiString labelText = TCollection_AsciiString("Exporting geometry to \"") + theFileN +TCollection_AsciiString("\"");
	testD->SetLabelText(labelText);
	Handle(Message_ProgressIndicator) theProgress = Handle(Message_ProgressIndicator)::DownCast(testD);

	theWriter.Writer().WS()->MapReader()->SetProgress(theProgress);
	theProgress->NewScope(70,"Transfering Data");
	if(!theWriter.Transfer(theTDoc))
	{
		cout << "_#_ McCadExDllStep_Step.cxx :: Error: data not transferred!!!\n";
		return Standard_False;
	}
	theProgress->EndScope();

	OSD_Path thePath(myFileName);
	OSD_File theFile(thePath);
	theFile.Build(OSD_ReadWrite , OSD_Protection());

	theWriter.Writer().WS()->MapReader()->SetProgress(theProgress);
	theProgress->NewScope(30,"Writing File");
	IFSelect_ReturnStatus theReturnStatus = theWriter.Write(myFileName.ToCString());
	theProgress->EndScope();

	if (theReturnStatus != IFSelect_RetDone)
	{
		cout << "_::_ McCadExDllStep_Step.cxx :: Export Failed\n";
		return Standard_False;
	}

	return Standard_True;
}

//
//extern "C" {
//
//	McCadEXPlug_ExchangePlugin *_McCadEXPlug_PluginMaker()
//	{
//		return new McCadExDllStep_Step;
//	}
//
//	class PluginProxy
//	{
//		public:
//			PluginProxy()
//			{
//				// register the maker into the MakerMap
//				PluginMakerMap[".step"] = _McCadEXPlug_PluginMaker();
//				PluginMakerMap[".stp"] = _McCadEXPlug_PluginMaker();
//			}
//	};
//	// our one instance of the proxy
//	PluginProxy StepPlug;
//}
