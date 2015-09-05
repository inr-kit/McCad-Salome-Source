#include <McCadIOTools.hxx>
#include <McCadEXPlug_ExchangePlugin.hxx>
#include <McCadEXPlug_PluginManager.hxx>

void McCadIOTools::Export(const TCollection_AsciiString &extension, Handle_McCadCom_CasDocument &document){
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();
    McCadEXPlug_ExchangePlugin* exportPlugin = pluginManager->GetPlugin(extension);

    // export brep file
    TCollection_AsciiString fileName = document->GetDocName();
    exportPlugin->SetFilename(fileName);
    exportPlugin->ExportFromDocument( document->GetTDoc() );
}

void McCadIOTools::Import(const TCollection_AsciiString &extension, Handle_McCadCom_CasDocument &document){
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();
    McCadEXPlug_ExchangePlugin* stepPlugin = pluginManager->GetPlugin(extension);

    // export step file
    TCollection_AsciiString fileName = document->GetDocName();
    stepPlugin->SetFilename(fileName);
    Handle_TDocStd_Document tDoc = document->GetTDoc();
    stepPlugin->ImportToDocument( tDoc );
}
