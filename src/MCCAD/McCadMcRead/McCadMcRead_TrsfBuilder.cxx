#include <McCadMcRead_TrsfBuilder.ixx>


bool isAngle=false;

McCadMcRead_TrsfBuilder::McCadMcRead_TrsfBuilder()
{

  myTrsf =  new McCadCSGGeom_Transformation;
  NotDone();

}
Standard_Boolean McCadMcRead_TrsfBuilder::IsDone() const
{
  return myIsDone;
}

void McCadMcRead_TrsfBuilder::Done()
{
  myIsDone = Standard_True;
}

void McCadMcRead_TrsfBuilder::NotDone()
{
  myIsDone = Standard_False;
}

void McCadMcRead_TrsfBuilder::BuildTrsf(const TCollection_AsciiString& sDesc)
{
  if(!Parse(sDesc))
    {
    ::cout << "Can not build Trsf matrix for this Card " << endl;
    ::cout << sDesc.ToCString() << endl;
    NotDone();
    }
  Done();
}

Standard_Boolean McCadMcRead_TrsfBuilder::Parse(const TCollection_AsciiString& sDesc)
{
  bool sig = false;
  TCollection_AsciiString sTmp =  sDesc;
  //Read  prefix;
  if(!ParsePrefix(sTmp))
    {
      return false;
    }
  // TCollection_AsciiString sTmp =  sDesc;
  sTmp.LeftAdjust();
  sTmp.RightAdjust();
  // sEnd is now the vector
  TCollection_AsciiString sEnd = sTmp.Split(sDesc.Search(" "));
  // sTmp is *TRn
  int num = (sTmp.SubString(sTmp.Search("R")+1,sTmp.Search(" ")-1)).IntegerValue();

  if (num <= 0 )
    {
      ::cout << "Trsf matrix for this Card is less than 0!" << endl;
      return false;
    }
  else myTrsf->SetIdNum(num);

  sTmp.Clear();
  sTmp = sEnd;

  sEnd.Clear();
  Handle(TColStd_HSequenceOfReal) vecSeq = new TColStd_HSequenceOfReal;


  // get transformation values
  while(sTmp.Length()>0)
  {
	  sTmp.LeftAdjust();

	  if(sTmp.Search(" ") > 1 )
	  {
		  sEnd = sTmp.Split(sTmp.Search(" "));
		  sEnd.UpperCase();

		  if(sEnd.Search("J") > 0) // skip operator used
		  {
			  TCollection_AsciiString theJ = sEnd.Split(sEnd.Search("J"));
			  Standard_Integer numberOfSkips(1);
			  if(sEnd.IsIntegerValue())
				  numberOfSkips = sEnd.IntegerValue();
			  else
				  cout << "WARNING!!! Number of skips is not an integer value in transformation card : " << num << endl;

			  Standard_Real cero(0.0), one(1.0);

			  for(int i=1; i<=numberOfSkips; i++)
			  {
				  if(i==4 || i==8 || i==12 )
					  vecSeq->Append(one);
				  else
					  vecSeq->Append(cero);
			  }
		  }
		  else if(sEnd.Search("R") > 0) // repeate operator used
		  {
			  TCollection_AsciiString theR = sEnd.Split(sEnd.Search("R"));
			  Standard_Integer numberOfRepeats(1);
			  if(sEnd.IsIntegerValue())
				  numberOfRepeats = sEnd.IntegerValue();
			  else
				  cout << "WARNING!!! Number of repeats is not an integer value in transformation card : " << num << endl;

			  Standard_Real repeatValue = vecSeq->Value(vecSeq->Length());

			  for(int i=1; i<=numberOfRepeats; i++)
					  vecSeq->Append(repeatValue);
		  }
		  else
			  vecSeq->Append(sTmp.RealValue());
	  }
      else // the last one
      {
    	  vecSeq->Append(sTmp.RealValue());
      }
      sTmp.Clear();
      sTmp = sEnd;
      sEnd.Clear();
  }
  // compute cosines of angels
  if(vecSeq->Length() >3 )
  {
      if(isAngle)
      {
		  for(int i = 4; i<=vecSeq->Length(); i++)
		  {
              vecSeq->SetValue(i,cos(vecSeq->Value(i)*M_PI/180.0));
		  }
      }
  }

  switch(vecSeq->Length())
  {
	  case 3:
		  myTrsf->SetVValues(vecSeq->Value(1),vecSeq->Value(2),vecSeq->Value(3));
		  sig = true;
		  break;
	  case 5:
		  cout << "Only full matrix entry is allowed" << endl;
		  sig = false;
		  break;
	  case 6:
		  cout << "Only full matrix entry is allowed" << endl;
		  sig = false;
		  break;
	  case 12:
		  myTrsf->SetValues(vecSeq->Value(1),vecSeq->Value(2),vecSeq->Value(3),
		  vecSeq->Value(4),vecSeq->Value(5),vecSeq->Value(6),
		  vecSeq->Value(7),vecSeq->Value(8),vecSeq->Value(9),
		  vecSeq->Value(10),vecSeq->Value(11),vecSeq->Value(12));
		  sig = true;
		  break;
    case 13:
    	cout << "Only full matrix entry is allowed" << endl;
    	sig = false;
    	break;
    default:
    	sig = false;
    	break;
  }
  if (sig == false)
    {
      cout << "Only full matrix entry is allowed" << endl;
      cout << "------------------------------------------------------------------------" << endl;
    }
  return sig;
}


Standard_Boolean McCadMcRead_TrsfBuilder::ParsePrefix(const TCollection_AsciiString& sDesc)
{
  if(sDesc.IsEmpty())                  //It is a blank line;
    {
      return false;
    }
  if(sDesc.Search("*") > 0 )
    {
      isAngle = true;
    }
  else
    {
      isAngle = false;
    }
  return true;
}

Handle(McCadCSGGeom_Transformation) McCadMcRead_TrsfBuilder::GetTrsf() const
{
  return myTrsf;
}

