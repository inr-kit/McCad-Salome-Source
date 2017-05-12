#ifndef MCCADDCOMPSOLID_HXX
#define MCCADDCOMPSOLID_HXX

#include <TopoDS_Solid.hxx>
#include <Handle_TopTools_HSequenceOfShape.hxx>
#include "McCadSurface.hxx"

#include "McCadBndSurface.hxx"
#include "McCadAstSurfPlane.hxx"

#include "McCadSelSplitSurf.hxx"

#include <STEPControl_Writer.hxx>

#include "McCadDcompGeomData.hxx"

class McCadDcompSolid : public TopoDS_Solid
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
    McCadDcompSolid();
    McCadDcompSolid(const TopoDS_Solid &theSolid);
    ~McCadDcompSolid();


public:    

    /**< Set the deflection for meshing */
    void SetDeflection(Standard_Real deflection);
    /**< Start the solid decomposition */
    Standard_Boolean Decompose(vector<McCadDcompSolid*> *& pDcompSolidList,
                               vector<McCadDcompSolid*> *& pErrorSolidList,
                               Standard_Integer & iLevel, int iSolid);

private:

    /**< Delete the allocated surface lists */
    void Free();
    /**< Generate a bundary surface according to input TopoDS Face */
    McCadBndSurface* GenSurface(TopoDS_Face face,Standard_Integer iSurfType);    
    /**< Calculate the convexities of the edges */
    void CalEdgeConvexity();
    /**< Trace the edges, generate the mccad edge class */
    void GenEdges(McCadBndSurface *& pBndSurf);

    /**< Select the splitting surface */
    McCadSurface* SelectSplitSurface(Standard_Integer iSel);

    /**< Merge the curved surfaces with same geometries and common edge */
    void MergeSurfaces(vector<McCadBndSurface *> &faceList);
    /**< Judge the bundary surfaces are splitting surfaces or not */
    void JudgeDecomposeSurface();
    /**< Generate the assisted splitting surfaces which are not bundary surfaces */
    void GenAssistSurfaces();
    /**< Judge the assisted surfaces are splitting surfaces or not */
    void JudgeAssistedDecomposeSurfaces();

    /**< Generate the surfaces list of solid */
    void GenSurfaceList();
    /**< Load the boolean operation, decompose the solid with splitting surfaces */
    Standard_Boolean SplitSolid(McCadSurface *& pSplitSurf,
                    Handle_TopTools_HSequenceOfShape &solid_list);
    /**< Judge each face through how many concave edges */
    void JudgeThroughConcaveEdges(vector<McCadBndSurface*> & theFaceList);
    void JudgeThroughConcaveEdges(vector<McCadAstSurface*> & theFaceList);
private:

    TopoDS_Solid m_Solid;                                   /**< TopoDS solid */

    vector<McCadBndSurface*> m_FaceList;                    /**< The bundary face list of solid */
    vector<McCadBndSurface*> m_PlaneList;                   /**< The bundary planes of solid */
    vector<McCadBndSurface*> m_CylinderList;                /**< The bundary cylinders of solid */
    vector<McCadSurface*> m_SplitFaceList;                  /**< The splitting face list */

    vector<McCadAstSurface*> m_AstFaceList;                 /**< The assisted splitting surface list */

    Standard_Boolean m_HasSplitSurf;                        /**< If the solid has splittng surface*/
    vector<McCadSurface*> m_SelSplitFaceList;               /**< The splitting surfaces after sorting */

    Standard_Real m_fDeflection;                            /**< The deflection for surface meshing */
    McCadSelSplitSurf *m_pSelSplitSurf;                     /**< Select the split surface */
    Standard_Real m_fBoxSqLength;                           /**< The length of squared diagonal of boundary box */


public:

    friend class McCadSplitCylnPln;
    friend class McCadSplitCylinders;

};

#endif // MCCADSOLID_HXX
