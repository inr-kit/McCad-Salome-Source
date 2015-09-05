#ifndef MCCADEXTFACE_HXX
#define MCCADEXTFACE_HXX

#include <TopoDS_Face.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <Handle_TColgp_HSequenceOfPnt.hxx>
#include <Bnd_Box.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <vector>

using namespace std;

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
    ~McCadExtFace();

private:
    Standard_Integer m_iFaceNo;                     /**< The number of face in the facelist */
    Standard_Integer m_iNumPnt;                     /**< The number of discrete points */

    Standard_Boolean m_bHaveAuxFace;                /**< Whether have auxiliary face */

    Handle_TColgp_HSequenceOfPnt m_DiscPntList;     /**< Discrete points of faces */
    Handle_TColgp_HSequenceOfPnt m_EdgePntList;     /**< Discrete points of edges of this face */
    vector< McCadExtFace* > m_AuxFaceList;          /**< Auxiliary face list */

    Standard_Integer m_iFaceNum;                    /**< Face number of the geomtetry face list */
    ORIENTATION m_symbol;                           /**< The direction of plane is positive or negative */

    //TopoDS_Face m_Face;
    Bnd_Box m_bBox;
    GeomAbs_SurfaceType m_FaceType;

    vector< McCadExtFace* > m_SameFaceList;         /**< Merge the same surface of one solid */
    Standard_Integer m_iAttri;

    Standard_Boolean m_bFusedFace;                  /**< The fused face is no need to do the collision detection with void box*/

  /*  Standard_Integer myMaxNbPnt;
    Standard_Integer myMinNbPnt;
    Standard_Real myXlen;
    Standard_Real myYlen;
    Standard_Real myRlen;
    Standard_Real myTolerance;*/


public:


    Standard_Boolean IsConcaveCurvedFace();         /**< Concave curved face need to add auxiliary face */
    void RemoveAuxFace(Standard_Integer index);     /**< Remove the face with the face number*/

    vector< McCadExtFace* > GetAuxFaces();
    void AddAuxFaces(vector<McCadExtFace *> faces); /**< Add a list of faces for auxiliary faces*/

    Handle_TColgp_HSequenceOfPnt GetDiscPntList();  /**< Get discrete point list of face */
    void AddPntList(Handle_TColgp_HSequenceOfPnt pnt_list); /** Add point list*/
    Handle_TColgp_HSequenceOfPnt GetEdgePntList();          /**< Get the discrete point list of edge frame*/

    void SetFaceNum(int iCodeNum);                  /**< Set face number */
    Standard_Integer GetFaceNum();                  /**< Get face number */

    void SetSurfSymbol();                           /**< Set the symbol based on direction of surface */
    Standard_Boolean GetSurfSymbol();               /**< Get the symbol */
    void ChangeSymbol(Standard_Boolean bSymbol);    /**< Change the symbol */
    ORIENTATION GetFaceOrientation();               /**< Get the orientation of face, is plus or minus */

    Bnd_Box GetBndBox();
    void UpdateBndBox(Bnd_Box theBBox);
    GeomAbs_SurfaceType GetFaceType();
    void SetFaceType(GeomAbs_SurfaceType theFaceType);
    Standard_Boolean HaveAuxSurf();

    void AddSameFaces( McCadExtFace* faces);
    vector< McCadExtFace* > GetSameFaces();  

    /**< Merge discrete points of the two connect surfaces with same geometries */
    void MergeDscPnt(TopoDS_Face &theFace);

    void AddAttri(Standard_Integer iAttri);
//qiu    Standard_Integer GetAttri(){return m_iAttri;};
    Standard_Integer GetAttri() const {return m_iAttri;};   
	Standard_Integer IsFusedFace(){return m_bFusedFace;};
    //McCadExtFace *m_mergedFace;


    //Standard_Boolean IsSame(McCadExtFace *& theFace);
    //Handle_TColgp_HSequenceOfPnt Init();test

};

#endif // MCCADEXTFACE_HXX
