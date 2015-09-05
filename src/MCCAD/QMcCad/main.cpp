#include <main.h>
#include <map>
#include <string>

#include <QtGui/QApplication>
#include <QtGui/QSplashScreen>
#include <QtCore/QFile>
#include <QtCore/QString>

#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <OSD_Directory.hxx>

#include <McCadConvertTools_Convertor.hxx>
#include <McCadConvertTools_VoidGenerator.hxx>
#include <McCadIOHelper_Expander.hxx>
#include <McCadIOHelper_Merger.hxx>
#include <McCadCSGTool_SurfaceChecker.hxx>
#include <McCadEXPlug_PluginMaker.hxx>
#include <McCadEXPlug_ExchangePlugin.hxx>
#include <McCadEXPlug_PluginManager.hxx>
#include <McCadIOHelper_InputFileParser.hxx>
#include <McCadMessenger_StdOut.hxx>

#include <McCadPrintUsage.h>
#include <QMcCad_Application.h>

#include "../McCadMcVoid/McCadVoidCellManager.hxx"
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadIOTools/McCadInputModelData.hxx"
#include "../McCadTool/MaterialManager.hxx"

using namespace std;

map<string, McCadEXPlug_PluginMaker , less<string> > PluginMakerMap;

// print Version number
///////////////////////////
void printVersion() {
    cout << "\n=============================================\n" <<
            "		" << "McCad" <<  " " << MCCAD_VERSION_MAJOR << "." <<
                MCCAD_VERSION_MINOR << "." <<
                MCCAD_VERSION_PATCH << endl <<
            "=============================================\n\n" << endl;
}

// read file, return geometry
///////////////////////////////
Handle_TopTools_HSequenceOfShape readFile( TCollection_AsciiString fileName){
    // get reader plugin
    TCollection_AsciiString fileExtension = fileName;
    fileExtension = fileExtension.Split(fileExtension.SearchFromEnd(".")-1);
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();
    McCadEXPlug_ExchangePlugin* readerPlugin = pluginManager->GetPlugin(fileExtension);
    // read file
    readerPlugin->SetFilename(fileName);
    Handle_TopTools_HSequenceOfShape shapes = new TopTools_HSequenceOfShape;
    shapes = readerPlugin->Import();
    return shapes;
}

// write geometry
//////////////////
void writeFile( TCollection_AsciiString outFileName, Handle_TopTools_HSequenceOfShape shapes){
    //get writer plugin
    TCollection_AsciiString fileExtension = outFileName;
    fileExtension = fileExtension.Split(fileExtension.SearchFromEnd(".")-1);
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();
    McCadEXPlug_ExchangePlugin* writerPlugin = pluginManager->GetPlugin(fileExtension);
    // write file
    writerPlugin->SetFilename(outFileName);
    writerPlugin->Export(shapes);
}

// MAIN
//////////////////////
int main(int argc, char *argv[]) {
    // define messenger system
    McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
    McCadMessenger_ObserverInterface* stdCoutObserver = new McCadMessenger_StdOut;
    msgr->Register(stdCoutObserver);

    // define plugin path
    char* pLibPath = getenv("MCCAD_LIBDIR");
    OSD_Path pluginPath("empty");
    if (pLibPath == NULL) {
        cout << "MCCAD_LIBDIR is not defined!!! " << endl;
        return 1;
    }
    else
        pluginPath = OSD_Path(pLibPath);

    // load plugins
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();
    pluginManager->LoadSharedLibrary(pluginPath);

    // handle input parameters
    TCollection_AsciiString Prog = argv[0];
    if(argc > 4 ) {
        string arg3(argv[3]), arg4(argv[4]);
        if(arg3 == ">" || arg3 == ">>") {
            argc = 3;
        }
        else if(arg4 == ">" || arg4 == ">>") {
            argc = 4;
        }
        else {
            printUsage(Prog);
            exit(0);
        }
    }
    bool openFile = false;

    // handle input
    TCollection_AsciiString inName;
 if(argc > 1)

//if (1)
   {
        TCollection_AsciiString inParameter;
        inParameter = argv[1];
        if(argc > 2)
            inName = argv[2];
        TCollection_AsciiString outputName;        
        TCollection_AsciiString strMatName = "";
        if (argc > 3)
            strMatName = argv[3];
        if(inParameter.IsEqual("-h") || inParameter.IsEqual("--help")) {
            printUsage();
            exit(0);
        }

        /* test code */
        //inParameter = "-m";
        TCollection_AsciiString config_file = "McCadConfig.txt";
        //inName = "convertedTest.stp";
        //strMatName = "material.xml";
        /* test code */

        // Expander
        if(inParameter.IsEqual("-e") || inParameter.IsEqual("--explode")) {
            cout << "\nMcCad_Exploder\n====================\n\n";
            Handle_TopTools_HSequenceOfShape inputShapes = readFile( inName );
            McCadIOHelper_Expander expander(inputShapes);
            Handle_TopTools_HSequenceOfShape expandedFiles = expander.GetExplodedShapes();
            if(expandedFiles->Length() < 2){
                TCollection_AsciiString message("Failed to expand file : ");
                message.AssignCat(inName);
                msgr->Message(message.ToCString());
                exit(0);
            }
            TCollection_AsciiString tmpName(inName);
            TCollection_AsciiString fileFilter = tmpName.Split( tmpName.SearchFromEnd(".") -1 );
            for(int i=1; i<=expandedFiles->Length(); i++){
                TCollection_AsciiString outName(tmpName);
                outName.Prepend("ExOut");
                outName.AssignCat("_");
                outName.AssignCat(i);
                outName.AssignCat(fileFilter);
                Handle_TopTools_HSequenceOfShape singleShape = new TopTools_HSequenceOfShape;
                singleShape->Append(expandedFiles->Value(i));
                writeFile(outName, singleShape);
            }
        }

        // Convertor
        else if(inParameter.IsEqual("-d") || inParameter.IsEqual("--decompose")) {
            cout << "\nMcCad_Decomposer\n====================\n\n";
            // read file
            Handle_TopTools_HSequenceOfShape inputShapes = readFile( inName );

            // decompose geometry
            McCadConvertTools_Convertor convertor(inputShapes);
            convertor.Convert();
            if (argc == 4 && !outputName.IsEmpty())
                convertor.SetFileName(outputName);

            if(!convertor.IsConverted()){
                cout << "Conversion failed!!!\n";
                return -1;
            }

            // export decomposed geometry to stp file
            TCollection_AsciiString exportName("converted");
            exportName += inName;
            inName = exportName.Split(exportName.SearchFromEnd("."));
            exportName += "stp";
            writeFile( exportName, convertor.GetConvertedModel());
            }

            // export decomposed geometry to stp file
        // Lei Lu 20150501
//            TCollection_AsciiString exportName("converted");
//            exportName += inName;
//            inName = exportName.Split(exportName.SearchFromEnd("."));
//            exportName += "stp";
//            writeFile( exportName, convertor.GetConvertedModel());
      //  }

        // Fuse
        else if(inParameter.IsEqual("-f") || inParameter.IsEqual("--fuse")) {
            cout << "\nMcCad_Fusioner\n====================\n\n";
            McCadIOHelper_Merger myMerger(inName);
            if(argc == 4 && !outputName.IsEmpty())
                myMerger.MergeToFile(outputName);
            else
                myMerger.Merge();
        }

        // Surface check
        else if(inParameter.IsEqual("-s") || inParameter.IsEqual("--surface-check")) {
            cout << "\n McCad_SurfaceChecker\n====================\n\n";
            if(argc < 3 || (argc == 4 && outputName.IsEmpty()) || argc > 4) {
                cout << "Output file name required\n\n";
                exit(-1);
            }
            McCadCSGTool_SurfaceChecker surfCheck;
            surfCheck.readDirectory(inName, outputName);
        }

        // Void Generator
        else if(inParameter.IsEqual("-m") || inParameter.IsEqual("--mcnp") ||
                inParameter.IsEqual("-t") || inParameter.IsEqual("--tripoli")||
                inParameter.IsEqual("-g") || inParameter.IsEqual("--gdml"))
        {
            // read parameter file
            if (!McCadConvertConfig::ReadPrmt(config_file))
            {
                return 0;
            }

            McCadInputModelData input_model;           
            if (!input_model.LoadSTEPModel(inName))
            {
                cout << "#Main Function: Read geometry data error! Please check the input file name!\n\n";
                return 0;
            }

            // New void generation algorithm
            // Modified by Lei Lu 12/08/2012 ~ 18/12/2013
            McCadVoidCellManager * pVoidCellManager = new McCadVoidCellManager();



            Handle(TopTools_HSequenceOfShape) hInputShape = input_model.GetModelData();
            pVoidCellManager->ReadGeomData( hInputShape );
            pVoidCellManager->ReadMatData(strMatName);
            TCollection_AsciiString outName = inName;       // Set the output file name
            outName.Split(outName.SearchFromEnd(".")-1);    // Remove the file extension.

            if(inParameter.IsEqual("-t") || inParameter.IsEqual("--tripoli"))
            {
                pVoidCellManager->SetConvetor("TRIPOLI");
                outName += "_TRIPOLI.txt";
            }
            else if(inParameter.IsEqual("-m") || inParameter.IsEqual("--mcnp"))
            {
                pVoidCellManager->SetConvetor("MCNP");
                outName += "_MCNP.txt";
            }
            //qiu add to generate GDML input
            else if (inParameter.IsEqual("-g") || inParameter.IsEqual("--gdml"))
            {
                pVoidCellManager->SetConvetor("GDML");
                outName += ".gdml";
            }


            pVoidCellManager->SetOutFileName(outName);
            pVoidCellManager->Process();                    // Process the conversion work

            delete pVoidCellManager;
            pVoidCellManager = NULL;
        }

        // open file for GUI
        else if(inParameter.Search(".stp")  > 0 || inParameter.Search(".mcn")  > 0 ||
                inParameter.Search(".d")    > 0 || inParameter.Search(".step") > 0 ||
                inParameter.Search(".mcnp") > 0 || inParameter.Search(".tri")  > 0 ||
                inParameter.IsEqual(".") )
        {
            openFile = true;
            inName = inParameter;
        }

        // print version
        else if(inParameter.IsEqual("-v") || inParameter.IsEqual("--version"))
            printVersion();
        else {
            cout << "\nUnknown Parameter ... " << argv[1] << endl;
            printUsage(Prog);
        }

        if(!openFile)
            return 0;
    }

    // GUI
    //////////////////////////////////////
    QApplication app(argc, argv);
    QSplashScreen* splash = new QSplashScreen(QPixmap(":images/splash.png"), Qt::WindowStaysOnTopHint);
    splash->show();
    splash->showMessage(app.tr("Loading libraries ....."), Qt::AlignRight | Qt::AlignBottom);
    QMcCad_Application QMcWin;

    // loading plugins
    QMcWin.SetPluginPath(pluginPath);
    QMcWin.LoadPlugins();
    Handle_TColStd_HSequenceOfAsciiString Formats = QMcWin.Formats();
    for (int j=1; j<= Formats->Length(); j++)
        splash->showMessage(app.tr(Formats->Value(j).ToCString()), Qt::AlignRight|Qt::AlignBottom);

    QMcWin.show();

    // delete splash
    if (splash) {
        splash->finish(&QMcWin);
        delete splash;
    }

    // open file imediately after window is loaded
    if(openFile)
        QMcWin.SetOpenFile(inName);

    // run Qt program
    return app.exec();
}
