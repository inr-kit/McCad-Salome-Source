#ifndef MCCADTRIPOLIWRITER_HXX
#define MCCADTRIPOLIWRITER_HXX

#include "IMcCadWriter.hxx"

class McCadTripoliWriter : public IMcCadWriter
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

    McCadTripoliWriter();
    McCadTripoliWriter(Standard_Boolean bMat, Standard_Boolean bVoid);
    ~McCadTripoliWriter();

//private: //qiu
protected:

    Standard_Integer m_virtualCellNum;  /**< The initial number of virtual cell */
    map<TCollection_AsciiString,TCollection_AsciiString> m_mapSymb;

public:
    void SetVirtCellNum(Standard_Integer theNum);       /**< Set the initial virtual cell number */
    void PrintFile();                                   /**< Print the file */
    void PrintHeadDesc(Standard_OStream& theStream);    /**< Print the head */
    void PrintCellDesc(Standard_OStream& theStream);    /**< Print the cell card */
    void PrintVoidDesc(Standard_OStream& theStream);    /**< Print the void card */
    void PrintSurfDesc(Standard_OStream& theStream);    /**< Print the surfaces list */
    void PrintTrsfDesc(Standard_OStream& theStream);    /**< Print the transform card */
    void PrintMatCard(Standard_OStream& theStream);     /**< Print the material card */

//private: //qiu
protected:

    /**< Print the outer spaces beside void cells and material cells */
    void PrintOutSpace(Standard_OStream& theStream);
    /** Print the group information, including material name, group name, material id and density */
    void PrintGroupInfo(const int iSolid, Standard_OStream& theStream);
    /** Generate the TRIPOLI cell object from a convex solid */
    vector<McCadTripoliCell *> GenTripoliCellList(McCadSolid *& pSolid);
    /** Generate the TRIPOLI cell object from a filling void solid */
    vector<McCadTripoliCell *> GenTripoliVoidCellList(McCadVoidCell *& pVoidCell);
    /** Generate the outer space tripoli cell list */
    vector<McCadTripoliCell *> GenOuterSpaceList(McCadVoidCell *& pVoidCell);

    vector<Standard_Real> GetSurfPrmt(IGeomFace *& pFace);  /**< Get the parameter list of a surface */
    TCollection_AsciiString GetSurfSymb(IGeomFace *& pFace);/**< Get the type of surface */

};

#endif // MCCADTRIPOLIWRITER_HXX
