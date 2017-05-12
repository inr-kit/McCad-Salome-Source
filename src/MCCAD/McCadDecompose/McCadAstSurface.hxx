#ifndef MCCADASTSURFACE_HXX
#define MCCADASTSURFACE_HXX

#include "McCadSurface.hxx"
#include "McCadBndSurface.hxx"

class McCadAstSurface : public McCadSurface
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

    McCadAstSurface(const TopoDS_Face &theFace);
    virtual ~McCadAstSurface();

    /**< Collison dectection */
    virtual Standard_Boolean FaceCollision(McCadBndSurface *& pBndFace, POSITION & ePosition);
    /**< Virtual function: detect the position relationship between triangle and face */
    virtual Standard_Boolean TriangleCollision(McCadTriangle *& pTri, POSITION & eSide) = 0;

protected:


};

#endif // MCCADASTSURFACE_HXX
