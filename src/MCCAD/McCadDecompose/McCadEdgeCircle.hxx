#ifndef MCCADEDGECIRCLE_HXX
#define MCCADEDGECIRCLE_HXX

#include "McCadEdge.hxx"

class McCadEdgeCircle : public McCadEdge
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
    McCadEdgeCircle();
    McCadEdgeCircle(const TopoDS_Edge &theEdge);
    ~McCadEdgeCircle();



};

#endif // MCCADEDGECIRCLE_HXX
