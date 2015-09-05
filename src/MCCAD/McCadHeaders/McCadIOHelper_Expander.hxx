#ifndef _McCadIOHelper_Expander_HeaderFile
#define _McCadIOHelper_Expander_HeaderFile

#include <TopTools_HSequenceOfShape.hxx>
#include <McCadEXPlug_ExchangePluginPtr.hxx>
#include <McCadEXPlug_PluginManagerPtr.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Boolean.hxx>

#include <Standard.hxx>
#include <Standard_Macro.hxx>

//! \brief Class to explode an assembly file to several files. <br>

//! The McCadIOHelper_Expander can be used to explode a step file which contains <br>
//! an assembly where the single solids are not visible. <br>
//! Example: 'McCad -e Infile.stp' creates the files <br>
//!          ExOutInfile_1.stp <br>
//!          ExOutInfile_2.stp <br>
//!          ExOutInfile_3.stp <br>
//!          ... <br>
//! where every single file contains one single solid. <br>


class McCadIOHelper_Expander  {

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

//! ctor - returns McCadIOHelper_Expander
 McCadIOHelper_Expander();

//! ctor - returns McCadIOHelper_Expander <br>
//! initialization with a HSequenceOfShape <br>
//! a file name must be specified elsewhere
 McCadIOHelper_Expander(const Handle(TopTools_HSequenceOfShape)& theHSeqOfShp);

//! Explode the assemblies in the HSequenceOfShape and return all single solids <br>
//! in another HSequenceOfShape
   Handle_TopTools_HSequenceOfShape Explode(const Handle(TopTools_HSequenceOfShape)& aHSeqOfShp) ;

//! Explode the assemblies and return all single solids <br>
//! in another HSequenceOfShape
   Handle_TopTools_HSequenceOfShape Explode() ;

//! Returns exploded shapes
   Handle_TopTools_HSequenceOfShape GetExplodedShapes();

private:

    Handle_TopTools_HSequenceOfShape m_shapesToExplode;
    Handle_TopTools_HSequenceOfShape m_explodedShapes;
};

#endif
