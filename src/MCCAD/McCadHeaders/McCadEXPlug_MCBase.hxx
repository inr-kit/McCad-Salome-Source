#ifndef _McCadEXPlug_MCBase_hxx_
#define _McCadEXPlug_MCBase_hxx_
/*
 McCadEXPlug_MCBase.hxx
	
Purpose: Base Class for import and export classes for Monte-Carlo codes
	Besides the McCadEXPlug_PluginManager which offers a basis for
	all kind of Data-Exchanges CAD & MC, the MC class will provide
	only features needed for MC data exchange

*/

#include <McCadDiscDs_HSequenceOfDiscSolid.hxx>
#include <TDocStd_Document.hxx>
#include <QMcCad_Application.h>
#include <QMcCad_VGParamDialog.h>
#include <TDataStd_Name.hxx>
#include <TDocStd_Application.hxx>
#include <Message_ProgressIndicator.hxx>
#include <McCadGUI_ProgressDialog.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TCollection_AsciiString.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <McCadConvertTools_Convertor.hxx>
#include <McCadConvertTools_VoidGenerator.hxx>
#include <McCadCSGGeom_Unit.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <McCadXCAF_TDocShapeLabelIterator.hxx>
#include <McCadMDReader_Reader.hxx>
#include <McCadEXPlug_ExchangePlugin.hxx>


//! \brief Base Class for all Import/Export classes for Monte Carlo codes.

//! Base Class for all Import and Export related classes for Monte Carlo codes. <br>

class McCadEXPlug_MCBase : public McCadEXPlug_ExchangePlugin {
public: // functions
	
//! ctor
    McCadEXPlug_MCBase();

//! Perform decomposition and void generation of CAD model from a XCAF-Document
    virtual Handle_McCadDiscDs_HSequenceOfDiscSolid	PrepareExport(const Handle(TDocStd_Document)& theTDoc);

//! By default void volumes will not be built when reading a MC file in McCad. <br>
//! Setting state to true will activate conversion from Void volumes in MC->CAD process.
    virtual void RecoverVoids(const Standard_Boolean& state);
	
protected : // fields
    Standard_Boolean myRecoverVoids; // for MC input reader, recover voids; or material volumes only?
    Handle_Message_ProgressIndicator myPI;
};


#endif
