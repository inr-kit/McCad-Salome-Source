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
#include <memory>

using namespace std;

typedef struct{
    int SolidNum;
    vector<int> FaceList;
    vector< vector<int> > AuxFaceList;
}COLLISION;

enum SplitAxis{ XAxis = 1,
                YAxis = 2,
                ZAxis = 3};

class McCadVoidCellManager;
class McCadGeomData;

class McCadVoidCell : public TopoDS_Solid
{
public:
    McCadVoidCell();
    McCadVoidCell(const TopoDS_Solid & theSolid);

private:
    enum cutplane{x=0,y=1,z=2};         // Cut the void cell when it is collied with too many solids


    vector<int> m_CollidedSolidNumList;     // Record the solid no who is collided with void cell
    // Which boundary faces of the solid is collied with the void cell.
    // the first parameter(int) is the solid no, the second is the list
    // of the collided boundary faces.
    map<int,TopTools_HSequenceOfShape> mapCollidedFaceNo;

    gp_Pnt m_MaxPnt;                      // The max point of the boundary box
    gp_Pnt m_MinPnt;                      // The min point of the boundary box

    TCollection_AsciiString m_szExpression;         // MCNP expression of solid
    int iLengthOfExpression;                        // Length of MCNP expression

    Standard_Boolean m_bHaveBndBox;                         /**< If Boundary box has been generated */
    Bnd_Box m_bBox;                                         /**< The boundary box */

    vector<COLLISION> m_Collision;
    vector<McCadExtFace*> m_FaceList;                       /**< Face list of void solid */

    SplitAxis m_SplitAxis;
    Standard_Integer m_iSplitDepth;

    Handle_TColgp_HSequenceOfPnt m_VertexList;
    //Handle_TopTools_HSequenceOfShape m_NFaceList;

public:

    Standard_Boolean IsCollided(const TopoDS_Shape & Solid);                      // Whether is collided with solid
    void DetectCollision(const Handle(TopTools_HSequenceOfShape) & SolidList);    // Detect the collision with solids list

    void GenerateMCNPExpression();                                                // Generate the MCNP expression
    Bnd_Box GetBntBox();
    void SetVertexList();
    void AddColliedSolidNum(Standard_Integer iNum);

    vector<int> GetColliedSolidList();    
    //void CalColliedFaces(vector <McCadConvexSolid *> & solid_list);
    Standard_Boolean CalColliedPoints(Handle_TColgp_HSequenceOfPnt point_list);

    Bnd_Box SetBntBox(Standard_Real fXmin, Standard_Real fYmin, Standard_Real fZmin,
                      Standard_Real fXmax, Standard_Real fYmax, Standard_Real fZmax);
    TCollection_AsciiString GetExpression();
    TCollection_AsciiString GetOutVoidExpression();
    vector<McCadExtFace*> GetGeomFaceList();

    Standard_Boolean SplitVoidCell(vector<McCadVoidCell*> & void_list,
                                   McCadGeomData * pData);

    void SetSplitAxis(SplitAxis split_axis);
    void SetCollidedSolidNumList(vector<int> theCollidedSolidNumList );
    void SetSplitDepth(Standard_Integer iSplitDepth);

    void ChangeFaceNum(McCadGeomData * pData);
    void CalColliedFaces(McCadGeomData *pData);

    Standard_Boolean IsPointInBBox(gp_Pnt pnt);

    Standard_Integer SideofFace(gp_Pnt pnt, McCadExtFace *& theFace);
    Standard_Boolean CalColliedVertex(McCadExtFace *& theFace);
    Standard_Boolean IsVertexInSolid(McCadConvexSolid *& pSolid);
    Standard_Boolean CalColliedFace(McCadExtFace *& theFace);
    Standard_Boolean CalColliedBox(Bnd_Box & box);
    Standard_Boolean PntInBox(gp_Pnt &pnt, Bnd_Box & box);

    vector<McCadExtFace*> GetFaces();
    vector<COLLISION> GetCollision();


};

#endif // MCCADVOIDCELL_HXX
