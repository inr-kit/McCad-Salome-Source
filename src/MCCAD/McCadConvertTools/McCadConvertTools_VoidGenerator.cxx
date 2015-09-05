//qiu #include <sys/stat.h>
#include <fstream>

#include <McCadConvertTools_VoidGenerator.hxx>
#include <McCadEXPlug_ExchangePlugin.hxx>
#include <McCadDiscDs_DiscFace.hxx>
#include <McCadDiscDs_HSequenceOfDiscFace.hxx>
#include <McCadDiscDs_HSequenceOfDiscSolid.hxx>
#include <McCadDiscretization_Solid.hxx>
#include <McCadMcVoid.hxx>
#include <McCadMcVoid_ForwardCollision.hxx>
#include <McCadMcVoid_Generator.hxx>
#include <McCadMessenger_Singleton.hxx>

#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <gp_Pnt.hxx>

//qiu add conditional include for mkdir()
#if defined WIN32
#   include <direct.h>
#else //linux
#   include <sys/types.h>
#   include <sys/stat.h>
#endif
//qiu using this function to replace mkdir
void Mkdir(std::string dir) {
#if defined WIN32
    _mkdir(dir.data());
#else //linux
    mkdir(dir.data(), 0777);
#endif
}

using namespace std;

McCadConvertTools_VoidGenerator::McCadConvertTools_VoidGenerator() {
    Init();
}


McCadConvertTools_VoidGenerator::McCadConvertTools_VoidGenerator( McCadIOHelper_VGParameters& parameters){
    Init();
    SetParameters(parameters);
}


void McCadConvertTools_VoidGenerator::Init() {
    myVVoids = new McCadDiscDs_HSequenceOfDiscSolid;
    myVSolids = new McCadDiscDs_HSequenceOfDiscSolid;
    myVOuterVoids = new McCadDiscDs_HSequenceOfDiscSolid;

    myFirstFile = true;

    myProgress = new McCadGUI_ProgressDialog;
    myProgress->SetTotalSteps(100);

    McCadGUI_ProgressDialogPtr theProgress = new McCadGUI_ProgressDialog();
    m_progressIndicator = Handle(Message_ProgressIndicator)::DownCast(theProgress);
    m_parameters.inputSolids = new McCadDiscDs_HSequenceOfDiscSolid;
    m_parameters.boundingBox = new TopTools_HSequenceOfShape;
}


Standard_Boolean McCadConvertTools_VoidGenerator::Init( McCadIOHelper_VGParameters& parameters) {
    SetParameters(parameters);

    return Perform();
}


void McCadConvertTools_VoidGenerator::SetProgressIndicator(Handle_Message_ProgressIndicator& thePI){
    m_progressIndicator = thePI;
}


void McCadConvertTools_VoidGenerator::AddDiscSolids(Handle_McCadDiscDs_HSequenceOfDiscSolid& theNamedSolids) {
    m_parameters.inputSolids->Append(theNamedSolids);
}


McCadCSGGeom_Unit McCadConvertTools_VoidGenerator::GetUnits(){
    return m_parameters.units;
}

Standard_Integer McCadConvertTools_VoidGenerator::GetInitSurfNb(){
    return m_parameters.initSurfaceNumber;
}

Standard_Integer McCadConvertTools_VoidGenerator::GetInitCellNb(){
    return m_parameters.initCellNumber;
}

// ------------------------------------------------------
//  Create a bounding box for the input model
// ------------------------------------------------------
Standard_Boolean McCadConvertTools_VoidGenerator::MakeBoundingBox() {
    Bnd_Box boundingBox;
    BRepBndLib boxExpander;
    TopLoc_Location loc;
    TopExp_Explorer shpEx;

    /*  we create a bounding box. Caution, when non-linear faces occur it might
        there are often tolerance mismatches on the border. to avoid the resulting
        overlaps we introduce a gap in the bounding box. unfortunately the gap
        is deleted by OCC code, when we create a bounding box for a face and add
        this bounding box to the overall BB. therefor we compute the extremal points
        of the face-bounding-box and add these.
     */

    for (int i=1 ; i <= myVSolids->Length(); i++) {
        TopoDS_Shape tmpShp = myVSolids->Value(i).GetSolid();
        //boxExpander.Add(tmpShp, aGlBox);

        for(shpEx.Init(tmpShp, TopAbs_FACE); shpEx.More(); shpEx.Next()) {
            TopoDS_Face curFace = TopoDS::Face(shpEx.Current());
            Bnd_Box tmpBox;
            boxExpander.Add(curFace, tmpBox);

            Handle(Geom_Surface) theFaceSurface = BRep_Tool::Surface(curFace,loc);
            GeomAdaptor_Surface adaptedFaceSurface(theFaceSurface);

            if(adaptedFaceSurface.GetType() != GeomAbs_Plane) {
                tmpBox.SetGap(0.1);
                Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
                tmpBox.Get(xMin, yMin, zMin, xMax, yMax, zMax);
                gp_Pnt lower(xMin, yMin, zMin), upper(xMax, yMax, zMax);
                boundingBox.Add(lower);
                boundingBox.Add(upper);
            }
            else
                boundingBox.Add(tmpBox);
        }
    }

    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    boundingBox.SetGap(0.0);
    boundingBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    gp_Pnt theUPnt = gp_Pnt(aXmax, aYmax, aZmax);
    gp_Pnt theLPnt = gp_Pnt(aXmin, aYmin, aZmin);

    m_parameters.boundingBox->Append(BRepPrimAPI_MakeBox(theLPnt,theUPnt).Shape());

    return Standard_True;
}


// ----------------------------------------------
//  Voxelize input solids
// ----------------------------------------------
void McCadConvertTools_VoidGenerator::Voxelize() {
    McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();
    bool first=true;
    ofstream tmpOut;

    Standard_Integer numSolids = m_parameters.inputSolids->Length();
    Standard_Real step(20.5/numSolids);
    m_progressIndicator->SetStep(step);

    // write voxel files
    if(m_parameters.writeVoxelFile) {
        msgr->Message("Voxelization started...");
        msgr->Message("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

        TCollection_AsciiString prevName("none");

        for(Standard_Integer i=1; i<= numSolids; i++) {
            McCadDiscDs_DiscSolid aDiscSolid = m_parameters.inputSolids->Value(i);
            if( aDiscSolid.GetFileName() != prevName)  {
                if(first) {
                    cout << "Processing file: " << aDiscSolid.GetFileName().ToCString() << endl;
                    first = false;
                }
                else
                    cout << "\nProcessing file: " << aDiscSolid.GetFileName().ToCString() << endl;
                prevName = aDiscSolid.GetFileName();
            }
            cout << "." << flush;

            McCadDiscretization_Solid theDiscer(aDiscSolid);
            theDiscer.SetMinNbPnt(m_parameters.minSamplePoints);
            theDiscer.SetMaxNbPnt(m_parameters.maxSamplePoints);
            theDiscer.SetXresolution(m_parameters.xResolution);
            theDiscer.SetYresolution(m_parameters.yResolution);
            theDiscer.SetTolerance(m_parameters.tolerance);
            theDiscer.Init();

            // write voxel files
            if(theDiscer.IsDone()) {

                // make voxel directory
//qiu                mkdir("voxels", 0755);
				Mkdir ("voxels");

                // get voxel file name
                TCollection_AsciiString voxelFileName("voxels/");

                // strip of path from STEP file name
                TCollection_AsciiString stepFileName = aDiscSolid.GetFileName();
                if(stepFileName.Search("/") > 0)
                    voxelFileName += stepFileName.Split(stepFileName.SearchFromEnd("/"));
                else
                    voxelFileName += stepFileName;
                voxelFileName += TCollection_AsciiString("_") + i + ".voxel";

                // open voxel file
                tmpOut.open(voxelFileName.ToCString(),ios::out);
                if(!tmpOut) {
                    cout << "_#_McCadConvertTools_VoidGenerator.cxx :: Cannot open file for voxel output:  " <<  voxelFileName << endl;
                    continue;
                }
                tmpOut << aDiscSolid.GetName().ToCString() << endl;


                Handle(McCadDiscDs_HSequenceOfDiscFace) ownVFaces = theDiscer.GetSolid().GetOwnVFaces();
                int  NbOfFaces = ownVFaces->Length();

                //write voxels for all faces of input solid
                tmpOut << NbOfFaces << endl;
                for (Standard_Integer i=1; i <= ownVFaces->Length();i++){
                        Handle(TColgp_HSequenceOfPnt) aPntSeq = ownVFaces->Value(i).GetPoints();
                    tmpOut << aPntSeq->Length()  << endl;
                    tmpOut.precision(12);
                    for (Standard_Integer j = 1; j<= aPntSeq->Length(); j++){
                        gp_Pnt aP = aPntSeq->Value(j);
                        tmpOut << aP.X() << " " << aP.Y() << " " << aP.Z() << endl;
                    }
                }
                tmpOut.close();

                // append voxelized solid to sequence of voxelized solids
                McCadDiscDs_DiscSolid tmpVSolid = theDiscer.GetSolid();
                tmpVSolid.SetIsVoid(Standard_False); // unset void flag : this cell is material
                myVSolids->Append(tmpVSolid);
            }
            else {
                    TCollection_AsciiString fldmsg("_#_McCadConvertTools_VoidGenerator.cxx :: Discretization failed for solid file: ");
                    fldmsg += aDiscSolid.GetFileName().ToCString();
                    msgr->Message(fldmsg.ToCString(), McCadMessenger_ErrorMsg);
            }

            m_progressIndicator->Increment();
        }

        msgr->Message("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        msgr->Message("...Voxelization done.\n");
    }
    else // read voxel files
    {
        msgr->Message("Reading voxel files...");
        msgr->Message("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

        Bnd_Box solBB;

        for(Standard_Integer i=1; i<= numSolids; i++){
            McCadDiscDs_DiscSolid aDiscSolid = m_parameters.inputSolids->Value(i);

            // get voxel file name
            TCollection_AsciiString voxelFileName("voxels/");
            // strip of path from STEP file name
            TCollection_AsciiString stepFileName = aDiscSolid.GetFileName();
            voxelFileName += stepFileName.Split(stepFileName.SearchFromEnd("/"));
            voxelFileName += TCollection_AsciiString("_") + i + ".voxel";

            ifstream tmpIn;
            tmpIn.open(voxelFileName.ToCString(),ios::in);

            if(!tmpIn){
                cout << "Couldn't open file: " << voxelFileName.ToCString() << " for reading." << endl;
                McCadDiscretization_Solid theDiscer(aDiscSolid);
                theDiscer.SetMinNbPnt(m_parameters.minSamplePoints);
                theDiscer.SetMaxNbPnt(m_parameters.maxSamplePoints);
                theDiscer.SetXresolution(m_parameters.xResolution);
                theDiscer.SetYresolution(m_parameters.yResolution);
                theDiscer.SetTolerance(m_parameters.tolerance);
                theDiscer.Init();

                if(theDiscer.IsDone()){
                    myVSolids->Append(theDiscer.GetSolid());
                }
                else{
                    TCollection_AsciiString fldmsg("_#_McCadConvertTools_VoidGenerator.cxx :: Discretization failed for solid file: ");
                    fldmsg += aDiscSolid.GetFileName().ToCString();
                    msgr->Message(fldmsg.ToCString(), McCadMessenger_ErrorMsg);
                }
            }
            else {
                cout << "Reading voxel data from file:  " << voxelFileName.ToCString() << endl;

                char inName[256];
                int  NbOfFaces=0;
                tmpIn >> inName;
                tmpIn >> NbOfFaces;

                Handle(McCadDiscDs_HSequenceOfDiscFace) ownVFaces = aDiscSolid.GetOwnVFaces();
                Handle(McCadDiscDs_HSequenceOfDiscFace) newOwnVFaces = new McCadDiscDs_HSequenceOfDiscFace;

                if(ownVFaces->Length() != NbOfFaces) {
                    cout << "_#_McCadConvertTools_VoidGenerator.cxx :: Number of faces does not match!!!\n";
                    cout << "                                          In : " << NbOfFaces << " -- Solid : " << ownVFaces->Length() << endl;
                }


                for (Standard_Integer k=1; k <= ownVFaces->Length(); k++){
                    McCadDiscDs_DiscFace theDFace = ownVFaces->Value(k);
                    Handle(TColgp_HSequenceOfPnt) aPntSeq = new TColgp_HSequenceOfPnt();;
                    int  NbOfPoints = 0;

                    tmpIn >> NbOfPoints;

                    if(NbOfPoints > 0 ){
                        for (Standard_Integer j = 1; j<= NbOfPoints; j++){
                            Standard_Real x,y,z;
                            tmpIn >> x >> y >> z;
                            gp_Pnt aP(x,y,z);
                            aPntSeq->Append(aP);
                        }
                    }

                    Bnd_Box  aBB;
                    TopoDS_Face aF = TopoDS::Face(theDFace.GetFace());

                    if(aF.IsNull())
                        cout << "_#_McCadConvertTools_VoidGenerator.cxx :: Null face found. " << endl;

                    for (TopExp_Explorer exv(aF,TopAbs_VERTEX); exv.More(); exv.Next()){
                        gp_Pnt  aPnt = BRep_Tool::Pnt(TopoDS::Vertex(exv.Current()));
                        aPntSeq->Prepend(aPnt);
                    }

                    for (Standard_Integer j = 1; j<= aPntSeq->Length(); j++)
                        aBB.Add(aPntSeq->Value(j));

                    aBB.SetGap(0.0);
                    theDFace.SetPoints(aPntSeq);
                    theDFace.SetBBox(aBB);
                    newOwnVFaces->Append(theDFace);
                    solBB.Add(aBB);
                }

                aDiscSolid.SetOwnVFaces(newOwnVFaces);
                solBB.SetGap(0.0);
                aDiscSolid.SetBBox(solBB);
                aDiscSolid.SetIsVoid(Standard_False);

                myVSolids->Append(aDiscSolid);
            }
            tmpIn.close();

            if(m_progressIndicator->UserBreak())
                return;

            m_progressIndicator->Increment();
        }

        msgr->Message("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        msgr->Message("...Reading done.\n");
    }
}


void McCadConvertTools_VoidGenerator::DeleteSmallVolumes() {
    McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance() ;
    cout << "\nLooking for small volumes to discard.\n";
    TopExp_Explorer ex;
    Standard_Integer idiscard(0);
    Standard_Real totVolDiscarded(0.0);
    Standard_Real overallVolume(0.0);
    double smallestVolumeFound(1.0e100);

    //find smallest volume of input solids - calculate overall volume of input model
    for(Standard_Integer i = 1; i <= m_parameters.inputSolids->Length(); i++) {
        Handle_TopTools_HSequenceOfShape validInputSolids = new TopTools_HSequenceOfShape;
        TopoDS_Shape curShp = m_parameters.inputSolids->Value(i).GetSolid();

        // explode Compound shapes into solids
        if( curShp.ShapeType() == TopAbs_COMPSOLID || curShp.ShapeType() == TopAbs_COMPOUND ) {
            for(ex.Init(curShp,TopAbs_SOLID); ex.More(); ex.Next() ) {
                TopoDS_Solid tmpSol = TopoDS::Solid(ex.Current());
                Standard_Real curVol = McCadMcVoid::VolumeOfShape(tmpSol);
                overallVolume += curVol;

                // add big enough input shapes
                if(curVol > m_parameters.minInputSolidVolume ) {
                    validInputSolids->Append(tmpSol);
                    if(curVol < smallestVolumeFound)
                        smallestVolumeFound = curVol;
                }
                else {
                    // negative volumes
                    if(curVol < 0)  {
                        TCollection_AsciiString message("_#_McCadConvertTools_VoidGenerator :: negative input volume found in file : ");
                        message.AssignCat( m_parameters.inputSolids->Value(i).GetName());
                        msgr->Message(message.ToCString(), McCadMessenger_ErrorMsg);
                    }
                    idiscard++;
                    totVolDiscarded += curVol;
                }
            }

             // remove input solids with small volume from input shapes
            if(validInputSolids->Length() < 1) {
                m_parameters.inputSolids->Remove(i);
                continue;
            }

            // rebuild compound of big enough solids
            if(validInputSolids->Length() > 1) {
                TopoDS_Compound newComp;
                BRep_Builder aBuilder;

                for(Standard_Integer j=1; j < validInputSolids->Length(); j++)
                    aBuilder.Add(newComp, validInputSolids->Value(j));

                aBuilder.MakeCompound(newComp);

                McCadDiscDs_DiscSolid aDiscSolid = m_parameters.inputSolids->Value(i);
                aDiscSolid.SetSolid(newComp);
                m_parameters.inputSolids->SetValue(i,aDiscSolid);
            }
            // exactly one solid
            else {
                McCadDiscDs_DiscSolid aDiscSolid = m_parameters.inputSolids->Value(i);
                aDiscSolid.SetSolid(validInputSolids->Value(1));
                m_parameters.inputSolids->SetValue(i, aDiscSolid);
            }
        }
        // add single solid if big enough
        else if (curShp.ShapeType() == TopAbs_SOLID) {
            TopoDS_Solid tmpSol = TopoDS::Solid(curShp);
            Standard_Real curVol = McCadMcVoid::VolumeOfShape(tmpSol);
            overallVolume += curVol;

            if(curVol < smallestVolumeFound)
                smallestVolumeFound = curVol;

            // discard small volumes
            if(curVol < m_parameters.minInputSolidVolume) {
                if(curVol < 0) {
                    TCollection_AsciiString message("_#_McCadConvertTools_VoidGenerator :: negative input volume found in file : ");
                    message.AssignCat( m_parameters.inputSolids->Value(i).GetName());
                    msgr->Message(message.ToCString(), McCadMessenger_ErrorMsg);
                }

                idiscard++;
                totVolDiscarded += curVol;
                m_parameters.inputSolids->Remove(i);
            }
        }
    }

    // define minimum void volume to 1/5th from smallest input volume if not specified by user
    if(m_parameters.minVoidVolume == 0.0) {
        smallestVolumeFound/=5.;
        if(m_parameters.minVoidVolume < smallestVolumeFound) {
            m_parameters.minVoidVolume = smallestVolumeFound;
            TCollection_AsciiString message(" # Setting MinimumVoidVolume to : ");
            message.AssignCat(m_parameters.minVoidVolume);
            msgr->Message(message.ToCString());
        }
    }

    TCollection_AsciiString msg("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    msg += TCollection_AsciiString(idiscard);
    msg += " solids have been discarded due to small volume.\nTotal volume of discarded solids: ";
    msg += TCollection_AsciiString(totVolDiscarded);
    msg += "\nTotal volume of input model     : ";
    msg += TCollection_AsciiString(overallVolume);
    msg += "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    msgr->Message(msg.ToCString());
}


void McCadConvertTools_VoidGenerator::CheckInputCollision() {
    McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();
    if( !m_parameters.writeCollisionFile ) {
        ifstream collIn("CollisionFile");

        cout << "\n\nReading CollisionFile...\n";

        if(!collIn) {
                msgr->Message("Couldn't read the 'CollisionFile'.\n",
                              McCadMessenger_WarningMsg);
                m_parameters.writeCollisionFile = true;
                CheckInputCollision();
                return;
        }

        int sct, nbCol;

        for(Standard_Integer i=1; i <= myVSolids->Length(); i++)
        {
            collIn >> sct >> nbCol;
            if (nbCol > 0 ) {
                McCadDiscDs_DiscSolid aDiscSolid = myVSolids->Value(i);
                Handle(TColStd_HSequenceOfInteger) intSeq = new TColStd_HSequenceOfInteger;
                int tmpInt;
                for(int j=1; j <=nbCol; j++) {
                    collIn >>  tmpInt;
                    intSeq->Append(tmpInt);
                }
                aDiscSolid.SetOtherVSolids(intSeq);
            }
        }
        cout << "...done\n";
    }
    else
    {
        ofstream collOut("CollisionFile");
        if(!collOut)
            cout << "Cannot open file for collision output !!!!" << endl;

        Handle(McCadDiscDs_HSequenceOfDiscSolid) tmpSeq = new McCadDiscDs_HSequenceOfDiscSolid;
        tmpSeq->Clear();

        for(Standard_Integer tr=1; tr<=myVSolids->Length(); tr++)
                tmpSeq->Append(myVSolids->Value(tr));

        McCadMcVoid_ForwardCollision theForwardChecker(tmpSeq);
        theForwardChecker.Init();
        if(theForwardChecker.IsDone()) {
            msgr->Message("Writing collision output to file: CollisionFile");
            // For all VSolids write collision data in file CollisionFile
            for(int i=1; i <= myVSolids->Length(); i++) {
                Handle(TColStd_HSequenceOfInteger) intSeq = myVSolids->Value(i).GetOtherVSolids();
                collOut << i << " " << intSeq->Length()  << " " ;

                if (intSeq->Length() > 0 )
                    for(int j=1; j <= intSeq->Length(); j++)
                            collOut << intSeq->Value(j) << " ";
                collOut << endl;
            }
        }
        else
        {
            msgr->Message("_#_VoidGenerator.cxx :: Collision check among input solids failed !!",
                          McCadMessenger_ErrorMsg);
        }
        collOut.close();
    }
}


Standard_Boolean McCadConvertTools_VoidGenerator::GenerateVoids() {
    McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();
    if (m_parameters.inputSolids->IsEmpty()) {
        msgr->Message("_#_McCadConvertTools_VoidGenerator.cxx :: No input solids found!!!! \n",
                      McCadMessenger_WarningMsg);
        return false;
    }

    // Discretize Model, delete solids due to small volume, handle collision in input file
    Voxelize();
    CheckInputCollision();

    // Make bounding box, if not defined
    if( m_parameters.boundingBox->Length() < 1 ) {
        if( !MakeBoundingBox()) {
            msgr->Message("_#_McCadConvertTools_VoidGenerator.cxx :: Could not create Bounding Box!!!\n",
                          McCadMessenger_ErrorMsg);
            return false;
        }
    }

    // Generate Voids
    cout << "\n Starting void generation  " << endl;
    cout << "==========================="  << endl;
    McCadMcVoid_Generator theVGen(myVSolids);
    theVGen.SetBSolid(m_parameters.boundingBox);
    theVGen.SetMinVolume(m_parameters.minVoidVolume);
    theVGen.SetMaxComplementedShapes(m_parameters.maxNumberOfComplementCells);
    theVGen.SetMaxNbOfPreDecomposition(m_parameters.maxNumberOfPreDecompositionCells);
    theVGen.SetMinDecompositionFaceArea(m_parameters.minSizeOfDecompositionFace);
    theVGen.SetMinRedecompositionFaceArea(m_parameters.minSizeOfRedecompositionFace);
    theVGen.SetDiscParameter( m_parameters.maxSamplePoints, m_parameters.minSamplePoints,
                              m_parameters.xResolution, m_parameters.yResolution, m_parameters.tolerance );
    theVGen.SetProgressIndicator(m_progressIndicator);

    theVGen.Init();	//running Decomposition of Bounding Box

    if( theVGen.IsDone()) {
        cout << "==========================="  << endl;
        cout << " End of void generation\n" << endl;
    }
    else {
        msgr->Message("_#_VoidGenerator.cxx :: Generation of void failed check output file for informations!!",
                      McCadMessenger_ErrorMsg);
        return false;
    }
    Handle(McCadDiscDs_HSequenceOfDiscSolid) theVVoid = theVGen.GetDiscVoids(); //Voxelized Voids
    myVVoids->Clear();
    myVVoids->Append(theVVoid);

    //make outer space
    myOuterSpace = theVGen.MkOuterSpace();

    //prepare outer space
    McCadDiscDs_DiscSolid tmpSol;

    //fuse boxes to a single BB if input Bounding Box is predecomposed by user
    for(Standard_Integer j=1; j<=m_parameters.boundingBox->Length(); j++) {
        tmpSol.SetSolid(m_parameters.boundingBox->Value(j));
        tmpSol.SetIsOuterVoid(Standard_True);
        myVOuterVoids->Append(tmpSol);
    }
    tmpSol.SetSolid(myOuterSpace);
    myVOuterVoids->Append(tmpSol);

    return Standard_True;
}


Handle(McCadDiscDs_HSequenceOfDiscSolid) McCadConvertTools_VoidGenerator::GetVVoids() {
    return myVVoids;
}

Handle(McCadDiscDs_HSequenceOfDiscSolid) McCadConvertTools_VoidGenerator::GetVSolids() {
    return myVSolids;
}

Handle(McCadDiscDs_HSequenceOfDiscSolid) McCadConvertTools_VoidGenerator::GetVOuterVoids() {
    return myVOuterVoids;
}


Standard_Boolean McCadConvertTools_VoidGenerator::Perform() {
    DeleteSmallVolumes();
    Standard_Boolean success = GenerateVoids();
    return success;
}

void McCadConvertTools_VoidGenerator::SetMDReader(McCadMDReader_Reader& theMDReader) {
    myMDReader = theMDReader;
}

McCadMDReader_Reader& McCadConvertTools_VoidGenerator::GetMDReader() {
    return myMDReader;
}

void McCadConvertTools_VoidGenerator::AddBoundingBox(Handle_TopTools_HSequenceOfShape& addBB) {
    m_parameters.boundingBox->Append(addBB);
}

void McCadConvertTools_VoidGenerator::SetParameters(McCadIOHelper_VGParameters &parameters){
    m_parameters = parameters;
    if( !parameters.materialDensityFileName.IsEmpty() )
        myMDReader.ReadFile(parameters.materialDensityFileName);
}
