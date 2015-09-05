#include "McCadInputModelData.hxx"

#include <TopTools_HSequenceOfShape.hxx>
#include <McCadEXPlug_PluginManager.hxx>
#include <McCadEXPlug_ExchangePlugin.hxx>
#include <McCadMessenger_Singleton.hxx>

#include "../McCadTool/McCadConvertConfig.hxx"
//qiu for access()
#ifdef WIN32
#include <io.h>
#else //linux
#include <unistd.h>
#endif


McCadInputModelData::McCadInputModelData()
{
    m_listModelData = new TopTools_HSequenceOfShape();
}


bool McCadInputModelData::LoadSTEPModel(TCollection_AsciiString inputModel)
{
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();

    TCollection_AsciiString strDir = McCadConvertConfig::GetDirectory();
    strDir.LeftAdjust();
    strDir.RightAdjust();

    if( strDir.IsEmpty())  // If the working directory is empty
    {
        TCollection_AsciiString message("_#_McCadVoidCellManager :: Working directory is empty");
        msgr->Message(message.ToCString(), McCadMessenger_WarningMsg);
        return false;
    }

    TCollection_AsciiString file_path = strDir + TCollection_AsciiString("/") + inputModel;
//qiu    if (access(file_path.ToCString(),F_OK))
    if (access(file_path.ToCString(),0))  //0 is F_OK for linux, Existence only 
	{
        return false;
    }

    // read all files that fit the mask ( beginning with converted, ending on known extension )
    TCollection_AsciiString theCommandString;
    theCommandString = TCollection_AsciiString("ls -1 ") + strDir + TCollection_AsciiString("/") + inputModel;
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
        perror("_#_McCadVoidCellManager :: Error in open file :");
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
        if(tmpName.Search(".stp") >= 0 || tmpName.Search(".step") >= 0) {
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


bool McCadInputModelData::AddFile(const TCollection_AsciiString &file)
{
    McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();

    cout << " file : " << file.ToCString() << endl;
    // extract units from first file (MM/CM)
    bool m_isFirstFile = true;

    if(m_isFirstFile){
        m_isFirstFile = false;
        ifstream readFile;
        readFile.open(file.ToCString(), ios::in);
        while(!readFile.eof()) {
            char str[255];
            readFile.getline(str,255);
            TCollection_AsciiString line(str);
            if(line.Search("SI_UNIT")) {
               // if(line.Search(".MILLI."))
                    //m_parameters.units = McCadCSGGeom_MM;
                //else
                    //m_parameters.units = McCadCSGGeom_CM;
               // break;
            }
        }
    }

    // get file filter
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
    m_listModelData = exchangePlugin->Import();

    if( m_listModelData->Length() < 1 ){
        TCollection_AsciiString message("_#_McCadIOHelper_InputFileParser :: could not read shapes from file : ");
        message.AssignCat(file);
        msgr->Message(message.ToCString(), McCadMessenger_WarningMsg);
        return false;
    }
    return true;
}


Handle_TopTools_HSequenceOfShape McCadInputModelData::GetModelData()
{
    return m_listModelData;
}
