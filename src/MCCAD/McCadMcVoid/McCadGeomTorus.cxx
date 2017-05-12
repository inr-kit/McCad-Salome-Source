#include "McCadGeomTorus.hxx"

#include <assert.h>
#include <gp_Torus.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadMathTool.hxx"

McCadGeomTorus::McCadGeomTorus()
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
McCadGeomTorus::McCadGeomTorus(const GeomAdaptor_Surface & theSurface)
{
    m_SurfType = surfTorus ;                        // The surface is a torus

    gp_Torus torus = theSurface.Torus();            // Get the geometry of torus
    torus.Scale(gp_Pnt(0,0,0),GetUnit());             // Scale the face based on unit CAD model used

    m_Axis = torus.Position();                      // The coordinator of torus
    m_Dir = m_Axis.Direction();                     // The direction of torus axis
    m_Center = m_Axis.Location();                   // The center of torus

    if( McCadMathTool::IsEqualZero(m_Center.X()))
        m_Center.SetX(0.0);
    if( McCadMathTool::IsEqualZero(m_Center.Y()))
        m_Center.SetY(0.0);
    if( McCadMathTool::IsEqualZero(m_Center.Z()))
         m_Center.SetZ(0.0);

    m_MajorRadius = torus.MajorRadius();            // The major radius of torus
    m_MinorRadius = torus.MinorRadius();            // The minor radius of torus

    m_bReverse = Standard_False;
    //m_PrmtList = new TColStd_HSequenceOfAsciiString();
    GetExpression();

}

McCadGeomTorus::~McCadGeomTorus()
{
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
TCollection_AsciiString McCadGeomTorus::GetExpression()
{
    if (!m_SurfExpn.IsEmpty())
    {
        return m_SurfExpn;
    }
    Standard_Character chExpn[255];

    Standard_Real dis_tol = McCadConvertConfig::GetTolerence();
    Standard_Real angle_tol = 1.0e-3; //McCadConvertConfig::GetTolerence();

    gp_Ax3 Ax3_X(m_Center, gp::DX());
    gp_Ax3 Ax3_Y(m_Center, gp::DY());
    gp_Ax3 Ax3_Z(m_Center, gp::DZ());

    if (m_Axis.IsCoplanar(Ax3_X, dis_tol, angle_tol))
    {
        sprintf(chExpn, "TX%8s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f%5s%15.7f%5s%15.7f\n",
                        "", m_Center.X(),
                        "", m_Center.Y(),
                        "", m_Center.Z(),
                        "", m_MajorRadius,
                        "", m_MinorRadius,
                        "", m_MinorRadius);

        m_SurfSymb = "TX";
        m_PrmtList.push_back(m_Center.X());
        m_PrmtList.push_back(m_Center.Y());
        m_PrmtList.push_back(m_Center.Z());
        m_PrmtList.push_back(m_MajorRadius);
        m_PrmtList.push_back(m_MinorRadius);
        m_PrmtList.push_back(m_MinorRadius);
    }
    else if (m_Axis.IsCoplanar(Ax3_Y, dis_tol, angle_tol))
    {
        sprintf(chExpn, "TY%8s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f%5s%15.7f%5s%15.7f\n",
                        "", m_Center.X(),
                        "", m_Center.Y(),
                        "", m_Center.Z(),
                        "", m_MajorRadius,
                        "", m_MinorRadius,
                        "", m_MinorRadius);

        m_SurfSymb = "TY";
        m_PrmtList.push_back(m_Center.X());
        m_PrmtList.push_back(m_Center.Y());
        m_PrmtList.push_back(m_Center.Z());
        m_PrmtList.push_back(m_MajorRadius);
        m_PrmtList.push_back(m_MinorRadius);
        m_PrmtList.push_back(m_MinorRadius);
    }
    else if (m_Axis.IsCoplanar(Ax3_Z, dis_tol, angle_tol))
    {
        sprintf(chExpn, "TZ%8s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f%5s%15.7f%5s%15.7f\n",
                        "", m_Center.X(),
                        "", m_Center.Y(),
                        "", m_Center.Z(),
                        "", m_MajorRadius,
                        "", m_MinorRadius,
                        "", m_MinorRadius);

        m_SurfSymb = "TZ";
        m_PrmtList.push_back(m_Center.X());
        m_PrmtList.push_back(m_Center.Y());
        m_PrmtList.push_back(m_Center.Z());
        m_PrmtList.push_back(m_MajorRadius);
        m_PrmtList.push_back(m_MinorRadius);
        m_PrmtList.push_back(m_MinorRadius);
    }
    else
    {
        sprintf(chExpn, "c      McCad Error: A generic Torus transformation.\n");
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
Standard_Boolean McCadGeomTorus::IsEqual(IGeomFace *& theSurf)
{
    assert(theSurf);
    if(theSurf->GetFaceType() != m_SurfType)
    {
        return Standard_False;
    }

    McCadGeomTorus *pTorus = (McCadGeomTorus*)(theSurf);
    assert(pTorus);

    Standard_Real dis_tol = McCadConvertConfig::GetTolerence();
    Standard_Real angle_tol = 1.0e-3; //McCadConvertConfig::GetTolerence();

    // If the center and radius is same, the two cylinder can be treated as same cylinder
    if( m_Dir.IsEqual(pTorus->GetAxisDir(), angle_tol)
         || m_Dir.IsOpposite(pTorus->GetAxisDir(), angle_tol))
    {
        if( m_Center.IsEqual(pTorus->GetCenter(), angle_tol )
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
gp_Pnt McCadGeomTorus::GetCenter() const
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
Standard_Real McCadGeomTorus::GetMajorRadius() const
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
Standard_Real McCadGeomTorus::GetMinorRadius() const
{
    return m_MinorRadius;
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
gp_Dir McCadGeomTorus::GetAxisDir() const
{
    return m_Dir;
}



/** ********************************************************************
* @brief Clean the generated objects
*
* @param
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadGeomTorus::CleanObj() const
{
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
Standard_Boolean McCadGeomTorus::Compare(const IGeomFace *& theGeoFace)
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
* @brief Get the transform card
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
TCollection_AsciiString McCadGeomTorus::GetTransfNum()const
{
    return "";
}
