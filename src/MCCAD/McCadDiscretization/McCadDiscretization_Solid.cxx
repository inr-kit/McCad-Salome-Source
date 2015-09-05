#include <McCadDiscretization_Solid.ixx>

#include <McCadDiscDs_DiscFace.hxx>
#include <McCadDiscretization_Face.hxx>
#include <McCadDiscDs_HSequenceOfDiscFace.hxx>

#include <gp_Pnt.hxx>

#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>

#include <BRep_Tool.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <STEPControl_Writer.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>


McCadDiscretization_Solid::McCadDiscretization_Solid()
{
  myMaxNbPnt = 50;
  myMinNbPnt = 10;
  myXlen = 50; // 5cm resolution
  myYlen = 50;
  myRlen = 3.14/(2*myMaxNbPnt); //angular resolution
  myTolrenace = 1.0e-07;
  myIsDone = false;
}

McCadDiscretization_Solid::McCadDiscretization_Solid( McCadDiscDs_DiscSolid& theDsSolid)
{

  myDiscSolid = theDsSolid;
  myMaxNbPnt = 50;
  myMinNbPnt = 10;
  myXlen = 50; // 5cm resolution
  myYlen = 50;
  myRlen = 3.14/(2*myMaxNbPnt);
  myTolrenace = 1.0e-07;
  myIsDone = false;

}

void McCadDiscretization_Solid::Init()
{
  Handle(McCadDiscDs_HSequenceOfDiscFace) theDFaces =  myDiscSolid.GetOwnVFaces();
  Handle(McCadDiscDs_HSequenceOfDiscFace) tmpDFaces =  new McCadDiscDs_HSequenceOfDiscFace;

  Bnd_Box  solBB;

  Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt();

  for (Standard_Integer i=1;  i <= theDFaces->Length(); i++)
  {
	  McCadDiscDs_DiscFace theVFace =  theDFaces->Value(i);
	  McCadDiscretization_Face myDisc(theVFace);

	  myDisc.SetMinNbPnt(myMinNbPnt);
	  myDisc.SetMaxNbPnt(myMaxNbPnt);
	  myDisc.SetXresolution(myXlen);
	  myDisc.SetYresolution(myYlen);
	  myDisc.SetTolerance(myTolrenace);

	  myDisc.Init();

	  if(myDisc.IsDone())
	  {
		  tmpDFaces->Append(myDisc.GetFace());
		  Bnd_Box aBB = myDisc.GetFace().GetBBox();
		  aBB.SetGap(0.0);
		  if(aBB.IsWhole() || aBB.IsVoid() || aBB.IsThin(1e-7))
			  cout << "_#_McCadDiscretization_Solid.cxx :: Face Bounding box computation failed !!" << endl;
		  solBB.Add(aBB);

		  thePntSeq->Append(myDisc.GetFace().GetPoints());

	  }
	  else
		  cout << "Face discretization failed!!" << endl;
  }

  //////////////////////////////////////////////////////////////////////////////
  solBB.SetGap(0.0);

  if(solBB.IsWhole() || solBB.IsVoid() || solBB.IsThin(1e-7))
  {
	  cout << "_#_McCadDiscretization_Solid.cxx :: Bounding box computation failed; new computation follows!!" << endl;
	  TopoDS_Shape aSol = myDiscSolid.GetSolid();
	  Bnd_Box  aBBox;
	  for (TopExp_Explorer exV(aSol,TopAbs_VERTEX); exV.More(); exV.Next())
	  {
		  gp_Pnt  aP = BRep_Tool::Pnt(TopoDS::Vertex(exV.Current()));
		  aBBox.Add(aP);
	  }
	  solBB = aBBox;
	  if(solBB.IsWhole() || solBB.IsVoid() || solBB.IsThin(1e-7))
		  cout << "_#_McCadDiscretization_Solid.cxx :: Bounding box computation definitely failed !!" << endl;
  }

  myDiscSolid.SetOwnVFaces(tmpDFaces);
  myDiscSolid.SetBBox(solBB);
  myDiscSolid.SetIsDiscret(true);
  myIsDone = true;
}

void McCadDiscretization_Solid::SetSolid(McCadDiscDs_DiscSolid& theDsSolid)
{
  myDiscSolid = theDsSolid;
}

McCadDiscDs_DiscSolid McCadDiscretization_Solid::GetSolid() const
{
  return myDiscSolid;
}

Standard_Boolean McCadDiscretization_Solid::IsDone() const
{
  return myIsDone;
}

void McCadDiscretization_Solid::SetMinNbPnt(const Standard_Integer theNumb)
{
  myMinNbPnt = theNumb;
}

Standard_Integer McCadDiscretization_Solid::GetMinNbPnt() const
{
  return myMinNbPnt;
}

void McCadDiscretization_Solid::SetMaxNbPnt(const Standard_Integer theNumb)
{
  myMaxNbPnt = theNumb;
}

Standard_Integer McCadDiscretization_Solid::GetMaxNbPnt() const
{
  return myMaxNbPnt;
}

void McCadDiscretization_Solid::SetXresolution(const Standard_Real theXlen)
{
  myXlen = theXlen;
}

Standard_Real McCadDiscretization_Solid::GetXresolution() const
{
  return myXlen;
}

void McCadDiscretization_Solid::SetYresolution(const Standard_Real theYlen)
{
  myYlen = theYlen;
}

Standard_Real McCadDiscretization_Solid::GetYresolution() const
{
   return myYlen;
}

void McCadDiscretization_Solid::SetTolerance(const Standard_Real theTol)
{
  myTolrenace = theTol;
}

Standard_Real McCadDiscretization_Solid::GetTolerance() const
{
  return myTolrenace;
}

