#ifndef MCCADGEOMSPHERE_HXX
#define MCCADGEOMSPHERE_HXX

#include "IGeomFace.hxx"

#include <GeomAdaptor_Surface.hxx>

class McCadGeomSphere : public IGeomFace
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

    McCadGeomSphere();    
    McCadGeomSphere(const GeomAdaptor_Surface & theSurface);
    virtual ~McCadGeomSphere();

    virtual TCollection_AsciiString GetExpression();                /**< Get the expression of surface */
    virtual Standard_Boolean IsEqual(IGeomFace *& theSurf);         /**< The two surfcce are same or not */
    virtual void CleanObj() const;                                  /**< Clean the generated objects */
    virtual Standard_Boolean Compare(const IGeomFace *& theGeoFace);/**< Clean the object */
    virtual TCollection_AsciiString GetTransfNum() const;           /**< Get the transform card */

    gp_Pnt GetCenter() const;                                       /**< Get the center of sphere */
    Standard_Real GetRadius() const;                                /**< Get the radius of sphere */

private:
    Standard_Real m_Radius;         /**< The radius of sphere */
    gp_Pnt m_Center;                /**< The center of sphere */

};

#endif // MCCADGEOMSPHERE_HXX
