#include "McCadEdgeCircle.hxx"


McCadEdgeCircle::McCadEdgeCircle()
{
}

McCadEdgeCircle::McCadEdgeCircle(const TopoDS_Edge &theEdge):McCadEdge(theEdge)
{
    m_CurveType = Circle;
}

McCadEdgeCircle::~McCadEdgeCircle()
{

}
