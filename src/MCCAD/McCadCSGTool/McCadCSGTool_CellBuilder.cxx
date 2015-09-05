#include <McCadCSGTool_CellBuilder.ixx>
#include <McCadCSGTool.hxx>
#include <McCadCSGGeom_DataMapOfIntegerSurface.hxx>
#include <McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface.hxx>
#include <McCadCSGGeom_Surface.hxx>
#include <McCadTDS_ExtSolid.hxx>

#include <McCadCSGGeom_Plane.hxx>
#include <McCadCSGGeom_Cylinder.hxx>
#include <McCadCSGGeom_Cone.hxx>
#include <McCadCSGGeom_Sphere.hxx>
#include <McCadCSGGeom_Torus.hxx>
#include <McCadCSGGeom_GQ.hxx>
#include <McCadCSGGeom_SurfaceOfRevolution.hxx>

#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

#include <gp_Vec.hxx>
#include <gp_Ax3.hxx>
#include <gp_XYZ.hxx>
#include <gp_Trsf.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Torus.hxx>
#include <gp_Sphere.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ElementarySurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Circle.hxx>
#include <IntTools_FaceFace.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_Curve.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <TColgp_HSequenceOfPnt.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <GC_MakePlane.hxx>
#include <GeomInt_IntSS.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAPI_IntSS.hxx>
#include <McCadGTOOL.hxx>
#include <Geom_Plane.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>

#include <math_SVD.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>


#include <McCadCSGAdapt_SolidAnalyser.hxx>
#include <TCollection_AsciiString.hxx>
// #include <McCadMcVoid.hxx>

McCadCSGTool_CellBuilder::McCadCSGTool_CellBuilder()
{
	Init();
}


McCadCSGTool_CellBuilder::McCadCSGTool_CellBuilder(const Handle(McCadTDS_HSequenceOfExtSolid)& theExtSolidSeq)
{
	Init();
	Init(theExtSolidSeq);
}


McCadCSGTool_CellBuilder::McCadCSGTool_CellBuilder(const Handle(McCadTDS_HSequenceOfExtSolid)& theExtSolidSeq, const Standard_Integer theMatNumber)
{
	Init();
	myMatNumber = theMatNumber;
	myIsVoid = Standard_False;
	Init(theExtSolidSeq);
}

McCadCSGTool_CellBuilder::McCadCSGTool_CellBuilder(const Handle(McCadTDS_HSequenceOfExtSolid)& theExtSolidSeq, const Standard_Integer theMatNumber, const Standard_Real theDensity)
{
	Init();
	myMatNumber = theMatNumber;
	myDensity = theDensity;
	myIsVoid = Standard_False;
	myMakeOuterVoid = Standard_False;

	Init(theExtSolidSeq);
}


void McCadCSGTool_CellBuilder::SetMatNumber(const Standard_Integer theMatNumber)
{
	myMatNumber = theMatNumber;
	myIsVoid = Standard_False;
}


void McCadCSGTool_CellBuilder::MakeOuterVoid(Standard_Boolean state)
{
	myMakeOuterVoid = state;
}


void McCadCSGTool_CellBuilder::SetDensity(const Standard_Real theDensity)
{
	myDensity = theDensity;
	myIsVoid = Standard_False;
}

void McCadCSGTool_CellBuilder::Init()
{
	myUnits = McCadCSGGeom_CM;
	myMatNumber = 0;
	myDensity = 0.0;
	myIsVoid = Standard_True;
}

void McCadCSGTool_CellBuilder::Init(const Handle(McCadTDS_HSequenceOfExtSolid)& theExtSolidSeq)
{
	NotDone();
	myExtSolidSeq = theExtSolidSeq;

	myCell = new McCadCSGGeom_Cell(0, myMatNumber, myDensity);

	if(myIsVoid)
		myCell->SetVoid(Standard_True);
	if(myMakeOuterVoid)
		myCell->SetOuterVoid(Standard_True);
	else
		myCell->SetOuterVoid(Standard_False);

	myCell->SetTrsfCounter(0);
	myCell->SetSurfCounter(0);

	McCadCSGGeom_DataMapOfIntegerSurface surfMap;
	TCollection_AsciiString theComment;
	Handle(TColStd_HSequenceOfAsciiString) aCSG = new TColStd_HSequenceOfAsciiString(); //aCSG, describes geometry of one cell

	if(myExtSolidSeq->Length() == 0)
	{
		Done();
		return;
	}

	Standard_Real cellVolume(0.0);

	if(myExtSolidSeq->Length() > 1) // for outer void
		aCSG->Append(" (");

	for (Standard_Integer i=1; i<= myExtSolidSeq->Length(); i++)//For all extended solids ...
	{
		if(myExtSolidSeq->Length() > 1 || myMakeOuterVoid)
			aCSG->Append("(");
		Handle(McCadTDS_ExtSolid) anExtSol = myExtSolidSeq->Value(i);


		theComment = anExtSol->GetComment();
		TopoDS_Solid aSol = anExtSol->GetSolid();

		// calculate volume and add to cellVolume
                GProp_GProps GP;
                BRepGProp::VolumeProperties(aSol, GP);
                cellVolume += GP.Mass();

		Handle(TopTools_HSequenceOfShape) theFaceSeq = new TopTools_HSequenceOfShape;

		for (TopExp_Explorer ex(aSol,TopAbs_FACE); ex.More(); ex.Next()) //append all faces of current solid to theFaceSeq
			theFaceSeq->Append(ex.Current());

		if(anExtSol->HaveExt())
		{
			theFaceSeq->Append(anExtSol->GetFaces());
		}
		//cout << "Number of faces including extensions = " << theFaceSeq->Length() << endl;
		/////////////////////////////////////////////////////////////////////////////
		//
		for (int j = 1; j<= theFaceSeq->Length(); j++)//For all faces of current solid put planes in right orientation
		{
			TopLoc_Location l;
			TopoDS_Face F = TopoDS::Face(theFaceSeq->Value(j));
			TopAbs_Orientation orient = F.Orientation();
			Handle(Geom_Surface) S = BRep_Tool::Surface(F, l);

			GeomAdaptor_Surface AS(S);
			if (AS.GetType() == GeomAbs_Plane)
			{
				gp_Pln aP1 = AS.Plane();
				gp_Ax3 locAxis = aP1.Position();
				gp_Dir locDir = locAxis.Direction();
				gp_Ax3 glAxis (locAxis.Location(),gp_Vec(fabs(locDir.X()),fabs(locDir.Y()),fabs(locDir.Z())));
				gp_Dir glDir = glAxis.Direction();

				///////////////////////////////             ////////////////////////
                                Standard_Real A(0.0),B(0.0),C(0.0),D(0.0);
                                if (locAxis.Direct()) // right handed coordinate system
				{
					A = locDir.X();
					B = locDir.Y();
					C = locDir.Z();
				}
                                else // left handed coordinate system - correction neccessary
				{
					A = -locDir.X();
					B = -locDir.Y();
					C = -locDir.Z();
				}
				const gp_Pnt& P = locAxis.Location();
				D =-(A * P.X() + B * P.Y() + C * P.Z());

                                // using 10^-7 instead of 0, because a value like -10^-18 wich is effectivly 0
                                // would be interpreted as a negative non-zero value
                                const double negZero(-1.e-7);
                                if ( (D > 0 && (locDir.X() < negZero || locDir.Y() < negZero || locDir.Z() < negZero )) ||
                                     (locDir.X() <= 0. && locDir.Y() <= 0. && locDir.Z() <= 0. ) )
				{
                                        gp_Ax3 glAxis (locAxis.Location(),gp_Vec(-1*locDir.X(),-1*locDir.Y(),-1*locDir.Z()));
					gp_Pln glPl(glAxis);
					TopoDS_Face glF = BRepBuilderAPI_MakeFace(glPl);

                                        if(orient == TopAbs_FORWARD)
                                            glF.Orientation(TopAbs_REVERSED);
					if(orient == TopAbs_REVERSED)
                                            glF.Orientation(TopAbs_FORWARD);

                                        theFaceSeq->SetValue(j,glF);
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////
		// here we delete redundant faces first
		Handle(TopTools_HSequenceOfShape) LPlan = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) LCyl = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) LCon = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) LSphere = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) LTor = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) LGQ = new TopTools_HSequenceOfShape();
		Handle(TopTools_HSequenceOfShape) LSOR = new TopTools_HSequenceOfShape();

		//gp_Pnt backupPnt1, backupPnt2;

		for (int j = 1; j<= theFaceSeq->Length(); j++) //Delete redundant faces
		{
			TopLoc_Location l;
			Handle(Geom_Surface) S;
			TopoDS_Face F = TopoDS::Face(theFaceSeq->Value(j));
			S = BRep_Tool::Surface(F, l);
			if (!S.IsNull())
			{
				GeomAdaptor_Surface AS(S);
				switch (AS.GetType())
				{
					case GeomAbs_Plane:
					{
						LPlan->Append(theFaceSeq->Value(j));
						break;
					}
					case GeomAbs_Cylinder:
					{
						LCyl->Append(theFaceSeq->Value(j));
						break;
					}
					case GeomAbs_Cone:
					{
						LCon->Append(theFaceSeq->Value(j));
						break;
					}
					case GeomAbs_Sphere:
					{
						LSphere->Append(theFaceSeq->Value(j));
						break;
					}
					case GeomAbs_Torus:
					{
						LTor->Append(theFaceSeq->Value(j));
						break;
					}
					case GeomAbs_BezierSurface:
					{
						cout << "_#_McCadCSGTool_CellBuilder.cxx :: BezierSurface can not be handled!" << endl;
						break;
					}
					case GeomAbs_BSplineSurface:
					{

						cout << "!!! BSpline Surface will be treated as General Quadric !!!\n";

						LGQ->Append(theFaceSeq->Value(j));
						break;
					}
					case GeomAbs_SurfaceOfRevolution:
					{
						//cout << "_#_McCadCSGTool_CellBuilder.cxx :: SurfaceOfRevolution can not be handled!" << endl;
						Handle(Geom_SurfaceOfRevolution) rev = Handle(Geom_SurfaceOfRevolution)::DownCast(S);
						Handle(Geom_Curve) basisCurve = rev->BasisCurve();
						GeomAdaptor_Curve gaCurve(basisCurve);

						//might be an elliptical torus
						if(gaCurve.GetType() == GeomAbs_Ellipse || gaCurve.GetType() == GeomAbs_Circle )
						{
							gp_Ax1 revAx = rev->Axis();
							gp_Dir revDir = revAx.Direction();
							gp_Vec revVec(revDir.X(), revDir.Y(), revDir.Z());

							// test if axis of revolution is perpendicular to two of the axis of the ellipse/circle
							if(gaCurve.GetType() == GeomAbs_Ellipse)
							{
								Handle(Geom_Ellipse) e = Handle(Geom_Ellipse)::DownCast(basisCurve);
								gp_Ax1 normAx = e->Axis();
								gp_Dir normDir = normAx.Direction();
								gp_Vec normVec(normDir.X(), normDir.Y(), normDir.Z());

								if(Abs(revVec*normVec) > 1.0e-7)
								{
									cout << "_#_McCadCSGGeom_CellBuilder.cxx :: Ellipse not perpendicular to rotation Axis !!!\n" << endl;
									break;
								}

								LSOR->Append(theFaceSeq->Value(j));

							}
							else
							{
								Handle(Geom_Circle) c = Handle(Geom_Circle)::DownCast(basisCurve);
								gp_Ax1 normAx = c->Axis();
								gp_Dir normDir = normAx.Direction();
								gp_Vec normVec(normDir.X(), normDir.Y(), normDir.Z());

								if(Abs(revVec*normVec) > 1.0e-7)
								{
									cout << "_#_McCadCSGGeom_CellBuilder.cxx :: Ellipse not perpendicular to rotation Axis !!!\n" << endl;
									break;
								}

								LSOR->Append(theFaceSeq->Value(j));
							}
						}
						else
							cout << "_#_McCadCSGGeom_CellBuilder.cxx :: Not supported kind of SurfaceOfRevolution !!!\n" << endl;

 						break;
					}
					case GeomAbs_SurfaceOfExtrusion:
					{
						cout << "_#_McCadCSGTool_CellBuilder.cxx :: SurfaceOfExtrusion can not be handled!" << endl;
						break;
					}
					case GeomAbs_OffsetSurface:
					{
						cout << "_#_McCadCSGTool_CellBuilder.cxx :: OffsetSurface can not be handled!" << endl;
						break;
					}
					case GeomAbs_OtherSurface:
					{
						cout << "_#_McCadCSGTool_CellBuilder.cxx :: Unknown type of surface!" << endl;
						break;
					}
				}

			}
			else
			{
				cout << "_#_McCadCSGTool_CellBuilder.cxx :: Nonanalytic surface " << endl;
			}
		}


		//cout << "\nnumber of Planes   : " << LPlan->Length() << endl;
		//cout << "number of cylinders: " << LCyl->Length() << endl;

		//test if two faces are the same and delete redundant
		LTor = McCadCSGTool::DeleteRedTorFace(LTor);
		LSphere= McCadCSGTool::DeleteRedSphFace(LSphere);
		LCon = McCadCSGTool::DeleteRedConFace(LCon);
		LCyl = McCadCSGTool::DeleteRedCylFace(LCyl);
		LPlan = McCadCSGTool::DeleteRedLinFace(LPlan);
		LSOR = McCadCSGTool::DeleteRedSORFace(LSOR);
		//TODO: LGQ = McCadCSGTool::DeleteRedGQ(LGQ);

		theFaceSeq->Clear();
		theFaceSeq->Append(LPlan);
		theFaceSeq->Append(LTor);
		theFaceSeq->Append(LSphere);
		theFaceSeq->Append(LCon);
		theFaceSeq->Append(LCyl);
		theFaceSeq->Append(LGQ);
		theFaceSeq->Append(LSOR);
		LTor->Clear();
		LSphere->Clear();
		LCon->Clear();
		LCyl->Clear();
		LPlan->Clear();
		LGQ->Clear();
		//cout << "Number of irredundant faces = " << theFaceSeq->Length() << endl;
		//Standard_Integer myCnt(0);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		for (Standard_Integer j=1; j<= theFaceSeq->Length(); j++)//Now construct geometry
		{
			TopLoc_Location L;
			TopoDS_Face theFace = TopoDS::Face(theFaceSeq->Value(j));
			TopAbs_Orientation orient = theFace.Orientation();

			BRepAdaptor_Surface BS(theFace,Standard_True);
			Handle(Geom_Surface) theSurface = BRep_Tool::Surface(theFace,L);
			//	GeomAdaptor_Surface theAdapSurface = BS.Surface();
			GeomAdaptor_Surface theAdapSurface(theSurface);
			//  Handle(Geom_Surface) theSurface = theAdapSurface.Surface();

			Handle(Geom_ElementarySurface) ES = Handle(Geom_ElementarySurface)::DownCast(theSurface);
			////////////////////////////////////////////////////////////////////////////////////////////////
			gp_Trsf T;
            if(theAdapSurface.GetType()!=GeomAbs_BSplineSurface && theAdapSurface.GetType()!=GeomAbs_SurfaceOfRevolution)
            {
                gp_Ax3 stdAx3(gp::Origin(),gp::DZ());
                gp_Ax3 pos = ES->Position();
                T.SetTransformation(stdAx3,pos);
                //gp_Dir theDirect = pos.Direction();
                //gp_Dir XDirect = pos.XDirection();
                //gp_Dir YDirect = pos.YDirection();
                //gp_Pnt theLoc = pos.Location();
            }
            //	cout << "The Position : " << theLoc.X() << "  " << theLoc.Y() << "  " << theLoc.Z() << "\n\n";
			Standard_Boolean negAxis = Standard_False;
			//if (theDirect.X() < 0 || theDirect.Y() < 0 || theDirect.Z() < 0)
				//cout << "McCadCSGTool_CellBuilder.cxx :: HAVE NEGAXIS\n\n\n";//negAxis = Standard_True;
			//	if(T.IsNegative())  negAxis = Standard_True;
			//        if(T.VectorialPart().Determinant() < 0.0) negAxis = Standard_True;
			// if (XDirect.Crossed(YDirect).Dot(pos.Direction()) < 0 ) negAxis = Standard_True;
			// if(negAxis) cout <<"_#_McCadCSGTool_CellBuilder.cxx :: %%%%%%%%% Negative transformation %%%%%%%% " << endl;
			//////////////////////////////////////////////////////////////////////////////////////////
			//	cout <<" XDir:       X = " <<  XDirect.X() << " Y = " <<  XDirect.Y() << " Z = " <<  XDirect.Z() <<endl;
			//	cout <<" YDir:       X = " <<  YDirect.X() << " Y = " <<  YDirect.Y() << " Z = " <<  YDirect.Z() <<endl;
			//  cout <<" ZDir:       X = " << theDirect.X()<< " Y = " << theDirect.Y()<< " Z = " <<  theDirect.Z() <<endl;
			//	cout <<" Location:   X = " <<  theLoc.X() << "   Y = " <<  theLoc.Y() << "   Z = " <<  theLoc.Z() <<endl;
			// cout <<" Y    Dir:   X = " <<  YDirect.X() << "   Y = " <<  YDirect.Y() << "   Z = " <<  YDirect.Z() <<endl;
			//////////////////////////////////////////////////////////////////////////////////////////
			// the following is a tmp solution. It will be replaced by an object factory implemented as
			// a singelton.
			Handle(McCadCSGGeom_Surface) aCSGSurf;

			switch (theAdapSurface.GetType())
			{
				case GeomAbs_Plane:
					aCSGSurf = new McCadCSGGeom_Plane;
					break;
				case GeomAbs_Cylinder:
					aCSGSurf = new McCadCSGGeom_Cylinder;
					break;
				case GeomAbs_Cone:
					aCSGSurf = new McCadCSGGeom_Cone;
					break;
				case GeomAbs_Sphere:
					aCSGSurf = new McCadCSGGeom_Sphere;
					break;
				case GeomAbs_Torus:
					aCSGSurf = new McCadCSGGeom_Torus;
					break;
				case GeomAbs_BezierSurface:
				{
					cout << "_#_McCadCSGTool_CellBuilder.cxx :: Bezier Curves are not supported " << endl;
					return;
				}
				case GeomAbs_BSplineSurface:
				{
					math_Vector paramVec(1,9);
					paramVec.Init(0.0);

					if(ComputeGQ(theFace, paramVec))
					{
						cout << "GQ fitting to BSpline-Surface  --  SUCCESSFUL\n";
						aCSGSurf = new McCadCSGGeom_GQ(paramVec.Value(1), paramVec.Value(2), paramVec.Value(3), paramVec.Value(4), paramVec.Value(5),
													   paramVec.Value(6), paramVec.Value(7), paramVec.Value(8), paramVec.Value(9), 1);
						break;
					}
					else
					{
						cout << "_#_McCadCSGTool_CellBuilder.cxx :: GQ fitting to BSpline-Surface failed!!!\n\n";
						//TODO : could be a torus; which cannot be approximated by a general QUADRIC. try to fit against toroidal surface...

						continue;
					}

					break;
				}
				case GeomAbs_SurfaceOfRevolution:
				{
					//cout << "_#_McCadCSGTool_CellBuilder.cxx :: Revolution is not supported " << endl;
					/*
					cout << "SOR : ";
					if(orient == TopAbs_FORWARD)
						cout << "FORWARD" << endl;
					else
						cout << "REVERSE" << endl;

					if(orient == TopAbs_REVERSED)
					{

						Handle(Geom_Surface) theSurf = theAdapSurface.Surface();
						gp_Pnt pnt1, pnt2;
						theSurf->D0(0,0, pnt1);
						theSurf->D0(0,PI, pnt2);

						gp_Pnt midPnt((pnt2.X()+pnt1.X())/2.0, (pnt2.Y()+pnt1.Y())/2.0, (pnt2.Z()+pnt1.Z())/2.0);

						BRepBuilderAPI_MakeShell shellMaker(theSurf, Standard_False);
						TopoDS_Shell theShell = shellMaker.Shell();
						BRepBuilderAPI_MakeSolid solidMaker(theShell);
						TopoDS_Shape theSolid = solidMaker.Shape();

						BRepClass3d_SolidClassifier clssfr(theSolid);
						cout << "1\n";
						clssfr.Perform(midPnt, 1.0e-3); // !!! CRASHES !!! ??? ??? ???
						cout << "2\n";

						if(clssfr.State() == TopAbs_IN)
							negAxis = Standard_True;
					}*/
					if(orient == TopAbs_REVERSED)
						negAxis = Standard_True;

					Handle(Geom_SurfaceOfRevolution) rev = Handle(Geom_SurfaceOfRevolution)::DownCast(theSurface);
					Handle(Geom_Curve) basisCurve = rev->BasisCurve();
					gp_Ax1 revAx = rev->Axis();

					aCSGSurf = new McCadCSGGeom_SurfaceOfRevolution(revAx, basisCurve);
					break;
				}
				case GeomAbs_SurfaceOfExtrusion:
				{
					cout << "_#_McCadCSGTool_CellBuilder.cxx :: Extrusion is not supported" << endl;
					return;
				}
				case GeomAbs_OffsetSurface:
				{
					cout << "_#_McCadCSGTool_CellBuilder.cxx :: Surface is not analytic " << endl;
					return;
				}
				case GeomAbs_OtherSurface:
				{
					cout << "_#_McCadCSGTool_CellBuilder.cxx :: Surface is not analytic " << endl;
					return;
				}
			}


			if(aCSGSurf == 0)
			{
				cout << "_#_McCadCSGTool_CellBuilder.cxx :: Problem with CSG surface creation." << endl;
				return;
			}
			else
			{
				aCSGSurf->SetCasSurf(theAdapSurface);
				//myUnits=McCadCSGGeom_MM;
				aCSGSurf->SetUnits(myUnits);
			}


			///////////////////////////////////////////////////////////////////////////////////////////////
			// we check first if the surface is already in the map!!
			McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface theIter;
			Standard_Boolean isFound = Standard_False;
			Standard_Integer foundSurfNumb = 0;
			for ( theIter.Initialize(surfMap); theIter.More(); theIter.Next())
			{
				Handle(McCadCSGGeom_Surface) tmpSurf = theIter.Value();
				if (aCSGSurf->IsEqual(tmpSurf))
				{
					isFound = Standard_True;
                    foundSurfNumb = theIter.Key();
					break;
				}
				else
					isFound = Standard_False;
			}
			///////////////////////////////////////////////////////////////////////////////////////////////
			if (isFound)
			{
				//cout << "_#_McCadCSGTool_CellBuilder.cxx :: A redundant surface has been discarded." << endl;
				if(orient == TopAbs_FORWARD)
				{
					 if(negAxis)
						 aCSG->Append(TCollection_AsciiString(foundSurfNumb));
					 else
						 aCSG->Append(TCollection_AsciiString(-foundSurfNumb));
				}
				else if (orient == TopAbs_REVERSED)
				{
					 if(negAxis)
					 	aCSG->Append(TCollection_AsciiString(-foundSurfNumb));
					 else
						aCSG->Append(TCollection_AsciiString(foundSurfNumb));
				}
				else
				{
					cout << "_#_McCadCSGTool_CellBuilder.cxx :: Incorrect surface orientation " << endl;
					return;
				}
			}
			else
			{
				if(orient == TopAbs_FORWARD)
				{
					Standard_Integer surfNumb = myCell->GetSurfCounter();
					surfNumb++;
					myCell->SetSurfCounter(surfNumb);
					aCSGSurf->SetNumber(surfNumb);

					/*if(theAdapSurface.GetType() == GeomAbs_Plane)
					{
						cout << " --- >Forward Plane : " << surfNumb << endl;

						if(negAxis)
							cout << "NEG";
						else
							cout << "POS";
						cout << endl;
					}*/

					if (aCSGSurf->HaveTransformation())
					{
						Standard_Integer trNumb = myCell->GetTrsfCounter();
						trNumb++;
						aCSGSurf->SetTrsfNumber(trNumb);
						myCell->SetTrsfCounter(trNumb);
					}
					 if(negAxis)
					 	aCSG->Append(TCollection_AsciiString(surfNumb));
					 else
						aCSG->Append(TCollection_AsciiString(-surfNumb));
					surfMap.Bind(surfNumb,aCSGSurf);
				}
                else if (orient == TopAbs_REVERSED)
				{
					Standard_Integer surfNumb = myCell->GetSurfCounter();
					surfNumb++;
					myCell->SetSurfCounter(surfNumb);

					aCSGSurf->SetNumber(surfNumb);


					/*if(theAdapSurface.GetType() == GeomAbs_Plane)
					{
						cout << " --- >Reversed Plane : " << surfNumb << endl;

						if(negAxis)
							cout << "NEG";
						else
							cout << "POS";
						cout << endl;
					}*/

					if (aCSGSurf->HaveTransformation())
					{
						Standard_Integer trNumb = myCell->GetTrsfCounter();
						trNumb++;
						aCSGSurf->SetTrsfNumber(trNumb);
						myCell->SetTrsfCounter(trNumb);
					}

					 if(negAxis)
						 aCSG->Append(TCollection_AsciiString(-surfNumb));
					 else
						aCSG->Append(TCollection_AsciiString(surfNumb));

					surfMap.Bind(surfNumb,aCSGSurf);
				}
				else
				{
					cout << "_#_McCadCSGTool_CellBuilder.cxx :: Incorrect surface orientation " << endl;
					return;
				}
			}
		}

		if(myExtSolidSeq->Length() > 1 || myMakeOuterVoid)
			aCSG->Append(")");
		// each time you have a solid behind you add a union sign but not to the last.
		// This is the semi-algebraic decomposition S = union*(i...n intersec s_i)
		if(i < myExtSolidSeq->Length())
			aCSG->Append(TCollection_AsciiString(" : "));
	}

	if(myExtSolidSeq->Length() > 1)
			aCSG->Append(") ");
	//test
	  /*for(int i=1; i<= aCSG->Length();i++)
	 {
	   cout << (aCSG->Value(i)).ToCString() <<  " ";
	 }
	 cout << endl;*/
	//test end
	myCell->SetCSG(aCSG);
	myCell->SetSurface(surfMap);
	myCell->SetComment(theComment);
	if(!myCell->IsVoid())
		myCell->SetVolume(cellVolume);

	//cout << "Number of surfaces used for cell = " << surfMap.Extent() << endl;
	//cout << "End cell building " << endl;
	//cout << "======================================= " << endl;
	Done();
}


Standard_Boolean McCadCSGTool_CellBuilder::IsDone() const
{
	return myIsDone;
}


void McCadCSGTool_CellBuilder::Done()
{
	myIsDone = Standard_True;
}


void McCadCSGTool_CellBuilder::NotDone()
{
	myIsDone = Standard_False;
}


Handle(McCadCSGGeom_Cell) McCadCSGTool_CellBuilder::GetCell() const
{
	return myCell;
}


Standard_Integer McCadCSGTool_CellBuilder::NbExtSolids() const
{
	return myExtSolidSeq->Length();
}


Standard_Integer McCadCSGTool_CellBuilder::NbFaces() const
{
	return myCell->GetSurfCounter();
}


Standard_Boolean McCadCSGTool_CellBuilder::IsSimpleSolid() const
{
	if (myExtSolidSeq->Length() ==1 && (myExtSolidSeq->Value(1))->HaveExt() == Standard_False)
		return Standard_True;
	else
		return Standard_False;
}


void McCadCSGTool_CellBuilder::SetUnits(McCadCSGGeom_Unit theUnit)
{
	myUnits = theUnit;
}

McCadCSGGeom_Unit McCadCSGTool_CellBuilder::GetUnits() const
{
	return myUnits;
}


/*----------------------------------
// PRIVATE FUNCTIONS
------------------------------------*/

Standard_Boolean McCadCSGTool_CellBuilder::ComputeGQ(const TopoDS_Face& theFace, math_Vector& pVec)
{
	/*
	 * Compute General Quadric (GQ) for given Face
	 *
	 * GQ is defined by: A x**2 + B y**2 + C z**2 + D xy + E yz + F zx + G x + H y + I z = -1
	 *
	 * Best fitting parameter vector (A,B,C,D,E,F,G,H,I) is found by solving the set of linear equations
	 * A x = b for x by using a singular value decomposition algorithm, implemented by OCC
	 * A herewith is the design matrix given by A_ij = X_j(x_i), with X_j \in {x**2, y**2, ... , y, z}
	 * b_j = -1 for all j=1..N, N=Number of Sample points. So A is a NxM matrix (N sample points, M parameters).
	 * Solving the above equation for x leads to the best fitting parameter vector in the xhisquare sense.
	 */

	//Get Sample Points on surface
	Standard_Integer sp=2, N=0;
	Handle(TColgp_HSequenceOfPnt) theSamplePoints;

	while(true)//collect sample points
	{
		theSamplePoints = McCadCSGTool::FaceSamplePoints(theFace,sp+1,sp);
		N = theSamplePoints->Length();

		/*for(int i=1; i<N; i++)
		{
			Standard_Integer sim=0;
			gp_Pnt pntA = theSamplePoints->Value(i);

			for(int j=i+1; j<=N; j++)
			{
				gp_Pnt pntB = theSamplePoints->Value(j);
				if(pntA.X()==pntB.X()) sim++;
				if(pntA.Y()==pntB.Y()) sim++;
				if(pntA.Z()==pntB.Z()) sim++;
			}

			if(sim<2)
			{
				cout << pntA.X() << "  " << pntA.Y() << "  " << pntA.Z() << endl;
				filteredPoints->Append(pntA);
			}
		}*/

		if(N >= 18)
			break;

		theSamplePoints->Clear();

		if(sp>4)
		{
			cout << "_#_McCadCSGTool_CellBuilder.cxx :: Can't create sufficient sample points for fit!!!" << endl;
			return Standard_False;
		}
		sp++;
	}

//Fill design matrix A
    Standard_Integer i,j;

	math_Vector b(1,N); // =-1
	math_Matrix A(1,N,1,9); //design matrix
	b.Init(-1); // A.a=b, where a='parameter vector', b='residual vector'

	for(i=1; i<=N; i++)
	{
		gp_Pnt pnt = theSamplePoints->Value(i);

		for(j=1;j<=9;j++)
		{
			Standard_Real eval = DjFunc(pnt,j);
			if(Abs(eval)<1.0e-9)
				eval = 0.0;

			A(i,j) = eval;
		}
	}

	math_SVD singVD(A); // perform singular value decomposition and solve herewith the least square problem
	singVD.Solve(b, pVec, 1e-7);

	//set small values to cero
	for(j=1; j<=9; j++)
		if(Abs(pVec(j))<1e-7)
			pVec(j)=0.0;

	//Test if sum of squared errors is smaller threshold (0.01)
	Standard_Real chiSqu(0.0);
	for(i=1;i<=N;i++)
	{
		Standard_Real val(0.0);
		for(j=1;j<=9;j++)
		{
			val += (A(i,j)*pVec(j));
		}
		val+=1;

		//cout << "val : " << val << endl;
		chiSqu += val*val;
	}

	cout << "\n\n\nCHISQU = " << chiSqu << "\n\n\n";

	if(chiSqu > 0.01)
	{
		cout << "_#_McCadCSGTool_CellBuilder.cxx :: Construction of General Quadric failed due to bad ChiSquare!!!\n";
		cout << "                                   ChiSquare = " << chiSqu << endl;
		cout << "-------------------------------------------------------------------------------------------------\n\n";
		return Standard_False;
	}


	return Standard_True;
}


Standard_Real McCadCSGTool_CellBuilder::DjFunc(const gp_Pnt& pnt, const Standard_Integer& indx)
{
	switch(indx)
	{
		case 1:	return (pnt.X() * pnt.X()); // x**2
		case 2: return (pnt.Y() * pnt.Y()); // y**2
		case 3: return (pnt.Z() * pnt.Z()); // z**2
		case 4: return (pnt.X() * pnt.Y()); // xy
		case 5: return (pnt.Y() * pnt.Z()); // yz
		case 6: return (pnt.Z() * pnt.X()); // zx
		case 7: return (pnt.X()); 			// x
		case 8: return (pnt.Y()); 			// y
		case 9: return (pnt.Z()); 			// z
		case 10:return (1);					// 1
		default: return 0;
	}
	return 0;
}

