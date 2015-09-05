#include <McCadCom_View.ixx>

McCadCom_View::McCadCom_View()
{
	myDocument = NULL;
}

void McCadCom_View::Update() const
{
}

Handle(McCadCom_Document) McCadCom_View::GetDocument() const
{
	return myDocument;
}

void McCadCom_View::SetDocument(const Handle(McCadCom_Document)& theDoc)
{
	myDocument = theDoc;
}

