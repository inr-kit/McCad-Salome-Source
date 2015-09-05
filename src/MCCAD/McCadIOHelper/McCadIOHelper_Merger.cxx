#include <McCadIOHelper_Merger.hxx>

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include <McCadEXPlug.hxx>
#include <McCadEXPlug_PluginMaker.hxx>
#include <McCadEXPlug_PluginManager.hxx>
#include <McCadExDllStep_Step.hxx>

#include <BRep_Builder.hxx>
#include <OSD_Path.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>


using namespace std;

McCadIOHelper_Merger::McCadIOHelper_Merger()
{
        myPath = "./";
        Init();
}


McCadIOHelper_Merger::McCadIOHelper_Merger(const TCollection_AsciiString& thePath)
{
        myPath = thePath;
        Init();
        AddDirectory(thePath);
}


void McCadIOHelper_Merger::Init()
{
        myFileName  = "all.stp";
        myOutFilter = ".stp";
        myInFilter = "none";
        myIsFirstShape = Standard_True;

        McCadEXPlug_PluginManagerPtr thePluginManager =	McCadEXPlug_PluginManager::Instance();
        myPluginManager = thePluginManager;
}


void McCadIOHelper_Merger::SetInFilter(const TCollection_AsciiString& theInFilter)
{
        myInFilter = theInFilter;
}


void McCadIOHelper_Merger::SetOutFilter(const TCollection_AsciiString& theOutFilter)
{
        myOutFilter = theOutFilter;
}


Standard_Boolean McCadIOHelper_Merger::AddFile(const TCollection_AsciiString& anotherFile)
{
        Handle(TopTools_HSequenceOfShape) tmpShpSeq = new TopTools_HSequenceOfShape;
        TCollection_AsciiString aFileFilter = anotherFile;
        aFileFilter.Remove(1,aFileFilter.SearchFromEnd(".")-1);
        myOutFilter = aFileFilter;

        if(myPluginManager->CheckExtension(aFileFilter))
        {
                myImpoExpo = myPluginManager->GetPlugin(aFileFilter);
                myImpoExpo->SetFilename(anotherFile);
                tmpShpSeq = myImpoExpo->Import();
                if(myIsFirstShape)
                {
                        myHSeqOfShp = tmpShpSeq;
                        myIsFirstShape = Standard_False;
                }
                else
                        myHSeqOfShp->Append(tmpShpSeq);
                cout << anotherFile.ToCString() << " has been appended.\n";
        }
        else
        {
                cout << "_#_ No file filter for " << aFileFilter.ToCString() << endl;
                return Standard_False;
        }

        return Standard_True;
}


Standard_Boolean McCadIOHelper_Merger::AddDirectory(const TCollection_AsciiString& thePath)
{
        TCollection_AsciiString theCommandString;
        Standard_Boolean FilesFound = Standard_False;
        myPath = thePath;
        if(myInFilter.IsEqual("none")) // if no file filter specified -> append all known file formats
        {
                cout << "No file filter specified: looking for all known file formats in directory: " << myPath << endl;
                theCommandString = TCollection_AsciiString("ls -1 ") + myPath + TCollection_AsciiString("/*");
        }
        else // append all files with specified file format
        {
                theCommandString = TCollection_AsciiString("ls -1 ") + myPath + TCollection_AsciiString("/*") + myInFilter;
        }

        static unsigned int BUF_SIZE = 1024;
        myPath = thePath;

        char command_string[80];
        strcpy(command_string, string(theCommandString.ToCString()).c_str());
        FILE *dl;
//qiu        char in_buf[BUF_SIZE];
		char in_buf[1024];
//qiu   
#ifdef WIN32
		dl = _popen(command_string, "r"); //get list of files in given directory
#else
		dl = popen(command_string, "r"); //get list of files in given directory
#endif
        if (!dl)
        {
                perror("popen");
                exit(-1);
        }

        while (fgets(in_buf, BUF_SIZE, dl)) //for all found files
        {
                TCollection_AsciiString tmpName;
                TCollection_AsciiString tmpExtension;

                char *w;	//handling line breaks
                w = strpbrk(in_buf, " \t\n");
                if (w)
                        *w='\0';

                if(myInFilter.IsEqual("none")) // if no file filter specified, extract extension and check for validity
                {
                        TCollection_AsciiString tmp(in_buf);
                        tmpName = tmp;
                        tmpExtension = tmpName;
                        tmpExtension.Remove(1,tmpExtension.SearchFromEnd(".")-1);
                }
                else //use specified file filter
                {
                        TCollection_AsciiString tmp(in_buf);
                        tmpName = tmp;
                        tmpExtension = myInFilter;
                }

                if(!myPluginManager->CheckExtension(tmpExtension))
                {
                        cout << "unknown extension: " << tmpExtension.ToCString() << " skipping file "<< tmpName.ToCString() << endl;
                        continue;
                }

                //Append File
                if(AddFile(tmpName))	//if but just a single file has been found return success
                        FilesFound = Standard_True;
        }

        if(FilesFound)
                return Standard_True;
        else
                return Standard_False;
}


Standard_Boolean McCadIOHelper_Merger::MergeToFile(const TCollection_AsciiString& theOutFile)
{
        myFileName = theOutFile;
        return Merge();
}


Handle(TopTools_HSequenceOfShape) McCadIOHelper_Merger::GetHSeqOfShp()
{
        return myHSeqOfShp;
}


Standard_Boolean McCadIOHelper_Merger::Merge() // build output file
{
        TopoDS_CompSolid theCSolid;
        BRep_Builder theBuilder;
        theBuilder.MakeCompSolid(theCSolid);
        TopExp_Explorer TopExp;
        TCollection_AsciiString outName = myPath + TCollection_AsciiString("/") + myFileName;

        cout << "\nMerging to output file : " << myFileName.ToCString() << endl;
        cout << "Number of shapes: " << myHSeqOfShp->Length() << endl;

        for (Standard_Integer i=1; i<= myHSeqOfShp->Length(); i++)
        {
                for (TopExp.Init(myHSeqOfShp->Value(i), TopAbs_SOLID); TopExp.More(); TopExp.Next())
                {
                        TopoDS_Solid tmpSol = TopoDS::Solid(TopExp.Current());
                        theBuilder.Add(theCSolid, tmpSol);
                }
        }

        Handle(TopTools_HSequenceOfShape) tmpShpSeq = new TopTools_HSequenceOfShape();

        for (TopExp.Init(theCSolid, TopAbs_SOLID); TopExp.More(); TopExp.Next())
        {
                TopoDS_Solid tmpSol = TopoDS::Solid(TopExp.Current());
                tmpShpSeq->Append(tmpSol);
        }

        if (myPluginManager->CheckExtension(myOutFilter))
        {
                myImpoExpo = myPluginManager->GetPlugin(myOutFilter);
                myImpoExpo->SetFilename(outName);
                myImpoExpo->Export(tmpShpSeq);
        }
        else
                return Standard_False;

        cout << "...done\n\n";
        return Standard_True;
}
