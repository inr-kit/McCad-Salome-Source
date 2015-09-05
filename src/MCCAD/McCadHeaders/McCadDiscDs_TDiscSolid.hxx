#include <Standard.hxx>
#include <TDF_Attribute.hxx>
#include <McCadDiscDs_DiscSolid.hxx>

#ifndef _McCadDiscDs_DiscSolid
#define _McCadDiscDs_DiscSolid



class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;

class McCadDiscDs_TDiscSolid : public TDF_Attribute
{
public:
	Standard_EXPORT static const Standard_GUID& GetID() ;
	Standard_EXPORT const Standard_GUID& ID() const;
	Standard_EXPORT void Restore(const Handle(TDF_Attribute)& With) ;
	Standard_EXPORT void Paste(const Handle(TDF_Attribute)& Into,const Handle(TDF_RelocationTable)& RT) const;
	Standard_EXPORT Handle_TDF_Attribute NewEmpty() const;
	Standard_EXPORT virtual  Standard_OStream& Dump(Standard_OStream& anOS) const;

	Standard_EXPORT McCadDiscDs_TDiscSolid();


};

#endif



