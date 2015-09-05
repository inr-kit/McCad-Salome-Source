#include <cstring>
#include <map>
#include <string>

#include <TCollection_AsciiString.hxx>
#include <OSD_FileIterator.hxx>
#include <OSD_File.hxx>
#include <OSD_SharedLibrary.hxx>
#include <OSD_LoadMode.hxx>

#include <McCadEXPlug_PluginMaker.hxx>
#include <McCadEXPlug_PluginManager.ixx>

using namespace std;

//qiu remove plugin, might be dangerous!
/*qiu extern*/ map<string, McCadEXPlug_PluginMaker , less<string> > PluginMakerMap;

McCadEXPlug_PluginManagerPtr McCadEXPlug_PluginManager::myInstance = 0;


McCadEXPlug_PluginManager::McCadEXPlug_PluginManager(){
    myExtensions = new TColStd_HSequenceOfAsciiString;
    myFormats = new TColStd_HSequenceOfAsciiString;
    myPluginCount=0;
    myPathName="";
    mySharedLibIsOpen= Standard_False;
}


McCadEXPlug_PluginManagerPtr McCadEXPlug_PluginManager::Instance(){
    if (myInstance == 0)
        myInstance = new McCadEXPlug_PluginManager;
    return myInstance;
}


void McCadEXPlug_PluginManager::Initialize() {
    map<string, McCadEXPlug_PluginMaker, less<string> >::iterator thePluginMakerIter;
    for (thePluginMakerIter=PluginMakerMap.begin(); thePluginMakerIter!=PluginMakerMap.end(); thePluginMakerIter++) {
        string astr = thePluginMakerIter->first;
//qiu         char acstr[astr.size()];
//qiu        strcpy(acstr, astr.c_str());
//qiu        TCollection_AsciiString atcstr= acstr;
		TCollection_AsciiString atcstr= astr.c_str();
        myExtensions->Append(atcstr);
        myFormats->Append(atcstr);
        myPluginCount++;
    }
}


McCadEXPlug_ExchangePluginPtr McCadEXPlug_PluginManager::GetPlugin(	const TCollection_AsciiString& theExtension) const {
    if (CheckExtension(theExtension)) {
        string theExStr = theExtension.ToCString();
        McCadEXPlug_PluginMaker theMaker = PluginMakerMap[theExStr];
        McCadEXPlug_ExchangePlugin* thePlugIn = theMaker;

        return thePlugIn;
    }
    else {
        cout << "No plugin found for this extension!!" << endl;
        return NULL;
    }
}


Handle(TColStd_HSequenceOfAsciiString) McCadEXPlug_PluginManager::GetExtensions() const{
	return myExtensions;
}


Handle(TColStd_HSequenceOfAsciiString) McCadEXPlug_PluginManager::GetFormats() const{
	return myFormats;
}


Standard_Boolean McCadEXPlug_PluginManager::CheckExtension(	const TCollection_AsciiString& theExtension) const{
    if ( !theExtension.IsEmpty()) {
        if ( !myExtensions.IsNull() && !myExtensions->IsEmpty()) {
            for (int it = 1; it <= myExtensions->Length(); it++) {
                if (myExtensions->Value(it)== theExtension) {
                    return Standard_True;
                }
            }
        }
    }
    return Standard_False;
}


Standard_Boolean McCadEXPlug_PluginManager::CheckFormat(const TCollection_AsciiString& theFormat) const {
    if ( !theFormat.IsEmpty()) {
        if ( !myFormats.IsNull() && !myFormats->IsEmpty()) {
            for (int it = 1; it <= myFormats->Length(); it++) {
                if (myFormats->Value(it) == theFormat) {
                    return Standard_True;
                }
            }
        }
    }
    return Standard_False;
}


Standard_Boolean McCadEXPlug_PluginManager::LoadSharedLibrary(const OSD_Path& thePath) {
    TCollection_AsciiString aName;
    thePath.SystemName(aName);
    myPathName = aName;

    //cout<<"find the path"<< myPathName <<endl;

    TCollection_AsciiString theMask = "libTKMcCadEx*.so";
    OSD_FileIterator theFileIterator;
    OSD_File theFile;
    OSD_SharedLibrary theSharedLib;
    OSD_LoadMode theMode = OSD_RTLD_NOW;

    for (theFileIterator.Initialize(thePath, theMask); theFileIterator.More(); theFileIterator.Next()) {
        theFile = theFileIterator.Values();
        OSD_Path theFilePath;
        theFile.Path(theFilePath);
        TCollection_AsciiString aLibName;
        theFilePath.SystemName(aLibName);
        theSharedLib.SetName(aLibName.ToCString());

        if ( !theSharedLib.DlOpen(theMode) )
            cout << "ERROR : " << theSharedLib.DlError() << endl;
        else
            mySharedLibs.Append(theSharedLib);
    }

    if (mySharedLibs.IsEmpty())
        return Standard_False;
    else {
        Initialize();
        return Standard_True;
    }
}


void McCadEXPlug_PluginManager::UnLoadSharedLibrary() const {
    if (!mySharedLibs.IsEmpty()) {
        for (int i = 1; i <= mySharedLibs.Length(); i++) {
            mySharedLibs.Value(i).DlClose();
        }
    }
}


OSD_Path McCadEXPlug_PluginManager::SharedLibraryPath() const {
    return myPath;
}


TCollection_AsciiString McCadEXPlug_PluginManager::GetPathName() const {
    return myPath.Name();
}
