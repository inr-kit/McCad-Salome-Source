#ifndef MCCADEXDLLTRIPOLI_TRIPOLI_HXX_
#define MCCADEXDLLTRIPOLI_TRIPOLI_HXX_


#include <McCadEXPlug_ExchangePlugin.hxx>
#include <McCadEXPlug_MCBase.hxx>

//! \brief Master Class for the export to Tripoli output <br>

/*! This class is the implementation of the CAD to Tripoli interface<br>
 *! It exports all discretized solids in McCadDiscDs_HSequenceOfDiscSolid to <br>
 *! a file with the file filter .d, or .tri. <br>
 *! The import function is not yet enabled.
 */


class McCadExDllTripoli_Tripoli  : /*public McCadEXPlug_ExchangePlugin,*/ public McCadEXPlug_MCBase
{

public:
	McCadExDllTripoli_Tripoli();

//	virtual  Standard_Boolean Export(const Handle(TopTools_HSequenceOfShape)& theEntitySequence) ;

	//! Exports a Sequence of DiscSolids to a previously specified filename.
	virtual  Standard_Boolean Export(const Handle(McCadDiscDs_HSequenceOfDiscSolid)& theVEntitySequence) ;

	//! Not yet implemented!!!
	virtual  Handle_TopTools_HSequenceOfShape Import() ;

	//! Returns the defined filename for the export
	virtual  TCollection_AsciiString GetFilename() const;

	//! Set the export filename
	virtual  void SetFilename(const TCollection_AsciiString& theFileName) ;

	//!  ??? - No function at all
	virtual  Standard_Integer GetSize() const;

	//! return a Sequence of file extension for this class (.d, .tri)
	virtual  Handle_TColStd_HSequenceOfAsciiString GetExtensions() const;

	//! Get first extension in list of extensions. ".d" by default.
	virtual  TCollection_AsciiString GetExtension() const;

	//! Returns the file format
	virtual  TCollection_AsciiString GetFormat() const;

	//! Returns true, if export is enabled.
	virtual  Standard_Boolean ExportEnabled() const;

	//! Returns true, if import is enabled
	virtual  Standard_Boolean ImportEnabled() const;

	//! Import to a TDocStd_Document. McCad is based on OpenCascades Application Framework.
	virtual Standard_Boolean ImportToDocument(Handle(TDocStd_Document)& theDoc);

	//! Export a TDocStd_Document to tripoli input geometry.
	virtual  Standard_Boolean ExportFromDocument(const Handle(TDocStd_Document)& theDoc);

	//! enables or disables the Build of Void Cells to Solids in CAD-Model <br>
	//void SetBuildVoids(const Standard_Boolean theStat) ;

	//   McCadGUI_ProgressDialogPtr GetProgressDialog();


protected:
	 // Fields PROTECTED
	 //
	TCollection_AsciiString myFormat;
	Standard_Boolean myExportEnabled;
	Standard_Boolean myImportEnabled;
	TCollection_AsciiString myFileName;
	Standard_Integer mySize;
	Standard_Integer myInitCellNb;
	Standard_Integer myInitSurfNb;


private:
	 // Fields PRIVATE
	 //
	Handle_TColStd_HSequenceOfAsciiString myExtensions;
	Standard_Boolean myBuildVoids;
	Handle(TopTools_HSequenceOfShape) myDecomposedModel;
};

#endif /* MCCADEXDLLTRIPOLI_TRIPOLI_HXX_ */
