#ifndef IMCCADWRITER_HXX
#define IMCCADWRITER_HXX

#include "McCadVoidCellManager.hxx"

class IMcCadWriter
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

Standard_EXPORT    IMcCadWriter();
Standard_EXPORT    virtual ~IMcCadWriter();

protected:
    McCadVoidCellManager * m_pManager;          /**< Corresponding manager */
    Standard_Boolean m_bHaveMaterial;           /**< If the material file is existed, set true, else set false */
    Standard_Boolean m_bGenerateVoid;           /**< The switch of void generation */
    TCollection_AsciiString m_OutputFileName;   /**< The name of output file */

    Standard_Integer m_iInitCellNum;            /**< The initial number of cell */
    Standard_Integer m_iInitFaceNum;            /**< The initial number of face */

public:
    /** Print the output file, include head information, cell, void, surface, transform card */
Standard_EXPORT    virtual void PrintFile(){};                 /**< Print the file */

Standard_EXPORT    virtual void PrintHeadDesc(Standard_OStream& theStream){};  /**< Print the head */
Standard_EXPORT    virtual void PrintCellDesc(Standard_OStream& theStream){};  /**< Print the cell card */
Standard_EXPORT    virtual void PrintVoidDesc(Standard_OStream& theStream){};  /**< Print the void card */
Standard_EXPORT    virtual void PrintSurfDesc(Standard_OStream& theStream){};  /**< Print the surfaces list */
Standard_EXPORT    virtual void PrintTrsfDesc(Standard_OStream& theStream){};  /**< Print the transform card */
Standard_EXPORT    virtual void PrintMatCard(Standard_OStream& theStream){};   /**< Print the material card */
//qiu not implement    virtual void PrintVolumeCard(Standard_OStream& theStream){};   /**< Print the volume card */

    /** Print the group information, including material name, group name, material id and density */
Standard_EXPORT    virtual void PrintGroupInfo(const int iSolid, Standard_OStream& theStream){};

Standard_EXPORT    virtual void SetFileName(TCollection_AsciiString theFileName);  /**< Set the output file name */
Standard_EXPORT    virtual void SetManager(McCadVoidCellManager *pManager);        /**< Connect to the manager */
Standard_EXPORT    virtual void SetMaterial(Standard_Boolean bMat);                /**< Set the switch of outputing of material*/
Standard_EXPORT    virtual void SetVoid(Standard_Boolean bVoid);                   /**< Set the switch of outputing of void card*/
};

#endif // MCCADWRITER_HXX
