#ifndef MCCADINPUTMODELDATA_HXX
#define MCCADINPUTMODELDATA_HXX


#include <Handle_TopTools_HSequenceOfShape.hxx>
#include <TCollection_AsciiString.hxx>

class McCadInputModelData
{
public:
    McCadInputModelData();
    bool LoadSTEPModel(TCollection_AsciiString inputModel);
    Handle_TopTools_HSequenceOfShape GetModelData();
    bool AddFile(const TCollection_AsciiString &file);

private:
    Handle_TopTools_HSequenceOfShape m_listModelData; // Add by Lei, 13/08/2012

};

#endif // MCCADINPUTMODELDATA_HXX
