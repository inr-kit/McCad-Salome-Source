#include <map>
#include <string>
#ifndef  _OSD_File_HeaderFile
#include <OSD_File.hxx>
#endif

#include <McCadGUI_BaseDialogPtr.hxx>
#include <McCadGUI_BaseDialog.hxx>
#include <McCadGUI_ProgressDialogPtr.hxx>
#include <McCadGUI_ProgressDialog.hxx>
#include <McCadGUI_InputDialogPtr.hxx>
#include <McCadGUI_InputDialog.hxx>
#include <McCadGUI_GuiFactoryPtr.hxx>
#include <McCadGUI_GuiFactory.hxx>

#include <McCadEXPlug_PluginMaker.hxx>

#include <McCadCSGGeom_HSequenceOfCell.hxx>
#include <McCadTDS_HSequenceOfExtSolid.hxx>
#include <McCadCSGTool_Extender.hxx>
#include <McCadCSGTool_CellBuilder.hxx>
#include <McCadCSGGeom_Cell.hxx>
#include <McCadTDS_ExtSolid.hxx>

#include <McCadDiscDs_DiscSolid.hxx>

#include <McCadMcRead_Cell.hxx>
#include <McCadMcRead_Reader.hxx>
#include <McCadMcRead_DataMapOfIntegerCell.hxx>
#include <McCadMcRead_DataMapIteratorOfDataMapOfIntegerCell.hxx>
#include <McCadCSGGeom_DataMapOfIntegerSurface.hxx>
#include <McCadCSGGeom_DataMapOfIntegerTransformation.hxx>

#include <McCadCSGUtil_SolidBuilder.hxx>

#include <BRep_Builder.hxx>

#include <McCadMcWrite_TripoliInputGenerator.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <McCadExDllTripoli_Tripoli.hxx>
#include <OSD_Protection.hxx>

#include <McCadEXPlug_PluginMaker.hxx>
#include <McCadConvertTools_Convertor.hxx>
#include <McCadConvertTools_VoidGenerator.hxx>

#include <QMcCad_VGParamDialog.h>

#include <TDF_Label.hxx>
#include <TDF.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_AsciiString.hxx>
#include <McCadXCAF_TDocShapeLabelIterator.hxx>

#include <TDocStd_Application.hxx>
//qiu remove gui, might be dangerous! #include <QMcCad_Application.h>
#include <McCadXCAF_Application.hxx>  //qiu
#include <TDataStd_TreeNode.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>


using namespace std;

//qiu we don't need "extern"
///*qiu extern */map<string, McCadEXPlug_PluginMaker, less<string> > PluginMakerMap;

McCadExDllTripoli_Tripoli::McCadExDllTripoli_Tripoli()
{
    myExtensions = new TColStd_HSequenceOfAsciiString;
    myExtensions->Append(TCollection_AsciiString(".d"));
    myExtensions->Append(TCollection_AsciiString(".tri"));
    myFormat = "tripoli";
    myExportEnabled = Standard_True;
    myImportEnabled = Standard_True;
    myFileName = " ";
    mySize = 0;
    myUnits = McCadCSGGeom_CM;

    McCadGUI_ProgressDialogPtr theProgress = new McCadGUI_ProgressDialog();
    myPI = Handle(Message_ProgressIndicator)::DownCast(theProgress); // DUMMY

//	cout << "~~~~~~~~~~~~~~~~~~~~~\nTRIPOLI-WRITER\n~~~~~~~~~~~~~~~~~~~~~\n";

}

/*Standard_Boolean McCadExDllTripoli_Tripoli::Export(const Handle(TopTools_HSequenceOfShape)& theEntitySequence)
{
    cout << " Use with converted discrete model. " << endl;
}*/


Standard_Boolean McCadExDllTripoli_Tripoli::Export(const Handle(McCadDiscDs_HSequenceOfDiscSolid)& theVEntitySequence)
{
    /*
     * Build Cell Structure and Export Cells to File
     */

    //delete outer voids ||>>|| THEY'RE NOT NEEDED IN TRIPILI
    theVEntitySequence->Remove(theVEntitySequence->Length());
    theVEntitySequence->Remove(theVEntitySequence->Length());

    if (!myFileName.IsEmpty())//FILE NAME GIVEN
    {
        OSD_Path thePath(myFileName);
        OSD_File theFile(thePath);
        theFile.Build(OSD_ReadWrite , OSD_Protection());

        if (theFile.Exists())//FILE EXISTS
        {

            cout << " Start Cell Building   " << endl;
            cout << " Total Number of cells  =  " << theVEntitySequence->Length() << endl;
            cout << "================================================================= " << endl;

            Standard_Integer NbMat(0);
            Standard_Integer NbVoid(0);
            Standard_Integer NbOuterVoids(0);

            Handle(McCadCSGGeom_HSequenceOfCell) cellSeq = new McCadCSGGeom_HSequenceOfCell;

            myPI->SetStep(10/theVEntitySequence->Length());

            for (int its = 1; its <= theVEntitySequence->Length(); its++)
            {
                McCadDiscDs_DiscSolid currentEntity = theVEntitySequence->Value(its);
                TCollection_AsciiString currentName = currentEntity.GetName();

                if(!currentEntity.IsVoid()) // Material Cells
                {
                    NbMat++;
                    cout << "**************************************\n";
                    cout << "  Building Material Cell .......... " << its << endl;

                    Handle(McCadTDS_HSequenceOfExtSolid) extSolSeq = new McCadTDS_HSequenceOfExtSolid;

                    TopoDS_Solid sol2ext = TopoDS::Solid(currentEntity.GetSolid());

                    McCadCSGTool_Extender Mext(sol2ext);

                    if(Mext.IsDone())
                    {
                        Handle(McCadTDS_ExtSolid) mcSol = Mext.ExtSolid();
                        mcSol->SetComment(currentEntity.GetComment());
                        extSolSeq->Append(mcSol);
                    }
                    else
                    {
                        cout << "_#_McCadExDllTripoli_Tripoli :: Extending solid failed!!" << endl;
                        continue;
                    }

                    // cell building
                    ////////////////////////////////////////////////////////////////////////////
                    Standard_Real theDensity  = myMDReader.GetDensity(currentName);
                    Standard_Integer theMatNb = myMDReader.GetMaterial(currentName);

                    McCadCSGTool_CellBuilder Mcel;

                    Mcel.SetDensity(theDensity);
                    Mcel.SetMatNumber(theMatNb);
                    Mcel.SetUnits(myUnits);
                    Mcel.Init(extSolSeq);

                    //McCadCSGTool_CellBuilder Mcel(extSolSeq, theMatNb, theDensity);

                    if(Mcel.IsDone())
                    {
                        Handle(McCadCSGGeom_Cell) aCell = Mcel.GetCell();

                        //we add here the complements
                        Handle(TColStd_HSequenceOfInteger) compSeq = currentEntity.GetOtherVSolids();
                        aCell->SetComplementCells(compSeq);
                        cellSeq->Append(aCell);
                    }
                    else
                    {
                        cout << "_#_McCadExDllTripoli_Tripoli :: Cell building failed on Material Cell!!" << endl;
                        continue;
                    }
                    cout << "**************************************   Done cell: " << its << endl;
                }
                else // Void Cells
                {
                    Handle(McCadTDS_HSequenceOfExtSolid) extSolSeq = new McCadTDS_HSequenceOfExtSolid;

                    TopoDS_Solid sol2ext;
                    try
                    {
                        sol2ext = TopoDS::Solid(currentEntity.GetSolid());
                    }
                    catch(...)
                    {
                        cout << "_#_McCadExDllTripoli_Tripoli.cxx :: FAILED\n";
                    }

                    //in TRIPOLI we don't need to define the outer space

                    NbVoid++;
                    cout << "**************************************\n";
                    cout << "  Building Void Cell ..........   " << its << endl;

                    McCadCSGTool_Extender Mext(sol2ext);
                    if(Mext.IsDone())
                    {
                        Handle(McCadTDS_ExtSolid) mcSol = Mext.ExtSolid();
                        mcSol->SetComment(TCollection_AsciiString(" McCad:  =====  Void Cells  ====="));
                        extSolSeq->Append(mcSol);
                    }
                    else
                    {
                        cout << "_#_McCadExDllTripoli_Tripoli :: Extending solid failed!!!" << endl;
                        continue;
                    }

                    ////////////////////////////////////////////////////////////////////////////
                    McCadCSGTool_CellBuilder Mcel;
                    Mcel.SetUnits(myUnits);
                    Mcel.Init(extSolSeq);

                    if(Mcel.IsDone())
                    {
                        Handle(McCadCSGGeom_Cell) aCell = Mcel.GetCell();

                    //********************************************************************

                        Handle(TColStd_HSequenceOfInteger) compSeq = currentEntity.GetOtherVSolids();
                        aCell->SetComplementCells(compSeq);
                        aCell->SetVoid(Standard_True);
                        cellSeq->Append(aCell);
                    }
                    else
                    {
                        cout << "_#_McCadExDllTripoli_Tripoli :: Cell building failed for Void Cell!!" << endl;
                        continue;
                    }
                    cout << "**************************************  Done cell: " << its << endl;
                }
                myPI->Increment();
            }

            if(myPI->UserBreak())
                return Standard_False;

            myPI->SetValue(100);
            ///////////////////////////////////////////////////////////////////////////////////////////////
            //adapt outer space
            // outer Box and Sphere print after modification

            cout << "GENERATE TRIPOLI SYNTAX \n..............................\n";
            McCadMcWrite_TripoliInputGenerator theTripoliWriter(cellSeq);
            theTripoliWriter.SetInitialSurfaceNumber(myInitSurfNb);
            theTripoliWriter.SetInitialCellNumber(myInitCellNb);
            theTripoliWriter.SurfNumbering();

        /*
         * In tripoli the overall shape has to be convex. This requirement is already fulfilled by the
         * bounding box. There is no need to define all space. Hence, the last two cell definitions which
         * are necessary for MCNP are redundant and can be deleted.
         */

            ///////////////////////////////////////////////////////////////////////////////////////////////

            theTripoliWriter.SetNbOfMatCells(NbMat);
            theTripoliWriter.SetNbOfVoidCells(NbVoid);
            theTripoliWriter.SetNbOfOuterCells(NbOuterVoids);
            theTripoliWriter.SetMDReader(myMDReader);
            theTripoliWriter.PrintAll(theFile);
            theFile.Close();

            cout << myFileName.ToCString() << " has been EXPORTED\n";
        }
        else
        {
            cout << myFileName.ToCString() << " does not exist." << endl;
            theFile.Close();
            return Standard_False;
        }
    }
    else
    {
        cout << " Select file first." << endl;
        return Standard_False;
    }
    return Standard_True;
}

Handle(TopTools_HSequenceOfShape) McCadExDllTripoli_Tripoli::Import()
{
/*	Handle(TopTools_HSequenceOfShape) theEnititySequence;
    McCadGUI_GuiFactoryPtr theGuiFactory = theGuiFactory->Instance();
    McCadGUI_BaseDialogPtr theMessage = theGuiFactory->MakeDialog();
    McCadGUI_BaseDialogPtr theError = theGuiFactory->MakeDialog();
    McCadGUI_BaseDialogPtr theProgress = theGuiFactory->MakeDialog();

    if (!myFileName.IsEmpty())
    {
        OSD_Path thePath(myFileName);
        OSD_File theFile(thePath);
        if (theFile.Exists())
        {

            //open file
            OSD_Protection aProtection;
            theFile.Open(OSD_ReadOnly, aProtection);

            theProgress->SetLabelText("Reading Tripoli file.");
            theProgress->SetCancelButtonText("Cancel Reading!");
            theProgress->SetTotalSteps(100);
            theProgress->SetProgress(0);
            theEnititySequence = new TopTools_HSequenceOfShape();

            McCadMcRead_Reader theReader;
            theReader.Parse(theFile);
            McCadMcRead_DataMapOfIntegerCell theCellMap;

            theReader.GetCells(theCellMap);
            //cout << "Map Extent = " << theCellMap.Extent() << endl;
            McCadMcRead_DataMapIteratorOfDataMapOfIntegerCell cIter;
            cIter.Initialize(theCellMap);

            for (; cIter.More();cIter.Next())//For all Cells construct Geometry...
            {
                Handle(McCadMcRead_Cell) aCell = cIter.Value();

                if(aCell->GetMaterialNumber() == 0)
                {
                    cout << "Not reconstructed void cell number:   " << aCell->GetCellNumber() << endl;
                    continue;
                }
                McCadCSGUtil_SolidBuilder aB;
                aB.Init(aCell);

                if(aB.IsDone())
                {
                    TopoDS_Shape aShape = aB.Result();
                    theEnititySequence->Append(aShape);
                }
            }
            return theEnititySequence;
        }
        else
        {
            theError->Print(TCollection_AsciiString("Can not Import the file:") + myFileName );
            theError->Print(TCollection_AsciiString("Error from the Step Reader"));
            delete theError;
            delete theMessage;
            return NULL;
        }
        theFile.Close();
    }
    else
    {
        theError->Print(myFileName + TCollection_AsciiString(" does not exist."));
        delete theMessage;
        delete theError;
        delete theProgress;
        return NULL;
    }*/
    return NULL;
}

TCollection_AsciiString McCadExDllTripoli_Tripoli::GetFilename() const
{
    return myFileName;
}

void McCadExDllTripoli_Tripoli::SetFilename(const TCollection_AsciiString& theFileName)
{
    myFileName = theFileName;
}

Standard_Integer McCadExDllTripoli_Tripoli::GetSize() const
{
    return mySize;
}

Handle(TColStd_HSequenceOfAsciiString) McCadExDllTripoli_Tripoli::GetExtensions() const
{
    return myExtensions;
}

TCollection_AsciiString McCadExDllTripoli_Tripoli::GetExtension() const
{
    return myExtensions->Value(1);
}

TCollection_AsciiString McCadExDllTripoli_Tripoli::GetFormat() const
{
    return myFormat;
}

Standard_Boolean McCadExDllTripoli_Tripoli::ExportEnabled() const
{
    return myExportEnabled;
}

Standard_Boolean McCadExDllTripoli_Tripoli::ImportEnabled() const
{
    return myImportEnabled;
}

Standard_Boolean McCadExDllTripoli_Tripoli::ImportToDocument(Handle(TDocStd_Document)& theTDoc)
{
/*	McCadGUI_GuiFactoryPtr theGuiFactory = theGuiFactory->Instance();
    McCadGUI_BaseDialogPtr theMessage = theGuiFactory->MakeDialog();
    McCadGUI_BaseDialogPtr theError = theGuiFactory->MakeDialog();
    McCadGUI_ProgressDialogPtr theProgress = new McCadGUI_ProgressDialog(QMcCad_Application::GetAppMainWin());
    //Handle(Message_ProgressIndicator)
    myPI = Handle(Message_ProgressIndicator)::DownCast(theProgress);

    Handle(XCAFDoc_ShapeTool) sTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
    Handle(TDF_Data) theData = theTDoc->GetData();

    if (!myFileName.IsEmpty())
    {
        OSD_Path thePath(myFileName);
        OSD_File theFile(thePath);
        if (theFile.Exists())
        {
            //open file
            OSD_Protection aProtection;
            theFile.Open(OSD_ReadOnly, aProtection);

            theProgress->SetLabelText("Reading Tripoli file.");
            theProgress->SetCancelButtonText("Cancel Reading!");
            theProgress->SetTotalSteps(100);
            theProgress->SetProgress(0);

            McCadMcRead_Reader theReader;
            theReader.Parse(theFile);

            McCadMcRead_DataMapOfIntegerCell theCellMap;

            theReader.GetCells(theCellMap);
            theReader.GetMDReader(myMDReader);

            McCadMcRead_DataMapIteratorOfDataMapOfIntegerCell cIter;
            Standard_Integer numShp = theCellMap.Extent();
            Standard_Integer pCnt(0);
            Standard_Real delta = 100.5/numShp;
            myPI->SetStep(delta);

            cIter.Initialize(theCellMap);

            QApplication::restoreOverrideCursor();
            TCollection_AsciiString labEntry("0:1:1:1:6");
            TDF_Label mRootLab;
            TDF_Tool::Label(theData, labEntry, mRootLab, Standard_True);

            Standard_Integer aMatNum(1);

            for (; cIter.More();cIter.Next())//For all Cells construct Geometry...
            {
                Handle(McCadMcRead_Cell) aCell = cIter.Value();

                myPI->Increment();
                if(myPI->UserBreak())
                    return Standard_False;

                if(aCell->GetMaterialNumber() == 0)
                {
                    cout << "Not reconstructed void cell number:   " << aCell->GetCellNumber() << endl;
                    continue;
                }
                if(aCell->IsPseudo()) // TODO: if a cell is pseudo it means, it is filled with a universe.
                    continue;         // build an assembly: fillNumber--+universeCell1--+UniverseCell2,...

                McCadCSGUtil_SolidBuilder aB;
                aB.Init(aCell);

                if(aB.IsDone()) // add shape and cell number as its name to the document
                {
                    TDF_Label newLab;
                    TopoDS_Shape aShape = aB.Result();
                    newLab = sTool->AddShape(aShape,0,0);
                    TCollection_ExtendedString theName;
                    if(aCell->GetPseudoNum()!=0)
                    {
                        theName = TCollection_AsciiString(aCell->GetPseudoNum());
                        //cout << "set pseudo number " << aCell->GetPseudoNum() << " for Cell " << aCell->GetCellNumber() << endl;
                    }
                    else
                        theName = TCollection_AsciiString(aCell->GetCellNumber());

                    TDataStd_Name::Set(newLab, theName);

                    //Test if material group already exists
                    TDF_ChildIterator matGrpLabs(mRootLab);
                    TDF_Label newChild;
                    Standard_Boolean found(Standard_False);
                    Handle_TDataStd_Name tstName;
                    TCollection_AsciiString aName = TCollection_AsciiString("material ") + TCollection_AsciiString(aCell->GetMaterialNumber());
                    TCollection_ExtendedString mName(aName);

                    for(;matGrpLabs.More(); matGrpLabs.Next())
                    {
                        if(matGrpLabs.Value().FindAttribute(TDataStd_Name::GetID(), tstName))
                        {
                            if(tstName->Get().IsEqual(mName))
                            {
                                Handle_TDataStd_Real tstDens;
                                if(matGrpLabs.Value().FindAttribute(TDataStd_Real::GetID(),tstDens))
                                {
                                    if(tstDens->Get()==aCell->GetDensity())
                                    {
                                        newChild = matGrpLabs.Value();
                                        found = Standard_True;
                                        break;
                                    }
                                    else // same name but different density --> new Material Group
                                        aName += TCollection_AsciiString("_") + TCollection_AsciiString(aMatNum++);
                                }
                                if(found)
                                    break;
                            }
                            if(found)
                                break;
                        }
                        if(found)
                            break;
                    }


                    if(!found)
                    {
                        newChild = mRootLab.NewChild();

                        TDataStd_Name::Set(newChild, mName);
                        TDataStd_Real::Set(newChild, aCell->GetDensity());
                        TCollection_AsciiString mCard;
                        myMDReader.GetMCard(aCell->GetMaterialNumber(), mCard);
                        TDataStd_AsciiString::Set(newChild,mCard);
                    }

                    Handle(TDataStd_TreeNode) refNode, mainNode;
                    mainNode = TDataStd_TreeNode::Set ( newChild, TDataStd_TreeNode::GetDefaultTreeID());
                    refNode  = TDataStd_TreeNode::Set ( newLab, TDataStd_TreeNode::GetDefaultTreeID());
                    refNode->Remove(); // abv: fix against bug in TreeNode::Append()
                    mainNode->Prepend(refNode);
                }
                else
                    cout << "BUILDING OF SOLID FAILED!!!!!!!\n\n";
            }
        }
        else
        {
            theError->Print(TCollection_AsciiString("Can not Import the file:") + myFileName );
            theError->Print(TCollection_AsciiString("Error from the Step Reader"));
            delete theError;
            delete theMessage;
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
*/
    return Standard_False;
}


Standard_Boolean McCadExDllTripoli_Tripoli::ExportFromDocument(const Handle(TDocStd_Document)& theTDoc)
{
    Handle(McCadDiscDs_HSequenceOfDiscSolid) theVEntitySequence = new McCadDiscDs_HSequenceOfDiscSolid;
    theVEntitySequence->Append(PrepareExport(theTDoc));

    return Export(theVEntitySequence);
}



////qiu we don't need "extern"
////qiu extern "C" {
//McCadEXPlug_ExchangePlugin* _McCadEXPlug_PluginMaker()
//{
//    return new McCadExDllTripoli_Tripoli;
//}
//
//class PluginProxy
//{
//public:
//    PluginProxy()
//    {
//        // register the maker into the MakerMap
//        McCadEXPlug_ExchangePlugin* tripoliPlugin =_McCadEXPlug_PluginMaker();
//        PluginMakerMap[".d"] = tripoliPlugin;
//        PluginMakerMap[".tri"] = tripoliPlugin;
//    }
//};
//// our one instance of the proxy
//PluginProxy TripoliPlug;
////qiu }
