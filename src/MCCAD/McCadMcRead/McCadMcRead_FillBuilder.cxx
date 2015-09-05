#include <McCadMcRead_FillBuilder.ixx>
#include <McCadMcRead_TrsfBuilder.hxx>

McCadMcRead_FillBuilder::McCadMcRead_FillBuilder()
{
  myFill = new McCadMcRead_Fill;
  NotDone();
}
Standard_Boolean McCadMcRead_FillBuilder::IsDone() const
{
  return myIsDone;
}

void McCadMcRead_FillBuilder::Done()
{
  myIsDone = Standard_True;
}

void McCadMcRead_FillBuilder::NotDone()
{
  myIsDone = Standard_False;
}

Standard_Boolean McCadMcRead_FillBuilder::Build(const TCollection_AsciiString& sDesc)
{
  if(!ExtractPrefix(sDesc))
    {
      NotDone();
      return false;

    }

  if(!ExtractEntry(sDesc))
    {
      NotDone();
      return false;
    }
  Done();
  return true;
}

Standard_Boolean McCadMcRead_FillBuilder::ExtractPrefix(const TCollection_AsciiString& sDesc)
{
  int index;
  index = sDesc.Search("*FILL");
  if (sDesc.Search("*FILL") > 0 )
    {
      myFill->SetTrsfMatTyp(1);
      return true;
    }
  else if (sDesc.Search("FILL") > 0 )
     {
      myFill->SetTrsfMatTyp(0);
      return true;
     }
  else return false;
}

Standard_Boolean McCadMcRead_FillBuilder::ExtractEntry(const TCollection_AsciiString& sDesc)
{
	  // theRest is card - " *Fill = "
	  TCollection_AsciiString tmpStr = sDesc;
	  tmpStr.LeftAdjust();
	  tmpStr.RightAdjust();

	  TCollection_AsciiString theRest = tmpStr.Split(tmpStr.Search("="));
	  theRest.LeftAdjust();
	  theRest.RightAdjust();

	  if (theRest.Search("(") > 0 &&  theRest.Search(")") > 0 ) // we have a TRSF Matrix or Num
	  {
		  TCollection_AsciiString sMiddle = theRest.SubString(theRest.Search("(")+1,theRest.Search(")")-1);
		  sMiddle.LeftAdjust();  sMiddle.RightAdjust();

		  if (sMiddle.Search(" ") > 0) // we have a TRSF Matrix
		  {
			  // prepend Number
			  if (myFill->GetTrsfMatTyp() == 0)
				  sMiddle.InsertBefore(1, TCollection_AsciiString("TR1000  "));
			  else
				  sMiddle.InsertBefore(1, TCollection_AsciiString("*TR1000  "));
			  // bulid a trsf matrix and give it a Number;
			  McCadMcRead_TrsfBuilder TrBuilder;
			  TrBuilder.BuildTrsf(sMiddle);
			  myFill->SetTrsfMatNum(1000);
			  myFill->SetTrsfMat(TrBuilder.GetTrsf());
			  myFill->SetTrsfMatTyp(0); // the TrsfBuilder generates only this type which is cosine of angle.
		}
		else // we have a TRSF Matrix Number;
		{
		  myFill->SetTrsfMatNum(sMiddle.IntegerValue());
		}
	  }
	  else // we don't have a TRSF at all
	  {
		  myFill->SetTrsfMatNum(0);
	  }
	  // we get here the fill number

	  if (myFill->GetTrsfMatNum() == 0)
	  {
		  Standard_Integer aNum(0);
		  if(theRest.Search(" ") > 0)
			  theRest = theRest.SubString(1,theRest.Search(" "));

		  if(theRest.IsIntegerValue())
			  aNum = theRest.IntegerValue();
		  else
			  cout << "Fill Number not an Integer Value!!!\n";

		  myFill->SetFillNum(aNum);
	  }
	  else
	  {
		  TCollection_AsciiString sTmp = theRest.SubString(theRest.Search(")")+1,theRest.Length());
		  sTmp.LeftAdjust();
		  sTmp.RightAdjust();
		  int aNum = sTmp.IntegerValue();//(sTmp.SubString(1,sTmp.Search(" "))).IntegerValue();
		  myFill->SetFillNum(aNum);
	}

	return Standard_True;
}

Handle(McCadMcRead_Fill) McCadMcRead_FillBuilder::GetFiller() const
{

  return myFill;

}

