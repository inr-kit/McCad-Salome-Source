#include "McCadEdgeLine.hxx"

#include <BRepAdaptor_Curve.hxx>
#include <BRep_Tool.hxx>

#include <gp_Lin.hxx>


McCadEdgeLine::McCadEdgeLine()
{
}

McCadEdgeLine::McCadEdgeLine(const TopoDS_Edge &theEdge):McCadEdge(theEdge)
{
    m_CurveType = Line;

    BRepAdaptor_Curve baCrv;      // Calculate the normal of edge
    baCrv.Initialize(theEdge);

    gp_Lin line = baCrv.Line();
    m_EdgeDir = line.Direction(); // Set normal direction of edge.
}

McCadEdgeLine::~McCadEdgeLine()
{

}


/** ***************************************************************************
* @brief  The two edges are parallel or not
* @param  McCadDcompEdge * pEdge     given edge for comparison
*         Standard_Real angle_tol    the angle tolenrance
* @return void
*
* @date 04/04/2016
* @modify 04/04/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadEdgeLine::IsParallel(McCadEdgeLine *& pEdgeLine, Standard_Real angle_tol)
{
    if(m_EdgeDir.IsParallel(pEdgeLine->GetEdgeDir(),angle_tol))
    {
         return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}









/** ***************************************************************************
* @brief  Get the direction of edge
* @param
* @return gp_Dir
*
* @date 13/04/2016
* @modify 13/04/2015
* @author  Lei Lu
******************************************************************************/
gp_Dir McCadEdgeLine::GetEdgeDir()
{
    return m_EdgeDir;
}

