#ifndef MCCADSPLITCYLINDERS_HXX
#define MCCADSPLITCYLINDERS_HXX

#include <vector>

#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <Handle_TopTools_HSequenceOfShape.hxx>

#include "McCadSurface.hxx"
#include "McCadAstSurfPlane.hxx"
#include "McCadBndSurfCylinder.hxx"
#include "McCadDcompSolid.hxx"

using namespace std;

class McCadSplitCylinders
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

    McCadSplitCylinders();
    ~McCadSplitCylinders();

public:

    void GenSplitSurfaces(McCadDcompSolid *& pSolid);  /**< Generate the splitting surface */

private:   

    /**< Merge the same split surfaces and combine the surfaces which are too close */
    void CombAndMergeSurfaces(vector<McCadAstSurface *> &theAstFaceList);

    /**< Two cylinders have common curved edges */
    Standard_Boolean HasComCurvEdge(McCadBndSurfCylinder *& pSurfA,
                                     McCadBndSurfCylinder *& pSurfB,
                                     McCadEdge *&pEdge);

    /**< Two cylinders have common straight edges */
    Standard_Boolean HasComLineEdge(McCadBndSurfCylinder *&pSurfA,
                                    McCadBndSurfCylinder *&pSurfB,
                                    vector<McCadEdge *> *&pEdgeList);

    /** Generate splitting surface through the curve */
    McCadAstSurfPlane * GenSurfThroughCurves(McCadEdge *&pEdge);
    /**< Generate splitting surfaces to seperate two surfaces */
    McCadAstSurfPlane * GenSurfThroughEdge(TopoDS_Face &theFaceA, TopoDS_Face &theFaceB, McCadEdge *&pEdge);
    /**< Generate the splitting surface throught edges */
    McCadAstSurfPlane * GenSurfThroughTwoEdges(McCadEdgeLine *&pEdgeA, McCadEdgeLine *&pEdgeB);
    /**< The two assisted splitting surfaces can be combined or not */
    Standard_Boolean CanCombTwoAstSurfaces(McCadAstSurfPlane *& pFaceA,McCadAstSurfPlane *& pFaceB,
                                           Standard_Real DisTolerence,Standard_Real AngleTolerence);
    /**< Combine the two assited splitting surfaces */
    McCadAstSurfPlane * CombSurfaces(McCadAstSurfPlane *& pFaceA,McCadAstSurfPlane *& pFaceB);

private:
    Standard_Real m_fLength;  /**< The length of input solid which deside the size of created splitting surface */

};

#endif // MCCADSPLITCYLINDERS_HXX
