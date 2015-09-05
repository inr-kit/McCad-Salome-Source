#include <McCadDiscretization_Face.ixx>

#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pln.hxx>
#include <gp_XYZ.hxx>
#include <gp_Trsf.hxx>

#include <TColgp_HSequenceOfPnt.hxx>
#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Surface.hxx>

#include <BRepClass_FaceClassifier.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <TopAbs_State.hxx>
#include <STEPControl_Writer.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>

#include <math_RealRandom.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

McCadDiscretization_Face::McCadDiscretization_Face()
{
	myMaxNbPnt = 50;
	myMinNbPnt = 10;
	myXlen = 50; // 5cm resolution
	myYlen = 50;
	myRlen = 3.14/(2*myMaxNbPnt);
	myTolerance = 1.0e-07;
	myIsDone = false;
}


McCadDiscretization_Face::McCadDiscretization_Face(	McCadDiscDs_DiscFace& theDsFace)
{
	myDiscFace = theDsFace;
	myMaxNbPnt = 50;
	myMinNbPnt = 10;
	myXlen = 50; // 5cm resolution
	myYlen = 50;
	myRlen = 3.14/(2*myMaxNbPnt);
	myTolerance = 1.0e-07;
	myIsDone = false;
}


void McCadDiscretization_Face::Init()
{
	BRepClass_FaceClassifier bsc3d;
	Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt();
	//////////////////////////////////////////////////////////////////////////////////////
	//!!!!!!! Note this is for test purpose!!!!!
	// for (TopExp_Explorer ex(aFace,TopAbs_VERTEX); ex.More(); ex.Next())
	// thePntSeq->Append(BRep_Tool::Pnt(TopoDS::Vertex(ex.Current())));
	// cout << "Total Number of sample points computed = " << thePntSeq->Length() << endl;
	// return thePntSeq;
	//////////////////////////////////////////////////////////////////////////////////////
	BRepAdaptor_Curve2d c;

	TopoDS_Face theFace = TopoDS::Face(myDiscFace.GetFace());

	BRepAdaptor_Surface BS(theFace, Standard_True);
	GeomAdaptor_Surface theASurface = BS.Surface();

	/*Standard_Real uMin, uMax, vMin, vMax, uMean, vMean;
	uMin = theASurface.FirstUParameter();
	uMax = theASurface.LastUParameter();
	vMin = theASurface.FirstVParameter();
	vMax = theASurface.LastVParameter();
	uMean = (uMin + uMax)/2.0;
	vMean = (vMin + vMax)/2.0;
*/

//	Standard_Integer nbBrimPoints(0);

	for (TopExp_Explorer ex(theFace, TopAbs_EDGE); ex.More(); ex.Next())
	{
		c.Initialize(TopoDS::Edge(ex.Current()), theFace);
		Standard_Real f = (!(Precision::IsNegativeInfinite(c.FirstParameter()))) ? c.FirstParameter() : -1.0;
		Standard_Real l = (!(Precision::IsPositiveInfinite(c.LastParameter()))) ? c.LastParameter()	: 1.0;

		gp_Pnt2d pf = c.Value(f);
		gp_Pnt2d pl = c.Value(l);
		gp_Pnt p3f;
		theASurface.D0(pf.X(), pf.Y(), p3f);
		gp_Pnt p3l;
		theASurface.D0(pl.X(), pl.Y(), p3l);
		Standard_Real Len = p3f.Distance(p3l);
		Standard_Integer NUMPNT;
		if (int(Len/myXlen) < myMinNbPnt)
			NUMPNT = myMinNbPnt;
		else if (int(Len/myXlen) > myMaxNbPnt)
			NUMPNT = myMaxNbPnt;
		else
			NUMPNT = int(Len/myXlen);

		for (int t=0; t<=NUMPNT; t++)
		{
			Standard_Real a = Standard_Real(t)/Standard_Real(NUMPNT);
			Standard_Real par = (1-a)*f + a*l;
			gp_Pnt2d p2 = c.Value(par);
			Standard_Real U = p2.X();
			Standard_Real V = p2.Y();

			gp_Pnt p1;
			theASurface.D0(U, V, p1);

			bsc3d.Perform(theFace, p2, myTolerance);

			if (bsc3d.State() == TopAbs_IN || bsc3d.State() == TopAbs_ON)
				thePntSeq->Append(p1);
		}

	}

//	nbBrimPoints = thePntSeq->Length();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	Standard_Real U1, U2, V1, V2;
	BRepTools::UVBounds(theFace, U1, U2, V1, V2);

	if (theASurface.GetType() == GeomAbs_Cone ||theASurface.GetType() == GeomAbs_Cylinder)
	{
		myXlen = myRlen;
	}
	if (theASurface.GetType() == GeomAbs_Sphere ||theASurface.GetType()	== GeomAbs_Torus)
	{
		myXlen = myRlen;
		myYlen = myRlen;
	}

	// cout << "U1 =  " << U1  << " U2 =  " << U2  <<  " V1 =  " << V1  <<  " V2 =  " << V2  << endl;
	Standard_Real du=1, dv=1;

	Standard_Real XLen=U2-U1;
	Standard_Real YLen=V2-V1;
	Standard_Integer XNUMPNT;
	if (int(XLen/myXlen) < myMinNbPnt)
		XNUMPNT = myMinNbPnt;
	else if (int(XLen/myXlen) > myMaxNbPnt)
		XNUMPNT = myMaxNbPnt;
	else
		XNUMPNT = int(XLen/myXlen);
	// cout << "XNUMPNT =  " << XNUMPNT  << endl;
	Standard_Integer YNUMPNT;
	if (int(YLen/myYlen) < myMinNbPnt)
		YNUMPNT = myMinNbPnt;
	else if (int(YLen/myYlen) > myMaxNbPnt)
		YNUMPNT = myMaxNbPnt;
	else
		YNUMPNT = int(YLen/myYlen);
	//  cout << "YNUMPNT =  " << YNUMPNT  << endl;

	Handle(TColgp_HSequenceOfPnt) tmpPntSeq = new TColgp_HSequenceOfPnt();
	Standard_Integer repeat = 0;
	do
	{
		if (repeat > 3)
			break;

		du = XLen/Standard_Real(XNUMPNT);
		dv = YLen/Standard_Real(YNUMPNT);

		tmpPntSeq->Clear();

		for (int j=0; j<=YNUMPNT; j++)
		{
			Standard_Real V = V1+dv*j;

			for (int i=0; i<=XNUMPNT; i++)
			{
				gp_Pnt p1;
				Standard_Real U = U1+du*i;
				theASurface.D0(U, V, p1);
				gp_Pnt2d p2(U, V);

				bsc3d.Perform(theFace, p2, myTolerance);
				if (bsc3d.State() == TopAbs_IN /*|| bsc3d.State() == TopAbs_ON*/) //MYTEST comment
					tmpPntSeq->Append(p1);
			}
		}

		XNUMPNT=XNUMPNT*2;
		YNUMPNT=YNUMPNT*2;
		repeat++;
	} while (tmpPntSeq->Length() < myMinNbPnt*myMinNbPnt);

	thePntSeq->Append(tmpPntSeq);

	// we add vetices here
	for (TopExp_Explorer exv(theFace, TopAbs_VERTEX); exv.More(); exv.Next())
	{
		gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(exv.Current()));
		thePntSeq->Prepend(aPnt);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// remove sample points which have been added multiple times
	for (int j=2; j<= thePntSeq->Length(); j++)
	{
		gp_XYZ p1 = (thePntSeq->Value(j)).XYZ();
		for (int jk=j-1; jk >= 1; jk--)
		{
			gp_XYZ p2 = (thePntSeq->Value(jk)).XYZ();

			//cout << p1.X()*p1.X() + p1.Y()*p1.Y() + p1.Z()*p1.Z() + p2.X()*p2.X() + p2.Y()*p2.Y() + p2.Z()*p2.Z() << endl;

			if (p1.IsEqual(p2, 1.e-05))
			{
				thePntSeq->Remove(jk);
				j=jk;
			}
		}
	}


	// cout << "Total Number of face sample points computed = " << thePntSeq->Length() << endl;
	Bnd_Box aBB; // make a tight box!!!!


	BRepBndLib::Add(theFace, aBB);
	aBB.SetGap(0);


	if (aBB.IsWhole() || aBB.IsVoid() || aBB.IsThin(1e-7))
		cout << "_#_McCadDiscretization_Face.cxx :: Face Bounding box computation failed !!" << endl;

	//myDiscFace.SetBrimPointIndex(nbBrimPoints);

	myDiscFace.SetPoints(thePntSeq);
	myDiscFace.SetBBox(aBB);

    myIsDone = true;
}


void McCadDiscretization_Face::SetFace(McCadDiscDs_DiscFace& theDsFace)
{
	myDiscFace = theDsFace;
}


McCadDiscDs_DiscFace McCadDiscretization_Face::GetFace() const
{
	return myDiscFace;
}


Standard_Boolean McCadDiscretization_Face::IsDone() const
{
	return myIsDone;
}


void McCadDiscretization_Face::SetMinNbPnt(const Standard_Integer theNumb)
{
	myMinNbPnt = theNumb;
}


Standard_Integer McCadDiscretization_Face::GetMinNbPnt() const
{
	return myMinNbPnt;
}


void McCadDiscretization_Face::SetMaxNbPnt(const Standard_Integer theNumb)
{
	myMaxNbPnt = theNumb;
}


Standard_Integer McCadDiscretization_Face::GetMaxNbPnt() const
{
	return myMaxNbPnt;
}

void McCadDiscretization_Face::SetXresolution(const Standard_Real theXlen)
{
	myXlen = theXlen;
}


Standard_Real McCadDiscretization_Face::GetXresolution() const
{
	return myXlen;
}


void McCadDiscretization_Face::SetYresolution(const Standard_Real theYlen)
{
	myYlen = theYlen;
}


Standard_Real McCadDiscretization_Face::GetYresolution() const
{
	return myYlen;
}


void McCadDiscretization_Face::SetTolerance(const Standard_Real theTol)
{
	myTolerance = theTol;
}


Standard_Real McCadDiscretization_Face::GetTolerance() const
{
	return myTolerance;
}

