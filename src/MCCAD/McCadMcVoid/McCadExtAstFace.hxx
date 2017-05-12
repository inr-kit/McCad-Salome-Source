#ifndef MCCADEXTASTFACE_HXX
#define MCCADEXTASTFACE_HXX

#include "McCadExtFace.hxx"

class McCadExtAstFace : public McCadExtFace
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

    McCadExtAstFace();
    McCadExtAstFace(const TopoDS_Face &theFace);
    McCadExtAstFace(const McCadExtAstFace &theExtFace);
    ~McCadExtAstFace();

private:

    Standard_Boolean m_bSplit;  /**< Judge if the assisted face can cut the solid into two parts */

public:

    void SetSplit(Standard_Boolean bSplit);     /**< Add split attribute to the assisted face */
    Standard_Boolean IsSplitFace();             /**< Get split attribute of the assisted face */   
};

#endif // MCCADEXTASTFACE_HXX
