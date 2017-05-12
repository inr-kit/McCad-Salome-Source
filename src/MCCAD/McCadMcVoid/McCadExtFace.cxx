#include "McCadExtFace.hxx"

#include <Geom_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>

McCadExtFace::McCadExtFace()
{
}

McCadExtFace::McCadExtFace(const TopoDS_Face &theFace):TopoDS_Face(theFace)
{
    /// Set the surface type.
//    TopLoc_Location loc;
//    Handle_Geom_Surface theGeomSurface = BRep_Tool::Surface(*this,loc);
//    GeomAdaptor_Surface theAdaptedSurface(theGeomSurface);
//    m_FaceType = theAdaptedSurface.GetType();

    BRepAdaptor_Surface BS(*this, Standard_True);
    m_AdpFace = BS.Surface();
    m_FaceType = m_AdpFace.GetType();
}


/** ********************************************************************
* @brief Copy construct function
*
* @date 14/3/2013
* @author  Lei Lu
************************************************************************/
McCadExtFace::McCadExtFace(const McCadExtFace &theExtFace):TopoDS_Face(theExtFace)
{
    m_iFaceNum = theExtFace.m_iFaceNum;    
}

McCadExtFace::~McCadExtFace()
{
}


/** ********************************************************************
* @brief Set the number of face according the storing position of face list
*
* @param int iNum
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadExtFace::SetFaceNum(int iNum)
{
    if(this->GetFaceOrientation() == MINUS )
    {
       m_iFaceNum = -1 * iNum ;//* m_Reverse;
    }
    else
    {
       m_iFaceNum = iNum ;//* m_Reverse;
    }
}



/** ********************************************************************
* @brief Get face number which can be used to find the geometry information
*
* @param
* @return Standard_Integer
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Integer McCadExtFace::GetFaceNum()
{
    return m_iFaceNum;
}



/** ********************************************************************
* @brief Get the orientation of face, is forward or reversed
*
* @param
* @return ENUM ORIENTATION (PLUS or MINUS)
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
ORIENTATION McCadExtFace::GetFaceOrientation()
{
    TopAbs_Orientation orient = this->Orientation();
    if (orient == TopAbs_FORWARD)
    {        
        return MINUS;                   // Add minus symbol
    }
    else if(orient == TopAbs_REVERSED)
    {        
        return PLUS;                    // Add plus symbol
    }
}




/** ********************************************************************
* @brief Get the orientation of face, is forward or reversed
*
* @param
* @return ENUM ORIENTATION (PLUS or MINUS)
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
TopAbs_Orientation McCadExtFace::GetOrientation()const
{
    TopAbs_Orientation orient = this->Orientation();
    return orient;
}




/** ********************************************************************
* @brief Get the surface type
*
* @param
* @return GeomAbs_SurfaceType
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
GeomAbs_SurfaceType McCadExtFace::GetFaceType()
{
    return m_FaceType;
}




/** ********************************************************************
* @brief Get the adaptor of the surface
*
* @param
* @return GeomAdaptor_Surface
*
* @date 29/7/2016
* @author  Lei Lu
************************************************************************/
GeomAdaptor_Surface McCadExtFace::GetAdpFace()
{
    return m_AdpFace;
}



