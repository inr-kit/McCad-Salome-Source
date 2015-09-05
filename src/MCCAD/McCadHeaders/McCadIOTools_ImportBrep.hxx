#ifndef MCCADIOTOOLS_IMPORTBREP_HXX
#define MCCADIOTOOLS_IMPORTBREP_HXX

#include <McCadTool_Task.hxx>

class McCadIOTools_ImportBrep : public McCadTool_Task {
public:
    McCadIOTools_ImportBrep();
    McCadIOTools_ImportBrep(const Handle_McCadCom_CasDocument& theDoc,
                            const Handle_McCadCom_CasView& theView,
                            const McCadTool_State theState,
                            const Standard_Boolean theUndoState,
                            const Standard_Boolean theRedoState);

    void Execute();
};

#endif // MCCADIOTOOLS_IMPORTBREP_HXX
