#include "McCadEdgeSpline.hxx"

McCadEdgeSpline::McCadEdgeSpline()
{
}


McCadEdgeSpline::McCadEdgeSpline(const TopoDS_Edge &theEdge):McCadEdge(theEdge)
{
    m_CurveType = Spline;
}


McCadEdgeSpline::~McCadEdgeSpline()
{
}
