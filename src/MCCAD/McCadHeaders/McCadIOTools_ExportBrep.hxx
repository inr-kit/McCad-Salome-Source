#ifndef MCCADIOTOOLS_EXPORTBREP_HXX
#define MCCADIOTOOLS_EXPORTBREP_HXX

#include <McCadTool_Task.hxx>

class McCadIOTools_ExportBrep : public McCadTool_Task {
public:
    McCadIOTools_ExportBrep();
    McCadIOTools_ExportBrep(const Handle_McCadCom_CasDocument& theDoc,
                            const Handle_McCadCom_CasView& theView,
                            const McCadTool_State theState,
                            const Standard_Boolean theUndoState,
                            const Standard_Boolean theRedoState);

    void Execute();
};

#endif // McCadIOTools_ExportBrep_HXX
