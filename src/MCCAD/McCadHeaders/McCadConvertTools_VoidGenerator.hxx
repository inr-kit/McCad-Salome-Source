#ifndef _McCadConvertTools_VoidGenerator_HeaderFile
#define _McCadConvertTools_VoidGenerator_HeaderFile

#include <Handle_McCadDiscDs_HSequenceOfDiscSolid.hxx>
#include <McCadCSGGeom_HSequenceOfCellPtr.hxx>
#include <McCadCSGGeom_Unit.hxx>
#include <McCadDiscDs_HSequenceOfDiscSolidPtr.hxx>
#include <McCadEXPlug_ExchangePluginPtr.hxx>
#include <McCadEXPlug_PluginManagerPtr.hxx>
#include <McCadIOHelper_VGParameters.hxx>
#include <McCadMDReader_Reader.hxx>

#include <Handle_TopTools_HSequenceOfShape.hxx>
#include <Standard_Integer.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Shape.hxx>

#include <McCadGUI_ProgressDialogPtr.hxx>
#include <McCadGUI_ProgressDialog.hxx>
#include <Message_ProgressIndicator.hxx>

//! \brief Master Class for the void completion

//! Purpose: The VoidGenerator takes as Input the sign-constant decomposed CAD-Model. <br>
//! It uses the faces of the sign-constant model to slice up a bounding box. <br>
//! The resulting Boxes are the potential void cells. Overlap tests allow <br>
//! to delete redundant void cells and to finalize the void cells by using the <br>
//! complement operator of the MC-Code selected. <br>
//! <br>
//! One can pass a Parameter-File to this class which contains the following information: <br>
//! <br>
//!	# Example: McCadInputFile.txt {comments begin with a '#'} <br>
//! WriteCollisionFile   1 <br>
//! WriteDiscreteModel   1 <br>
//! Units	{MM | CM}  # defines the used units in the input model... <br>
//! InitSurfNb  1000   # defines the initial surface number for the output model <br>
//! InitCellNb  1000   # defines the initial cell number for the output model <br>
//! MinimumInputSolidVolume  1.00 <br>
//! MinimumVoidVolume  1.25e2 <br>
//! MinimumSizeOfDecompositionFaceArea   50 <br>
//! MinimumSizeOfRedecompositionFaceArea  5 <br>
//! MaximumNumberOfPreDecompositionCells 500 <br>
//! MaximumNumberOfComplementedCells 10 <br>
//! MinimumNumberOfSamplePoints 10 <br>
//! MaximumNumberOfSamplePoints 50 <br>
//! XResolution  50 <br>
//! YResolution  50 <br>
//! Tolerance  1e-7 <br>
//! # Bounding Box as step file <br>
//! BoundingBox  /home/user/Testarea/Bounding_Box/BB.stp <br>
//! # Material List <br>
//! MaterialList /home/user/Testarea/MaterialList.txt <br>
//! # directories of converted files <br>
//! /home/user/Testarea/stp/ <br>
//! <br>
//! \e WriteCollisionFile {0|1} : makes the VoidGenerator write (1) or read (0) the results of a collision <br>
//! detection between input solids. The collision information will be stored in the file .CollisionFile <br>
//! in the working directory. <br>
//! \e WriteDiscreteModel {0|1} : write (1) or read (0) the voxel files for each solid. For each face of each <br>
//! solid sample points will be generated for a latter overlapp test. This takes a while. If the sample <br>
//! points have been generated once they can fastly be read in a following run. If this parameter is set <br>
//! to cero but no '.voxel' files can be found the voxelization will take place, but the voxel files will <br>
//! not be written. <br>
//! Voxel files will be searched in the subdirectory voxels. <br>
//!	\e MinimumInputSolid {real} : defines the minimum volume of an input solid. Each solid with a <br>
//! smaller volume will be ignored and do not appear in th final MC model. <br>
//! \e MinimumVoidVolume {real} : defines the minimum volume for void solids in the cad system. <br>
//! this assures that the number of generated potential void cells is not too big. <br>
//! \e MinimumSizeOfDecompositionFaceArea {real} : For the void generation a bounding box is split <br>
//! by all planar, sign-changing faces of the solid model. This parameter defines a minimum size for the <br>
//! splitting planes. <br>
//! MinimumSizeOfRedecompositionFaceArea {real} : if a void volume collides with too many solid volumes <br>
//! the void volume will be decomposed further. This value defines the minimum face area for the second <br>
//! cut. <br>
//! \e MaximumNumberOfPreDecompositionCells {integer} : sets an upper limit for the number of potential <br>
//! void cells after the first decomposition <br>
//! \e MaximumNumberOfComplementedCells {integer} : McCad makes use of the complement operator to exclude <br>
//! colliding solid volumes from void volumes. This parameter defines the maximum number of allowd collisions. <br>
//! If the number of collisions is exceeded, the void will be further decomposed with planar faces with the <br>
//! minimum size MinimumSizeOfRedecompositionFaceArea <br>
//! \e MinimumNumberOfSamplePoints {integer} , \e MaximumNumberOfSamplePoints {integer} : defines the bounds <br>
//! for the number of sample points along one edge of a solids face. The higher the values, the more sample <br>
//! points will be generated and the less the possibility for an undetected collision is. But this also <br>
//! affects the computation time and, very important for big models,the usage of memory. <br>
//! \e XResolution {real}, \e YResolution {real} : space between sample points on edges along x,y direction <br>
//! in local coordinate system (in mm). The smaller the resolution, the more sample points will be generated. <br>
//! \e BoundingBox {path to a bounding box as step file} : defines a bounding box for the void generation <br>
//! if no bounding box is defined, an axis parallel BB will be generated. This might lead to more void cells <br>
//! than necessary. <br>
//! \e MaterialList {path to a material list} : this is a MCNP feature only. The file list contains a list <br>
//! of solid names and a corresponding material number and density: <br>
//! <br>
//! 		Example: # name		materialNumber	Density <br>
//!					  solid1    1				-0.89 <br>
//! <br>
//! If a line is not a comment (#) and not introduced by a codeword, it will be interpreted as path to a <br>
//! directory which contains all converted (McCadConvertTools_Convertor) files of the current model to be <br>
//! converted. All files 'converted*.stp' will be added. <br>

class McCadConvertTools_VoidGenerator {
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

    // Methods

    //! ctor
    Standard_EXPORT McCadConvertTools_VoidGenerator();

    //! ctor with parameter input
    Standard_EXPORT McCadConvertTools_VoidGenerator( McCadIOHelper_VGParameters& parameters);

    //! initialize internal parameters
    void Init() ;

    //! Expand bounding box, so that addBB does fit in.
    void AddBoundingBox(Handle_TopTools_HSequenceOfShape& addBB) ;

    //! Main call to generate the voids. This method starts the voxelization and the decomposition <br>
    //! of the bounding box into potential void volumes.
    Standard_Boolean GenerateVoids() ;

    //! Return the Voxelized predecomposed void volumes
    Handle_McCadDiscDs_HSequenceOfDiscSolid GetVVoids() ;

    //! Return the voxelized solid volumes
    Handle_McCadDiscDs_HSequenceOfDiscSolid GetVSolids() ;

    //! Return the voxelized outer voids. In MCNP all universe has to be defined. McCad generates a sphere <br>
    //! around the bounding box. Everything inside the sphere, but outside the bouning box is the one outer void  <br>
    //! with importance 1. Everything outside of the Sphere has importance 0 and is the infinite outer wordl. <br>
    //! This function returns the sphere which will be used in the McCadMcWrite_McInputGenerator classes to generate <br>
    //! the outer world.
    Handle_McCadDiscDs_HSequenceOfDiscSolid GetVOuterVoids() ;

    //! If no Bounding Box has been passed, create one.
    Standard_Boolean MakeBoundingBox() ;

    //! Check for small volumes to delete and call GenerateVoids()
    Standard_Boolean Perform() ;

    //! Return the defined units {McCadCSGGeom_MM, McCadCSGGeom_CM}
    McCadCSGGeom_Unit GetUnits();

    //! Return the initial surface number of the MC file to be generated
    Standard_Integer  GetInitSurfNb();

    //! Return the initial Volume (solid) number of the MC file to be generated
    Standard_Integer  GetInitCellNb();

    //! Initialize with a ParameterFile
    Standard_Boolean Init( McCadIOHelper_VGParameters& parameters) ;

    void SetProgressIndicator(Handle_Message_ProgressIndicator& thePI);

    //! Add discretized solids to VSolids
    void AddDiscSolids(Handle_McCadDiscDs_HSequenceOfDiscSolid& theNamedSolids);

    //! Define the Material Density Reader. MCNP only. The MD-Reader will read the MaterialList <br>
    //! and links a solid name to a material composition.
    void SetMDReader(McCadMDReader_Reader& theMDReader);

    //! Return the MDReader, see SetMDReader()
    McCadMDReader_Reader& GetMDReader( );

    //! Sets the parameter
    void SetParameters( McCadIOHelper_VGParameters& parameters );

    private:

     // Methods
    //! Test the input solids for collisions. Save collisions in .CollisionFile in the working directory.
    void CheckInputCollision() ;

    //! delete small volumes due to the Parameter \e MinimumInputSolidVolume
    void DeleteSmallVolumes() ;

    //! Discretize the Input Solids
    void Voxelize() ;

    // Fields
    bool myFirstFile;

    Handle_McCadDiscDs_HSequenceOfDiscSolid myVOuterVoids;
    Handle_McCadDiscDs_HSequenceOfDiscSolid myVSolids;
    Handle_McCadDiscDs_HSequenceOfDiscSolid myVVoids;

    McCadIOHelper_VGParameters m_parameters;
    McCadMDReader_Reader myMDReader;

    TopoDS_Shape myOuterSpace;

    //For Progress Visualization
    McCadGUI_ProgressDialogPtr myProgress;
    Handle_Message_ProgressIndicator m_progressIndicator;
};

#endif
