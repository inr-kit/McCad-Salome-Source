#include "McCadGeomCylinder.hxx"

#include <sstream>

#include <gp_Cylinder.hxx>
#include <Standard_IStream.hxx>
#include <gp_Pnt.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <assert.h>
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadMathTool.hxx"

using namespace std;

McCadGeomCylinder::McCadGeomCylinder()
{

}

McCadGeomCylinder::McCadGeomCylinder(const GeomAdaptor_Surface & theSurface)
{
    m_SurfType = surfCylinder ;                         // The surface is a cylinder
    gp_Cylinder cyln = theSurface.Cylinder();           // Get the geometry of cylinder
    cyln.Scale(gp_Pnt(0,0,0),GetUnit());                // Scale the face based on unit CAD model used

    m_Axis = cyln.Position();                           // Get the coordinate system Ax3
    m_Radius = cyln.Radius();                           // Get the radius
    m_Point = m_Axis.Location();                        // Assign a point on the axis
    m_Dir = m_Axis.Direction();                         // Get the direction of axis
    m_vec.SetXYZ(m_Dir.XYZ());
    m_vec.Normalize();

    if( McCadMathTool::IsEqualZero(m_Point.X()))
        m_Point.SetX(0.0);
    if( McCadMathTool::IsEqualZero(m_Point.Y()))
        m_Point.SetY(0.0);
    if( McCadMathTool::IsEqualZero(m_Point.Z()))
        m_Point.SetZ(0.0);

    cyln.Coefficients( m_dPrmtA1,m_dPrmtA2,m_dPrmtA3,   // Get the parameters
                       m_dPrmtB1,m_dPrmtB2,m_dPrmtB3,
                       m_dPrmtC1,m_dPrmtC2,m_dPrmtC3,
                       m_dPrmtD);

    m_bReverse = Standard_False;

    /*SimplifyPrmt(m_dPrmtA1);
    SimplifyPrmt(m_dPrmtA2);
    SimplifyPrmt(m_dPrmtA3);
    SimplifyPrmt(m_dPrmtB1);
    SimplifyPrmt(m_dPrmtB2);
    SimplifyPrmt(m_dPrmtB3);
    SimplifyPrmt(m_dPrmtC1);
    SimplifyPrmt(m_dPrmtC2);
    SimplifyPrmt(m_dPrmtC3);
    SimplifyPrmt(m_dPrmtD);*/

    // Make sign of the m_dPrmtA to be positive
    if (m_dPrmtA1 < 0.0)
    {
        m_dPrmtA1 *= -1;
        m_dPrmtA2 *= -1;
        m_dPrmtA3 *= -1;
        m_dPrmtB1 *= -1;
        m_dPrmtB2 *= -1;
        m_dPrmtB3 *= -1;
        m_dPrmtC1 *= -1;
        m_dPrmtC2 *= -1;
        m_dPrmtC3 *= -1;
        m_dPrmtD *= -1;

        m_bReverse = Standard_True;
    }

    GetExpression();
}


McCadGeomCylinder::~McCadGeomCylinder()
{
    m_PrmtList.clear();
}


void McCadGeomCylinder::SimplifyPrmt(Standard_Real &thePrmt)
{
    if(McCadMathTool::IsEqualZero(thePrmt))
    {
       thePrmt = 0.0;
    }
}



/** ********************************************************************
* @brief
*
* @param
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
TCollection_AsciiString McCadGeomCylinder::GetExpression()
{
    if (!m_SurfExpn.IsEmpty())
    {
        return m_SurfExpn;
    }

    Standard_Character chExpn[255];

    Standard_Real dis_tol = McCadConvertConfig::GetTolerence();
    Standard_Real angle_tol = 1.0e-3; //McCadConvertConfig::GetTolerence();

    // Create the standard X, Y, Z  axis
    gp_Ax3 Ax3_X(m_Point, gp::DX());
    gp_Ax3 Ax3_Y(m_Point, gp::DY());
    gp_Ax3 Ax3_Z(m_Point, gp::DZ());

    if (m_Axis.IsCoplanar(Ax3_X, dis_tol, angle_tol))
    {
        if (fabs(m_Point.Y()) <= dis_tol && fabs(m_Point.Z()) <= dis_tol)
        {
            //sprintf(chExpn, "CX%8s%15.7f\n", "", m_Radius);

            m_SurfSymb = "CX";
            m_PrmtList.push_back(m_Radius);
        }
        else
        {
            /*sprintf(chExpn, "C/X%8s%15.7f%5s%15.7f%5s%15.7f\n",
                            "", m_Point.Y(),
                            "", m_Point.Z(),
                            "", m_Radius);*/

            m_SurfSymb = "C/X";
            m_PrmtList.push_back(m_Point.Y());
            m_PrmtList.push_back(m_Point.Z());
            m_PrmtList.push_back(m_Radius);
        }
    }
    else if (m_Axis.IsCoplanar(Ax3_Y, dis_tol, angle_tol))
    {
        if (fabs(m_Point.X()) <= dis_tol && fabs(m_Point.Z()) <= dis_tol)
        {
            //sprintf(chExpn, "CY%8s%15.7f\n", "", m_Radius);

            m_SurfSymb = "CY";
            m_PrmtList.push_back(m_Radius);
        }
        else
        {
            /*sprintf(chExpn, "C/Y%8s%15.7f%5s%15.7f%5s%15.7f\n",
                                "", m_Point.X(),
                                "", m_Point.Z(),
                                "", m_Radius);*/

            m_SurfSymb = "C/Y";
            m_PrmtList.push_back(m_Point.X());
            m_PrmtList.push_back(m_Point.Z());
            m_PrmtList.push_back(m_Radius);
        }
    }
    else if (m_Axis.IsCoplanar(Ax3_Z, dis_tol, angle_tol))
    {
        if (fabs(m_Point.X()) <= dis_tol && fabs(m_Point.Y()) <= dis_tol)
        {
            //sprintf(chExpn, "CZ%8s%15.7f\n", "", m_Radius);

            m_SurfSymb = "CZ";
            m_PrmtList.push_back(m_Radius);
        }
        else
        {
            /*sprintf(chExpn, "C/Z%8s%15.7f%5s%15.7f%5s%15.7f\n",
                                "", m_Point.X(),
                                "", m_Point.Y(),
                                "", m_Radius);*/

            m_SurfSymb = "C/Z";
            m_PrmtList.push_back(m_Point.X());
            m_PrmtList.push_back(m_Point.Y());
            m_PrmtList.push_back(m_Radius);
        }
    }
    else
    {
       sprintf(chExpn, "GQ%8s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f\n",
                "",m_dPrmtA1,"",m_dPrmtA2,"",m_dPrmtA3,
                "",2*m_dPrmtB1,"",2*m_dPrmtB2,"",2*m_dPrmtB3,
                "",2*m_dPrmtC1,"",2*m_dPrmtC2,"",2*m_dPrmtC3,"",m_dPrmtD);

        m_SurfSymb = "GQ";
        m_PrmtList.push_back(m_dPrmtA1);
        m_PrmtList.push_back(m_dPrmtA2);
        m_PrmtList.push_back(m_dPrmtA3);
        m_PrmtList.push_back(2*m_dPrmtB1);
        m_PrmtList.push_back(2*m_dPrmtB3);
        m_PrmtList.push_back(2*m_dPrmtB2);
        m_PrmtList.push_back(2*m_dPrmtC1);
        m_PrmtList.push_back(2*m_dPrmtC2);
        m_PrmtList.push_back(2*m_dPrmtC3);
        m_PrmtList.push_back(m_dPrmtD);
    }

    TCollection_AsciiString szExpn( chExpn );
    szExpn.LeftAdjust();
    szExpn.RightAdjust();

    m_SurfExpn.Copy(szExpn);
    return m_SurfExpn;
}



/** ********************************************************************
* @brief Judge the geometry of two cylinders are same or not
*
* @param IGeomFace *& theSurf
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadGeomCylinder::IsEqual(IGeomFace *& theSurf)
{
    assert(theSurf);
    if(theSurf->GetFaceType() != m_SurfType)
    {
        return Standard_False;
    }

    McCadGeomCylinder *pCyln = (McCadGeomCylinder*)(theSurf);
    assert(pCyln);

    //Standard_Real dis_tol = 1.0e-5;
    Standard_Real dis_tol = McCadConvertConfig::GetTolerence();
    Standard_Real angle_tol = 1.0e-3*M_PI; //McCadConvertConfig::GetTolerence();

    // If the Axis is same or opposite, the radius is same and the point on axis is same
    // the two cylinder can be treated as same cylinder
    if( m_vec.IsParallel(pCyln->GetAxisDir(), angle_tol )
            || m_vec.IsOpposite( pCyln->GetAxisDir(), angle_tol ))
    {
         if ( fabs(m_Radius - pCyln->GetRadius()) < dis_tol
                    /*&& m_Point.IsEqual(pCyln->GetPoint(),dis_tol)*/)
         {

             if(!m_Point.IsEqual(pCyln->GetPoint(),dis_tol))
             {
                 gp_Dir line_dir(m_Point.XYZ()- pCyln->GetPoint().XYZ());
                 if(line_dir.IsParallel(m_Dir, angle_tol) || line_dir.IsOpposite(m_Dir, angle_tol))
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
                 return Standard_True;
             }
         }
    }
    return Standard_False;
}






/** ********************************************************************
* @brief Get the direction of axis of cylinder
*
* @param
* @return gp_Dir m_Dir
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
gp_Dir McCadGeomCylinder::GetAxisDir() const
{
    return m_Dir;
}




/** ********************************************************************
* @brief Get the center of bottom section
*
* @param
* @return gp_Pnt m_Point
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
gp_Pnt McCadGeomCylinder::GetPoint() const
{
    return m_Point;
}




/** ********************************************************************
* @brief Get the radius of section
*
* @param
* @return Standard_Real m_Radius
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Real McCadGeomCylinder::GetRadius() const
{
    return m_Radius;
}


/** ********************************************************************
* @brief Clean the created objects
*
* @param
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadGeomCylinder::CleanObj() const
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
Standard_Boolean McCadGeomCylinder::Compare(const IGeomFace *& theGeoFace)
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
TCollection_AsciiString McCadGeomCylinder::GetTransfNum()const
{
    return "";
}

