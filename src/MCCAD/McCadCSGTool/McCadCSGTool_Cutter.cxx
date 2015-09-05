#include <McCadCSGTool_Cutter.ixx>
#include <McCadGTOOL.hxx>
#include <McCadMessenger_Singleton.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>

#include <BRepAdaptor_Surface.hxx>

#include <BRepCheck.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepCheck_Analyzer.hxx>

#include <BRepClass3d_SolidClassifier.hxx>

#include <BRepTools.hxx>

#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>

#include <ShapeFix_ShapeTolerance.hxx>
#include <TCollection_AsciiString.hxx>


McCadCSGTool_Cutter::McCadCSGTool_Cutter()
{
    myIsDone = false;
    myLastCut = Standard_False;
}


McCadCSGTool_Cutter::McCadCSGTool_Cutter(const TopoDS_Shape& theSolid,const Handle(McCadTDS_ExtFace)& theExtFace)
{
    Init(theSolid,theExtFace);
}


void McCadCSGTool_Cutter::Init(const TopoDS_Shape& theSolid,const Handle(McCadTDS_ExtFace)& theExtFace)
{
        McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
	NotDone();
	mySolid = theSolid;
	myExtFace = theExtFace;
	myResultSolids = new TopTools_HSequenceOfShape();
	if ( (myExtFace->NbPosPnt() == 0) || (myExtFace->NbNegPnt() == 0) )
	{
		NotDone();
                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Extended face without sample points", McCadMessenger_ErrorMsg);
		return;
	}
	gp_Pnt posPnt(0,0,0);
	posPnt = myExtFace->GetPosPnt();
	gp_Pnt negPnt(0,0,0);
	negPnt = myExtFace->GetNegPnt();

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//  this is a test for extents and evaluation
	TopoDS_Face aFace = myExtFace->GetFace();
	BRepAdaptor_Surface BS(aFace,Standard_True);
	GeomAdaptor_Surface theAdaptFaceSurface = BS.Surface();

	////////////////////////////////////////////////////////////////
	// construct cutting half spaces
	TopoDS_Shape PosHSol = BRepPrimAPI_MakeHalfSpace(aFace,posPnt).Solid(); //positive cutting half space SOLID
	if(PosHSol.IsNull())
                msgr->Message("\n_#_McCadCSGTool_Cutter.cxx :: Solid of positive cutting half space is empty!!!",
                              McCadMessenger_WarningMsg);
	TopoDS_Shape NegHSol = BRepPrimAPI_MakeHalfSpace(aFace,negPnt).Solid(); //negative cutting half space SOLID
	if(NegHSol.IsNull())
                msgr->Message("\n_#_McCadCSGTool_Cutter.cxx :: Solid of negative cutting half space is empty!!!",
                              McCadMessenger_WarningMsg);

	Standard_Boolean pHalfCut = Standard_False;
	Standard_Boolean nHalfCut = Standard_False;

	////////////////////////////////////////////////////////////////////////////
	//
	Bnd_Box B;
	BRepBndLib::Add(mySolid, B);
	B.SetGap(1000.0);
	Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
	B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
	TopoDS_Shape aBox = BRepPrimAPI_MakeBox(gp_Pnt(aXmin, aYmin, aZmin), gp_Pnt(aXmax, aYmax, aZmax)).Shape();

	//////////////////////////////////////////////////////////////////////////
	// create pseudo half spaces (shrink to suitable size)
	TopoDS_Shape PosBox, NegBox;
	try
	{
		BRepAlgoAPI_Common posHCommon(PosHSol,aBox);
		if(posHCommon.IsDone())
		{
			PosBox = posHCommon.Shape();
			PosHSol = PosBox;
		}
		posHCommon.Destroy();
	}
	catch(Standard_DomainError)
	{
		Standard_Failure::Caught()->Print(cout); cout << endl;
	}
	try
	{
		BRepAlgoAPI_Common negHCommon(NegHSol,aBox);
		if(negHCommon.IsDone())
		{
			NegBox = negHCommon.Shape();
			NegHSol = NegBox;
		}
		negHCommon.Destroy();
	}
	catch(Standard_DomainError)
	{
		Standard_Failure::Caught()->Print(cout); cout << endl;
	}


	int itol=0, highTolCount=0;

	//////////////////////////////////////////////////////////////
	// JUMP-MASTER
 MASTER: //goto label; if cutting fails: try again using a higher tolerance

	////////////////////////////////////////////////////////////////////////
	// cutting with negative half space
	if (NegHSol.IsNull())
                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: A negative  Null Cut appears!!",
                              McCadMessenger_ErrorMsg);
	else
	{
		try
		{
			BRepAlgoAPI_Cut posCut(mySolid,NegHSol);

			if(posCut.IsDone())
			{
				pHalfCut = Standard_True;
				myPosPartSol = posCut.Shape();
			}
			else
			{
				pHalfCut = Standard_False;
                                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Positive side solid Cutting failed !!",
                                              McCadMessenger_WarningMsg);
			}
			posCut.Destroy();
		}
		catch(Standard_DomainError)
		{
			pHalfCut = Standard_False;
                        msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Boolean Operation on a halfspace has failed !!!",
                                      McCadMessenger_WarningMsg);
			cout << "OCC error message:  ";
			Standard_Failure::Caught()->Print(cout); cout << endl;
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	// cutting with positive half space
	if (PosHSol.IsNull())
                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: A positive  Null Cut appears!!",
                              McCadMessenger_WarningMsg);
	else
	{
		try
		{
			BRepAlgoAPI_Cut negCut(mySolid,PosHSol);

			if(negCut.IsDone())
			{
				nHalfCut = Standard_True;
				myNegPartSol = negCut.Shape();
			}
			else
			{
				nHalfCut = Standard_False;
                                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Negative side solid cutting failed !!",
                                              McCadMessenger_WarningMsg);
			}
			negCut.Destroy();
		}
		catch(Standard_DomainError)
		{
			nHalfCut = Standard_False;
                        msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Boolean Operation on a halfspace failed !!!",
                                      McCadMessenger_WarningMsg);
			cout << "OCC error :  ";
			Standard_Failure::Caught()->Print(cout); cout << endl;
		}
	}


	int sCount=0;

	////////////////////////////////////////////////////////////////
	// check if positive part is valid
	Standard_Boolean posPartNotValid(Standard_False);

	for (TopExp_Explorer ex(myPosPartSol,TopAbs_SOLID);ex.More();ex.Next())
	{
		sCount++;
		TopoDS_Solid solid = TopoDS::Solid(ex.Current());

		if(solid.IsNull())
		{
                    TCollection_AsciiString warningMessage("_#_McCadCSGTool_Cutter.cxx :: NULL Cut: P Cutting solid failed !!\n    Trying to use Boolean Common operation...");
                    msgr->Message(warningMessage.ToCString(), McCadMessenger_WarningMsg);
			posPartNotValid = Standard_True;
			break;
		}

		BRepCheck_Analyzer BA(solid,Standard_True);

		if(!BA.IsValid())
		{
			posPartNotValid = Standard_True;
			break;
		}
	}

		///////////////////////////////////////////////////////////
		// if cutting failed, try common operation
	if(posPartNotValid)
	{
		sCount = 0;

		try
		{
			BRepAlgoAPI_Common negCommon(mySolid, NegHSol);

			if(negCommon.IsDone())
			{
				pHalfCut = Standard_True;
				myPosPartSol = negCommon.Shape();
			}
			else
			{
				pHalfCut = Standard_False;
                                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Positive side solid Common Operation failed !!",
                                              McCadMessenger_WarningMsg);
			}
			negCommon.Destroy();
		}
		catch(Standard_DomainError)
		{
			pHalfCut = Standard_False;
                        msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Boolean Operation on a halfspace has failed !!!",
                                      McCadMessenger_WarningMsg);
			cout << "OCC error message:  ";
			Standard_Failure::Caught()->Print(cout); cout << endl;
		}

		for (TopExp_Explorer ex(myPosPartSol,TopAbs_SOLID);ex.More();ex.Next())
		{
			sCount++;
			TopoDS_Solid solid = TopoDS::Solid(ex.Current());

			if(solid.IsNull())
			{
                                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: NULL Common: P Common solid failed !!",
                                              McCadMessenger_WarningMsg);

				pHalfCut = Standard_False;
				break;
			}

			BRepCheck_Analyzer BA(solid, Standard_True);

			if(!BA.IsValid())
			{
				pHalfCut = Standard_False;
				break;
			}
		}
	}

	if(sCount==0)
		pHalfCut = Standard_False;

	sCount=0;

	//////////////////////////////////////////////////////////
	// check if negative part is valid
	Standard_Boolean negPartNotValid(Standard_False);

	for (TopExp_Explorer ex(myNegPartSol,TopAbs_SOLID);ex.More();ex.Next())
	{
		sCount++;
		TopoDS_Solid solid = TopoDS::Solid(ex.Current());

		if(solid.IsNull())
		{
                        msgr->Message("_#_McCadCSGTool_Cutter.cxx :: NULL Cut: N Cutting solid failed !!",
                                      McCadMessenger_WarningMsg);
			negPartNotValid = Standard_True;
			break;
		}

		BRepCheck_Analyzer BA(solid,Standard_True);


		if(!BA.IsValid())
		{
			negPartNotValid = Standard_True;

			break;
		}
	}
		////////////////////////////////////////////////////////
		// if cutting failed, try common operation
	if(negPartNotValid)
	{
		sCount = 0;

		try
		{
			BRepAlgoAPI_Common negCommon(mySolid, NegHSol);

			if(negCommon.IsDone())
			{
				nHalfCut = Standard_True;
				myNegPartSol = negCommon.Shape();
			}
			else
			{
				nHalfCut = Standard_False;
                                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Negative side solid Common Operation failed !!",
                                              McCadMessenger_DefaultMsg);
			}
			negCommon.Destroy();
		}
		catch(Standard_DomainError)
		{
			nHalfCut = Standard_False;
                        msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Boolean Operation on a halfspace has failed !!!",
                                      McCadMessenger_WarningMsg);
			cout << "OCC error message:  ";
			Standard_Failure::Caught()->Print(cout); cout << endl;
		}

		for (TopExp_Explorer ex(myNegPartSol,TopAbs_SOLID);ex.More();ex.Next())
		{
			sCount++;
			TopoDS_Solid solid = TopoDS::Solid(ex.Current());

			if(solid.IsNull())
			{
                                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: NULL Common: N Common solid failed !!",
                                              McCadMessenger_WarningMsg);
				nHalfCut = Standard_False;
				break;
			}

			BRepCheck_Analyzer BA(solid,Standard_True);

			if(!BA.IsValid())
			{
				nHalfCut = Standard_False;
				break;
			}
		}
	}

	if(sCount==0)
		nHalfCut = Standard_False; // if check is run on compound containing no solid

	///////////////////////////////////////////////////////////////////
	// Test if a boolean operation failed
	sCount=0;
	itol++;
	Standard_Real highTol(1.e-3);

	if ((!nHalfCut || !pHalfCut) && itol <=3)
	{
//qiu		Standard_Real locTol = 1.e-07*(pow(10,itol));
        Standard_Real locTol = 1.e-07*(pow(10.0,itol));
		highTol = locTol;
		cout << "_#_McCadCSGTool_Cutter.cxx :: Tolerance correction due to failed boolean operation.  --- " << itol << endl;
		cout << "                              Tolerance is :  " << locTol <<endl;
		ShapeFix_ShapeTolerance setter; Standard_Boolean sett;
		//sett = setter.LimitTolerance(mySolid,locTol,0.0,TopAbs_SOLID);
		sett = setter.LimitTolerance(PosHSol,locTol,0.0,TopAbs_SOLID);
		sett = setter.LimitTolerance(NegHSol,locTol,0.0,TopAbs_SOLID);
		goto MASTER;
	}

	////////////////////////////////////////////////////////////////
	if ((!pHalfCut || !nHalfCut) && !IsLastCut())
	{
                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Warning: Cutting failed may try again !!",
                              McCadMessenger_WarningMsg);
		NotDone();
		return;
	}
	////////////////////////////////////////////////////////////////
	// if cutting certainly fails we try common;
	// hihg tolerance BOP
	if ((!pHalfCut || !nHalfCut) && IsLastCut() && highTolCount < 1 )
	{
		highTolCount++;
		Standard_Real locTol = highTol;
//qiu		highTol = locTol*(pow(10,highTolCount));
		highTol = locTol*(pow(10.0,highTolCount));

		cout << "******************************************************************************" << endl;
		cout << highTolCount << ".  High Tolerance correction due to failed boolean operation. " << endl;
		cout << "Warning: Tolerance is high may introduce error :  " << highTol <<endl;
		cout << "******************************************************************************" << endl;

		ShapeFix_ShapeTolerance setter;
		Standard_Boolean sett;
		sett = setter.LimitTolerance(PosHSol,highTol,0.0,TopAbs_SOLID);
		sett = setter.LimitTolerance(NegHSol,highTol,0.0,TopAbs_SOLID);
		goto MASTER;
	}

	// repair
	/*
	 if (!pHalfCut && IsLastCut())
	 {
	 myPosPartSol =  RepairOpenShell(mySolid,PosBox,aFace);
	 }
	 if (!nHalfCut && IsLastCut())
	 {
	 myNegPartSol =  RepairOpenShell(mySolid,NegBox,aFace);
	 }
	 */
	//////////////////////////////////////////////////////////////////////
	//if (myPosPartSol.IsNull()) cout << "Common of Positive part is Null!!" << endl;
	//if (myNegPartSol.IsNull()) cout << "Common of negative part is Null!!" << endl;


/*
	if(myPosPartSol.IsNull())
		cout << "_#_McCadCSGTool_Cutter.cxx :: myPosPartSol is NULL! " << endl;
	else
	{
	//	cout << "Common of Positive part is   "; TopAbs::Print(myPosPartSol.ShapeType(),cout); cout << endl;
	}
	if(myNegPartSol.IsNull())
		cout << "_#_McCadCSGTool_Cutter.cxx :: myNegPartSol is NULL! " << endl;
	else
	{
	//	cout << "Common of Negative part is   "; TopAbs::Print(myNegPartSol.ShapeType(),cout); cout << endl;
	}
*/

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//expand by  shell shells

	for (TopExp_Explorer ex(myPosPartSol,TopAbs_SHELL);ex.More();ex.Next())
	{
		TopoDS_Shell tmpShell = TopoDS::Shell(ex.Current());
		//////////////////////////////////////////////////////////////////////////
		BRepCheck_Shell shellCheck(tmpShell);
		//BRepCheck::Print(shellCheck.Orientation(),cout);cout<< endl;
		if (shellCheck.Closed(Standard_False) == BRepCheck_NoError)
		{
			BRepBuilderAPI_MakeSolid Bu(tmpShell);
			if(Bu.IsDone())
			{
				TopoDS_Solid aSolid = Bu.Solid();
				if(aSolid.IsNull())
                                        msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Warning: Null solid in Positive part!!",
                                                      McCadMessenger_WarningMsg);
				BRepClass3d_SolidClassifier bsc3d (aSolid);
				Standard_Real t = Precision::Confusion();
				bsc3d.PerformInfinitePoint(t);

				if (bsc3d.State() == TopAbs_IN)
					cout << "Infinity in Solid, therefore discaded !! " << endl;
				else
					myResultSolids->Append(aSolid);
			}
			else
			{
                                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Model may be invalid: Solid computation from shell failed!! ",
                                              McCadMessenger_WarningMsg);
			}
		}
		else
                        msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Model may be invalid - open shell",
                                      McCadMessenger_WarningMsg);
	}

	//  cout << " Total number of shells after cutting   " << endl;

	for (TopExp_Explorer ex(myNegPartSol,TopAbs_SHELL);ex.More();ex.Next())
	{

		TopoDS_Shell tmpShell = TopoDS::Shell(ex.Current());
		BRepCheck_Shell shellCheck(tmpShell);
	//	BRepCheck::Print(shellCheck.Orientation(),cout); cout << endl;
		if (shellCheck.Closed(Standard_False) == BRepCheck_NoError)
		{
			BRepBuilderAPI_MakeSolid Bu(tmpShell);
			if(Bu.IsDone())
			{
				TopoDS_Solid aSolid = Bu.Solid();
				if(aSolid.IsNull())
                                        msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Warning: Null solid in Positive part!!",
                                                      McCadMessenger_WarningMsg);

				BRepClass3d_SolidClassifier bsc3d (aSolid);
				Standard_Real t = Precision::Confusion();
				bsc3d.PerformInfinitePoint(t);

				if (bsc3d.State() == TopAbs_IN)
                                        msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Infinity in Solid, therefore discarded !! ",
                                                      McCadMessenger_WarningMsg);
				else
					myResultSolids->Append(aSolid);
			}
			else
			{
                                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Model may be invalid: Solid computation from shell failed!! ",
                                              McCadMessenger_WarningMsg);
			}
		}
		else
		{
                        msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Model may be invalid - open shell!!",
                                      McCadMessenger_WarningMsg);
		}
		/////////////////////////////////////////////////////////////////////////
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// content analysis of result solids
	//cout << "////////////////////////////////////////////////////////////////////////////////// " << endl;
	//cout << " Cut Result =  " << myResultSolids->Length() << endl;
	if (!IsLastCut() && myResultSolids->Length() <= 1)
	{
                msgr->Message("_#_McCadCSGTool_Cutter.cxx :: Warning: Cut result may be degenerate !!",
                              McCadMessenger_WarningMsg);
		NotDone();
		return;
	}
	TopExp_Explorer ex;
	int iv;
	for ( int i=1; i<=myResultSolids->Length(); i++)
	{
		for (ex.Init(myResultSolids->Value(i),TopAbs_FACE), iv=1;ex.More();ex.Next(), iv++)
		{
			TopoDS_Face theFace = TopoDS::Face(ex.Current());
			BRepAdaptor_Surface BS(theFace,Standard_True);
			GeomAdaptor_Surface theAdaptFaceSurface = BS.Surface();
			if(theAdaptFaceSurface.GetType() != GeomAbs_Plane)
			{
			//	cout << "Solid:  "<< i << " Face =  " << iv << "     non-planar face .....................!!"<< endl;
			}
			else
			{
				gp_Pln aPln = theAdaptFaceSurface.Plane();
				Standard_Real A,B,C,D;
				aPln. Coefficients(A,B,C,D);
			//	cout << "Solid:  "<< i << " Face =  " << iv << "  A =  " << A << " B =  " << B << " C =  " << C <<" D =  " << D << endl;
			}
		}
	}
	//cout << "////////////////////////////////////////////////////////////////////////////////// " << endl;
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	Done();
}


Standard_Boolean McCadCSGTool_Cutter::IsDone() const
{
	return myIsDone;
}


void McCadCSGTool_Cutter::Done()
{
	myIsDone = Standard_True;
}


void McCadCSGTool_Cutter::NotDone()
{
	myIsDone = Standard_False;
}


Handle(TopTools_HSequenceOfShape) McCadCSGTool_Cutter::Solids() const
{
	return myResultSolids;
}


void McCadCSGTool_Cutter::SetLastCut(const Standard_Boolean isLast)
{
	myLastCut = isLast;
}


Standard_Boolean McCadCSGTool_Cutter::IsLastCut() const
{
	return myLastCut;
}
