#include <map>
#include <string>

#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <OSD_Directory.hxx>



#include "../McCadMcVoid/McCadVoidCellManager.hxx"
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/MaterialManager.hxx"
#include "../McCadDecompose/McCadDecompose.hxx"
#include "../McCadTool/McCadGeneTool.hxx"

#include <time.h>

#define CLOCKS_PER_SEC 1000001

using namespace std;


// MAIN
//////////////////////
int main(int argc, char *argv[])
{


    // handle input
    TCollection_AsciiString inName;
    if(argc > 1)
    {
        TCollection_AsciiString inParameter;
        inParameter = argv[1];
        if(argc > 2)
            inName = argv[2];
        TCollection_AsciiString outputName, errorOutputName;
        TCollection_AsciiString prmtName;
        if (argc > 3)
            prmtName = argv[3];
        if(inParameter.IsEqual("-h") || inParameter.IsEqual("--help"))
        {
            exit(0);
        }


        // Convertor
         if(inParameter.IsEqual("-d") || inParameter.IsEqual("--decompose"))
        {
            Standard_Integer iCombine;
            if(prmtName.IsEqual("-1"))
            {
                iCombine = 1;
            }
            else if(prmtName.IsEqual("-0"))
            {
                iCombine = 0;
            }
            else if(prmtName.IsEqual("-2"))
            {
                iCombine = 2;
            }
            else
            {
                cout<<"#Error! Unrecognized character of the third parameter! It should be -0,-1,-2"<<endl;
                cout<<"# -0 means the decomposed solids will be saved as independent solids"<<endl;
                cout<<"# -1 means the the decomposed solids which belong to one original solid will be combined "<<endl;
                cout<<"# -2 means all the decomposed solids will be combined as one solid"<<endl;
                return 0;
            }

            Handle(TopTools_HSequenceOfShape) hInputShape = McCadGeneTool::readFile(inName);


            if (hInputShape == NULL)
            {
                cout << "#Main Function: Read geometry data error! Please check the input file name!\n\n";
                return 0;
            }

            clock_t start, end;
            double runtime;
            start = clock();

            McCadDecompose * pDecompose = new McCadDecompose();

            pDecompose->InputData(hInputShape,inName);
            pDecompose->SetCombine(iCombine);
//            pDecompose->Decompose();
            pDecompose->Perform();

//            Handle_TopTools_HSequenceOfShape OutputSolids = new TopTools_HSequenceOfShape();
//            Handle_TopTools_HSequenceOfShape ErrorSolids = new TopTools_HSequenceOfShape();

//            Handle(TopTools_HSequenceOfShape) OutputSolids, ErrorSolids;
//            OutputSolids= pDecompose->getOutCompSolids();
//            ErrorSolids = pDecompose->getErrCompSolids();

//            cout<< "Output solids" <<OutputSolids->Length() << ", error solids"<<ErrorSolids->Length() <<endl;

//            delete pDecompose;
//            pDecompose = NULL;

            end = clock();
            runtime = (double)(end-start)/CLOCKS_PER_SEC;
            cout<<"run  "<<runtime<<" sec"<<endl;

//            TCollection_AsciiString fileExtension;
//            TCollection_AsciiString tempName = inName;
//            fileExtension = tempName.Split(tempName.SearchFromEnd(".")-1);

//            TCollection_AsciiString outputName = tempName + "_out" + fileExtension;
//            TCollection_AsciiString errouttName = tempName + "_err" + fileExtension;

//  //          OutputSolids->Append(ErrorSolids);
//            McCadGeneTool::WriteFile(outputName, OutputSolids);
            if (pDecompose->getErrCompSolids()->IsEmpty()) {
                delete pDecompose;
                exit(0);
            }
            else {
//                McCadGeneTool::WriteFile(errouttName, ErrorSolids);
                delete pDecompose;
                cout<<"Decomposition failed, sorry..."<<endl;
                exit(2);
            }

        }
         else {
             cout<<"ERROR# False input parameters!"<<endl;
             cout<<"Usage: McCadDecomposer -d yourstepfile.stp -0/1/2"<<endl;
             exit(1);
         }
    }
    else {
        cout<<"ERROR# False input parameters!"<<endl;
        cout<<"Usage: McCadDecomposer -d yourstepfile.stp -0/1/2"<<endl;
        exit(1);
    }
}
