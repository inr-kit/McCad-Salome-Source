#include "McCadGeomPlane.hxx"

#include <gp_Pln.hxx>
#include <assert.h>

#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadMathTool.hxx"

using namespace std;

McCadGeomPlane::McCadGeomPlane()
{

}
McCadGeomPlane::~McCadGeomPlane()
{
    m_PrmtList.clear();
}

/** ********************************************************************
* @brief Construct the plane class based on the geometry surface
*
* @param const GeomAdaptor_Surface & theSurface
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
McCadGeomPlane::McCadGeomPlane(const GeomAdaptor_Surface & theSurface)
{
    m_SurfType = surfPlane ;    // The surface is a plane

    gp_Pln plane = theSurface.Plane();      // Get the geometry plane
    plane.Scale(gp_Pnt(0,0,0),GetUnit());   // Scale the face based on unit CAD model used
    m_Axis = plane.Position();              // Get the coordinate system Ax3   

    plane.Coefficients(m_dPrmtA,m_dPrmtB,m_dPrmtC,m_dPrmtD); // Get the parameters of plane

    m_bReverse = Standard_False;    

    /*if(Abs(m_dPrmtA) < 1.0e-5)
       m_dPrmtA = 0.0;
    if(Abs(m_dPrmtB) < 1.0e-5)
       m_dPrmtB = 0.0;
    if(Abs(m_dPrmtC) < 1.0e-5)
       m_dPrmtC = 0.0;
    if(Abs(m_dPrmtD) < 1.0e-5)
       m_dPrmtD = 0.0;*/

    // If the parameter is less than 1.0e-07, it can be take as zero
    if(McCadMathTool::IsEqualZero(m_dPrmtA))
       m_dPrmtA = 0.0;
    if(McCadMathTool::IsEqualZero(m_dPrmtB))
       m_dPrmtB = 0.0;
    if(McCadMathTool::IsEqualZero(m_dPrmtC))
       m_dPrmtC = 0.0;
    if(McCadMathTool::IsEqualZero(m_dPrmtD))
       m_dPrmtD = 0.0;    

    m_Dir.SetCoord(m_dPrmtA,m_dPrmtB,m_dPrmtC);     // Assign the direction of face
    m_PntOnFace = m_Axis.Location();                // Assign a point on the face

    //m_PrmtList = new vector<Standard_Real>();
    GetExpression();
}



/** ********************************************************************
* @brief Get mcnp expression of plane
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
TCollection_AsciiString McCadGeomPlane::GetExpression()
{
    if (!m_SurfExpn.IsEmpty())
    {
        return m_SurfExpn;
    }

    Standard_Character chExpn[255];

    Standard_Real dis_tol = McCadConvertConfig::GetTolerence();

    if (m_dPrmtD != 0.0)
    {
        m_dPrmtD *= -1;
    }

    if ( fabs(m_dPrmtA) > dis_tol && fabs(m_dPrmtB) < dis_tol && fabs(m_dPrmtC) < dis_tol )
    {
        sprintf(chExpn, "PX%8s%15.7f\n", "", m_dPrmtD);

        m_SurfSymb = "PX";
        m_PrmtList.push_back(m_dPrmtD/m_dPrmtA);
        if(m_dPrmtA < 0.0)
        {
            m_dPrmtA *= -1;
            m_dPrmtD *= -1;
            m_bReverse = Standard_True;
            m_Dir.Reverse();
        }
    }
    else if ( fabs(m_dPrmtB) > dis_tol && fabs(m_dPrmtA) < dis_tol && fabs(m_dPrmtC) < dis_tol)
    {
        sprintf(chExpn, "PY%8s%15.7f\n", "",m_dPrmtD);

        m_SurfSymb = "PY";
        m_PrmtList.push_back(m_dPrmtD/m_dPrmtB);
        if(m_dPrmtB < 0.0)
        {
            m_dPrmtB *= -1;
            m_dPrmtD *= -1;
            m_bReverse = Standard_True;
            m_Dir.Reverse();
        }
    }
    else if ( fabs(m_dPrmtC) > dis_tol && fabs(m_dPrmtA) < dis_tol && fabs(m_dPrmtB) < dis_tol)
    {
        sprintf(chExpn, "PZ%8s%15.7f\n", "", m_dPrmtD);

        m_SurfSymb = "PZ";
        m_PrmtList.push_back(m_dPrmtD/m_dPrmtC);
        if(m_dPrmtC < 0.0)
        {
            m_dPrmtC *= -1;
            m_dPrmtD *= -1;
            m_bReverse = Standard_True;
            m_Dir.Reverse();
        }
    }
    else
    {
        sprintf(chExpn, "P %8s%15.7f%5s%15.7f%5s%15.7f%5s%15.7f\n",
                "",m_dPrmtA,"",m_dPrmtB,"",m_dPrmtC,"",m_dPrmtD);

        // Make sign of the m_dPrmtA to be positive
        if (m_dPrmtA < 0.0)
        {
            m_dPrmtA *= -1;
            m_dPrmtB *= -1;
            m_dPrmtC *= -1;
            m_dPrmtD *= -1;
            m_bReverse = Standard_True;
            m_Dir.Reverse();
        }

        m_SurfSymb =  "P";
        m_PrmtList.push_back(m_dPrmtA);
        m_PrmtList.push_back(m_dPrmtB);
        m_PrmtList.push_back(m_dPrmtC);
        m_PrmtList.push_back(m_dPrmtD);
    }

    TCollection_AsciiString szExpn( chExpn );
    szExpn.LeftAdjust();
    szExpn.RightAdjust();
    m_SurfExpn.Copy(szExpn);
    return m_SurfExpn;
}





/** ********************************************************************
* @brief Judge the two planes is same or not
*
* @param IGeomFace *& theSurf
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadGeomPlane::IsEqual(IGeomFace *& theSurf)
{
    assert(theSurf);
    if(theSurf->GetFaceType() != m_SurfType)
    {
        return Standard_False;
    }

    Standard_Real dis_tol = 1.0e-5 /*McCadConvertConfig::GetTolerence()*/;
    Standard_Real angle_tol = 1.0e-3*M_PI; //McCadConvertConfig::GetTolerence();

    /* If the direction and parameter d is same, the two faces are treated as same */
    if( m_Dir.IsEqual( ((McCadGeomPlane*)(theSurf))->GetDir(), angle_tol )
            && ( fabs(m_dPrmtD - ((McCadGeomPlane*)(theSurf))->GetPrmtD()) < dis_tol))
    {
        return Standard_True;
    }
    /* If the direction and parameter d is opposite, the two faces are also same */
    else if (m_Dir.IsOpposite( ((McCadGeomPlane*)(theSurf))->GetDir(), angle_tol )
            && (fabs(m_dPrmtD + ((McCadGeomPlane*)(theSurf))->GetPrmtD()) < dis_tol))
    {
        if (!m_bReverse)
        {
            m_bReverse = Standard_True;
        }
        else
        {
            m_bReverse = Standard_False;
        }
        return Standard_True;
    }

    return Standard_False;
}



/** ********************************************************************
* @brief Get the direction of plane
*
* @param
* @return gp_Dir m_Dir
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
gp_Dir McCadGeomPlane::GetDir() const
{
    return m_Dir;
}



/** ********************************************************************
* @brief Get the value of parameter D
*
* @param
* @return Standard_Real m_dPrmtD
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Real McCadGeomPlane::GetPrmtD() const
{
    return m_dPrmtD;
}



/** ********************************************************************
* @brief Clean the created objects
*
* @param
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadGeomPlane::CleanObj() const
{

}


/** ********************************************************************
* @brief Compared with other faces, get the priorites for surface sorting.
*        The sequence is given at McCadConvertConfig.
*
* @param const IGeomFace *& theGeoFace
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadGeomPlane::Compare(const IGeomFace *& theGeoFace)
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
TCollection_AsciiString McCadGeomPlane::GetTransfNum() const
{
    return "";
}


