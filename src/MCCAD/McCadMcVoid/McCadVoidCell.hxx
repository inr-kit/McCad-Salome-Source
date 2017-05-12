#ifndef MCCADVOIDCELL_HXX
#define MCCADVOIDCELL_HXX

#include <TopTools_HSequenceOfShape.hxx>

#include <map>
#include <vector>

#include <gp_Pnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Solid.hxx>
#include <Bnd_Box.hxx>
#include "McCadConvexSolid.hxx"
#include "McCadVoidCollision.hxx"
#include <memory>

using namespace std;
enum SplitAxis{ XAxis = 1,
                YAxis = 2,
                ZAxis = 3};

class McCadVoidCellManager;
class McCadGeomData;

class McCadVoidCell : public TopoDS_Solid
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

Standard_EXPORT    McCadVoidCell();
Standard_EXPORT    McCadVoidCell(const TopoDS_Solid & theSolid);
Standard_EXPORT    ~McCadVoidCell();

private:

    vector<int> m_CollidedSolidNumList;   /**< Record the index of convex solid list who is collided with void cell*/

    gp_Pnt m_MaxPnt;                      /**< The max point of the boundary box */
    gp_Pnt m_MinPnt;                      /**< The min point of the boundary box */

    TCollection_AsciiString m_szExpression;         /**< MCNP expression of solid */

    Standard_Boolean m_bHaveBndBox;                 /**< If Boundary box has been generated */
    Bnd_Box m_bBox;                                 /**< The boundary box */

    vector<McCadVoidCollision *> m_CollisionList;   /**< Record the collied convex solids */
    vector<McCadExtBndFace*> m_BndFaceList;         /**< Boundary faces list of void solid */

    SplitAxis m_SplitAxis;          /**< if the box contains too many collied surfaces, split the box using X,Y or Z*/
    Standard_Integer m_iSplitDepth; /**< How many levels has been splitted */

    Handle_TColgp_HSequenceOfPnt m_VertexList;      /**< The vertex of void box */

public:  

Standard_EXPORT    Bnd_Box GetBntBox();            /**< Get the boundary box */
Standard_EXPORT    void AddColliedSolidNum(Standard_Integer index); /**< Add the index of collied convex solid in list */

    /**< Set the boundary box */
Standard_EXPORT    void SetBntBox(Standard_Real fXmin, Standard_Real fYmin, Standard_Real fZmin,
                   Standard_Real fXmax, Standard_Real fYmax, Standard_Real fZmax);

Standard_EXPORT    TCollection_AsciiString GetOutVoidExpression(); /**< Get the outside void space beside the material solid and filled void space */
Standard_EXPORT    vector<McCadExtBndFace*> GetGeomFaceList();     /**< Get the boundary faces of void box */

    /**< if the void expression is too long, split it into two parts */
Standard_EXPORT    Standard_Boolean SplitVoidCell(vector<McCadVoidCell*> & void_list,McCadGeomData * pData);

Standard_EXPORT    void SetSplitDepth(Standard_Integer iSplitDepth);   /**< Set the split depth */
Standard_EXPORT    void ChangeFaceNum(McCadGeomData * pData);          /**< Change the face number after surface sorting */
Standard_EXPORT    void CalColliedFaces(McCadGeomData *pData);         /**< Calculate which surfaces of material solid collied with box */

Standard_EXPORT    vector<McCadExtBndFace *> GetBndFaces();            /**< Get the boundary faces of void box */
Standard_EXPORT    vector<McCadVoidCollision*> GetCollisions();        /**< Get the collisions with material solids*/

private:

    void SetVertexList();                               /**< Get the vertexes and save into list */
    TCollection_AsciiString GetExpression();            /**< Get the expression of void cell */
    Standard_Boolean IsPointInBBox(gp_Pnt pnt);         /**< If the point is in the box */

    /**< Set the collied solid index list */
    void SetCollidedSolidNumList(vector<int> theCollidedSolidNumList );

    Standard_Boolean IsVertexInSolid(McCadConvexSolid *& pSolid);   /**< If the vertex of box is in the solid */
    Standard_Boolean CalColliedFace(McCadExtBndFace *&theFace);     /**< The box is collied with face */
    Standard_Boolean CalColliedBox(Bnd_Box & box);                  /**< If the box is collied with another box*/

    /**< Calculate the collision with sample points */
    Standard_Boolean CalColliedPoints(Handle_TColgp_HSequenceOfPnt point_list);
};

#endif // MCCADVOIDCELL_HXX
