#include <McCadMcRead_Cell.ixx>
#include <McCadBoolExp_PostFixer.hxx>

McCadMcRead_Cell::McCadMcRead_Cell()
{
  myIsPseudo = false;

  myTRCLNumber = 0;
  myHaveLat = false;
  myHaveLike = false;
  myHaveU = false;
  myHaveFill = false;
  myHaveTRCL  = false;
  myHaveMacro  = false;

  myLat = 0;
  myLike= 0;
  myU = 0;
  myFillSeq = new  McCadMcRead_HSequenceOfFill;
  myFillPlaceSeq = new TColgp_HSequenceOfPnt;

  myLikeNum = 0;
  myLikePtr = NULL;

  myCompsNum =  new TColStd_HSequenceOfInteger;
  myCompPtrs = new McCadMcRead_HSequenceOfCellPtr;

  myPseudoNum = 0;

  myInFixCSG = new TColStd_HSequenceOfAsciiString;
  myPostFixCSG = new TColStd_HSequenceOfAsciiString;
  myIsInfiniteLattice = Standard_False;

  myCSGHasChanged = true;

  // myTrsf McCadCSGGeom_Transformation;

}

McCadMcRead_Cell::McCadMcRead_Cell(const Standard_Integer theCNum,const Standard_Integer theMNum,const Standard_Real theDensity):
  McCadCSGGeom_Cell(theCNum,theMNum,theDensity)
{
  myIsPseudo = false;
  myTRCLNumber = 0;
  myHaveLat = false;
  myHaveLike = false;
  myHaveU = false;
  myHaveFill = false;
  myHaveTRCL  = false;
  myHaveMacro  = false;

  myLat = 0;
  myLike= 0;
  myU = 0;
  myFillSeq = new  McCadMcRead_HSequenceOfFill;

  myLikeNum = 0;
  myLikePtr = NULL;

  myCompsNum =  new TColStd_HSequenceOfInteger;
  myCompPtrs = new McCadMcRead_HSequenceOfCellPtr;

  myPseudoNum = 0;

  myInFixCSG = new TColStd_HSequenceOfAsciiString;
  myPostFixCSG = new TColStd_HSequenceOfAsciiString;
  myIsInfiniteLattice = Standard_False;

  myCSGHasChanged = true;
}

void McCadMcRead_Cell::SetPseudo(const Standard_Boolean thePseudo)
{
  myIsPseudo = thePseudo;

}

Standard_Boolean McCadMcRead_Cell::IsPseudo() const
{
  return myIsPseudo;
}

void McCadMcRead_Cell::SetHaveTRCL(const Standard_Boolean theIndic)
{
  myHaveTRCL = theIndic;

}

Standard_Boolean McCadMcRead_Cell::IsTRCL() const
{
  return myHaveTRCL;
}

void McCadMcRead_Cell::SetTRCLNumber(const Standard_Integer theCount)
{
  myTRCLNumber = theCount;
}

Standard_Integer McCadMcRead_Cell::GetTRCLNumber() const
{

  return myTRCLNumber;

}

void McCadMcRead_Cell::SetHaveLat(const Standard_Boolean theIndic)
{

  myHaveLat =  theIndic;

}
void McCadMcRead_Cell::SetLat(const Standard_Integer theNum)
{

  myLat = theNum;

}
Standard_Integer McCadMcRead_Cell::GetLatNum() const
{
  return myLat;
}

Standard_Boolean McCadMcRead_Cell::IsLat() const
{
  return myHaveLat;
}

void McCadMcRead_Cell::SetHaveLike(const Standard_Boolean theIndic)
{
  myHaveLike = theIndic;
}

Standard_Integer McCadMcRead_Cell::GetLikeNum() const
{
  return myLikeNum;
}

McCadMcRead_CellPtr McCadMcRead_Cell::GetLikePtr() const
{
  return myLikePtr;
}

void McCadMcRead_Cell::SetLikeNum(const Standard_Integer theIndex)
{

  myLikeNum = theIndex;

}

void McCadMcRead_Cell::SetLikePtr(const McCadMcRead_CellPtr& thePtr)
{
  myLikePtr = thePtr;
}

Standard_Boolean McCadMcRead_Cell::IsLike() const
{

  return myHaveLike;

}

void McCadMcRead_Cell::SetHaveU(const Standard_Boolean theIn)
{
  myHaveU = theIn;

}

void McCadMcRead_Cell::SetUNum(const Standard_Integer theIndex)
{

  myU = theIndex;
}
Standard_Integer McCadMcRead_Cell::GetUNum() const
{

  return myU;

}



void McCadMcRead_Cell::SetPseudoNum(const Standard_Integer theIndex)
{
	myPseudoNum = theIndex;
}

Standard_Integer McCadMcRead_Cell::GetPseudoNum() const
{
	return myPseudoNum;
}

Standard_Boolean McCadMcRead_Cell::IsU() const
{
  return myHaveU;
}

void McCadMcRead_Cell::SetHaveFill(const Standard_Boolean theIn)
{
  myHaveFill = theIn;
}

void McCadMcRead_Cell::SetFill(const Handle(McCadMcRead_Fill)& theFiller)
{

  myFill = theFiller;

}

Standard_Boolean McCadMcRead_Cell::IsFill() const
{

  return myHaveFill;

}

void McCadMcRead_Cell::SetHaveMacro(const Standard_Boolean theIn)
{

  myHaveMacro = theIn;

}

Standard_Boolean McCadMcRead_Cell::HaveMacro() const
{

  return myHaveMacro;

}

void McCadMcRead_Cell::SetMacroIndex(const Handle(TColStd_HSequenceOfInteger)& theInSeq)
{

  myCompsNum->Append(theInSeq);

}

void McCadMcRead_Cell::SetMacroPtrs(const Handle(McCadMcRead_HSequenceOfCellPtr)& theInPtrSeq)
{

  myCompPtrs->Append(theInPtrSeq);

}

Handle(TColStd_HSequenceOfInteger) McCadMcRead_Cell::GetMacroIndex() const
{

  return myCompsNum;

}

Handle(McCadMcRead_HSequenceOfCellPtr) McCadMcRead_Cell::GetMacroPtrs() const
{

  return myCompPtrs;

}

void McCadMcRead_Cell::SetInFixCSG(const Handle(TColStd_HSequenceOfAsciiString)& theCSG)
{

  myInFixCSG->Append(theCSG);

}

Handle(TColStd_HSequenceOfAsciiString) McCadMcRead_Cell::GetInFixCSG() const
{

  return myInFixCSG;

}

void McCadMcRead_Cell::SetPostFixCSG(const Handle(TColStd_HSequenceOfAsciiString)& theCSG)
{
	myPostFixCSG->Clear();
	myPostFixCSG->Append(theCSG);
}

Handle(TColStd_HSequenceOfAsciiString) McCadMcRead_Cell::GetPostFixCSG()
{
    if(myCSGHasChanged) // recompute postfix csg
    {
        TCollection_AsciiString tmpStr;
        for(int i=1; i<= myCSG->Length(); i++)
        {
            tmpStr.InsertAfter(tmpStr.Length(), myCSG->Value(i));
            tmpStr.InsertAfter(tmpStr.Length()," ");
        }

        McCadBoolExp_PostFixer thePostFixer;
        thePostFixer.Init(tmpStr);

        myPostFixCSG = thePostFixer.GetPostFix();

        myCSGHasChanged = false;
    }

    return myPostFixCSG;
}

Handle(McCadMcRead_Fill) McCadMcRead_Cell::GetFill() const
{
  return myFill;

}

Handle(McCadMcRead_HSequenceOfFill) McCadMcRead_Cell::GetFillSeq() const
{
  return myFillSeq;
}

void McCadMcRead_Cell::SetFillSeq(const Handle(McCadMcRead_HSequenceOfFill)& theFillerSeq)
{
  myFillSeq->Append(theFillerSeq);
}

Handle(TColgp_HSequenceOfPnt) McCadMcRead_Cell::GetFillPlaceSeq() const
{
  return myFillPlaceSeq;
}

void McCadMcRead_Cell::SetFillPlaceSeq(const Handle(TColgp_HSequenceOfPnt)& thePlaceSeq)
{

  myFillPlaceSeq->Append(thePlaceSeq);

}

void McCadMcRead_Cell::SetTrsf(const Handle(McCadCSGGeom_Transformation)& theTrsf)
{
  myTrsf = theTrsf;

}

Handle(McCadCSGGeom_Transformation) McCadMcRead_Cell::GetTrsf() const
{
  return myTrsf;
}


void McCadMcRead_Cell::SetInfiniteLattice()
{
	myIsInfiniteLattice = Standard_True;
}

Standard_Boolean McCadMcRead_Cell::IsInfiniteLattice()
{
	return myIsInfiniteLattice;
}


void McCadMcRead_Cell::SetOuterCSG(Handle_TColStd_HSequenceOfAsciiString& csg)
{
    myOuterCSG = csg;
}

Handle_TColStd_HSequenceOfAsciiString McCadMcRead_Cell::GetOuterCSG()
{
    return myOuterCSG;
}

void McCadMcRead_Cell::SetLatFillBoundary(Handle_TColStd_HSequenceOfAsciiString& csg)
{
    myLatFillBoundary = csg;
}

Handle_TColStd_HSequenceOfAsciiString McCadMcRead_Cell::GetLatFillBoundary()
{
    return myLatFillBoundary;
}


void McCadMcRead_Cell::SetCSG(const Handle(TColStd_HSequenceOfAsciiString)& theCSG, bool updatePostFixCSG)
{
    myCSGHasChanged = updatePostFixCSG;
    myCSG = theCSG;
}
