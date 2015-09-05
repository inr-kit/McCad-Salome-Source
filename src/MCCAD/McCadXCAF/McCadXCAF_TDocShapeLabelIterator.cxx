/*
 * McCadXCAF_TDocShapeLabelIterator.cxx
 *
 *  Created on: Mar 5, 2009
 *      Author: grosse
 */

#include <McCadXCAF_TDocShapeLabelIterator.hxx>

McCadXCAF_TDocShapeLabelIterator::McCadXCAF_TDocShapeLabelIterator()
{

}

McCadXCAF_TDocShapeLabelIterator::McCadXCAF_TDocShapeLabelIterator(const Handle_TDocStd_Document& theTDoc)
{
	Initialize(theTDoc);
}


void McCadXCAF_TDocShapeLabelIterator::Initialize(const Handle_TDocStd_Document& theTDoc)
{
	myCurrentCnt = 1;

	mySTool = XCAFDoc_DocumentTool::ShapeTool(theTDoc->Main());
	mySTool->GetFreeShapes(myFreeShapes);

	//cout << ":: Number of free shapes : " << myFreeShapes.Length() << endl;

	myEnd = NumberOfShapes();
}


Standard_Boolean McCadXCAF_TDocShapeLabelIterator::More()
{
	if(myCurrentCnt <= myEnd)
		return Standard_True;

	return Standard_False;
}


void McCadXCAF_TDocShapeLabelIterator::Next()
{
	myCurrentCnt++;
	myPrevious = myCurrent;
}

TDF_Label McCadXCAF_TDocShapeLabelIterator::Current()
{
	Standard_Integer cnt(0);
	TDF_Label retLab;

	for(Standard_Integer i=1; i <= myFreeShapes.Length(); i++)
	{
		cnt++; //increase number of shapes
		TDF_Label curLab = myFreeShapes.Value(i);

		if(cnt == myCurrentCnt)
		{
			retLab = curLab;
			break;
		}

		retLab = Traverse(curLab, cnt, myCurrentCnt);

		if(cnt==myCurrentCnt)
			break;
	}

	myCurrent = retLab;

	return myCurrent;
}


TDF_Label McCadXCAF_TDocShapeLabelIterator::GetAt(int index)
{
    myCurrentCnt = index;
    return Current();
}



TDF_Label McCadXCAF_TDocShapeLabelIterator::Previous()
{
	return myPrevious;
}

TDF_Label McCadXCAF_TDocShapeLabelIterator::Value()
{
	return Current();
}

/**
 *
 *
 *  Private Functions
 */
Standard_Integer McCadXCAF_TDocShapeLabelIterator::NumberOfShapes()
{
	Standard_Integer cnt(0);

	for(Standard_Integer i=1; i <= myFreeShapes.Length(); i++)
	{
		cnt++; //increase number of shapes
		TDF_Label curLab = myFreeShapes.Value(i);
		Traverse(curLab, cnt);
	}

	return cnt;
}


TDF_Label McCadXCAF_TDocShapeLabelIterator::Traverse(const TDF_Label& theLab, Standard_Integer& theInt, const Standard_Integer& stop)
{
	TDF_Label aLab = theLab;

	for( TDF_ChildIterator it( theLab, Standard_False ); it.More(); it.Next() )
	{
		if(mySTool->IsShape(it.Value()))
		{
			theInt++;

			Traverse( it.Value() , theInt, stop );
			if(theInt == stop && stop != 0)
				return it.Value();
		}
	}

	return aLab;
}
