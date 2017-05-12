#include "McCadGeomCone.hxx"

#include <gp_Cone.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <assert.h>

#include "McCadTransfCard.hxx"
#include "McCadVoidCellManager.hxx"
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadMathTool.hxx"


McCadGeomCone::McCadGeomCone()
{
}

McCadGeomCone::~McCadGeomCone(){}


/** ********************************************************************
* @brief Construct a cone based on the geometry surface
*
* @param
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
McCadGeomCone::McCadGeomCone(const GeomAdaptor_Surface & theSurface)
{
    m_SurfType = surfCone ;                             // The surface is a cone
    gp_Cone cone = theSurface.Cone();                   // Get the geometry of cone
    cone.Scale(gp_Pnt(0,0,0),GetUnit());                  // Scale the face based on unit CAD model used

    m_Axis = cone.Position();                           // Get the coordinate system Ax3
    m_SemiAngle = cone.SemiAngle();                     // Get the semi angle of cone
    m_Dir = m_Axis.Direction();                         // Get the direction of axis
    m_Apex = cone.Apex();                               // Get the peak point of cone

    if( McCadMathTool::IsEqualZero(m_Apex.X()))
        m_Apex.SetX(0.0);
    if( McCadMathTool::IsEqualZero(m_Apex.Y()))
        m_Apex.SetY(0.0);
    if( McCadMathTool::IsEqualZero(m_Apex.Z()))
        m_Apex.SetZ(0.0);

    cone.Coefficients( m_dPrmtA1,m_dPrmtA2,m_dPrmtA3,   // Get the parameters
                       m_dPrmtB1,m_dPrmtB2,m_dPrmtB3,
                       m_dPrmtC1,m_dPrmtC2,m_dPrmtC3,
                       m_dPrmtD);

    m_bReverse = Standard_False;

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

    /* test code
    cout<<"cone"<<endl;
    cout<<m_dPrmtA1<<","<<m_dPrmtA2<<","<<m_dPrmtA3<<endl;
    cout<<m_dPrmtB1<<","<<m_dPrmtB2<<","<<m_dPrmtB3<<endl;
    cout<<m_dPrmtC1<<","<<m_dPrmtC2<<","<<m_dPrmtC3<<endl;
    cout<<m_dPrmtD<<endl;*/

    m_bTransfCrad = Standard_False;
    m_iTrNum = 0;

    //m_PrmtList = new TColStd_HSequenceOfAsciiString();
    GetExpression();
}


/** ********************************************************************
* @brief Get the mcnp expression of cone
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
TCollection_AsciiString McCadGeomCone::GetExpression()
{
    if (!m_SurfExpn.IsEmpty())
    {
        return m_SurfExpn;
    }
    Standard_Character chExpn[255];

    // Create the standard X, Y, Z  axis
    gp_Ax3 Ax3_X(m_Axis.Location(), gp::DX());
    gp_Ax3 Ax3_Y(m_Axis.Location(), gp::DY());
    gp_Ax3 Ax3_Z(m_Axis.Location(), gp::DZ());

    Standard_Real dis_tol = McCadConvertConfig::GetTolerence();
    Standard_Real angle_tol = 1.0e-3; //McCadConvertConfig::GetTolerence();

    if ( m_Axis.IsCoplanar(Ax3_X, dis_tol,angle_tol))
    {
        if (fabs(m_Apex.X()) >= dis_tol
                && fabs(m_Apex.Y()) <= dis_tol
                && fabs(m_Apex.Z()) <= dis_tol)
        {
            //sprintf(chExpn, "KX%8s%15.7f%5s%15.7f\n","",m_Apex.X(),"",pow(tan(m_SemiAngle),2));

            m_SurfSymb = "KX";
            m_PrmtList.push_back(m_Apex.X());
            m_PrmtList.push_back(pow(tan(m_SemiAngle),2));            
        }
        else
        {
            /*sprintf(chExpn, "K/X%8s%15.7f%5s%15.7f%5s%15.7f%5s%15.7f\n",
                            "", m_Apex.X(),
                            "", m_Apex.Y(),
                            "", m_Apex.Z(),
                            "", pow(tan(m_SemiAngle),2));*/

            m_SurfSymb = "K/X";
            m_PrmtList.push_back(m_Apex.X());
            m_PrmtList.push_back(m_Apex.Y());
            m_PrmtList.push_back(m_Apex.Z());
            m_PrmtList.push_back(pow(tan(m_SemiAngle),2));
        }

        if(m_Dir.IsOpposite(gp::DX(),angle_tol))
        {
            m_PrmtList.push_back(-1);
        }
        else
        {
            m_PrmtList.push_back(1);
        }
    }
    else if( m_Axis.IsCoplanar(Ax3_Y, dis_tol,angle_tol))
    {
        if (fabs(m_Apex.Y()) >= dis_tol
                && fabs(m_Apex.X()) <= dis_tol
                && fabs(m_Apex.Z()) <= dis_tol)
        {
            //sprintf(chExpn, "KY%8s%15.7f%5s%15.7f\n","",m_Apex.Y(),"",pow(tan(m_SemiAngle),2));

            m_SurfSymb = "KY";
            m_PrmtList.push_back(m_Apex.Y());
            m_PrmtList.push_back(pow(tan(m_SemiAngle),2));
        }
        else
        {
            /*sprintf(chExpn, "K/Y%8s%15.7f%5s%15.7f%5s%15.7f%5s%15.7f\n",
                            "", m_Apex.X(),
                            "", m_Apex.Y(),
                            "", m_Apex.Z(),
                            "", pow(tan(m_SemiAngle),2));*/

            m_SurfSymb = "K/Y";
            m_PrmtList.push_back(m_Apex.X());
            m_PrmtList.push_back(m_Apex.Y());
            m_PrmtList.push_back(m_Apex.Z());
            m_PrmtList.push_back(pow(tan(m_SemiAngle),2));
        }

        if(m_Dir.IsOpposite(gp::DY(),angle_tol))
        {
            m_PrmtList.push_back(-1);
        }
        else
        {
            m_PrmtList.push_back(1);
        }

    }
    else if( m_Axis.IsCoplanar(Ax3_Z, dis_tol,angle_tol))
    {
        if (fabs(m_Apex.Z()) >= dis_tol
                && fabs(m_Apex.X()) <= dis_tol
                && fabs(m_Apex.Y()) <= dis_tol)
        {
            //sprintf(chExpn, "KZ%8s%15.7f%5s%15.7f\n","",m_Apex.Z(),"",pow(tan(m_SemiAngle),2));

            m_SurfSymb = "KZ";
            m_PrmtList.push_back(m_Apex.Z());
            m_PrmtList.push_back(pow(tan(m_SemiAngle),2));
        }
        else
        {
            /*sprintf(chExpn, "K/Z%8s%15.7f%5s%15.7f%5s%15.7f%5s%15.7f\n",
                            "", m_Apex.X(),
                            "", m_Apex.Y(),
                            "", m_Apex.Z(),
                            "", pow(tan(m_SemiAngle),2));*/

            m_SurfSymb = "K/Z";
            m_PrmtList.push_back(m_Apex.X());
            m_PrmtList.push_back(m_Apex.Y());
            m_PrmtList.push_back(m_Apex.Z());
            m_PrmtList.push_back(pow(tan(m_SemiAngle),2));
        }

        if(m_Dir.IsOpposite(gp::DZ(),angle_tol))
        {
            m_PrmtList.push_back(-1);
        }
        else
        {
            m_PrmtList.push_back(1);
        }
    }
    else
    {
        /*sprintf(chExpn, "GQ%8s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f%5s%15.7f%5s%15.7f\n%10s%15.7f\n",
                "",m_dPrmtA1,"",m_dPrmtA2,"",m_dPrmtA3,
                "",2*m_dPrmtB1,"",2*m_dPrmtB2,"",2*m_dPrmtB3,
                "",2*m_dPrmtC1,"",2*m_dPrmtC2,"",2*m_dPrmtC3,"",m_dPrmtD);*/

       // m_SurfSymb = "KZ";
       // m_PrmtList.push_back(0);
       // m_PrmtList.push_back(pow(tan(m_SemiAngle),2));
        //m_PrmtList.push_back(1);

       // m_bTransfCrad = Standard_True;

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
* @brief Add the transform card
*
* @param
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadGeomCone::AddTransfCard(McCadGeomData * pData)
{
    m_iTrNum = pData->AddTransfCard(m_Axis,m_Apex);
}



/** ********************************************************************
* @brief Get the transform card number
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
TCollection_AsciiString McCadGeomCone::GetTransfNum()const
{
    if(m_iTrNum != 0)
    {
        TCollection_AsciiString strNum(m_iTrNum);
        return strNum;
    }
    else
    {
        return "";
    }

}



/** ********************************************************************
* @brief If the surface has transform card
*
* @param
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadGeomCone::HaveTransfCard()
{
    return m_bTransfCrad;
}




/** ********************************************************************
* @brief Judge the geometry of two cones are same or not
*
* @param IGeomFace *& theSurf
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadGeomCone::IsEqual(IGeomFace *& theSurf)
{
    assert(theSurf);
    if(theSurf->GetFaceType() != m_SurfType)
    {
        return Standard_False;
    }

    McCadGeomCone *pCone = (McCadGeomCone*)(theSurf);
    assert(pCone);

    Standard_Real dis_tol = McCadConvertConfig::GetTolerence();
    Standard_Real angle_tol = 1.0e-3*M_PI; //McCadConvertConfig::GetTolerence();

    // If the Axis is same or opposite, the radius is same and the point on axis is same
    // the two cylinder can be treated as same cylinder
    if( m_Dir.IsEqual(pCone->GetAxisDir(), angle_tol ))
            //|| m_Dir.IsOpposite( pCone->GetAxisDir(), 1.0e-03 ))
    {
         if ( fabs(m_SemiAngle - pCone->GetSemiAngle()) < dis_tol
                    && m_Apex.IsEqual(pCone->GetApex(),dis_tol))
            {
                return Standard_True;
            }
    }

    return Standard_False;
}



/** ********************************************************************
* @brief Get the peak point of the cone
*
* @param
* @return gp_Pnt m_Apex
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
gp_Pnt McCadGeomCone::GetApex() const
{
    return m_Apex;
}

/** ********************************************************************
* @brief Get the semi angle of the cone
*
* @param
* @return Standard_Real m_SemiAngle
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Real McCadGeomCone::GetSemiAngle() const
{
    return m_SemiAngle;
}


/** ********************************************************************
* @brief Get the direction of axis of the cone
*
* @param
* @return gp_Dir m_Dir
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
gp_Dir McCadGeomCone::GetAxisDir() const
{
    return m_Dir;
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
void McCadGeomCone::CleanObj() const
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
Standard_Boolean McCadGeomCone::Compare(const IGeomFace *& theGeoFace)
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
