#ifndef MCCADGEOMDATA_HXX
#define MCCADGEOMDATA_HXX

#include "McCadConvexSolid.hxx"
#include "McCadVoidCell.hxx"
#include "McCadTransfCard.hxx"
#include "IGeomFace.hxx"
#include "McCadGeomCone.hxx"
#include "McCadSolid.hxx"

#include <OSD_File.hxx>
#include <vector>

class McCadGeomData
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

Standard_EXPORT    McCadGeomData();
Standard_EXPORT    McCadGeomData(Handle_TopTools_HSequenceOfShape & solid_list, Standard_Boolean bGenVoid);
Standard_EXPORT    ~McCadGeomData();

//qiu private:
protected:

    McCadVoidCellManager * m_pManager;              /**< The manager object which data belong to */

//qiu private:
protected:

    Standard_Integer m_iNumSolid;                           /**< The number of convex solids */

    Handle(TopTools_HSequenceOfShape) m_listConvexSolid;    /**< The input convex solid after decomposition */
    Handle(TopTools_HSequenceOfShape) m_listVoidCell;       /**< The input convex solid after decomposition */

    vector <McCadSolid *> m_SolidList;                      /**< Solid list */
    vector <McCadConvexSolid *> m_ConvexSolidList;          /**< Convex solid list */
    vector <McCadVoidCell *> m_VoidCellList;                /**< void cell list */
    McCadVoidCell * m_pOutVoid;                             /**< The out box of the whole model */

    vector <IGeomFace *> m_GeomSurfList;                    /**< Geometry surface list */    
    vector <McCadTransfCard *> m_TransfCardList;            /**< Surface transf card List */

    map<Standard_Integer,Standard_Integer> m_mapSurfNum;    /**< When sort the surface list, reset the surface number */

public:

    /**< Input the geometry solid and assign the parameters. */
Standard_EXPORT    void InputData(Handle_TopTools_HSequenceOfShape & solid_list, Standard_Boolean bGenVoid);
    /**< Sort the surface list based on surface type and parameters */
Standard_EXPORT    void SortSurface();
    /**< Update the face number of cell expression after sorting the surface */
Standard_EXPORT    void UpdateFaceNum();
    /**< Connect the manager to data */
Standard_EXPORT    void SetManager(const McCadVoidCellManager * pManager);
    /**< Add Transform card */
Standard_EXPORT    Standard_Integer AddTransfCard(gp_Ax3 theAxis, gp_Pnt theApex);
    /**< Get the convex solid list */
Standard_EXPORT    vector <McCadConvexSolid *> GetConvexSolid();
    /**< Get the transformation cards */
Standard_EXPORT    vector <McCadTransfCard *> GetTransfCard();
    /**< Get the new surface number according to the old surface number */
Standard_EXPORT    Standard_Integer GetNewFaceNum(Standard_Integer iOldNum);

//qiu private:
protected:

    /**< Traverse the face list and construct geometry surface list.*/
Standard_EXPORT    void AddGeomSurfList(const vector<McCadExtBndFace *> &theExtFaceList);
    /**< Traverse the face list, if the face need to add auxiliary surface,
         generate the auxiliary faces and add them into geometry face list. */
Standard_EXPORT    void AddGeomAstSurfList(const vector<McCadExtBndFace *> &theExtFaceList);

    /**< Generate a map to record the old surface number of new one */
Standard_EXPORT    Standard_Boolean GenMapSurfNum(Standard_Integer,Standard_Integer);
    /**< Generate a geometry face */
    IGeomFace *GenGeomSurface(McCadExtFace *& pExtFace);
    /**< Add the generated geometry face to geometry face list */
Standard_EXPORT    void AddInGeomFaceList(IGeomFace*& pGeomFace, McCadExtFace *& pExtFace);

    /** Friend function which can be used to compare the weight of each surface and sort them */
Standard_EXPORT    friend Standard_Boolean compare(const IGeomFace * surfA, const IGeomFace * surfB);

public:

    friend class McCadVoidGenerator;
    friend class McCadVoidCellManager;
    friend class McCadMcnpWriter;
    friend class McCadTripoliWriter;
    //qiu add new friend class
    friend class S_McCadMcnpWriter;
    friend class S_McCadTripoliWriter;
    friend class McCadGDMLWriter;
    friend class S_McCadGDMLWriter;

};

#endif // MCCADGEOMDATA_HXX
