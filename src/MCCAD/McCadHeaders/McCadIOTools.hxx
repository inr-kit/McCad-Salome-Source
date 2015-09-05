#ifndef MCCADIOTOOLS_HXX
#define MCCADIOTOOLS_HXX

#include <TCollection_AsciiString.hxx>
#include <McCadCom_CasDocument.hxx>

class McCadIOTools {
public:
    static void Export( const TCollection_AsciiString& extension, Handle_McCadCom_CasDocument& document );
    static void Import( const TCollection_AsciiString& extension, Handle_McCadCom_CasDocument& document );
};


#endif // MCCADIOTOOLS_HXX
