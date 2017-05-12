#ifndef MCCADGEOMTORUS_HXX
#define MCCADGEOMTORUS_HXX

#include "IGeomFace.hxx"

#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>

class McCadGeomTorus : public IGeomFace
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

    McCadGeomTorus();
    McCadGeomTorus(const GeomAdaptor_Surface & theSurface);
    virtual ~McCadGeomTorus();

    virtual TCollection_AsciiString GetExpression();            /**< Get the expression of torus */
    virtual Standard_Boolean IsEqual(IGeomFace *& theSurf);     /**< Judge the two torus is same or not */
    virtual void CleanObj() const;                              /**< Clean the generated objects */
    virtual Standard_Boolean Compare(const IGeomFace *& theGeoFace) ;  /**< Compare the priority for sorting */
    virtual TCollection_AsciiString GetTransfNum() const;       /**< Get the transform card */

    gp_Pnt GetCenter() const;                       /**< Get the center of torus */
    gp_Dir GetAxisDir() const;                      /**< Get the direction of axis */
    Standard_Real GetMajorRadius() const;           /**< Get the major radius */
    Standard_Real GetMinorRadius() const;           /**< Get the minor radius */

private:

    gp_Ax3 m_Axis;                                  /**< The rotate axis of torus */
    gp_Dir m_Dir;                                   /**< The direction of axis */
    Standard_Real m_MajorRadius;                    /**< Major radius */
    Standard_Real m_MinorRadius;                    /**< Minor radius */
    gp_Pnt m_Center;                                /**< Center of torus */
};

#endif // MCCADGEOMTORUS_HXX
