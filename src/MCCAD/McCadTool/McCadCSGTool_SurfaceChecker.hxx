/*
 * McCadCSGTool_SurfaceChecker.hxx
 *
 *  Created on: Jun 7, 2010
 *      Author: grosse
 */

#ifndef MCCADCSGTOOL_SURFACECHECKER_HXX_
#define MCCADCSGTOOL_SURFACECHECKER_HXX_

//! \brief perform a check for all surfaces in a CAD file

//! Tests for Surface Types and prints a list of all used surfaces in a STP file<br>
//! to the outputfile outp

#include <TCollection_AsciiString.hxx>
#include <TopTools_HSequenceOfShape.hxx>

class McCadCSGTool_SurfaceChecker {
public:
//qiu Standard_EXPORT    bool readFile(TCollection_AsciiString inName, TCollection_AsciiString logFileName="surfaceCheck.log");
//qiu Standard_EXPORT    void readDirectory(TCollection_AsciiString astring, TCollection_AsciiString logFileName="surfaceCheck.log");
Standard_EXPORT    bool printSurfaceInfo(Handle_TopTools_HSequenceOfShape& shapes,
                          TCollection_AsciiString inName="undefined",
                          TCollection_AsciiString logFileName="surfaceCheck.log");

Standard_EXPORT    McCadCSGTool_SurfaceChecker();
Standard_EXPORT    ~McCadCSGTool_SurfaceChecker();
};

#endif /* MCCADCSGTOOL_SURFACECHECKER_HXX_ */
