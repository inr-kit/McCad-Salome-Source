#ifndef MCCADGEOMREVOLUTION_HXX
#define MCCADGEOMREVOLUTION_HXX

#include "IGeomFace.hxx"
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Handle_Geom_Curve.hxx>

class McCadGeomRevolution : public IGeomFace
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

    McCadGeomRevolution();
    McCadGeomRevolution(const GeomAdaptor_Surface & theSurface);

    virtual ~McCadGeomRevolution();

    virtual TCollection_AsciiString GetExpression();            /**< Calculate the equation and direction */
    virtual Standard_Boolean IsEqual(IGeomFace *& theGeoFace);  /**< Judge the two surface are same or not */
    virtual void CleanObj() const;                              /**< Clean the generated objects */
    virtual Standard_Boolean Compare(const IGeomFace *& theGeoFace);    /**< Compare the priority for sorting */
    virtual TCollection_AsciiString GetTransfNum()const;        /**< Get the transform card */

    gp_Pnt GetCenter() const;                       /**< Get the center of ellipse torus */
    gp_Dir GetAxisDir() const;                      /**< Get the direction of axis */
    Standard_Real GetMajorRadius() const;           /**< Get the major radius */
    Standard_Real GetMinorRadius() const;           /**< Get the minor radius */
    Standard_Real GetRadius() const;               /**< Get the minor radius */
    void ExtrFaceInfo(const GeomAdaptor_Surface & theSurface);  /**< Extract the geometry data from revolution face */
    void ScalePrmt();                               /**< Scale the parameters */

private:

    /**< If the section curve is not circle or ellipse circle, use the fitting function to calculate the
     *   coefficients of torus */
    void FitCurve(Handle_Geom_Curve BasisCurve,Handle_Geom_SurfaceOfRevolution rev);

private:

    gp_Ax3 m_Axis;                                  /**< The rotate axis of ellipse torus/torus */
    gp_Dir m_Dir;                                   /**< The direction of axis */
    Standard_Real m_Radius;                         /**< The major radius of torus or ellipsal torus/torus */
    Standard_Real m_MajorRadius;                    /**< Major radius of section */
    Standard_Real m_MinorRadius;                    /**< Minor radius of section */
    gp_Pnt m_Center;                                /**< Center of ellipse torus/torus */
};

#endif // MCCADGEOMREVOLUTION_HXX
