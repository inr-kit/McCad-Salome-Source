#include <McCadMcRead_Fill.ixx>

McCadMcRead_Fill::McCadMcRead_Fill()
{
  myFillNum= 0; 
  myTrsfMatNum = 0; 
 
  myTrsfMatType = 0; 
}

McCadMcRead_Fill::McCadMcRead_Fill(const Standard_Integer theNum)
{
  myFillNum = theNum;
  myTrsfMatNum = 0;
  myTrsfMatType = 0; 
}

void McCadMcRead_Fill::SetFillNum(const Standard_Integer theNum) 
{
  myFillNum = theNum;
}

Standard_Integer McCadMcRead_Fill::GetFillNum() const
{
  return  myFillNum;
}

void McCadMcRead_Fill::SetTrsfMatNum(const Standard_Integer theNum) 
{

  myTrsfMatNum = theNum;

}

Standard_Integer McCadMcRead_Fill::GetTrsfMatNum() const
{

  return myTrsfMatNum;

}

void McCadMcRead_Fill::SetTrsfMatTyp(const Standard_Integer theNum) 
{
  myTrsfMatType = theNum;

}

Standard_Integer McCadMcRead_Fill::GetTrsfMatTyp() const
{
  return myTrsfMatType;
}

void McCadMcRead_Fill::SetTrsfMat(const Handle(McCadCSGGeom_Transformation)& theTrsf) 
{
  myTrsf = theTrsf; 
}

Handle(McCadCSGGeom_Transformation) McCadMcRead_Fill::GetTrsfMat() const
{
   return myTrsf; 
}

