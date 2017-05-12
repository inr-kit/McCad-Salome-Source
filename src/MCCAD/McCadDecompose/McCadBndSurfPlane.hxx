#ifndef MCCADBNDSURFPLANE_HXX
#define MCCADBNDSUTRFPLANE_HXX

#include "McCadBndSurface.hxx"

class McCadBndSurfPlane : public McCadBndSurface
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

    McCadBndSurfPlane();
    McCadBndSurfPlane(const TopoDS_Face &theFace);
    ~McCadBndSurfPlane();


public:

    void GenExtPlane(Standard_Real length);      /**< Set the length of cylinder */
    /**< Detect the collision between plane and triangle */
    virtual Standard_Boolean TriangleCollision(McCadTriangle *&triangle, POSITION & eSide);
    /**< The two planes can be fused or not*/
    virtual Standard_Boolean CanFuseSurf(McCadBndSurface *& pSurf);
    /**< Fuse the planes and generate a new one */
    virtual TopoDS_Face FuseSurfaces(McCadBndSurface *& pSurf);
    virtual Standard_Boolean IsPntOnSurf(gp_Pnt &thePnt, Standard_Real disTol); /**< The point is on surface or not */

private:

    Standard_Real m_ExtLength;     /**< The length of plane generated */
    TopoDS_Face m_ExtSurf;         /**< Generated a extended plane surface */


};

#endif // MCCADBNDSUFPLANE_HXX
