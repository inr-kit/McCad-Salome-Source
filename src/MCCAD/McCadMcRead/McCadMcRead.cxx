#include <McCadMcRead.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfReal.hxx>

Standard_Boolean McCadMcRead::ExtractInt(TCollection_AsciiString& sIn,Standard_Integer& nVal) 
{       
  sIn.LeftAdjust();
  Standard_Integer i = sIn.Search(" ");
  TCollection_AsciiString theRest = sIn.Split(i);
  if (sIn.Length() == 0) return false;
  nVal = sIn.IntegerValue();
  sIn =theRest;  
  return true;

}

Standard_Boolean McCadMcRead::ExtractInt(TCollection_AsciiString& sIn,Standard_Integer& nStart,Standard_Integer& nVal) 
{ 
  if(nStart >= sIn.Length())
    {
      return false;
    }
  TCollection_AsciiString theRest = sIn.Split(nStart);
  if(! ExtractInt(theRest,nVal)) 
    {
      sIn = sIn + theRest;
      return false;
    }
  sIn = sIn + theRest;
  return true;
}

Standard_Boolean McCadMcRead::ExtractReal(TCollection_AsciiString& sIn,Standard_Real& nVal) 
{

  sIn.LeftAdjust();
  Standard_Integer i = sIn.Search(" ");
  TCollection_AsciiString theRest = sIn.Split(i);
  if (sIn.Length() == 0) return false;
  nVal = sIn.RealValue();
  sIn =theRest;  
  return true;


}

Standard_Boolean McCadMcRead::ExtractParenthesisPair(TCollection_AsciiString& sIn,Standard_Integer& nStart,
TCollection_AsciiString& sOut) 
{
  if(nStart<0 || nStart >= sIn.Length())        //nStart should between [0,sIn.length()];
    {
      sOut.Clear();
      return true;
    }
  Standard_Integer iBegin = sIn.Search("(");
  Standard_Integer iEnd = sIn.Search(")");
  sOut = sIn.SubString(iBegin,iEnd);
  sIn.Remove(iBegin,iEnd-iBegin);
  return true;

}

Standard_Boolean McCadMcRead::ExtractFloat(TCollection_AsciiString& sIn,Standard_Integer& nStart,Standard_Real& fVal) 
{
  if(sIn.Length() <= nStart)         //Start Position exceed  end of the string
    {
      return false;
    }
  Standard_Integer i = sIn.Search(" ");
  TCollection_AsciiString theRest = sIn.Split(i);
  if (sIn.Length() == 0) return false;
  fVal = sIn.RealValue();
  sIn =sIn + theRest;  
  return true; 
}

Standard_Boolean McCadMcRead::ExtractFloat(TCollection_AsciiString& sIn,Standard_Integer& nStart,
Handle(TColStd_HSequenceOfReal)& fValVec) 
{
  if(sIn.Length() <= nStart)         //Start Position exceed  end of the string
    {
      return false;
    }
  Standard_Real fVal;
  while(ExtractFloat(sIn,nStart,fVal))
    {
      fValVec->Append(fVal);
    }
  return true;
}

void McCadMcRead::TrimLeft(TCollection_AsciiString& sIn) 
{
  sIn.LeftAdjust();
}

void McCadMcRead::TrimRight(TCollection_AsciiString& sIn) 
{
  sIn.RightAdjust();
}

