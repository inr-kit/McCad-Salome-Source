#ifndef MCCADIOHELPER_VGPARAMETERS_HXX
#define MCCADIOHELPER_VGPARAMETERS_HXX

#include <TCollection_AsciiString.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <McCadDiscDs_DiscSolid.hxx>
#include <McCadDiscDs_HSequenceOfDiscSolid.hxx>
#include <McCadCSGGeom_Unit.hxx>

/** This is a container for all neccessary parameters passed to the Void Generation algorithm (VG)
    of McCad. The values will be read from a file parser or from UI.
*/

struct McCadIOHelper_VGParameters{

    McCadIOHelper_VGParameters(){
        boundingBox = new TopTools_HSequenceOfShape();
        inputSolids = new McCadDiscDs_HSequenceOfDiscSolid();
        m_listMatSolids = new TopTools_HSequenceOfShape();
    }

    McCadIOHelper_VGParameters(const McCadIOHelper_VGParameters& parameters){
        *this = parameters;
    }

    McCadIOHelper_VGParameters& operator=(const McCadIOHelper_VGParameters& parameters){
        if( this != &parameters ){
            boundingBox = new TopTools_HSequenceOfShape;
            inputSolids = new McCadDiscDs_HSequenceOfDiscSolid;

            m_listMatSolids = new TopTools_HSequenceOfShape();

            writeCollisionFile = parameters.writeCollisionFile;
            writeVoxelFile = parameters.writeVoxelFile;

            minInputSolidVolume          = parameters.minInputSolidVolume;
            minSizeOfDecompositionFace   = parameters.minSizeOfDecompositionFace;
            minSizeOfRedecompositionFace = parameters.minSizeOfRedecompositionFace;
            minVoidVolume                = parameters.minVoidVolume;
            tolerance                    = parameters.tolerance;

            initCellNumber               = parameters.initCellNumber;
            initSurfaceNumber            = parameters.initSurfaceNumber;
            maxNumberOfPreDecompositionCells = parameters.maxNumberOfPreDecompositionCells;
            xResolution                  = parameters.xResolution;
            yResolution                  = parameters.yResolution;

            maxNumberOfComplementCells   = parameters.maxNumberOfComplementCells;
            maxSamplePoints              = parameters.maxSamplePoints;
            minSamplePoints              = parameters.minSamplePoints;

            inputFileName                = parameters.inputFileName;
            materialDensityFileName      = parameters.materialDensityFileName;
            directories                  = parameters.directories;
            boundingBoxName              = parameters.boundingBoxName;

            boundingBox->Append(parameters.boundingBox);
            inputSolids->Append(parameters.inputSolids);

            m_listMatSolids->Append(parameters.m_listMatSolids);

            units = parameters.units;
        }

        return *this;
    }

    bool writeCollisionFile;
    bool writeVoxelFile;

    double minInputSolidVolume;
    double minSizeOfDecompositionFace;
    double minSizeOfRedecompositionFace;
    double minVoidVolume;
    double tolerance;

    unsigned int initCellNumber;
    unsigned int initSurfaceNumber;
    unsigned int maxNumberOfPreDecompositionCells;
    unsigned int xResolution;
    unsigned int yResolution;

    unsigned short maxNumberOfComplementCells;
    unsigned short maxSamplePoints;
    unsigned short minSamplePoints;

    TCollection_AsciiString inputFileName;
    TCollection_AsciiString materialDensityFileName;
    TCollection_AsciiString directories;
    TCollection_AsciiString boundingBoxName;

    Handle_TopTools_HSequenceOfShape boundingBox;
    Handle_McCadDiscDs_HSequenceOfDiscSolid inputSolids;

    Handle_TopTools_HSequenceOfShape m_listMatSolids; // Add by Lei, 13/08/2012

    McCadCSGGeom_Unit units;
};

#endif // MCCADIOHELPER_VGPARAMETERS_HXX
