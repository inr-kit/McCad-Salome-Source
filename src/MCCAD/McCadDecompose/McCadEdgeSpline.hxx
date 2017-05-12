#ifndef MCCADEDGESPLINE_HXX
#define MCCADEDGESPLINE_HXX

#include "McCadEdge.hxx"

class McCadEdgeSpline : public McCadEdge
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
    McCadEdgeSpline();
    McCadEdgeSpline(const TopoDS_Edge &theEdge);
    ~McCadEdgeSpline();
};


#endif // MCCADEDGESPLINE_HXX
