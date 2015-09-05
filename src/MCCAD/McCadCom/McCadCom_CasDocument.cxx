#include <McCadCom_CasDocument.ixx>

#include <McCadCom_CasView.hxx>
#include <McCadCom_ListIteratorOfListOfCasView.hxx>
#include <TPrsStd_AISViewer.hxx>

McCadCom_CasDocument::McCadCom_CasDocument()
{
	myContext = NULL;
}

McCadCom_CasDocument::McCadCom_CasDocument(const Handle(AIS_InteractiveContext)& theContext)
{
	myContext = theContext;
}


McCadCom_CasDocument::McCadCom_CasDocument(const Standard_Integer& theID)
{
	myContext = NULL;
	NewDocument(theID);
}

void McCadCom_CasDocument::SetTDoc(const Handle(TDocStd_Document)& theTDoc)
{
	myTDoc = theTDoc;
}

Handle(TDocStd_Document) McCadCom_CasDocument::GetTDoc()
{
	return myTDoc;
}


void McCadCom_CasDocument::SetBgColor(const Quantity_Color& theCol)
{
	myBgCol = theCol;
}

Quantity_Color McCadCom_CasDocument::GetBgColor() const
{
	return myBgCol;
}

void McCadCom_CasDocument::Notify() const
{
	// view update
	Handle_McCadCom_CasView theView;
	McCadCom_ListIteratorOfListOfCasView iter;
	for (iter.Initialize(myViews); iter.More(); iter.Next())
	{
		theView = iter.Value();
		theView->Update();
	}
}

void McCadCom_CasDocument::Attach(const Handle(McCadCom_CasView)& theView)
{
	myViews.Prepend(theView);
}

void McCadCom_CasDocument::AddShape(const Handle(AIS_Shape)& theAISShape)
{
	if(myContext != NULL)
	{
		myContext->Display(theAISShape);
	}
	else
	{
		cout << "The Document does not have a Viewer Context!! " << endl;
	}
}

void McCadCom_CasDocument::RemoveShape(const Handle(AIS_Shape)& theAISShape)
{
	if(myContext != NULL)
	{
		myContext->Erase(theAISShape);
	}
	else
	{
		cout << "The Document does not have a Viewer Context!! " << endl;
	}
}

void McCadCom_CasDocument::SetContext(const Handle(AIS_InteractiveContext)& theContext)
{
	myContext = theContext;
}

Handle(AIS_InteractiveContext) McCadCom_CasDocument::GetContext() const
{
	return myContext;
}

Handle(McCadCom_CasView) McCadCom_CasDocument::NewView()
{
	Handle(McCadCom_CasView) aView = new McCadCom_CasView(this);

	return aView;
}

void McCadCom_CasDocument::Views(McCadCom_ListOfCasView& theViewList)
{
	McCadCom_ListIteratorOfListOfCasView iter;
	for (iter.Initialize(myViews); iter.More(); iter.Next())
	{
		theViewList.Append(iter.Value());
	}
}

