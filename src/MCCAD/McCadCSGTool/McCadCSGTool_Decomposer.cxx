#include <McCadCSGTool_Decomposer.ixx>
#include <McCadCSGTool.hxx>
#include <McCadCSGTool_TrivialCheck.hxx>
#include <McCadCSGTool_SignCheck.hxx>
#include <McCadCSGTool_Cutter.hxx>

#include <TColgp_HSequenceOfPnt.hxx>

#include <TopoDS_Solid.hxx>
#include <TopoDS.hxx>

//qiu #include <IntTools_Context.hxx>

#include <BRepCheck.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <TCollection_AsciiString.hxx>
#include <McCadTDS_ExtFace.hxx>

#include <McCadMessenger_Singleton.hxx>
#include <McCadCSGAdapt_SolidAnalyser.hxx>

#include <BRepTools.hxx>

McCadCSGTool_Decomposer::McCadCSGTool_Decomposer()
{
    myIsDone = false;
}

McCadCSGTool_Decomposer::McCadCSGTool_Decomposer(const TopoDS_Solid& theSolid)
{
    myIsDone = false;
    Init(theSolid);
}

void McCadCSGTool_Decomposer::Init(const TopoDS_Solid& theSolid) {
    //local init
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    NotDone();
    TopoDS_Solid aSolid = theSolid;
    myProcessedSolids = new TopTools_HSequenceOfShape();
    myUnProcessedSolids = new TopTools_HSequenceOfShape();
    myUnProcessedSolids->Append(aSolid);//Append unprocessed Solid to Sequence of unprocessed Solids

    //perform cut for all unprocessed solids
    for (Standard_Integer i = 1; i <= myUnProcessedSolids->Length(); i++) {
        TopoDS_Solid tmpSol = TopoDS::Solid(myUnProcessedSolids->Value(i));

        // test for topological correctness
        BRepCheck_Analyzer BA5(tmpSol, Standard_True);
        if(!BA5.IsValid()) {
            McCadCSGAdapt_SolidAnalyser solAna(tmpSol);
            tmpSol = solAna.FixedSolid();
        }

        //check if solid is trivial (box, sphere,...)
        McCadCSGTool_TrivialCheck trivial(tmpSol);
        if (trivial.IsDone() && trivial.Elementary()) {
            myProcessedSolids->Append(tmpSol);
        }
        else {
            // test solid's surfaces for sign-constancy and cut if sign-changing surface is found
            McCadCSGTool_SignCheck check(tmpSol);
            if (check.IsDone()) {
                if (check.SignStateChanges()) {
                    // if cutting fails due to cas BOP we try with the next extFace!
                    Handle(McCadTDS_ExtFace) tmpExtFace;
                    while (check.NbExtFace() > 0) {
                        cout << "." << flush;
                        tmpExtFace = check.GetExtFace();

                        // perform cut
                        McCadCSGTool_Cutter cut;
                        if (check.NbExtFace() < 1)
                            cut.SetLastCut(Standard_True);
                        cut.Init(tmpSol, tmpExtFace);

                        if (cut.IsDone()) {
                            Handle(TopTools_HSequenceOfShape) tmpSolSeq = cut.Solids();
                            if (tmpSolSeq->Length() >=2) {
                                myUnProcessedSolids->Append(tmpSolSeq);
                                break;
                            }
                            else if (cut.IsLastCut() && tmpSolSeq->Length() <= 1)  {
                                // at the last cut we accept also 1 solid with warning.
                                msgr->Message("_#_McCadCSGTool_Decomposer.cxx :: Warning: Cutted solid may have degenerated result!!",
                                              McCadMessenger_WarningMsg);
                                myUnProcessedSolids->Append(tmpSolSeq);
                                break;
                            }
                        }
                        else if (check.NbExtFace() == 0) {
                            msgr->Message("_#_McCadCSGTool_Decomposer.cxx :: Cutting solid failed certainly !!\n",
                                          McCadMessenger_ErrorMsg);
                            break;
                        }
                        else {
                            msgr->Message("_#_McCadCSGTool_Decomposer.cxx :: Cutting solid failed !!",
                                          McCadMessenger_ErrorMsg);
                            msgr->Message("                                Trying to cut with next face ...... ",
                                          McCadMessenger_ErrorMsg);
                        }
                    }
                }
                else
                    myProcessedSolids->Append(tmpSol);
            }
            else
                msgr->Message("_#_McCadCSGTool_Decomposer.cxx :: Sign check failed ",
                              McCadMessenger_ErrorMsg);
        }
    }
    Done();
}


Standard_Boolean McCadCSGTool_Decomposer::IsDone() const
{
	return myIsDone;
}


void McCadCSGTool_Decomposer::Done()
{
	myIsDone = Standard_True;
}


void McCadCSGTool_Decomposer::NotDone()
{
	myIsDone = Standard_False;
}


Handle(TopTools_HSequenceOfShape) McCadCSGTool_Decomposer::GetProcessedSolids() const
{
	return myProcessedSolids;
}


Standard_Integer McCadCSGTool_Decomposer::NbProcessedSolids() const
{
	return myProcessedSolids->Length();
}

