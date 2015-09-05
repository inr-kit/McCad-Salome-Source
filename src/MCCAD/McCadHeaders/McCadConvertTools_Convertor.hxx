#ifndef _McCadConvertTools_Convertor_HeaderFile
#define _McCadConvertTools_Convertor_HeaderFile

#include <Handle_TopTools_HSequenceOfShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Boolean.hxx>

#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <TopoDS_Shape.hxx>

#include <McCadGUI_BaseDialogPtr.hxx>
#include <McCadGUI_BaseDialog.hxx>
#include <McCadGUI_ProgressDialogPtr.hxx>
#include <McCadGUI_ProgressDialog.hxx>
#include <McCadGUI_InputDialogPtr.hxx>
#include <McCadGUI_InputDialog.hxx>
#include <McCadGUI_GuiFactoryPtr.hxx>
#include <McCadGUI_GuiFactory.hxx>
#include <McCadDiscDs_HSequenceOfDiscSolid.hxx>

//! \brief Decomposition Master Class <br>

//! The Convertor performes a sign-constant decomposition on input solids. <br>
//! Besides the standard constructor one can initialize an object of this class <br>
//! with a HSequenceOfShape (Sequence of TopoDS_Solids), a CAD file (step, iges, brep), <br>
//! and a directory and an additional file filter (latter defines the type of CAD file that <br>
//! should be read). If a directory and file filter are given each CAD file of the <br>
//! given file format will be read. A sign constant decomposition <br>
//! for all TopoDS_Solids found will be executed by calling McCadCSGTool_Decomposer. <br>

class McCadConvertTools_Convertor {

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

//! Standard c-tor returns a McCadConvertTools_Convertor
//qiu add Standard_EXPORT
McCadConvertTools_Convertor();

//! Ctor which takes a Sequence of TopoDS_Shapes (solids) as input.
Standard_EXPORT McCadConvertTools_Convertor(const Handle(TopTools_HSequenceOfShape)& theHSeqOfShp);

//! Initialize the object. All c-tors call this method. <br>
//! It can be used to reset an object to the ground state.
Standard_EXPORT  void Init() ;

//! Perform sign constant decomposition on input solids <br>
//! This method calls the McCadCSGTool_Decomposer which performes a sign constant <br>
//! decomposition. <br>
//! Before the decomposition begins an analyses of all surfaces is performed and <br>
//! printed to the screen. Only the first five surface types can be processed <br>
//! if surfaces of other surface type occure, the file can not be converted into <br>
//! MC-syntax. <br>
//! After the surface check each solid is tested for geometrical/topological errors. <br>
//! McCad uses the standard OCC features for error detection and repair. <br>
//! The volumes of the unprocessed CAD files and the decomposed objects are recorded <br>
//! and compared. If a solid is not decomposed correctly, i.e. parts are missing, <br>
//! the name of the corresponing file and information about the volume comparision <br>
//! are written in the file ".failedVolumeControl" in the working directory. <br>
Standard_EXPORT  Standard_Boolean Convert() ;

//! Perform a sign constant decomposition on all input solids, which are given as parameter <br>
//! to this method. Calls Convert() internally <br>
//! Returns the converted model. <br>
Standard_EXPORT  Handle_TopTools_HSequenceOfShape Convert(const Handle(TopTools_HSequenceOfShape)& theHSeqOfShp) ;

//! Return the converted Model as a HSequenceOfShape
Standard_EXPORT  Handle_TopTools_HSequenceOfShape GetConvertedModel() ;

//! Return the file name
Standard_EXPORT  TCollection_AsciiString GetFileName() const { return myFileName; }

//! Set the file name of the input file
Standard_EXPORT  void SetFileName(const TCollection_AsciiString& theFileName) { myFileName = theFileName; }

//! Returns true, if the conversion has been successfully performed.
Standard_EXPORT  Standard_Boolean IsConverted() const;

//! Return progress dialog
//! Allows the visualization of the progress in the decomposition phase.
Standard_EXPORT  McCadGUI_ProgressDialogPtr GetProgressDialog();

private:

 // Fields
    Handle_TopTools_HSequenceOfShape myInputSolids;
    Handle_TopTools_HSequenceOfShape myConvertedSolids;
    TCollection_AsciiString myFileName;
    Standard_Boolean myIsConverted;

    McCadGUI_ProgressDialogPtr myProgress;
};

#endif
