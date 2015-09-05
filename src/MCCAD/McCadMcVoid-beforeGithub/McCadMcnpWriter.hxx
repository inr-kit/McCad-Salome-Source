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


    McCadMcnpWriter();
    McCadMcnpWriter(Standard_Boolean bMat, Standard_Boolean bVoid);
    ~McCadMcnpWriter();

public:
    /** Print the output file, include head information, cell, void, surface, transform card */
    void PrintFile();                 /**< Print the file */

    void PrintHeadDesc(Standard_OStream& theStream);  /**< Print the head */
    void PrintCellDesc(Standard_OStream& theStream);  /**< Print the cell card */
    void PrintVoidDesc(Standard_OStream& theStream);  /**< Print the void card */
    void PrintSurfDesc(Standard_OStream& theStream);  /**< Print the surfaces list */
    void PrintTrsfDesc(Standard_OStream& theStream);  /**< Print the transform card */
    void PrintMatCard(Standard_OStream& theStream);   /**< Print the material card */

    /** Print the group information, including material name, group name, material id and density */
    virtual void PrintGroupInfo(const int iSolid, Standard_OStream& theStream);

    TCollection_AsciiString GetCellExpn(McCadConvexSolid *& pSolid);
    TCollection_AsciiString GetVoidExpn(McCadVoidCell *& pVoidCell);
};

#endif // MCCADMCNPWRITER_HXX
