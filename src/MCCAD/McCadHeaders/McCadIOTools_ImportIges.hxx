#ifndef MCCADIOTOOLS_IMPORTIGES_HXX
#define MCCADIOTOOLS_IMPORTIGES_HXX

#include <McCadTool_Task.hxx>

class McCadIOTools_ImportIges : public McCadTool_Task {
public:
    McCadIOTools_ImportIges();
    McCadIOTools_ImportIges(const Handle_McCadCom_CasDocument& theDoc,
                            const Handle_McCadCom_CasView& theView,
                            const McCadTool_State theState,
                            const Standard_Boolean theUndoState,
                            const Standard_Boolean theRedoState);

    void Execute();
};

#endif // MCCADIOTOOLS_IMPORTIGES_HXX
