#ifndef MCCADADDASTSURFACE_HXX
#define MCCADADDASTSURFACE_HXX

#include <TopoDS_Face.hxx>
#include <Standard.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include "McCadExtBndFace.hxx"

class McCadAddAstSurface
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

    McCadAddAstSurface();
    ~McCadAddAstSurface();

    /**< Import a curved surface, and generate its assisted surface list*/
    Standard_EXPORT static Standard_Boolean AddAstSurf(const McCadExtBndFace *&pFace,
                                                       Handle_TopTools_HSequenceOfShape &seqResultant);

private:

    /**< Generate the assisted surfaces of a revolution surface */
    static Standard_Boolean GenAstFaceOfRevolution( Handle_TopTools_HSequenceOfShape &seqResultant,
                                                    const McCadExtBndFace *&pFace);
    /**< Generate the assisted surfaces of a tori */
    static Standard_Boolean GenAstFaceOfTorus(  Handle_TopTools_HSequenceOfShape &seqResultant,
                                                const McCadExtBndFace *& pFace);
    /**< Generate the assisted surfaces of a cylinder or cone or sphere */
    static Standard_Boolean GenericAstFace(Handle_TopTools_HSequenceOfShape & seqResultant,
                                           const McCadExtBndFace *&pFace);

};

#endif // MCCADADDASTSURFACE_HXX
