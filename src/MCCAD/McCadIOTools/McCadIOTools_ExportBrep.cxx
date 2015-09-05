#include <McCadIOTools_ExportBrep.hxx>
#include <McCadIOTools.hxx>

McCadIOTools_ExportBrep::McCadIOTools_ExportBrep( const Handle_McCadCom_CasDocument& theDoc,
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


void McCadIOTools_ExportBrep::Execute() {
    // get export plugin
    TCollection_AsciiString extension(".brep");
    McCadIOTools::Export( extension, myDoc );
}


