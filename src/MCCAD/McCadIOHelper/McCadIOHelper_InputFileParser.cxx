#include <McCadDiscDs_DiscSolid.hxx>
#include <McCadDiscDs_HSequenceOfDiscSolid.hxx>
#include <McCadEXPlug_PluginManager.hxx>
#include <McCadEXPlug_ExchangePlugin.hxx>
#include <McCadIOHelper_InputFileParser.hxx>
#include <McCadMessenger_Singleton.hxx>

#include <fstream>
#include <string>

#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>


McCadIOHelper_InputFileParser::McCadIOHelper_InputFileParser(){
    Init();
}


McCadIOHelper_InputFileParser::McCadIOHelper_InputFileParser(const TCollection_AsciiString& inputFileName){
    Init();
    Parse(inputFileName);
}


void McCadIOHelper_InputFileParser::Init(){
    m_isDone = false;
    m_isFirstFile = true;
    m_parameters.writeCollisionFile = true;
    m_parameters.writeVoxelFile = true;

    m_partieIndex = 0;
    m_parameters.minInputSolidVolume = 1.;
    m_parameters.minSizeOfDecompositionFace = 1.;
    m_parameters.minSizeOfRedecompositionFace = 0.1;
    m_parameters.minVoidVolume = 0.0;
    m_parameters.tolerance = 1.e-7;

    m_parameters.initCellNumber    = 0;
    m_parameters.initSurfaceNumber = 0;
    m_parameters.maxNumberOfPreDecompositionCells = 5000;
    m_parameters.xResolution = 50;
    m_parameters.yResolution = 50;

    m_parameters.maxNumberOfComplementCells = 5;
    m_parameters.maxSamplePoints = 50;
    m_parameters.minSamplePoints = 10;

    m_parameters.boundingBox = new TopTools_HSequenceOfShape;
    m_parameters.inputSolids = new McCadDiscDs_HSequenceOfDiscSolid;
    m_parameters.inputFileName = "none";
    m_inputFileName = "none";
    m_parameters.materialDensityFileName = "none";

    m_parameters.m_listMatSolids = new TopTools_HSequenceOfShape;


}


bool McCadIOHelper_InputFileParser::Parse(const TCollection_AsciiString& inputFileName){
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    m_inputFileName = inputFileName;
    m_parameters.inputFileName = m_inputFileName;
    if(!Parse()) {
        msgr->Message("Error during parsing of parameter file\n") ;
        return false;
    }
    if(!ReadBoundingBoxFile()){
        TCollection_AsciiString message("Error while reading bounding box file : ");
        message.AssignCat(m_parameters.boundingBoxName);
        msgr->Message(message.ToCString()) ;
        return false;
    }
    if(!ReadGeometryFiles()){
        TCollection_AsciiString message("Error while reading geometry file : ");
        message.AssignCat(m_currentGeometryFileName);
        msgr->Message("Error while reading geometry file\n") ;
        return false;
    }
    m_parameters.directories = m_directories;
    m_isDone = true;

    return true;
}


//
// P R I V A T E
//

bool McCadIOHelper_InputFileParser::Parse(){
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    TCollection_AsciiString message("_#_McCadConvertTools_InputFileParser :: ");

    // open parameter file
    ifstream inFile(m_inputFileName.ToCString());
    if (!inFile) {
        message.AssignCat("Cannot open parameter file : ");
        message.AssignCat(m_inputFileName);
        msgr->Message( message.ToCString(), McCadMessenger_ErrorMsg );
        return false;
    }

    // read file line by line
    while(inFile) {
        char str[255];
        inFile.getline(str,255);
        TCollection_AsciiString iString(str);

        // skip empty lines
        if(iString.IsEmpty())
            continue;

        // skip comment lines
        iString.LeftAdjust();
        iString.RightAdjust();
        if(iString.Value(1) == '#')
            continue;

        // turn tabs into spaces
        iString.ChangeAll('\t', ' ', Standard_False);

        // directories to add
        if( iString.Search(" ") < 0 ) {
                iString += TCollection_AsciiString(" ");
                m_directories += iString;
        }
        //codeword with parameter
        else {
            int i = iString.Search(" ");
            TCollection_AsciiString numString = iString.Split(i);
            numString.LeftAdjust();
            numString.RightAdjust();
            iString.LeftAdjust();
            iString.RightAdjust();
            iString.UpperCase();

            // search for codewords
            if(iString.IsEqual("WRITECOLLISIONFILE")) {
                if( numString.IsEqual("1")    || numString.IsEqual("true") ||
                    numString.IsEqual("True") || numString.IsEqual("TRUE")  )
                {
                    m_parameters.writeCollisionFile = true;
                }
                else
                    m_parameters.writeCollisionFile = false;
            }
            else if(iString.IsEqual("WRITEDISCRETEMODEL")) {
                if( numString.IsEqual("1")    || numString.IsEqual("true") ||
                    numString.IsEqual("True") || numString.IsEqual("TRUE")  )
                {
                    m_parameters.writeVoxelFile = true;
                }
                else
                    m_parameters.writeVoxelFile = false;
            }
            else if(iString.IsEqual("MINIMUMINPUTSOLIDVOLUME")) {
                if(!numString.IsRealValue())
                    MissmatchMessage(iString, numString);
                else
                    m_parameters.minInputSolidVolume = numString.RealValue();
            }
            else if(iString.IsEqual("MINIMUMVOIDVOLUME")) {
                if(!numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else {
                    m_parameters.minVoidVolume = numString.RealValue();
                }
            }
            else if(iString.IsEqual("MINIMUMSIZEOFDECOMPOSITIONFACEAREA")) {
                if(!numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_parameters.minSizeOfDecompositionFace = numString.RealValue();
            }
            else if(iString.IsEqual("MINIMUMSIZEOFREDECOMPOSITIONFACEAREA")) {
                if(!numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_parameters.minSizeOfRedecompositionFace = numString.RealValue();
            }
            else if(iString.IsEqual("MAXIMUMNUMBEROFPREDECOMPOSITIONCELLS")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_parameters.maxNumberOfPreDecompositionCells = numString.IntegerValue();
            }
            else if(iString.IsEqual("MAXIMUMNUMBEROFCOMPLEMENTEDCELLS")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_parameters.maxNumberOfComplementCells = numString.IntegerValue();
            }
            else if(iString.IsEqual("BOUNDINGBOX")) {
                m_parameters.boundingBoxName = numString;
            }
            else if(iString.IsEqual("MINIMUMNUMBEROFSAMPLEPOINTS")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_parameters.minSamplePoints = numString.IntegerValue();
            }
            else if(iString.IsEqual("MAXIMUMNUMBEROFSAMPLEPOINTS")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_parameters.maxSamplePoints = numString.IntegerValue();
            }
            else if(iString.IsEqual("XRESOLUTION")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_parameters.xResolution = numString.IntegerValue();
            }
            else if(iString.IsEqual("YRESOLUTION")) {
                if(!numString.IsIntegerValue() && !numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_parameters.yResolution = numString.IntegerValue();
            }
            else if(iString.IsEqual("TOLERANCE")) {
                if(!numString.IsRealValue())
                    MissmatchMessage(iString,numString);
                else
                    m_parameters.tolerance = numString.RealValue();
            }
            else if(iString.IsEqual("MDFILE")) {
                    m_parameters.materialDensityFileName = numString;
            }
            else if(iString.IsEqual("INITSURFNB")) {
                if(!numString.IsIntegerValue())
                    MissmatchMessage(iString,numString);
                else
                    m_parameters.initSurfaceNumber = numString.IntegerValue();
            }
            else if(iString.IsEqual("INITCELLNB")) {
                if(!numString.IsIntegerValue())
                    MissmatchMessage(iString,numString);
                else
                    m_parameters.initCellNumber = numString.IntegerValue();
            }            
            else if(iString.IsEqual("UNITS")) {
                if(numString.IsEqual("CM") || numString.IsEqual("cm"))
                    m_parameters.units = McCadCSGGeom_CM;
                if(numString.IsEqual("MM") || numString.IsEqual("mm"))
                    m_parameters.units = McCadCSGGeom_MM;
                if(numString.IsEqual("INCH") || numString.IsEqual("inch"))
                    m_parameters.units = McCadCSGGeom_Inch;
            }            
            else{
                TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: Unknown keyword : ");
                message.AssignCat(iString);
                msgr->Message(message.ToCString(), McCadMessenger_WarningMsg);
            }
        }
    }
    return true;
}


void McCadIOHelper_InputFileParser::MissmatchMessage(TCollection_AsciiString &keyword, TCollection_AsciiString &parameter){
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    TCollection_AsciiString message("_#_McCadConvertTools_InputFileParser :: ");
    message.AssignCat("unexpected data type for keyword \'");
    message.AssignCat(keyword);
    message.AssignCat("\' ( ");
    message.AssignCat(parameter);
    message.AssignCat(" ) ");

    msgr->Message(message.ToCString());
}


bool McCadIOHelper_InputFileParser::ReadBoundingBoxFile(){
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    // get file type and initialize plugin manager
    TCollection_AsciiString fileExtension = m_parameters.boundingBoxName;
    if(fileExtension.IsEmpty()){
        msgr->Message("_#_McCadIOHelper_InputFileParser :: no bounding box defined! ", McCadMessenger_WarningMsg);
        return true;
    }
    fileExtension = fileExtension.Split(fileExtension.SearchFromEnd(".")-1);
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();
    if( !pluginManager->CheckFormat(fileExtension) ){
        TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: Unknown file extension : ");
        message += fileExtension;
        msgr->Message(message.ToCString(), McCadMessenger_ErrorMsg);
        return false;
    }
    McCadEXPlug_ExchangePlugin* exchangePlugin = pluginManager->GetPlugin(fileExtension);
    exchangePlugin->SetFilename(m_parameters.boundingBoxName);

    // import geometry
    Handle_TopTools_HSequenceOfShape importedShapes = exchangePlugin->Import();
    if(importedShapes->Length() < 1){
        TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: Empty or non-existing file : ");
        message += m_parameters.boundingBoxName;
        msgr->Message(message.ToCString(), McCadMessenger_ErrorMsg);
        return false;
    }
    m_parameters.boundingBox->Append(importedShapes);
    return true;
}


bool McCadIOHelper_InputFileParser::ReadGeometryFiles(){
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    if(m_directories.IsEmpty()){
        msgr->Message("_#_McCadIOHelper_InputFileParser :: No input directories in parameter file defined!\n", McCadMessenger_ErrorMsg);
        return false;
    }

    // search for all converted*.[supported file extension] files in given directories
    TCollection_AsciiString theDirectories = m_directories;
    theDirectories.LeftAdjust();
    theDirectories.RightAdjust();
    while(theDirectories.Search(" ") > 0) {
        TCollection_AsciiString directory = theDirectories.Split(theDirectories.SearchFromEnd(" "));
        theDirectories.RightAdjust();
        directory.LeftAdjust();
        if( !AddDirectory(directory) ) {
            TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: error during file reading in directory : ");
            message.AssignCat(directory);
            msgr->Message(message.ToCString(), McCadMessenger_WarningMsg);
        }
    }
    theDirectories.RightAdjust();
    if( !AddDirectory(theDirectories) ){
        TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: error during reading file in directory : ");
        message.AssignCat(theDirectories);
        msgr->Message(message.ToCString(), McCadMessenger_WarningMsg);
    }

    return true;
}


bool McCadIOHelper_InputFileParser::AddDirectory(const TCollection_AsciiString& directory){
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();
    cout<<directory<<"---"<<endl;
    if( directory.IsEmpty() ){
        TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: empty directory passed to AddDirectory()");
        msgr->Message(message.ToCString(), McCadMessenger_WarningMsg);
        return false;
    }

    // read all files that fit the mask ( beginning with converted, ending on known extension )
    TCollection_AsciiString theCommandString;
    theCommandString = TCollection_AsciiString("ls -1 ") + directory + TCollection_AsciiString("/converted*");
    static unsigned int BUF_SIZE = 1024;
    char command_string[80];
    strcpy(command_string, std::string( theCommandString.ToCString()).c_str() );
    FILE *files;
//qiu     char in_buf[BUF_SIZE];
	    char in_buf[1024];

    //get list of files in given directory
//qiu
#ifdef WIN32
		files = _popen(command_string, "r");   
#else
		files = popen(command_string, "r"); 
#endif

    if (!files) {
        perror("_#_McCadIOHelper_InputFileParser :: error in popen :");
        return false;
    }

    // call AddFile for all files in current directory
    while (fgets(in_buf, BUF_SIZE, files)) {
        //handling line breaks
        char *w;
        w = strpbrk(in_buf, "\t\n");
        if (w)
            *w='\0';

        TCollection_AsciiString tmpName(in_buf);
        TCollection_AsciiString fileFilter(tmpName);
        fileFilter = fileFilter.Split( fileFilter.SearchFromEnd(".") -1 );
        if(! pluginManager->CheckExtension(fileFilter)) {
            TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: skipping file with unknown extension : ");
            message.AssignCat(tmpName);
            msgr->Message( message.ToCString(), McCadMessenger_WarningMsg );
            continue;
        }

        cout << " -- " << tmpName.ToCString() << endl;

        //Append Files beginning with converted
        if(tmpName.Search("converted") >= 0 ) {
            if( !AddFile(tmpName) ){
                TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: Failed adding file : ");
                message.AssignCat(tmpName);
                msgr->Message( message.ToCString(), McCadMessenger_WarningMsg );
            }
        }
        else{
            TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: file doesn't contain \'converted\' prefix : ");
            message.AssignCat(tmpName);
            msgr->Message(message.ToCString(), McCadMessenger_WarningMsg);
        }
    }
    return true;
}


bool McCadIOHelper_InputFileParser::AddFile(const TCollection_AsciiString &file){
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();
    cout << " file : " << file.ToCString() << endl;
    // extract units from first file (MM/CM)
    if(m_isFirstFile){
        m_isFirstFile = false;
        ifstream readFile;
        readFile.open(file.ToCString(), ios::in);
        while(!readFile.eof()) {
            char str[255];
            readFile.getline(str,255);
            TCollection_AsciiString line(str);
            if(line.Search("SI_UNIT")) {
                if(line.Search(".MILLI."))
                    m_parameters.units = McCadCSGGeom_MM;
                else
                    m_parameters.units = McCadCSGGeom_CM;
                break;
            }
        }
    }

    // get file filter
    Handle_TopTools_HSequenceOfShape inputShapes = new TopTools_HSequenceOfShape;
    TCollection_AsciiString fileFilter(file);
    fileFilter.Remove(1,fileFilter.SearchFromEnd(".")-1);

    // initialize import plugin and import geometry from file
    if( !pluginManager->CheckFormat(fileFilter) ){
        TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: No file filter for file : ");
        message.AssignCat(file);
        msgr->Message(message.ToCString(), McCadMessenger_WarningMsg);
        return false;
    }
    McCadEXPlug_ExchangePlugin* exchangePlugin = pluginManager->GetPlugin(fileFilter);
    exchangePlugin->SetFilename(file);
    inputShapes = exchangePlugin->Import();
    if( inputShapes->Length() < 1 ){
        TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: could not read shapes from file : ");
        message.AssignCat(file);
        msgr->Message(message.ToCString(), McCadMessenger_WarningMsg);
        return false;
    }

    // make vsolid and add to list
    for (Standard_Integer i = 1; i<= inputShapes->Length(); i++) {
        m_partieIndex++;
        McCadDiscDs_DiscSolid aVSol;
        aVSol.SetName(file, i);
        TopoDS_Solid tmpSol = TopoDS::Solid(inputShapes->Value(i));
        aVSol.SetSolid(tmpSol);
        aVSol.SetIndex(m_partieIndex);
        m_parameters.inputSolids->Append(aVSol);

        m_parameters.m_listMatSolids->Append(inputShapes->Value(i));

    }

    // Read the solid and generate the MatSolidList
    return true;
}
