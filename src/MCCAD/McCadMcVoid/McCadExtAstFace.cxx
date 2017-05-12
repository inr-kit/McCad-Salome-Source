#include "McCadExtAstFace.hxx"

McCadExtAstFace::McCadExtAstFace()
{
}

McCadExtAstFace::McCadExtAstFace(const TopoDS_Face &theFace):McCadExtFace(theFace)
{
    m_bSplit = Standard_False;
}

McCadExtAstFace::McCadExtAstFace(const McCadExtAstFace &theExtFace):McCadExtFace(theExtFace)
{
 //qiu   m_bSplit = theExtFace.IsSplitFace();
    m_bSplit = const_cast <McCadExtAstFace&> (theExtFace).IsSplitFace();

}

McCadExtAstFace::~McCadExtAstFace()
{

}


/** ********************************************************************
* @brief The added assisted surface can split the solid into several
*        parts or not
*
* @param
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadExtAstFace::IsSplitFace()
{
    return m_bSplit;
}



/** ********************************************************************
* @brief Set the the surface to be a split surface or not
*
* @param
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadExtAstFace::SetSplit(Standard_Boolean bSplit)
{
    m_bSplit = bSplit;
}
