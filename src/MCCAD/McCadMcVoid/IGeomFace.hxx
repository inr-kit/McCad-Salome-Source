#ifndef IGEOMFACE_HXX
#define IGEOMFACE_HXX

#include <TCollection_AsciiString.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pnt.hxx>
#include <Handle_TColStd_HSequenceOfAsciiString.hxx>
#include <vector>

#ifndef _Standard_TypeDef_HeaderFile
#include <Standard_TypeDef.hxx>
#endif

using namespace std;

enum SurfType { surfPlane    = 0,
                surfCylinder = 1,
                surfSphere   = 2,
                surfCone     = 3,
                surfTorus    = 4,
                surfRev      = 5};

class IGeomFace
{
public:    
    IGeomFace(){}
    virtual ~IGeomFace(){}

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

    virtual TCollection_AsciiString GetExpression() = 0;            /**< Get the expression of surface */
    virtual Standard_Boolean IsEqual(IGeomFace *& theGeoFace) = 0;  /**< Judge the two surface is same or not */
    virtual void CleanObj() const = 0;                              /**< Clean the object */
    virtual Standard_Boolean Compare(const IGeomFace *& theGeoFace) = 0;  /**< Clean the object */    
    virtual TCollection_AsciiString GetTransfNum() const = 0;   

public:

    SurfType GetFaceType() const{ return m_SurfType; }              /**< Get the surface type */
    void SetSurfNum(int iNum){ m_iNum = iNum; }                     /**< Set the number of surface */
    Standard_Integer GetSurfNum(){ return m_iNum; }                 /**< Get the number of surface */
    Standard_Real GetUnit(){return 0.1;}                            /**< Get the unit used */
    void SetUnit(Standard_Real theRatio){m_ScaleRatio = theRatio;}  /**< Set the unit */

    TCollection_AsciiString GetSurfSymb() const{ return m_SurfSymb; }       /**< Get the surface type */
    vector<Standard_Real> GetPrmtList(){return m_PrmtList;}                 /**< Get the coefficients list */
    Standard_Boolean IsReversed(){return m_bReverse;}                       /**< The surface is reversed or not*/

protected:    

    Standard_Integer m_iNum;             /**< The surface number in list */
    SurfType m_SurfType;                 /**< Face Type */
    gp_Pnt m_PntOnFace;                  /**< A point on the suface */
    TCollection_AsciiString m_SurfExpn;  /**< Face Expression of Solid */
    Standard_Integer m_iCodeNum;         /**< MaCadExtFace can find the geomtry surface using the number */
    Standard_Real m_ScaleRatio;          /**< The scale ratio according to the unit CAD model used */

    TCollection_AsciiString m_SurfSymb;  /**< The surface symbol */
    vector<Standard_Real> m_PrmtList;    /**< The surface coefficients list */

    Standard_Boolean m_bReverse;         /**< The surface has been reversed or not */
};

#endif // IGEOMFACE_HXX
