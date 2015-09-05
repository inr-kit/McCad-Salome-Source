#include <McCadIOTools_MCExportBase.hxx>

#include <Handle_McCadDiscDs_HSequenceOfDiscSolid.hxx>
#include <McCadConvertTools_Convertor.hxx>
#include <McCadConvertTools_VoidGenerator.hxx>
#include <McCadDiscDs_HSequenceOfDiscSolid.hxx>
#include <McCadDiscDs_DiscSolid.hxx>
#include <McCadExDllMcnp_Mcnp.hxx>
#include <McCadEXPlug_PluginManager.hxx>
#include <McCadGUI_ProgressDialog.hxx>
#include <McCadGUI_ProgressDialogPtr.hxx>
#include <McCadMDReader_Reader.hxx>
#include <McCadMessenger_Singleton.hxx>
#include <McCadXCAF_TDocShapeLabelIterator.hxx>
//qiu remove gui, might be dangerous! #include <QMcCad_Application.h>
#include <McCadXCAF_Application.hxx> //qiu
#include <QMcCad_VGParamDialog.h>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>

McCadIOTools_MCExportBase::McCadIOTools_MCExportBase( const Handle_McCadCom_CasDocument &theDoc,
                                                      const Handle_McCadCom_CasView &theView,
                                                      const McCadTool_State theState,
                                                      const Standard_Boolean theUndoState,
                                                      const Standard_Boolean theRedoState )
{
    myDoc = theDoc;
    myView = theView;
    myState = theState;
    myUndoState = theUndoState;
    myRedoState = theRedoState;
    myID = McCadTool_ExportMCBase;

    m_parameters.inputSolids = new McCadDiscDs_HSequenceOfDiscSolid;
    m_parameters.boundingBox = new TopTools_HSequenceOfShape;
}

McCadIOTools_MCExportBase::McCadIOTools_MCExportBase(){

}


void McCadIOTools_MCExportBase::Execute(){
    // do nothing
}

void McCadIOTools_MCExportBase::PrepareExport(){
    McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();

    Handle_TDocStd_Document theTDoc = myDoc->GetTDoc();

    // count number of solids and determine face areas
    McCadXCAF_TDocShapeLabelIterator shapeIterator(theTDoc);
    Standard_Real smallestFaceArea(1.0e+99), biggestFaceArea(0.0);
    Standard_Integer solidCnt(0);
    Handle_XCAFDoc_ShapeTool shapeTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
    Handle_McCadDiscDs_HSequenceOfDiscSolid originalSolids = new McCadDiscDs_HSequenceOfDiscSolid;
    for(; shapeIterator.More(); shapeIterator.Next()) {
        TopoDS_Shape curSolid = shapeTool->GetShape(shapeIterator.Current());
        // find smallest and biggest face
        for(TopExp_Explorer faceExp(curSolid, TopAbs_FACE); faceExp.More(); faceExp.Next()) {
            TopoDS_Face f1 = TopoDS::Face(faceExp.Current());
            GProp_GProps GP1;
            BRepGProp::SurfaceProperties(f1, GP1);
            if(GP1.Mass() > biggestFaceArea)
                biggestFaceArea = GP1.Mass();
            else if(GP1.Mass() < smallestFaceArea)
                smallestFaceArea = GP1.Mass();
        }
        solidCnt++;
    }

    // no solids found
    if(solidCnt == 0) {
        msgr->Message("_#_McCadEXPlug_MCBase :: No solids found!!!\n",
                      McCadMessenger_ErrorMsg);
        return;
    }

    // print message about faces
    TCollection_AsciiString faceMessage("\n Face Info\n=========\nsmallest face area : ");
    faceMessage += smallestFaceArea;
    faceMessage += "  --  biggest face area : ";
    faceMessage += biggestFaceArea;
    faceMessage += "\n";
    msgr->Message(faceMessage.ToCString());

    // start parameter dialog
    QMcCad_VGParamDialog* theVGParamDialog = new QMcCad_VGParamDialog();
    theVGParamDialog->exec();
    if(!theVGParamDialog->Accepted())
        return;

    // init progress indicator
//qiu remove gui, might be dangerous!     McCadGUI_ProgressDialogPtr theProgress = new McCadGUI_ProgressDialog(QMcCad_Application::GetAppMainWin());
	    McCadGUI_ProgressDialogPtr theProgress = new McCadGUI_ProgressDialog(/*qiu*/);
    Handle_Message_ProgressIndicator progressIndicator = Handle_Message_ProgressIndicator::DownCast(theProgress);
    theProgress->SetLabelText("Decomposing Geometry");
    theProgress->SetTotalSteps(100);
    theProgress->SetProgress(0);

    Standard_Real localWorkLoad = 20;
    progressIndicator->SetRange(0,100);
    progressIndicator->SetStep(localWorkLoad/(solidCnt+1));
    progressIndicator->Increment();
    shapeIterator.Initialize(theTDoc);

    //decompose all shapes
    ///////////////////////////////
    McCadConvertTools_Convertor convertor;
    Handle_TopTools_HSequenceOfShape bbShapes = new TopTools_HSequenceOfShape;
    msgr->Message("\nPerforming sign constant decomposition...",
                          McCadMessenger_HeadingMsg);

    for(; shapeIterator.More(); shapeIterator.Next(), progressIndicator->Increment()) {
        TDF_Label curL = shapeIterator.Current();
        TopoDS_Shape curS;

        //progressIndicator->Increment();
        if(progressIndicator->UserBreak()) {
            progressIndicator->SetValue(100);
            return;
        }

        if(!shapeTool->IsAssembly(curL))
            curS = shapeTool->GetShape(curL);
        else
            continue;

        Handle_TDataStd_Name tShpName;
        TCollection_AsciiString shpName;

        if(!curL.FindAttribute(TDataStd_Name::GetID(),tShpName))
            shpName = "unnamed";
        else
            shpName = TCollection_AsciiString(tShpName->Get());

        Handle_TopTools_HSequenceOfShape shpSeq = new TopTools_HSequenceOfShape;
        shpSeq->Append(curS);



        TCollection_AsciiString decMsg("\n+++++++++++++++++++++++++++++++++++++++++++++\n");
        decMsg += shpName;
        decMsg += "\n+++++++++++++++++++++++++++++++++++++++++++++";
        msgr->Message(decMsg.ToCString());

        // decompose solid
        Handle_TopTools_HSequenceOfShape decSolid = new TopTools_HSequenceOfShape;
        decSolid = convertor.Convert(shpSeq);

        // look for bounding box
        TCollection_AsciiString tmpNm(shpName);
        tmpNm.UpperCase();
        tmpNm.LeftAdjust();
        tmpNm.RightAdjust();
        tmpNm.RemoveAll(' ');
        tmpNm.RemoveAll('_');
        tmpNm.RemoveAll('-');

        if(tmpNm.Search("BOUNDINGBOX") > 0) {
            bbShapes->Append(decSolid);
            solidCnt--;
            progressIndicator->SetStep(localWorkLoad/(solidCnt));
            continue;
        }

        // process decomposed shapes
        for(Standard_Integer i=1; i<=decSolid->Length(); i++) {
            TCollection_AsciiString appendix(i);
            appendix.Prepend("_");
            McCadDiscDs_DiscSolid tmpSolid;
            tmpSolid.SetSolid(decSolid->Value(i));
            tmpSolid.SetName(shpName+appendix);

            originalSolids->Append(tmpSolid);
        }
    }
    progressIndicator->SetValue(20.);

    // decomposition failed
    if(originalSolids->Length() < 1) {
        progressIndicator->SetValue(100);
        return;
    }

    //Load material and density information
    ///////////////////////////////////////////
    McCadMDReader_Reader mdReader(theTDoc);
    m_mdReader = mdReader;

    // export decomposed model to step file
    ///////////////////////////////////////
//qiu remove gui, might be dangerous!     Handle_TDocStd_Application theXCAFApp = QMcCad_Application::GetAppMainWin()->GetXCAFApp();
	    Handle_TDocStd_Application theXCAFApp = McCadXCAF_Application::GetApplication(); //qiu find from QMcCad_Application
    Handle_TDocStd_Document decomposedTDoc;
    theXCAFApp->NewDocument("XmlXCAF", decomposedTDoc); //initialize Document
    Handle_XCAFDoc_ShapeTool sTool = XCAFDoc_DocumentTool::ShapeTool(decomposedTDoc->Main());

    //append solids, names, material information to the document of decomposed solids
    for(Standard_Integer i=1; i<=originalSolids->Length(); i++) {
        TDataStd_Name curName;
        TCollection_ExtendedString shpName(originalSolids->Value(i).GetName());
        TDF_Label curLab = sTool->AddShape(originalSolids->Value(i).GetSolid());
        curName.Set(curLab, shpName);
    }

    // write decomposed geometry into CAD file
    TCollection_AsciiString saveName("mc_Material_Cells.stp");
    STEPCAFControl_Writer theWriter;
    theWriter.SetNameMode(true);
    theWriter.SetColorMode(true);
    theWriter.SetMaterialMode(true);
    theWriter.Transfer(decomposedTDoc);
    theWriter.Write(saveName.ToCString());
    theXCAFApp->Close(decomposedTDoc);

    msgr->Message("\nDecomposed Model has been exported...\n");
    msgr->Message("Starting Void Completion...",
                  McCadMessenger_HeadingMsg);

    // set parameters for VoidGenerator
    if(bbShapes->Length()>0) {
        m_parameters.boundingBox->Append(bbShapes);
    }
    m_parameters.writeCollisionFile  = theVGParamDialog->GetWriteCollision();
    m_parameters.writeVoxelFile      = theVGParamDialog->GetWriteDiscretModel();
    m_parameters.minInputSolidVolume = theVGParamDialog->GetMinInputVolume();
    m_parameters.minVoidVolume       = theVGParamDialog->GetMinVoidVolume();
    m_parameters.minSamplePoints     = theVGParamDialog->GetMinSamplePnts();
    m_parameters.maxSamplePoints     = theVGParamDialog->GetMaxSamplePnts();
    m_parameters.xResolution         = theVGParamDialog->GetXRes();
    m_parameters.yResolution         = theVGParamDialog->GetYRes();
    m_parameters.tolerance           = theVGParamDialog->GetTolerance();
    m_parameters.initCellNumber      = theVGParamDialog->GetInitialCellNumber();
    m_parameters.initSurfaceNumber   = theVGParamDialog->GetInitialSurfaceNumber();
    m_parameters.units               = theVGParamDialog->GetUnits();
    m_parameters.minSizeOfDecompositionFace       = theVGParamDialog->GetMinDecFace();
    m_parameters.minSizeOfRedecompositionFace     = theVGParamDialog->GetMinReDecFace();
    m_parameters.maxNumberOfPreDecompositionCells = theVGParamDialog->GetMaxPreDec();
    m_parameters.maxNumberOfComplementCells       = theVGParamDialog->GetMaxComplemented();

    //run void completion algorithm
    theProgress->SetLabelText("Generating Voids");
    McCadConvertTools_VoidGenerator voidGenerator;
    voidGenerator.SetParameters(m_parameters);
    voidGenerator.AddDiscSolids(originalSolids);
    voidGenerator.SetProgressIndicator(progressIndicator);
    if(!voidGenerator.Perform()) {
        msgr->Message("Void Completion Failed, or progress has been aborted.\n",
                      McCadMessenger_ErrorMsg);
        progressIndicator->SetValue(100);
        return;
    }

    theProgress->SetLabelText("Building Cells");
    m_voxelizedOutputGeometry = new McCadDiscDs_HSequenceOfDiscSolid;
    m_voxelizedOutputGeometry->Append(voidGenerator.GetVSolids());
    m_voxelizedOutputGeometry->Append(voidGenerator.GetVVoids());
    m_voxelizedOutputGeometry->Append(voidGenerator.GetVOuterVoids());
}
