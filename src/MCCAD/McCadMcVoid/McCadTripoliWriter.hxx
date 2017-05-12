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

//qiu private:
protected:

    Standard_Integer m_virtualCellNum;  /**< The initial number of virtual cell */
    map<TCollection_AsciiString,TCollection_AsciiString> m_mapSymb;

public:
Standard_EXPORT    virtual void SetVirtCellNum(Standard_Integer theNum);       /**< Set the initial virtual cell number */
//qiu add "const" because they are used in the printFile() const
Standard_EXPORT    virtual void PrintCellDesc(Standard_OStream& theStream)  ;         /**< Print the cell card */
Standard_EXPORT    virtual void PrintVoidDesc(Standard_OStream& theStream) ;         /**< Print the void card */
//qiu comment "const" because PrintCellDesc is not a const method
Standard_EXPORT    virtual void PrintFile()/*const*/;                                   /**< Print the file */
Standard_EXPORT    virtual void PrintHeadDesc(Standard_OStream& theStream)const;    /**< Print the head */
Standard_EXPORT    virtual void PrintSurfDesc(Standard_OStream& theStream)const;    /**< Print the surfaces list */
Standard_EXPORT    virtual void PrintTrsfDesc(Standard_OStream& theStream)const;    /**< Print the transform card */
Standard_EXPORT    virtual void PrintMatCard(Standard_OStream& theStream)const;     /**< Print the material card */
Standard_EXPORT    virtual void PrintVolumeCard(Standard_OStream& theStream)const;  /**< Print the volume card */

//qiu private:
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
    /**< Find in the cell number list, are there any repeated cells */
Standard_EXPORT    Standard_Boolean FindRepeatCell(int iFaceNum, vector<Standard_Integer> & list);
//qiu add const because they are used in const PrintSurfDesc()
Standard_EXPORT    vector<Standard_Real> GetSurfPrmt(IGeomFace *& pFace)const;  /**< Get the parameter list of a surface */
Standard_EXPORT    TCollection_AsciiString GetSurfSymb(IGeomFace *& pFace) const;/**< Get the type of surface */

};

#endif // MCCADTRIPOLIWRITER_HXX
