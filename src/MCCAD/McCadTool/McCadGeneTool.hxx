#ifndef MCCADGENETOOL_HXX
#define MCCADGENETOOL_HXX

#include <TCollection_AsciiString.hxx>
#include <QString>

class McCadGeneTool
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

public:
    McCadGeneTool();
    Standard_EXPORT static TCollection_AsciiString ToAsciiString(const QString& theStr);
};

#endif // MCCADGENETOOL_HXX
