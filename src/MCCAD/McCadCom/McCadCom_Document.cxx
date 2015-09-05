#include <McCadCom_Document.ixx>
#include <McCadCom_ListIteratorOfListOfView.hxx>

McCadCom_Document::McCadCom_Document()
{
	//  myViews;
	myName = "McCadDocument";
	myIsModified = Standard_False;
	myIsInitialized = Standard_False;
	myIsNew = Standard_True;

}

Standard_Boolean McCadCom_Document::NewDocument(const Standard_Integer& theID)
{
	TCollection_ExtendedString Name;

	if(theID < 0)
		Name = "Unnamed Document";
	else
		Name = TCollection_ExtendedString("Unnamed Document ") + TCollection_ExtendedString(theID);


	SetDocName(Name);
	//SetModified(Standard_True);
	myIsInitialized = Standard_True;
	myIsNew = Standard_True;

        return Standard_True;
}


Standard_Boolean McCadCom_Document::IsNew()
{
	return myIsNew;
}


Standard_Boolean McCadCom_Document::IsInitialized() const
{
	return myIsInitialized;
}

Standard_Boolean McCadCom_Document::OpenFile(const TCollection_ExtendedString& aFileName)
{
    return Standard_False;
}

Standard_Boolean McCadCom_Document::SaveFile()
{
    return Standard_False;
}

Standard_Boolean McCadCom_Document::SaveAsFile(const TCollection_ExtendedString& aFileName)
{
    return Standard_False;
}

void McCadCom_Document::Update() const
{
}

void McCadCom_Document::Notify() const
{

}

void McCadCom_Document::Detach(const Handle(McCadCom_View)& theView)
{

}

void McCadCom_Document::SetDocName(const TCollection_ExtendedString& theName)
{
	myIsNew = Standard_False;
	myName = theName;
}

TCollection_ExtendedString McCadCom_Document::GetDocName() const
{
	return myName;
}

void McCadCom_Document::SetIsModified(const Standard_Boolean theMod)
{
	myIsModified = theMod;
}

Standard_Boolean McCadCom_Document::IsSetModified() const
{
	return myIsModified;
}

