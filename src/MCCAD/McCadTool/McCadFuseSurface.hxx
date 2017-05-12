/** @file McCadFuseSurface.hxx
  *
  * @brief  The class implements the fuse of two surfaces in solid with same geometries
  *         and common edge, the fuse of these surfaces will not generate the assisted
  *         surfaces for two separated surfaces respectively, but only one to the surface
  *         after fused.
  *
  * @author Lei Lu
  * @date   4st.Feb.2014
  */

#ifndef MCCADFUSESURFACE_HXX
#define MCCADFUSESURFACE_HXX

#include <vector>

#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>

#include <GeomAdaptor_Surface.hxx>

using namespace std;

class McCadFuseSurface
{
public:
    McCadFuseSurface();
    ~McCadFuseSurface();


public:
    bool IsSame();                                          /**< The two surfaces are same or not */
    //bool Fuse();                                           /**< Fuse the surfaces */
    void SetSurfaces(TopoDS_Face surfA, TopoDS_Face surfB); /**< Input the surfaces */
    TopoDS_Face GetNewSurf();                               /**< Return the surface after fused */

    TopoDS_Face GetCombSurf();

    bool FindCutEdge(TopoDS_Edge &edge);                    /**< Find the edge through the two fused surfaces */
    bool CombineSurfs();                                    /**< Combine two surfaces, create a new surface */
    bool FuseCylinders();                                   /**< Fuse the surfaces */

private:

    TopoDS_Face m_surfA;        /**< SurfaceA to be fused */
    TopoDS_Face m_surfB;        /**< SurfaceB to be fused */
    TopoDS_Face m_newSurf;      /**< New surface after fused */
    TopoDS_Face m_combSurf;     /**< New surface after combined */

    TopoDS_Wire m_newWire;      /**< New surface after fused */

    /**< Judge the two surfaces satisfy the conditions of fuse */
    bool CanbeFused();
    /**< Judge the two planes satisfy the conditions of combination */
    bool CanbeCombined();
    /**< The surfaces have common edge.*/
    bool IsSameEdge(TopoDS_Edge & edgeA, TopoDS_Edge & edgeB);
    /**< Fuse the two surfaces, use the UV boundaries */
    TopoDS_Face FuseSurfaces();
    /**< Connect the edges and make a wire*/
    TopoDS_Wire MakeWire(vector<TopoDS_Edge> edge_list);

    /**< Judge the two surfaces have same geometries */
    bool IsSameSurfaces(GeomAdaptor_Surface &SurfA, GeomAdaptor_Surface &SurfB);
    /**< Judge the surfaces have common edge or not */
    bool HasCommonEdge(GeomAdaptor_Surface &SurfA, GeomAdaptor_Surface &SurfB);
    /**< Judge the surfaces have common edge or not and return the edge */
    bool FindCommonEdge(GeomAdaptor_Surface &SurfA,
                        GeomAdaptor_Surface &SurfB,
                        TopoDS_Edge &edge);

    /**< Create a new surface which combine the two surfaces */
    TopoDS_Face CrtNewSurf();

    TopoDS_Face FuseTorus();

    /**< Joint the two edges with same geometries to one edge */
    Standard_Boolean JointEdges(TopoDS_Edge &edgeA,TopoDS_Edge &edgeB, TopoDS_Edge &new_edge);
    /**< Compare the vertex of two edges and connect them */
    Standard_Boolean CompareVertex(TopoDS_Edge &edgeA,
                                   TopoDS_Edge &edgeB,
                                   gp_Pnt &posVexA,
                                   gp_Pnt &posVexB);

};

#endif // MCCADFUSESURFACE_HXX
