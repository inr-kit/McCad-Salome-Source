#include <McCadTDS_ExtSolid.ixx>
#include <TopoDS.hxx>

McCadTDS_ExtSolid::McCadTDS_ExtSolid()
{
	myFaces = new TopTools_HSequenceOfShape;
	myHaveExt = Standard_False;
	myComplementFaces = new TopTools_HSequenceOfShape;
	myHaveComplement = Standard_False;
	myComment = TCollection_AsciiString("c       McCad Generated Decomposed Solid!");
}


McCadTDS_ExtSolid::McCadTDS_ExtSolid(const TopoDS_Solid& theSolid)
{
	mySolid = theSolid;
	myFaces = new TopTools_HSequenceOfShape;
	myHaveExt = Standard_False;
	myComplementFaces = new TopTools_HSequenceOfShape;
	myHaveComplement = Standard_False;
	myComment = TCollection_AsciiString("c       McCad Generated Decomposed Solid!");
}


void McCadTDS_ExtSolid::SetSolid(const TopoDS_Solid& theSolid)
{
	mySolid = theSolid;
}


TopoDS_Solid McCadTDS_ExtSolid::GetSolid() const
{
	return mySolid;
}


void McCadTDS_ExtSolid::AppendFace(const TopoDS_Face& theFace)
{
	myFaces->Append(theFace);
}


void McCadTDS_ExtSolid::AppendFaces(const Handle(TopTools_HSequenceOfShape)& theFaceSeq)
{
	myFaces->Append(theFaceSeq);
}


TopoDS_Face McCadTDS_ExtSolid::GetFace() const
{
	return TopoDS::Face(myFaces->Value(myFaces->Length()));
}


Handle(TopTools_HSequenceOfShape) McCadTDS_ExtSolid::GetFaces() const
{
	return myFaces;
}


void McCadTDS_ExtSolid::SetHaveExt(const Standard_Boolean theHaveExt)
{
	myHaveExt = theHaveExt;
}


Standard_Boolean McCadTDS_ExtSolid::HaveExt() const
{
	return myHaveExt;
}


void McCadTDS_ExtSolid::SetHaveComp(const Standard_Boolean theHaveComp)
{
	myHaveComplement = theHaveComp;
}


Standard_Boolean McCadTDS_ExtSolid::HaveComp() const
{
	return myHaveComplement;
}


void McCadTDS_ExtSolid::AppendCompFace(const TopoDS_Face& theFace)
{
	myComplementFaces->Append(theFace);
}


void McCadTDS_ExtSolid::AppendCompFaces(const Handle(TopTools_HSequenceOfShape)& theFaceSeq)
{
	myComplementFaces->Append(theFaceSeq);
}


TopoDS_Face McCadTDS_ExtSolid::GetCompFace() const
{
	return TopoDS::Face(myComplementFaces->Value(myFaces->Length()));
}


Handle(TopTools_HSequenceOfShape) McCadTDS_ExtSolid::GetCompFaces() const
{
	return myComplementFaces;
}


void McCadTDS_ExtSolid::SetComment(const TCollection_AsciiString& theComment)
{
	myComment = theComment;
}


TCollection_AsciiString McCadTDS_ExtSolid::GetComment() const
{
	return myComment;
}


void McCadTDS_ExtSolid::PrintComment(Standard_OStream& theStream)
{
	theStream << (myComment).ToCString() << "  " << endl;
}
