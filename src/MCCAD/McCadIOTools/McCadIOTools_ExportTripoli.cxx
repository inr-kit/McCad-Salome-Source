#include <McCadIOTools_ExportTripoli.hxx>
#include <McCadEXPlug_ExchangePlugin.hxx>
#include <McCadEXPlug_PluginManager.hxx>

McCadIOTools_ExportTripoli::McCadIOTools_ExportTripoli( const Handle_McCadCom_CasDocument& theDoc,
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


void McCadIOTools_ExportTripoli::Execute() {
    // decompose geometry and calculate void space
    PrepareExport();

    // export via McCadEXDllTripoli_Tripoli::Export()
    McCadEXPlug_PluginManager* pluginManager = McCadEXPlug_PluginManager::Instance();
    TCollection_AsciiString filter(".tri");
    McCadEXPlug_ExchangePlugin* tripoliWriter = pluginManager->GetPlugin(filter);
    tripoliWriter->SetInitSurfNb(m_parameters.initSurfaceNumber);
    tripoliWriter->SetInitCellNb(m_parameters.initCellNumber);
    tripoliWriter->SetMDReader(m_mdReader);
    tripoliWriter->SetUnits(m_parameters.units);
    tripoliWriter->SetFilename(myDoc->GetDocName());
    tripoliWriter->Export(m_voxelizedOutputGeometry);
}


