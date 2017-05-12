#ifndef MCCADSURFACE_HXX
#define MCCADSURFACE_HXX

#include <TopoDS_Face.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Standard.hxx>
#include <vector>

#include "McCadEdge.hxx"

enum SURFTYPE{Plane = 0,
              Cylinder = 1,
              Cone = 2,
              Sphere = 3,
              Torus = 4};

class McCadSurface : public TopoDS_Face
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

    McCadSurface();
    McCadSurface(const TopoDS_Face &theFace);
    virtual ~McCadSurface();

    void SetSurfNum(Standard_Integer iSurfNum); /**< Set the surface number */
    Standard_Integer GetSurfNum() const;        /**< Get the surface number */

    void SetSplitSurfNum(Standard_Integer iSurfNum);    /**< Set how many collied surface */
    Standard_Integer GetSplitSurfNum() const;           /**< Get number of collied surface */
    SURFTYPE GetSurfType() const;                       /**< Get the surface type */

    void SetRepeatSurfNum(Standard_Integer iSurfNum);   /**< Set number of repeat surface */
    Standard_Integer GetRepeatSurfNum() const;          /**< Get number of collied surface */

    Standard_Boolean IsAstSurf() const;                 /**< The surface is assisted surface or not */
    void SetAstSurf(Standard_Boolean iAstSurf);         /**< Set the surface as assisted surface */

    Standard_Integer GetLoopsNum()const ;               /**< Get the number of internal loops */
    Standard_Integer SplitCurvSurfNum() const ;         /**< Get the number of split curved surface */    

    void SetThroughConcaveEdges(Standard_Integer iConcaveEdge); /**< Set how many concave edges the face throught */
    Standard_Integer GetThroughConcaveEdges();                  /**< Get how many concave edges the face throught */

    void SetSplitCurvSurfNum(Standard_Integer iSplitCurvSurfNum);           /**< Set the number of split curved surface */
    Standard_Boolean IsSameSurface(const McCadSurface * pFace) const;       /**< The two surfaces have same geometries or not */

    virtual Standard_Boolean IsEdgeOnFace(McCadEdge *& pEdge);              /**< Judge a edge is included entirly by a face */
    /**< The position between suface and a point */
    virtual Standard_Boolean IsPntOnSurf(gp_Pnt &thePnt, Standard_Real disTol);


protected:

    GeomAdaptor_Surface m_AdpSurface;   /**< adapt surface for geometry extraction */

    Standard_Integer m_iSurfNum;        /**< Surface number */
    Standard_Integer m_iSplitSurfNum;   /**< How many surfaces this surface has collision*/
    SURFTYPE m_SurfType;                /**< Surface type */

    Standard_Integer m_iRepeatSurfNum;  /**< How many repeated surfaces in one solid does it have */

    Standard_Boolean m_bIsAstSurf;      /**< The surface is assisted surface or not */
    Standard_Integer m_iInternalLoops;  /**< How many internal loops does surface have */

    Standard_Integer m_iSplitCurvSurfNum;       /**< How many curved surfaces the split surface throught */
    Standard_Integer m_iThroughConcaveEdge;     /**< How many concave edges the face through*/

};

#endif // MCCADSURFACE_HXX
