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

    IMcCadWriter();
    virtual ~IMcCadWriter();

protected:
    McCadVoidCellManager * m_pManager;          /**< Corresponding manager */
    Standard_Boolean m_bHaveMaterial;           /**< If the material file is existed, set true, else set false */
    Standard_Boolean m_bGenerateVoid;           /**< The switch of void generation */
    TCollection_AsciiString m_OutputFileName;   /**< The name of output file */

    Standard_Integer m_iInitCellNum;            /**< The initial number of cell */
    Standard_Integer m_iInitFaceNum;            /**< The initial number of face */

public:
    /** Print the output file, include head information, cell, void, surface, transform card */
    virtual void PrintFile(){};                 /**< Print the file */

    virtual void PrintHeadDesc(Standard_OStream& theStream){};  /**< Print the head */
    virtual void PrintCellDesc(Standard_OStream& theStream){};  /**< Print the cell card */
    virtual void PrintVoidDesc(Standard_OStream& theStream){};  /**< Print the void card */
    virtual void PrintSurfDesc(Standard_OStream& theStream){};  /**< Print the surfaces list */
    virtual void PrintTrsfDesc(Standard_OStream& theStream){};  /**< Print the transform card */
    virtual void PrintMatCard(Standard_OStream& theStream){};   /**< Print the material card */

    /** Print the group information, including material name, group name, material id and density */
    virtual void PrintGroupInfo(const int iSolid, Standard_OStream& theStream){};

    virtual void SetFileName(TCollection_AsciiString theFileName);  /**< Set the output file name */
    virtual void SetManager(McCadVoidCellManager *pManager);        /**< Connect to the manager */
    virtual void SetMaterial(Standard_Boolean bMat);                /**< Set the switch of outputing of material*/
    virtual void SetVoid(Standard_Boolean bVoid);                   /**< Set the switch of outputing of void card*/
};

#endif // MCCADWRITER_HXX
