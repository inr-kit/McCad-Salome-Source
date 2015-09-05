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
Standard_EXPORT    void PrintFile();                 /**< Print the file */

Standard_EXPORT    void PrintHeadDesc(Standard_OStream& theStream);    /**< Print the head */
Standard_EXPORT    void PrintCellDesc(Standard_OStream& theStream);    /**< Print the cell card */
Standard_EXPORT    void PrintVoidDesc(Standard_OStream& theStream);    /**< Print the void card */
Standard_EXPORT    void PrintSurfDesc(Standard_OStream& theStream);    /**< Print the surfaces list */
Standard_EXPORT    void PrintTrsfDesc(Standard_OStream& theStream);    /**< Print the transform card */
Standard_EXPORT    void PrintMatCard(Standard_OStream& theStream);     /**< Print the material card */
//qiu not implement    void PrintVolumeCard(Standard_OStream& theStream);  /**< Print the volume card */

    /** Print the group information, including material name, group name, material id and density */
Standard_EXPORT    virtual void PrintGroupInfo(const int iSolid, Standard_OStream& theStream);

Standard_EXPORT    TCollection_AsciiString GetCellExpn(McCadConvexSolid *& pSolid);
Standard_EXPORT    TCollection_AsciiString GetVoidExpn(McCadVoidCell *& pVoidCell);
};

#endif // MCCADMCNPWRITER_HXX
