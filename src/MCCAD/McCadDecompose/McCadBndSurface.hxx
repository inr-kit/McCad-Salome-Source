#ifndef MCCADBNDSURFACE_HXX
#define MCCADBNDSURFACE_HXX

#include "McCadTriangle.hxx"
#include "McCadSurface.hxx"
#include "McCadEdge.hxx"

#include <Bnd_Box.hxx>

/** The positional relationship between triangle and face or two faces
    1 is positive, -1 is negative, 0 is on the face*/
enum POSITION{POSITIVE = 1, NEGATIVE = -1, MIDDLE = 0};

class McCadBndSurface : public McCadSurface
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

    McCadBndSurface();
    McCadBndSurface(const TopoDS_Face &theFace);
    virtual ~McCadBndSurface();

public:

    Standard_Boolean GenTriangles(Standard_Real aDeflection);   /**< Generate Triangles of face */
    vector<McCadTriangle *> GetTriangleList();                  /**< Get triangle list */

    /**< Combine the triangles of two surfaces */
    void AddTriangles(McCadBndSurface *& pFace);
    /**< Detect the face is collied with other surface or not */
    virtual Standard_Boolean FaceCollision(McCadBndSurface *& pBndFace, POSITION & ePosition);    
    /**< The position between the surface and triangle */
    virtual Standard_Boolean TriangleCollision(McCadTriangle *& pTri, POSITION & eSide) = 0;
    /**< The two surfaces can be fused or not */
    virtual Standard_Boolean CanFuseSurf(McCadBndSurface *& pSurf) = 0;
    /**< Fuse two same surfaces */
    virtual TopoDS_Face FuseSurfaces(McCadBndSurface *& pSurf) = 0;

    /**< When merge two surfaces, combine the edges of them */
    void CombineEdges(McCadBndSurface *& pSurfA, McCadBndSurface *& pSurfB );

    vector<McCadEdge*> GetEdgeList() const;         /**< Get the edge list */
    void AddEdge(McCadEdge *& pEdge);               /**< Add a edge into edge list */
    void SetLoopNum(Standard_Integer iLoopNum);     /**< Set the inner loop numbers */

protected:

    void Free();                                /**< Free the triangles of surface */
    McCadEdge* CopyEdge(McCadEdge *& pEdge);    /**< Copy McCadEdge object */
    Standard_Integer CountInternalLoops();      /**< The boundary surface has internal loop or not */

protected:

    vector<McCadTriangle*> m_TriangleList;      /**< Triangle list of surface */
    vector<McCadEdge*> m_EdgeList;              /**< Edge list */
    //Bnd_Box m_BndBox;                           /**< The boundary box */

};

#endif // MCCADBNDSURFACE_HXX
