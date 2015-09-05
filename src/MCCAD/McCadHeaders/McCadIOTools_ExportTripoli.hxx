#ifndef MCCADIOTOOLS_EXPORTTRIPOLI_HXX
#define MCCADIOTOOLS_EXPORTTRIPOLI_HXX

#include <McCadIOTools_MCExportBase.hxx>

class McCadIOTools_ExportTripoli : public McCadIOTools_MCExportBase {
public:
    McCadIOTools_ExportTripoli();
    McCadIOTools_ExportTripoli(const Handle_McCadCom_CasDocument& theDoc,
                            const Handle_McCadCom_CasView& theView,
                            const McCadTool_State theState,
                            const Standard_Boolean theUndoState,
                            const Standard_Boolean theRedoState);

    void Execute();
};

#endif // MCCADIOTOOLS_EXPORTTRIPOLI_HXX
