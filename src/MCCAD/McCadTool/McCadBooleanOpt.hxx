#ifndef MCCADBOOLEANOPT_HXX
#define MCCADBOOLEANOPT_HXX

#include <Standard.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>

#include <gp_Pnt.hxx>
#include <Bnd_Box.hxx>
#include <vector>
#include <Handle_TopTools_HSequenceOfShape.hxx>
#include <GeomAdaptor_Surface.hxx>

using namespace std;

class McCadBooleanOpt
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

    McCadBooleanOpt();
    McCadBooleanOpt(const TopoDS_Solid &theSolid, const TopoDS_Face &theFace);
    ~McCadBooleanOpt();

private:

    TopoDS_Solid m_InputSolid;                          /**< Input solid for boolean operation */
    TopoDS_Face m_SplitFace;                            /**< Splitting surface */
    TopoDS_Shape m_BndBox;                              /**< The boundary box of split solid */
    Standard_Real m_DglLength;                          /**< The length of diagonal of boundary box */
    Handle_TopTools_HSequenceOfShape m_ResultSolids;    /**< The splitted result solid list */

public:

    /**< Input the solid and splitting surface */
    void InputData(const TopoDS_Solid &theSolid, const TopoDS_Face &theFace);
    /**< Get the result solids after splitting */
    void GetResultSolids(Handle_TopTools_HSequenceOfShape & solid_list);
    Standard_Boolean Perform();                /**< Perform the splitting */


private:

    /**< Split solid with splittin surface */
    Standard_Boolean Split(TopoDS_Face &split_face);
    /**< Split solid with cylinder surface */
    Standard_Boolean SplitCyln(TopoDS_Face & theFace);
    /**< Create extended splitting surface */
    TopoDS_Face CrtExtSplitFace(/*GeomAdaptor_Surface &AdpSurf*/);
    /**< Calculate the positive point and negative point of splitting surface */
    void CalPoints(TopoDS_Face & theFace, gp_Pnt &posPnt, gp_Pnt &negPnt);
    /**< Calculate the boundary box of input solid */
    void CalBndBox();
    /**< Check the errors of output solids */
    Standard_Boolean CheckAndRepair();
    /** Free the solid list */
    void FreeSolidList(Handle_TopTools_HSequenceOfShape & theList);
    /** Rebuild the solid from shells with error */
    Standard_Boolean RebuildSolidFromShell(TopoDS_Solid &theSolid);
    /** Split the solid with positive and negative half boxes */
    Standard_Boolean SplitWithBoxes(TopoDS_Shape &theBoxA, TopoDS_Shape &theBoxB);

    /**< Remove the internal wires of surfaces */
    TopoDS_Shape RemoveInternalWires(TopoDS_Shape & theSolid);
};

#endif // MCCADBOOLEANOPT_HXX
