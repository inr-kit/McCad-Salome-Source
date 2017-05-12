#ifndef MCCADGEOMTOOL_HXX
#define MCCADGEOMTOOL_HXX

#include <Handle_TColgp_HSequenceOfPnt.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Handle_TopTools_HSequenceOfShape.hxx>

#include <Standard.hxx>

class McCadGeomTool
{

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

public:
    McCadGeomTool();

    /**< Judge the surfaces are same or not */
    Standard_EXPORT static Standard_Boolean IsSameSurfaces(const TopoDS_Face &SurfA,
                                                           const TopoDS_Face &SurfB);

    /**< Create the sample points on a face */
    Standard_EXPORT static Handle_TColgp_HSequenceOfPnt GetFaceSamplePnt(const TopoDS_Face& theFace);
    /**< Create the sample points on a wire frame of given face */
    Standard_EXPORT static Handle_TColgp_HSequenceOfPnt GetWireSamplePnt(const TopoDS_Face& theFace);

    /**< Fuse two cylinders which are connected and have same geometries */
    Standard_EXPORT static TopoDS_Face FuseCylinders(TopoDS_Face &faceA, TopoDS_Face &faceB);
    /**< Fuse two cylinders which are connected and have same geometries */
    Standard_EXPORT static TopoDS_Face FusePlanes(TopoDS_Face &faceA, TopoDS_Face &faceB);
    /**< Fuse two Torus which are connected and have same geometries */
    Standard_EXPORT static TopoDS_Face FuseTorus(TopoDS_Face &faceA, TopoDS_Face &faceB);


    /**< Calculate the volume of a solid */
    Standard_EXPORT static Standard_Real GetVolume(const TopoDS_Shape &theShape);
    /**< Create the sample points on a edge */
    static void DiscreteEdge(   const TopoDS_Edge& theEdge,
                                const TopoDS_Face& theFace,
                                const GeomAdaptor_Surface & theSurf,
                                Handle_TColgp_HSequenceOfPnt & thePntSeq);


    /**< Fuse two cylinders which are connected and have same geometries */
    Standard_EXPORT static Standard_Boolean IsSameEdge(const TopoDS_Edge &edgeA,
                                                       const TopoDS_Edge &edgeB,
                                                       Standard_Real dis);

    Standard_EXPORT static gp_Dir NormalOnFace(const TopoDS_Face& F, const gp_Pnt& P);

    /**< Increase the number of sample points */
    static void ScaleSampleNum(int iCycle, int &xNum, int &yNum );
    /**< Remove the repeated points */
    static void RemoveRepeatPnt(Handle_TColgp_HSequenceOfPnt & thePntList);
    /**< According the input parameters for laying points on X,Y directions,
     *   adjust the number of sample points on a face*/
    static Standard_Integer AdjustSampleNum(Standard_Real dLen, Standard_Real dPreDefLen);
    /**< According the input parameters for laying points on X,Y directions,
     *   adjust the number of sample points on a edge*/
    static Standard_Integer AdjustEdgeSampleNum(Standard_Real dLen, Standard_Real dPdfLen);

    /**< Remove the redundant digitals of the coordinations of input points */
    static void SimplifyPoint(gp_Pnt &pnt3d);

private:

    /**< Judge the input two edges are same straight edges or not */
    static Standard_Boolean IsSameLineEdge(const TopoDS_Edge & edgeA,
                                                               const TopoDS_Edge & edgeB,
                                                               Standard_Real dis);
    /**< Judge the input two edges are same ellipse circle edges or not */
    static Standard_Boolean IsSameEllipseEdge(const TopoDS_Edge & edgeA,
                                                              const TopoDS_Edge & edgeB,
                                                              Standard_Real dis);
    /**< Judge the input two edges are same circle edges or not */
    static Standard_Boolean IsSameCircleEdge(const TopoDS_Edge &edgeA,
                                             const TopoDS_Edge &edgeB,
                                             Standard_Real dis);

private:

    static Standard_Real m_MaxNbPnt;    /**< The maximum number of points on one direction for discreting */
    static Standard_Real m_MinNbPnt;    /**< The minimum number of points on one direction for discreting */
    static Standard_Real m_Xlen;        /**< The resolution of X direction */
    static Standard_Real m_Ylen;        /**< The resolution of Y direction */
    static Standard_Real m_Rlen;        /**< The resolution of R direction */
    static Standard_Real m_Tolerance;   /**< Tolerance of the minimum distance */

    static void SetPrmt();              /**< Set the parameters */

};

#endif // MCCADGEOMTOOL_HXX
