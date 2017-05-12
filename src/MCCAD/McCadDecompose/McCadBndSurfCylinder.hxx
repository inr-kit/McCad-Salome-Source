#ifndef MCCADBNDFACECYLINDER_HXX
#define MCCADBNDFACECYLINDER_HXX

#include "McCadBndSurface.hxx"
#include <Bnd_Box.hxx>

class McCadBndSurfCylinder : public McCadBndSurface
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

    McCadBndSurfCylinder();
    McCadBndSurfCylinder(const TopoDS_Face & theFace);
    ~McCadBndSurfCylinder();

public:

    /**< Detect the collision between cylinder and triangle */
    virtual Standard_Boolean TriangleCollision(McCadTriangle *& triangle, POSITION & eSide);
    void GenExtCylinder(Standard_Real length);      /**< Set the length of cylinder */

    Standard_Boolean HasAstSplitSurfaces();         /**< The cylinder has assisted splitting surface or not */

    Standard_Boolean HasCylnPlnSplitSurf();         /**< Has assisted splitting surface for separating cylinder and plane */
    void AddCylnCylnSplitEdge(McCadEdge *& pEdge);  /**< Add the edge connects cylinders for adding assited splitting surface */
    void AddCylnPlnSplitEdge(McCadEdge *& pEdge);   /**< Add the edge connects cylinder and plane for adding assited splitting surface */

    vector<McCadEdge*> GetCylnPlnSplitEdgeList() const;     /**< Get the splitting edge list */
    Standard_Real GetRadian() const;                        /**< Get the radian of cylinder */
    gp_Pnt GetCenter() const;                               /**< Get the center of cylinder */

    virtual Standard_Boolean CanFuseSurf(McCadBndSurface *& pSurf); /**< The two cylinders can be fused or not*/
    virtual TopoDS_Face FuseSurfaces(McCadBndSurface *& pSurf);     /**< Fuse the cylinders and generate a new one */  
    virtual Standard_Boolean IsPntOnSurf(gp_Pnt &thePnt, Standard_Real disTol); /**< The point is on surface or not */

private:

    /**< Detect the collision between cylinder and triangle with Boolean operation*/
    Standard_Boolean BooleanCollisionDetect(McCadTriangle *& triangle, POSITION &eSide);

    /**< If there are one vertex locates at the outside of cylinder,
         detect the collision between cylinder and triangle. */
    Standard_Boolean CollisionDetectOnePosPoint(McCadTriangle *& triangle,POSITION & eSide);
    Standard_Real m_radius;         /**< The radius of cylinder */
    TopoDS_Face m_cylSurf;          /**< Generated a close and extended cylinder surface */
    Standard_Real m_CylnLength;     /**< The length of cylinder generated */

    /**< Detect the collision between cylinder and triangle with intersection operations*/
    Standard_Boolean FurtherCollisionDetect(McCadTriangle *& triangle,
                                            POSITION &eSide,
                                            Standard_Integer posPnt);

    Standard_Boolean m_bHasAstSplitSurfaces;/**< If the cylinder has assisted splitting surface */
    Bnd_Box m_CylnBndBox;                   /**< The boundary box of whole cylinder */

    /**< The list of edge that connect cylinder and plane and can be added splitting surface */
    vector<McCadEdge*> m_CylnPlnSplitEdgeList;
    /**< The list of edge that connect cylinders can be added splitting surface */
    vector<McCadEdge*> m_CylnCylnSplitEdgeList;

    Standard_Real m_Radian;                     /**< The radian of cylinder */
    gp_Pnt m_Center;                            /**< The center of cylinder, namely one point on the axis */
};

#endif // MCCADBNDFACECYLINDER_HXX
