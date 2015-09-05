#include <McCadIOTools_ImportMcnp.hxx>
#include <McCadIOTools.hxx>

McCadIOTools_ImportMcnp::McCadIOTools_ImportMcnp( const Handle_McCadCom_CasDocument& theDoc,
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


void McCadIOTools_ImportMcnp::Execute() {
    // get export plugin
    TCollection_AsciiString extension(".mcn");
    McCadIOTools::Import( extension, myDoc );
}


