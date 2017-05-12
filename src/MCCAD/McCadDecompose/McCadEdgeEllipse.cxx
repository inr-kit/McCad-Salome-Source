#include "McCadEdgeEllipse.hxx"

McCadEdgeEllipse::McCadEdgeEllipse()
{
}

McCadEdgeEllipse::McCadEdgeEllipse(const TopoDS_Edge &theEdge):McCadEdge(theEdge)
{
    m_CurveType = ellipse;
}



McCadEdgeEllipse::~McCadEdgeEllipse()
{

}



