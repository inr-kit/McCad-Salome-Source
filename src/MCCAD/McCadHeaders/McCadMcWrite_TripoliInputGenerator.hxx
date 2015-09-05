/*
 * McCadMcWrite_TripoliInputGenerator.hxx
 *
 *  Created on: Aug 7, 2009
 *      Author: grosse
 */

#ifndef MCCADMCWRITE_TRIPOLIINPUTGENERATOR_HXX_
#define MCCADMCWRITE_TRIPOLIINPUTGENERATOR_HXX_


#include <McCadCSGGeom_HSequenceOfCell.hxx>
#include <McCadMcWrite_McInputGenerator.hxx>
#include <OSD_File.hxx>
#include <McCadMDReader_Reader.hxx>
#include <McCadCSGGeom_DataMapOfIntegerSurface.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>

//! \brief Generate Tripoli Geometry Description

//! Generates Tripoli Geometry Description from a independent McCad internally volume description.

class McCadMcWrite_TripoliInputGenerator  : public McCadMcWrite_McInputGenerator {

public:
   Standard_EXPORT McCadMcWrite_TripoliInputGenerator();

   McCadMcWrite_TripoliInputGenerator(const Handle(McCadCSGGeom_HSequenceOfCell)& theCellSeq);

   void SetCells(const Handle(McCadCSGGeom_HSequenceOfCell)& theCells) ;

   Handle_McCadCSGGeom_HSequenceOfCell GetCells() const;

   void SetNbOfMatCells(const Standard_Integer theNumber) ;

   Standard_Integer GetNbOfMatCells() const;

   void SetNbOfVoidCells(const Standard_Integer theNumber) ;

   Standard_Integer GetNbOfVoidCells() const;

   void SetNbOfOuterCells(const Standard_Integer theNumber) ;

   Standard_Integer GetNbOfOuterCells() const;

   virtual  void MakeHeader() ;

   void AppendToHeader(const TCollection_AsciiString& theText) ;

   void SurfNumbering() ;

   virtual  void PrintHeader(Standard_OStream& theStream) ;

   void PrintCell(Standard_OStream& theStream) ;

   void PrintSurface(Standard_OStream& theStream) ;

   void PrintTrsf(Standard_OStream& theStream) ;

   virtual  void PrintAll(Standard_OStream& theStream) ;

   void PrintAll(OSD_File& theFile) ;

   void PrintAllCells(Standard_OStream& theStream) ;

   void PrintMaterial(Standard_OStream& theStream) ;

   void SetMDReader(const McCadMDReader_Reader& theMDReader);

   void SetInitialSurfaceNumber(Standard_Integer& theInt);

   void SetInitialCellNumber(Standard_Integer& theInt);

private:
	 // Fields PRIVATE
	 //
	Handle_TColStd_HSequenceOfAsciiString myHeader;
	Handle_McCadCSGGeom_HSequenceOfCell myCellSeq;
	Standard_Integer myNbMatCells;
	Standard_Integer myNbVoidCells;
	Standard_Integer myNbOuterCells;
	Standard_Integer myNbTrsfCards;
	Standard_Integer myNbSurfCards;
	Standard_Boolean myIsHeaded;
	Standard_Boolean myIsNumbered;
	McCadCSGGeom_DataMapOfIntegerSurface myGlSurfaceMap;
	McCadMDReader_Reader myMDReader;
	Standard_Integer myInitSurfNb;
	Standard_Integer myInitCellNb;
};




#endif /* MCCADMCWRITE_TRIPOLIINPUTGENERATOR_HXX_ */
