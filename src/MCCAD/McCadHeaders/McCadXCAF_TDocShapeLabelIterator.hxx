/*
 * McCadXCAF_TDocShapeLabelIterator.hxx
 *
 *  Created on: Mar 5, 2009
 *      Author: grosse
 */

#ifndef MCCADXCAF_TDOCSHAPELABELITERATOR_HXX_
#define MCCADXCAF_TDOCSHAPELABELITERATOR_HXX_

#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Tool.hxx>


//qiu defined Standard_EXPORT
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
//! \brief Shape iterator for a TDocStd_Document

/** Purpose
 *
 * Iterates through the free TopLevel Shapes and their children (recursively) of the given document structure
 * and looks for shapes.
 */

class McCadXCAF_TDocShapeLabelIterator
{

public:

	//ctor
	McCadXCAF_TDocShapeLabelIterator() ;
Standard_EXPORT	McCadXCAF_TDocShapeLabelIterator(const Handle_TDocStd_Document& theTDoc) ;

Standard_EXPORT	void Initialize(const Handle_TDocStd_Document& theTDoc) ;
Standard_EXPORT	Standard_Boolean More() ;
Standard_EXPORT	void Next() ;
Standard_EXPORT	TDF_Label Current() ;
Standard_EXPORT	TDF_Label Value() ; // same as Current
Standard_EXPORT	TDF_Label Previous() ;
Standard_EXPORT    Standard_Integer NumberOfShapes();
Standard_EXPORT    TDF_Label GetAt(int index);

private:
	//functions
    //Standard_Integer NumberOfShapes();
Standard_EXPORT	TDF_Label Traverse(const TDF_Label& theLab, Standard_Integer& theInt, const Standard_Integer& stop = 0);

	//fields
	Handle_XCAFDoc_ShapeTool mySTool;
	TDF_LabelSequence myFreeShapes;
	Standard_Integer myCurrentCnt;
	Standard_Integer myEnd;
	TDF_Label myPrevious;
	TDF_Label myCurrent;
};

#endif /* MCCADXCAF_TDOCSHAPELABELITERATOR_HXX_ */
