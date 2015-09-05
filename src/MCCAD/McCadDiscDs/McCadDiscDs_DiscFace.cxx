#include <McCadDiscDs_DiscFace.ixx>

#include <GeomAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>

#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

McCadDiscDs_DiscFace::McCadDiscDs_DiscFace()
{

  myVoxelSeq = new TColgp_HSequenceOfPnt();



}

McCadDiscDs_DiscFace::McCadDiscDs_DiscFace(const TopoDS_Face& theFace)
{

 myVoxelSeq = new TColgp_HSequenceOfPnt();
 myFace = theFace;

}

void McCadDiscDs_DiscFace::SetFace(const TopoDS_Face& theFace) 
{

  myFace = theFace;

}

TopoDS_Face McCadDiscDs_DiscFace::GetFace() const
{
  return myFace;
}

Standard_Boolean McCadDiscDs_DiscFace::IsDiscret() const
{
  if (myVoxelSeq->Length() > 0 ) return true;
  else return false;
}

void McCadDiscDs_DiscFace::SetPoints(const Handle(TColgp_HSequenceOfPnt)& thePntSeq) 
{
  myVoxelSeq = thePntSeq;
}

Handle(TColgp_HSequenceOfPnt) McCadDiscDs_DiscFace::GetPoints() const
{
  return myVoxelSeq;
}

void McCadDiscDs_DiscFace::SetBBox(const Bnd_Box& theBBox) 
{

  myBBox = theBBox;

}

Bnd_Box McCadDiscDs_DiscFace::GetBBox() const
{
  return myBBox;
}

Standard_Integer McCadDiscDs_DiscFace::NbOfPoints() const
{
  return myVoxelSeq->Length();
}

Standard_Real McCadDiscDs_DiscFace::FaceArea() const
{
  GProp_GProps GP;
  BRepGProp::SurfaceProperties(myFace,GP);
  return GP.Mass();
}

GeomAbs_SurfaceType McCadDiscDs_DiscFace::FaceSurfaceType() const
{
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface(myFace);
  GeomAdaptor_Surface theAdapter(aSurface);
  return theAdapter.GetType();
}

