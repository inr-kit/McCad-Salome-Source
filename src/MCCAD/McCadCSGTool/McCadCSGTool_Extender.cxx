#include <McCadCSGTool_Extender.ixx>

#include <McCadGTOOL.hxx>
#include <McCadCSGTool.hxx>
#include <McCadTDS_ExtFace.hxx>

#include <McCadCSGGeom_Surface.hxx>
#include <McCadCSGGeom_Plane.hxx>
#include <McCadCSGGeom_Cylinder.hxx>
#include <McCadCSGGeom_Cone.hxx>
#include <McCadCSGGeom_Sphere.hxx>
#include <McCadCSGGeom_Torus.hxx>
#include <McCadCSGGeom_SurfaceOfRevolution.hxx>

#include <gp_Pln.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>

#include <Geom_Surface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomTools.hxx>

#include <TColgp_HSequenceOfPnt.hxx>

#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>

#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>

#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
// #include <STEPControl_Writer.hxx>
#include <ShapeFix_ShapeTolerance.hxx>

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgo_Fuse.hxx>

#include <ShapeFix_Shape.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <gp_Cylinder.hxx>

#include <BRepAlgoAPI_Common.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeExtend_WireData.hxx>
#include <BRepTools_WireExplorer.hxx>


/*
 * Purpose:
 *
 * Extend the boundary support set in order to establish definability. That is, adding resultants
 * and partial derivatives to the already existing support set.
 *
 */


Handle(McCadCSGGeom_Surface) MakeMcCadSurf(const GeomAdaptor_Surface& theAdapSurface)
{
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
			cout << "_#_McCadCSGTool_Extender.cxx :: Surface is not analytic " << endl;
			break;
		}
		case GeomAbs_BSplineSurface:
		{
			cout << "_#_McCadCSGTool_Extender.cxx :: Surface is not analytic " << endl;
			break;
		}
		case GeomAbs_SurfaceOfRevolution:
		{
			//cout << "_#_McCadCSGTool_Extender.cxx :: Surface is not analytic " << endl;
			aCSGSurf = new McCadCSGGeom_SurfaceOfRevolution;
			break;
		}
		case GeomAbs_SurfaceOfExtrusion:
		{
			cout << "_#_McCadCSGTool_Extender.cxx :: Surface is not analytic " << endl;
			break;
		}
		case GeomAbs_OffsetSurface:
		{
			cout << "_#_McCadCSGTool_Extender.cxx :: Surface is not analytic " << endl;
			break;
		}
		case GeomAbs_OtherSurface:
		{
			cout << "_#_McCadCSGTool_Extender.cxx :: Surface is not analytic " << endl;
			break;
		}
	}

	aCSGSurf->SetCasSurf(theAdapSurface);

	return aCSGSurf;

}

McCadCSGTool_Extender::McCadCSGTool_Extender()
{

}

McCadCSGTool_Extender::McCadCSGTool_Extender(const TopoDS_Solid& theSolid)
{
	Init(theSolid);
}

void McCadCSGTool_Extender::Init(const TopoDS_Solid& theSolid)
{
	NotDone();

	mySolid = theSolid;
	myExtSolid = new McCadTDS_ExtSolid(mySolid);
	myExtSolid->SetHaveExt(Standard_False);

	Handle(TopTools_HSequenceOfShape) theResultantSeq = new TopTools_HSequenceOfShape();

	///////////////////////////////////////////////////////////////////////////////////////////
	/*TopTools_IndexedDataMapOfShapeListOfShape aMEF;
	TopExp::MapShapesAndAncestors(mySolid, TopAbs_EDGE, TopAbs_FACE, aMEF);*/

	//TopTools_MapOfShape theFinalFaceMap;
	//TopTools_MapOfShape theRemoveFaceMap;
//	Standard_Boolean isFused = Standard_False;

	// a map is too rigid. if there are more than two faces on one surface it produces to many fused faces
	// especially for toroidal surfaces this is bad and leads to meaningless partials, which might
	// destroy the final shape of the solid
	Handle(TopTools_HSequenceOfShape) fcSeq = new TopTools_HSequenceOfShape;

	for(TopExp_Explorer ex(theSolid, TopAbs_FACE); ex.More(); ex.Next())
		fcSeq->Append(ex.Current());

	//TopLoc_Location loc;

	//cout << "Number of faces in solid : " << fcSeq->Length() << endl;

	// fuse faces on the same surface that share an identical edge (different orientation)
	////////////////////////////////////////////////////////////////////////////////////////

	FuseFaces(fcSeq);

	if(fcSeq->Length() < 1)
	{
		Done();
		return;
	}

	//cout << "Number Of Faces after fusing : " << fcSeq->Length() << endl;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Compute Partials

	for(int i=1; i<=fcSeq->Length(); i++)
	{
		TopoDS_Face aFace = TopoDS::Face(fcSeq->Value(i));

		if(aFace.Orientation() != TopAbs_REVERSED)
			continue;

		theResultantSeq->Append(McCadCSGTool::Partials(aFace));
	}

	if (theResultantSeq->Length() == 0)
	{
		Done();
		return;
	}
	myExtSolid->SetHaveExt(Standard_True);

	//////////////////////////////////////////////////////////////////////////////
	// Generate sample points
	Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt;
	thePntSeq->Append(McCadCSGTool::SamplePoints(myExtSolid->GetSolid()));

	///////////////////////////////////////////////
	// Remove sign-changing resultants
	//STEPControl_Writer wrt;
	for (Standard_Integer i = 1; i <= theResultantSeq->Length(); i++)
	{
		TopoDS_Face theFace = TopoDS::Face(theResultantSeq->Value(i));
		Handle(McCadTDS_ExtFace) extFace = new McCadTDS_ExtFace(theFace);
		BRepAdaptor_Surface BS(theFace, Standard_True);
		GeomAdaptor_Surface theAdaptFaceSurface = BS.Surface();

		for (Standard_Integer i=1; i <= thePntSeq->Length(); i++)
		{
			Standard_Real aVal = McCadGTOOL::Evaluate(theAdaptFaceSurface, thePntSeq->Value(i));

			if (aVal > 5.)
				extFace->AppendPosPnt(thePntSeq->Value(i));
			if (aVal < -5.)
				extFace->AppendNegPnt(thePntSeq->Value(i));
		}

		Standard_Integer NbPosPnt = extFace->NbPosPnt();
		Standard_Integer NbNegPnt = extFace->NbNegPnt();

		if (NbNegPnt > 0 && NbPosPnt > 0) // i.e sign changing
		{
			/*wrt.Transfer(theFace, STEPControl_AsIs);
			cout << "SIGN CHANGING RESULTANT!!!\n";*/
			continue;
		}

		myExtSolid->AppendFace(theFace);
	}

	//wrt.Write("signChangingResultant.stp");

	Done();
}


Standard_Boolean McCadCSGTool_Extender::IsDone() const
{
	return myIsDone;
}

void McCadCSGTool_Extender::Done()
{
	myIsDone = Standard_True;
}

void McCadCSGTool_Extender::NotDone()
{
	myIsDone = Standard_False;
}

Handle(McCadTDS_ExtSolid) McCadCSGTool_Extender::ExtSolid() const
{
	return myExtSolid;
}


void McCadCSGTool_Extender::FuseFaces(Handle(TopTools_HSequenceOfShape)& fcSeq, Standard_Boolean reversedOnly)
{
	TopLoc_Location loc;
	Standard_Boolean isFused = Standard_False;
	// fuse faces on the same surface that share an identical edge (different orientation)
	////////////////////////////////////////////////////////////////////////////////////////

	// face 1
	for(int i=1; i<=fcSeq->Length(); i++)
	{
		isFused = Standard_False;
		TopoDS_Face f1 = TopoDS::Face(fcSeq->Value(i));
		const Handle(Geom_Surface)& aS1 = BRep_Tool::Surface(f1,loc);
		GeomAdaptor_Surface aAs1(aS1);
		Handle(McCadCSGGeom_Surface) mS1 = MakeMcCadSurf(aAs1);

		TopAbs_Orientation orient1 = f1.Orientation();

		// remove planes and surfaces with forward orientation
		////////////////////////////////////////////////////////
		if(aAs1.GetType() == GeomAbs_Plane || (orient1 != TopAbs_REVERSED && reversedOnly))
		{
			if(reversedOnly)
			{
				fcSeq->Remove(i);
				i--;
			}
			continue;
		}

		// face 2
		for(int j=fcSeq->Length(); j>i ; j--)
		{
			TopoDS_Face f2 = TopoDS::Face(fcSeq->Value(j));
			const Handle(Geom_Surface)& aS2 = BRep_Tool::Surface(f2,loc);
			GeomAdaptor_Surface aAs2(aS2);
			TopAbs_Orientation orient2 = f2.Orientation();

			if(aAs2.GetType() == GeomAbs_Plane || (orient2 != TopAbs_REVERSED && reversedOnly))
			{
				if(reversedOnly)
				{
					fcSeq->Remove(j);
				}
				continue;
			}

			if((aAs1.GetType() != aAs2.GetType()) || (orient1 != orient2))
				continue;

			Handle(McCadCSGGeom_Surface) mS2 = MakeMcCadSurf(aAs2);

			// check if faces lie on same surface
			//////////////////////////////////////////////
			if (!mS1->IsEqual(mS2))
			{
				//cout << "NOT EQUAL!!!\n\n\n";
				continue;
			}

			/*// edges of face 1
			for(TopExp_Explorer ex1(f1, TopAbs_EDGE); ex1.More(); ex1.Next())
			{
				TopoDS_Edge e1 = TopoDS::Edge(ex1.Current());

				// edges of face 2
				for(TopExp_Explorer ex2(f2, TopAbs_EDGE); ex2.More(); ex2.Next())
				{
					TopoDS_Edge e2 = TopoDS::Edge(ex2.Current());

					// faces share same edge (regardless of the orientation)
					if(e1.IsSame(e2))
					{*/

			// !!! Edge comparison doesn't work well
			// new method tests if the faces share the same surface
			// and if the UV-Bounds overlap or touch each other
			// GOOD LUCK !!!

						if (  (orient1 == TopAbs_REVERSED && reversedOnly) || ( !reversedOnly && (
							   aAs1.GetType() == GeomAbs_Cylinder || aAs1.GetType() == GeomAbs_Sphere  ||
							   aAs1.GetType() == GeomAbs_Cone     || aAs1.GetType() == GeomAbs_Torus   ) ) )
						{
							try
							{
								// check surface area
								/////////////////////////////////
								GProp_GProps GP1;
								BRepGProp::SurfaceProperties(f1, GP1);
								if(GP1.Mass() < 0.1)
									continue;
								// check surface area
								/////////////////////////////////
								GProp_GProps GP2;
								BRepGProp::SurfaceProperties(f2, GP2);
								if(GP2.Mass() < 0.1)
									continue;

								// NON-Boolean Fusion
								//////////////////////////////////
								/*
								 * Boolean operations in OCC tend to freeze the program or
								 * crash it, from time to time. Therefore for simple shapes
								 * (i.e. cutted along iso(UV)lines) we don't use boolean ops.
								 */

								// check if bounds coincide
								Standard_Real UMin1,UMax1, VMin1, VMax1;
								BRepTools::UVBounds(f1,UMin1,UMax1, VMin1, VMax1);
								Standard_Real UMin2,UMax2, VMin2, VMax2;
								BRepTools::UVBounds(f2,UMin2,UMax2, VMin2, VMax2);
                                                                Standard_Real UMin(0.0),UMax(0.0), VMin(0.0), VMax(0.0);

								// Nullify small dimensions
								if(Abs(UMin1) < 1.e-10)
									UMin1 = 0.0;
								if(Abs(UMin2) < 1.e-10)
									UMin2 = 0.0;
								if(Abs(UMax1) < 1.e-10)
									UMax1 = 0.0;
								if(Abs(UMax2) < 1.e-10)
									UMax2 = 0.0;
								if(Abs(VMin1) < 1.e-10)
									VMin1 = 0.0;
								if(Abs(VMin2) < 1.e-10)
									VMin2 = 0.0;
								if(Abs(VMax1) < 1.e-10)
									VMax1 = 0.0;
								if(Abs(VMax2) < 1.e-10)
									VMax2 = 0.0;


								// Perform UV test
								{
									// if U-Values do not collide in any direction continue
                                                                        if( (   ((UMin1 < UMin2) && (UMin1 < UMax2)) &&
										((UMax1 < UMin2) && (UMax1 < UMax2)) &&
                                                                                (UMin1 != 0) ) ||
                                                                            (	((UMin1 > UMin2) && (UMin1 > UMax2)) &&
										((UMax1 > UMin2) && (UMax1 > UMax2)) &&
                                                                                (UMin2 != 0)) )
									{
										continue;
									}


									if(aAs1.GetType() == GeomAbs_Torus)
									{
                                                                                if( (   ((VMin1 < VMin2) && (VMin1 < VMax2)) &&
                                                                                        ((VMax1 < VMin2) && (VMax1 < VMax2)) )  ||
                                                                                    (   ((VMin1 > VMin2) && (VMin1 > VMax2)) &&
                                                                                        ((VMax1 > VMin2) && (VMax1 > VMax2)) )
										)
											continue;
									}
								}

								// make sure uv bounds lie on iso-lines
								Standard_Boolean boundsOnISO(Standard_True);

								// get main axis
								gp_Ax1 mainAx;

								if(aAs1.GetType() == GeomAbs_Cylinder)
									mainAx = aAs1.Cylinder().Axis();
								else if(aAs1.GetType() == GeomAbs_Cone)
									mainAx = aAs1.Cone().Axis();
								else if(aAs1.GetType() == GeomAbs_Sphere)
									;
								else if(aAs1.GetType() == GeomAbs_Torus)
									;
								else
									continue;

								if(aAs1.GetType() == GeomAbs_Cylinder || aAs1.GetType() == GeomAbs_Cone)
								{
									for(TopExp_Explorer ex1(f1, TopAbs_EDGE); ex1.More(); ex1.Next())
									{
										Standard_Real first, mid, last;
										TopoDS_Edge e1 = TopoDS::Edge(ex1.Current());

										Handle(Geom_Curve) theCurve = BRep_Tool::Curve(e1, first, last);

										if(GeomAdaptor_Curve(theCurve).GetType() == GeomAbs_Line)
											continue;

										mid = (first+last)/2.0;

										gp_Pnt p1, p2, p3;
										theCurve->D0(first, p1);
										theCurve->D0(mid, p2);
										theCurve->D0(last, p3);

										gp_Vec v1(p2,p1), v2(p2,p3), v1xv2;
										v1xv2 = v1.Crossed(v2);

										gp_Dir dir(v1xv2);
										gp_Ax1 ax(p2, dir);

										/*cout << "mainAxis : " << mainAx.Direction().X() << " , " << mainAx.Direction().Y() << " , " << mainAx.Direction().Z() << endl;
										cout << "ax : " << ax.Direction().X() << " , " << ax.Direction().Y() << " , " << ax.Direction().Z() << endl;*/

										if(!ax.IsParallel(mainAx, 0.01))
										{
											boundsOnISO = Standard_False;
											break;
										}
									}
								}
								else
									boundsOnISO = Standard_True;


						if(boundsOnISO)
						{ //ISO LINES

								// test if cylinders and cones can be fused without boolean fusion
								if(aAs1.GetType() == GeomAbs_Cylinder || aAs1.GetType() == GeomAbs_Cone || aAs1.GetType() == GeomAbs_Sphere)
								{

									/*if(aAs1.GetType() == GeomAbs_Cylinder)
									{
										gp_Cylinder theCylinder = aAs1.Cylinder();

										cout << "Cylinder : " << theCylinder.Radius() << endl;
										cout << "U1 : " << UMin1 << " , " << UMax1 << "   U2 : " << UMin2 << " , " << UMax2 << endl;
										cout << "V1 : " << VMin1 << " , " << VMax1 << "   V2 : " << VMin2 << " , " << VMax2 << endl;
									}*/
									{
                                        if(Abs(UMax1-UMin1+UMax2-UMin2 - 2*M_PI) < 1e-5)
										{
											UMin = 0;
                                            UMax = 2*M_PI;
										}
										else
										{
                                                                                        if( Abs(UMin1-UMax2) < 1.0e-5 || ( UMin1 == 0 && Abs(UMax2-2*M_PI) < 1e-7 ))
											{
												UMin = UMin2;
												UMax = UMax1;
											}
                                                                                        else if( Abs(UMin2-UMax1) < 1.0e-5 || ( UMin2 == 0 && Abs(UMax1-2*M_PI) < 1e-7 ) )
											{
												UMin = UMin1;
												UMax = UMax2;
											}

											if(UMax < UMin)
                                                UMax += 2*M_PI;
										}

										if (VMin1 <= VMin2)
											VMin = VMin1;
										else
											VMin = VMin2;

										if (VMax1 >= VMax2)
											VMax = VMax1;
										else
											VMax = VMax2;
									}
									/*if(aAs1.GetType() == GeomAbs_Cylinder)
										cout << "U : " << UMin << " , " << UMax << "    V :" << VMin << " , " << VMax << endl;*/
								}
								else if(aAs1.GetType() == GeomAbs_Torus)
								{
									// Maximize
									/////////////////////////
									if (UMin1 <= UMin2)
										UMin = UMin1;
									else
										UMin = UMin2;

									if (VMin1 <= VMin2)
										VMin = VMin1;
									else
										VMin = VMin2;

									if (UMax1 >= UMax2)
										UMax = UMax1;
									else
										UMax = UMax2;

									if (VMax1 >= VMax2)
										VMax = VMax1;
									else
										VMax = VMax2;

                                    if((Abs(VMax-VMin) - 2*M_PI) < 1e-5)
									{
										VMin = 0;
                                        VMax = 2*M_PI;
									}
								}
								else
									continue;

								// make new face
								/////////////////////////
#ifdef OCC650
								TopoDS_Face theNewFace  = BRepBuilderAPI_MakeFace(aS1, UMin,UMax, VMin, VMax).Face();
#else
						TopoDS_Face theNewFace  = BRepBuilderAPI_MakeFace(aS1, UMin,UMax, VMin, VMax, 1.e-7).Face();
#endif

								theNewFace.Orientation(f1.Orientation());

								if(!theNewFace.IsNull())
								{
									fcSeq->Remove(j);
									fcSeq->Remove(i);
									fcSeq->Append(theNewFace);

									isFused = Standard_True;
									i--;
									break;
								}
								else
									cout << "theNewFace is NULL!!!\n";

						} //# ISO lines

								//////////////////////////////////////////
								// TRY Boolean Fusion
								//////////////////////////////////////////

								// perform boolean operation
							cout << "fusing ... \n";
								TopoDS_Face theNewFace;

								/*STEPControl_Writer wrt;
								wrt.Transfer(f1, STEPControl_AsIs);
								wrt.Transfer(f2, STEPControl_AsIs);
								wrt.Write("beforecrash.stp");*/

								TopoDS_Shape fsdShp;

								try
								{
									BRepAlgoAPI_Fuse fuser(f1, f2);

									if(!fuser.IsDone())
										cout << "### not fused!!!\n";

									fsdShp = fuser.Shape();
								}
								catch(...)
								{
								//	cout << "Hello, I'm your fuser. I refuse to fuse. Sincerely, BRepAlgo_Fuse\n";
								}

								Handle(TopTools_HSequenceOfShape) fuseResults = new TopTools_HSequenceOfShape;

								Standard_Integer fcCnt(0);
								for(TopExp_Explorer ex(fsdShp, TopAbs_FACE); ex.More(); ex.Next())
								{
									fcCnt++;
									theNewFace = TopoDS::Face(ex.Current());
									fuseResults->Append(theNewFace);
								}


								if(fcCnt != 1) // fusion failed # try diffrent fuse tool
								{
									cout << "1st fusing attempt failed\n";
									if(aAs1.GetType() == GeomAbs_Cylinder)
									{
										//cout << "# CYLINDER\n";
										// build Bounding Box
										////////////////////////////
										Bnd_Box aBB;
										BRepBndLib::Add(f1, aBB);
										BRepBndLib::Add(f2, aBB);
										aBB.SetGap(1);

										Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
										aBB.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

										TopoDS_Shape aBox = BRepPrimAPI_MakeBox(gp_Pnt(aXmin, aYmin, aZmin),gp_Pnt(aXmax, aYmax, aZmax)).Shape();
										gp_Pnt x(aXmin, aYmin, aZmin);
										gp_Pnt y(aXmax, aYmax, aZmax);

										gp_Pnt centralPnt((y.X()+x.X())/2.0, (y.Y()+x.Y())/2.0, (y.Z()+x.Z())/2.0);

										gp_Cylinder aCyl = aAs1.Cylinder();
										TopoDS_Face cylFc = BRepBuilderAPI_MakeFace(aCyl).Face();

										// Find Common from Bounding Box and the cylindrical Surface
										/////////////////////////////////////////////////////////////
										BRepAlgoAPI_Common cmn1( cylFc, aBox);
										TopoDS_Shape cmnShp1;
										if(cmn1.IsDone())
											cmnShp1 = cmn1.Shape();

										if(cmnShp1.IsNull())
										{
											cout << "Null Shape computed!!\n";
											continue;
										}

										// find curved edges
										/////////////////////////////////////
										Handle(TopTools_HSequenceOfShape) edges = new TopTools_HSequenceOfShape;
										for(TopExp_Explorer ex(f1, TopAbs_EDGE); ex.More(); ex.Next())
											edges->Append(ex.Current());
										for(TopExp_Explorer ex(f2, TopAbs_EDGE); ex.More(); ex.Next())
											edges->Append(ex.Current());

										Handle(TopTools_HSequenceOfShape) curvedEdges = new TopTools_HSequenceOfShape;

										for(Standard_Integer i=1; i<=edges->Length(); i++)
										{
											TopoDS_Edge e = TopoDS::Edge(edges->Value(i));
											BRepAdaptor_Curve baCrv;
											baCrv.Initialize(e);

											if(baCrv.GetType() == GeomAbs_Line)
												continue;
											else
												curvedEdges->Append(e);

										//	BRepTools::Dump(e, cout);

										}
										edges->Clear();


										// split into edges for two faces
										//////////////////////////////////
										Handle(TopTools_HSequenceOfShape) frstSeq = new TopTools_HSequenceOfShape;

										Standard_Integer rejCnt(0);
										Standard_Integer maxRej(curvedEdges->Length()-3);

										//fill first
										///////////////////
										for(Standard_Integer i=1; i<=curvedEdges->Length(); i++)
										{
											TopoDS_Edge curE = TopoDS::Edge(curvedEdges->Value(i));

											if(frstSeq->Length() < 1)
											{
												frstSeq->Append(curE);
												curvedEdges->Remove(1);
												i--;
												continue;
											}

											TopExp_Explorer ex(curE, TopAbs_VERTEX);
											TopoDS_Vertex v1 = TopoDS::Vertex(ex.Current());
											ex.Next();
											TopoDS_Vertex v2 = TopoDS::Vertex(ex.Current());

											Standard_Boolean isAppended(Standard_False);

											for(Standard_Integer j=1; j<=frstSeq->Length(); j++)
											{
												TopoDS_Edge e_j = TopoDS::Edge(frstSeq->Value(j));
												for(TopExp_Explorer vEx(e_j, TopAbs_VERTEX); vEx.More(); vEx.Next())
												{
													TopoDS_Vertex v_j = TopoDS::Vertex(vEx.Current());

													if(v_j.IsSame(v1) || v_j.IsSame(v2))
													{
														frstSeq->Append(curE);
														curvedEdges->Remove(i);
														maxRej = curvedEdges->Length() - 3;
														rejCnt=0;
														i--;
														isAppended = Standard_True;
														break;
													}
												}
												if(isAppended)
													break;
											}

											if(!isAppended)
											{
												if(rejCnt == maxRej) // we're done here
													break;
												rejCnt++;
											}
										}

										// Build cut faces
										/////////////////////////
										TopoDS_Face cF1, cF2;

										if(frstSeq->Length() < 2 || curvedEdges->Length() < 2)
											continue;

										if(frstSeq->Length() == 2)
										{
											TopoDS_Edge ed = TopoDS::Edge(frstSeq->Value(1));
											gp_Pnt p1, p2, p3;
											BRepAdaptor_Curve baC(ed);
											baC.D0(baC.FirstParameter(), p1);
											baC.D0((baC.FirstParameter()+baC.LastParameter())/2.0, p2);
											baC.D0(baC.LastParameter(), p3);

											gp_Vec v1(p1, p2), v2(p1, p3), v3;
											v3 = v1.Crossed(v2);
											gp_Dir dir(v3);
											gp_Pln pln1(p1, dir);

											cF1 = BRepBuilderAPI_MakeFace(pln1).Face();
										}
										else
										{
											TopoDS_Edge ed1 = TopoDS::Edge(frstSeq->Value(1));
											TopoDS_Edge ed2 = TopoDS::Edge(frstSeq->Value(2));
											gp_Pnt p1, p2, p3, p4;
											BRepAdaptor_Curve baC1(ed1);
											BRepAdaptor_Curve baC2(ed2);

											baC1.D0(baC1.FirstParameter(), p1);
											baC1.D0(baC1.LastParameter(), p2);
											baC2.D0(baC2.FirstParameter(), p3);
											baC2.D0(baC2.LastParameter(), p4);

											if(p3.IsEqual(p1, 1.e-3) || p3.IsEqual(p2, 1.e-3))
												p3 = p4;

											gp_Vec v1(p1, p2), v2(p1, p3), v3;
											v3 = v1.Crossed(v2);
											gp_Dir dir(v3);
											gp_Pln pln1(p1, dir);

											cF1 = BRepBuilderAPI_MakeFace(pln1).Face();
										}

										if(curvedEdges->Length() == 2)
										{
											TopoDS_Edge ed = TopoDS::Edge(curvedEdges->Value(1));
											gp_Pnt p1, p2, p3;
											BRepAdaptor_Curve baC(ed);
											baC.D0(baC.FirstParameter(), p1);
											baC.D0((baC.FirstParameter()+baC.LastParameter())/2.0, p2);
											baC.D0(baC.LastParameter(), p3);

											gp_Vec v1(p1, p2), v2(p1, p3), v3;
											v3 = v1.Crossed(v2);
											gp_Dir dir(v3);
											gp_Pln pln1(p1, dir);

											cF2 = BRepBuilderAPI_MakeFace(pln1).Face();
										}
										else
										{
											TopoDS_Edge ed1 = TopoDS::Edge(curvedEdges->Value(1));
											TopoDS_Edge ed2 = TopoDS::Edge(curvedEdges->Value(2));
											gp_Pnt p1, p2, p3, p4;
											BRepAdaptor_Curve baC1(ed1);
											BRepAdaptor_Curve baC2(ed2);

											baC1.D0(baC1.FirstParameter(), p1);
											baC1.D0(baC1.LastParameter(), p2);
											baC2.D0(baC2.FirstParameter(), p3);
											baC2.D0(baC2.LastParameter(), p4);

											if(p3.IsEqual(p1, 1.e-3) || p3.IsEqual(p2, 1.e-3))
												p3 = p4;

											gp_Vec v1(p1, p2), v2(p1, p3), v3;
											v3 = v1.Crossed(v2);
											gp_Dir dir(v3);
											gp_Pln pln1(p1, dir);

											cF2 = BRepBuilderAPI_MakeFace(pln1).Face();
										}

										// cut result face on its ends
										/////////////////////////////////
										TopoDS_Shape hSol1 = BRepPrimAPI_MakeHalfSpace(cF1,centralPnt).Solid();
										TopoDS_Shape hSol2 = BRepPrimAPI_MakeHalfSpace(cF2,centralPnt).Solid();

										//resize half spaces
										BRepAlgoAPI_Common resizer1(hSol1, aBox);
										hSol1 = resizer1.Shape();
										BRepAlgoAPI_Common resizer2(hSol2, aBox);
										hSol2 = resizer2.Shape();

										BRepAlgoAPI_Common cmn3( cmnShp1, hSol1);
										TopoDS_Shape cmnShp2 = cmn3.Shape();
										BRepAlgoAPI_Common cmn4( cmnShp2, hSol2);
										TopoDS_Shape finalFace = cmn4.Shape();
										Standard_Integer cnt(0);

										for(TopExp_Explorer ex(finalFace, TopAbs_FACE); ex.More(); ex.Next())
										{
											theNewFace = TopoDS::Face(ex.Current());
											cnt++;
										}

										if(cnt != 1)
											continue;
									}
									else
									{
									//	cout << "OTHER\n";
										//BRepTools::Dump(f1, cout);
									}
								}

								// shape Heal
								Handle(ShapeFix_Shape) shpFixer = new ShapeFix_Shape(theNewFace);
								shpFixer->SetPrecision(1e-7);
								shpFixer->SetMaxTolerance(1e-3);
								shpFixer->FixWireTool()->FixRemovePCurveMode() = 1;
								shpFixer->Perform();

								fcCnt = 0;
								if(shpFixer->Status(ShapeExtend_DONE) )
								{
									TopoDS_Shape resFace = shpFixer->Shape();

									for(TopExp_Explorer ex1(resFace, TopAbs_FACE); ex1.More(); ex1.Next())
									{
										theNewFace = TopoDS::Face(ex1.Current());
										fcCnt++;
									}
								}
								else
									cout << "_#_McCadCSGTool_Extender.cxx :: Shape Heal failed!\n";

								if(fcCnt > 1)
									continue;

								// proceed
								if(!theNewFace.IsNull())
								{
									fcSeq->Remove(j);
									fcSeq->Remove(i);
									fcSeq->Append(theNewFace);
									isFused = Standard_True;

									i--;
									break;
								}
							}
							catch(...)
							{
								cout << "_#_McCadCSGTool_Extender.cxx :: Face Fuse failed !!!" << endl;
							}
						}
						else
						{
							cout << reversedOnly << endl;
							exit(-1);
						}
					/*}
					else
					{
						cout << "Not the same edge\n";
						if(aAs1.GetType() != GeomAbs_Torus)
						{
							STEPControl_Writer wrt;
							wrt.Transfer(f1, STEPControl_AsIs);
							wrt.Transfer(f2, STEPControl_AsIs);
							wrt.Write("edges.stp");
							exit(-1);
						}
					}
				}
				if(isFused)
					break;
			}*/

			if(isFused)
				break;
		}
	}
//	cout << "# Number of faces after fusing : " << fcSeq->Length() << endl;
//	cout << "##################### LEAVING FUSE #########################\n\n";
	/*STEPControl_Writer wrt;
	for(Standard_Integer i=1; i<=fcSeq->Length(); i++)
		wrt.Transfer(fcSeq->Value(i), STEPControl_AsIs);
	wrt.Write("fused.stp");
	exit(-1);*/
}



// we delete redundant resultant surfaces. Note that they are linear.

//////////////////////////////////////////////////////////////////////
/*
 for (Standard_Integer i = 1 ; i <= theResultantSeq->Length(); i++)
 {

 TopoDS_Face theFace =  TopoDS::Face(theResultantSeq->Value(i));

 BRepAdaptor_Surface BS(theFace,Standard_True);

 GeomAdaptor_Surface theAdaptSurface = BS.Surface();

 if (theAdaptSurface.GetType() !=  GeomAbs_Plane) { cout << "Non planar resultant " << endl; return;}

 gp_Pln rPln = theAdaptSurface.Plane();
 Standard_Real rA,rB,rC,rD;
 rPln.Coefficients(rA,rB,rC,rD);

 // cout << " rDir    X = " << rA << "  Y = " << rB  << "  Z = " << rC << " " << rD << endl;

 for (ex.Init(mySolid,TopAbs_FACE); ex.More(); ex.Next())
 {

 TopoDS_Face aFace = TopoDS::Face(ex.Current());

 BRepAdaptor_Surface BS(aFace,Standard_True);

 GeomAdaptor_Surface adSurface = BS.Surface();

 if (adSurface.GetType() == GeomAbs_Plane)
 {
 gp_Pln sPln = adSurface.Plane();

 Standard_Real sA,sB,sC,sD;

 sPln.Coefficients(sA,sB,sC,sD);
 //  cout << " sDir    X = " << sA << "  Y = " << sB  << "  Z = " << sC << " " << sD << endl;
 // this is a tmp solution: the tolerances should be corrected later.
 if( (fabs(rA - sA) < 1e-3)  &&
 (fabs(rB - sB) < 1e-3)  &&
 (fabs(rC - sC) < 1e-3)  &&
 (fabs(rD - sD) < 1e-3) )
 {
 theResultantSeq->Remove(i);
 cout << "... .. .. Deleting redundant resultants !!! " << endl;
 }
 }
 }
 }
 */
/////////////////////////////////////////////////////////////////

/// we keep only sign constant resultants; reason see theoretical part
// if(theResultantSeq->Length() == 0){
// Done();
//  cout << "All resultants seem to be redundant!!! " << endl;
// return;
// }

