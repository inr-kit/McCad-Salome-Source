#include "McCadCSGTool_SurfaceChecker.hxx"

#include <iostream>
#include <ostream>
#include <fstream>
#include <map>

#include <BRep_Tool.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepLib.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GProp_GProps.hxx>
#include <Standard.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#ifndef SALOME
#include <McCadMessenger_Singleton.hxx>
#endif
using namespace std;

McCadCSGTool_SurfaceChecker::McCadCSGTool_SurfaceChecker() {}

McCadCSGTool_SurfaceChecker::~McCadCSGTool_SurfaceChecker() {}

bool McCadCSGTool_SurfaceChecker::printSurfaceInfo(Handle_TopTools_HSequenceOfShape & shapes,
                                                   TCollection_AsciiString inName,
                                                   TCollection_AsciiString logFileName)
{
#ifndef SALOME
    McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
#endif
    bool freeFromInvalidSurfaces(true);
    ofstream output;

    if(logFileName.IsEmpty())
    {
        cout << "\n\n\n**************************************************\n\nFILE: " << inName.ToCString() << "\n" << endl;
        cout << "number of solids : " << shapes->Length() << endl;
    }
    else
    {
        output.open(logFileName.ToCString(), ios_base::app);
        output << "\n\n\n**************************************************\n\nFILE: " << inName.ToCString() << "\n" << endl;
        output << "number of solids : " << shapes->Length() << endl;
    }


    Handle_TopTools_HSequenceOfShape invalidSurfaceSeq = new TopTools_HSequenceOfShape;

    // check surface types!!!
    for(int i=1; i<=shapes->Length(); i++)
    {
        TopoDS_Shape tmpShp = shapes->Value(i);
        TopExp_Explorer ex(tmpShp, TopAbs_FACE);

        Standard_Integer plnCnt(0), cylCnt(0), conCnt(0), sphCnt(0), torCnt(0), bzrCnt(0), splnCnt(0), extrCnt(0), revCnt(0), offsetCnt(0), otherCnt(0);
        Standard_Integer allCnt(0);

        for(; ex.More(); ex.Next())
        {
            allCnt++;
            TopoDS_Face curF = TopoDS::Face(ex.Current());
            TopLoc_Location aLoc;
            Handle(Geom_Surface) aSurf = BRep_Tool::Surface(curF, aLoc);
            GeomAdaptor_Surface adSurf(aSurf);
            switch(adSurf.GetType())
            {
            case GeomAbs_Plane:
                plnCnt++;
                break;
            case GeomAbs_Cylinder:
                cylCnt++;
                break;
            case GeomAbs_Cone:
                conCnt++;
                break;
            case GeomAbs_Sphere:
                sphCnt++;
                break;
            case GeomAbs_Torus:
            { // TODO : check for right orientation || Tori must be axis parallel
                torCnt++;
                break;
            }
            case GeomAbs_BezierSurface:
                invalidSurfaceSeq->Append(curF);
                bzrCnt++;
                break;
            case GeomAbs_BSplineSurface:
                invalidSurfaceSeq->Append(curF);
                splnCnt++;
                break;
            case GeomAbs_SurfaceOfRevolution:
                invalidSurfaceSeq->Append(curF);
                revCnt++;
                break;
            case GeomAbs_SurfaceOfExtrusion:
                invalidSurfaceSeq->Append(curF);
                extrCnt++;
                break;
            case GeomAbs_OffsetSurface:
                invalidSurfaceSeq->Append(curF);
                offsetCnt++;
                break;
            default:
                invalidSurfaceSeq->Append(curF);
                otherCnt++;
                break;
            }
        }

        TCollection_AsciiString message("\nSurface Information for Solid : ");
        message.AssignCat(i);
        message.AssignCat("\n========================================\n");
        message.AssignCat("Number Of Surfaces In Model : ");
        message.AssignCat(allCnt);
        message.AssignCat("\n----------------------------------------");
        message.AssignCat("\nPlanes                  : ");
        message.AssignCat(plnCnt);
        message.AssignCat("\nCylinder                : ");
        message.AssignCat(cylCnt);
        message.AssignCat("\nCones                   : ");
        message.AssignCat(conCnt);
        message.AssignCat("\nSpheres                 : ");
        message.AssignCat(sphCnt);
        message.AssignCat("\nToroidal Surfaces       : ");
        message.AssignCat(torCnt);

        if(bzrCnt > 0 || splnCnt > 0 || revCnt > 0 || extrCnt > 0 || offsetCnt > 0 || otherCnt > 0)
        {
            message.AssignCat("\n\n--- invalid surface types ---");
            message.AssignCat("\nBezier Surfaces         : ");
            message.AssignCat(bzrCnt);
            message.AssignCat("\nBSpline Surfaces        : ");
            message.AssignCat(splnCnt);
            message.AssignCat("\nSurfaces Of Revolution  : ");
            message.AssignCat(revCnt);
            message.AssignCat("\nSurfaces Of Extrusion   : ");
            message.AssignCat(extrCnt);
            message.AssignCat("\nOffset Surfaces         : ");
            message.AssignCat(offsetCnt);
            message.AssignCat("\nUnknown Type Of Surface : ");
            message.AssignCat(otherCnt);
            freeFromInvalidSurfaces = false;
        }
        message.AssignCat("\n========================================\n\n");
#ifndef SALOME
        msgr->Message( message.ToCString() );
//#else
//        cout <<message.ToCString() <<endl;

#endif
        if(!logFileName.IsEmpty())
        {
            output << "\nSurface Information for Solid " << i << endl;
            output << "========================================\n";
            output << "Number Of Surfaces In Model : " << allCnt << endl;
            output << "----------------------------------------"<< endl;
            output << "Planes                  : " << plnCnt << endl;
            output << "Cylinder                : " << cylCnt << endl;
            output << "Cones                   : " << conCnt << endl;
            output << "Spheres                 : " << sphCnt << endl;
            output << "Toroidal Surfaces       : " << torCnt << endl;
            if(bzrCnt > 0 || splnCnt > 0 || revCnt > 0 || extrCnt > 0 || offsetCnt > 0 || otherCnt > 0)
            {
                output << "\n--- invalid surface types ---" << endl;
                output << "Bezier Surfaces         : " << bzrCnt << endl;
                output << "BSpline Surfaces        : " << splnCnt << endl;
                output << "Surfaces Of Revolution  : " << revCnt << endl;
                output << "Surfaces Of Extrusion   : " << extrCnt << endl;
                output << "Offset Surfaces         : " << offsetCnt << endl;
                output << "Unknown Type Of Surface : " << otherCnt << endl;
            }
            output << "========================================\n\n";
        }
    }

    if(invalidSurfaceSeq->Length() > 0){
        STEPControl_Writer wrt;
        for(int i=1; i<=invalidSurfaceSeq->Length(); i++)
            wrt.Transfer(invalidSurfaceSeq->Value(i), STEPControl_AsIs);
        wrt.Write("invalidSurfaces.stp");
    }

    output.close();

    return freeFromInvalidSurfaces;
}

/* qiu no use
bool McCadCSGTool_SurfaceChecker::readFile(TCollection_AsciiString inName, TCollection_AsciiString logFileName)
{
    STEPControl_Reader stpRdr;
    stpRdr.ReadFile(inName.ToCString());
    stpRdr.TransferRoots();
    Handle(TopTools_HSequenceOfShape) shpSeq = new TopTools_HSequenceOfShape;

    for(int	 i=1; i<=stpRdr.NbShapes(); i++) // transfer shapes
        shpSeq->Append(stpRdr.Shape(i));

    if(logFileName.IsEmpty())
        logFileName = "surfaceCheck.log";

    return printSurfaceInfo( shpSeq, inName, logFileName );
}


void McCadCSGTool_SurfaceChecker::readDirectory(TCollection_AsciiString inp, TCollection_AsciiString logFileName) {

    if(!logFileName.IsEmpty())
    {
        ofstream output;
        output.open(logFileName.ToCString(), ios_base::out);
        output.close();			// clear output-file
    }

    cout << inp.ToCString() << "  ------------------- \n\n";

    if(inp.IsEqual("."))
        inp = "./";

    if(inp.SearchFromEnd("/") == inp.Length())
    {
        TCollection_AsciiString command;
        command = TCollection_AsciiString("ls -1 ") + inp + TCollection_AsciiString("*.stp");
        char command_string[77];
        //static unsigned int size = 1024;
        strcpy(command_string, string(command.ToCString()).c_str());
        FILE *dl;
        int BUF_SIZE = 1024;
//qiu         char in_buf[BUF_SIZE];
		char in_buf[1024];
//qiu   
#ifdef WIN32
		dl = _popen(command_string, "r");
#else 
		dl = popen(command_string, "r");
#endif
        int control = 0;

        while (fgets(in_buf, BUF_SIZE, dl)) {
            TCollection_AsciiString tmp(in_buf);

            TCollection_AsciiString NAME;
            char* env = getenv("PWD");
            TCollection_AsciiString ENV(env);

            NAME = ENV + TCollection_AsciiString("/") + tmp;

            NAME = NAME.Token("\n", 1);			// remove "newline" before aString

            readFile(NAME, logFileName);

            control++;
        }

        if (control == 0) {
            remove(logFileName.ToCString());
            cout << "The respective directory is empty!" << endl;
        }
    }
    else if(inp.Length()-inp.SearchFromEnd(".stp") == 3)
        readFile(inp, logFileName);
    else
     //   cout << "Invalid input. Input must either be a '.stp' - file or a '' - directory!" << endl;
}
*/


