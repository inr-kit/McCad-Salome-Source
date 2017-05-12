#ifndef MCCADEXTBNDFACE_HXX
#define MCCADEXTBNDFACE_HXX

#include <vector>
#include "McCadExtFace.hxx"
#include "McCadExtAstFace.hxx"

#include <TopTools_HSequenceOfShape.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <Handle_TColgp_HSequenceOfPnt.hxx>
#include <Bnd_Box.hxx>

using namespace std;

class McCadExtBndFace : public McCadExtFace
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

    McCadExtBndFace();
    McCadExtBndFace(const TopoDS_Face &theFace);
    McCadExtBndFace(const McCadExtBndFace &theExtFace);
    ~McCadExtBndFace();

private:

    Handle_TColgp_HSequenceOfPnt m_DiscPntList;     /**< Discrete points of faces */
    Handle_TColgp_HSequenceOfPnt m_EdgePntList;     /**< Discrete points of edges of this face */

    Standard_Boolean m_bHaveAstFace;                /**< Whether have assisted face */
    vector<McCadExtAstFace*> m_AstFaceList;         /**< Assisted face list */

    Bnd_Box m_bBox;                                 /**< The boundary box of given surface */

    vector< McCadExtBndFace* > m_SameFaceList;      /**< Merge the same surface of one solid */
    Standard_Boolean m_bFusedFace;                  /**< The fused face is no need to do the collision detection with void box*/

public:

    void RemoveAstFace(Standard_Integer index);             /**< Remove the face with the face number*/
    Standard_Boolean IsConcaveCurvedFace();                 /**< Concave curved face need to add assisted face */
    void AddSameFaces(McCadExtBndFace *& pFace);            /**< Add the face with same geometry into same face list*/
    vector<McCadExtAstFace *> GetAstFaces();                /**< Get the assisted faces list */
    void AddAstFaces(vector<McCadExtAstFace *> faces);      /**< Add a list of faces as assisted faces*/

    Standard_Boolean IsFusedFace();                         /**< The surface is fused surface or not*/
    Handle_TColgp_HSequenceOfPnt GetEdgePntList();          /**< Get the discrete point list of edges*/
    Standard_Boolean HaveAstSurf();                         /**< If this face has auxiliary face*/
    vector< McCadExtBndFace* > GetSameFaces();              /**< Get the face in same face list*/
    Handle_TColgp_HSequenceOfPnt GetDiscPntList();          /**< Get discrete point list of face */
    Bnd_Box GetBndBox();                                    /**< Get the boundary box of face*/

    /**< Merge discrete points of the two connect surfaces with same geometries */
    void Merge(McCadExtBndFace *& pExtFace);

private:

    void AddPntList(Handle_TColgp_HSequenceOfPnt pnt_list); /**< Add point list*/
    void UpdateBndBox(Bnd_Box theBBox);                     /**< Update the boundary box of face*/
};

#endif // MCCADEXTBNDFACE_HXX
