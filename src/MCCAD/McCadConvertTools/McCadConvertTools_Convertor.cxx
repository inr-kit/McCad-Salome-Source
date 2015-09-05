#include <McCadConvertTools_Convertor.hxx>

#include <cstdlib>
#include <iostream>

#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GProp_GProps.hxx>
#include <Geom_Surface.hxx>
#include <OSD_Path.hxx>
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <McCadCSGAdapt_SolidAnalyser.hxx>
#include <McCadCSGGeom_HSequenceOfCell.hxx>
#include <McCadCSGTool_Decomposer.hxx>
#include <McCadCSGTool_Extender.hxx>
#include <McCadCSGTool_SurfaceChecker.hxx>
#include <McCadMessenger_Singleton.hxx>
#include <McCadTDS_ExtSolid.hxx>
#include <McCadTDS_HSequenceOfExtSolid.hxx>

using namespace std;

McCadConvertTools_Convertor::McCadConvertTools_Convertor()//std ctor
{
    Init();
}


McCadConvertTools_Convertor::McCadConvertTools_Convertor(const Handle(TopTools_HSequenceOfShape)& theHSeqOfShp)
{
    Init();
    myInputSolids = theHSeqOfShp;
}


void McCadConvertTools_Convertor::Init()
{
    myFileName = "noname";
    myIsConverted = Standard_False;

    myInputSolids = new TopTools_HSequenceOfShape;
    myConvertedSolids = new TopTools_HSequenceOfShape;

    myProgress = new McCadGUI_ProgressDialog;
}


McCadGUI_ProgressDialogPtr McCadConvertTools_Convertor::GetProgressDialog()
{
    return myProgress;
}


Standard_Boolean McCadConvertTools_Convertor::Convert()
{
    Handle(TopTools_HSequenceOfShape) origSolids = new TopTools_HSequenceOfShape;
    Handle(McCadCSGGeom_HSequenceOfCell) cellSeq = new McCadCSGGeom_HSequenceOfCell;
    McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();

    Standard_Integer totalProgressSteps = 0;

    // Print Surface information
    ///////////////////////////////
    McCadCSGTool_SurfaceChecker surfaceChecker;
    surfaceChecker.printSurfaceInfo(myInputSolids);

    // Append all original solids to origSolids
    //////////////////////////////////////////////////
    for (Standard_Integer i=1; i<= myInputSolids->Length(); i++)
    {
        if ((myInputSolids->Value(i)).ShapeType() == TopAbs_COMPSOLID || (myInputSolids->Value(i)).ShapeType() == TopAbs_COMPOUND)
        {
            TopExp_Explorer ex;
            for (ex.Init(myInputSolids->Value(i), TopAbs_SOLID); ex.More(); ex.Next())
            {
                TopoDS_Solid tmpSol = TopoDS::Solid(ex.Current());
                origSolids->Append(tmpSol);
            }
        }
        else if ((myInputSolids->Value(i)).ShapeType() == TopAbs_SOLID)
        {
            origSolids->Append(myInputSolids->Value(i));
        }
    }

    //check if model is empty
    /////////////////////////////////
    if (origSolids->Length() == 0)
    {
                msgr->Message("_#_Convertor.cxx :: The model contains no solids!!\n",
                              McCadMessenger_ErrorMsg);
        return Standard_False;
    }
    else
    {
            TCollection_AsciiString message("Number of original solids: ");
            message += origSolids->Length();
            msgr->Message(message.ToCString());
    }

    totalProgressSteps = origSolids->Length();
    myProgress->SetTotalSteps(totalProgressSteps);


    // perform sign constant decomposition if neccessary
    //////////////////////////////////////////////////////////////////////////////////////
    for (Standard_Integer i=1; i<= origSolids->Length(); i++)//Process all Solids
    {
        myProgress->SetProgress(int(double(i-1)*100./double(totalProgressSteps)));
        cout << "|" << flush;
    //	cout << "\nStart processing solid number:  " << i << endl;
    //	cout << "====================================================" << endl;
        TopoDS_Solid tmpSol0 = TopoDS::Solid(origSolids->Value(i));//current solid

        //check if solid is valid; try to fix if not
        ///////////////////////////////////////////////
        McCadCSGAdapt_SolidAnalyser solAna(tmpSol0);
        TopoDS_Solid tmpSol = solAna.FixedSolid();

        // Count Faces of current Solid after generic fix
        ////////////////////////////////////////////////////////////////////////
        int fcount = 0;
        for (TopExp_Explorer ex(tmpSol, TopAbs_FACE); ex.More(); ex.Next())
        {
            TopoDS_Face iFace = TopoDS::Face(ex.Current());
            TopAbs_Orientation orient = iFace.Orientation();
            if ((orient != TopAbs_FORWARD) && (orient != TopAbs_REVERSED ))
                                msgr->Message("_#_Convertor.cxx :: Face with unknown Orientation!!",
                                              McCadMessenger_WarningMsg);
                //cout << "_#_Convertor.cxx :: Face with unknown Orientation!!" << endl;
            fcount++;
        }
    //	cout << " Number of faces after generic fix : " << fcount << "\n" << endl;

        GProp_GProps GP;
        BRepGProp::VolumeProperties(tmpSol, GP);
        Standard_Real origVol = GP.Mass();

    //////////////////////////////////////////////////////////////////////////
    //					Perform solid decomposition							//
    //////////////////////////////////////////////////////////////////////////

        McCadCSGTool_Decomposer Mdec(tmpSol);
        Handle(TopTools_HSequenceOfShape) solSeq;

        if (Mdec.IsDone())
        {
            solSeq = Mdec.GetProcessedSolids();
            //cout << i << " Nb of Processed Solids: " << solSeq->Length() << endl;
        }
        else
        {
                        msgr->Message("_#_Convertor.cxx :: Decomposing solid failed!!\n",
                                      McCadMessenger_ErrorMsg);
            break;
        }

        // Perform volume comparison between original solid and decomposed solids
        /////////////////////////////////////////////////////////////////////////
        Standard_Real decVol(0.0);
        for(Standard_Integer i=1; i<=solSeq->Length(); i++)
        {
            TopoDS_Shape tmpShp = solSeq->Value(i);
            BRepGProp::VolumeProperties(tmpShp, GP);
            decVol += GP.Mass();
        }

        if( Abs(1 - Abs(decVol/origVol)) > 1.0e-5) // Decomposition not correct
        {
                    TCollection_AsciiString errorMessage;
                        errorMessage += "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                        errorMessage += "!  Decomposed solid failed volume test: ";
                        errorMessage += decVol;
                        errorMessage += " / ";
                        errorMessage += origVol;

            if(!myFileName.IsEqual("noname.stp"))
            {
                                errorMessage += "\n!  for file : ";
                                errorMessage += myFileName;
                ofstream volOut;
                volOut.open("failedVolumeControl.log", ios_base::app);
                volOut << myFileName.ToCString() << "  --  original Volume : " << origVol << "  --  volume after decomposition : " << decVol << endl;
                volOut.close();
            }

                        errorMessage += "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                        msgr->Message(errorMessage.ToCString());
            //return Standard_False;
        }

        myConvertedSolids->Append(solSeq);
    }

    myIsConverted = Standard_True;

    myProgress->SetProgress(100);

    return Standard_True;
}


Handle(TopTools_HSequenceOfShape) McCadConvertTools_Convertor::Convert(const Handle(TopTools_HSequenceOfShape)& theHSeqOfShp)
{
    myConvertedSolids->Clear();
    myInputSolids = theHSeqOfShp;

    if(!Convert())	//Convert myHSeqOfShp
        cout << "_#_McCadConvertTools_Convertor.cxx :: Conversion failed!!!\n\n";

    return myConvertedSolids;
}


Handle(TopTools_HSequenceOfShape) McCadConvertTools_Convertor::GetConvertedModel()
{
    if(!myIsConverted)
    {
        cout << "Model has not yet been converted! Starting conversion now ...\n";
        if(!myConvertedSolids->IsEmpty())
            Convert();
        else
            cout << "No model set. Nothing to convert...\n";
    }
    return myConvertedSolids;
}


Standard_Boolean McCadConvertTools_Convertor::IsConverted() const
{
    return myIsConverted;
}

