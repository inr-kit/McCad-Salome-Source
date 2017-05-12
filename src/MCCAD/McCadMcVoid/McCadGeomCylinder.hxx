#ifndef MCCADGEOMCYLINDER_HXX
#define MCCADGEOMCYLINDER_HXX

#include "IGeomFace.hxx"
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>

class McCadGeomCylinder : public IGeomFace
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

    McCadGeomCylinder();
    McCadGeomCylinder(const GeomAdaptor_Surface & theSurface);
    virtual ~McCadGeomCylinder();

public:
    virtual TCollection_AsciiString GetExpression();            /**< Calculate the equation and direction of plane */
    virtual Standard_Boolean IsEqual(IGeomFace *& theGeoFace);  /**< Judge the two cylinder are same or not */
    virtual void CleanObj() const;                              /**< Clean the generated objects */
    virtual Standard_Boolean Compare(const IGeomFace *& theGeoFace) ;    /**< Compare the priority for sorting */

    virtual TCollection_AsciiString GetTransfNum() const;

    Standard_Real GetRadius() const;                            /**< Get radius */
    gp_Pnt GetPoint() const;                                    /**< Get point on axis */
    gp_Dir GetAxisDir() const;                                  /**< Get direction of axis */
    void SimplifyPrmt(Standard_Real &thePrmt);                  /**< Simplify the coefficients */

private:

    gp_Dir m_Dir;                                               /**< Direction of a axis */
    gp_Ax3 m_Axis;                                              /**< Axis of cylinder */
    gp_Pnt m_Point;                                             /**< A point on axis */
    Standard_Real m_Radius;                                     /**< Radius of section */
    gp_Vec m_vec;

    Standard_Real m_dPrmtA1;                     /**< Parameter A1 of plane equation */
    Standard_Real m_dPrmtA2;                     /**< Parameter A2 of plane equation */
    Standard_Real m_dPrmtA3;                     /**< Parameter A3 of plane equation */
    Standard_Real m_dPrmtB1;                     /**< Parameter B1 of plane equation */
    Standard_Real m_dPrmtB2;                     /**< Parameter B2 of plane equation */
    Standard_Real m_dPrmtB3;                     /**< Parameter B3 of plane equation */
    Standard_Real m_dPrmtC1;                     /**< Parameter C1 of plane equation */
    Standard_Real m_dPrmtC2;                     /**< Parameter C2 of plane equation */
    Standard_Real m_dPrmtC3;                     /**< Parameter C3 of plane equation */
    Standard_Real m_dPrmtD;                      /**< Parameter D of plane equation  */
};

#endif // MCCADGEOMCYLINDER_HXX




