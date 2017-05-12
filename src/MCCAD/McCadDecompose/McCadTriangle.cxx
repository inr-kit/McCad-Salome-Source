#include "McCadTriangle.hxx"
#include <TColgp_HSequenceOfPnt.hxx>

#include <BRepBndLib.hxx>

McCadTriangle::McCadTriangle(const McCadTriangle& theTriangle):TopoDS_Face(theTriangle)
{
    m_iNumOfPnt = theTriangle.m_iNumOfPnt;
    m_iSurfNum = theTriangle.m_iSurfNum;

    m_PntList = new TColgp_HSequenceOfPnt();
    Handle_TColgp_HSequenceOfPnt pnt_list = theTriangle.GetVexList();
    for (Standard_Integer i = 1; i <= pnt_list->Length(); i++)
    {
        gp_Pnt point = pnt_list->Value(i);
        this->AddVertex(point);
    }

    BRepBndLib::Add(*this, m_BndBox);
    m_BndBox.SetGap(0.0);
}


McCadTriangle::McCadTriangle(const TopoDS_Face &theFace):TopoDS_Face(theFace)
{
    m_iNumOfPnt = 0;
    m_iSurfNum = 0;
    m_PntList = new TColgp_HSequenceOfPnt();

    BRepBndLib::Add(*this, m_BndBox);
    m_BndBox.SetGap(0.0);
}


McCadTriangle::~McCadTriangle()
{
    m_PntList->Clear();
}

/** ***************************************************************************
* @brief  Add the vertex into the point list of triangle
* @param
* @return void
*
* @date 15/05/2015
* @modify 15/05/2015
* @author  Lei Lu
******************************************************************************/
void McCadTriangle::AddVertex(gp_Pnt pnt)
{    
    m_PntList->Append(pnt);
    m_iNumOfPnt++;    
}


/** ***************************************************************************
* @brief  Set the father surface which triangle belong to
* @param
* @return void
*
* @date 15/05/2015
* @modify 15/05/2015
* @author  Lei Lu
******************************************************************************/
void McCadTriangle::SetSurfNum(Standard_Integer iSurfNum)
{
    m_iSurfNum = iSurfNum;
}


/** ***************************************************************************
* @brief  Get the vertex list of triangle
* @param
* @return Handle_TColgp_HSequenceOfPnt
*
* @date 15/05/2015
* @modify 15/05/2015
* @author  Lei Lu
******************************************************************************/
Handle_TColgp_HSequenceOfPnt McCadTriangle::GetVexList() const
{
    return m_PntList;
}


/** ***************************************************************************
* @brief  Get the number of surface which triangle belong to
* @param
* @return Handle_TColgp_HSequenceOfPnt
*
* @date 15/05/2015
* @modify 15/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Integer McCadTriangle::GetSurfNum() const
{
    return m_iSurfNum;
}



/** ***************************************************************************
* @brief  Calculate the boundary box of triangle for calculate the collision \
*         between boundary surfaces
* @param
* @return Bnd_Box
*
* @date   15/03/2016
* @modify
* @author Lei Lu
******************************************************************************/
Bnd_Box McCadTriangle::GetBndBox() const
{
    return m_BndBox;
}




