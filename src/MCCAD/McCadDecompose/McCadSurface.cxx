#include "McCadSurface.hxx"

#include <BRepAdaptor_Surface.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>
#include <BRepTools.hxx>


#include "../McCadTool/McCadMathTool.hxx"
#include "../McCadTool/McCadGeomTool.hxx"

McCadSurface::McCadSurface()
{
}

McCadSurface::McCadSurface(const TopoDS_Face &theFace):TopoDS_Face(theFace)
{
    BRepAdaptor_Surface BS(theFace, Standard_True);
    m_AdpSurface = BS.Surface();

    m_iSplitSurfNum = 0;              // How many boundary surfaces the surface through
    m_iSplitCurvSurfNum = 0;          // How many curved boundary surfaces the surface through

    m_bIsAstSurf = Standard_False;    // The surface is assisted splitting surface or not
    m_iInternalLoops = 0;             // The default number of internal loops of surface
}

McCadSurface::~McCadSurface()
{

}


/** ***************************************************************************
* @brief Set surface number
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
void McCadSurface::SetSurfNum(Standard_Integer iSurfNum)
{
    m_iSurfNum = iSurfNum;
}



/** ***************************************************************************
* @brief Get surface number
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Integer McCadSurface::GetSurfNum() const
{
    return m_iSurfNum;
}



/** ***************************************************************************
* @brief  Set how many collied boundary surfaces
* @param  *
* @return void
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
void McCadSurface::SetSplitSurfNum(Standard_Integer iSplitSurfNum)
{
    m_iSplitSurfNum = iSplitSurfNum;
}


/** ***************************************************************************
* @brief  Set how many repeat surfaces number, which used for splitting
*         surfaces sorting
* @param  *
* @return void
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
void McCadSurface::SetRepeatSurfNum(Standard_Integer iRepeatSurfNum)
{
    m_iRepeatSurfNum = iRepeatSurfNum;
}



/** ***************************************************************************
* @brief  Get the number of collied boundary surfaces
* @param
* @return Standard_Integer
*
* @date 13/05/2015
* @modify 21/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Integer McCadSurface::GetSplitSurfNum() const
{
    return m_iSplitSurfNum;
}



/** ***************************************************************************
* @brief  Get the number of collied boundary surfaces
* @param
* @return Standard_Integer
*
* @date 09/03/2016
* @modify 09/03/2016
* @author  Lei Lu
******************************************************************************/
Standard_Integer McCadSurface::GetRepeatSurfNum() const
{
    return m_iRepeatSurfNum;
}



/** ***************************************************************************
* @brief  Get the surface type
* @param
* @return Standard_Integer
*
* @date 13/05/2015
* @modify 21/05/2015
* @author  Lei Lu
******************************************************************************/
SURFTYPE McCadSurface::GetSurfType() const
{
    return m_SurfType;
}




/** ***************************************************************************
* @brief  Calculate the distance between point and surface, if it less than
*         distance tolerence, the point is consided on the surface.
* @param  point(gp_Pnt), tolerence(Standard_Real)
*         Standard_Real fLength                extended length
* @return
*
* @date 08/10/2015
* @modify 08/10/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadSurface::IsPntOnSurf(gp_Pnt &thePnt, Standard_Real disTol)
{
    return Standard_False;
}



/** ***************************************************************************
* @brief The surface is assisted surface or not
* @param
* @return
*
* @date 23/03/2016
* @modify 23/03/2016
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadSurface::IsAstSurf() const
{
    return m_bIsAstSurf;
}




/** ***************************************************************************
* @brief
* @param
* @return
*
* @date 29/03/2016
* @modify 29/03/2016
* @author  Lei Lu
******************************************************************************/
Standard_Integer McCadSurface::GetLoopsNum() const
{    
    return m_iInternalLoops;
}


/** ***************************************************************************
* @brief
* @param
* @return
*
* @date 29/03/2016
* @modify 29/03/2016
* @author  Lei Lu
******************************************************************************/
void McCadSurface::SetSplitCurvSurfNum(Standard_Integer iSplitCurvSurfNum)
{
    m_iSplitCurvSurfNum = iSplitCurvSurfNum;
}


/** ***************************************************************************
* @brief
* @param
* @return
*
* @date 29/03/2016
* @modify 29/03/2016
* @author  Lei Lu
******************************************************************************/
Standard_Integer McCadSurface::SplitCurvSurfNum() const
{
    return m_iSplitCurvSurfNum;
}



/** ***************************************************************************
* @brief  Judge the two surfaces are same surface or not
* @param  McCadBndSurfCylinder *pFace (Compared assisted surface)
* @return Boolean
*
* @date   09/03/2016
* @modify 09/03/2015
* @author Lei Lu
******************************************************************************/
Standard_Boolean McCadSurface::IsSameSurface(const McCadSurface * pFace) const
{
    //TopoDS_Face SurfB = pFace->GetTopoFace();
    if (McCadGeomTool::IsSameSurfaces(*this,*pFace))
    {
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}


/** ***************************************************************************
* @brief  Judge the edge in on the face or not, if the edge is line, just
*         calculate the two vertexes are on the face, if the edge is circle
*         calculate four points from the edge are on the face or not, for avoid
*         the circle is close that start and end points are same point.
* @param  McCadEdge *& pEdge
* @return Standard_Boolean
*
* @date 20/06/2016
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadSurface::IsEdgeOnFace(McCadEdge *& pEdge)
{
    if ( pEdge->GetType() == Line)
    {
        gp_Pnt pntStart = pEdge->StartPoint();
        gp_Pnt pntEnd   = pEdge->EndPoint();

        if (IsPntOnSurf(pntStart,1.0e-4) && IsPntOnSurf(pntEnd, 1.0e-4))
        {
            return Standard_True;
        }
    }
    else
    {
        gp_Pnt pntStart = pEdge->StartPoint();
        gp_Pnt pntMid   = pEdge->MidPoint();
        gp_Pnt pntEnd   = pEdge->EndPoint();
        gp_Pnt pntExtra = pEdge->ExtraPoint(); // If the the edge is close, a extra point is required

        if (IsPntOnSurf(pntStart,1.0e-4) && IsPntOnSurf(pntEnd, 1.0e-4)
                && IsPntOnSurf(pntMid,1.0e-4) && IsPntOnSurf(pntExtra, 1.0e-4) )
        {
            return Standard_True;
        }
    }

    return Standard_False;
}



/** ******************************************************************************
* @brief Set how many concave edges the face go through
* @param Standard_Integer iConcaveEdge
* @return void
*
* @date 13/06/2016
* @modify
* @author Lei Lu
*********************************************************************************/
void McCadSurface::SetThroughConcaveEdges(Standard_Integer iConcaveEdge)
{
    m_iThroughConcaveEdge = iConcaveEdge;
}




/** ******************************************************************************
* @brief Get how many concave edges the face go through
* @param
* @return Standard_Integer
*
* @date 13/06/2016
* @modify
* @author Lei Lu
*********************************************************************************/
Standard_Integer McCadSurface::GetThroughConcaveEdges()
{
    return m_iThroughConcaveEdge;
}
