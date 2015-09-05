#ifndef _McCadIOHelper_Merger_HeaderFile
#define _McCadIOHelper_Merger_HeaderFile

#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Macro.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TCollection_AsciiString.hxx>

#include <McCadEXPlug_PluginManager.hxx>
#include <McCadEXPlug_ExchangePlugin.hxx>

//! \brief Fuse all step files in one directory in a single file.

//! Merges all files in a given directory into one single file. <br>
//! This class is implemented in the fusion call in McCad. <br>
//! 'McCad -f ./ outfilename.stp' fuses all STEP files in the <br>
//! working directory into the file outfilename.stp <br>

class McCadIOHelper_Merger  {

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

//! ctor
McCadIOHelper_Merger();

//! ctor plus path to directory
McCadIOHelper_Merger(const TCollection_AsciiString& thePath);

//! set to ground state
  void Init() ;

//! define the file filter for the input file
  void SetInFilter(const TCollection_AsciiString& theInFilter) ;

//! define the file filter for the output file
  void SetOutFilter(const TCollection_AsciiString& theOutFilter) ;

//! add a file to be merged
  Standard_Boolean AddFile(const TCollection_AsciiString& anotherFile) ;

//! add a directory that contains files to be merged
  Standard_Boolean AddDirectory(const TCollection_AsciiString& thePath) ;

//! perform merging process
  Standard_Boolean Merge() ;

//! call Merge() and write results into theOutFile
  Standard_Boolean MergeToFile(const TCollection_AsciiString& theOutFile) ;

//! return a HSequenceOfShape containing all shapes which have been added so far.
  Handle_TopTools_HSequenceOfShape GetHSeqOfShp() ;





protected:

 // Methods PROTECTED
 //


 // Fields PROTECTED
 //


private:

 // Methods PRIVATE
 //


 // Fields PRIVATE
 //
Handle_TopTools_HSequenceOfShape myHSeqOfShp;
TCollection_AsciiString myFileName;
TCollection_AsciiString myOutFilter;
TCollection_AsciiString myInFilter;
TCollection_AsciiString myPath;
McCadEXPlug_PluginManagerPtr myPluginManager;
McCadEXPlug_ExchangePluginPtr myImpoExpo;
Standard_Boolean myIsFirstShape;


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif
