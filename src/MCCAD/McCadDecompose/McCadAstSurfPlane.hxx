#ifndef MCCADASTSURFPLANE_HXX
#define MCCADASTSURFPLANE_HXX

#include "McCadAstSurface.hxx"
#include "McCadEdgeLine.hxx"

#include "gp_Dir.hxx"
#include "TopoDS_Edge.hxx"

class McCadAstSurfPlane : public McCadAstSurface
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

    McCadAstSurfPlane(const TopoDS_Face &theFace);
    ~McCadAstSurfPlane();

public:

    //Standard_Boolean FaceCollision(McCadBndSurface *& pBndFace, POSITION & eSide);
    /**< Virtual function: detect the position relationship between triangle and face */
    virtual Standard_Boolean TriangleCollision(McCadTriangle *& triangle, POSITION & eSide);

    //Standard_Boolean CanbeCombined(McCadAstSurfPlane *pFace);   /**< The two surfaces can be combined or not */
    McCadAstSurfPlane * CombSurf(McCadAstSurfPlane *pFace);     /**< Return the combined surface */
    gp_Dir GetDir();                                            /**< Get the normal direction of plane */

    void SetEdge(TopoDS_Edge edge); /**< Set the edge which are throughed by the assisted splitting surface.*/
    McCadEdgeLine *GetEdge() const;       /**< Get the edge which are throughed by the assisted splitting surface.*/

    Standard_Boolean CanbeCombined();           /**< This assisted surface has been merged, it will not be merged again */
    void SetCombined(Standard_Boolean bMerged); /**< Set the surface can be combined or not */

    virtual Standard_Boolean IsPntOnSurf(gp_Pnt &thePnt, Standard_Real disTol);/**< The point is on surface or not */

private:

    gp_Dir m_Dir;                   /**< The normal direction of assisted plane */
    McCadEdgeLine * m_pEdgeLine;        /**< The edge which the assisted surface goes through */
    Standard_Boolean m_bCombined;   /**< The symble of surface if it is a merged surface */

};

#endif // MCCADASTSURFPLANE_HXX
