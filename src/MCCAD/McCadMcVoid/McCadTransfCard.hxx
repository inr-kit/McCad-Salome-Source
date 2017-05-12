#ifndef MCCADTRANSFCARD_HXX
#define MCCADTRANSFCARD_HXX

#include <gp_Ax3.hxx>
#include <vector>
#include <TCollection_AsciiString.hxx>

using namespace::std;

class McCadTransfCard
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

    McCadTransfCard();
    McCadTransfCard(gp_Ax3 theAxis, gp_Pnt theApex);    

    vector<Standard_Real> GetPrmtList();    /**< Get coefficients list */
    TCollection_AsciiString GetSymb();      /**< Get the symbol of transform card */

    void SetTrNum(Standard_Integer iNum);   /**< Set the transform card number */
    Standard_Integer GetTrNum();            /**< Get the transform card number */

private:

    void GenerateTransfCard();              /**< Generate the transform card */

private:

    Standard_Integer m_iTrNum;              /**< Transform card number */
    vector<Standard_Real> m_PrmtList;       /**< Coefficient list */
    gp_Ax3 m_Axis;                          /**< The axis of rotated cone or cylinder */
    gp_Pnt m_pntApex;                       /**< The rotated point */
};

#endif // MCCADTRANSFCARD_HXX
