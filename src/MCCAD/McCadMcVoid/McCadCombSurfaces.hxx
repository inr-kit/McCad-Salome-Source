#ifndef MCCADSEWSURFACES_HXX
#define MCCADSEWSURFACES_HXX

#include "McCadExtFace.hxx"
#include <Standard.hxx>
#include <GeomAdaptor_Surface.hxx>

class McCadSewSurfaces
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
    McCadSewSurfaces();
    ~McCadSewSurfaces();

public:
    TopoDS_Face Merge(McCadExtFace *& pFaceA,McCadExtFace *& pFaceB );
    Standard_Boolean CanbeMerged(McCadExtFace *& pFaceA,McCadExtFace *& pFaceB);

private:
    Standard_Boolean IsSameSurface(TopoDS_Face &faceA, TopoDS_Face &faceB) const;
    Standard_Boolean HasCommonEdge(TopoDS_Face &faceA, TopoDS_Face &faceB) const;

};

#endif // MCCADSEWSURFACES_HXX
