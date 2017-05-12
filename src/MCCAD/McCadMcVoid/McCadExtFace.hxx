#ifndef MCCADEXTFACE_HXX
#define MCCADEXTFACE_HXX

#include <Standard.hxx>
#include <TopoDS_Face.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <TopAbs_Orientation.hxx>

enum ORIENTATION{PLUS = 0, MINUS = 1};

class McCadExtFace : public TopoDS_Face
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

    McCadExtFace();
    McCadExtFace(const TopoDS_Face &theFace);
    McCadExtFace(const McCadExtFace &theExtFace);
    virtual ~McCadExtFace();

private:
    Standard_Integer m_iFaceNo;                         /**< The number of face in the facelist */
    Standard_Integer m_iFaceNum;                        /**< Face number of the geomtetry face list */
    ORIENTATION m_symbol;                               /**< The direction of plane is positive or negative */

    GeomAbs_SurfaceType m_FaceType;                     /**< The surface type */
    GeomAdaptor_Surface m_AdpFace;                      /**< The adaptor of surface */

public:

    void SetFaceNum(int iCodeNum);                      /**< Set face number */
    Standard_Integer GetFaceNum();                      /**< Get face number */
    GeomAbs_SurfaceType GetFaceType();                  /**< Get the surface type */
    GeomAdaptor_Surface GetAdpFace();                   /**< Get the adpator of surface */
    TopAbs_Orientation GetOrientation() const;          /**< Get the orientation of surface */

private:
    ORIENTATION GetFaceOrientation();                   /**< Get the orientation of face, is plus or minus */

};

#endif // MCCADEXTFACE_HXX
