#ifndef MCCADGEOMPLANE_HXX
#define MCCADGEOMPLANE_HXX

#include "IGeomFace.hxx"
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>

class McCadGeomPlane : public IGeomFace
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

    McCadGeomPlane();
    McCadGeomPlane(const GeomAdaptor_Surface & theSurface);

    virtual ~McCadGeomPlane();
    virtual TCollection_AsciiString GetTransfNum() const;            /**< Get the transformation card number */

    virtual TCollection_AsciiString GetExpression();                 /**< Calculate the equation and direction of plane */
    virtual Standard_Boolean IsEqual(IGeomFace *& theGeoFace);       /**< Judge the two planes are same or not */
    virtual void CleanObj() const;                                   /**< Clean the generated objects */
    virtual Standard_Boolean Compare(const IGeomFace *& theGeoFace); /**< Clean the object */

    Standard_Real GetPrmtD() const;                                  /**< Get parameter D */
    gp_Dir GetDir() const;                                           /**< Get the parameter of surface */

private:

    gp_Ax3 m_Axis;                              /**< Coordinate system of plane */
    gp_Dir m_Dir;                               /**< Direction of a plane */

    // the equation of plane is ax + by + cz + d = 0
    Standard_Real m_dPrmtA;                     /**< Parameter A of plane equation */
    Standard_Real m_dPrmtB;                     /**< Parameter B of plane equation */
    Standard_Real m_dPrmtC;                     /**< Parameter C of plane equation */
    Standard_Real m_dPrmtD;                     /**< Parameter D of plane equation */



};

#endif // MCCADGEOMPLANE_HXX
