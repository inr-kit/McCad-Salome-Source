#ifndef MCCADIOTOOLS_EXPORTIGES_HXX
#define MCCADIOTOOLS_EXPORTIGES_HXX

#include <McCadTool_Task.hxx>

class McCadIOTools_ExportIges : public McCadTool_Task {
public:
    McCadIOTools_ExportIges();
    McCadIOTools_ExportIges(const Handle_McCadCom_CasDocument& theDoc,
                            const Handle_McCadCom_CasView& theView,
                            const McCadTool_State theState,
                            const Standard_Boolean theUndoState,
                            const Standard_Boolean theRedoState);

    void Execute();
};

#endif // MCCADIOTOOLS_EXPORTIGES_HXX
