#ifndef MCCADSPLITCYLNPLN_HXX
#define MCCADSPLITCYLNPLN_HXX

#include "McCadSurface.hxx"
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Solid.hxx>
#include <Handle_TopTools_HSequenceOfShape.hxx>

#include "McCadBndSurfCylinder.hxx"
#include "McCadAstSurfPlane.hxx"

#include "McCadDcompSolid.hxx"

using namespace std;

class McCadSplitCylnPln
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

    McCadSplitCylnPln();
    ~McCadSplitCylnPln();    

public:
    /**< Generate assiste splitting surfaces and add into the assisted surface list */
    void GenSplitSurfaces( McCadDcompSolid *& pSolid );   

           // const vector<McCadBndSurface *> &cylinder_list,
           // const vector<McCadBndSurface *> &plane_list,
           // vector<McCadAstSurfPlane*> *& AstFace_list);

private:     

    McCadAstSurfPlane* CrtSplitSurfThroughEdge(McCadBndSurfCylinder *& pCylnFace,McCadEdge *& pEdge);
    McCadAstSurfPlane* CrtSplitSurfThroughTwoEdges(McCadEdge *& pEdgeA, McCadEdge *& pEdgeB);
    void CrtSplitSurfaces(McCadBndSurfCylinder *& pCylnFace, vector<McCadAstSurface *> &astFace_list);

    Standard_Boolean FindComLineEdge(McCadBndSurfCylinder *&pSurfCyln,
                                     McCadBndSurface *& pSurfPln);

    //Standard_Boolean HasStraightEdge(TopoDS_Face theFace);            /**< If the face has straight edge */
    //void MergeFaces(vector<McCadAstSurfPlane*> *& AstFace_list);        /**< Merge the two faces */
    Standard_Real CalCurveRadian(McCadBndSurface *& pBndCyln);      /**< Calculate the radian of curved surface */
    void MergeSurfaces(vector<McCadAstSurface*> & theAstFaceList);  /**< Merge the repeated surfaces */


    //    void GenSurfThroughEdge(McCadBndSurfCylinder *&pCylnFace,
    //                            TopoDS_Edge & theEdge,
    //                            vector<McCadAstSurfPlane *> *& AstSurf_list);   /**< Generate splitting surfaces */
    //    void GenAssistSurfaces(McCadBndSurfCylinder *& pCylnFace,
    //                           vector<McCadAstSurfPlane*> *& AstFace_list);     /**< Generate assisted surface */


private:
    Standard_Real m_fLength;            /**< The length of input solid which deside the size of created splitting surface */
};

#endif // MCCADSPLITCYLNPLN_HXX
