#include <McCadIOTools_ExportStep.hxx>
#include <McCadIOTools.hxx>

McCadIOTools_ExportStep::McCadIOTools_ExportStep( const Handle_McCadCom_CasDocument& theDoc,
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


void McCadIOTools_ExportStep::Execute() {
    // get export plugin
    TCollection_AsciiString extension(".stp");
    McCadIOTools::Export( extension, myDoc );
}


