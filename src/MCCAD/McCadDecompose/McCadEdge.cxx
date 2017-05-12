#include "McCadEdge.hxx"
#include "../McCadTool/McCadGeomTool.hxx"

#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>

McCadEdge::McCadEdge()
{
}

McCadEdge::McCadEdge(const TopoDS_Edge &theEdge):TopoDS_Edge(theEdge)
{
    m_bCanSplit  =  Standard_False;
    m_bCalPoints =  Standard_False;
    m_iConvexity =  100;
}

McCadEdge::~McCadEdge()
{

}


/** ***************************************************************************
* @brief  Get the type of edge
* @param
* @return CURVE
*
* @date 06/05/2016
* @modify 06/05/2016
* @author  Lei Lu
******************************************************************************/
CURVE McCadEdge::GetType()
{
    return m_CurveType;
}



/** ***************************************************************************
* @brief  The two edges are same or not, if they have two same vertexes which
*         are consided as same edges
* @param  McCadDcompEdge * pEdge     given edge for comparison
*         Standard_Real dis          the distance tolenrance of two vertexes
* @return Standard_Boolean
*
* @date 15/04/2016
* @modify 15/04/2016
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadEdge::IsSame(McCadEdge *& pEdge,Standard_Real dis)
{
    if (McCadGeomTool::IsSameEdge(*this, *pEdge, dis))
    {
        return Standard_True;
    }
    else
    {
       return Standard_False;
    }
}




/** ***************************************************************************
* @brief Copy the attributions of edges
* @param McCadEdge *& theEdge
* @return void
*
* @date 13/06/2016
* @modify 13/06/2016
* @author  Lei Lu
******************************************************************************/
void McCadEdge::CopyAttri(McCadEdge *& theEdge)
{
    m_bCanSplit = theEdge->CanAddAstSplitSurf();
    m_bIsConcave = theEdge->IsConcaveEdge();
}




/** ***************************************************************************
* @brief Set the edge can be used for adding splitting surfaces
* @param Standard_Boolean bSplit
* @return void
*
* @date 13/06/2016
* @modify 13/06/2016
* @author  Lei Lu
******************************************************************************/
void McCadEdge::AddAstSplitSurf(Standard_Boolean bSplit)
{
    m_bCanSplit = bSplit;
}



/** ***************************************************************************
* @brief The edge is can be added assisted splitting surface or not
* @param
* @return Standard_Boolean
*
* @date 13/06/2016
* @modify 13/06/2016
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadEdge::CanAddAstSplitSurf()
{
    return m_bCanSplit;
}



/** ***************************************************************************
* @brief Set the convexity of edge, 1 is convex, -1 is concave, 0 is flat
* @param Standard_Integer bConvexity
* @return void
*
* @date 13/06/2016
* @modify 13/06/2016
* @author  Lei Lu
******************************************************************************/
void McCadEdge::SetConvexity(Standard_Integer bConvexity)
{
    m_iConvexity = bConvexity;
}



/** ***************************************************************************
* @brief The ege is concave or not
* @param
* @return Standard_Boolean
*
* @date 13/06/2016
* @modify 13/06/2016
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadEdge::IsConcaveEdge()
{
    if( m_iConvexity == -1 || m_iConvexity == 0)
    {
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}



/** ***************************************************************************
* @brief Calculate the points of edge, if the points has already been calculated
*        it can be used directly.
* @param
* @return void
*
* @date 13/06/2016
* @modify 13/06/2016
* @author  Lei Lu
******************************************************************************/
void McCadEdge::CalPoints()
{
    if(m_bCalPoints)
    {
        return;
    }

    Standard_Real fStart, fEnd;
    Handle(Geom_Curve) theCurve = BRep_Tool::Curve(*this, fStart, fEnd);

    theCurve->D0(fStart, m_pntStart);
    theCurve->D0(fEnd, m_pntEnd);

    Standard_Real fMid = (fStart+fEnd)/2.0;
    theCurve->D0(fMid, m_pntMid);

    Standard_Real fExt = (fStart+fEnd)/4.0;
    theCurve->D0(fExt, m_pntExtra);

    m_bCalPoints = Standard_True;
}




/** ***************************************************************************
* @brief Get the start point
* @param
* @return gp_Pnt
*
* @date 13/06/2016
* @modify
* @author  Lei Lu
******************************************************************************/
gp_Pnt McCadEdge::StartPoint()
{
    CalPoints();
    return m_pntStart;
}



/** ***************************************************************************
* @brief Get the end point
* @param
* @return gp_Pnt
*
* @date 13/06/2016
* @modify
* @author  Lei Lu
******************************************************************************/
gp_Pnt McCadEdge::EndPoint()
{
    CalPoints();
    return m_pntEnd;
}



/** ***************************************************************************
* @brief  Get the middle point
* @param
* @return gp_Pnt
*
* @date 13/06/2016
* @modify
* @author  Lei Lu
******************************************************************************/
gp_Pnt McCadEdge::MidPoint()
{
    CalPoints();
    return m_pntMid;
}



/** ***************************************************************************
* @brief Get the extra point, if the edge is close, the start point and end
*        point are same, so extra point is required.
* @param
* @return gp_Pnt
*
* @date 13/06/2016
* @modify 13/06/2016
* @author  Lei Lu
******************************************************************************/
gp_Pnt McCadEdge::ExtraPoint()
{
    CalPoints();
    return m_pntExtra;
}
