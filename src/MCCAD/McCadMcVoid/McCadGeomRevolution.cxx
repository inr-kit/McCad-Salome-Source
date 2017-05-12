#include "McCadGeomRevolution.hxx"

#include <GeomAdaptor_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <assert.h>

#include <TColStd_HSequenceOfAsciiString.hxx>
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadMathTool.hxx"

McCadGeomRevolution::McCadGeomRevolution()
{
}

/** ********************************************************************
* @brief Constructor
*
* @param const GeomAdaptor_Surface & theSurface
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
McCadGeomRevolution::McCadGeomRevolution(const GeomAdaptor_Surface & theSurface)
{
    m_SurfType = surfRev ;                          // Set the surface type
    ExtrFaceInfo(theSurface);                       // Get the geometry data   
}


McCadGeomRevolution::~McCadGeomRevolution()
{
}



/** ********************************************************************
* @brief Extract the geometry information of given rotated surface,
*        normally, this surface is tori.
*
* @param const GeomAdaptor_Surface & theSurface
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadGeomRevolution::ExtrFaceInfo(const GeomAdaptor_Surface & theSurface)
{
    Handle(Geom_Surface) surf = theSurface.Surface();

    Handle(Geom_SurfaceOfRevolution) rev = Handle(Geom_SurfaceOfRevolution)::DownCast(surf);
    Handle(Geom_Curve) BasisCurve = rev->BasisCurve();      // Get the section curve
    GeomAdaptor_Curve SectionCurve(BasisCurve);

    Standard_Real dis_tol = McCadConvertConfig::GetTolerence();

    /* The revolution surface might be an elliptical torus or torus */
    if(SectionCurve.GetType() == GeomAbs_Ellipse || SectionCurve.GetType() == GeomAbs_Circle )
    {
        /* Get the geomtry info of revolution axis */
        gp_Ax1 revAx = rev->Axis();
        gp_Dir revDir = revAx.Direction();
        gp_Vec revVec(revDir.X(), revDir.Y(), revDir.Z());  // The direction of axis

        m_Center = revAx.Location();                        // The center of revolution axis
        m_Axis.SetAxis(revAx);                              // Revolution axis
        m_Dir = m_Axis.Direction();                         // The direction of revolution axis

        /* Test if axis of revolution is perpendicular to two of the axis of the ellipse/circle */
        if(SectionCurve.GetType() == GeomAbs_Ellipse)
        {
            Handle(Geom_Ellipse) section = Handle(Geom_Ellipse)::DownCast(BasisCurve);

            gp_Ax1 normAx = section->Axis();
            gp_Dir normDir = normAx.Direction();
            gp_Vec normVec(normDir.X(), normDir.Y(), normDir.Z());

            if(Abs( revVec * normVec ) > dis_tol) // Judge if the section vector is perpendicular to axis vector.
            {
                cout << "Ellipse not perpendicular to rotation Axis !\n" << endl;
            }

            gp_Pnt pCntrSection = section->Location();      // The center of section
            m_Radius = m_Center.Distance(pCntrSection);     // The major radius of ellipital torus
            m_MajorRadius = section->MajorRadius();         // The major radius of ellipse section
            m_MinorRadius = section->MinorRadius();         // The minor radius of ellipse section
        }
        else
        {
            Handle(Geom_Circle) section = Handle(Geom_Circle)::DownCast(BasisCurve);

            gp_Ax1 normAx = section->Axis();
            gp_Dir normDir = normAx.Direction();
            gp_Vec normVec(normDir.X(), normDir.Y(), normDir.Z());

            if(Abs( revVec * normVec ) > dis_tol) // Judge if the section vector is perpendicular to axis vector.
            {
                cout << "Ellipse not perpendicular to rotation Axis !\n" << endl;
            }

            gp_Pnt pCntrSection = section->Location();      // The center of section
            m_Radius = m_Center.Distance(pCntrSection);     // The major radius of torus
            m_MajorRadius = section->Radius();              // The radius of circle section
            m_MinorRadius = section->Radius();              // The radius of circle section
        }
    }
    else
    {
        cout<<"# A rotated surface is detected, but the rotated curve is not cirle or ellipse circle"<<endl;
        cout<<"# Generate the sample points on curve to fit the equation"<<endl;
        FitCurve(BasisCurve,rev);

        cout<<"Radius: "<<m_Radius<<endl;
        cout<<"MajorRadius: "<<m_MajorRadius<<endl;
        cout<<"MinorRadius: "<<m_MinorRadius<<endl<<endl;
    }

    if( McCadMathTool::IsEqualZero(m_Center.X()))
        m_Center.SetX(0.0);
    if( McCadMathTool::IsEqualZero(m_Center.Y()))
        m_Center.SetY(0.0);
    if( McCadMathTool::IsEqualZero(m_Center.Z()))
         m_Center.SetZ(0.0);

    m_bReverse = Standard_False;
    ScalePrmt();                                            // Scale the parameter based on unit used
    GetExpression();
    return;
}




/** ********************************************************************
* @brief Get the mcnp expression of torus
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
TCollection_AsciiString McCadGeomRevolution::GetExpression()
{
    if (!m_SurfExpn.IsEmpty())
    {
        return m_SurfExpn;
    }
    Standard_Character chExpn[255];

    Standard_Real dis_tol = McCadConvertConfig::GetTolerence();
    Standard_Real angle_tol = 1.0e-3; //McCadConvertConfig::GetTolerence()

    gp_Ax3 Ax3_X(m_Center, gp::DX());
    gp_Ax3 Ax3_Y(m_Center, gp::DY());
    gp_Ax3 Ax3_Z(m_Center, gp::DZ());

    if (m_Axis.IsCoplanar(Ax3_X, dis_tol, angle_tol))
    {
        /*sprintf(chExpn, "TX%8s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f%5s%15.7f%5s%15.7f\n",
                        "", m_Center.X(),
                        "", m_Center.Y(),
                        "", m_Center.Z(),
                        "", m_Radius,
                        "", m_MajorRadius,
                        "", m_MinorRadius);*/

        m_SurfSymb = "TX";
        m_PrmtList.push_back(m_Center.X());
        m_PrmtList.push_back(m_Center.Y());
        m_PrmtList.push_back(m_Center.Z());
        m_PrmtList.push_back(m_Radius);
        m_PrmtList.push_back(m_MajorRadius);
        m_PrmtList.push_back(m_MinorRadius);


    }
    else if (m_Axis.IsCoplanar(Ax3_Y, dis_tol, angle_tol))
    {
       /* sprintf(chExpn, "TY%8s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f%5s%15.7f%5s%15.7f\n",
                        "", m_Center.X(),
                        "", m_Center.Y(),
                        "", m_Center.Z(),
                        "", m_Radius,
                        "", m_MajorRadius,
                        "", m_MinorRadius);*/

        m_SurfSymb = "TY";
        m_PrmtList.push_back(m_Center.X());
        m_PrmtList.push_back(m_Center.Y());
        m_PrmtList.push_back(m_Center.Z());
        m_PrmtList.push_back(m_Radius);
        m_PrmtList.push_back(m_MajorRadius);
        m_PrmtList.push_back(m_MinorRadius);
    }
    else if (m_Axis.IsCoplanar(Ax3_Z, dis_tol, angle_tol))
    {
        /*sprintf(chExpn, "TZ%8s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f%5s%15.7f%5s%15.7f\n",
                        "", m_Center.X(),
                        "", m_Center.Y(),
                        "", m_Center.Z(),
                        "", m_Radius,
                        "", m_MajorRadius,
                        "", m_MinorRadius);*/

        m_SurfSymb = "TZ";
        m_PrmtList.push_back(m_Center.X());
        m_PrmtList.push_back(m_Center.Y());
        m_PrmtList.push_back(m_Center.Z());
        m_PrmtList.push_back(m_Radius);
        m_PrmtList.push_back(m_MajorRadius);
        m_PrmtList.push_back(m_MinorRadius);
    }
    else
    {
        //m_SurfSymb = "TX";
        m_SurfSymb = "TZ";
        m_PrmtList.push_back(m_Center.X());
        m_PrmtList.push_back(m_Center.Y());
        m_PrmtList.push_back(m_Center.Z());
        //sprintf(chExpn, "c      McCad Error: A generic ellipital torus transformation.\n");
    }

    TCollection_AsciiString szExpn( chExpn );
    szExpn.LeftAdjust();
    szExpn.RightAdjust();

    m_SurfExpn.Copy(szExpn);
    return m_SurfExpn;
}



/** ********************************************************************
* @brief Judge the two torus is same or not
*
* @param IGeomFace *& theSurf
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadGeomRevolution::IsEqual(IGeomFace *& theSurf)
{
    assert(theSurf);
    if(theSurf->GetFaceType() != m_SurfType)
    {
        return Standard_False;
    }

    McCadGeomRevolution *pTorus = (McCadGeomRevolution*)(theSurf);
    assert(pTorus);

    Standard_Real dis_tol = McCadConvertConfig::GetTolerence();
    Standard_Real angle_tol = 1.0e-3*M_PI; //McCadConvertConfig::GetTolerence();

    // If the center and radius is same, the two cylinder can be treated as same cylinder
    if( m_Dir.IsEqual(pTorus->GetAxisDir(), angle_tol)
         || m_Dir.IsOpposite(pTorus->GetAxisDir(), angle_tol))
    {
        if( m_Center.IsEqual(pTorus->GetCenter(), dis_tol )
        && fabs(m_MajorRadius - pTorus->GetMajorRadius()) < dis_tol
        && fabs(m_MinorRadius - pTorus->GetMinorRadius()) < dis_tol )
        {
            return Standard_True;
        }
    }

    return Standard_False;
}



/** ********************************************************************
* @brief Get the center of torus
*
* @param
* @return gp_Pnt
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
gp_Pnt McCadGeomRevolution::GetCenter() const
{
    return m_Center;
}



/** ********************************************************************
* @brief Get major radius of torus
*
* @param
* @return Standard_Real
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Real McCadGeomRevolution::GetMajorRadius() const
{
    return m_MajorRadius;
}



/** ********************************************************************
* @brief Get minor radius of torus
*
* @param
* @return Standard_Real
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Real McCadGeomRevolution::GetMinorRadius() const
{
    return m_MinorRadius;
}


/** ********************************************************************
* @brief Get radius of torus
*
* @param
* @return Standard_Real
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Real McCadGeomRevolution::GetRadius() const
{
    return m_Radius;
}


/** ********************************************************************
* @brief Get axis direction of torus
*
* @param
* @return gp_Dir
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
gp_Dir McCadGeomRevolution::GetAxisDir() const
{
    return m_Dir;
}



/** ********************************************************************
* @brief Clean the generated objects
*
* @param
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadGeomRevolution::CleanObj() const
{

}




/** ********************************************************************
* @brief Scale the surface and related coefficients
*
* @param
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadGeomRevolution::ScalePrmt()
{
    gp_Pnt ori(0,0,0);
    m_Axis.Scale(ori,GetUnit());
    m_Center.Scale(ori,GetUnit());
    m_Radius *= GetUnit();
    m_MajorRadius *= GetUnit();
    m_MinorRadius *= GetUnit();
}




/** ********************************************************************
* @brief Compared with other faces, get the priorites for surface sorting.
*
* @param const IGeomFace *& theGeoFace
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadGeomRevolution::Compare(const IGeomFace *& theGeoFace)
{
    if( McCadConvertConfig::GetSurfSequNum(m_SurfSymb)
            < McCadConvertConfig::GetSurfSequNum(theGeoFace->GetSurfSymb()) )
    {
        return Standard_True;
    }
    else if ( McCadConvertConfig::GetSurfSequNum(m_SurfSymb)
                 == McCadConvertConfig::GetSurfSequNum(theGeoFace->GetSurfSymb()))
    {
//qiu        if ( m_PrmtList[0] < theGeoFace->GetPrmtList()[0])
        if ( m_PrmtList[0] < const_cast<IGeomFace *&>(theGeoFace)->GetPrmtList()[0])
        {
            return Standard_True;
        }
        else
        {
            return Standard_False;
        }
    }
    else
    {
        return Standard_False;
    }

}


/** ********************************************************************
* @brief If the curve of cross section is not circle or ellipse circle
*        fit the curve as a circle with the start, end and middle points
*        Thus calculate the minor radius and major radius and center of
*        this rotated surface, which is approxiated as a tori.
*
* @param Handle_Geom_Curve BasisCurve,Handle_Geom_SurfaceOfRevolution rev
* @return void
*
* @date 31/8/2012
* @modify 20/07/2016
* @author  Lei Lu
************************************************************************/
void McCadGeomRevolution::FitCurve(Handle_Geom_Curve BasisCurve,
                                   Handle_Geom_SurfaceOfRevolution rev)
{
    gp_Ax1 revAx = rev->Axis();
    m_Center = revAx.Location();                        // The center of revolution axis
    m_Axis.SetAxis(revAx);
    m_Dir = m_Axis.Direction();                         // The direction of revolution axis

    gp_Pnt p1 = BasisCurve->Value(BasisCurve->FirstParameter());
    gp_Pnt p2 = BasisCurve->Value(BasisCurve->LastParameter());
    gp_Pnt p3 = BasisCurve->Value((BasisCurve->FirstParameter()+BasisCurve->LastParameter())/2);

    double a, b, c, d, e, f;

    double pA1,pA2,pA3,pB1,pB2,pB3;
    double xA,xB;

    int i;

    if (p1.X()==p2.X() && p2.X()==p3.X())
    {
        pA1 = p1.Y();
        pB1 = p1.Z();
        pA2 = p2.Y();
        pB2 = p2.Z();
        pA3 = p3.Y();
        pB3 = p3.Z();

        i=0;
    }
    else if (p1.Y()==p2.Y() && p2.Y()==p3.Y())
    {
        pA1 = p1.X();
        pB1 = p1.Z();
        pA2 = p2.X();
        pB2 = p2.Z();
        pA3 = p3.X();
        pB3 = p3.Z();

        i=1;
    }
    else if (p1.Z()==p2.Z() && p2.Z()==p3.Z())
    {
        pA1 = p1.X();
        pB1 = p1.Y();
        pA2 = p2.X();
        pB2 = p2.Y();
        pA3 = p3.X();
        pB3 = p3.Y();

        i=2;
    }

    a = 2*(pA2-pA1);
    b = 2*(pB2-pB1);
    c = pA2*pA2 + pB2*pB2 - pA1*pA1 - pB1*pB1;

    d = 2*(pA3-pA1);
    e = 2*(pB3-pB1);

    f = pA3*pA3 + pB3*pB3 - pA1*pA1 - pB1*pB1;

    xA = (b*f - e*c)/(b*d - e*a);
    xB = (d*c - a*f)/(b*d - e*a);

    if (Abs(xA)<1.0e-5)
        xA = 0.0;
    if (Abs(xB)<1.0e-5)
        xB = 0.0;

    double center_x, center_y, center_z;

    if (i == 0)
    {
        center_y = xA;
        center_z = xB;
        center_x = p1.X();
    }
    else if (i == 1)
    {
        center_x = xA;
        center_z = xB;
        center_y = p1.Y();
    }
    else if (i == 2)
    {
        center_x = xA;
        center_y = xB;
        center_z = p1.Z();
    }


    double maj_radius = Sqrt((xA-pA1)*(xA-pA1)+(xB-pB1)*(xB-pB1));
    double radius = Sqrt(pow(center_x-m_Center.X(),2)
                         +pow(center_y-m_Center.Y(),2)
                         +pow(center_z-m_Center.Z(),2));

    m_Radius = radius;    
    m_MajorRadius = maj_radius;
    m_MinorRadius = maj_radius;    
}


/** ********************************************************************
* @brief Get the transform card
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
TCollection_AsciiString McCadGeomRevolution::GetTransfNum()const
{
    return "";
}

