#ifndef MCCADEDGELINE_HXX
#define MCCADEDGELINE_HXX

#include "McCadEdge.hxx"
#include <gp_Dir.hxx>

class McCadEdgeLine : public McCadEdge
{    
public:

    void* operator new(size_t,void* anAddress)
    {
        return anAddress;
    }
    void* operator new(size_t size)
    {
        return Standard::Allocate(size);
    }
    void  operator delete(void *anAddress)
    {
        if (anAddress) Standard::Free((Standard_Address&)anAddress);
    }

public:
    McCadEdgeLine();
    McCadEdgeLine(const TopoDS_Edge &theEdge);
    ~McCadEdgeLine();

    gp_Dir GetEdgeDir();            /**< Get the direction of edge*/

    /**< The two edges are parellel or not */
    Standard_Boolean IsParallel(McCadEdgeLine *&pEdgeLine, Standard_Real angle_tol);

private:
    gp_Dir m_EdgeDir;               /**< The direction of edge */

};

#endif // MCCADEDGELINE_HXX
