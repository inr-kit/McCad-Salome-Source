/*
 * McCadCSGTool_FaceFuser.hxx
 *
 *  Created on: Dec 15, 2010
 *      Author: grosse
 */

#ifndef MCCADCSGTOOL_FACEFUSER_HXX_
#define MCCADCSGTOOL_FACEFUSER_HXX_

#include <Standard.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <TopLoc_Location.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgo_Fuse.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <McCadCSGGeom_Surface.hxx>
#include <McCadCSGGeom_Plane.hxx>
#include <McCadCSGGeom_Cylinder.hxx>
#include <McCadCSGGeom_Cone.hxx>
#include <McCadCSGGeom_Sphere.hxx>
#include <McCadCSGGeom_Torus.hxx>
//#include <McCadGUI_Messenger.hxx>


//! This class provides an algorithm to fuse

class McCadCSGTool_FaceFuser
{

public:
	// ctors
	/////////////////
	//! Standard ctor
	McCadCSGTool_FaceFuser();
        /** Initialize _FaceFuser with a Sequence of Objects, that contain faces
            skipForwardOriented is used when only reverse oriented faces should be
            taken into account.
        */
	McCadCSGTool_FaceFuser(Handle_TopTools_HSequenceOfShape& faceSeq, Standard_Boolean skipForwardOriented=Standard_False);
	//! To fuse just two faces
	McCadCSGTool_FaceFuser(TopoDS_Face& f1, TopoDS_Face& f2);


	// public Methods
	////////////////////
        //! returns false if all fusion operations were successful
        Standard_Boolean FusionFailed();
	//! Returns true if fusion has been performed
        Standard_Boolean IsDone();
	//! Return fused faces. If fuse failed for one pair of faces, a warning will be printed.
	Handle_TopTools_HSequenceOfShape GetFused();
	//! Return original faces
	Handle_TopTools_HSequenceOfShape GetOriginal();
	//! Add any kind of shape that includes faces (Solid, Face, Shell,..)
	void AddShapes(Handle_TopTools_HSequenceOfShape& theShapes);
	//! Reset all settings
	void Init();
	//! Test if two faces share the same surface and are neighbours
	Standard_Boolean IsLateral(const TopoDS_Face& f1, const TopoDS_Face& f2);
	//! switch; don't handle Forward oriented faces; <br>
	//! this comes in handy during the search for a boundary expansion for <br>
	//! non-linear solids <br>
	void SkipForwardOriented(Standard_Boolean& state) ;
        //! Master fusion method, calls the slave fusion methods
        void Fuse();

private:

	// Methods
	///////////////

	//! fuses lateral cylinders
	void CylinderFuse(Handle_TopTools_HSequenceOfShape& faces);
	//! fuses lateral cones
	void ConeFuse(Handle_TopTools_HSequenceOfShape& faces);
	//! fuses lateral spheres
	void SphereFuse(Handle_TopTools_HSequenceOfShape& faces);
	//! fuses lateral tori
	void TorusFuse(Handle_TopTools_HSequenceOfShape& faces);
	//! fuses lateral planes
	void PlaneFuse(Handle_TopTools_HSequenceOfShape& faces);
	//! try OCC BOP_Fuse
	Standard_Boolean OCCFuse(Handle_TopTools_HSequenceOfShape& faces);
	//! Check if the boundary of the faces ly on UV-Iso-Lines. <br>
	Standard_Boolean FaceBoundaryOnUVIsoLines(const TopoDS_Face& theFaces) ;

	// Fields
	///////////////

	Standard_Boolean myIsDone;
        Standard_Boolean myFusionFailed;

	Handle_TopTools_HSequenceOfShape myFusedFaces;
	Handle_TopTools_HSequenceOfShape myOriginalFaces;
	GeomAbs_SurfaceType myCurrentSurfaceType;
	Standard_Boolean myUseReverseOrientedOnly;

};



#endif /* MCCADCSGTOOL_FACEFUSER_HXX_ */
