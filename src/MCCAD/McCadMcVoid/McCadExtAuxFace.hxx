#ifndef MCCADEXTAUXFACE_HXX
#define MCCADEXTAUXFACE_HXX

#include "McCadExtFace.hxx"

class McCadExtAuxFace : public McCadExtFace
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

    McCadExtAuxFace();
    McCadExtAuxFace(const TopoDS_Face &theFace);
    McCadExtAuxFace(const McCadExtAuxFace &theExtFace);
    ~McCadExtAuxFace();

private:

    Standard_Integer m_iConvexity;  /**< Judge if the auxiliary face can cut the solid into two parts */

public:

    void SetConvexity(Standard_Integer m_iConvexity);   /**< Add convexity to the auxiliary face */
    Standard_Integer GetConvexity();                    /**< Get convexity of the auxiliary face */

};

#endif // MCCADEXTAUXFACE_HXX
