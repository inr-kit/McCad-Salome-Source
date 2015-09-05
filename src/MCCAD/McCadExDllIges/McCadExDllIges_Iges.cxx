#ifndef  _OSD_File_HeaderFile
#include <OSD_File.hxx>
#endif
#ifndef  _IGESControl_Reader_HeaderFile
#include <IGESControl_Reader.hxx>
#endif
#ifndef  _IGESControl_Controller_HeaderFile
#include <IGESControl_Controller.hxx>
#endif
#ifndef  _IGESControl_Writer_HeaderFile
#include <IGESControl_Writer.hxx>
#endif
#ifndef  _Interface_Static_HeaderFile
#include <Interface_Static.hxx>
#endif
#ifndef  _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#include <map>
#include <string>

#include <McCadGUI_BaseDialogPtr.hxx>
#include <McCadGUI_BaseDialog.hxx>
#include <McCadGUI_ProgressDialogPtr.hxx>
#include <McCadGUI_ProgressDialog.hxx>
#include <McCadGUI_GuiFactoryPtr.hxx>
#include <McCadGUI_GuiFactory.hxx>
#include <McCadExDllIges_Iges.ixx>
#include <OSD_Protection.hxx>
#include <IGESCAFControl_Writer.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <McCadXCAF_TDocShapeLabelIterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Name.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <Quantity_Color.hxx>
//qiu remove gui, might be dangerous!  #include <QMcCad_Application.h>
#include <McCadXCAF_Application.hxx> //qiu
#include <TDocStd_Application.hxx>

#include <McCadEXPlug_PluginMaker.hxx>

using namespace std;

//qiu we don't need "extern"
///*qiu extern */map<string, McCadEXPlug_PluginMaker , less<string> > PluginMakerMap;

McCadExDllIges_Iges::McCadExDllIges_Iges()
{
  myExtensions = new TColStd_HSequenceOfAsciiString;
  myExtensions->Append(TCollection_AsciiString(".iges"));
  myExtensions->Append(TCollection_AsciiString(".igs"));
  myFormat = "iges";
  myExportEnabled = Standard_True;
  myImportEnabled = Standard_True;
  myFileName = " ";
  mySize = 0;
}

Standard_Boolean McCadExDllIges_Iges::Export(const Handle(TopTools_HSequenceOfShape)& theEntitySequence)
{
  McCadGUI_GuiFactoryPtr  theGuiFactory = theGuiFactory->Instance();
  theGuiFactory->SetType(McCadGUI_Base);
  McCadGUI_BaseDialogPtr theMessage  = theGuiFactory->MakeDialog();
  McCadGUI_BaseDialogPtr theError  = theGuiFactory->MakeDialog();
  theGuiFactory->SetType(McCadGUI_Progress);
  McCadGUI_BaseDialogPtr theProgress  = theGuiFactory->MakeDialog();

  if (theEntitySequence->Length() == 0)
    {
      theError->Print(TCollection_AsciiString("The model is empty, can not be  exported."));
      theError->Print(TCollection_AsciiString("Error: exporting empty model."));
      delete theError;
      delete theMessage;
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
      theProgress->SetLabelText("Preparing Iges File.");
      theProgress->SetCancelButtonText("Cancel Export!");
      theProgress->SetTotalSteps(100);
      theProgress->SetProgress(0);

      IGESControl_Controller::Init();
      IGESControl_Writer myWriter (Interface_Static::CVal("XSTEP.iges.unit"),Interface_Static::IVal("XSTEP.iges.writebrep.mode"));

      for (Standard_Integer i=1;i<=theSequenceLength;i++){
             theProgress->SetProgress(i*100/theSequenceLength);
         TopoDS_Shape myShape = theEntitySequence->Value(i);
         if ( !myShape.IsNull() )
           {
         myWriter.AddShape(myShape);
           }
      }

      // the compute dialog.
      theProgress->SetLabelText("Computing Iges Model.");
      theProgress->SetCancelButtonText("Cancel Export!");
      theProgress->SetTotalSteps(100);
      theProgress->SetProgress(0);
      myWriter.ComputeModel();
          theProgress->SetProgress(100);

          theProgress->SetLabelText("Exporting Iges File.");
      theProgress->SetCancelButtonText("Cancel Export!");
      theProgress->SetTotalSteps(100);
      theProgress->SetProgress(0);
      Standard_Boolean isValid = myWriter.Write(myFileName.ToCString());
      theProgress->SetProgress(100);

      if(isValid)
        {
          theMessage->Print(myFileName + TCollection_AsciiString(" has been Exported."));
          delete theMessage;
              delete theError;
              delete theProgress;
          return isValid;
        }
      else
        {
          theError->Print(TCollection_AsciiString("Can not Export to the file:") + myFileName );
              theError->Print(TCollection_AsciiString("Error from the Iges Writer."));
          delete theError;
              delete theMessage;
              delete theProgress;
          return Standard_False;
        }
    }
      else
    {
      theError->Print(myFileName + TCollection_AsciiString(" does not exist."));
      delete theMessage;
          delete theError;
          delete theProgress;
          return Standard_False;
    }
    }
  else
    {
      theError->Print(TCollection_AsciiString("Select file first."));
      delete theMessage;
      delete theError;
      delete theProgress;
      return Standard_False;
    }


}

Handle(TopTools_HSequenceOfShape) McCadExDllIges_Iges::Import()
{
  Handle(TopTools_HSequenceOfShape) theEnititySequence;
  McCadGUI_GuiFactoryPtr  theGuiFactory = theGuiFactory->Instance();
  theGuiFactory->SetType(McCadGUI_Base);
  McCadGUI_BaseDialogPtr theMessage  = theGuiFactory->MakeDialog();
  McCadGUI_BaseDialogPtr theError  = theGuiFactory->MakeDialog();
  theGuiFactory->SetType(McCadGUI_Progress);
  McCadGUI_BaseDialogPtr theProgress  = theGuiFactory->MakeDialog();

if (!myFileName.IsEmpty())
    {

      OSD_Path thePath(myFileName);
      OSD_File theFile(thePath);

      if (theFile.Exists())
    {

      theProgress->SetLabelText("Reading Iges file.");
      theProgress->SetCancelButtonText("Cancel Reading!");
      theProgress->SetTotalSteps(100);
      theProgress->SetProgress(0);

      IGESControl_Reader myReader;
      IFSelect_ReturnStatus myReturnStatus = myReader.ReadFile(myFileName.ToCString());

      theProgress->SetProgress(100);

      if ( myReturnStatus == IFSelect_RetDone)
        {
              theProgress->SetProgress(0);
          theProgress->SetLabelText("Translating Iges file.");
          theProgress->SetCancelButtonText("Cancel Translation!");
          theProgress->SetTotalSteps(100);

          myReader.TransferRoots();

          theProgress->SetProgress(100);

          Standard_Integer theNumberOfEnteties = myReader.NbShapes();

              theProgress->SetProgress(0);
          theProgress->SetLabelText("Inserting Shapes.");
          theProgress->SetCancelButtonText("Cancel Insertion!");
          theProgress->SetTotalSteps(theNumberOfEnteties);

          if (theNumberOfEnteties != 0)
        {
          TopoDS_Shape theShape;
          theEnititySequence = new TopTools_HSequenceOfShape();
          for (Standard_Integer i = 1; i <= theNumberOfEnteties ; i++)
            {
              theProgress->SetProgress(i*100/theNumberOfEnteties);
              theShape = myReader.Shape(i);
              theEnititySequence->Append(theShape);
            }
        }
          theMessage->Print(myFileName + TCollection_AsciiString(" has been Imported."));
          delete theMessage;
              delete theError;
          delete theProgress;
              return theEnititySequence;
        }
      else
        {
          theError->Print(TCollection_AsciiString("Can not Import the file:") + myFileName );
              theError->Print(TCollection_AsciiString("Error from the Iges Reader"));
          delete theError;
              delete theMessage;
          return NULL;
        }
    }
      else
    {
      theError->Print(myFileName + TCollection_AsciiString(" does not exist."));
      delete theMessage;
          delete theError;
          return NULL;
    }
    }
  else
    {
      theError->Print(TCollection_AsciiString("Select file first."));
      delete theMessage;
      delete theError;
      return NULL;
    }
}


TCollection_AsciiString McCadExDllIges_Iges::GetFilename() const
{
  return myFileName;
}

void McCadExDllIges_Iges::SetFilename(const TCollection_AsciiString& theFileName)
{
  myFileName = theFileName;
}

Standard_Integer McCadExDllIges_Iges::GetSize() const
{
  return mySize;
}

Handle(TColStd_HSequenceOfAsciiString) McCadExDllIges_Iges::GetExtensions() const
{
  return myExtensions;
}

TCollection_AsciiString McCadExDllIges_Iges::GetExtension() const
{
  return myExtensions->Value(1);
}

TCollection_AsciiString McCadExDllIges_Iges::GetFormat() const
{
  return myFormat;
}

Standard_Boolean McCadExDllIges_Iges::ExportEnabled() const
{
  return myExportEnabled;
}

Standard_Boolean McCadExDllIges_Iges::ImportEnabled() const
{
  return myImportEnabled;
}

Standard_Boolean McCadExDllIges_Iges::ImportToDocument(Handle(TDocStd_Document)& theTDoc)
{
    IGESCAFControl_Reader theReader;
    //Handle(Message_ProgressIndicator) theProgress = theReader.Reader().WS()->MapReader()->GetProgress();
    theReader.SetNameMode(Standard_True);
    theReader.SetColorMode(Standard_True);
    //theReader.SetMatMode(Standard_True);
    //theReader.Reader().ClearShapes();
        theReader.ReadFile(myFileName.ToCString());

    Handle(TDocStd_Document) tstDoc;
//qiu remove gui, might be dangerous!     QMcCad_Application::GetAppMainWin()->GetXCAFApp()->NewDocument("XmlXCAF", tstDoc);
	McCadXCAF_Application::GetApplication()->NewDocument("XmlXCAF", tstDoc); //qiu from QMcCad_Application 

    if(!theReader.Transfer(tstDoc))
        return Standard_False;

    Handle(XCAFDoc_ShapeTool) _sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
    Handle(XCAFDoc_ColorTool) _cTool = XCAFDoc_DocumentTool::ColorTool(theTDoc->Main());
    Handle(XCAFDoc_ColorTool) cTool = XCAFDoc_DocumentTool::ColorTool(tstDoc->Main());

    TDF_LabelSequence Labels;
    Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(tstDoc->Main());
    sTool->GetShapes(Labels);

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
        Quantity_Color newCol;
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
    }

    return Standard_True;
}

Standard_Boolean McCadExDllIges_Iges::ExportFromDocument(const Handle(TDocStd_Document)& theTDoc)
{
    cout << "???\n";
    IGESCAFControl_Writer theWriter;

    cout << "Export\n";

    if(!theWriter.Transfer(theTDoc))
    {
        cout << "_#_ McCadExDllStep_Step.cxx :: Error: data not transferred!!!\n";
        return Standard_False;
    }
    cout << "MODEL TRANSFERED\n";

    OSD_Path thePath(myFileName);
    OSD_File theFile(thePath);
    theFile.Build(OSD_ReadWrite , OSD_Protection());

        Standard_Boolean theReturnStatus = theWriter.Write(myFileName.ToCString());
        if (!theReturnStatus)
    {
        cout << "_::_ McCadExDllStep_Step.cxx :: Export Failed\n";
        return Standard_False;
    }

    return Standard_True;
}


////qiu we don't need "extern"
////qiu extern "C" {
//  McCadEXPlug_ExchangePlugin* _McCadEXPlug_PluginMaker()
//  {
//    return new McCadExDllIges_Iges;
//  }
//
//  class PluginProxy {
//  public:
//    PluginProxy(){
//      // register the maker into the MakerMap
//        McCadEXPlug_ExchangePlugin* igesPlugin = _McCadEXPlug_PluginMaker();
//      PluginMakerMap[".iges"] = igesPlugin;
//      PluginMakerMap[".igs"] = igesPlugin;
//    }
//  };
//  // our one instance of the proxy
//  PluginProxy  IgesPlug;
////qiu }
