#ifndef MCCADEDGEELLIPSE_HXX
#define MCCADEDGEELLIPSE_HXX

#include "McCadEdge.hxx"

class McCadEdgeEllipse : public McCadEdge
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
    McCadEdgeEllipse();
    McCadEdgeEllipse(const TopoDS_Edge &theEdge);

    ~McCadEdgeEllipse();
};


#endif // MCCADEDGEELLIPSE_HXX
