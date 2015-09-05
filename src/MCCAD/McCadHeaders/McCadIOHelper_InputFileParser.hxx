#ifndef MCCADIOHELPER_INPUTFILEPARSER_HXX
#define MCCADIOHELPER_INPUTFILEPARSER_HXX

#include <TCollection_AsciiString.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <McCadCSGGeom_Unit.hxx>
#include <McCadIOHelper_VGParameters.hxx>


/*
    McCadIOHELPER_InputFileParser reads the parameter file for McCadIOHELPER_VoidGenerator
*/

class McCadIOHelper_InputFileParser{
public:
    //std ctor
    McCadIOHelper_InputFileParser();

    // ctor with filename as input
    McCadIOHelper_InputFileParser( const TCollection_AsciiString& inputFileName );

    // parse
    bool Parse();

    // parse input file
    bool Parse(const TCollection_AsciiString& inputFileName );

    // Inline Getters
    ////////////////////////////////

    inline McCadIOHelper_VGParameters GetVGParameters() { return m_parameters; }

    // done?
    bool IsDone() const { return m_isDone; }

private:
    // methods
    void Init();

    // print missmatch message
    void MissmatchMessage(TCollection_AsciiString& keyword, TCollection_AsciiString& parameter);

    // read bounding box from geometry file (stp) and save in in m_parameters.boundingBox
    bool ReadBoundingBoxFile() ;

    // read geometry for void generation and save it in m_parameters.inputSolids (McCadDiscDs_Solids)
    bool ReadGeometryFiles() ;

    // add all converted files from directory
    bool AddDirectory(const TCollection_AsciiString& directory) ;

    // add converted* file
    bool AddFile(const TCollection_AsciiString &file);

    // fields
    bool m_isDone;
    bool m_isFirstFile;
    unsigned int m_partieIndex;
    TCollection_AsciiString m_directories;
    TCollection_AsciiString m_inputFileName;
    TCollection_AsciiString m_currentGeometryFileName;
    McCadIOHelper_VGParameters m_parameters;
};

#endif // MCCADIOHELPER_INPUTFILEPARSER_HXX
