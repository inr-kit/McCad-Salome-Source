#ifndef  _OSD_File_HeaderFile
#include <OSD_File.hxx>
#endif
#ifndef  _BRep_Builder_HeaderFile
#include <BRep_Builder.hxx>
#endif
#include <BRepTools.hxx>
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
#include <TDF_Tool.hxx>
#include <TDataStd_Name.hxx>

#include <McCadExDllBrep_Brep.ixx>
#include <OSD_Protection.hxx>


#include <McCadEXPlug_PluginMaker.hxx>

using namespace std;

//qiu we don't need "extern"
///*qiu extern */map<string, McCadEXPlug_PluginMaker , less<string> > PluginMakerMap;

McCadExDllBrep_Brep::McCadExDllBrep_Brep()
{
  myExtensions = new TColStd_HSequenceOfAsciiString;
  myExtensions->Append(TCollection_AsciiString(".brep"));
  myExtensions->Append(TCollection_AsciiString(".Brep"));
  myExtensions->Append(TCollection_AsciiString(".rle"));
  myFormat = "brep";
  myExportEnabled = Standard_True;
  myImportEnabled = Standard_True;
  myFileName = " ";
  mySize = 0;
}

Standard_Boolean McCadExDllBrep_Brep::Export(const Handle(TopTools_HSequenceOfShape)& theEntitySequence)
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
      theProgress->SetLabelText("Exporting Brep File.");
      theProgress->SetCancelButtonText("Cancel Export!");
      theProgress->SetTotalSteps(100);
      theProgress->SetProgress(0);

      TopoDS_Shape theShape = theEntitySequence->Value(1);

      Standard_Boolean isValid = BRepTools::Write(theShape,myFileName.ToCString());

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
              theError->Print(TCollection_AsciiString("Error from the Brep Writer."));
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

Handle(TopTools_HSequenceOfShape) McCadExDllBrep_Brep::Import()
{
  /*McCadGUI_GuiFactoryPtr  theGuiFactory = theGuiFactory->Instance();
  theGuiFactory->SetType(McCadGUI_Base);
  McCadGUI_BaseDialogPtr theMessage  = theGuiFactory->MakeDialog();
  McCadGUI_BaseDialogPtr theError  = theGuiFactory->MakeDialog();
  theGuiFactory->SetType(McCadGUI_Progress);
  McCadGUI_BaseDialogPtr theProgress  = theGuiFactory->MakeDialog();*/

  if (!myFileName.IsEmpty())
    {
      OSD_Path thePath(myFileName);
      OSD_File theFile(thePath);
      if (theFile.Exists())
    {
      /*theProgress->SetLabelText("Importing Brep File.");
      theProgress->SetCancelButtonText("Cancel Import!");
      theProgress->SetTotalSteps(100);
          theProgress->SetProgress(0);*/

      TopoDS_Shape theShape;
      BRep_Builder theBuilder;
      Standard_Boolean isValid = BRepTools::Read(theShape,myFileName.ToCString(),theBuilder);

//	  theProgress->SetProgress(100);

      if(isValid)
        {
          Handle(TopTools_HSequenceOfShape) theShapeSequence = new TopTools_HSequenceOfShape();
          theShapeSequence->Append(theShape);
              /*theMessage->Print(myFileName + TCollection_AsciiString(" has been Imported."));
          delete theMessage;
              delete theError;
              delete theProgress;*/
          return theShapeSequence;
        }
      else
        {
              /*theError->Print(TCollection_AsciiString("Can not Import the file:") + myFileName );
              theError->Print(TCollection_AsciiString("Error from the Brep Reader"));
              delete theError;
              delete theMessage;
              delete theProgress;*/
          return NULL;
        }
    }
      else
    {
          /*theError->Print(myFileName + TCollection_AsciiString(" does not exist."));
      delete theMessage;
          delete theError;
          delete theProgress;*/
          return NULL;
    }
    }
  else
    {
      /*theError->Print(TCollection_AsciiString("Select file first."));
      delete theMessage;
      delete theError;
      delete theProgress;*/
      return NULL;
    }

}

TCollection_AsciiString McCadExDllBrep_Brep::GetFilename() const
{
  return myFileName;
}

void McCadExDllBrep_Brep::SetFilename(const TCollection_AsciiString& theFileName)
{
  myFileName = theFileName;
}

Standard_Integer McCadExDllBrep_Brep::GetSize() const
{
  return mySize;
}

Handle(TColStd_HSequenceOfAsciiString) McCadExDllBrep_Brep::GetExtensions() const
{
  return myExtensions;
}

TCollection_AsciiString McCadExDllBrep_Brep::GetExtension() const
{
  return myExtensions->Value(1);
}

TCollection_AsciiString McCadExDllBrep_Brep::GetFormat() const
{
  return myFormat;
}

Standard_Boolean McCadExDllBrep_Brep::ExportEnabled() const
{
 return myExportEnabled;
}

Standard_Boolean McCadExDllBrep_Brep::ImportEnabled() const
{
 return myImportEnabled;
}


Standard_Boolean McCadExDllBrep_Brep::ImportToDocument(Handle(TDocStd_Document)& theTDoc)
{
    Handle(TopTools_HSequenceOfShape) inShps = Import();

    Handle(XCAFDoc_ShapeTool) _sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());

    for(Standard_Integer i=1; i<=1/*inShps->Length()*/; i++)
    {
        TDF_Label newLab = _sTool->AddShape(inShps->Value(i), 0, 0);
        TCollection_AsciiString labelName;
        TDF_Tool::Entry(newLab, labelName);
        TCollection_AsciiString newName = labelName.Split(labelName.SearchFromEnd(":"));
        newName.Prepend("Solid ");
        TDataStd_Name::Set(newLab, newName);
    }

    return Standard_True;
}

Standard_Boolean McCadExDllBrep_Brep::ExportFromDocument(const Handle(TDocStd_Document)& theTDoc)
{
    Handle(XCAFDoc_ShapeTool) _sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
    TDF_LabelSequence freeShp;
    _sTool->GetFreeShapes(freeShp);
    TopoDS_Shape theShape;
    _sTool->GetShape(freeShp.First(), theShape);

    Handle(TopTools_HSequenceOfShape) exportShape = new TopTools_HSequenceOfShape;
    exportShape->Append(theShape);

    return Export(exportShape);
}

////qiu we don't need "extern"
////qiu extern "C" {
//  McCadEXPlug_ExchangePlugin* _McCadEXPlug_PluginMaker()
//  {
//    return new McCadExDllBrep_Brep;
//  }
//
//class PluginProxy {
//public:
//  PluginProxy(){
//      // register the maker into the MakerMap
//      McCadEXPlug_ExchangePlugin* brepPlugin = _McCadEXPlug_PluginMaker();
//    PluginMakerMap[".BRep"] = brepPlugin;
//    PluginMakerMap[".brep"] = brepPlugin;
//    PluginMakerMap[".rle"] = brepPlugin;
//   }
//};
//// our one instance of the proxy
//PluginProxy  BrepPlug;
////qiu }
