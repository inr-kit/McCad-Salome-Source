#include <McCadMcVoid_ForwardCollision.ixx>
#include <McCadMcVoid.hxx>

#include <McCadDiscDs_DiscFace.hxx>
#include <McCadDiscDs_DiscSolid.hxx>
#include <McCadDiscDs_HSequenceOfDiscFace.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>

#include <TColgp_HSequenceOfPnt.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepClass_FaceClassifier.hxx>

#include <Bnd_Box.hxx>

#include <TopOpeBRep_ShapeIntersector.hxx>
#include <omp.h>

#include <time.h>
#include <STEPControl_Writer.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>


McCadMcVoid_ForwardCollision::McCadMcVoid_ForwardCollision()
{
	myTolerance = 1e-7;
	myIsDone = false;
	myVSolids = NULL;
}

McCadMcVoid_ForwardCollision::McCadMcVoid_ForwardCollision(Handle(McCadDiscDs_HSequenceOfDiscSolid)& theDShape)
{
	myTolerance = 1e-7;
	myIsDone = false;
	myVSolids = theDShape;
}


void McCadMcVoid_ForwardCollision::Init()
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// forward collision test among solids
	//  SeqCSG  is master complement set
	cout << "\nPerforming  forward collision test among solids!!"<< endl;
	cout << "Number of solids = "<< myVSolids->Length() << endl;
	cout << "=================================================================== " << endl;

	Standard_Integer shProgress(0), shNum(myVSolids->Length()), shOldCnt(-1);
	Standard_Integer useCores(1); // number of cores to use

	////////////////////////////////////////////////////////////////////////////////////////
	//settings for parallelization
#ifndef MCCAD_USECORES
	useCores = 1; //(omp_get_num_procs()-1);

	if (useCores < 1)
		useCores = 1;
#endif //MCCAD_USECORES

	// get environmental variable to set number of cores to use
	TCollection_AsciiString useProc;
	if(getenv("MCCAD_USECORES"))
		useProc = TCollection_AsciiString(getenv("MCCAD_USECORES"));
	else
		useProc = 1;
	useProc.LeftAdjust();
	useProc.RightAdjust();
	if(useProc.Length()>0)
	{
		if(useProc.IsIntegerValue())
		{
			Standard_Integer tmpInt = useProc.IntegerValue();
			if(tmpInt>0)
				useCores = tmpInt;
		}
	}

	if(useCores > 1)
	{
		cout << "\tNumber of available processors : " << omp_get_num_procs() << endl;
		cout << "\tNumber of used processors      : " << useCores << endl;
	}

	omp_set_num_threads(useCores);

	//////////////////////////////////////////////////////////////////////////////////////////
	// Perform forward collision test
#pragma omp parallel for
	for (int iter = 1; iter <= shNum; iter++)//For all Solids check for collision with other solid
	{
		// show progress in shell
		shProgress = Standard_Integer(100.0*Standard_Real(iter)/Standard_Real(shNum));
		if(shOldCnt < shProgress)
		{
			cout << "." << flush;
			shOldCnt = shProgress;
		}

		TopoDS_Solid aSol = TopoDS::Solid((myVSolids->Value(iter)).GetSolid());
		BRepClass3d_SolidClassifier bsc3d(aSol);
                McCadDiscDs_DiscSolid aDiscSolid = myVSolids->Value(iter);
                Bnd_Box aBB = aDiscSolid.GetBBox();
		//bsc3d.PerformInfinitePoint(myTolerance);

		//if (!bsc3d.State()==TopAbs_OUT)
		//	cout << "_#_McCadMcVoid_ForwardCollision.cxx :: Warning: There is a hole in the Shape!"<< endl;

		Handle(TopTools_HSequenceOfShape) aFaceSeq = new TopTools_HSequenceOfShape();
		for (int iter2 = iter+1; iter2 <= myVSolids->Length(); iter2++)//For all solids other then current solid test for overlaps
		{
			//bbox check
			TopoDS_Solid sSol = TopoDS::Solid((myVSolids->Value(iter2)).GetSolid());
			BRepClass3d_SolidClassifier asc3d (sSol);

                        McCadDiscDs_DiscSolid aDiscSolid = myVSolids->Value(iter2);
                        Bnd_Box sBB = aDiscSolid.GetBBox();

                        if(aBB.IsOut(sBB))
			{
					continue;
			}
			////////////////////////////////////////////////////////////////////
			// the same face with different orientation
			Standard_Boolean found = Standard_False;
			for (TopExp_Explorer exF(aSol,TopAbs_FACE); exF.More(); exF.Next()) //For all faces of first current solid delete 'distorted' twins
			{
				TopoDS_Face aF = TopoDS::Face(exF.Current());
				aFaceSeq->Clear();
				for (TopExp_Explorer exFF(sSol,TopAbs_FACE); exFF.More(); exFF.Next())//For all faces of second current solid
				{
					TopoDS_Face sF = TopoDS::Face(exFF.Current());
					aFaceSeq->Append(aF);
					aFaceSeq->Append(sF);
					aFaceSeq = McCadMcVoid::DeleteOne(aFaceSeq); //if twin faces occure that differ in their orientation delete one.
					if (aFaceSeq->Length() < 2)
					{
						found = Standard_True;
						break;
					}
					aFaceSeq->Clear();
				}
				if(found) break;
			}
			if(found) //same face different orientation --> two non colliding neighbouring solids
				continue;

			///////////////////////////////////////////////////////////////////////
			Standard_Boolean inState = Standard_False;
			// Standard_Boolean oneInState = Standard_False;
			///////////////////////////////////////////////////////////////////////
			Handle(McCadDiscDs_HSequenceOfDiscFace) vFaces = (myVSolids->Value(iter2)).GetOwnVFaces();
			for(int ip = 1; ip <= vFaces->Length(); ip++)//For all faces check if their sample points lie inside the current solid
			{
				Bnd_Box fBB = (vFaces->Value(ip)).GetBBox();
				if(aBB.IsOut(fBB))//check if face is out of BB
				{
					// cout<< ssct <<  "." << ip << " BB based exclusion of a Face!!" << endl;
					continue;
				}
				// this is forward check

				Handle(TColgp_HSequenceOfPnt) aPntSeq = (vFaces->Value(ip)).GetPoints();
				for (int k=1; k<=aPntSeq->Length();k++)
				{
					gp_Pnt aP = aPntSeq->Value(k);

					if(aBB.IsOut(aP))
						continue;

					// if(McCadMcVoid::PointState(aSol,aP) == TopAbs_IN )	//Check if point is inside the first current solid
					bsc3d.Perform(aP,1.0e-05); // we check with higher tolerance to avoid  backward check!
					if ( bsc3d.State() == TopAbs_IN && McCadMcVoid::PointState(aSol,aP) == TopAbs_IN)
					{
						inState = Standard_True; //Collision Found!!!
						/*STEPControl_Writer colWrt;
						colWrt.Transfer(BRepBuilderAPI_MakeVertex(aP).Shape(), STEPControl_AsIs);
						colWrt.Transfer(aSol, STEPControl_AsIs);
						colWrt.Write("aCollision.stp");*/
						//exit(-1);

						break;
					}
				}
				if (inState)
					break;
			}

			if (inState) //vanilla
			{
				(myVSolids->ChangeValue(iter)).AppendOtherVSolid(iter2);
				continue;
			}
			///////////////////////////////////////////////////////////////////////////////////////////////////
			// otherwise simple backward check due to small voids may be available !!
			// this is necessary!!
			Handle(TColgp_HSequenceOfPnt) aPntSeq = (myVSolids->Value(iter)).GetVoxel();
			for (int k=1; k<=aPntSeq->Length();k++)
			{
				gp_Pnt aP = aPntSeq->Value(k);

				if(sBB.IsOut(aP))
					continue;

				// asc3d.Perform(aP,1.0e-07);
				// if (asc3d.State() == TopAbs_IN)
				if(McCadMcVoid::PointState(sSol,aP) == TopAbs_IN && asc3d.State() == TopAbs_IN)
				{
					inState = Standard_True;
					break;
				}
			}
			if (inState) //vanilla
			{
				(myVSolids->ChangeValue(iter)).AppendOtherVSolid(iter2);
				continue;
			}
		}
	}

	cout << "#" << endl;
	myIsDone = true;

	//cout << "=================================================================== " << endl;
	//cout << "Collision check finished    \n"<< endl;
	///////////////////////////////////////////////////////////////////////////////////////////
}

void McCadMcVoid_ForwardCollision::SetDsSolid(const Handle(McCadDiscDs_HSequenceOfDiscSolid)& theDShape)
{
	myVSolids = theDShape;
}

Standard_Boolean McCadMcVoid_ForwardCollision::IsDone() const
{
	return myIsDone;
}

void McCadMcVoid_ForwardCollision::SetTolerance(const Standard_Real theTol)
{
	myTolerance = theTol;
}

Standard_Real McCadMcVoid_ForwardCollision::GetTolerance() const
{
	return myTolerance;
}
