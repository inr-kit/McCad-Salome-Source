#include <McCadTDS_NormFace.ixx>

McCadTDS_NormFace::McCadTDS_NormFace()
{ 
  myHaveExt = Standard_False;
  myID = 0;
  myVertPnt = new TColgp_HSequenceOfPnt;
  myVoxelSeq = new TColgp_HSequenceOfPnt;
}

McCadTDS_NormFace::McCadTDS_NormFace(const TopoDS_Face& theFace)
{
  myFace  = theFace;      
  myOirentation = theFace.Orientation();
  myHaveExt = Standard_False;
  myID = 0;
  myVertPnt = new TColgp_HSequenceOfPnt;
  myVoxelSeq = new TColgp_HSequenceOfPnt;
}

void McCadTDS_NormFace::SetFace(const TopoDS_Face& theFace) 
{
  myFace  = theFace;      
  myOirentation = theFace.Orientation();
}

TopoDS_Face McCadTDS_NormFace::GetFace() const
{
  return myFace;
}

void McCadTDS_NormFace::SetVoxel(const Handle(TColgp_HSequenceOfPnt)& thePntSeq) 
{
  myVoxelSeq->Clear();
  myVoxelSeq->Append(thePntSeq);

}

Handle(TColgp_HSequenceOfPnt) McCadTDS_NormFace::GetVoxel() const
{
  return  myVoxelSeq;
}

void McCadTDS_NormFace::SetVertPnt(const Handle(TColgp_HSequenceOfPnt)& thePntSeq) 
{
 myVertPnt->Clear();
 myVertPnt->Append(thePntSeq);
}

Handle(TColgp_HSequenceOfPnt) McCadTDS_NormFace::GetVertPnt() const
{
  return myVertPnt;
}

void McCadTDS_NormFace::SetOirentation(const TopAbs_Orientation theOrient) 
{
  myOirentation = theOrient;
}

TopAbs_Orientation McCadTDS_NormFace::GetOirentation() const
{
  return myOirentation;
}

Standard_Integer McCadTDS_NormFace::NbVoxelPnt() const
{
  return myVoxelSeq->Length();
}

Standard_Integer McCadTDS_NormFace::NbVertPnt() const
{
  return myVertPnt->Length();
}

void McCadTDS_NormFace::SetBBox(const Bnd_Box& theBBox) 
{
  myBBox = theBBox;
}

Bnd_Box McCadTDS_NormFace::GetBBox() const
{
  return myBBox;
}

void McCadTDS_NormFace::SetID(const Standard_Integer theID) 
{
  myID = theID;
}

Standard_Integer McCadTDS_NormFace::GetID() const
{
  return myID;
}

void McCadTDS_NormFace::DoVertPnt() 
{
  for (TopExp_Explorer exV(myFace,TopAbs_VERTEX); exV.More(); exV.Next())
    { 
      gp_Pnt  aPnt = BRep_Tool::Pnt(TopoDS::Vertex(exV.Current()));
      myVertPnt->Append(aPnt);
    }


}

void McCadTDS_NormFace::DoVoxel() 
{
}

void McCadTDS_NormFace::DoBBox() 
{
}
