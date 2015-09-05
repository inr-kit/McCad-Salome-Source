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

    vector<Standard_Real> GetPrmtList();
    TCollection_AsciiString GetSymb();

    void SetTrNum(Standard_Integer iNum);
    Standard_Integer GetTrNum();

private:

    Standard_Integer m_iTrNum;
    vector<Standard_Real> m_PrmtList;

};

#endif // MCCADTRANSFCARD_HXX
