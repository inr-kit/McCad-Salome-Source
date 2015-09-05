#include <McCadIOTools_ExportMcnp.hxx>

#include <McCadEXPlug_ExchangePlugin.hxx>
#include <McCadEXPlug_PluginManager.hxx>

McCadIOTools_ExportMcnp::McCadIOTools_ExportMcnp( const Handle_McCadCom_CasDocument& theDoc,
                                                  const Handle_McCadCom_CasView& theView,
                                                  const McCadTool_State theState,
                                                  const Standard_Boolean theUndoState,
                                                  const Standard_Boolean theRedoState )
{
    myDoc = theDoc;
    myView = theView;
    myState = theState;
    myUndoState = theUndoState;
    myRedoState = theRedoState;
    myID = McCadTool_ExportMCNP;
}


McCadIOTools_ExportMcnp::McCadIOTools_ExportMcnp()
{
    myDoc = NULL;
    myView = NULL;
    myState = McCadTool_Waiting;
    myUndoState = false;
    myRedoState = false;
    myID = McCadTool_ExportMCNP;
}

void McCadIOTools_ExportMcnp::Execute() {
    // decompose geometry and calculate void space
    PrepareExport();

    // export via McCadEXDllMcnp_Mcnp::Export()
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();
    TCollection_AsciiString filter(".mcn");
    McCadEXPlug_ExchangePlugin* mcnpWriter = pluginManager->GetPlugin(filter);
    mcnpWriter->SetInitSurfNb(m_parameters.initSurfaceNumber);
    mcnpWriter->SetInitCellNb(m_parameters.initCellNumber);
    mcnpWriter->SetMDReader(m_mdReader);
    mcnpWriter->SetUnits(m_parameters.units);
    mcnpWriter->SetFilename(myDoc->GetDocName());
    mcnpWriter->Export(m_voxelizedOutputGeometry);
}


