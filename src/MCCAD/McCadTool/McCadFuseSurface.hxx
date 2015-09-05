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
    bool Fuse();                                            /**< Fuse the surfaces */
    void SetSurfaces(TopoDS_Face surfA, TopoDS_Face surfB); /**< Input the surfaces */
    TopoDS_Face GetNewSurf();                               /**< Return the surface after fused */

private:

    TopoDS_Face m_surfA;        /**< SurfaceA to be fused */
    TopoDS_Face m_surfB;        /**< SurfaceB to be fused */
    TopoDS_Face m_newSurf;      /**< New surface after fused */

    bool CanbeFused();          /**< Judge the two surfaces satisfy the conditions of fuse */
    bool IsSameEdge(TopoDS_Edge & edgeA, TopoDS_Edge & edgeB);  /**< The surfaces have common edge.*/
    TopoDS_Face FuseSurfaces(); /**< Fuse the two surfaces, use the UV boundaries */
    TopoDS_Wire MakeWire(vector<TopoDS_Edge> &edge_list); /**< Connect the edges and make a wire*/

    /**< Judge the two surfaces have same geometries */
    bool IsSameSurfaces(GeomAdaptor_Surface &SurfA, GeomAdaptor_Surface &SurfB);
    bool HasCommonEdge(GeomAdaptor_Surface &SurfA, GeomAdaptor_Surface &SurfB);

};

#endif // MCCADFUSESURFACE_HXX
