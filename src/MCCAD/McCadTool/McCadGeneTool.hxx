#ifndef MCCADGENETOOL_HXX
#define MCCADGENETOOL_HXX

#include <TCollection_AsciiString.hxx>
#include <QString>
#include <TopTools_HSequenceOfShape.hxx>

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
    Standard_EXPORT static void WriteFile(TCollection_AsciiString outFileName,
                                    Handle_TopTools_HSequenceOfShape &shapes);
    Standard_EXPORT static Handle_TopTools_HSequenceOfShape readFile( TCollection_AsciiString fileName);
};

#endif // MCCADGENETOOL_HXX
