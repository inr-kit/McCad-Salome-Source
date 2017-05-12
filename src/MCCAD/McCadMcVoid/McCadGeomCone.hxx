#ifndef MCCADGEOMCONE_H
#define MCCADGEOMCONE_H

#include "IGeomFace.hxx"
#include "McCadGeomData.hxx"

#include <GeomAdaptor_Surface.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>

class McCadVoidCellManager;
class McCadGeomCone : public IGeomFace
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

    McCadGeomCone();
    McCadGeomCone(const GeomAdaptor_Surface & theSurface);
    virtual ~McCadGeomCone();

public:
    virtual void CleanObj() const;                              /**< Clean the generated objects */

    virtual TCollection_AsciiString GetExpression();            /**< Get the mcnp expression of cone */
    virtual Standard_Boolean IsEqual(IGeomFace *& theSurf);     /**< Judge two cones is same or not */

    virtual Standard_Boolean Compare(const IGeomFace *& theGeoFace) ;   /**< Compare the priority for sorting */

    gp_Pnt GetApex() const;                                     /**< Get the peak point of cone */
    Standard_Real GetSemiAngle() const;                         /**< Get the semi angle of cone */
    gp_Dir GetAxisDir() const;                                  /**< Get the dir of cone's axis */

    void AddTransfCard(McCadGeomData *pData);
    virtual TCollection_AsciiString GetTransfNum()const;
    Standard_Boolean HaveTransfCard();

private:

    gp_Dir m_Dir;                                /**< Direction of axis */
    gp_Ax3 m_Axis;                               /**< Coordinate of cone */

    Standard_Real m_SemiAngle;                   /**< SemiAngle of cone */
    gp_Pnt m_Apex;                               /**< Top point of cone */

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

    Standard_Integer m_iTrNum;
    Standard_Boolean m_bTransfCrad;
};

#endif // MCCADGEOMCONE_H
