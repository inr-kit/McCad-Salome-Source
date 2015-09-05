#ifndef MCCADIOTOOLS_EXPORTMCNP_HXX
#define MCCADIOTOOLS_EXPORTMCNP_HXX

#include <McCadIOTools_MCExportBase.hxx>

/*! GUI entrance point for file export into MCNP geometry description
*/


class McCadIOTools_ExportMcnp : public McCadIOTools_MCExportBase {
public:
    McCadIOTools_ExportMcnp();
    McCadIOTools_ExportMcnp(const Handle_McCadCom_CasDocument& theDoc,
                            const Handle_McCadCom_CasView& theView,
                            const McCadTool_State theState,
                            const Standard_Boolean theUndoState,
                            const Standard_Boolean theRedoState);

    void Execute();
};

#endif // MCCADIOTOOLS_EXPORTMCNP_HXX
