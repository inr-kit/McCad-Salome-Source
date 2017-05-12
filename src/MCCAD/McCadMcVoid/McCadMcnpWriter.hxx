#ifndef MCCADMCNPWRITER_HXX
#define MCCADMCNPWRITER_HXX

#include "IMcCadWriter.hxx"

class McCadMcnpWriter : public IMcCadWriter
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


Standard_EXPORT    McCadMcnpWriter();
Standard_EXPORT    McCadMcnpWriter(Standard_Boolean bMat, Standard_Boolean bVoid);
Standard_EXPORT    ~McCadMcnpWriter();

public:
    /** Print the output file, include head information, cell, void, surface, transform card */
//qiu comment "const" because PrintCellDesc is not a const method
Standard_EXPORT    void PrintFile()/*const*/;                 /**< Print the file */
Standard_EXPORT    void PrintCellDesc(Standard_OStream& theStream) ;        /**< Print the cell card */
Standard_EXPORT    void PrintVoidDesc(Standard_OStream& theStream) ;        /**< Print the void card */

Standard_EXPORT    void PrintHeadDesc(Standard_OStream& theStream)const;   /**< Print the head */
Standard_EXPORT    void PrintSurfDesc(Standard_OStream& theStream)const;   /**< Print the surfaces list */
Standard_EXPORT    void PrintTrsfDesc(Standard_OStream& theStream)const;   /**< Print the transform card */
Standard_EXPORT    void PrintMatCard(Standard_OStream& theStream)const;    /**< Print the material card */
Standard_EXPORT    void PrintVolumeCard(Standard_OStream& theStream)const; /**< Print the volume card */

    /** Print the group information, including material name, group name, material id and density */
Standard_EXPORT    virtual void PrintGroupInfo(const int iSolid, Standard_OStream& theStream);

//qiu private:
protected:



    /**< A new cell expression writor, which generate a cell number list first, then
         remove the repeated surfaces */
Standard_EXPORT    TCollection_AsciiString GetCellExpn(McCadConvexSolid *& pSolid);
    /**< Find in the cell number list, are there any repeated cells */
    Standard_Boolean FindRepeatCell(int iFaceNum, vector<Standard_Integer> &list);
    /**< Get the void expression */
Standard_EXPORT    TCollection_AsciiString GetVoidExpn(McCadVoidCell *& pVoidCell);

//Standard_EXPORT    TCollection_AsciiString GetCellExpnOld(McCadConvexSolid *& pSolid); /**< Get the cell expression */
};

#endif // MCCADMCNPWRITER_HXX
