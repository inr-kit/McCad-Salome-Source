#include "McCadGeomSphere.hxx"
#include <GeomAdaptor_Surface.hxx>
#include <gp_Sphere.hxx>
#include <assert.h>

#include <TColStd_HSequenceOfAsciiString.hxx>
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadMathTool.hxx"

McCadGeomSphere::McCadGeomSphere()
{
}

/** ********************************************************************
* @brief Constructor, construct a sphere class based on geometry surface.
*
* @param const GeomAdaptor_Surface & theSurface
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
McCadGeomSphere::McCadGeomSphere(const GeomAdaptor_Surface & theSurface)
{
    m_SurfType = surfSphere ;                         // The surface is a sphere

    gp_Sphere sphere = theSurface.Sphere();           // Get the geometry of sphere
    sphere.Scale(gp_Pnt(0,0,0),GetUnit());            // Scale the face based on unit CAD model used

    m_Radius = sphere.Radius();                       // Get the radius
    m_Center = sphere.Location();                     // Get the center of sphere

    if( McCadMathTool::IsEqualZero(m_Center.X()))
        m_Center.SetX(0.0);
    if( McCadMathTool::IsEqualZero(m_Center.Y()))
        m_Center.SetY(0.0);
    if( McCadMathTool::IsEqualZero(m_Center.Z()))
         m_Center.SetZ(0.0);

    m_bReverse = Standard_False;

    GetExpression();
}

McCadGeomSphere::~McCadGeomSphere()
{
}



/** ********************************************************************
* @brief Generate the mcnp expression of a sphere
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
TCollection_AsciiString McCadGeomSphere::GetExpression()
{
    if (!m_SurfExpn.IsEmpty())
    {
        return m_SurfExpn;
    }
    Standard_Character chExpn[255];

    if (m_Center.X() == 0.0 && m_Center.Y() == 0.0 && m_Center.Z() == 0.0)
    {
        sprintf(chExpn, "SO%8s%15.7f\n", "", m_Radius);

        m_SurfSymb = "SO";
        m_PrmtList.push_back(m_Radius);
    }
    else if(m_Center.X() != 0.0 && m_Center.Y() == 0.0 && m_Center.Z() == 0.0)
    {
        sprintf(chExpn, "SX%8s%15.7f%5s%15.7f\n", "",m_Center.X(),"", m_Radius);

        m_SurfSymb = "SX";
        m_PrmtList.push_back(m_Center.X());
        m_PrmtList.push_back(m_Radius);
    }
    else if(m_Center.X() == 0.0 && m_Center.Y() != 0.0 && m_Center.Z() == 0.0)
    {
        sprintf(chExpn, "SY%8s%15.7f%5s%15.7f\n", "",m_Center.Y(),"", m_Radius);

        m_SurfSymb = "SY";
        m_PrmtList.push_back(m_Center.Y());
        m_PrmtList.push_back(m_Radius);
    }
    else if(m_Center.X() == 0.0 && m_Center.Y() == 0.0 && m_Center.Z() != 0.0)
    {
        sprintf(chExpn, "SZ%8s%15.7f%5s%15.7f\n", "",m_Center.Z(),"", m_Radius);

        m_SurfSymb = "SZ";
        m_PrmtList.push_back(m_Center.Z());
        m_PrmtList.push_back(m_Radius);
    }
    else
    {
        sprintf(chExpn, "S %8s%15.7f\%5s%15.7f%5s%15.7f%5s%15.7f\n",
                "",m_Center.X(),
                "",m_Center.Y(),
                "",m_Center.Z(),
                "",m_Radius);

        m_SurfSymb = "S ";
        m_PrmtList.push_back(m_Center.X());
        m_PrmtList.push_back(m_Center.Y());
        m_PrmtList.push_back(m_Center.Z());
        m_PrmtList.push_back(m_Radius);
    }

    TCollection_AsciiString szExpn( chExpn );
    szExpn.LeftAdjust();
    szExpn.RightAdjust();

    m_SurfExpn.Copy(szExpn);
    return m_SurfExpn;
}




/** ********************************************************************
* @brief Judge the two sphere is same or not
*
* @param IGeomFace *& theSurf
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadGeomSphere::IsEqual(IGeomFace *& theSurf)
{
    assert(theSurf);
    if(theSurf->GetFaceType() != m_SurfType)
    {
        return Standard_False;
    }

    McCadGeomSphere *pSphere = (McCadGeomSphere*)(theSurf);
    assert(pSphere);

    Standard_Real dis_tol = McCadConvertConfig::GetTolerence();
    //Standard_Real angle_tol = 1.0e-3*M_PI; //McCadConvertConfig::GetTolerence();

    // If the center and radius is same, the two cylinder can be treated as same cylinder
    if( m_Center.IsEqual(pSphere->GetCenter(), dis_tol )
            && fabs(m_Radius - pSphere->GetRadius()) < dis_tol )
    {
        return Standard_True;
    }

    return Standard_False;
}



/** ********************************************************************
* @brief Get the center of a sphere
*
* @param
* @return gp_Pnt m_Center
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
gp_Pnt McCadGeomSphere::GetCenter() const
{
    return m_Center;
}



/** ********************************************************************
* @brief Get the radius of a sphere
*
* @param
* @return Standard_Real m_Radius
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Real McCadGeomSphere::GetRadius() const
{
    return m_Radius;
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
void McCadGeomSphere::CleanObj() const
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
Standard_Boolean McCadGeomSphere::Compare(const IGeomFace *& theGeoFace)
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
TCollection_AsciiString McCadGeomSphere::GetTransfNum()const
{
    return "";
}
