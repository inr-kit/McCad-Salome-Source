#ifndef MCCADEXTBNDFACE_HXX
#define MCCADEXTBNDFACE_HXX

#include "McCadExtFace.hxx"
#include "McCadExtAuxFace.hxx"

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

    Standard_Boolean m_bHaveAuxFace;                /**< Whether have auxiliary face */
    vector< McCadExtAuxFace* > m_AuxFaceList;       /**< Auxiliary face list */

    ORIENTATION m_symbol;                           /**< The direction of plane is positive or negative */

    Bnd_Box m_bBox;
    GeomAbs_SurfaceType m_FaceType;

    vector< McCadExtBndFace* > m_SameFaceList;      /**< Merge the same surface of one solid */

public:

    Standard_Boolean IsConcaveCurvedFace();         /**< Concave curved face need to add auxiliary face */

    void RemoveAuxFace(Standard_Integer index);             /**< Remove the face with the face number*/
    vector<McCadExtAuxFace *> GetAuxFaces();                /**< Get the auxiliary faces list */
    void AddAuxFaces(vector<McCadExtAuxFace *> faces);      /**< Add a list of faces as auxiliary faces*/
    Standard_Boolean HaveAuxSurf();                         /**< If this face has auxiliary face*/

    Handle_TColgp_HSequenceOfPnt GetDiscPntList();          /**< Get discrete point list of face */
    void AddPntList(Handle_TColgp_HSequenceOfPnt pnt_list); /** Add point list*/
    Handle_TColgp_HSequenceOfPnt GetEdgePntList();          /**< Get the discrete point list of edges*/

    //Bnd_Box GetBndBox();                                    /**< Get the boundary box of face*/
    void UpdateBndBox(Bnd_Box theBBox);                     /**< Update the boundary box of face*/

    void AddSameFaces(McCadExtBndFace *faces);              /**< Add the face with same geometry into same face list*/
    vector< McCadExtBndFace* > GetSameFaces();              /**< Get the face in same face list*/

};

#endif // MCCADEXTBNDFACE_HXX
