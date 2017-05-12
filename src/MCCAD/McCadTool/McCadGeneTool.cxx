#include "McCadGeneTool.hxx"

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Solid.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <StlAPI_Writer.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS.hxx>
//qiu #include "McCadEXPlug_PluginManager.hxx"
//qiu #include "McCadEXPlug_ExchangePlugin.hxx"

McCadGeneTool::McCadGeneTool()
{
}

TCollection_AsciiString McCadGeneTool::ToAsciiString(const QString& theStr)
{
    //qiu error in windows compilation
//    char aStr[theStr.length()];
//    QByteArray ba = theStr.toAscii();
//    strcpy(aStr, ba.data());
//    TCollection_AsciiString anAsciiStr(aStr);
//    return anAsciiStr;
    return theStr.toStdString().c_str();
}

Handle_TopTools_HSequenceOfShape McCadGeneTool::readFile( TCollection_AsciiString FileName)
{

    TopoDS_Shape aShape ;

    TCollection_AsciiString fileExtension = FileName;
    fileExtension = fileExtension.Split(fileExtension.SearchFromEnd("."));

    if(fileExtension == "brep" ){
        BRep_Builder aBuilder;
        BRepTools::Read(aShape,FileName.ToCString(),aBuilder);
    }
    else if( fileExtension == "iges" || fileExtension == "igs"){
        IGESControl_Reader Reader;
        Standard_Integer status = Reader.ReadFile( FileName.ToCString() );
        if (status != IFSelect_RetDone ) {
            cout<<"ERROR: Cannot read this file as iges format! "<<endl;
            return NULL;
        }
        Reader.TransferRoots();
        aShape = Reader.OneShape();
    }
    else if( fileExtension == "step" || fileExtension == "stp"){

        STEPControl_Reader Reader;
        Standard_Integer status = Reader.ReadFile( FileName.ToCString());
        if (status != IFSelect_RetDone ) {
            cout<<"ERROR: Cannot read this file as STEP format! "<<endl;
            return NULL;
        }
        Reader.TransferRoots();
        aShape = Reader.OneShape();
    }
    else {
        cout<<"ERROR: Cannot read this file, unrecognized format! "<<endl;
        return NULL;
    }

    Handle_TopTools_HSequenceOfShape aInputSolidList = new TopTools_HSequenceOfShape;

     TopExp_Explorer ex;
    for (ex.Init(aShape, TopAbs_SOLID); ex.More(); ex.Next())        {

        TopoDS_Solid tmpSol = TopoDS::Solid(ex.Current());
        aInputSolidList->Append(tmpSol);
    }

    return aInputSolidList;

}

void McCadGeneTool::WriteFile( TCollection_AsciiString outFileName,
                               Handle_TopTools_HSequenceOfShape & shapes)
{
    //get writer plugin
    TCollection_AsciiString fileExtension = outFileName;
    fileExtension = fileExtension.Split(fileExtension.SearchFromEnd("."));

//qiu implement the export geometry function here to disconnect with McCadEXPlug_PluginManager
//    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();
//    McCadEXPlug_ExchangePlugin* writerPlugin = pluginManager->GetPlugin(fileExtension);
//    // write file
//    writerPlugin->SetFilename(outFileName);
//    writerPlugin->Export(shapes);

    //obtain the geom and make a compound
    TopoDS_Compound aCompound;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound (aCompound);
    for (int i = 1; i <= shapes->Length(); i++) //i must start from 1 !!
    {

        TopoDS_Shape aShape =  shapes->Value(i);
            aBuilder.Add (aCompound, aShape);
    }


    if (fileExtension.IsEmpty()) { //if no extention, add one
        outFileName += ".stp";
        fileExtension = "stp";
    }

    if(fileExtension == "brep" ){

        BRepTools::Write(aCompound, outFileName.ToCString());
    }
    else if( fileExtension == "iges" || fileExtension == "igs"){
//        new IGESControl_Controller().init(); //??
        IGESControl_Writer aWriter ("MM", 0); //unit : mm
        aWriter.AddShape(aCompound);
        aWriter.ComputeModel();
        aWriter.Write(outFileName.ToCString());
    }
    else if( fileExtension == "step" || fileExtension == "stp"){
        STEPControl_Writer aWriter  ;
        aWriter.Transfer(aCompound, STEPControl_AsIs);
        aWriter.Write(outFileName.ToCString());
    }
    else {  //other case
        TCollection_AsciiString aFileName= outFileName + ".stp";
        STEPControl_Writer aWriter  ;
        aWriter.Transfer(aCompound, STEPControl_AsIs);
        aWriter.Write(aFileName.ToCString());
    }

}
