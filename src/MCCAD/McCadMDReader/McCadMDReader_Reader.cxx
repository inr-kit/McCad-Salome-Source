#include <McCadMDReader_Reader.ixx>
#include <fstream>
#include <McCadXCAF_TDocShapeLabelIterator.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>
#include <OSD_Path.hxx>
#include <OSD_File.hxx>

McCadMDReader_Reader::McCadMDReader_Reader(){
    Init();
}


McCadMDReader_Reader::McCadMDReader_Reader(const Handle_TDocStd_Document& theTDoc) {
    Init();
    ReadTDoc(theTDoc);
}


void McCadMDReader_Reader::Init() {
    myMCardNumbers = new TColStd_HSequenceOfInteger;
    myFileName = TCollection_AsciiString("none");
    myHasMCard = Standard_False;
}


//parse through a input file to extract material,name,density,MCARD (latter not yet implemented)
Standard_Boolean McCadMDReader_Reader::ReadFile(const TCollection_AsciiString& theFileName) {
    ifstream inFile(theFileName.ToCString());
    if (!inFile){
        cout << "Cannot open file :" <<  theFileName.ToCString() << endl;
        return Standard_False;
    }
    myFileName = theFileName;

    while(inFile) {
        char str[255];
        inFile.getline(str,255);
        TCollection_AsciiString iString(str);

        // skip empty line
        if(iString.IsEmpty())
            continue;

        // skip comment
        iString.LeftAdjust();
        iString.RightAdjust();
        if(iString.Value(1) == '#')
            continue;

        //substitute tabs with spaces
        if(iString.Search("\t") > 0) {
            iString.ChangeAll('\t', ' ');
        }

        // invalid single words are discarded
        if(iString.Search(" ") == -1 ) {
            cout << "invalid line is skipped\n";
            continue;
        }

        // eat string from behind, so the file name is allowed to contain spaces
        //Extract Density
        TCollection_AsciiString DenString, MatString;
        DenString = iString.Split(iString.SearchFromEnd(" "));
        DenString.LeftAdjust();
        DenString.RightAdjust();
        if(!DenString.IsRealValue()){
            cout << "Real value expected : " << DenString.ToCString() << endl;
            continue;
        }
        Standard_Real theDen(0.0);
        theDen = DenString.RealValue();

        if(iString.Search(" ") < 1) {
            cout << "invalid line skipped\n ";
            continue;
        }

        //Extract Material Number
        iString.RightAdjust();
        MatString = iString.Split(iString.SearchFromEnd(" "));
        MatString.LeftAdjust();
        MatString.RightAdjust();
        iString.RightAdjust();

        if(!MatString.IsIntegerValue()) {
            cout << "Integer value expected : " << MatString.ToCString() << endl;
            continue;
        }

        iString.RemoveAll('\\', Standard_False);

        // bind material and density to map
        if(myMatMap.IsBound(iString)){
            cout << iString.ToCString() << " is already bound...\n";
            continue;
        }
        myDensMap.Bind(iString, theDen);
        myMatMap.Bind(iString, MatString.IntegerValue());
    }

    if(myMatMap.Extent()<1)
         return Standard_False;

    return Standard_True;
}

Standard_Real McCadMDReader_Reader::GetDensity(const TCollection_AsciiString& theName) {
    if(myDensMap.IsBound(theName))
         return myDensMap(theName);
    else {
        TCollection_AsciiString decompName(theName);
        if(decompName.Search("_")) {
            decompName.Remove(decompName.SearchFromEnd("_"),decompName.Length()-decompName.SearchFromEnd("_")+1);
        }
        if(myDensMap.IsBound(decompName))
            return myDensMap(decompName);
    }
    return 0.0;
}

Standard_Integer McCadMDReader_Reader::GetMaterial(const TCollection_AsciiString& theName) {
    if(myMatMap.IsBound(theName))
        return myMatMap(theName);
    else {
        TCollection_AsciiString decompName(theName);
        if(decompName.Search("_")) {
            decompName.Remove(decompName.SearchFromEnd("_"),decompName.Length()-decompName.SearchFromEnd("_")+1);
        }
        if(myMatMap.IsBound(decompName))
            return myMatMap(decompName);
    }
    return 0;
}


TCollection_AsciiString McCadMDReader_Reader::GetFileName(){
    return myFileName;
}

void McCadMDReader_Reader::SetDensity(const TCollection_AsciiString& theName, const Standard_Real& theDens) {
    myDensMap.Bind(theName, theDens);
}


void McCadMDReader_Reader::SetMaterial(const TCollection_AsciiString& theName, const Standard_Integer& theMat) {
    if(myMatMap.IsBound(theName))
        myMatMap.UnBind(theName);
    myMatMap.Bind(theName, theMat);
}


void McCadMDReader_Reader::SetMCard(const Standard_Integer& theMat, const TCollection_AsciiString& theMCard){
    if(myMCardMap.IsBound(theMat))
        myMCardMap.UnBind(theMat);

    myMCardMap.Bind(theMat, theMCard);
    myHasMCard = Standard_True;
}

void McCadMDReader_Reader::GetMCard(const Standard_Integer& theMat, TCollection_AsciiString& theMCard) {
    if(!myHasMCard){
        TCollection_AsciiString mtrlNm(theMat);
        mtrlNm += " undefined material";
        theMCard = mtrlNm;
        return ;
    }

     if(myMCardMap.IsBound(theMat))
        theMCard = myMCardMap(theMat);
     else
        theMCard = TCollection_AsciiString("C No such material defined");
}


Standard_Boolean McCadMDReader_Reader::HasMCard(){
    return myHasMCard;
}


void McCadMDReader_Reader::ClearMCard(){
    myHasMCard = Standard_False;
}


void McCadMDReader_Reader::ReadTDoc(const Handle_TDocStd_Document& theTDoc){
    McCadXCAF_TDocShapeLabelIterator it(theTDoc);

    for( ; it.More(); it.Next()) {
        TDF_Label curL = it.Current();

        Handle(TDataStd_TreeNode) aTreeNode;

        if(curL.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aTreeNode)) {
            if(aTreeNode->HasFather()) {
                TDF_Label matLab = aTreeNode->Father()->Label();
                Handle(TDataStd_Name) curName;
                TCollection_AsciiString theName;
                if(curL.FindAttribute(TDataStd_Name::GetID(),curName))
                    theName = TCollection_AsciiString(curName->Get());

                Handle(TDataStd_Name) mName;
                Handle(TDataStd_Real) mDens;
                Standard_Real matDens(0.0);
                Handle(TDataStd_AsciiString) mMCard;
                TCollection_AsciiString matMCard;

                if(matLab.FindAttribute(TDataStd_Real::GetID(),mDens))
                    matDens = mDens->Get();
                if(matLab.FindAttribute(TDataStd_AsciiString::GetID(),mMCard))
                    matMCard = mMCard->Get();

                Standard_Integer matNo = ExtractMCardNumber(matMCard);

                SetMaterial(theName,matNo);
                SetDensity(theName,matDens);
                SetMCard(matNo,matMCard);
                myMCardNumbers->Append(matNo);
            }
        }// else VOIDS
    }
}

Standard_Integer McCadMDReader_Reader::ExtractMCardNumber(const TCollection_AsciiString& theMCard) {
    Standard_Integer theNo(0);
    TCollection_AsciiString evalMCard(theMCard);
    evalMCard.UpperCase();
    evalMCard.LeftAdjust();
    Standard_Integer firstM = evalMCard.Search("M");

    if(firstM < 1) // not an MCard
        return 0;

    Standard_Boolean comment=Standard_True;

    if(firstM > 1) {
        while(comment) {
            if(evalMCard.Search("C") != 1) //not a comment? that's strange, if not to say INVALID
                return 0;
            if(evalMCard.Search("\n") > 0)
                evalMCard = evalMCard.Split(evalMCard.Search("\n"));
            evalMCard.LeftAdjust();
            if(evalMCard.Search("M") == 1)
                    break;
        }
    }

    evalMCard.Remove(1,1);
    evalMCard.Split(evalMCard.Search(" ")-1);

    if(evalMCard.IsIntegerValue())
        theNo = evalMCard.IntegerValue();

    return theNo;
}

Handle(TColStd_HSequenceOfInteger) McCadMDReader_Reader::GetMCardNumbers() {
    return myMCardNumbers;
}
