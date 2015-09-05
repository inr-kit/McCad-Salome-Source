#include <McCadCSGUtil_SolidBuilder.ixx>

#include <stack>

#include <McCadCSGGeom_Surface.hxx>
#include <McCadGTOOL.hxx>
#include <McCadCSGGeom_DataMapOfIntegerSurface.hxx>
#include <McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface.hxx>

#include <McCadCSGBuild_SolidFromCSG.hxx>

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax3.hxx>
#include <gp_Ax2.hxx>
#include <gp_Trsf.hxx>
#include <gp_Torus.hxx>
#include <gp_Sphere.hxx>

#include <TColgp_HSequenceOfPnt.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ElementarySurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Surface.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfIntegerShape.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRep_Builder.hxx>

#include <BRepCheck_Shell.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Face.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <McCadCSGGeom_Transformation.hxx>

#include <STEPControl_Writer.hxx>
#include <gp_Cone.hxx>
//qiu
#include <cmath>

using namespace std;


TopoDS_Shape BigShape = BRepPrimAPI_MakeBox(gp_Pnt(-1.e5,-1.e5,-1.e5),gp_Pnt(1.e5,1.e5,1.e5)).Shape();
//TopoDS_Shape BigShape = BRepPrimAPI_MakeBox(gp_Pnt(-1000,-1000,-1000),gp_Pnt(1000,1000,1000)).Shape();

void Prepare()
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	//cout << "Boolean operation support for  started!"<< endl;
}

McCadCSGUtil_SolidBuilder::McCadCSGUtil_SolidBuilder()
{

}

McCadCSGUtil_SolidBuilder::McCadCSGUtil_SolidBuilder(const Handle(McCadMcRead_Cell)& theCell)
{
	Init(theCell);
}

void McCadCSGUtil_SolidBuilder::Init(const Handle(McCadMcRead_Cell)& theCell)
{
	NotDone();
	myCell = theCell;
	myType = McCadCSGBuild_Other;
	MakeSolid();
}

Standard_Boolean McCadCSGUtil_SolidBuilder::IsDone() const
{
	return myIsDone;
}

void McCadCSGUtil_SolidBuilder::Done()
{
	myIsDone = Standard_True;
}

void McCadCSGUtil_SolidBuilder::NotDone()
{
	myIsDone = Standard_False;
}

void McCadCSGUtil_SolidBuilder::MakeSolid()
{
	cout << "Making a solid\n";
	/////////////////////////////////////////////////////////////////////////////////
	// For a single product term we use the old evaluator:
	// Handle(TColStd_HSequenceOfAsciiString) theInFixCSG =  myCell->GetInFixCSG();
	//  cout << endl;
	// cout << myCell->GetCellNumber() << ".  theInFix:   ";
	//  for(int i=1; i<= theInFixCSG->Length();i++)
	//  {
	//
	//   cout  << (theInFixCSG->Value(i)).ToCString();
	// }
	// cout << endl;
	/////////////////////////////////////////////////////////////////////////////////
	Handle(TColStd_HSequenceOfAsciiString) thePostFixCSG = myCell->GetPostFixCSG();
	//Handle(TColStd_HSequenceOfAsciiString) the1CSG = myCell->GetCSG();
	McCadCSGGeom_DataMapOfIntegerSurface surfMap;
	McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface surfIter = myCell->GetSurface();

	for (; surfIter.More(); surfIter.Next())
		surfMap.Bind(surfIter.Key(), surfIter.Value());
	///////////////////////////////////////////////////////////////////////////////////////////////
/*	if (surfMap.Extent() > 1) //prepare
	{
		surfIter.Initialize(surfMap);
		int iQCount = 0;
		for (; surfIter.More(); surfIter.Next())
		{
			Handle(McCadCSGGeom_Surface) theCSGSurf = surfIter.Value();
			Handle(Geom_Surface) theSurf = theCSGSurf->CasSurf();
			GeomAdaptor_Surface theAdaptSurf(theSurf);
			if (theAdaptSurf.GetType() != GeomAbs_Plane)
				iQCount++;
		}
		if (iQCount > 0)
		{
			//cout << "Total number of quadric surface in the Cell = " << iQCount	<< endl;
			//cout << "Boolean operation support in  preparation .... " << endl;
			Prepare();
		}
	}
	else
		cout << "### No content in Surf map\n";*/
	///////////////////////////////////////////////////////////////////////////////////////////////
	if (thePostFixCSG->Length() == 0 || surfMap.Extent() == 0)
	{
		cout << "Empty CSG Expression or number of surfaces is zero !! "<< endl;

		NotDone();
		return;
	}
	cout << "Processing cell number: " << myCell->GetCellNumber() << "  started!"<< endl;
	cout << "======================== "<< endl;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	TCollection_AsciiString tmpStr;
	for (int i=1; i<= thePostFixCSG->Length(); i++)
	{
		tmpStr.InsertAfter(tmpStr.Length(), thePostFixCSG->Value(i));
		tmpStr.InsertAfter(tmpStr.Length(), " ");
	}
	/*
	 * if simple geometry, i.e. no ':' or '#', run McCadCSGBuild_SolidFromCSG and return; else proceed here
	 */

	if (tmpStr.Search(":") < 0 && tmpStr.Search("#") < 0)
	{
		Handle(TColStd_HSequenceOfAsciiString) theCSG = new TColStd_HSequenceOfAsciiString;
		theCSG->Append(" ( ");
		for (int i=1; i<= thePostFixCSG->Length(); i++)
		{
			if ((thePostFixCSG->Value(i)).IsIntegerValue())
				theCSG->Append((thePostFixCSG->Value(i)).IntegerValue());
		}
		theCSG->Append(" ) ");
		myCell->SetCSG(theCSG);

		McCadCSGBuild_SolidFromCSG theBuilder(myCell);

		if(theBuilder.IsDone())
		{
			myShape = theBuilder.Fused();

			if(myCell->IsTRCL()) // set transformation to current object
			{
				cout << myCell->GetTRCLNumber() << " HAS TRAFO : " << myCell->GetTRCLNumber() << endl;
				gp_Trsf theTrsf = myCell->GetTrsf()->GetTrsf();

				/*for(int i=1; i<=3; i++)
				{
					for(int j=1; j<=4; j++)
						cout << theTrsf.Value(i,j) << " ";
					cout << endl;
				}*/

				TopLoc_Location newLoc(theTrsf);
				myShape.Location(newLoc);
			}

			cout << "#Boolean evaluation completed for cell number: " << myCell->GetCellNumber() << endl;
			Done();
			cout << "==================================================== \n"<< endl;
			return;
		}
	}
	//cout << "}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n";


	cout << "postFixCSG : ";
	for(Standard_Integer k=1; k<=thePostFixCSG->Length(); k++)
		cout << (thePostFixCSG->Value(k)).ToCString() <<  " ";
	cout << endl;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//cout << "ThePostFix befor halfspace construction : " << thePostFixCSG->Length() << endl;
	for(int i=1; i<= thePostFixCSG->Length(); i++)
	{
		if((thePostFixCSG->Value(i)).IsIntegerValue()) //for all integer values, build half spaces and bind them to myHSolidMap
		{
			Standard_Integer surfNo = (thePostFixCSG->Value(i)).IntegerValue();

			if(myHSolidMap.IsBound(surfNo))//half space allready been constructed
				continue;

			Standard_Integer key = abs(surfNo);

			if(!surfMap.IsBound(key))
			{
				NotDone();
				return;
			}

			Handle(McCadCSGGeom_Surface) aCSGSurf = surfMap(key);

			Handle(Geom_Surface) theSurf = aCSGSurf->CasSurf();
			GeomAdaptor_Surface theAdaptSurf(theSurf);
			gp_Cone theCone;
		/*	switch(theAdaptSurf.GetType())
			{
				case GeomAbs_Plane:
					cout << "Plane\n";
					break;
				case GeomAbs_Cylinder:
					cout << "Cylinder\n";
					break;
				case GeomAbs_Torus:
					cout << "Torus\n";
					break;
				case GeomAbs_Cone:
					cout << "Cone\n";
					Standard_Real A1, A2, A3, B1, B2, B3, C1, C2, C3, D;
					theCone = theAdaptSurf.Cone();
					theCone.Coefficients(A1, A2, A3, B1, B2, B3, C1, C2, C3, D);
					cout << "Coefficients : " << A1 << " " << A2 << " " << A3 << " " << B1 << " " << B2 << " " << B3 << " " << C1 << " " << C2 << " " << C3 << " " << D << endl;
					break;
				case GeomAbs_Sphere:
					cout << "Sphere\n";
					break;
				default:
					cout << "DEFAULT\n";
			}*/
			TopoDS_Shape HSol = MakeHalfSpace(aCSGSurf,surfNo);

			if (HSol.IsNull())
			{
				cout << "A null  half space computed!!" << endl;
				return;
			}
			else
				myHSolidMap.Bind(surfNo,HSol);
				//MYMOD myHSolidMap.Bind(key,HSol);//!!!!!!!!!!! NOT KEY; BUT SURFNO !!!!!!!!!!!!!!!!!!
		}
	}
	////////////////////////////////////////////////////////////////////////////////////
	//we evaluate the postfix csg now!!
	TopTools_DataMapOfIntegerShape aResultMap;
	stack<TCollection_AsciiString> theResult;
	int aNewKey = 1000000;
	//cout << "McCadCSGUtil_SolidBuilder.cxx :: " << thePostFixCSG->Length() << "  map size: " << myHSolidMap.Extent() << endl;

	for ( int it = 1; it <= thePostFixCSG->Length(); it++ )
	{
		TCollection_AsciiString aTerm = thePostFixCSG->Value(it);
		if(aTerm.IsIntegerValue())
		{ // an operand
			Standard_Integer aNum = aTerm.IntegerValue();
			Standard_Integer aKey = abs(aNum);

			theResult.push(aNum);
			if(!aResultMap.IsBound(aNum))
			{
				aResultMap.Bind(aNum,myHSolidMap(aNum));
			}

			if(thePostFixCSG->Length()==1)
				aNewKey = aKey; //in case of a single closed surface such as sphere
		}
		else if(aTerm == "#")
		{
			// note that this is an unary operator
			TCollection_AsciiString anItem = theResult.top();
			Standard_Integer aNum = anItem.IntegerValue();
			TopoDS_Shape tmpShape1 = aResultMap(aNum);

			if (tmpShape1.IsNull())
				cout << " Null solid computed !!! " << endl;

			theResult.pop();

			TopoDS_Shape resultShape = Reverse(tmpShape1);
			if(CheckShape(resultShape))
			{
				aNewKey++;
				aResultMap.Bind(aNewKey,resultShape);
				TCollection_AsciiString aToken(aNewKey);
				theResult.push(aToken);
			}
			else
			{
				aNewKey = -1;
				cout << "Boolean evaluation failed for this cell!" << endl;
				break;
			}
		}
		else if(aTerm == "|")
		{
		//	cout << "|\n";
			TCollection_AsciiString anItem = theResult.top();
			Standard_Integer aNum = anItem.IntegerValue();
			Standard_Integer aKey = abs(aNum);
			TopoDS_Shape tmpShape1 = aResultMap(aNum);

			if (tmpShape1.IsNull())
				cout << " Null solid computed !!! " << endl;

			theResult.pop();
			if(theResult.size()==0)
				continue;

			anItem = theResult.top();
			aNum = anItem.IntegerValue();
			aKey = abs(aNum);
			TopoDS_Shape tmpShape2 = aResultMap(aNum);
			if (tmpShape2.IsNull())
				cout << " Null solid computed !!! " << endl;

			theResult.pop();

			TopoDS_Shape resultShape = Common(tmpShape1, tmpShape2);
			if (resultShape.IsNull())
				cout << " resultShape::  Null solid computed !!! " << endl;


			if(CheckShape(resultShape))
			{
				aNewKey++;
				aResultMap.Bind(aNewKey,resultShape);
				TCollection_AsciiString aToken(aNewKey);
				theResult.push(aToken);
			}
			else
			{
				aNewKey = -1;
				cout << "Boolean evaluation failed for this cell!!" << endl;
				break;
			}
		}
		else if(aTerm == ":")
		{
		//	cout << ":\n";
			TCollection_AsciiString anItem = theResult.top();
			Standard_Integer aNum = anItem.IntegerValue();
			Standard_Integer aKey = abs(aNum);
			TopoDS_Shape tmpShape1 = aResultMap(aNum);

			theResult.pop();

			anItem = theResult.top();
			aNum = anItem.IntegerValue();
			aKey = abs(aNum);
			TopoDS_Shape tmpShape2 = aResultMap(aNum);

			theResult.pop();

			TopoDS_Shape resultShape = Fuse(tmpShape1, tmpShape2);

			if(CheckShape(resultShape))
			{
				aNewKey++;
				aResultMap.Bind(aNewKey,resultShape);
				TCollection_AsciiString aToken(aNewKey);
				theResult.push(aToken);
			}
			else
			{
				aNewKey = -1;
				cout << "Boolean evaluation failed for this cell!!!" << endl;
				break;
			}
		}
		else
		{
			cout << "Unkown term type in the Boolean expression for this cell!" << endl;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////
	// here we are with the result
	if(aNewKey == -1) // MYTEST
	{
		cout << "Boolean evaluation failed for cell number:  " << myCell->GetCellNumber() << endl;
		NotDone();
	}
	else
	{
		cout << "Boolean evaluation completed for cell number: " << myCell->GetCellNumber() << endl;

		myShape = aResultMap(aNewKey);

		if(myCell->IsTRCL()) // set transformation to current object
		{
			//cout << "HAS TRAFO 1\n";
			//cout << "myCell Trafo Nr " << myCell->GetTRCLNumber() << endl;
			if(!myCell->GetTRCLNumber()==0)
			{
				gp_Trsf theTrsf = myCell->GetTrsf()->GetTrsf();
				TopLoc_Location newLoc(theTrsf);
				myShape.Move(newLoc);
			}
		}
		//else
			//cout << "NO TRafo for cell no: "<<myCell->GetCellNumber() << endl;

		Done();
	}
	cout << "___ DONE\n";
	cout << "==================================================== "<< endl;
}


TopoDS_Shape McCadCSGUtil_SolidBuilder::MakeHalfSpace(const Handle(McCadCSGGeom_Surface)& theCSGSurf, const Standard_Integer sNum)
{
	Standard_Integer surfNo = sNum;

	Handle(Geom_Surface) theSurf = theCSGSurf->CasSurf();
	GeomAdaptor_Surface theAdaptSurf(theSurf);
#ifdef OCC650
	TopoDS_Face theFace = BRepBuilderAPI_MakeFace(theSurf);
#else
	TopoDS_Face theFace = BRepBuilderAPI_MakeFace(theSurf, 1.0e-7);
#endif
	//Standard_Boolean negAxis = Standard_False;
	Handle(Geom_ElementarySurface) ES = Handle(Geom_ElementarySurface)::DownCast(theSurf);

	gp_Ax3 pos = ES->Position();
	gp_Trsf T;

	if(!pos.Direct())
		cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: LHS\n\n\n";

	//test for negative axis:
//MYMOD 300508
	/*gp_Dir theDirect = pos.Direction();
	gp_Dir XDirect = pos.XDirection();
	gp_Dir YDirect = pos.YDirection();
	gp_Pnt theLoc = pos.Location();

	if (theDirect.X() < 0 || theDirect.Y() < 0 || theDirect.Z() <= 0)
	{
		negAxis = Standard_True;
		cout << "NEGATIV AXIS\n";
	}*/
//MYMOD #

	T.SetTransformation(pos);
/*
	if (negAxis)
		cout << " negative axis " << endl;*/

	Standard_Real U=1, V=1;
	gp_Pnt thePnt(0,0,0);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	thePnt = theSurf->Value(U,V);

//qiu	
#ifdef WIN32
	if(_isnan(thePnt.X()) || _isnan(thePnt.Y()) || _isnan(thePnt.Z()))
#else 
	if(isnan(thePnt.X()) || isnan(thePnt.Y()) || isnan(thePnt.Z()))
#endif

	{
		cout << "Sample point by projection ...";
		gp_Pnt aPnt(1,1,1);
		GeomAPI_ProjectPointOnSurf thePntPro(aPnt,theSurf);
		if(thePntPro.IsDone() && thePntPro.NbPoints() > 0)
		{
			thePnt = thePntPro.NearestPoint();
			cout << " ... finished" << endl;
		}
		else
		{
			cout << " ... failed" << endl;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*cout <<" XDir:       X = " << XDirect.X() << " Y = " << XDirect.Y() << " Z = " << XDirect.Z() <<endl;
	cout <<" YDir:       X = " << YDirect.X() << " Y = " << YDirect.Y() << " Z = " << YDirect.Z() <<endl;
	cout <<" ZDir:       X = " << theDirect.X()<< " Y = " << theDirect.Y()<< " Z = " << theDirect.Z() <<endl;
	cout <<" Location:   X = " << theLoc.X() << "   Y = " << theLoc.Y() << "   Z = " << theLoc.Z() <<endl;*/
	////////////////////////////////////////////////////////////////////////////////////////////////////

	//cout << " Original Point Value    X = "	<< thePnt.X() << "  Y = " << thePnt.Y() << "  Z = " << thePnt.Z() << endl;

	Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt;

	for ( int j=1; j <=3; j++)
	{
		// perturbation coordinatewise
		Standard_Real r = thePnt.Coord(j);
		gp_Pnt tmpPnt = thePnt;
		tmpPnt.SetCoord(j,r+0.1);
		thePntSeq->Append(tmpPnt);
		tmpPnt.SetCoord(j,r-0.1);
		thePntSeq->Append(tmpPnt);
	}
	////////////////////////////////////////////////////////////////////////////////////////


	gp_Pnt samPnt;
	Standard_Boolean foundSamPnt(Standard_False);

	if(surfNo < 0)
	{
		//     cout << " TopAbs_FORWARD - Evaluation  = " ;
		/*if(negAxis)
		{
			for ( int j=1; j<=thePntSeq->Length(); j++)
			{
				Standard_Real aVal = McCadGTOOL::Evaluate(theAdaptSurf,thePntSeq->Value(j));
				if (aVal > 1.e-04 )
				{
					samPnt = thePntSeq->Value(j);
					break;
				}
			}
		}
		else
		{*/
			for ( int j=1; j<=thePntSeq->Length(); j++)
			{
				Standard_Real aVal = McCadGTOOL::Evaluate(theAdaptSurf,thePntSeq->Value(j));
				if (aVal < -1.e-04 )
				{
					foundSamPnt = Standard_True;
					samPnt = thePntSeq->Value(j);
					break;
				}
			}
		//}
	}
	else
	{
		// cout << " TopAbs_REVERSED - Evaluation  = " ;
		/*if(negAxis)
		{
			for ( int j=1; j<=thePntSeq->Length(); j++)
			{
				Standard_Real aVal = McCadGTOOL::Evaluate(theAdaptSurf,thePntSeq->Value(j));
				if (aVal < -1.e-04 )
				{
					samPnt = thePntSeq->Value(j);
					break;
				}
			}
		}
		else
		{*/
			for ( int j=1; j<=thePntSeq->Length(); j++)
			{
				Standard_Real aVal = McCadGTOOL::Evaluate(theAdaptSurf,thePntSeq->Value(j));
				if (aVal > 1.e-04 )
				{
					foundSamPnt = Standard_True;
					samPnt = thePntSeq->Value(j);
					break;
				}
			}
		//}
	}

	//////////////////////////////////////////////////////////////////////////////
	//  Standard_Real  aVal =  McCadGTOOL::Evaluate(theAdaptSurf,samPnt);
	// cout << " ===  " << aVal << endl;
	// cout << " evaluated at    X = "
	//	 << samPnt.X() << "  Y = " << samPnt.Y() << "  Z = " << samPnt.Z() << endl;
	//////////////////////////////////////////////////////////////////////////////
	TopoDS_Shape aResultShape;
	TopoDS_Shape HSol = BRepPrimAPI_MakeHalfSpace(theFace,samPnt).Solid();

	if (surfNo < 0 && (theAdaptSurf.GetType()== GeomAbs_Sphere))
	{
		gp_Sphere S = theAdaptSurf.Sphere();
		gp_Ax3 theAxis = S.Position();
		Standard_Real theRadius = S.Radius();
		gp_Ax2 the2Axis = theAxis.Ax2();
		aResultShape = BRepPrimAPI_MakeSphere(the2Axis,theRadius).Solid();
	}
	else if (surfNo < 0 && (theAdaptSurf.GetType()== GeomAbs_Torus))
	{
		gp_Torus T = theAdaptSurf.Torus();
		gp_Ax3 theAxis = T.Position();
		Standard_Real theMajorRadius = T.MajorRadius();
		Standard_Real theMinorRadius = T.MinorRadius();
		gp_Ax2 the2Axis = theAxis.Ax2();
		aResultShape = BRepPrimAPI_MakeTorus(the2Axis,theMajorRadius,theMinorRadius).Solid();
	}
	else
	{
		aResultShape = Common(HSol,BigShape);
	}

	if(!CheckShape(aResultShape))
	{
		if(!foundSamPnt)
			cout << "Didn't find a useful sample point\n";

		STEPControl_Writer stpWrtr;
		stpWrtr.Transfer(BigShape, STEPControl_AsIs);
		cout << " 1" << endl;
		stpWrtr.Transfer(aResultShape, STEPControl_AsIs);
		cout << " 2" << endl;
		stpWrtr.Write("aTest.stp");

		cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: Computing halfspace failed !!!" << endl;
	}

	return aResultShape;
}


TopoDS_Shape McCadCSGUtil_SolidBuilder::Reverse(const TopoDS_Shape& theShape)
{

	TopoDS_Shape aResultShape;
	Standard_Boolean bOp = Standard_False;
	try
	{
		BRepAlgoAPI_Cut aCutOp(BigShape,theShape);

		if(aCutOp.IsDone())
		{
			bOp = Standard_True;
			aResultShape = aCutOp.Shape();
		}
		else
		{
			bOp = Standard_False;
			cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: Boolean Operation on a halfspace failed !!!" << endl;
		}

	}
	catch(...)
	{
		bOp = Standard_False;
		cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: Boolean Operation on a halfspace failed !!!" << endl;
	}

	return aResultShape;
}


TopoDS_Shape McCadCSGUtil_SolidBuilder::Common(const TopoDS_Shape& theShape1, const TopoDS_Shape& theShape2)
{
	TopoDS_Shape aResultShape;
	Standard_Boolean bOp = Standard_False;

	TopoDS_Shape shp1, shp2;
	shp1 = theShape1;
	shp2 = theShape2;
	Standard_Boolean isSwitched(Standard_False);

//MYTEST :: Try to switch shapes if common operation fails
ShapesSwitched:

	try
	{
		BRepAlgoAPI_Common aCommonOp(theShape1, theShape2);

		if(aCommonOp.IsDone())
		{
			bOp= Standard_True;
			aResultShape = aCommonOp.Shape();
		}
		else
		{
			if(!isSwitched)
			{
				shp1 = theShape2;
				shp2 = theShape1;
				isSwitched = Standard_True;
				goto ShapesSwitched;
			}

			bOp= Standard_False;
			cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: Boolean Operation on a halfspace failed !!!" << endl;
		}

	}
	catch(...)
	{
		bOp= Standard_False;
		cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: Boolean Operation on a halfspace failed !!! - " << endl;
	}
	return aResultShape;
}


TopoDS_Shape McCadCSGUtil_SolidBuilder::Fuse(const TopoDS_Shape& theShape1,	const TopoDS_Shape& theShape2)
{
	TopoDS_Shape aResultShape;
	Standard_Boolean bOp = Standard_False;
	try
	{

		BRepAlgoAPI_Fuse aFuseOp(theShape1, theShape2);

		if(aFuseOp.IsDone())
		{
			bOp= Standard_True;
			aResultShape = aFuseOp.Shape();
		}
		else
		{
			bOp= Standard_False;
			cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: Boolean Operation on a halfspace failed !!!" << endl;
		}

	}
	catch(...)
	{
		bOp= Standard_False;
		cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: Boolean Operation on a halfspace failed !!!" << endl;
	}
	return aResultShape;

}

Standard_Boolean McCadCSGUtil_SolidBuilder::CheckShape(	const TopoDS_Shape& theShape) const
{
	const TopoDS_Shape& aShape = theShape;
	Standard_Boolean bOp = Standard_True;
	if (theShape.IsNull())
	{
		bOp = Standard_False;
		cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: Null solid computed !!! " << endl;
		return bOp;
	}
	BRepCheck_Analyzer BA(aShape, Standard_False);
	int sCount=0;
	for (TopExp_Explorer ex(aShape, TopAbs_SOLID); ex.More(); ex.Next())
	{
		sCount++;
		const TopoDS_Solid& solid = TopoDS::Solid(ex.Current());
		if (solid.IsNull())
		{
			cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: BOP for solid failed: NULL SOLID computed !!" << endl;
			bOp = Standard_False;
			break;
		}
		for (TopExp_Explorer exs(solid, TopAbs_SHELL); exs.More(); exs.Next())
		{
			const TopoDS_Shell& sh = TopoDS::Shell(exs.Current());
			BA.Init(sh, Standard_False);
			/*Handle(BRepCheck_Result) RES = BA.Result(sh);
			BRepCheck_ListOfStatus StatusList;
			StatusList = RES->Status();
			BRepCheck_ListIteratorOfListOfStatus iter;*/
			//cout << endl;
			/*for (iter.Initialize(StatusList); iter.More(); iter.Next())
			{
				BRepCheck::Print(iter.Value(), cout);
			}*/
			//cout << endl;
			if(!BA.IsValid())
			{
				cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: BRepCheck_Analyzer for shell failed !" << endl;
				bOp = Standard_False;
				break;
			}
		}
	}
	BA.Init(aShape, Standard_False);
	if (!BA.IsValid())
	{
		cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: BRepCheck_Analyzer for solid failed !!!" << endl;
		bOp = Standard_False;
	}
	if (sCount==0)
	{
		bOp = Standard_False; // if check is run on compound containing no solid
		cout << "_#_McCadCSGUtil_SolidBuilder.cxx :: Compound containing no solid computed !!" << endl;
	}

	return bOp;
}


Handle(TopTools_HSequenceOfShape) McCadCSGUtil_SolidBuilder::HalfSpaces() const
{
	TopTools_DataMapIteratorOfDataMapOfIntegerShape Iter(myHSolidMap);
	Handle(TopTools_HSequenceOfShape) aShapeSeq = new TopTools_HSequenceOfShape;
	for (;Iter.More(); Iter.Next()) aShapeSeq->Append(Iter.Value());
	return aShapeSeq;
}


TopoDS_Shape McCadCSGUtil_SolidBuilder::Result() const
{
	return myShape;
}


Standard_Integer McCadCSGUtil_SolidBuilder::NbHSolids() const
{
	return myHSolidMap.Extent();
}


McCadCSGBuild_SolidType McCadCSGUtil_SolidBuilder::ResultType() const
{
	return McCadCSGBuild_Other;
}

