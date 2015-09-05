#ifndef MCCADIOTOOLS_MCEXPORTBASE_HXX
#define MCCADIOTOOLS_MCEXPORTBASE_HXX

#include <McCadTool_Task.hxx>

#include <McCadMDReader_Reader.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <McCadDiscDs_HSequenceOfDiscSolid.hxx>
#include <McCadIOHelper_VGParameters.hxx>

/*! Base class for all export tools for Monte Carlo Codes
    This class manages the decomposition and the generation
    of void space from the GUI level.
*/

class McCadIOTools_MCExportBase : public McCadTool_Task {
public:
    McCadIOTools_MCExportBase();
    McCadIOTools_MCExportBase(const Handle_McCadCom_CasDocument& theDoc,
                            const Handle_McCadCom_CasView& theView,
                            const McCadTool_State theState,
                            const Standard_Boolean theUndoState,
                            const Standard_Boolean theRedoState);

    void Execute();
protected:
    void PrepareExport();

    Handle_McCadDiscDs_HSequenceOfDiscSolid m_voxelizedOutputGeometry;
    McCadIOHelper_VGParameters m_parameters;
    McCadMDReader_Reader m_mdReader;
};

#endif // MCCADIOTOOLS_MCEXPORTBASE_HXX

