#include <McCadMcVoid_Generator.ixx>

#include <McCadMcVoid.hxx>
#include <McCadDiscDs_DiscFace.hxx>
#include <McCadDiscretization_Solid.hxx>
#include <McCadDiscretization_Face.hxx>

#include <McCadCSGTool_Extender.hxx>
#include <McCadTDS_ExtSolid.hxx>

#include <McCadCSGGeom_Surface.hxx>
#include <McCadMcVoid.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopoDS_Face.hxx>
#include <TopAbs_Orientation.hxx>

#include <TColStd_HSequenceOfReal.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColgp_HSequenceOfPnt.hxx>

#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Geom_Plane.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>

#include <BRepPrim_Sphere.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTools.hxx>

#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRep_Builder.hxx>

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgo_Common.hxx>
#include <BRepAlgo_Cut.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Face.hxx>

#include <STEPControl_Writer.hxx>
#include <STEPControl_Reader.hxx>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <McCadGTOOL.hxx>
#include <McCadCSGTool.hxx>
#include <GeomAPI_IntSS.hxx>

#include <BRepCheck_Analyzer.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeUpgrade_ShellSewing.hxx>

//qiu #include <BOPTools_DSFiller.hxx>

#include <IntTools_FaceFace.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_Curve.hxx>
#include <TopoDS_Edge.hxx>

#include <GC_MakePlane.hxx>

#include <McCadMessenger_Singleton.hxx>
#include <TopOpeBRep_ShapeIntersector.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepClass_FaceClassifier.hxx>

#include <IntTools_FaceFace.hxx>
#include <IntTools_SequenceOfCurves.hxx>
#include <IntTools_Curve.hxx>


Standard_Boolean decompose(Standard_False);
Standard_Real originalBoxSize(0.0);
int glblCnt(0);

McCadMcVoid_Generator::McCadMcVoid_Generator()
{
        myMinVolume = 1.25e2;
        myMaxComplementedShapes = 10;
        myMaxNbOfPreDecomposition = 500;
        myMinDecompositionFaceArea = 500;
        myMinRedecompositionFaceArea = 50;
        myTolerance = 1.e-7;
        myIsDone = false;
        myVSolids = NULL;
        myVVoids = new McCadDiscDs_HSequenceOfDiscSolid;

        myMaxNbPnt = 50;
        myMinNbPnt = 10;
        myXlen = 50; // 5cm resolution
        myYlen = 50;
        myCFRes = 5;
}

McCadMcVoid_Generator::McCadMcVoid_Generator(Handle(McCadDiscDs_HSequenceOfDiscSolid)& theDShape)
{
        myMinVolume = 1.25e2;
        myMaxComplementedShapes = 10;
        myMaxNbOfPreDecomposition = 500;
        myMinDecompositionFaceArea = 500;
        myMinRedecompositionFaceArea = 50;
        myTolerance = 1.e-7;
        myIsDone = false;
        myVSolids = theDShape;
        myVVoids = new McCadDiscDs_HSequenceOfDiscSolid;

        myMaxNbPnt = 50;
        myMinNbPnt = 10;
        myXlen = 50; // 5cm resolution
        myYlen = 50;
        myCFRes = 5;
}

void McCadMcVoid_Generator::SetDiscSolids(const Handle(McCadDiscDs_HSequenceOfDiscSolid)& theSolids)
{
        myVSolids = theSolids;
}

#ifndef MCCAD_NONGUI
void McCadMcVoid_Generator::SetProgressIndicator(Handle_Message_ProgressIndicator& thePI)
{
        myPI = thePI;
}
#endif

Handle(McCadDiscDs_HSequenceOfDiscSolid) McCadMcVoid_Generator::GetDiscSolids() const
{
        return myVSolids;
}


Handle(McCadDiscDs_HSequenceOfDiscSolid) McCadMcVoid_Generator::GetDiscVoids() const
{
        if(myIsDone) return myVVoids;
        else
        {
                cout << "_#_McCadMcVoid_Generator.cxx :: Void generation has not yet been performed or failed!" << endl;
                return myVVoids;
        }
}


void McCadMcVoid_Generator::SetBSolid(const Handle(TopTools_HSequenceOfShape)& theBSolid)
{
        myBSolid = theBSolid;
}


Handle(TopTools_HSequenceOfShape)McCadMcVoid_Generator::GetBSolid() const
{
        return myBSolid;
}


TopoDS_Shape McCadMcVoid_Generator::MkOuterSpace()
{
        ///////////////////////////////////////////////////////////////////////////////////////////
        // here we make the outer sphere
        Bnd_Box aBBBox;

        for(Standard_Integer i=1; i<=myBSolid->Length(); i++)
        {
                for (TopExp_Explorer exv(myBSolid->Value(i), TopAbs_VERTEX); exv.More(); exv.Next())
                {
                        gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(exv.Current()));
                        aBBBox.Add(aPnt);
                }
        }

        aBBBox.SetGap(0.0);
        Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
        aBBBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

        gp_Pnt theUPnt = gp_Pnt(aXmax, aYmax, aZmax);
        gp_Pnt theLPnt = gp_Pnt(aXmin, aYmin, aZmin);


        gp_Pnt theCenterOfMass;
        GProp_GProps Props;
        Standard_Real theRadius;

        TopoDS_Shape BigB = BRepPrimAPI_MakeBox(theLPnt,theUPnt).Shape();
        BRepGProp::VolumeProperties(BigB, Props);
        theCenterOfMass = Props.CentreOfMass();

        theRadius = theCenterOfMass.Distance(theLPnt);

        if(theCenterOfMass.Distance(theUPnt) > theRadius)
                theRadius = theCenterOfMass.Distance(theUPnt);

        mySphere = BRepPrimAPI_MakeSphere(gp_Pnt((theLPnt.X() + theUPnt.X())/2, (theLPnt.Y()+ theUPnt.Y())/2, (theLPnt.Z()+ theUPnt.Z())/2), theRadius + 100.0).Shape();

        ////////////////////////////////////////////////////////////////////////////////////////
        return mySphere;
}


void McCadMcVoid_Generator::SetMinVolume(const Standard_Real theVol)
{
        myMinVolume = theVol;
}


void McCadMcVoid_Generator::SetMaxComplementedShapes(const Standard_Integer theNumb)
{
        myMaxComplementedShapes = theNumb;
}


void McCadMcVoid_Generator::SetMaxNbOfPreDecomposition(	const Standard_Integer theNumb)
{
        myMaxNbOfPreDecomposition = theNumb;
}


void McCadMcVoid_Generator::SetMinDecompositionFaceArea(const Standard_Real theArea)
{
        myMinDecompositionFaceArea = theArea;
}


void McCadMcVoid_Generator::SetMinRedecompositionFaceArea(const Standard_Real theArea)
{
        myMinRedecompositionFaceArea = theArea;
}


void McCadMcVoid_Generator::SetTolerance(const Standard_Real theTol)
{
        myTolerance = theTol;
}


Standard_Boolean McCadMcVoid_Generator::IsDone() const
{
        return myIsDone;
}


void McCadMcVoid_Generator::SetDiscParameter(const Standard_Integer theMaxNbPnt, const Standard_Integer theMinNbPnt,
                                                                                         const Standard_Real theXRes, const Standard_Real theYRes,	const Standard_Real theTol)
{
        myMaxNbPnt = theMaxNbPnt;
        myMinNbPnt = theMinNbPnt;
        myXlen = theXRes;
        myYlen = theYRes;
}


void McCadMcVoid_Generator::SetCutFaceResolution(const Standard_Real theCFRes)
{
        myCFRes = theCFRes;
}



//////////////////////////////////////////////////////////////////
/*++++++++++++++++++++++++++++  INIT  ++++++++++++++++++++++++++*/
//////////////////////////////////////////////////////////////////
void McCadMcVoid_Generator::Init()
{
        PreDecompose();
        VoidGenerate();
        myIsDone = true;
}

//////////////////////////////////////////////////////////////////
/*++++++++++++++++++++  PREDECOMPOSE  ++++++++++++++++++++++++++*/
//////////////////////////////////////////////////////////////////

void McCadMcVoid_Generator::PreDecompose()
{
        McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
        Handle(TopTools_HSequenceOfShape) BoxSeq = new TopTools_HSequenceOfShape();

        // For Debugging -- skip decomposition part and jump directly to
        // voxelization and voidgenerate !!!
        decompose = Standard_True;
        Standard_Boolean havePredecomposed = Standard_False;

        if(getenv("MCCAD_PREDECOMPOSED") != NULL)
        {
                havePredecomposed = Standard_True;
                TCollection_AsciiString PreDecomposedFile(getenv("MCCAD_PREDECOMPOSED"));
                if(PreDecomposedFile.Length() > 0)
                {
                        decompose = Standard_False;

                        if(getenv("MCCAD_DECOMPOSE") != NULL)
                                decompose = Standard_True;

                        cout << "\n==================================================\n" <<
                                        "!!! WARNING :  Predecomposed Voids used from file : " <<
                                        PreDecomposedFile.ToCString() << endl <<
                                        "==================================================\n\n";
                        STEPControl_Reader rdr;
                        rdr.ReadFile(PreDecomposedFile.ToCString());
                        Standard_Integer nbRoots = rdr.NbRootsForTransfer();

                        cout << "\n-----------------------------------------\nNumber of roots : " << nbRoots << "\n\n";

                        for(Standard_Integer i=1; i<=nbRoots; i++)
                        {
                                if(!rdr.TransferRoot(i))
                                {
                                        cout << "!!! Warning : skipping root nb : " << i << endl;
                                        continue;
                                }

                                TopoDS_Shape aShape = rdr.Shape(i);
                                BoxSeq->Append(aShape);
                        }
                }
        }

        if(decompose)
        {
                /////////////////////////////////////////////////////////////////////
                // generate cut faces and do predecomposition using only planar faces
                // compose cutfaces

                Handle(McCadDiscDs_HSequenceOfDiscFace) theVCutFace = new McCadDiscDs_HSequenceOfDiscFace();
                Handle(McCadDiscDs_HSequenceOfDiscFace) theResultants = new McCadDiscDs_HSequenceOfDiscFace();
                //NONPLANAR
                Handle(McCadDiscDs_HSequenceOfDiscFace) theNPFace = new McCadDiscDs_HSequenceOfDiscFace();
                //#NONPLANAR
                myPI->SetStep(5./myVSolids->Length());


                /*
                 * Use planar faces to cut bounding box
                 */
                for(int i = 1; i <= myVSolids->Length(); i++)//For all voxel solids append planar faces to theVCutFace and discretize these faces
                {
                        TopoDS_Solid tmpSol = TopoDS::Solid(myVSolids->Value(i).GetSolid());
                        Handle(McCadDiscDs_HSequenceOfDiscFace) ownVFaces = myVSolids->Value(i).GetOwnVFaces();

                //	cout << "\nSOLID No.: " << i << "....................." << endl;

                        //Standard_Integer plnCnt(0);
                        for(int j = 1; j <= ownVFaces->Length(); j++)//For all faces check if planar, if true and if area is bigger MinDecomposition: append
                        {
                                TopoDS_Face aF = TopoDS::Face(ownVFaces->Value(j).GetFace());
                                BRepAdaptor_Surface Bs1(aF,Standard_True);
                                GeomAdaptor_Surface AS = Bs1.Surface();

                                Standard_Real area = McCadMcVoid::AreaOfShape(aF);

                                if(AS.GetType() == GeomAbs_Plane)
                                {
                                  // cout << "Plane No: " << ++plnCnt <<  " area = " << area << endl;
                                        if(area > myMinDecompositionFaceArea)
                                                theVCutFace->Append(ownVFaces->Value(j));
                                        else
                                        {

                                        }
                                }
                                else if(AS.GetType() == GeomAbs_Cylinder)// CYLINDRICAL ONLY -- so far
                                {
                                        if(area > myMinDecompositionFaceArea)
                                                theNPFace->Append(ownVFaces->Value(j));
                                }
                                        //#NONPLANAR
                        }

                        ///////////////////////////////////////////////////////////////////////////////
                        McCadCSGTool_Extender Mext(tmpSol);
                        if(Mext.IsDone())// Append resultants for nonlinear surfaces.
                        {
                                Handle(McCadTDS_ExtSolid) mcSol = Mext.ExtSolid();
                                //TopoDS_Solid stSolid = mcSol->GetSolid();
                                Handle(TopTools_HSequenceOfShape) aSeq = new TopTools_HSequenceOfShape();

                                if(mcSol->HaveExt())
                                        aSeq->Append(mcSol->GetFaces());

                                if (aSeq->Length()> 0)
                                {
                                        for (Standard_Integer j = 1; j<= aSeq->Length(); j++)
                                        {
                                                TopoDS_Face aFace = TopoDS::Face(aSeq->Value(j));
                                                Standard_Real area = McCadMcVoid::AreaOfShape(aFace);
                                        //	cout << "\t\tarea:" << area << endl;
                                                if(aFace.IsNull() || area < myMinDecompositionFaceArea)
                                                        continue;

                                                McCadDiscDs_DiscFace theDFace;
                                                theDFace.SetFace(aFace);
                                                McCadDiscretization_Face theDiscer(theDFace);
                                                theDiscer.SetMinNbPnt(myMinNbPnt);
                                                theDiscer.SetMaxNbPnt(myMaxNbPnt);
                                                theDiscer.SetXresolution(myXlen);
                                                theDiscer.SetYresolution(myYlen);
                                                theDiscer.SetTolerance(myTolerance);
                                                theDiscer.Init();

                                                if(theDiscer.IsDone())
                                                        theResultants->Append(theDiscer.GetFace());
                                                        //theVCutFace->Append(theDiscer.GetFace());
                                                else
                                                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: Face discretization failed!!",
                                                                      McCadMessenger_ErrorMsg);
                                        }
                                }
                        }
                        else
                        {
                                msgr->Message("_#_McCadMcVoid_Generator.cxx :: Extending solid failed!!",
                                              McCadMessenger_ErrorMsg);
                                break;
                        }

                        if(myPI->UserBreak())
                                return;
                        myPI->Increment();
                }
                /////////////////////////////////////////////////////////////////////////////////////////
                // we delete quasi redundant faces
                // cout << "==================================================================" << endl;
                // cout << " Total Number of Solid =  " << myVSolids->Length() << endl;
                // cout << " Total Number of redundant Cut Faces =  " << theVCutFace->Length() << endl;

                ////////////////////////////////////////////////////////////////////////////////////////
                // Rank by surface area DECREASING
                cout << "Sorting cut faces : " << endl;

                for (int i=1; i<=theVCutFace->Length(); i++)
                {
                        Standard_Real iArea, jArea;
                        TopoDS_Face iFace = TopoDS::Face(theVCutFace->Value(i).GetFace());

                        iArea = McCadMcVoid::AreaOfShape(iFace);

                        for (int j=i+1; j<=theVCutFace->Length(); j++)
                        {
                                TopoDS_Face jFace = TopoDS::Face(theVCutFace->Value(j).GetFace());

                                jArea = McCadMcVoid::AreaOfShape(jFace);

                                if(jArea > iArea) // swap faces
                                {
                                        theVCutFace->Exchange(i,j);
                                        iArea = jArea;
                                }
                        }

                        cout << "." << flush;
                }
                cout << "Done\n";


                for(int i=1; i<=theResultants->Length(); i++)
                {
                        Standard_Real iArea, jArea;
                        TopoDS_Face iFace = TopoDS::Face(theResultants->Value(i).GetFace());
                        iArea = McCadMcVoid::AreaOfShape(iFace);
                        for(int j=i+1; j<=theResultants->Length(); j++)
                        {
                                TopoDS_Face jFace = TopoDS::Face(theResultants->Value(j).GetFace());
                                jArea = McCadMcVoid::AreaOfShape(jFace);

                                if(jArea > iArea) // swap faces
                                {
                                        theResultants->Exchange(i,j);
                                        iArea = jArea;
                                }
                        }
                }

                //remove non-planar faces
                for(int i=1; i<=theResultants->Length(); i++)
                {
                        TopoDS_Face iFace = TopoDS::Face(theResultants->Value(i).GetFace());
                        BRepAdaptor_Surface iBS(iFace,Standard_True);
                        GeomAdaptor_Surface iAS = iBS.Surface();
                        if(iAS.GetType() != GeomAbs_Plane)
                                theResultants->Remove(i--);
                }


               // now the faces in theVCutFaces are sorted by their decreasing area
               //  theVCutFace = McCadMcVoid::DeleteApproxRedundant(theVCutFace, myCFRes);

                if(myBSolid->Length() < 1)//If bounding box does not exist, create overall BB for all voxelized Solids
                {
                        cout << "\nCreating Bounding Box\n" << endl;
                        Bnd_Box bigBB;
                        for (Standard_Integer i = 1; i<= myVSolids->Length(); i++)
                        {
                                TopoDS_Solid bSol = TopoDS::Solid(myVSolids->Value(i).GetSolid());
                                BRepBndLib::Add(bSol,bigBB);
                                bigBB.SetGap(0.0);
                        }
                        Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
                        bigBB.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
                        myBSolid->Append(BRepPrimAPI_MakeBox(gp_Pnt(aXmin, aYmin, aZmin),gp_Pnt(aXmax, aYmax, aZmax)).Shape());
                }

                // start predecomposition using the Bsolid
                if(!havePredecomposed)
                        BoxSeq->Append(myBSolid);

                TopoDS_Shape theBox = myBSolid->Value(1);
                GProp_GProps GP1;
                BRepGProp::VolumeProperties(theBox,GP1);
                originalBoxSize = GP1.Mass();

/*STEPControl_Writer wrt;
for(int p=1; p<=theVCutFace->Length(); p++)
        wrt.Transfer(theVCutFace->Value(p).GetFace(), STEPControl_AsIs);
for(int p=1; p<=theResultants->Length(); p++)
        wrt.Transfer(theResultants->Value(p).GetFace(), STEPControl_AsIs);
wrt.Write("THECUTFACES.stp");*/


        cout << "\n\n##### LINEAR HALF SPACES #####" << endl;
                BoxSeq = GoDecompose(BoxSeq, theVCutFace, myMaxNbOfPreDecomposition);			//!!! run pre-decomposition
        cout << "##### LINEAR HALF SPACES FROM RESULTANTS #####" << endl;
                BoxSeq = GoDecompose(BoxSeq, theResultants, myMaxNbOfPreDecomposition);


                ///////////////////////////////////
                // perform cylindrical cuts, if environmental variable MCCAD_CYLINDRICALCUT is defined and set to 1
                if(getenv("MCCAD_CYLINDRICALCUT") != NULL)
                {
                        TCollection_AsciiString val(getenv("MCCAD_CYLINDRICALCUT"));
                        if(val.IsIntegerValue())
                        {
                                if(val.IntegerValue() == 1)
                                {
                                        //NONPLANAR
                                        /*
                                         * Use non-planar faces to cut predecomposed bounding box solids
                                         */

                                        cout << "\n\nNumber of planar cutted BOXES : " << BoxSeq->Length()  << endl;
                                        cout << "Performing second order cuts!!!\n\n";

                                        Standard_Real nbFaces(theNPFace->Length());
                                        myPI->SetStep(15./nbFaces);

                                        for(Standard_Integer npCnt=1; npCnt<=nbFaces; npCnt++) // for every non linear surface...
                                        {
                                                cout << "\t Face No.: " << npCnt << "  of  " << nbFaces << endl;
                                                TopoDS_Shape faceShape = theNPFace->Value(npCnt).GetFace();

                                                Handle(TColgp_HSequenceOfPnt) curPnts = theNPFace->Value(npCnt).GetPoints();	// get sample points on surf...
                                                for(Standard_Integer bxCnt=1; bxCnt<=BoxSeq->Length(); bxCnt++) // for every box solid ...
                                                {
                                                        Standard_Boolean hasInPoints(Standard_False);
                                                        TopoDS_Solid curSol = TopoDS::Solid(BoxSeq->Value(bxCnt));
                                                        BRepClass3d_SolidClassifier solidClassifier(curSol);
                                                        Standard_Integer inCnt(0);

                                                        for(Standard_Integer pntCnt = 1; pntCnt <= curPnts->Length(); pntCnt++)// for every sample point of current face, test if inside vsolid
                                                        {
                                                                solidClassifier.Perform(curPnts->Value(pntCnt),1e-7);

                                                                if(solidClassifier.State() == TopAbs_IN && solidClassifier.State() != TopAbs_OUT && solidClassifier.State() != TopAbs_ON)
                                                                {
                                                                        hasInPoints = Standard_True;
                                                                        inCnt++;
                                                                }
                                                        }

                                                        if(hasInPoints && Standard_Real(inCnt)/Standard_Real(curPnts->Length()) > 0.1)
                                                        {
                                                                /*STEPControl_Writer w;
                                                                w.Transfer(faceShape, STEPControl_AsIs);
                                                                w.Transfer(curSol, STEPControl_AsIs);


                                                                TopoDS_Compound newComp;
                                                                BRep_Builder compBuilder;
                                                                compBuilder.MakeCompound(newComp);

                                                                for(Standard_Integer j=1; j < curPnts->Length(); j++)
                                                                        compBuilder.Add(newComp, BRepBuilderAPI_MakeVertex(curPnts->Value(j)).Vertex());


                                                                w.Transfer(newComp, STEPControl_AsIs);

                                                                TCollection_AsciiString name("fc_");
                                                                name += npCnt;
                                                                name += ".stp";
                                                                w.Write(name.ToCString());*/

                                /*				cout << "IN Points :  " << inCnt << " of " << curPnts->Length() << " = " << Standard_Real(inCnt)/Standard_Real(curPnts->Length()) << endl;

                                //  03092009
                                                        // *
                                                         * this is a pre-"boolean-cut" test! if this test fails, it is most likely that the boolean operation fails or
                                                         * that it even causes the program to freeze.
                                                         * Put test in McCadMcVoid::HalfSpace and perform for non-planar surfaces only!!!
                                                         * */
                                                        TopoDS_Face theFace = TopoDS::Face(faceShape);

                                                        // make box
                                                        Bnd_Box tmpBox;
                                                        BRepBndLib::Add(curSol, tmpBox);
                                                        tmpBox.SetGap(10);
                                                        Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
                                                        tmpBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
                                                        gp_Pnt theUPnt = gp_Pnt(aXmax, aYmax, aZmax);
                                                        gp_Pnt theLPnt = gp_Pnt(aXmin, aYmin, aZmin);

                                                        Handle(TColStd_HSequenceOfReal) uVals = new TColStd_HSequenceOfReal;
                                                        Handle(TColStd_HSequenceOfReal) vVals = new TColStd_HSequenceOfReal;
                                                        TopoDS_Shape theBox = BRepPrimAPI_MakeBox(theLPnt,theUPnt).Shape();

                                                        // find extremal points for Face Creation
                                                        // we maximize the face, so it cuts the whole (void-)solid
                                                        TopLoc_Location loc;
                                                        Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(theFace,loc);
                                                        GeomAdaptor_Surface AS(theFaceSurface);
                                                        TopExp_Explorer ex;
                                                        gp_Pnt2d the2dPnt;
                                                        Standard_Real uMin, uMax, vMin, vMax;
                                                        ShapeAnalysis_Surface sAs(theFaceSurface);

                                                        for(ex.Init(theBox, TopAbs_VERTEX); ex.More(); ex.Next())
                                                        {
                                                                //get point
                                                                TopoDS_Vertex theVertex = TopoDS::Vertex(ex.Current());
                                                                gp_Pnt thePnt = BRep_Tool::Pnt(theVertex);

                                                                //project on surface
                                                                GeomAPI_ProjectPointOnSurf pntProj(thePnt, theFaceSurface);
                                                                for(int j=1; j<=pntProj.NbPoints(); j++)
                                                                {
                                                                        the2dPnt = sAs.ValueOfUV(pntProj.Point(j), 1e-3);
                                                                        uVals->Append(the2dPnt.X());
                                                                        vVals->Append(the2dPnt.Y());
                                                                }
                                                        }

                                                        // minimize and maximize
                                                        uMin = uVals->Value(1);
                                                        uMax = uVals->Value(1);
                                                        for(int i=2; i<=uVals->Length(); i++)
                                                        {
                                                                Standard_Real theVal = uVals->Value(i);

                                                                if(uMin > theVal)
                                                                        uMin = theVal;
                                                                if(uMax < theVal)
                                                                        uMax = theVal;
                                                        }

                                                        vMin = vVals->Value(1);
                                                        vMax = vVals->Value(1);
                                                        for(int i=2; i<=vVals->Length(); i++)
                                                        {
                                                                Standard_Real theVal = vVals->Value(i);
                                                                if(vMin > theVal)
                                                                        vMin = theVal;
                                                                if(vMax < theVal)
                                                                        vMax = theVal;
                                                        }

                                                        if(AS.GetType() == GeomAbs_Cylinder)
                                                        {
                                                                uMin = 0;
                                                                uMax = 2.*M_PI;
                                                        }
                                                        ///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef OCC650
                                                        TopoDS_Face F1 = BRepBuilderAPI_MakeFace(theFaceSurface, uMin, uMax, vMin, vMax);
#else
                                                        TopoDS_Face F1 = BRepBuilderAPI_MakeFace(theFaceSurface, uMin, uMax, vMin, vMax, 1.e-7);
#endif
                                                        F1.Orientation(TopAbs_FORWARD);

                                                        ///////////////////////////////////////////////////////////////////////////////////////////////////
                                                        // Build new solids

                                                        Standard_Boolean preTestFailed(Standard_False);
                                                        // if pretest failes bop will most likely fail, in some cases it will freez the whole prog., that's why we do this
                                                        // pretest.

                                                        for(TopExp_Explorer fExp(curSol, TopAbs_FACE); fExp.More(); fExp.Next())
                                                        {
                                                                TopoDS_Face curF = TopoDS::Face(fExp.Current());

                                                                IntTools_FaceFace ffInt;
                                                                ffInt.SetParameters(Standard_True, Standard_False, Standard_True, 1e-3);
                                                                ffInt.Perform(curF, F1);

                                                                if(!ffInt.IsDone())
                                                                {
                                                                        preTestFailed=Standard_True;
                                                                        break;
                                                                }

                                                                IntTools_SequenceOfCurves curves;
                                                                curves = ffInt.Lines();

                                                                Standard_Integer nbLines(0);
                                                                for(int i=1; i<=curves.Length(); i++)
                                                                        nbLines++;

                                                                if(nbLines < 1)
                                                                        continue;

                                                                if(nbLines > 2)
                                                                {
                                                                        cout << "NB LINES : " << nbLines << endl;

                                                                        preTestFailed = Standard_True;
                                                                        break;
                                                                }

                                                                for(int i=1; i<=curves.Length(); i++)
                                                                {
                                                                        IntTools_Curve intCurve = curves.Value(i);
                                                                        if(!intCurve.HasBounds())
                                                                        {
                                                                                preTestFailed = Standard_True;
                                                                                break;
                                                                        }
                                                                }
                                                        }

                                                        if(preTestFailed)
                                                        {
                                                                cout << "Pre-Boolean_Operation-Test failed\nSkipping this solid\n";
                                                                continue;
                                                        }

                                                        Handle(TopTools_HSequenceOfShape) tmpShp = new TopTools_HSequenceOfShape;
                                                        tmpShp->Append(curSol);

                                                        tmpShp = Decompose(tmpShp, faceShape);

                                // #
                                                        //	cout << "\t\t\tNumber Of Result Boxes : " << tmpShp->Length() << endl;
                                                                if(tmpShp->Length()>1) // decomposition successfull
                                                                {
                                                        //		cout << "CUT SUCCESSFUL : " << tmpShp->Length() << endl;
                                                                        Standard_Integer failCnt(0);
                                                                        Standard_Boolean isFailed(Standard_False);

                                                                        for(int k=1; k<=tmpShp->Length(); k++) //check again if cutted model is valid, append valid boxes to BoxSeq
                                                                        {
                                                                                TopoDS_Shape aSol = tmpShp->Value(k);

                                                                                for (TopExp_Explorer exs(aSol,TopAbs_SHELL);exs.More();exs.Next())
                                                                                {
                                                                                        const TopoDS_Shell& sh = TopoDS::Shell(exs.Current());
                                                                                        //////////////////////////////////////////////////////////////////////////
                                                                                        BRepCheck_Shell shellCheck(sh);
                                                                                        if (shellCheck.Closed(Standard_False) == BRepCheck_NoError)
                                                                                        {
                                                                                                BRepBuilderAPI_MakeSolid Bu(sh);
                                                                                                if(Bu.IsDone())
                                                                                                {
                                                                                                        TopoDS_Solid aSolid = Bu.Solid();

                                                                                                        if(aSolid.IsNull())
                                                                                                        {
                                                                                                                isFailed = Standard_True;
                                                                                                                msgr->Message("_#_McCadMcVoid_Generator.cxx :: Null Solid Resulted",
                                                                                                                              McCadMessenger_ErrorMsg);
                                                                                                        }
                                                                                                        else
                                                                                                        {
                                                                                                                BoxSeq->Append(aSolid);
                                                                                                                failCnt++;
                                                                                                        }
                                                                                                }
                                                                                                else
                                                                                                        cout << "# # # Could not build Solid!!!\n";
                                                                                        }
                                                                                        else
                                                                                                msgr->Message("_#_McCadMcVoid_Generator.cxx :: Model may be invalid!!",
                                                                                                              McCadMessenger_WarningMsg);
                                                                                }
                                                                        }

                                                                        if(isFailed)	// if resulting solids are invalid - remove already appended solids and keep original one
                                                                        {
                                                                                for(int l=1; l<=failCnt; l++)
                                                                                        BoxSeq->Remove(BoxSeq->Length());
                                                                        }
                                                                        else
                                                                                BoxSeq->Remove(bxCnt);
                                                                }
                                                                else
                                                                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: Cut Failed !!!",
                                                                                      McCadMessenger_WarningMsg);
                                                        }
                                                }
                                                myPI->Increment();
                                        }

                                        cout << "Total number of BOXES after second order cut : " << BoxSeq->Length()  << endl;
                                }
                        }
                }//#NONPLANAR
        }


        if(getenv("MCCAD_WRITE_DECOMPOSED") != NULL)
        {
                STEPControl_Writer wrt;
                for(int i=1; i<=BoxSeq->Length(); i++)
                        wrt.Transfer(BoxSeq->Value(i), STEPControl_AsIs);
                wrt.Write("theVVoids.stp");
        }


        /////////////////////////////////////////////////////////////////////////
        cout << "\nVoxelization of predecomposed voids" << endl;
        cout << "====================================" << endl;

        for (Standard_Integer i = 1; i<= BoxSeq->Length(); i++)//Discretize cutted solids
        {
                cout << "." << flush;
                TopoDS_Solid tmpSol = TopoDS::Solid(BoxSeq->Value(i));
                McCadDiscDs_DiscSolid aVSol;
                aVSol.SetSolid(tmpSol);
                aVSol.SetIndex(i);

                // voxelize
                McCadDiscretization_Solid theDiscer(aVSol);
                theDiscer.SetMinNbPnt(myMinNbPnt);
                theDiscer.SetMaxNbPnt(myMaxNbPnt);
                theDiscer.SetXresolution(myXlen);
                theDiscer.SetYresolution(myYlen);
                theDiscer.SetTolerance(myTolerance);
                theDiscer.Init();

                if(theDiscer.IsDone())
                {
                        myVVoids->Append(theDiscer.GetSolid()); //myVVoids contains now discretized cutted solids with planar faces
                }
                else
                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: Voxelization of a predecomposed void failed",
                                      McCadMessenger_ErrorMsg);
        }
        cout << "\n====================================" << endl;
        cout << "End voxelization of predecomposition Voids  \n" << endl;

        BoxSeq->Clear();
}


Standard_Boolean McCadMcVoid_Generator::IsPolyhedral(TopoDS_Shape& shp)
{
        for(TopExp_Explorer ex(shp, TopAbs_FACE); ex.More(); ex.Next())
        {
                TopoDS_Face face = TopoDS::Face(ex.Current());
                TopLoc_Location loc;
                Handle(Geom_Surface) geomSurf = BRep_Tool::Surface(face,loc);
                GeomAdaptor_Surface adaptorSurf(geomSurf);
                if(adaptorSurf.GetType() != GeomAbs_Plane)
                        return Standard_False;
        }

        return Standard_True;
}


Standard_Boolean McCadMcVoid_Generator::PolyhedralOverlap(TopoDS_Shape& shp1, TopoDS_Shape& shp2)
{
        /*
         * After sign-constant decomposition each polyhedral solid is convex.
         * For 2 convex polyhedra that do not overlap there is at least one
         * bounding surface of the two solids for which all vertices of the one solid
         * lie on one side and all vertices of the other solid lie on the other side.
         * If no such separating, bounding surface can be found the two solids must overlap.
         *
         * This method is supposed to be faster and more accurate than the "voxelized solids"-method!
         */

       Handle_TopTools_HSequenceOfShape vertex1Sequence = new TopTools_HSequenceOfShape;
       Handle_TopTools_HSequenceOfShape vertex2Sequence = new TopTools_HSequenceOfShape;
       Handle_TopTools_HSequenceOfShape faces1Sequence = new TopTools_HSequenceOfShape;
       Handle_TopTools_HSequenceOfShape faces2Sequence = new TopTools_HSequenceOfShape;

       // 1
       ///////////////////////////////////////////////
       for(TopExp_Explorer ex(shp1, TopAbs_VERTEX); ex.More(); ex.Next())
       {
           TopoDS_Vertex aVertex = TopoDS::Vertex(ex.Current());
           vertex1Sequence->Append(aVertex);
       }

       for(int i=1; i<=vertex1Sequence->Length(); i++)
       {
           for(int j=vertex1Sequence->Length(); j>i; j--)
           {
               if(vertex1Sequence->Value(i).IsSame(vertex1Sequence->Value(j)))
               {
                   vertex1Sequence->Remove(j);
                   i--;
                   break;
               }
           }
       }


       for(TopExp_Explorer ex(shp1, TopAbs_FACE); ex.More(); ex.Next())
       {
           TopoDS_Face aFace = TopoDS::Face(ex.Current());
           faces1Sequence->Append(aFace);
       }

       for(int i=1; i<=faces1Sequence->Length(); i++)
       {
           for(int j=faces1Sequence->Length(); j>i; j--)
           {
               if(faces1Sequence->Value(i).IsSame(faces1Sequence->Value(j)))
               {
                   faces1Sequence->Remove(j);
                   i--;
                   break;
               }
           }
       }


       // 2
       //////////////////////////////////////////////////
       for(TopExp_Explorer ex(shp2, TopAbs_VERTEX); ex.More(); ex.Next())
       {
           TopoDS_Vertex aVertex = TopoDS::Vertex(ex.Current());
           vertex2Sequence->Append(aVertex);
       }

      /* for(int i=1; i<=vertex2Sequence->Length(); i++)
       {
           for(int j=vertex2Sequence->Length(); j>i; j--)
           {
               if(vertex2Sequence->Value(i).IsSame(vertex2Sequence->Value(j)))
               {
                   vertex2Sequence->Remove(j);
                   i--;
                   break;
               }
           }
       }*/


       for(TopExp_Explorer ex(shp2, TopAbs_FACE); ex.More(); ex.Next())
       {
           TopoDS_Face aFace = TopoDS::Face(ex.Current());
           faces2Sequence->Append(aFace);
       }

       for(int i=1; i<=faces2Sequence->Length(); i++)
       {
           for(int j=faces2Sequence->Length(); j>i; j--)
           {
               if(faces2Sequence->Value(i).IsSame(faces2Sequence->Value(j)))
               {
                   faces2Sequence->Remove(j);
                   i--;
                   break;
               }
           }
       }


       /*cout << "STATS : \n======================\n\n";
       cout << vertex1Sequence->Length() << endl;
       cout << faces1Sequence->Length() << endl;
       cout << vertex2Sequence->Length() << endl;
       cout << faces2Sequence->Length() << endl << endl;

       STEPControl_Writer wrt;
       for(int i=1; i<=vertex2Sequence->Length(); i++)
           wrt.Transfer(vertex2Sequence->Value(i), STEPControl_AsIs);
       wrt.Transfer(shp1, STEPControl_AsIs);
       wrt.Write("model.stp");*/

       // exit(0);

       // 1 vs 2
       ///////////////////////////////////////
       for(int i=1; i<=faces1Sequence->Length(); i++)
       {
           TopoDS_Face curFace = TopoDS::Face(faces1Sequence->Value(i));
           TopLoc_Location loc;
           Handle(Geom_Surface) geomSurf = BRep_Tool::Surface(curFace,loc);
           GeomAdaptor_Surface adaptorSurf(geomSurf);
           int curSig(0);

           // determine in solid signum
           for(int j=1; j<=vertex1Sequence->Length(); j++)
           {
               TopoDS_Vertex curVertex = TopoDS::Vertex(vertex1Sequence->Value(j));
               gp_Pnt pnt = BRep_Tool::Pnt(curVertex);
               double val = McCadGTOOL::Evaluate(adaptorSurf, pnt);

               if(fabs(val) > 1.0e-6)
               {
                   curSig = int(val/fabs(val));
                   break;
               }
           }

           int breakCnt(0);
           bool isFace(false);

           if(curSig == 0)
               isFace = true;

           // test if solid has vertices with same signum
           for(int j=1; j<=vertex2Sequence->Length(); j++)
           {
               TopoDS_Vertex curVertex = TopoDS::Vertex(vertex2Sequence->Value(j));
               gp_Pnt pnt = BRep_Tool::Pnt(curVertex);
               double val = McCadGTOOL::Evaluate(adaptorSurf, pnt);

               if(fabs(val) > 1.0e-6)
               {
                   int sign = int(val/fabs(val));

                   if(isFace)
                   {
                       if(curSig == 0)
                       {
                           curSig = sign;
                           continue;
                       }
                       else if(sign != curSig)
                       {
                           breakCnt++;
                           break;
                       }
                   }
                   else
                   {
                       if(sign == curSig)
                       {
                           breakCnt++;
                           break;
                       }
                   }
               }
           }

           if(breakCnt == 0)
           {
               //cout << "No Collision 1\n";
               return false;
           }
       }

       // 2 vs 1
       //////////////////////////////////////
       for(int i=1; i<=faces2Sequence->Length(); i++)
       {
           TopoDS_Face curFace = TopoDS::Face(faces2Sequence->Value(i));
           TopLoc_Location loc;
           Handle(Geom_Surface) geomSurf = BRep_Tool::Surface(curFace,loc);
           GeomAdaptor_Surface adaptorSurf(geomSurf);
           int curSig(0);

           // determine in solid signum
           for(int j=1; j<=vertex2Sequence->Length(); j++)
           {
               TopoDS_Vertex curVertex = TopoDS::Vertex(vertex2Sequence->Value(j));
               gp_Pnt pnt = BRep_Tool::Pnt(curVertex);
               double val = McCadGTOOL::Evaluate(adaptorSurf, pnt);

               if(fabs(val) > 1.0e-6)
               {
                   curSig = int(val/fabs(val));
                   break;
               }
           }

           int breakCnt(0);
           bool isFace(false);

           if(curSig == 0)
               isFace = true;

           // test if solid has vertices with same signum
           for(int j=1; j<=vertex1Sequence->Length(); j++)
           {
               TopoDS_Vertex curVertex = TopoDS::Vertex(vertex1Sequence->Value(j));
               gp_Pnt pnt = BRep_Tool::Pnt(curVertex);
               double val = McCadGTOOL::Evaluate(adaptorSurf, pnt);

               if(fabs(val) > 1.0e-6)
               {
                   int sign = int(val/fabs(val));

                   if(isFace)
                   {
                       if(curSig == 0)
                       {
                           curSig = sign;
                           continue;
                       }
                       else if(sign != curSig)
                       {
                           breakCnt++;
                           break;
                       }
                   }
                   else
                   {
                       if(sign == curSig)
                       {
                           breakCnt++;
                           break;
                       }
                   }
               }
           }

           if(breakCnt == 0)
           {
               //cout << "No Collision 2\n";
               return false;
           }
       }


       return true;

        // check all vertices of solid 2 with all faces of solid 1
/*	for(TopExp_Explorer exF(shp2, TopAbs_FACE); exF.More(); exF.Next())
        {
           Standard_Integer sign(0);
           TopoDS_Face face2 = TopoDS::Face(exF.Current());

           TopLoc_Location loc;
           Handle(Geom_Surface) geomSurf = BRep_Tool::Surface(face2,loc);
           GeomAdaptor_Surface adaptorSurf(geomSurf);

           Standard_Integer breakCount(0);

           for(TopExp_Explorer exV(shp1, TopAbs_VERTEX); exV.More(); exV.Next())
           {
               TopoDS_Vertex vertex1 = TopoDS::Vertex(exV.Current());
               gp_Pnt thePnt = BRep_Tool::Pnt(vertex1);

               Standard_Real val = McCadGTOOL::Evaluate(adaptorSurf, thePnt);
               val = val/Abs(val);
               sign = Standard_Integer(val);

               // forward oriented face => all vertices of own solid are on negative side,
               // for reverse oriented face => the other way around
               if(face2.Orientation() == TopAbs_REVERSED)
               {
                   if(sign < -1.0e-6)
                       continue;
               }
               else if(sign > 1.0e-6)
                   continue;

               breakCount++;
               break;
           }

           // no break-call => no collision
           if(breakCount == 0)
               return Standard_False;
        }


        // check all faces of solid 1 with all vertices of solid 2
        for(TopExp_Explorer exF(shp1, TopAbs_FACE); exF.More(); exF.Next())
        {
           Standard_Integer sign(0);
           TopoDS_Face face1 = TopoDS::Face(exF.Current());

           TopLoc_Location loc;
           Handle(Geom_Surface) geomSurf = BRep_Tool::Surface(face1,loc);
           GeomAdaptor_Surface adaptorSurf(geomSurf);

           Standard_Integer breakCount(0);

           for(TopExp_Explorer exV(shp2, TopAbs_VERTEX); exV.More(); exV.Next())
           {
               TopoDS_Vertex vertex2 = TopoDS::Vertex(exV.Current());
               gp_Pnt thePnt = BRep_Tool::Pnt(vertex2);

               Standard_Real val = McCadGTOOL::Evaluate(adaptorSurf, thePnt);
               val = val/Abs(val);
               sign = Standard_Integer(val);

               if(face1.Orientation() == TopAbs_REVERSED)
               {
                   if(sign < -1.0e-6)
                       continue;
               }
               else if(sign > 1.0e-6)
                   continue;

               breakCount++;
               break;
           }

           // no break-call => no collision
           if(breakCount == 0)
                   return Standard_False;
        }

        // no sign-constant surface found? => collision
       return Standard_True;*/
}


//////////////////////////////////////////////////////////////////
/*++++++++++++++++++++  VOIDGENERATE  ++++++++++++++++++++++++++*/
//////////////////////////////////////////////////////////////////
// perform collision test between linearly cut void cells and decomposed input solids
void McCadMcVoid_Generator::VoidGenerate()
{
        /////////////////////////////////////////////////////////////////////////////////////////
        // append empty exp. after cell and outer seq. so that the output is smooth.
        McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
        myPI->SetStep(5./myVVoids->Length());

        Handle(TopTools_HSequenceOfShape) theFaceSeq = new TopTools_HSequenceOfShape();

        cout << "\nFinalize void generation  " << endl;
        cout << "Checking voids against solids  " << myVVoids->Length() << "  " << myVSolids->Length() << endl;
        cout << "====================================================== " << endl;

        Handle(TopTools_HSequenceOfShape) F1Seq = new TopTools_HSequenceOfShape();
        Handle(TopTools_HSequenceOfShape) F2Seq = new TopTools_HSequenceOfShape();

        for (int itv = 1; itv <= myVVoids->Length(); itv++)//For all voids
        {
                Standard_Boolean isErased = Standard_False;

//                cout << "Void : " << itv << " of " << myVVoids->Length() << endl;
                cout << "." << flush;

                myPI->Increment();
                if(myPI->UserBreak())
                        return;

                TopoDS_Solid aVoidSolid = TopoDS::Solid(myVVoids->Value(itv).GetSolid());					//Void-solids
                Standard_Real aVol = McCadMcVoid::VolumeOfShape(aVoidSolid);

                //cout << "\n" << aVol << "\n\n";
                BRepClass3d_SolidClassifier voidClassifier (aVoidSolid);

                McCadDiscDs_DiscSolid aDiscSolid = myVVoids->Value(itv);
                Bnd_Box aBB = aDiscSolid.GetBBox();
                if(aBB.IsWhole()) cout << "Is whole" << endl;
                if(aBB.IsVoid()) cout << "Is Void" << endl;
                if(aBB.IsXThin(1e-7)) cout << "Is xthin" << endl;
                if(aBB.IsYThin(1e-7)) cout << "Is ythin" << endl;
                if(aBB.IsZThin(1e-7)) cout << "Is zthin" << endl;
                if(aBB.IsThin(1e-7)) cout << "Is thin" << endl;

                Standard_Real bXmin, bYmin, bZmin, bXmax, bYmax, bZmax;
                aBB.Get(bXmin, bYmin, bZmin, bXmax, bYmax, bZmax);
                //cout << bXmin << "   " << bYmin<< "   " << bZmin<< "   " << bXmax<< "   " << bYmax<< "   " << bZmax << endl;
                gp_Pnt bPnt1(bXmin, bYmin, bZmin), bPnt2(bXmax, bYmax, bZmax);

                for (int its = 1; its <= myVSolids->Length(); its++)
                {
                //	cout<< "Checking Void number " << itv << " (" << myVVoids->Length() << ")" << " against solid number " << its << " of " << myVSolids->Length() << endl;

                        ///////////////////////////////////////////////////////////////////////////////////////////////////
                        Standard_Boolean inState = Standard_False;
                        TopoDS_Solid sSol = TopoDS::Solid(myVSolids->Value(its).GetSolid());		//material solid
                        Standard_Real sVol = McCadMcVoid::VolumeOfShape(sSol);
                        BRepClass3d_SolidClassifier solidClassifier (sSol);

                //////////////////////////////////////////////////////////////
                //bounding box based exclusion
                        Bnd_Box sBB; // = myVSolids->Value(its).GetBBox();
                        BRepBndLib::Add(myVSolids->Value(its).GetSolid(), sBB);
                        sBB.SetGap(0);

                        Standard_Real sXmin, sYmin, sZmin, sXmax, sYmax, sZmax;
                        sBB.Get(sXmin, sYmin, sZmin, sXmax, sYmax, sZmax);
                        gp_Pnt sPnt1(sXmin, sYmin, sZmin), sPnt2(sXmax, sYmax, sZmax);

                        if(aBB.IsOut(sBB)   &&  sBB.IsOut(aBB)   &&
                           aBB.IsOut(sPnt1) &&  aBB.IsOut(sPnt2) &&
                           sBB.IsOut(bPnt1) &&  sBB.IsOut(bPnt2)    )
                                continue;

                ////////////////////////////////////////////////////////////////////
                // identical faces but different orientation
                        Standard_Boolean found(Standard_False);
                        Standard_Boolean haveNonLinFace(Standard_False);
                        F1Seq->Clear();
                        F2Seq->Clear();

                        for (TopExp_Explorer exFF(sSol,TopAbs_FACE); exFF.More(); exFF.Next())
                        {
                                TopoDS_Face sF = TopoDS::Face(exFF.Current());
                                F2Seq->Append(sF);
                        }

                        for (TopExp_Explorer exF(aVoidSolid,TopAbs_FACE); exF.More(); exF.Next())
                        {
                                TopoDS_Face aF = TopoDS::Face(exF.Current());
                                F1Seq->Append(aF);

                                for(Standard_Integer i=1; i<=F2Seq->Length(); i++)
                                {
                                        TopoDS_Face sF = TopoDS::Face(F2Seq->Value(i));
                                        TopLoc_Location loc;
                                        Handle(Geom_Surface) gS2 = BRep_Tool::Surface(sF,loc);
                                        GeomAdaptor_Surface As2(gS2);

                                        if (As2.GetType() != GeomAbs_Plane)
                                        {
                                                haveNonLinFace = Standard_True;
                                                continue;
                                        }

                                        theFaceSeq->Clear();
                                        theFaceSeq->Append(aF);
                                        theFaceSeq->Append(sF);
                                        theFaceSeq = McCadMcVoid::DeleteOne(theFaceSeq);
                                        if (theFaceSeq->Length() < 2)
                                        {
                                                //cout << "Opposing faces\n";
                                                found = Standard_True;
                                                break;
                                        }
                                }

                                if(found) // voids are supposed to be all planar -- in further development this might change !!!
                                        break;
                        }

                        if(found)
                                continue;
                ///////////////////////////////////////////////////////////////////////
                // all surfaces are identical
                //	Handle(TopTools_HSequenceOfShape) nonIdenticalFaces = new TopTools_HSequenceOfShape();
                        Standard_Integer if2(0);
                        TopLoc_Location loc;

                        //cout << "\n\nSURFACE CHECK!!! : \n";

                        if(F1Seq->Length() == F2Seq->Length())
                        {
                                for(int ic = 1; ic <= F1Seq->Length(); ic++) //For all void-faces compare with solid faces
                                {
                                        TopoDS_Face F1 = TopoDS::Face(F1Seq->Value(ic));
                                        Handle(Geom_Surface) gS1 = BRep_Tool::Surface(F1,loc);
                                        GeomAdaptor_Surface As1(gS1);
                                        Handle(McCadCSGGeom_Surface) S1 = McCadMcVoid::MakeMcCadSurf(As1);
                                        S1->SetCasSurf(As1);

                                        for(int ic2 = 1; ic2 <= F2Seq->Length(); ic2++)
                                        {
                                                //cout << " faces : " << ic << " - " << ic2 << endl;
                                                TopoDS_Face F2 = TopoDS::Face(F2Seq->Value(ic2));
                                                Handle(Geom_Surface) gS2 = BRep_Tool::Surface(F2,loc);
                                                GeomAdaptor_Surface As2(gS2);

                                                if (As1.GetType() == As2.GetType())
                                                {
                                                        Handle(McCadCSGGeom_Surface) S2 = McCadMcVoid::MakeMcCadSurf(As2);
                                                        S2->SetCasSurf(As2);

                                                        if (S1->IsGeomEqual(S2))
                                                        {
                                                                if2++;
                                                        }
                                                }
                                        }

                                        if (F2Seq->Length() == if2) //if all faces of void and solid are the same, the void-solid must be deleted
                                        {
                                                //cout << "Void erased because of \"identical faces\"\n";
                                                myVVoids->Remove(itv);
                                                itv--;
                                                isErased = Standard_True;
                                                break;
                                        }
                                }

                                if(isErased)
                                        break; //the void is deleted!!!
                        }

                //////////////////////////////////////////////////////////////////////
                // vertex based check void against solid///
                        Standard_Boolean outState = Standard_False;

                        //For all void vertices check if any lies outside the current solid
                        for (TopExp_Explorer exV(aVoidSolid,TopAbs_VERTEX); exV.More(); exV.Next())
                        {
                                gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(exV.Current()));
                                solidClassifier.Perform(aP, 1.e-9); // 1.e-3 before 26082009

                                if (solidClassifier.State() == TopAbs_OUT )
                                {
                                        outState = Standard_True;
                                        break;
                                }
                        }

                        if(aVol - sVol <= 1.0e-5  && !outState)
                        {
                                Standard_Boolean deleteIt = Standard_True;
                                ///////////////////////////////////////////////////////////////////////////
                                if (haveNonLinFace)
                                        deleteIt = Standard_False;

                                if(deleteIt)
                                {
                                        //cout << "Void erased because of \"inclusion in solid\"\n";
                                        myVVoids->Remove(itv);
                                        itv--;
                                        isErased = Standard_True;
                                        break;
                                }
                        }

                        if(isErased)
                                break; //the void is deleted!!!


                        // check if void center of mass lies inside solid
                        GProp_GProps Props;
                        BRepGProp::VolumeProperties(aVoidSolid, Props);
                        gp_Pnt vCenterOfMass = Props.CentreOfMass();
                        solidClassifier.Perform(vCenterOfMass, 1e-9);

                        if(solidClassifier.State() == TopAbs_IN)
                        {
                                myVVoids->ChangeValue(itv).AppendOtherVSolid(its);
                                continue;
                        }


                        ///////////////////////////////////////////////////////////////////
                        // simplified overlap test for polyhedral solids -- the voids ARE polyhedral
                                if(!haveNonLinFace)
                                {
                                        if(PolyhedralOverlap(aVoidSolid, sSol)) // OVERLAP
                                                myVVoids->ChangeValue(itv).AppendOtherVSolid(its);
                                        continue;
                                }


                        // Voxel collision test
                        ///////////////////////////////////////////////////////////////////////
                        // forward check
                        Handle(McCadDiscDs_HSequenceOfDiscFace) vFaces = myVSolids->Value(its).GetOwnVFaces();
                        inState = Standard_False;
                        Standard_Boolean allBBout(Standard_True);

                        for(int ip = 1; ip <= vFaces->Length(); ip++)//For all Voxel Faces check for intersection
                        {
                                Bnd_Box fBB = vFaces->Value(ip).GetBBox();

                                if(aBB.IsOut(fBB))
                                        continue;

                                allBBout = Standard_False;

                        // this is forward check
                                Handle(TColgp_HSequenceOfPnt) aPntSeq = vFaces->Value(ip).GetPoints();
                                for (int k=1; k<=aPntSeq->Length();k++) //For all voxel points of current face
                                {
                                        gp_Pnt aP = aPntSeq->Value(k);

                                        if(aBB.IsOut(aP))
                                                continue;

                                        voidClassifier.Perform(aP, 1.0e-9);

                                        TopAbs_State theForwardState = voidClassifier.State();
                                        // before _MYTEST_ perform tolerance was 1e-7 and we only tested for in, not on.
                                        if(theForwardState == TopAbs_IN && theForwardState != TopAbs_ON) // MYTEST!!! Wed Jan 19 09:54:20 CET 2011
                                        {
                                                inState = Standard_True;
                                                break;
                                        }
                                }
                                if (inState)
                                        break;
                        }

                        // if no face bounding box collides with the void bounding box, continue
                        if(allBBout)
                                continue;

                        if (inState) //vanilla voxel based method: inclusion should be checked here!!!
                        {
                                //cout << "myVVoids collides with : " << its << " and ";
                                //for(int d=1; d<=myVVoids->Value(itv).GetOtherVSolids()->Length(); d++)
                                //	cout << myVVoids->Value(itv).GetOtherVSolids()->Value(d) << endl;
                                myVVoids->ChangeValue(itv).AppendOtherVSolid(its); //otherVSolids are excluded later during cell building
                                continue;
                        }


                        ///////////////////////////////////////////////////////////////////////////////////////////////////
                        // otherwise simple backward check due to small voids may be available !!
                        // this is necessery!!

                        vFaces = myVVoids->Value(itv).GetOwnVFaces();
                        for(int ip = 1; ip <= vFaces->Length(); ip++)//For all Voxel Faces check for intersection
                        {
                                Bnd_Box fBB = vFaces->Value(ip).GetBBox();

                                if(sBB.IsOut(fBB))
                                        continue;

                        // this is backward check
                                Handle(TColgp_HSequenceOfPnt) aPntSeq = vFaces->Value(ip).GetPoints();
                                for (int k=1; k<=aPntSeq->Length();k++) //For all voxel points of current face
                                {
                                        gp_Pnt aP = aPntSeq->Value(k);

                                        if(sBB.IsOut(aP))
                                                continue;

                                        solidClassifier.Perform(aP, 1.0e-9);

                                        TopAbs_State theBackState = solidClassifier.State();
                                        if(theBackState == TopAbs_IN && theBackState != TopAbs_ON) //
                                        {
                                                inState = Standard_True;
                                                break;
                                        }
                                }
                                if (inState)
                                        break;
                        }
                        if (inState) //vanilla voxel based method: inclusion should be checked here!!!
                        {
                                myVVoids->ChangeValue(itv).AppendOtherVSolid(its);
                                continue;
                        }
                        ///////////////////////////////////////////////////////////////////////////////////////////////
                }

                if(isErased)
                        continue;

                //////////////////////////////////////////////////////////////////////
                //MYTEST Test for monster voids and split further if monster is found
                //new implementation Wed Feb 15 16:38:43 CET 2004
                // We check for monster voids and decompose them further!!!
                Handle(TColStd_HSequenceOfInteger) compList = myVVoids->Value(itv).GetOtherVSolids();

                if (compList->Length() > myMaxComplementedShapes)
                {
                        //cout << " \nbegin... Processing Monster Void : " << endl;
                        cout << " Monster Void : number of complement solids = " << compList->Length() << endl;
                        Handle(TopTools_HSequenceOfShape) locSeq = new TopTools_HSequenceOfShape();

                        locSeq->Append(aVoidSolid); // append the void to it

                        Handle(McCadDiscDs_HSequenceOfDiscSolid) tmpVSol = new McCadDiscDs_HSequenceOfDiscSolid;
                        Handle(McCadDiscDs_HSequenceOfDiscFace) tmpVCutFace = new McCadDiscDs_HSequenceOfDiscFace;

                        for(int i = 1; i <= compList->Length(); i++)
                        {
                                int index = compList->Value(i);
                                tmpVSol->Append(myVSolids->Value(index));
                        }

                        for (int i = 1; i <= tmpVSol->Length(); i++)
                        {
                                TopoDS_Solid tmpSol = TopoDS::Solid(tmpVSol->Value(i).GetSolid());

                                Handle(McCadDiscDs_HSequenceOfDiscFace) lvFaces = tmpVSol->Value(i).GetOwnVFaces();

                                for (int j = 1; j <= lvFaces->Length(); j++)
                                {
                                        TopoDS_Face aF = lvFaces->Value(j).GetFace();
                                        BRepAdaptor_Surface Bs1(aF,Standard_True);
                                        GeomAdaptor_Surface AS = Bs1.Surface();
                                        if(AS.GetType() == GeomAbs_Plane)
                                        {
                                                Standard_Real area = McCadMcVoid::AreaOfShape(aF);

                                                if( area > myMinRedecompositionFaceArea)
                                                        tmpVCutFace->Append(lvFaces->Value(j));
                                        }
                                }
                                ///////////////////////////////////////////////////////////////////////////////
                                McCadCSGTool_Extender Mext(tmpSol);
                                if(Mext.IsDone())
                                {
                                        Handle(McCadTDS_ExtSolid) mcSol = Mext.ExtSolid();
                                        TopoDS_Solid stSolid = mcSol->GetSolid();
                                        Handle(TopTools_HSequenceOfShape) aSeq = new TopTools_HSequenceOfShape();
                                        if(mcSol->HaveExt()) aSeq->Append(mcSol->GetFaces());
                                        if (aSeq->Length()> 0)
                                        {
                                                for (Standard_Integer i = 1; i<= aSeq->Length(); i++)
                                                {
                                                        TopoDS_Face aFace = TopoDS::Face(aSeq->Value(i));

                                                        Standard_Real area = McCadMcVoid::AreaOfShape(aFace);

                                                        if(aFace.IsNull() || area < myMinRedecompositionFaceArea )
                                                                continue;

                                                        McCadDiscDs_DiscFace theDFace;
                                                        theDFace.SetFace(aFace);

                                                        McCadDiscretization_Face theDiscer(theDFace);
                                                        theDiscer.SetMinNbPnt(myMinNbPnt);
                                                        theDiscer.SetMaxNbPnt(myMaxNbPnt);
                                                        theDiscer.SetXresolution(myXlen);
                                                        theDiscer.SetYresolution(myYlen);
                                                        theDiscer.SetTolerance(myTolerance);
                                                        theDiscer.Init();
                                                        if(theDiscer.IsDone())
                                                        {
                                                                tmpVCutFace->Append(theDiscer.GetFace());
                                                        }
                                                        else
                                                                msgr->Message("_#_McCadMcVoid_Generator.cxx :: Face discretization failed!!",
                                                                              McCadMessenger_ErrorMsg);
                                                }
                                        }
                                }
                                else
                                {
                                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: Extending solid failed!!",
                                                      McCadMessenger_ErrorMsg);
                                        break;
                                }
                        }
                        /////////////////////////////////////////////////////////////////////////////////////////
                        // we delet quasi redundant faces and the extend
                        tmpVCutFace = McCadMcVoid::DeleteApproxRedundant(tmpVCutFace, myCFRes);
                        ////////////////////////////////////////////////////////////////////////////////////////

                        if(tmpVCutFace->Length() > 0)
                        {
                                int maxNbDec=5;
                                locSeq = GoDecompose(locSeq,tmpVCutFace,maxNbDec);
                        }
                        else
                        {
                                cout << " Number Cut Faces is zero =  " << tmpVCutFace->Length() << endl;
                                cout << " Monster Void remains unprocessed.. " << endl;
                        }

                        if(locSeq->Length() > 1)
                        {
                                cout << " Monster Void expanded to Voids:  " << locSeq->Length() << endl;

                                Handle(McCadDiscDs_HSequenceOfDiscSolid) tmpVVoid = new McCadDiscDs_HSequenceOfDiscSolid;
                                for (Standard_Integer i = 1; i<= locSeq->Length(); i++)
                                {

                                        TopoDS_Solid tmpSol = TopoDS::Solid(locSeq->Value(i));

                                        //check size of decomposed monster void
                                        GProp_GProps volProp;
                                        BRepGProp::VolumeProperties(tmpSol,volProp);
                                        if(volProp.Mass() < myMinVolume)
                                        {
                                                tmpVVoid->Clear();
                                                break;
                                        }

                                        McCadDiscDs_DiscSolid aVSol;
                                        aVSol.SetSolid(tmpSol);
                                        aVSol.SetIndex(i);
                                        // voxelize
                                        McCadDiscretization_Solid theDiscer(aVSol);
                                        theDiscer.SetMinNbPnt(myMinNbPnt);
                                        theDiscer.SetMaxNbPnt(myMaxNbPnt);
                                        theDiscer.SetXresolution(myXlen);
                                        theDiscer.SetYresolution(myYlen);
                                        theDiscer.SetTolerance(myTolerance);
                                        theDiscer.Init();
                                        if(theDiscer.IsDone())
                                        {
                                                tmpVVoid->Append(theDiscer.GetSolid());
                                        }
                                        else
                                                msgr->Message("_#_McCadMcVoid_Generator.cxx :: Voxelization of a predecomposition Void failed.",
                                                              McCadMessenger_ErrorMsg);

                                }

                                if(tmpVVoid->Length() > 1)
                                {
                                        myVVoids->Remove(itv);
                                        itv--; // delete the current monster void
                                        myVVoids->Append(tmpVVoid);
                                }
                        }
                        else if(aVol > myMinVolume) // make sure voids don't get too many and too small
                        {
                                // The idea was to cut monster voids along bisectioning planes of the bounding box
                                // the results are not good :( !!!

                                /*STEPControl_Writer wrt;
                                wrt.Transfer(aVoidSolid, STEPControl_AsIs);
                                for(int i=1; i<=tmpVSol->Length(); i++)
                                        wrt.Transfer(tmpVSol->Value(i).GetSolid(), STEPControl_AsIs);
                                wrt.Write("Monster.stp");
                                exit(-1);*/

/*				cout << "Performing bisectioning...\n";

                                STEPControl_Writer wrt;
                                for(int i=1; i<=tmpVSol->Length(); i++)
                                        wrt.Transfer(tmpVSol->Value(i).GetSolid(), STEPControl_AsIs);
                                wrt.Write("bisectioning.stp");


                                tmpVCutFace->Clear();

                                for(Standard_Integer i=1; i<=locSeq->Length(); i++) // max 1
                                {
                                        BRepBndLib boxExpander;
                                        Bnd_Box tmpBox;
                                        boxExpander.Add(locSeq->Value(i), tmpBox);

                                        Standard_Real aXMin, aYMin, aZMin, aXMax, aYMax, aZMax;
                                        tmpBox.SetGap(0.0);
                                        tmpBox.Get(aXMin, aYMin, aZMin, aXMax, aYMax, aZMax);

                                        // define points
                                        gp_Pnt p1, p2, p3, p4;

                                        // make planes
                                        // z half
                                        p1 = gp_Pnt(aXMax, aYMin, (aZMin+aZMax)/2.0 );
                                        p2 = gp_Pnt(aXMax, aYMax, (aZMin+aZMax)/2.0 );
                                        p3 = gp_Pnt(aXMin, aYMax, (aZMin+aZMax)/2.0 );
                                        p4 = gp_Pnt(aXMin, aYMin, (aZMin+aZMax)/2.0 );
                                        TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(p1, p2).Edge();
                                        TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(p2, p3).Edge();
                                        TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(p3, p4).Edge();
                                        TopoDS_Edge e4 = BRepBuilderAPI_MakeEdge(p4, p1).Edge();
                                        TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(e1, e2, e3, e4).Wire();
                                        TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aWire, Standard_True).Face();

                                        McCadDiscDs_DiscFace theDFace;
                                        theDFace.SetFace(aFace);

                                        McCadDiscretization_Face theDiscer(theDFace);
                                        theDiscer.SetMinNbPnt(myMinNbPnt);
                                        theDiscer.SetMaxNbPnt(myMaxNbPnt);
                                        theDiscer.SetXresolution(myXlen);
                                        theDiscer.SetYresolution(myYlen);
                                        theDiscer.SetTolerance(myTolerance);
                                        theDiscer.Init();

                                        if(theDiscer.IsDone())
                                                tmpVCutFace->Append(theDiscer.GetFace());
                                        else
                                                msgr.Error("_#_McCadMcVoid_Generator.cxx :: Face discretization failed!!");


                                        // y half
                                        p1 = gp_Pnt(aXMax, (aYMin+aYMax)/2.0, aZMin );
                                        p2 = gp_Pnt(aXMin, (aYMin+aYMax)/2.0, aZMin );
                                        p3 = gp_Pnt(aXMin, (aYMin+aYMax)/2.0, aZMax );
                                        p4 = gp_Pnt(aXMax, (aYMin+aYMax)/2.0, aZMax );
                                        e1 = BRepBuilderAPI_MakeEdge(p1, p2).Edge();
                                        e2 = BRepBuilderAPI_MakeEdge(p2, p3).Edge();
                                        e3 = BRepBuilderAPI_MakeEdge(p3, p4).Edge();
                                        e4 = BRepBuilderAPI_MakeEdge(p4, p1).Edge();
                                        aWire = BRepBuilderAPI_MakeWire(e1, e2, e3, e4).Wire();
                                        aFace = BRepBuilderAPI_MakeFace(aWire, Standard_True).Face();

                                        theDFace.SetFace(aFace);
                                        theDiscer.SetFace(theDFace);
                                        theDiscer.Init();

                                        if(theDiscer.IsDone())
                                                tmpVCutFace->Append(theDiscer.GetFace());
                                        else
                                                msgr.Error("_#_McCadMcVoid_Generator.cxx :: Face discretization failed!!");


                                        // x half
                                        p1  = gp_Pnt((aXMin+aXMax)/2.0, aYMin, aZMin );
                                        p2 = gp_Pnt((aXMin+aXMax)/2.0, aYMax, aZMin );
                                        p3 = gp_Pnt((aXMin+aXMax)/2.0, aYMax, aZMax );
                                        p4 = gp_Pnt((aXMin+aXMax)/2.0, aYMin, aZMax );
                                        e1 = BRepBuilderAPI_MakeEdge(p1, p2).Edge();
                                        e2 = BRepBuilderAPI_MakeEdge(p2, p3).Edge();
                                        e3 = BRepBuilderAPI_MakeEdge(p3, p4).Edge();
                                        e4 = BRepBuilderAPI_MakeEdge(p4, p1).Edge();
                                        aWire = BRepBuilderAPI_MakeWire(e1, e2, e3, e4).Wire();
                                        aFace = BRepBuilderAPI_MakeFace(aWire, Standard_True).Face();

                                        theDFace.SetFace(aFace);
                                        theDiscer.SetFace(theDFace);
                                        theDiscer.Init();

                                        if(theDiscer.IsDone())
                                                tmpVCutFace->Append(theDiscer.GetFace());
                                        else
                                                msgr.Error("_#_McCadMcVoid_Generator.cxx :: Face discretization failed!!");
                                }

                                int maxNbDec=3;
                                locSeq = GoDecompose(locSeq,tmpVCutFace,maxNbDec);

                                if(locSeq->Length() > 1)
                                {
                                        cout << " Monster Void expanded to Voids:  " << locSeq->Length() << endl;

                                        Handle(McCadDiscDs_HSequenceOfDiscSolid) tmpVVoid = new McCadDiscDs_HSequenceOfDiscSolid;
                                        for (Standard_Integer i = 1; i<= locSeq->Length(); i++)
                                        {

                                                TopoDS_Solid tmpSol = TopoDS::Solid(locSeq->Value(i));

                                                //check size of decomposed monster void
                                                GProp_GProps volProp;
                                                BRepGProp::VolumeProperties(tmpSol,volProp);
                                                if(volProp.Mass() < myMinVolume)
                                                {
                                                        tmpVVoid->Clear();
                                                        break;
                                                }

                                                McCadDiscDs_DiscSolid aVSol;
                                                aVSol.SetSolid(tmpSol);
                                                aVSol.SetIndex(i);
                                                // voxelize
                                                McCadDiscretization_Solid theDiscer(aVSol);
                                                theDiscer.SetMinNbPnt(myMinNbPnt);
                                                theDiscer.SetMaxNbPnt(myMaxNbPnt);
                                                theDiscer.SetXresolution(myXlen);
                                                theDiscer.SetYresolution(myYlen);
                                                theDiscer.SetTolerance(myTolerance);
                                                theDiscer.Init();
                                                if(theDiscer.IsDone())
                                                {
                                                        tmpVVoid->Append(theDiscer.GetSolid());
                                                }
                                                else
                                                        msgr.Error("_#_McCadMcVoid_Generator.cxx :: Voxelization of a predecomposition Void failed.");

                                        }

                                        if(tmpVVoid->Length() > 1)
                                        {
                                                myVVoids->Remove(itv);
                                                itv--; // delete the current monster void
                                                myVVoids->Append(tmpVVoid);
                                        }
                                }
                                else
                                        msgr.Warning("_#_McCadMcVoid_Generator.cxx :: Can't expand Monster Void!");
*/
                                msgr->Message("_#_McCadMcVoid_Generator.cxx :: Can't expand Monster Void!",
                                              McCadMessenger_WarningMsg);
                        }
                        cout << endl << locSeq->Length() << endl;
                        cout << " end... Processing Monster Void ...end " << endl;
                }

        }

        if(getenv("MCCAD_WRITE_VOIDS") != NULL)
        {

                STEPControl_Writer stpWrtr;
                for(int p=1; p<=myVVoids->Length(); p++)
                        stpWrtr.Transfer(myVVoids->Value(p).GetSolid(), STEPControl_AsIs);

                stpWrtr.Write("mc_Void_Cells.stp");
        }

        cout<< "\n====================================================== " << endl;
        cout<< "End checking voids against solids  \n" << endl;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


void McCadMcVoid_Generator::VoidVoxelization(Handle(TopTools_HSequenceOfShape)& voidSolids)
{

}

//Standard_Boolean McCadMcVoid_Generator::FacesIntersect(TopoDS_Shape shp1, TopoDS_Shape shp2)
Standard_Boolean McCadMcVoid_Generator::FacesIntersect(TopoDS_Shape& shp1, TopoDS_Shape& shp2)
{
        Standard_Boolean collides(Standard_False);
        TopOpeBRep_ShapeIntersector shapeIntersector;
        IntTools_FaceFace ffIntersector;

        for(TopExp_Explorer faceExp1(shp1, TopAbs_FACE); faceExp1.More(); faceExp1.Next())
        {
                TopoDS_Face face1 = TopoDS::Face(faceExp1.Current());

                for(TopExp_Explorer faceExp2(shp2, TopAbs_FACE); faceExp2.More(); faceExp2.Next())
                {
                        TopoDS_Face face2 = TopoDS::Face(faceExp2.Current());

                        shapeIntersector.InitIntersection(face1, face2);

                        if(shapeIntersector.MoreIntersection())
                        {
                                //cout << "\n\n----------------------------------\nCollision Detected\n----------------------------------\n\n";
                                if(shapeIntersector.ChangeFacesIntersector().IsEmpty())
                                {
                                        // test if faces overlapp
                                        // voxelize faces
                                        TopoDS_Face theFace(TopoDS::Face(face1));
                                        Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt();
                                        BRepClass_FaceClassifier bsc3d;

                                        BRepAdaptor_Surface BS(theFace, Standard_True);
                                        GeomAdaptor_Surface theASurface = BS.Surface();

                                        Standard_Real uMin, uMax, vMin, vMax, uMean, vMean;
                                        uMin = theASurface.FirstUParameter();
                                        uMax = theASurface.LastUParameter();
                                        vMin = theASurface.FirstVParameter();
                                        vMax = theASurface.LastVParameter();
                                        uMean = (uMin + uMax)/2.0;
                                        vMean = (vMin + vMax)/2.0;

                                        BRepAdaptor_Curve2d curveAdaptor;
                                        for(TopExp_Explorer edgeExp(theFace, TopAbs_EDGE); edgeExp.More(); edgeExp.Next())
                                        {
                                                curveAdaptor.Initialize(TopoDS::Edge(edgeExp.Current()), theFace);
                                                Standard_Real frstParam, lstParam;
                                                frstParam = (!(Precision::IsNegativeInfinite(curveAdaptor.FirstParameter()))) ? curveAdaptor.FirstParameter() : -1.0;
                                                lstParam  = (!(Precision::IsPositiveInfinite(curveAdaptor.LastParameter())))  ? curveAdaptor.LastParameter()  :  1.0;

                                                // ignore edges
                                                frstParam += 1.0e-5;
                                                lstParam  -= 1.0e-5;

                                                for (int i=1; i<=100; i++)
                                                {
                                                        Standard_Real a = Standard_Real(i)/100.;
                                                        Standard_Real par = (1.0-a)*frstParam + a*lstParam;
                                                        gp_Pnt2d p2 = curveAdaptor.Value(par);
                                                        Standard_Real U = p2.X();
                                                        Standard_Real V = p2.Y();

                                                        // exclude edges
                                                        if(U < uMean)
                                                                U += 1.e-5;
                                                        else if(U > uMean)
                                                                U -= 1.e-5;
                                                        if(V < vMean)
                                                                V += 1.e-5;
                                                        else if(V > vMean)
                                                                V -= 1.e-5;

                                                        gp_Pnt p1;
                                                        theASurface.D0(U, V, p1);

                                                        bsc3d.Perform(theFace, p2, myTolerance);

                                                        if (bsc3d.State() == TopAbs_IN || bsc3d.State() == TopAbs_ON)
                                                                thePntSeq->Append(p1);
                                                }

                                                theFace = TopoDS::Face(face2);

                                                int cnt(0);
                                                for(int i=1; i<=thePntSeq->Length(); i++)
                                                {
                                                        bsc3d.Perform(theFace, thePntSeq->Value(i), 1.0e-7);
                                                        if( bsc3d.State() == TopAbs_IN )
                                                        {
                                                                cnt++;
                                                                if(cnt >= 3)
                                                                        break;
                                                        }
                                                }

                                                if(cnt >= 3) // overlapp !!!
                                                {
                                                        collides = true;
                                                        break;
                                                }
                                        }
                                }
                                else
                                {
                                        ffIntersector.Perform(face1, face2);

                                        if(!ffIntersector.IsDone())
                                                return Standard_True;

                                        if(ffIntersector.Lines().Length() < 1)
                                                return Standard_False;

                                /*	IntTools_SequenceOfCurves intCurves;
                                        intCurves = ffIntersector.Lines();

                                        cout << "Number of Intersection Lines : " << intCurves.Length() << endl;

                                        Standard_Boolean notAnEdge(Standard_False);

                                        for(Standard_Integer i=1; i<=intCurves.Length(); i++)
                                        {
                                                Standard_Boolean hitEdge(Standard_False);
                                                IntTools_Curve curIntCurve = intCurves.Value(i);
                                                Standard_Real first, last;
                                                gp_Pnt pnt_f, pnt_l;
                                                curIntCurve.Bounds(first, last, pnt_f, pnt_l);
                                                Handle(Geom_Curve) aCurve = curIntCurve.Curve();

                                                TopoDS_Edge intEdge = BRepBuilderAPI_MakeEdge(aCurve, pnt_f, pnt_l).Edge();

                                                for(TopExp_Explorer edgeExp1(face1, TopAbs_EDGE); edgeExp1.More(); edgeExp1.Next())
                                                {
                                                        if(TopoDS::Edge(edgeExp1.Current()).IsSame(intEdge))
                                                        {
                                                                hitEdge = Standard_True;
                                                                break;
                                                        }
                                                }

                                                if(hitEdge)
                                                {
                                                        cout << "HIT EDGE\n";
                                                        continue;
                                                }

                                                for(TopExp_Explorer edgeExp2(face2, TopAbs_EDGE); edgeExp2.More(); edgeExp2.Next())
                                                {
                                                        if(TopoDS::Edge(edgeExp2.Current()).IsSame(intEdge))
                                                        {
                                                                hitEdge = Standard_True;
                                                                break;
                                                        }
                                                }

                                                if(hitEdge)
                                                {
                                                        cout << "HIT EDGE\n";
                                                        return Standard_False;
                                                }
                                        }


                                        cout << "TRUE THE HARD WAY\n\n";*/

                                        collides = true;
                                        break;
                                }
                        }
                }

                if(collides)
                        break;
        }




        /*
        Handle(TColgp_HSequenceOfPnt) solidVoxelPoints = solidDS.GetVoxel();
        Handle(TColgp_HSequenceOfPnt) voidVoxelPoints = voidDS.GetVoxel();

        // without forward check goes here!!!! Wed Nov 16 17:20:27 CET 2005
        ///////////////////////////////////////////////////////////////////////
        // forward check
        BRepClass3d_SolidClassifier solidClassifier (solidDS.GetSolid());
        BRepClass3d_SolidClassifier voidClassifier  (voidDS.GetSolid());

        for(Standard_Integer i=1; i<=solidVoxelPoints->Length(); i++)
        {
                voidClassifier.Perform(solidVoxelPoints->Value(i), 1.0e-7);
                if(voidClassifier.State() == TopAbs_IN)
                        return Standard_True;
        }

        for(Standard_Integer i=1; i<=voidVoxelPoints->Length(); i++)
        {
                solidClassifier.Perform(voidVoxelPoints->Value(i), 1.0e-7);
                if(solidClassifier.State() == TopAbs_IN)
                        return Standard_True;
        }

        return Standard_False;*/

                                ///////////////////////////////////////////////////////////////////////////////////////////////



/*	Handle(TColgp_HSequenceOfPnt) seamPointsOfSolid1 = new TColgp_HSequenceOfPnt();
        Handle(TColgp_HSequenceOfPnt) seamPointsOfSolid2 = new TColgp_HSequenceOfPnt();

        BRepClass_FaceClassifier bfc3d;

        BRepAdaptor_Curve2d curveAdaptor;

        for(TopExp_Explorer faceExp1(shp1, TopAbs_FACE); faceExp1.More(); faceExp1.Next())
        {
                TopoDS_Face face1 = TopoDS::Face(faceExp1.Current());
                BRepAdaptor_Surface BS(face1, Standard_True);
                GeomAdaptor_Surface theASurface = BS.Surface();

                Standard_Real uMin, uMax, vMin, vMax, uMean, vMean;
                uMin = theASurface.FirstUParameter();
                uMax = theASurface.LastUParameter();
                vMin = theASurface.FirstVParameter();
                vMax = theASurface.LastVParameter();
                uMean = (uMin + uMax)/2.0;
                vMean = (vMin + vMax)/2.0;

                for(TopExp_Explorer edgeExp(face1, TopAbs_EDGE); edgeExp.More(); edgeExp.Next())
                {
                        curveAdaptor.Initialize(TopoDS::Edge(edgeExp.Current()), face1);
                        Standard_Real frstParam, lstParam;
                        frstParam = (!(Precision::IsNegativeInfinite(curveAdaptor.FirstParameter()))) ? curveAdaptor.FirstParameter() : -1.0;
                        lstParam  = (!(Precision::IsPositiveInfinite(curveAdaptor.LastParameter())))  ? curveAdaptor.LastParameter()  :  1.0;

                        // ignore edges
                //	frstParam += 1.0e-5;
                //	lstParam  -= 1.0e-5;

                        for (int i=1; i<=100; i++)
                        {
                                Standard_Real a = Standard_Real(i)/100.;
                                Standard_Real par = (1.0-a)*frstParam + a*lstParam;
                                gp_Pnt2d p2 = curveAdaptor.Value(par);
                                Standard_Real U = p2.X();
                                Standard_Real V = p2.Y();

                                // exclude edges
                                if(U < uMean)
                                        U += 1.e-5;
                                else if(U > uMean)
                                        U -= 1.e-5;
                                if(V < vMean)
                                        V += 1.e-5;
                                else if(V > vMean)
                                        V -= 1.e-5;

                                gp_Pnt p1;
                                theASurface.D0(U, V, p1);

                                bfc3d.Perform(face1, p2, myTolerance);

                                if (bfc3d.State() == TopAbs_IN || bfc3d.State() == TopAbs_ON)
                                        seamPointsOfSolid1->Append(p1);
                        }
                }

                for(TopExp_Explorer faceExp2(shp2, TopAbs_FACE); faceExp2.More(); faceExp2.Next())
                {
                        TopoDS_Face face2 = TopoDS::Face(faceExp2.Current());
                        BRepAdaptor_Surface BS(face2, Standard_True);
                        GeomAdaptor_Surface theASurface = BS.Surface();

                        Handle(TColgp_HSequenceOfPnt) seamVoxelPntsOfFace2 = new TColgp_HSequenceOfPnt();

                        for(TopExp_Explorer edgeExp(face1, TopAbs_EDGE); edgeExp.More(); edgeExp.Next())
                        {
                                curveAdaptor.Initialize(TopoDS::Edge(edgeExp.Current()), face2);
                                Standard_Real frstParam, lstParam;
                                frstParam = (!(Precision::IsNegativeInfinite(curveAdaptor.FirstParameter()))) ? curveAdaptor.FirstParameter() : -1.0;
                                lstParam  = (!(Precision::IsPositiveInfinite(curveAdaptor.LastParameter())))  ? curveAdaptor.LastParameter()  :  1.0;

                                // ignore edges
                        //	frstParam += 1.0e-5;
                        //	lstParam  -= 1.0e-5;

                                for (int i=1; i<=100; i++)
                                {
                                        Standard_Real a = Standard_Real(i)/100.;
                                        Standard_Real par = (1.0-a)*frstParam + a*lstParam;
                                        gp_Pnt2d p2 = curveAdaptor.Value(par);
                                        Standard_Real U = p2.X();
                                        Standard_Real V = p2.Y();

                                        // exclude edges
                                        if(U < uMean)
                                                U += 1.e-5;
                                        else if(U > uMean)
                                                U -= 1.e-5;
                                        if(V < vMean)
                                                V += 1.e-5;
                                        else if(V > vMean)
                                                V -= 1.e-5;

                                        gp_Pnt p1;
                                        theASurface.D0(U, V, p1);

                                        bfc3d.Perform(face2, p2, myTolerance);

                                        if (bfc3d.State() == TopAbs_IN || bfc3d.State() == TopAbs_ON)
                                                seamPointsOfSolid2->Append(p1);
                                }
                        }
                }
        }


        // classify seam points
        BRepClass3d_SolidClassifier solidClassifier;
        solidClassifier.Load(shp2);

        for(Standard_Integer i=1; i<=seamPointsOfSolid1->Length(); i++)
        {
                solidClassifier.Perform(seamPointsOfSolid1->Value(i), 1.e-6);

                if(solidClassifier.State() == TopAbs_IN)
                        return Standard_True;
        }

        solidClassifier.Load(shp1);

        for(Standard_Integer i=1; i<=seamPointsOfSolid2->Length(); i++)
        {
                if(solidClassifier.State() == TopAbs_IN)
                        return Standard_True;
        }
*/


/*			ffIntersector.Perform(face1, face2);

                                if(!ffIntersector.IsDone()) // something went wrong
                                        return Standard_True;


                                if(ffIntersector.Lines().Length() > 0)
                                {

                                        IntTools_SequenceOfCurves intCurves;
                                        intCurves = ffIntersector.Lines();

                                        cout << "Number of Intersection Lines : " << intCurves.Length() << endl;

                                        Standard_Boolean notAnEdge(Standard_False);

                                        for(Standard_Integer i=1; i<=intCurves.Length(); i++)
                                        {
                                                Standard_Boolean hitEdge(Standard_False);
                                                IntTools_Curve curIntCurve = intCurves.Value(i);
                                                Standard_Real first, last;
                                                gp_Pnt pnt_f, pnt_l;
                                                curIntCurve.Bounds(first, last, pnt_f, pnt_l);
                                                Handle(Geom_Curve) aCurve = curIntCurve.Curve();

                                                TopoDS_Edge intEdge = BRepBuilderAPI_MakeEdge(aCurve, pnt_f, pnt_l).Edge();

                                                STEPControl_Writer wrt;
                                                wrt.Transfer(intEdge, STEPControl_AsIs);
                                                wrt.Transfer(face1, STEPControl_AsIs);
                                                wrt.Transfer(face2, STEPControl_AsIs);
                                                wrt.Transfer(shp1, STEPControl_AsIs);
                                                wrt.Transfer(shp2, STEPControl_AsIs);
                                                wrt.Write("collision.stp");
                                                exit(-1);

                                                for(TopExp_Explorer edgeExp1(face1, TopAbs_EDGE); edgeExp1.More(); edgeExp1.Next())
                                                {
                                                        if(TopoDS::Edge(edgeExp1.Current()).IsSame(intEdge))
                                                        {
                                                                hitEdge = Standard_True;
                                                                break;
                                                        }
                                                }

                                                if(hitEdge)
                                                {
                                                        cout << "HIT EDGE\n";
                                                        continue;
                                                }

                                                for(TopExp_Explorer edgeExp2(face2, TopAbs_EDGE); edgeExp2.More(); edgeExp2.Next())
                                                {
                                                        if(TopoDS::Edge(edgeExp2.Current()).IsSame(intEdge))
                                                        {
                                                                hitEdge = Standard_True;
                                                                break;
                                                        }
                                                }

                                                if(!hitEdge)
                                                {
                                                        return Standard_True;
                                                }
                                                else
                                                        cout << "Hit Edge\n";
                                        }
                                }
                                else
                                        return Standard_False; // no lines, no intersections, right? ...
*/


        return collides;
}



//////////////////////////////////////////////////////////////////
/*+++++++++++++++++++++  GODECOMPOSE  ++++++++++++++++++++++++++*/
//////////////////////////////////////////////////////////////////
Handle(TopTools_HSequenceOfShape) McCadMcVoid_Generator::GoDecompose(Handle(TopTools_HSequenceOfShape)& BoxSeq,
                                                                    const Handle(McCadDiscDs_HSequenceOfDiscFace)& theVCutFace,
                                                                    const Standard_Integer MaxNbVoids                           )
{
   McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();

   cout << "\nPerforming Decomposition\n=========================" << endl;
   myPI->SetStep(30./theVCutFace->Length());

   Standard_Real numOfCutFaces = theVCutFace->Length();


// PERFORM CUTTING FOR ALL CUT FACES
//////////////////////////////////////
   for (int fit = 1; fit <= numOfCutFaces; fit++)
   {
       if (BoxSeq->Length() >= MaxNbVoids )
       {
           // this happens if saturation is achived ahead of end of the cutseq! legitimate situation
           cout << "   Cut processing is finished due to reaching maximum Number of Decomposition at :   " << fit << endl;
           cout << "   maximum Number of Decompositions: " << MaxNbVoids << endl;
           break;
       }

       if (BoxSeq->Length() == 0)
       {
           // this happens if saturation is achived ahead of end of the cutseq! legitimate situation
           cout << "************************************************************************************* " << endl;
           cout << "   Cut Processing is finished due to saturation at cut sequence number   " << fit << endl;
           cout << "************************************************************************************* " << endl;
       }

       cout << "." << flush;
//       cout << " Start Processing Face   " << fit << " of " << numOfCutFaces << endl;

       TopoDS_Shape CutFaceSolid = theVCutFace->Value(fit).GetFace();
       Bnd_Box theBB = theVCutFace->Value(fit).GetBBox();

       Handle(TopTools_HSequenceOfShape) tmpBoxSeq = new TopTools_HSequenceOfShape();
       Handle(TopTools_HSequenceOfShape) locBoxSeq = new TopTools_HSequenceOfShape();

   //  search for sign changing cut faces
   ////////////////////////////////////////
       for (Standard_Integer j=1; j<= BoxSeq->Length(); j++)//For all Shapes in BoxSeq
       {
           TopoDS_Solid aSol = TopoDS::Solid(BoxSeq->Value(j));

           Bnd_Box aBB;
           BRepBndLib::Add(aSol,aBB);
           aBB.SetGap(0.0);
           if(aBB.IsOut(theBB) && theBB.IsOut(aBB))
           {
               locBoxSeq->Append(aSol);
               continue;
           }

           ////////////////////////////////////////////////////////////////////////////////
           // if identical surface as bounding surface of void -> skip cutting
       /*    bool isSame(false);
           for(TopExp_Explorer ex(aSol, TopAbs_FACE); ex.More(); ex.Next())
           {
               Handle(TopTools_HSequenceOfShape) shapes;
               TopoDS_Face curFace = TopoDS::Face(ex.Current());

               shapes->Append(curFace);
               shapes->Append(CutFaceSolid);
               shapes = McCadMcVoid::DeleteOne(shapes);

               if(shapes->Length() != 2)
               {
                   locBoxSeq->Append(aSol);
                   isSame = true;
                   break;
               }
           }
           if(isSame)
               continue;*/

//		cout << j << "<<<<<<<" << endl;
//TODO more specific testing if plane interferes with solid volume
/*		BRepClass3d_SolidClassifier solidClassifier (aSol);
           Standard_Integer inPnts(0), outPnts(0);

           for(TopExp_Explorer vExp(CutFaceSolid); vExp.More(); vExp.Next())
           {
                   TopoDS_Vertex curV = TopoDS::Vertex(vExp.Current());
                   gp_Pnt curPnt = BRep_Tool::Pnt(curV);
                   // check for all vertices!!!

                   solidClassifier.Perform(curPnt, 1.0e-7);
                   if(solidClassifier.State() == TopAbs_IN && solidClassifier.State() != TopAbs_ON && solidClassifier.State() != TopAbs_OUT)
                           inPnts++;
                   else
                           outPnts++;
           }

           if(inPnts == 0)
           {
                   locBoxSeq->Append(aSol);
                   cout << "CONTINUE\n";
                   continue;
           }*/
//#TODO
           /////////////////////////////////////////////////////////////////////


           // if (McCadMcVoid::SignChanging(aSol, theVCutFace->Value(fit)))
           if (PolyhedralOverlap(CutFaceSolid, aSol))
               tmpBoxSeq->Append(aSol);
           else
               locBoxSeq->Append(aSol);
       }

       BoxSeq->Clear();
       BoxSeq->Append(locBoxSeq);
       ////////////////////////////////////////////////////////////////////////////////////////////


       if(tmpBoxSeq->Length() == 0)
       {
//           cout << " End Processing face   " << fit << "\n" << endl;
           continue;
       }


   // CALL DECOMPOSE
   //////////////////////////////////////////
       Handle(TopTools_HSequenceOfShape) uniBoxSeq = Decompose(tmpBoxSeq,CutFaceSolid);	//cut tmpBoxSeq, along CutFaceSolid and return resulting solid

   //check again if cutted model is valid, append valid boxes to BoxSeq
   ///////////////////////////////////////////////////////////////////////////////////////////////
       for(int k=1; k<=uniBoxSeq->Length(); k++)
       {
           TopoDS_Shape aSol = uniBoxSeq->Value(k);
           for (TopExp_Explorer exs(aSol,TopAbs_SHELL);exs.More();exs.Next())
           {
               const TopoDS_Shell& sh = TopoDS::Shell(exs.Current());
               //////////////////////////////////////////////////////////////////////////
               BRepCheck_Shell shellCheck(sh);
               if (shellCheck.Closed(Standard_False) == BRepCheck_NoError)
               {
                   BRepBuilderAPI_MakeSolid Bu(sh);
                   if(Bu.IsDone())
                   {
                       TopoDS_Solid aSolid = Bu.Solid();
                       if(!aSolid.IsNull())
                            BoxSeq->Append(aSolid);
                   }
               }
               else
                    msgr->Message("Model may be invalid!! ",
                                  McCadMessenger_WarningMsg);
           }
       }

//       cout << " End Processing face   " << fit << "\n" << endl;

       if(myPI->UserBreak())
               return NULL;
       myPI->Increment();
   }
   cout << "\n=========================\nDecomposition finished\n" << endl;

   return BoxSeq;
}


//////////////////////////////////////////////////////////////////
/*+++++++++++++++++++++++  DECOMPOSE  ++++++++++++++++++++++++++*/
//////////////////////////////////////////////////////////////////

Handle(TopTools_HSequenceOfShape) McCadMcVoid_Generator::Decompose(Handle(TopTools_HSequenceOfShape)& tmpBoxSeq, TopoDS_Shape& aFaceSolid)
{
        // the heart of void the generation algorithm!!
        // keep it simple and robust; it should work thousandfold !!
        McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();

        Handle(TopTools_HSequenceOfShape) uniBoxSeq = new TopTools_HSequenceOfShape();

       // redefine MakeHalfSpace, to make pseudo Half-Space with suitable size. Define bounding values by boxes to cut...
        // this increases the chance of validity of boolean operations and reduces computation time during
        // boolean operations, which are known to work slowly.
        Bnd_Box tmpBox;
        for(int cnt=1; cnt<=tmpBoxSeq->Length(); cnt++)
           BRepBndLib::Add(tmpBoxSeq->Value(cnt), tmpBox);

        tmpBox.SetGap(10.);
        Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
        tmpBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

        gp_Pnt theUPnt = gp_Pnt(aXmax, aYmax, aZmax);
        gp_Pnt theLPnt = gp_Pnt(aXmin, aYmin, aZmin);

        TopoDS_Shape tmpS1;
        TopoDS_Shape polSol1;

        TopoDS_Shape oldSol1 = McCadMcVoid::MakeHalfSpace(aFaceSolid, theLPnt, theUPnt); 	//half space created by theVFace-member (aFaceSolid)

////////////////////////////////////////////////////////////////
// process all solids in the void box sequence
       for (int kt=1; kt<=tmpBoxSeq->Length(); kt++)
        {
                polSol1 = tmpBoxSeq->Value(kt);	//polSol1

               Standard_Boolean bOp = Standard_False;
               Standard_Boolean small = Standard_False;

        ///////////////////////////////////////////////////////////////
        // discard small volumes from cutting
                GProp_GProps GP1;
               BRepGProp::VolumeProperties(polSol1,GP1);

                if (GP1.Mass() < myMinVolume)
                {
                // cout << "Small Box Cutting discarded ....   " << kt  << endl;
                  uniBoxSeq->Append(polSol1);
                  continue;
                }


///////////////////////////////////////////////////////////////
// Compute Common part

                TopoDS_Shape bopShp1 = polSol1;
                TopoDS_Shape bopShp2 = oldSol1;

                // This will speed up the boolean operations!!!
 /*qiu               BOPTools_DSFiller aDF;
                aDF.SetShapes (bopShp1, bopShp2);
                if(!aDF.IsDone())
                {
                        cout << "_#_McCadMcVoid_Generator.cxx :: ERROR!!! Could not init DSFiller!!!\n";
                        uniBoxSeq->Append(polSol1);
                        continue;
                }

                try
                {
                        aDF.Perform();
                }
                catch(...)
                {
                        uniBoxSeq->Append(polSol1);
                        continue;
                }
*/
                //////
                // perform boolean common operation
                try
                {
 //qiu                       BRepAlgoAPI_Common aCommonOp(bopShp1, bopShp2, aDF);
                     BRepAlgoAPI_Common aCommonOp(bopShp1, bopShp2);
                        if(aCommonOp.BuilderCanWork() && aCommonOp.ErrorStatus() == 0 )
                        {
                                if(aCommonOp.IsDone())
                                {
                                        bOp = Standard_True;
                                        tmpS1 = aCommonOp.Shape();								//tmpS1 now is the common shape of half space oldSol and tmpBoxSeq(kt)
                                }
                                else
                                {
                                        bOp = Standard_False;
                                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: Boolean Operation on a halfspace has failed !!!",
                                                      McCadMessenger_WarningMsg);
                                }
                        }
                        else
                                bOp = Standard_False;

                }
                catch(...)
                {
                        bOp = Standard_False;
                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: Boolean Operation on a halfspace has failed (OCC Input?) !!!",
                                      McCadMessenger_WarningMsg);
                }


                if(!bOp)
                {
                        uniBoxSeq->Append(polSol1);
                        continue;
                }

        ///////////////////////////////////////////////////////////////////////////////////
        // Test for validity of Common Operation
                if (tmpS1.IsNull())
                {
                        msgr->Message("_#_McCadMcVoid_Generator.cxx ::  Null solid computed !!! ",
                                      McCadMessenger_WarningMsg);
                        bOp = Standard_False;

                        cout << "Cutting discarded ....   " << kt << endl;
                        uniBoxSeq->Append(polSol1);
                        continue;
                }

                if(!bOp)
                {
                        uniBoxSeq->Append(polSol1);
                        continue;
                }

          // Test for valid solids in computed shape
                TopoDS_Shape& aShape = tmpS1;
                BRepCheck_Analyzer BA(aShape,Standard_False);

                BA.Init(aShape,Standard_False);
                if(!BA.IsValid())
                {
                        {
                                msgr->Message("_#_McCadMcVoid_Generator.cxx :: BRepCheck_Analyzer for solid failed !!",
                                              McCadMessenger_WarningMsg);
                                uniBoxSeq->Append(polSol1);
                                continue;
                        }
                }

                int sCount=0;

                for (TopExp_Explorer ex(aShape,TopAbs_SOLID);ex.More();ex.Next()) //For all Solids in Common Shape check for validity
                {
                        sCount++;
                        const TopoDS_Solid& solid = TopoDS::Solid(ex.Current());

                        if(solid.IsNull())
                        {
                                msgr->Message("_#_McCadMcVoid_Generator.cxx ::  BOP for solid failed: NULL SOLID computed !!",
                                              McCadMessenger_WarningMsg);
                                bOp = Standard_False;
                                break;
                        }

                        for (TopExp_Explorer exs(solid,TopAbs_SHELL);exs.More();exs.Next()) //For all Shells in current Solid check if orientable
                        {
                                const TopoDS_Shell& sh = TopoDS::Shell(exs.Current());
                                BA.Init(sh,Standard_False);
                                Handle(BRepCheck_Result) RES = BA.Result(sh);
                                BRepCheck_ListOfStatus StatusList;
                                StatusList = RES->Status();
                                BRepCheck_ListIteratorOfListOfStatus iter;

                                for (iter.Initialize(StatusList); iter.More(); iter.Next())
                                {
                                        //	  BRepCheck::Print(iter.Value(),cout);
                                        if( iter.Value() == BRepCheck_UnorientableShape)
                                        {
                                                msgr->Message("_#_McCadMcVoid_Generator.cxx :: BRepCheck_Analyzer for shell failed: unorientable shape!!!",
                                                              McCadMessenger_WarningMsg);
                                                bOp = Standard_False;
                                                break;
                                        }
                                }

                                if(!BA.IsValid())
                                {
                                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: BRepCheck_Analyzer for shell failed !!",
                                                      McCadMessenger_WarningMsg);
                                        bOp = Standard_False;
                                        break;
                                }
                        }
                }

                if(sCount!=1) // we take only one solid
                {
                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: Compound containing no solid computed !!",
                                      McCadMessenger_WarningMsg);
                        bOp = Standard_False;
                }


                if(!bOp)
                {
                        uniBoxSeq->Append(polSol1);
                        continue;
                }

          ////////////////////////////////////////////////
          // See if Shell is closed
                Standard_Real theVol(0.0);
                if (bOp)
                {
                        for (TopExp_Explorer exs(aShape,TopAbs_SHELL);exs.More();exs.Next())	//For all Shells of aShape ...
                        {
                                TopoDS_Shell tmpShell = TopoDS::Shell(exs.Current());
                                //////////////////////////////////////////////////////////////////////////
                                BRepCheck_Shell shellCheck(tmpShell);
                                if (shellCheck.Closed(Standard_False) == BRepCheck_NoError) // check if Shell is closed
                                {
                                        BRepBuilderAPI_MakeSolid Bu(tmpShell);	//Build a Solid
                                        if(Bu.IsDone())
                                        {
                                                TopoDS_Solid aSolid = Bu.Solid();

                                                if(!aSolid.IsNull())
                                                {
                                                        //reject to thin solids
                                                        Bnd_Box theBB;
                                                        BRepBndLib::Add(aSolid, theBB);
                                                        if(theBB.IsXThin(0.5) || theBB.IsYThin(0.5) || theBB.IsZThin(0.5))
                                                        {
                                                                small = Standard_True;

                                                                cout << "IS THIN\n";
                                                                break;
                                                        }

                                                        GProp_GProps GP2;
                                                        // BRepGProp::VolumeProperties(polSol1,GP1);
                                                        BRepGProp::VolumeProperties(aSolid,GP2);

                                                //	cout << "MYMINVOLUME : " << myMinVolume << "  -- GP1.Mass() : " << GP1.Mass() << "  -- GP2.Mass() : " << GP2.Mass() << endl;

                                                        if (GP2.Mass() >= myMinVolume) // > resolution ^3
                                                                uniBoxSeq->Append(aSolid); // big volume case
                                                        else
                                                        {
                                                                theVol = GP2.Mass();
                                                                cout << "Cutting discarded .... small volume " << kt << endl;
                                                                cout << "Volume-Fraction :  " << GP2.Mass()/GP1.Mass() << endl;
                                                                small = Standard_True;
                                                                break;
                                                        }
                                                }
                                        }
                                }
                                else
                                {
                                        //try to repair open shell
                                        if (shellCheck.Closed(Standard_False) == BRepCheck_NotClosed)
                                        {
                                                ShapeUpgrade_ShellSewing tailor;
                                                TopoDS_Shape resultShp = tailor.ApplySewing(tmpShell, 1e-7);

                                                for (TopExp_Explorer shellExpl(resultShp, TopAbs_SHELL); shellExpl.More(); shellExpl.Next())	//For all Shells of aShape ...
                                                {
                                                        TopoDS_Shell tmpShll = TopoDS::Shell(shellExpl.Current());
                                                        BRepCheck_Shell shellCheck1(tmpShll);
                                                        if (shellCheck1.Closed(Standard_False) == BRepCheck_NoError) // check if Shell is closed
                                                        {
                                                                BRepBuilderAPI_MakeSolid sldMkr(tmpShll);
                                                                if(sldMkr.IsDone())
                                                                        uniBoxSeq->Append(sldMkr.Solid());
                                                                else
                                                                {
                                                                        cout << "* Warning: SubModel may be invalid!* " << endl;
                                                                        uniBoxSeq->Append(polSol1);
                                                                        continue;
                                                                }
                                                        }
                                                        else
                                                        {
                                                                cout << "* Warning: SubModel may be invalid!* " << endl;
                                                                uniBoxSeq->Append(polSol1);
                                                                continue;
                                                        }
                                                }
                                        }
                                        else
                                        {
                                                cout << "************************************ " << endl;
                                                cout << "* Warning: SubModel may be invalid!* " << endl;
                                                cout << "************************************ " << endl;

                                                uniBoxSeq->Append(polSol1);
                                                continue;
                                        } //tst
                                }
                        }
                }
                else
                {
                        cout << "Cutting discarded ....   " << kt << endl;
                        uniBoxSeq->Append(polSol1);
                        continue;
                }


                if(small || uniBoxSeq->Length() < 1)
                {
                        uniBoxSeq->Append(polSol1);
                        cout << "Putting back .... small volume " << kt  << " ### Volume : " << theVol << endl;
                        continue;
                }


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// compute the cut part
                bOp = Standard_False;

        // CUTMASTER:
                try	//Perform Cutting
                {
//qiu                         BRepAlgoAPI_Cut aCutOp(bopShp1,bopShp2, aDF);
                    BRepAlgoAPI_Cut aCutOp(bopShp1,bopShp2);

                        if(aCutOp.BuilderCanWork() && aCutOp.ErrorStatus() == 0 )
                        {
                                Standard_Boolean done = Standard_False;

                                try
                                {
                                        done = aCutOp.IsDone();
                                }
                                catch(...)
                                {
                                        continue;
                                }

                                if(done)
                                {
                                        bOp= Standard_True;
                                        tmpS1 = aCutOp.Shape();
                                }
                                else
                                {
                                        bOp= Standard_False;
                                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: Boolean Cut Operation on a halfspace failed !!!",
                                                      McCadMessenger_WarningMsg);
                                }
                        }
                        else
                                bOp = Standard_False;
                }
                catch(...)
                {
                        bOp = Standard_False;
                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: Cut Operation on a halfspace failed !!!",
                                      McCadMessenger_WarningMsg);
                }

                if (tmpS1.IsNull())
                {
                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: Null solid computed !!!",
                                      McCadMessenger_WarningMsg);
                        bOp = Standard_False;
                }

                TopoDS_Shape& acShape = tmpS1;

                BRepCheck_Analyzer BAc(acShape,Standard_False);

                sCount=0;

                for (TopExp_Explorer ex(acShape,TopAbs_SOLID);ex.More();ex.Next())	//For all Shapes of cutted Solid...
                {
                        sCount++;
                        const TopoDS_Solid& aSolo = TopoDS::Solid(ex.Current());
                        if(aSolo.IsNull())
                        {
                                msgr->Message("_#_McCadMcVoid_Generator.cxx :: BOP for solid failed: NULL SOLID computed !!",
                                              McCadMessenger_WarningMsg);
                                bOp = Standard_False;
                                break;
                        }

                        for (TopExp_Explorer exs(aSolo,TopAbs_SHELL);exs.More();exs.Next()) // check Validity
                        {
                                const TopoDS_Shell& sh = TopoDS::Shell(exs.Current());
                                BAc.Init(sh,Standard_False);
                                Handle(BRepCheck_Result) RES = BAc.Result(sh);
                                BRepCheck_ListOfStatus StatusList;
                                StatusList = RES->Status();
                                BRepCheck_ListIteratorOfListOfStatus iter;

                                for (iter.Initialize(StatusList); iter.More(); iter.Next())
                                {
                                        //	  BRepCheck::Print(iter.Value(),cout);
                                        if( iter.Value() == BRepCheck_UnorientableShape)
                                        {
                                                msgr->Message("_#_McCadMcVoid_Generator.cxx :: BRepCheck_Analyzer : Unorientable Shape!!",
                                                              McCadMessenger_WarningMsg);
                                                bOp = Standard_False;
                                                BAc.Init(sh,Standard_False);
                                                break;
                                        }
                                }

                                if(BAc.IsValid());
                                else
                                {
                                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: BRepCheck_Analyzer for shell failed !!!",
                                                      McCadMessenger_WarningMsg);
                                        bOp = Standard_False;
                                        break;
                                }
                        }
                }

                BAc.Init(acShape,Standard_False);
                if(!BAc.IsValid())
                {
                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: BRepCheck_Analyzer for solid failed !!!!",
                                      McCadMessenger_WarningMsg);
                        bOp = Standard_False;
                }
                if(sCount==0)
                {
                        msgr->Message("_#_McCadMcVoid_Generator.cxx :: Compound contains no solids!!",
                                      McCadMessenger_WarningMsg);
                      /* STEPControl_Writer wrt;
                       wrt.Transfer(polSol1, STEPControl_AsIs);
                       wrt.Transfer(aFaceSolid, STEPControl_AsIs);
                       wrt.Write("failed.stp");
                       exit(0);*/
                        bOp = Standard_False;
                }
                ////////////////////////////////////////////////
                // No Tolerance  correction
                if(bOp)
                {
                        GProp_GProps GP2;
                        BRepGProp::VolumeProperties(acShape,GP2);

                        if (GP2.Mass() >= myMinVolume) // > resolution ^3
                                uniBoxSeq->Append(acShape); // big volume case
                        else
                        {
                                uniBoxSeq->Remove(uniBoxSeq->Length());
                                uniBoxSeq->Append(polSol1);
                                TCollection_AsciiString msg("_#_McCadMcVoid_Generator.cxx :: Cut Processing void box number ....   ");
                                msg += TCollection_AsciiString(kt);
                                msg += " .... failed.";
                                msgr->Message( msg.ToCString(), McCadMessenger_WarningMsg );
                        }
                }
                else
                {
                        // delet the common part and append the original box.
                        uniBoxSeq->Remove(uniBoxSeq->Length());
                        uniBoxSeq->Append(polSol1);
                        TCollection_AsciiString msg("_#_McCadMcVoid_Generator.cxx :: Cut Processing void box number ....   ");
                        msg += TCollection_AsciiString(kt);
                        msg += " .... failed.";
                        msgr->Message( msg.ToCString(), McCadMessenger_WarningMsg );
                }
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }



        return uniBoxSeq;
}

