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

Standard_EXPORT    McCadTripoliWriter();
Standard_EXPORT    McCadTripoliWriter(Standard_Boolean bMat, Standard_Boolean bVoid);
Standard_EXPORT    ~McCadTripoliWriter();

//private: //qiu
protected:

    Standard_Integer m_virtualCellNum;  /**< The initial number of virtual cell */
    map<TCollection_AsciiString,TCollection_AsciiString> m_mapSymb;

public:
Standard_EXPORT    void SetVirtCellNum(Standard_Integer theNum);       /**< Set the initial virtual cell number */
Standard_EXPORT    void PrintFile();                                   /**< Print the file */
Standard_EXPORT    void PrintHeadDesc(Standard_OStream& theStream);    /**< Print the head */
Standard_EXPORT    void PrintCellDesc(Standard_OStream& theStream);    /**< Print the cell card */
Standard_EXPORT    void PrintVoidDesc(Standard_OStream& theStream);    /**< Print the void card */
Standard_EXPORT    void PrintSurfDesc(Standard_OStream& theStream);    /**< Print the surfaces list */
Standard_EXPORT    void PrintTrsfDesc(Standard_OStream& theStream);    /**< Print the transform card */
Standard_EXPORT    void PrintMatCard(Standard_OStream& theStream);     /**< Print the material card */

//private: //qiu
protected:

    /**< Print the outer spaces beside void cells and material cells */
Standard_EXPORT    void PrintOutSpace(Standard_OStream& theStream);
    /** Print the group information, including material name, group name, material id and density */
Standard_EXPORT    void PrintGroupInfo(const int iSolid, Standard_OStream& theStream);
    /** Generate the TRIPOLI cell object from a convex solid */
Standard_EXPORT    vector<McCadTripoliCell *> GenTripoliCellList(McCadSolid *& pSolid);
    /** Generate the TRIPOLI cell object from a filling void solid */
Standard_EXPORT    vector<McCadTripoliCell *> GenTripoliVoidCellList(McCadVoidCell *& pVoidCell);
    /** Generate the outer space tripoli cell list */
Standard_EXPORT    vector<McCadTripoliCell *> GenOuterSpaceList(McCadVoidCell *& pVoidCell);

Standard_EXPORT    vector<Standard_Real> GetSurfPrmt(IGeomFace *& pFace);  /**< Get the parameter list of a surface */
Standard_EXPORT    TCollection_AsciiString GetSurfSymb(IGeomFace *& pFace);/**< Get the type of surface */

};

#endif // MCCADTRIPOLIWRITER_HXX
