#ifndef MCCADREPAIR_HXX
#define MCCADREPAIR_HXX

#include <Standard.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>

class McCadRepair
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
    McCadRepair();
    //McCadRepair(TopoDS_Shape &theShape);
    ~McCadRepair();
    //void Init(TopoDS_Shape &theShape);

    /**< Remove the small faces from solid */
    Standard_EXPORT static TopoDS_Shape RemoveSmallFaces(TopoDS_Shape & theSolid);
    /**< Repair the solid */
    Standard_EXPORT static TopoDS_Solid RepairSolid(TopoDS_Solid &theSolid);

private:
    //TopoDS_Shape m_Shape;
    //TopoDS_Shape m_newShape;

};

#endif // MCCADREPAIR_HXX
