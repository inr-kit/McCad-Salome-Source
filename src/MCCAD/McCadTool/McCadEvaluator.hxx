#ifndef MCCADEVALUATOR_HXX
#define MCCADEVALUATOR_HXX

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Real.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <TColStd_Array1OfReal.hxx>

class McCadEvaluator
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

    McCadEvaluator();
    ~McCadEvaluator();

public:
    //! Evaluate a surface -> Get surface type and call responsible method from below
    Standard_EXPORT static Standard_Real Evaluate(const GeomAdaptor_Surface& theSurf,
                                                  const gp_Pnt& thePoint);

    /**< Evaluate the relative position between point and plane */
    Standard_EXPORT static  Standard_Real Evaluate(const gp_Pln& Pl,const gp_Pnt& thePoint);
    /**< Evaluate the relative position between point and cone */
    Standard_EXPORT static  Standard_Real Evaluate(const gp_Cone& Co,const gp_Pnt& thePoint);
    /**< Evaluate the relative position between point and cylinder */
    Standard_EXPORT static  Standard_Real Evaluate(const gp_Cylinder& Cy,const gp_Pnt& thePoint);
    /**< Evaluate the relative position between point and sphere */
    Standard_EXPORT static  Standard_Real Evaluate(const gp_Sphere& Sp,const gp_Pnt& thePoint);
    /**< Evaluate the relative position between point and torus */
    Standard_EXPORT static  Standard_Real Evaluate(const gp_Torus& To, const gp_Pnt& thePoint);
    /**< Evaluate the relative position between point and revolution surface */
    Standard_EXPORT static Standard_Real RevolutionEvaluate(const GeomAdaptor_Surface& theSurf,
                                            const gp_Pnt& thePoint);

    /**< Calculate the result of plane's equation with given point */
    static  Standard_Real PlaneEvaluate(const Standard_Real X,
                                        const Standard_Real Y,
                                        const Standard_Real Z,
                                        const Standard_Real A,
                                        const Standard_Real B,
                                        const Standard_Real C,
                                        const Standard_Real D);
    /**< Calculate the result of GQ surface equation with given point */
    static  Standard_Real QuadricEvaluate(const Standard_Real X,
                                          const Standard_Real Y,
                                          const Standard_Real Z,
                                          const Standard_Real A1,
                                          const Standard_Real A2,
                                          const Standard_Real A3,
                                          const Standard_Real B1,
                                          const Standard_Real B2,
                                          const Standard_Real B3,
                                          const Standard_Real C1,
                                          const Standard_Real C2,
                                          const Standard_Real C3,
                                          const Standard_Real D);

    /**< Calculate the result of tori equation with given point */
    static Standard_Real TorusEvaluate(const Standard_Real X,
                                       const Standard_Real Y,
                                       const Standard_Real Z,
                                       const TColStd_Array1OfReal& Coef);

};

#endif // MCCADEVALUATOR_HXX
