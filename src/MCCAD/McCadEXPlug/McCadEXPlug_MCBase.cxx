#include <McCadDiscDs_HSequenceOfDiscSolid.hxx>
#include <McCadDiscDs_DiscSolid.hxx>
#include <McCadEXPlug_MCBase.hxx>
#include <McCadIOHelper_VGParameters.hxx>
#include <McCadMessenger_Singleton.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS.hxx>
#include <BRepGProp.hxx>

#include <McCadXCAF_Application.hxx> //qiu 

McCadEXPlug_MCBase::McCadEXPlug_MCBase() {
	myRecoverVoids = Standard_False;
}

Handle_McCadDiscDs_HSequenceOfDiscSolid	McCadEXPlug_MCBase::PrepareExport(const Handle_TDocStd_Document& theTDoc) {
    McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();

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
        return originalSolids;
    }

    // print message about faces
    TCollection_AsciiString faceMessage("\n Face Info\n=========\nsmallest face area : ");
    faceMessage += smallestFaceArea;
    faceMessage += "  --  biggest face area : ";
    faceMessage += biggestFaceArea;
    faceMessage += "\n";
    msgr->Message(faceMessage.ToCString());

    // start parameter dialog
    QApplication::restoreOverrideCursor();
    QMcCad_VGParamDialog* theVGParamDialog = new QMcCad_VGParamDialog();
    theVGParamDialog->exec();
    if(!theVGParamDialog->Accepted())
        return originalSolids;

    // init progress indicator
//qiu remove gui, might be dangerous!        McCadGUI_ProgressDialogPtr theProgress = new McCadGUI_ProgressDialog(QMcCad_Application::GetAppMainWin());
	    McCadGUI_ProgressDialogPtr theProgress = new McCadGUI_ProgressDialog(/*qiu*/);
    myPI = Handle_Message_ProgressIndicator::DownCast(theProgress);
    theProgress->SetLabelText("Decomposing Geometry");
    theProgress->SetTotalSteps(100);
    theProgress->SetProgress(0);

    myUnits = theVGParamDialog->GetUnits();
    myInitCellNb = theVGParamDialog->GetInitialCellNumber() -1 ;
    myInitSurfNb = theVGParamDialog->GetInitialSurfaceNumber() -1 ;

    Standard_Real localWorkLoad = 20;
    myPI->SetRange(0,100);
    myPI->SetStep(localWorkLoad/(solidCnt+1));
    myPI->Increment();
    shapeIterator.Initialize(theTDoc);

    //decompose all shapes
    ///////////////////////////////
    McCadConvertTools_Convertor convertor;
    Handle_TopTools_HSequenceOfShape bbShapes = new TopTools_HSequenceOfShape;
    msgr->Message("\nPerforming sign constant decomposition...",
                          McCadMessenger_HeadingMsg);

    for(; shapeIterator.More(); shapeIterator.Next(), myPI->Increment()) {
        TDF_Label curL = shapeIterator.Current();
        TopoDS_Shape curS;

        //myPI->Increment();
        if(myPI->UserBreak()) {
            myPI->SetValue(100);
            return originalSolids;
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
            myPI->SetStep(localWorkLoad/(solidCnt));
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
    myPI->SetValue(20.);

    // decomposition failed
    if(originalSolids->Length() < 1) {
        myPI->SetValue(100);
        return originalSolids;
    }


    //Load material and density information
    ///////////////////////////////////////////
    McCadMDReader_Reader newMDReader(theTDoc);
    myMDReader = newMDReader;

    // export decomposed model to step file
    ///////////////////////////////////////
//qiu remove gui, might be dangerous!    Handle_TDocStd_Application theXCAFApp = QMcCad_Application::GetAppMainWin()->GetXCAFApp();
	    Handle_TDocStd_Application theXCAFApp = McCadXCAF_Application::GetApplication(); //qiu find from QMcCad_Application
    Handle_TDocStd_Document decTDoc;
    theXCAFApp->NewDocument("XmlXCAF", decTDoc); //initialize Document
    Handle_XCAFDoc_ShapeTool sTool = XCAFDoc_DocumentTool::ShapeTool(decTDoc->Main());

    TCollection_AsciiString saveName("mc_Material_Cells.stp");

    for(Standard_Integer i=1; i<=originalSolids->Length(); i++) {
        //append solids, names, material information to the document of decomposed solids
        TDataStd_Name curName;

        TCollection_ExtendedString shpName(originalSolids->Value(i).GetName());
        TDF_Label curLab = sTool->AddShape(originalSolids->Value(i).GetSolid());

        curName.Set(curLab, shpName);
    }

    // write decomposed geometry into CAD file
    STEPCAFControl_Writer theWriter;
    theWriter.SetNameMode(true);
    theWriter.SetColorMode(true);
    theWriter.SetMaterialMode(true);
    theWriter.Transfer(decTDoc);
    theWriter.Write(saveName.ToCString());
    theXCAFApp->Close(decTDoc);

    msgr->Message("\nDecomposed Model has been exported...\n");
    msgr->Message("Starting Void Completion...",
                  McCadMessenger_HeadingMsg);

    // set parameters for VG
    McCadIOHelper_VGParameters parameters;
    parameters.inputSolids = new McCadDiscDs_HSequenceOfDiscSolid;
    parameters.boundingBox = new TopTools_HSequenceOfShape;
    if(bbShapes->Length()>0) {
        parameters.boundingBox->Append(bbShapes);
    }
    parameters.writeCollisionFile  = theVGParamDialog->GetWriteCollision();
    parameters.writeVoxelFile      = theVGParamDialog->GetWriteDiscretModel();
    parameters.minInputSolidVolume = theVGParamDialog->GetMinInputVolume();
    parameters.minVoidVolume       = theVGParamDialog->GetMinVoidVolume();
    parameters.minSamplePoints     = theVGParamDialog->GetMinSamplePnts();
    parameters.maxSamplePoints     = theVGParamDialog->GetMaxSamplePnts();
    parameters.xResolution         = theVGParamDialog->GetXRes();
    parameters.yResolution         = theVGParamDialog->GetYRes();
    parameters.tolerance           = theVGParamDialog->GetTolerance();
    parameters.initCellNumber      = myInitCellNb;
    parameters.initSurfaceNumber   = myInitSurfNb;
    parameters.units               = myUnits;
    parameters.minSizeOfDecompositionFace       = theVGParamDialog->GetMinDecFace();
    parameters.minSizeOfRedecompositionFace     = theVGParamDialog->GetMinReDecFace();
    parameters.maxNumberOfPreDecompositionCells = theVGParamDialog->GetMaxPreDec();
    parameters.maxNumberOfComplementCells       = theVGParamDialog->GetMaxComplemented();

    //run void completion algorithm
    theProgress->SetLabelText("Generating Voids");
    McCadConvertTools_VoidGenerator voidGenerator;
    voidGenerator.SetParameters(parameters);
    voidGenerator.AddDiscSolids(originalSolids);
    voidGenerator.SetProgressIndicator(myPI);
    if(!voidGenerator.Perform()) {
        msgr->Message("Void Completion Failed, or progress has been aborted.\n",
                      McCadMessenger_ErrorMsg);
        myPI->SetValue(100);
        return new McCadDiscDs_HSequenceOfDiscSolid;
    }

    theProgress->SetLabelText("Building Cells");
    Handle_McCadDiscDs_HSequenceOfDiscSolid theVEntitySequence = new McCadDiscDs_HSequenceOfDiscSolid;

    theVEntitySequence->Append(voidGenerator.GetVSolids());
    theVEntitySequence->Append(voidGenerator.GetVVoids());
    theVEntitySequence->Append(voidGenerator.GetVOuterVoids());

    return theVEntitySequence;
} 


void McCadEXPlug_MCBase::RecoverVoids(const Standard_Boolean& state) {
    myRecoverVoids = state;
}
