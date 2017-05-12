#include "McCadEvaluator.hxx"

#include <GeomAbs_SurfaceType.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax3.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <GeomAPI_IntCS.hxx>
#include <GeomAPI_IntSS.hxx>
#include <Geom_SurfaceOfRevolution.hxx>

#include <GeomAPI_ProjectPointOnSurf.hxx>


McCadEvaluator::McCadEvaluator()
{
}

McCadEvaluator::~McCadEvaluator()
{
}




/** ********************************************************************
* @brief Evaluate the relative position between point and surface, if
*        result returned is minus, it means the point in the surface. if
*        plus, means outside the surface.
* @param GeomAdaptor_Surface& theSurf
*        gp_Pnt& thePoint
* @return Standard_Real
*
* @date 09/08/2016
* @author  Lei Lu
***********************************************************************/
Standard_Real McCadEvaluator::Evaluate(const GeomAdaptor_Surface& theSurf,
                                       const gp_Pnt& thePoint)
{
    GeomAbs_SurfaceType surfType = theSurf.GetType();
    switch (surfType)
    {
        case GeomAbs_Plane:
        {
            gp_Pln aPlane = theSurf.Plane();
            return Evaluate(aPlane, thePoint);
            break;
        }
        case GeomAbs_Cylinder:
        {
            gp_Cylinder aCyl = theSurf.Cylinder();
            return Evaluate(aCyl, thePoint);
            break;
        }
        case GeomAbs_Cone:
        {
            gp_Cone aCone = theSurf.Cone();
            return Evaluate(aCone, thePoint);
            break;
        }
        case GeomAbs_Sphere:
        {
            gp_Sphere aSp = theSurf.Sphere();
            return Evaluate(aSp, thePoint);
            break;
        }
        case GeomAbs_Torus:
        {
            gp_Torus aTor = theSurf.Torus();
            return Evaluate(aTor, thePoint);
            break;
        }
        case GeomAbs_SurfaceOfRevolution:
        {
            return RevolutionEvaluate(theSurf,thePoint);
            break;
        }
        case GeomAbs_BezierSurface:
        {
            cout << "_#_McCadEvaluator.cxx :: No evaluation for BezierSurfaces available!" << endl;
            return 0;
            break;
        }
        case GeomAbs_BSplineSurface:
        {
            cout << "_#_McCadEvaluator.cxx :: No evaluation for BSplineSurfaces available!" << endl;
            return 0;
            break;
        }
        case GeomAbs_SurfaceOfExtrusion:
        {
            cout << "_#_McCadEvaluator.cxx :: No evaluation extruded surfaces available!" << endl;
            return 0;
            break;
        }
        case GeomAbs_OffsetSurface:
        {
            cout << "_#_McCadEvaluator.cxx :: No evaluation for this type of surface available!!!" << endl;
            return 0;
            break;
        }
        case GeomAbs_OtherSurface:
        {
            cout << "_#_McCadEvaluator.cxx :: No evaluation for this type of surface available!!" << endl;
            return 0;
            break;
        }
    }
    cout << "_#_McCadEvaluator.cxx :: No evaluation for this type of surface available!" << endl;
    return 0.0;
}



/** ********************************************************************
* @brief Evaluate the relative position between point and plane
* @param gp_Pln& plane
*        gp_Pnt& thePoint
* @return Standard_Real
*
* @date 09/08/2016
* @author  Lei Lu
***********************************************************************/
Standard_Real McCadEvaluator::Evaluate(const gp_Pln& plane, const gp_Pnt& thePoint)
{
    Standard_Real A=0, B=0, C=0, D=0;
    plane.Coefficients(A, B, C, D);
    return McCadEvaluator::PlaneEvaluate(thePoint.X(), thePoint.Y(), thePoint.Z(), A, B, C, D);
}



/** ********************************************************************
* @brief Evaluate the relative position between point and cone
* @param gp_Cone& Cone
*        gp_Pnt& thePoint
* @return Standard_Real
*
* @date 09/08/2016
* @author  Lei Lu
***********************************************************************/
Standard_Real McCadEvaluator::Evaluate(const gp_Cone& cone,
                                       const gp_Pnt& thePoint)
{
    Standard_Real A1=0, A2=0, A3=0, B1=0, B2=0, B3=0, C1=0, C2=0, C3=0, D=0;
    cone.Coefficients(A1, A2, A3, B1, B2, B3, C1, C2, C3, D);
    return McCadEvaluator::QuadricEvaluate(thePoint.X(), thePoint.Y(), thePoint.Z(), A1, A2, A3, B1, B2, B3, C1, C2, C3, D);
}



/** ********************************************************************
* @brief Evaluate the relative position between point and cylinder
* @param gp_Cylinder& cylinder
*        gp_Pnt& thePoint
* @return Standard_Real
*
* @date 09/08/2016
* @author  Lei Lu
***********************************************************************/
Standard_Real McCadEvaluator::Evaluate(const gp_Cylinder& cylinder,
                                       const gp_Pnt& thePoint)
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    Standard_Real A1, A2, A3, B1, B2, B3, C1, C2, C3, D;
    cylinder.Coefficients(A1, A2, A3, B1, B2, B3, C1, C2, C3, D);
    return McCadEvaluator::QuadricEvaluate(thePoint.X(), thePoint.Y(),	thePoint.Z(), A1, A2, A3, B1, B2, B3, C1, C2, C3, D);
}



/** ********************************************************************
* @brief Evaluate the relative position between point and sphere
* @param gp_Sphere& sphere
*        gp_Pnt& thePoint
* @return Standard_Real
*
* @date 09/08/2016
* @author  Lei Lu
***********************************************************************/
Standard_Real McCadEvaluator::Evaluate(const gp_Sphere& sphere,
                                       const gp_Pnt& thePoint)
{
    Standard_Real A1, A2, A3, B1, B2, B3, C1, C2, C3, D;
    sphere.Coefficients(A1, A2, A3, B1, B2, B3, C1, C2, C3, D);
    return McCadEvaluator::QuadricEvaluate(thePoint.X(), thePoint.Y(), thePoint.Z(), A1, A2, A3, B1, B2, B3, C1, C2, C3, D);
}



/** ********************************************************************
* @brief Evaluate the relative position between point and tori
* @param gp_Torus& tori
*        gp_Pnt& thePoint
* @return Standard_Real
*
* @date 09/08/2016
* @author  Lei Lu
***********************************************************************/
Standard_Real McCadEvaluator::Evaluate(const gp_Torus& tori, const gp_Pnt& thePoint)
{
    // Due to problems
    // calssical point location is performed.
    gp_Trsf T;
    gp_Ax3 s3x(gp::Origin(), gp::DZ());
    gp_Ax3 a3x = tori.Position();
    T.SetTransformation(s3x, a3x);
    gp_Pnt loc = a3x.Location();
    gp_Vec V(loc, thePoint);
    gp_Dir d(V);

    Standard_Real dist = loc.Distance(thePoint);
    Standard_Boolean OutSide = Standard_False, OnSide = Standard_False, InSide=
            Standard_False, Failed = Standard_False;

    Handle(Geom_Line) L = new Geom_Line(loc,d);
    Handle(Geom_ToroidalSurface) Tt = new Geom_ToroidalSurface(tori);
    Standard_Real s=0, b=0;
    Standard_Real d1, d2, d3, d4;
    GeomAPI_IntCS Inter(L, Tt);
    Standard_Integer NbP;

    if (Inter.IsDone())
    {
        NbP = Inter.NbPoints();
        switch (NbP)
        {
        case 0:
            OutSide = Standard_True;
            break;
        case 1:
        case 2:
            OnSide = Standard_True;
            break;
        case 3:
            break;
        case 4:
            d1 = loc.Distance(Inter.Point(1));
            d2 = loc.Distance(Inter.Point(2));
            d3 = loc.Distance(Inter.Point(3));
            d4 = loc.Distance(Inter.Point(4));

            if (d1 <= d2)
                s = d1;
            else
                s=d2;
            if (s <= d3)
                ;
            else
                s = d3;
            if (s <= d4)
                ;
            else
                s = d4;
            if (d1 >= d2)
                b = d1;
            else
                b=d2;
            if (b >= d3)
                ;
            else
                b = d3;
            if (b >= d4)
                ;
            else
                b = d4;
            if ((dist - s) > 1.e-03 && (b - dist) > 1.e-03)
                InSide = Standard_True;
            else if ((s - dist) > 1.e-03 || (dist - b) > 1.e-03)
                OutSide = Standard_True;
            else if (fabs(dist - s) < 1.e-03 || fabs(b-dist) < 1.e-03)
                OnSide = Standard_True;
            else
                cout << "Torus evaluation failed!!" << endl;
            break;
        default:
            cout << "Torus evaluation failed!!" << endl;
            Failed = Standard_True;
            break;
        }
    }
    else
    {
        cout << "Torus evaluation failed!!" << endl;
        Failed = Standard_True;
    }

    //  cout << "Number of Points =   " << NbP << endl;
    if (InSide)
        return -dist;
    else if (OutSide)
        return dist;
    else if (OnSide)
        return 0.0;
    else
    {
        cout << "Unclassified  Point:  X = " << thePoint.X() << " Y = "	<< thePoint.Y() << " Z = " << thePoint.Z() << " Value  = "	<< dist << endl;
        Failed = Standard_True;
    }

    if (Failed)
    {
        cout << "Cascade Torus evaluation !!!" << endl;
        TColStd_Array1OfReal Coef(1, 31);
        tori.Coefficients(Coef);
        return McCadEvaluator::TorusEvaluate(thePoint.X(), thePoint.Y(), thePoint.Z(), Coef);
    }

    return 0.0;
}




/** ********************************************************************
* @brief Calculate the result with point and plane equation
* @param const Standard_Real X,
         const Standard_Real Y,
         const Standard_Real Z,
         const Standard_Real A,
         const Standard_Real B,
         const Standard_Real C,
         const Standard_Real D
* @return Standard_Real
*
* @date 09/08/2016
* @author  Lei Lu
***********************************************************************/
Standard_Real McCadEvaluator::PlaneEvaluate(const Standard_Real X,
                                            const Standard_Real Y,
                                            const Standard_Real Z,
                                            const Standard_Real A,
                                            const Standard_Real B,
                                            const Standard_Real C,
                                            const Standard_Real D)
{
    //cout << X << " " << Y << " " << Z << " " << endl;
    return A * X + B * Y + C * Z + D;
}



/** ********************************************************************
* @brief Calculate the result with point and GQ surface equation
* @param  Standard_Real X,
          Standard_Real Y,
          Standard_Real Z,
          Standard_Real A1,
          Standard_Real A2,
          Standard_Real A3,
          Standard_Real B1,
          Standard_Real B2,
          Standard_Real B3,
          Standard_Real C1,
          Standard_Real C2,
          Standard_Real C3,
          Standard_Real D
* @return Standard_Real
*
* @date 09/08/2016
* @author  Lei Lu
***********************************************************************/
Standard_Real McCadEvaluator::QuadricEvaluate(const Standard_Real X,
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
                                              const Standard_Real D)
{
    return A1*X*X + A2*Y*Y + A3*Z*Z + 2.*(B1*X*Y + B2*X*Z + B3*Y*Z) + 2.*(C1*X + C2*Y + C3*Z) + D;
}




/** ********************************************************************
* @brief Calculate the result with point and tori surface equation
* @param    const Standard_Real X,
            const Standard_Real Y,
            const Standard_Real Z,
            const TColStd_Array1OfReal& Coef
* @return Standard_Real
*
* @date 09/08/2016
* @author  Lei Lu
***********************************************************************/
Standard_Real McCadEvaluator::TorusEvaluate(const Standard_Real X,
                                            const Standard_Real Y,
                                            const Standard_Real Z,
                                            const TColStd_Array1OfReal& Coef)
{

    return Coef(1)*Pow(X, 4) + Coef(2)*Pow(Y, 4) + Coef(3)*Pow(Z, 4) + Coef(4)
            *Pow(X, 3)*Y + Coef(5)*Pow(X, 3)*Z + Coef(6)*Pow(Y, 3)*X + Coef(7)
            *Pow(Y, 3)*Z + Coef(8)*Pow(Z, 3)*X + Coef(9)*Pow(Z, 3)*Y + Coef(10)
            *X*X*Y*Y + Coef(11)*X*X*Z*Z + Coef(12)*Y*Y*Z*Z + Coef(13)*Pow(X, 3)
            + Coef(14)*Pow(Y, 3) + Coef(15)*Pow(Z, 3) + Coef(16)*X*X*Y
            + Coef(17)*X*X*Z + Coef(18)*Y*Y*X + Coef(19)*Y*Y*Z + Coef(20)*Z*Z*X
            + Coef(21)*Z*Z*Y + Coef(22)*X*X + Coef(23)*Y*Y + Coef(24)*Z*Z
            + Coef(25)*X*Y + Coef(26)*X*Z + Coef(27)*Y*Z + Coef(28)*X
            + Coef(29)*Y + Coef(30)*Z;
}



/** ********************************************************************
* @brief Evaluate the relative position between point and revolution surface
*        Create a solid with the revolution surface, and calculate the
*        relative position between solid and given point
* @param GeomAdaptor_Surface& theSurf
*        gp_Pnt& thePoint
* @return Standard_Real
*
* @date 09/08/2016
* @author  Lei Lu
***********************************************************************/
Standard_Real McCadEvaluator::RevolutionEvaluate(const GeomAdaptor_Surface& theSurf,
                                                 const gp_Pnt& thePoint)

{
    /// compute nearest point on surface
    Handle(Geom_Surface) s = theSurf.Surface();
    GeomAPI_ProjectPointOnSurf projector(thePoint, s);
    gp_Pnt nearestPnt = projector.NearestPoint();

    Standard_Real dist = nearestPnt.Distance(thePoint);
    if(dist < 1.0e-5)
    {
        return 0.0;
    }

    /// Make solid and check if thePoint lies on the inside or the outside
    BRepBuilderAPI_MakeShell shellMaker(s, Standard_False);
    TopoDS_Shell theShell = shellMaker.Shell();
    BRepBuilderAPI_MakeSolid solidMaker(theShell);
    TopoDS_Shape theSolid = solidMaker.Shape();

    Standard_Integer sgn = 1;

    BRepClass3d_SolidClassifier clssfr(theSolid, thePoint, 1e-7);

    if(clssfr.State() == TopAbs_IN)
    {
        sgn = -1;
    }
    else if(clssfr.State() == TopAbs_ON)
    {
        dist = 0.0;
    }

    return dist*sgn;
}
