/** @file
  * @brief
  *
  * @author
  * @date
  */

#ifndef MCCADCONVEXSOLID_HXX
#define MCCADCONVEXSOLID_HXX

#include <vector>
#include <string>

#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <Bnd_Box.hxx>

#include "McCadExtBndFace.hxx"
#include "McCadTripoliCell.hxx"
#include "McCadExtAstFace.hxx"

using namespace std;

class McCadGeomData;

class McCadConvexSolid : public TopoDS_Solid{
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

Standard_EXPORT    McCadConvexSolid();
Standard_EXPORT    McCadConvexSolid(const TopoDS_Solid & theSolid);
Standard_EXPORT    ~McCadConvexSolid();

private:

    int m_iNum;                       /**< Number of Cell */
    int m_iMatNo;                     /**< Number of Material */
    double m_dDensity;                /**< Material Density */    
    int m_iNeutronIMP;                /**< Importance of Neutron */
    int m_iPhotonIMP;                 /**< Importance of Photon */

    Standard_Real m_fVolume;          /**< Volume of the solid*/

    TCollection_AsciiString m_szExpression;     /**< MCNP Expression of Solid */
    int m_iNumOfFaces;                          /**< Number of Boundary Surface */

    TopoDS_Shape m_bBoxShape;                   /**< The shape of boundary box */
    Standard_Boolean m_bHaveBndBox;             /**< If Boundary box has been already generated */
    Bnd_Box m_bBox;                             /**< The boundary box */


    vector<McCadExtBndFace*> m_STLFaceList;                 /**< Face list of convex solid */
    Handle_TColgp_HSequenceOfPnt m_DiscPntList;             /**< Discrete points list */
    Handle_TColgp_HSequenceOfPnt m_EdgePntList;             /**< Discrete points of edges */

private:

    void GenFacesList(const TopoDS_Solid & theSolid);       /**< Get the data of faces of solid */
    void AddAstSurfaces();                                  /**< Add Auxiliary Surface to each concave curved surface*/

    /** Judge the assisted surface is splitting surface or not, add it into auxiliary face list when it is not */
    void JudgeAssistFaces(vector<McCadExtAstFace*> & theAstFaceList);

    void MergeAstFaces(McCadExtBndFace *& pLeftFace, McCadExtBndFace *& pRightFace); /**< Merge assisted surfaces*/
    void MergeBndAstFaces();                                /**< Merge boundary surfaces and auxiliary surfaces */
    void MergeBndFaces(Standard_Boolean bMergeDiscPnt);     /**< Merge boundary surfaces */

public:

    /** Set the number material number and some information of cell */
Standard_EXPORT    void SetCellInfo(int iNum, int iMat, double fDensity, int iNeutronIMP, int iPhotonIMP);

Standard_EXPORT    Bnd_Box GetBntBox();                                    /**< Get boundary box */
Standard_EXPORT    Handle_TColgp_HSequenceOfPnt GetDiscPntList();          /**< Get the sample points of face */

Standard_EXPORT    vector<McCadExtBndFace *> GetSTLFaceList();             /**< Get the face list */
Standard_EXPORT    TCollection_AsciiString GetExpression();                /**< Generate solid expression */
Standard_EXPORT    void GenDescPoints(Standard_Boolean bGenVoid);          /**< Discrete the faces and store the points into list */
Standard_EXPORT    void GenEdgePoints();                                   /**< Discrete the edges and get the sample points on edge */

Standard_EXPORT    void ChangeFaceNum(McCadGeomData * pData);              /**< Change the surface number after sorting the surfaces */
Standard_EXPORT    void DeleteRepeatFaces(Standard_Boolean bMergeDiscPnt); /**< Delete the repeat faces */

Standard_EXPORT    void SetVolume(Standard_Real fVolume);                  /**< Set the volume */
Standard_EXPORT    Standard_Real GetVolume();                              /**< Get the volume */

Standard_EXPORT    vector<McCadExtBndFace*> GetFaces();                    /**< Get the face list */
};

#endif // MCCADCONVEXSOLID_HXX
