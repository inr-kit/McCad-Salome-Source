#ifndef MCCADIOTOOLS_IMPORTSTEP_HXX
#define MCCADIOTOOLS_IMPORTSTEP_HXX

#include <McCadTool_Task.hxx>

class McCadIOTools_ImportStep : public McCadTool_Task {
public:
    McCadIOTools_ImportStep();
    McCadIOTools_ImportStep(const Handle_McCadCom_CasDocument& theDoc,
                            const Handle_McCadCom_CasView& theView,
                            const McCadTool_State theState,
                            const Standard_Boolean theUndoState,
                            const Standard_Boolean theRedoState);

    void Execute();
};

#endif // MCCADIOTOOLS_IMPORTSTEP_HXX
