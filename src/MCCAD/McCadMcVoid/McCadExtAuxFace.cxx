#include "McCadExtAuxFace.hxx"

McCadExtAuxFace::McCadExtAuxFace()
{
}

Standard_Integer McCadExtAuxFace::GetConvexity()
{
    return m_iConvexity;
}
/** ********************************************************************
* @brief Construct function
*
* @param const TopoDS_Face &theFace
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
McCadExtAuxFace::McCadExtAuxFace(const TopoDS_Face &theFace):McCadExtFace(theFace)
{

}



/** ********************************************************************
* @brief Copy construct function
*
* @param const McCadExtFace &theExtFace
* @return
*
* @date 14/3/2013
* @author  Lei Lu
************************************************************************/
McCadExtAuxFace::McCadExtAuxFace(const McCadExtAuxFace &theExtFace)
{

}


/** ********************************************************************
* @brief Remove the auxiliary face
*
* @param Standard_Integer i
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
McCadExtAuxFace::~McCadExtAuxFace()
{

}
