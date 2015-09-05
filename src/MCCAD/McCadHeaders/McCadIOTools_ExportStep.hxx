#ifndef MCCADIOTOOLS_EXPORTSTEP_HXX
#define MCCADIOTOOLS_EXPORTSTEP_HXX

#include <McCadTool_Task.hxx>

class McCadIOTools_ExportStep : public McCadTool_Task {
public:
    McCadIOTools_ExportStep();
    McCadIOTools_ExportStep(const Handle_McCadCom_CasDocument& theDoc,
                            const Handle_McCadCom_CasView& theView,
                            const McCadTool_State theState,
                            const Standard_Boolean theUndoState,
                            const Standard_Boolean theRedoState);

    void Execute();
};

#endif // MCCADIOTOOLS_EXPORTSTEP_HXX
