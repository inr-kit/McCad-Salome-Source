#ifndef MCCADSTITCHSURFACES_HXX
#define MCCADSTITCHSURFACES_HXX

#include "McCadExtBndFace.hxx"
#include <Standard.hxx>
#include <GeomAdaptor_Surface.hxx>

class McCadStitchSurfaces
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
    McCadStitchSurfaces();
    ~McCadStitchSurfaces();

public:
    /**< Stitch the two same surfaces */
    TopoDS_Face Stitch(McCadExtBndFace *& pFaceA,McCadExtBndFace *& pFaceB );
    /**< The two surfaces can be stitched or not */
    Standard_Boolean CanbeStitched(McCadExtBndFace *&pFaceA, McCadExtBndFace *&pFaceB);

private:
    /**< The two surface are same surface or not */
    Standard_Boolean IsSameSurface(TopoDS_Face &faceA, TopoDS_Face &faceB) const;
    /**< The two surfaces has common connected edge or not */
    Standard_Boolean HasCommonEdge(TopoDS_Face &faceA, TopoDS_Face &faceB) const;

};

#endif // MCCADSTITCHSURFACES_HXX
