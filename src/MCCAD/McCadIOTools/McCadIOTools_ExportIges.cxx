#include <McCadIOTools_ExportIges.hxx>
#include <McCadIOTools.hxx>

McCadIOTools_ExportIges::McCadIOTools_ExportIges( const Handle_McCadCom_CasDocument& theDoc,
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
    myID = McCadTool_ExportIGES;
}


void McCadIOTools_ExportIges::Execute() {
    // get export plugin
    TCollection_AsciiString extension(".iges");
    McCadIOTools::Export( extension, myDoc );
}


