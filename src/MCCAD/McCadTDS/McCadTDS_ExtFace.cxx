#include <McCadTDS_ExtFace.ixx>

McCadTDS_ExtFace::McCadTDS_ExtFace()
{
  myPosPoints = new  TColgp_HSequenceOfPnt();
  myNegPoints = new  TColgp_HSequenceOfPnt();
  myHaveExt = Standard_False;
	
}

McCadTDS_ExtFace::McCadTDS_ExtFace(const TopoDS_Face& theFace)
{

  myPosPoints = new  TColgp_HSequenceOfPnt;
  myNegPoints = new  TColgp_HSequenceOfPnt;
  myHaveExt = Standard_False;
  myFace = theFace;
}

void McCadTDS_ExtFace::SetFace(const TopoDS_Face& theFace) 
{
  myFace = theFace;
}

TopoDS_Face McCadTDS_ExtFace::GetFace() const
{
  return myFace;
}

void McCadTDS_ExtFace::AppendPosPnt(const gp_Pnt& thePnt) 
{
   myPosPoints->Append(thePnt);
}

void McCadTDS_ExtFace::AppendNegPnt(const gp_Pnt& thePnt) 
{
   myNegPoints->Append(thePnt);
}

gp_Pnt McCadTDS_ExtFace::GetPosPnt() const
{
  return myPosPoints->Value(myPosPoints->Length());
}

gp_Pnt McCadTDS_ExtFace::GetNegPnt() const
{
  return myNegPoints->Value(myNegPoints->Length());
}

Standard_Integer McCadTDS_ExtFace::NbPosPnt() const
{
  return myPosPoints->Length();
}

Standard_Integer McCadTDS_ExtFace::NbNegPnt() const
{
 return myNegPoints->Length();
}

Handle(TColgp_HSequenceOfPnt) McCadTDS_ExtFace::GetPosPoints() const
{
    return myPosPoints;
}

Handle(TColgp_HSequenceOfPnt) McCadTDS_ExtFace::GetNegPoints() const
{
   return myNegPoints;
}

void McCadTDS_ExtFace::SetHaveExt(const Standard_Boolean theHaveExt) 
{
  myHaveExt   = theHaveExt;
}

Standard_Boolean McCadTDS_ExtFace::HaveExt() const
{
  return myHaveExt;
}

