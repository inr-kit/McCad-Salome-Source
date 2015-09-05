#ifndef _McCadEXPlug_ExchangePlugin_HeaderFile
#define _McCadEXPlug_ExchangePlugin_HeaderFile

#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _Handle_TopTools_HSequenceOfShape_HeaderFile
#include <Handle_TopTools_HSequenceOfShape.hxx>
#endif
#ifndef _Handle_McCadDiscDs_HSequenceOfDiscSolid_HeaderFile
#include <Handle_McCadDiscDs_HSequenceOfDiscSolid.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Handle_TColStd_HSequenceOfAsciiString_HeaderFile
#include <Handle_TColStd_HSequenceOfAsciiString.hxx>
#endif
class TopTools_HSequenceOfShape;
class McCadDiscDs_HSequenceOfDiscSolid;
class TCollection_AsciiString;
class TColStd_HSequenceOfAsciiString;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#include <McCadGUI_BaseDialogPtr.hxx>
#include <McCadGUI_BaseDialog.hxx>
#include <McCadGUI_ProgressDialogPtr.hxx>
#include <McCadGUI_ProgressDialog.hxx>
#include <McCadGUI_InputDialogPtr.hxx>
#include <McCadGUI_InputDialog.hxx>
#include <McCadGUI_GuiFactoryPtr.hxx>
#include <McCadGUI_GuiFactory.hxx>

#include <TDocStd_Document.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <TDF_LabelSequence.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <McCadMDReader_Reader.hxx>
#include <McCadCSGGeom_Unit.hxx>

//! \brief Base class for all Data Exchange between CAD and MC

//! This is the base class for all data exchange for CAD and MC data.


class McCadEXPlug_ExchangePlugin {

public:

    void* operator new(size_t,void* anAddress)
      {
        return anAddress;
      }
    void* operator new(size_t size)
      {
        return Standard::Allocate(size);
      }
    void  operator delete(void *anAddress)
      {
        if (anAddress) Standard::Free((Standard_Address&)anAddress);
      }
 // Methods PUBLIC
 //

//! This is Plugin proxy <br>
Standard_EXPORT McCadEXPlug_ExchangePlugin();

//! Export a HSequenceOfShape to a selected file type
Standard_EXPORT virtual  Standard_Boolean Export(const Handle(TopTools_HSequenceOfShape)& theEntitySequence) ;

//! Export a HSequenceOfDiscSolid to a selected file type - naming information will be preserved
Standard_EXPORT virtual  Standard_Boolean Export(const Handle(McCadDiscDs_HSequenceOfDiscSolid)& theVEntitySequence) ;

//! Import a file of given file type
Standard_EXPORT virtual  Handle_TopTools_HSequenceOfShape Import() ;

//! Returns the model type - depends on the file type, e.g. for STEP it would be <br>
// STEPControl_AsIs, STEPControl_ManifoldSolid,...
Standard_EXPORT virtual  Standard_Integer GetModelType() const;

//! Define the model type for export, see GetModelType()
Standard_EXPORT virtual  void SetModelType(const Standard_Integer theType) ;

//! Returns the filename
Standard_EXPORT virtual  TCollection_AsciiString GetFilename() ;

//! define the filename
Standard_EXPORT virtual  void SetFilename(const TCollection_AsciiString& theFileName) ;

//! ??? not used!
Standard_EXPORT virtual  Standard_Integer GetSize() const;

//! Returns all extensions of chosen implementation of proxy
Standard_EXPORT virtual  Handle_TColStd_HSequenceOfAsciiString GetExtensions() const;

//! Get main extension
Standard_EXPORT virtual  TCollection_AsciiString GetExtension() const;

//! Gets the selected fileformat
Standard_EXPORT virtual  TCollection_AsciiString GetFormat() const;

//! true if Export is enabled
Standard_EXPORT virtual  Standard_Boolean ExportEnabled() const;

//! true if Import is enabled
Standard_EXPORT virtual  Standard_Boolean ImportEnabled() const;

//! import to TDocStd_Document
Standard_EXPORT virtual Standard_Boolean ImportToDocument(Handle(TDocStd_Document)& theDoc);

//! Export from a TDocStd_Document
Standard_EXPORT virtual  Standard_Boolean ExportFromDocument(const Handle(TDocStd_Document)& theDoc);

//! Define the initial Surface number. This is necessary if a part for an already <br>
//! existing model should be converted.
Standard_EXPORT void SetInitSurfNb(Standard_Integer nb){myInitSurfNb = nb;}

//! Define the initial Volume number. This is necessary if a part for an already <br>
//! existing model should be converted.
Standard_EXPORT void SetInitCellNb(Standard_Integer nb){myInitCellNb = nb;}

//! Define the unit of the CAD model to be converted. Default is MM. If CM is chosen, <br>
//! the units will be recalculated.
Standard_EXPORT void SetUnits(McCadCSGGeom_Unit unit){myUnits = unit;}

//! Define the Material Density reader. MCNP only. It reads material information and <br>
//! prepares them for the cell description. This is not necessary in Tripoli, because <br>
//! material compositions can easily be defined with scripts. In MCNP for each volume <br>
//! individual material number and density have to be defined in the lines of <br>
//! geometrical description of the volume.
Standard_EXPORT void SetMDReader(McCadMDReader_Reader& rdr){myMDReader = rdr;}

#ifndef MCCAD_NONGUI
Standard_EXPORT   McCadGUI_ProgressDialogPtr GetProgressDialog();
#endif

protected:
    McCadGUI_ProgressDialogPtr myProgress;
    Standard_Integer myInitCellNb, myInitSurfNb;
    McCadCSGGeom_Unit myUnits;
    McCadMDReader_Reader myMDReader;

};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
