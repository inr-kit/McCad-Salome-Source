#include <McCadCSGGeom_Cell.ixx>
#include <TCollection_AsciiString.hxx>
#include <McCadCSGGeom_Surface.hxx>

McCadCSGGeom_Cell::McCadCSGGeom_Cell()
{
    myCellNumber = 0;
    myMaterialNumber = 0;
    myDensity = 0.;//arbitrary density
    mySurfCounter = 0;
    myTrsfCounter = 0;
    myCSG = new TColStd_HSequenceOfAsciiString();
    myCSG->Clear();
    mySurfaces.Clear();
    myComment = TCollection_AsciiString("c       McCad Generated Cell!");
    myComplementCells = new TColStd_HSequenceOfInteger;
    myIsVoid = Standard_False;
    myMCType = McCadCSGGeom_MCNP;
}


McCadCSGGeom_Cell::McCadCSGGeom_Cell(const Standard_Integer theCNum, const Standard_Integer theMNum, const Standard_Real theDensity)
{
    myCellNumber = theCNum;
    myMaterialNumber = theMNum;
    myDensity = theDensity;
    mySurfCounter = 0;
    myTrsfCounter = 0;
    myCSG = new TColStd_HSequenceOfAsciiString;
    myCSG->Clear();
    mySurfaces.Clear();
    myComment = TCollection_AsciiString("c       McCad Generated Cell!");
    myComplementCells = new TColStd_HSequenceOfInteger;
    myIsVoid = Standard_False;
    myMCType = McCadCSGGeom_MCNP;
}


void McCadCSGGeom_Cell::SetCellNumber(const Standard_Integer theCNum)
{
    myCellNumber = theCNum;
}


Standard_Integer McCadCSGGeom_Cell::GetCellNumber() const
{
    return myCellNumber;
}


void McCadCSGGeom_Cell::SetMaterialNumber(const Standard_Integer theMNum)
{
    myMaterialNumber = theMNum;
}


Standard_Integer McCadCSGGeom_Cell::GetMaterialNumber() const
{
    return myMaterialNumber;
}


void McCadCSGGeom_Cell::SetDensity(const Standard_Real theDensity)
{
    myDensity = theDensity;
}


Standard_Real McCadCSGGeom_Cell::GetDensity() const
{
    return myDensity;
}


void McCadCSGGeom_Cell::SetSurface(	const McCadCSGGeom_DataMapOfIntegerSurface& theSurfaces)
{
    mySurfaces.Clear();
    mySurfaces.Assign(theSurfaces);
}


McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface McCadCSGGeom_Cell::GetSurface() const
{
    McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface surfIter(mySurfaces);
    return surfIter;
}


void McCadCSGGeom_Cell::PrintCell(Standard_OStream& theStream)
{
    // used for absolute counting  mySurfCounter && myTrsfCounter
    // should be set to zero before starting printing;

    theStream.precision(5);
    theStream.width(5);
    theStream.fill(' ');
    theStream.setf(ios::left);

    if(GetMCType() == McCadCSGGeom_TRIPOLI)
    {
        TColStd_HSequenceOfInteger plusList; // sort list by positive and negative values. >> MINUS >> PLUS and print volumes.
        TColStd_HSequenceOfInteger minusList;
        TColStd_HSequenceOfInteger vminusList;
        Standard_Boolean isFictive (Standard_False);

        for(int i=1; i<=myCSG->Length(); i++)
        {
            TCollection_AsciiString curVal = myCSG->Value(i);
            if(curVal.IsIntegerValue())
            {
                Standard_Integer intVal = curVal.IntegerValue();
                if(intVal < 0)
                    minusList.Append(intVal);
                else
                    plusList.Append(intVal);
            }
            else if(curVal.Search("#") > 0) //complement
            {
                while(curVal.Search("#") > 0)
                {
                    curVal.LeftAdjust();
                    curVal.Remove(1,curVal.Search("#"));

                    TCollection_AsciiString tmpStr = curVal;

                    if(tmpStr.Search(" ") > 0)
                        curVal = tmpStr.Split(curVal.Search(" "));
                    else
                        curVal = "";

                    tmpStr.RightAdjust();

                    if(tmpStr.IsIntegerValue())
                        vminusList.Append(tmpStr.IntegerValue());
                    else
                        cout << "###### NOT AN INTEGER VALUE" << endl;
                }
            }
            else if(curVal.Search("FICTIVE") > 0) // make cell Virtual
            {
                isFictive = Standard_True;
            }
        }

        //Print Surfaces
        if(plusList.Length() > 0)
        {
            theStream << "\n           PLUS   " << plusList.Length() << "  ";
            for(int i=1; i<=plusList.Length(); i++)
                theStream << plusList.Value(i) << " ";
        }
        if(minusList.Length() > 0)
        {
            theStream << "\n           MINUS  " << minusList.Length() << "  ";
            for(int i=1; i<=minusList.Length(); i++)
                theStream << Abs(minusList.Value(i)) << " ";
        }
        if(vminusList.Length()>0) // we have complementary cells
        {
            theStream << "\n            VMINUS " << vminusList.Length() << " ";
            for(int i=1; i<=vminusList.Length(); i++)
                theStream << Abs(vminusList.Value(i)) << " ";
        }
        if(isFictive)
            theStream << " FICTIVE ";
    }
    else // Default MCNP
    {
        theStream.setf(ios::left);
        theStream.unsetf(ios::right);
        theStream << setw(5) << myCellNumber << " "; // cell number
        theStream << setw(3) << myMaterialNumber << " ";

        if( !myIsVoid && myDensity != 0 )
            theStream << setw(8) << myDensity << " ";
        theStream << endl;

        theStream.unsetf(ios::left);
        theStream.setf(ios::right);
        theStream.width(7);
        theStream.fill(' ');
        theStream << setw(12) << " ";

        int stcount = 12;

        // print importances
        if(myIsOuterVoid)
        {
            myCSG->Append(TCollection_AsciiString(" IMP:N=0"));
            myCSG->Append(TCollection_AsciiString(" IMP:P=0"));
        }
        else
        {
            myCSG->Append(TCollection_AsciiString(" IMP:N=1"));
            myCSG->Append(TCollection_AsciiString(" IMP:P=1"));
        }

        // make universe
        if(getenv("MCCAD_MAKEUNIVERSE") != NULL)
        {
            TCollection_AsciiString universe(" U=");
            TCollection_AsciiString uNumber(getenv("MCCAD_MAKEUNIVERSE"));
            if(uNumber.IsIntegerValue())
            {
                universe += uNumber;
                myCSG->Append(universe);
            }
        }

    //	cout << "\n" << stcount << endl;

        theStream.setf(ios::right);

        bool notYetInComplementSection(true);
        bool notYetImp(true);

        for (int i=1; i<= myCSG->Length(); i++)
        {
            TCollection_AsciiString curCSG = myCSG->Value(i);
            curCSG.RightAdjust();

            if(curCSG.Search("#") > 0 && notYetInComplementSection)
            {
                notYetInComplementSection = false;
                theStream << endl << setw(12) << " ";
                stcount = 12;
            }

            if( curCSG.Search("imp") > 0 || curCSG.Search("IMP") > 0 )
            {
                if(notYetImp)
                {
                    notYetImp = false;
                    theStream << endl << setw(12) << " ";
                    stcount = 12;
                }
            }

            if( stcount+8 > 78 )
            {
                theStream << endl << setw(12) << " ";
                stcount = 12;
            }

            theStream << setw(8) << curCSG.ToCString();
            stcount += 8;
        }

        theStream.unsetf(ios::right);
    }

    theStream << endl;
}


void McCadCSGGeom_Cell::PrintSurfaces(Standard_OStream& theStream)
{
    McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface theIter;

    theStream.precision(5);
    theStream.width(5);
    theStream.fill(' ');
    theStream.setf(ios::left);

    // we set first the trsf numbers correctly
    Standard_Integer iCount = GetTrsfCounter();
    for (theIter.Initialize(mySurfaces) ; theIter.More() ; theIter.Next())
    {
        Handle(McCadCSGGeom_Surface) tmpSurf = theIter.Value();

        if (tmpSurf->HaveTransformation())
        {
            iCount++;
            tmpSurf->SetTrsfNumber(iCount);
        }
    }
    for (theIter.Initialize(mySurfaces) ; theIter.More() ; theIter.Next())
    {
        Handle(McCadCSGGeom_Surface) tmpSurf = theIter.Value();
        Standard_Integer surffNb = tmpSurf->GetNumber() + mySurfCounter;
        tmpSurf->SetNumber(surffNb);
        tmpSurf->SetMCType(myMCType);
        tmpSurf->Print(theStream);
    }
}


void McCadCSGGeom_Cell::PrintSurfaceTrsfMat(Standard_OStream& theStream)
{
    theStream.precision(7);
    // theStream.width(5);
    theStream.fill(' ');
    theStream.setf(ios::left);

    McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface theIter;

    for (theIter.Initialize(mySurfaces) ; theIter.More() ; theIter.Next())
    {
        Handle(McCadCSGGeom_Surface) tmpSurf = theIter.Value();

        if (tmpSurf->HaveTransformation())
        {
            gp_Trsf theTrsf;
            gp_Ax3 theAxis = tmpSurf->GetAxis();
            gp_Ax3 stdAxis;
            theTrsf.SetTransformation(theAxis, stdAxis);

            TCollection_AsciiString trStr("tr");
            trStr += tmpSurf->GetTrsfNumber();
            theStream << setw(7) << trStr.ToCString();

            gp_XYZ Vec = theTrsf.TranslationPart();
            theStream.precision(7);
            theStream.width(7);
            theStream.fill(' ');

            if(Abs(Vec.X()) < 1.e-7)
                Vec.SetX(0.0);
            if(Abs(Vec.Y()) < 1.e-7)
                Vec.SetY(0.0);
            if(Abs(Vec.Z()) < 1.e-7)
                Vec.SetZ(0.0);

            theStream.setf(ios::right);
            theStream << setw(15) << Vec.X() << "  "
                      << setw(15) << Vec.Y() << "  "
                      << setw(15) << Vec.Z() << endl;
            gp_Mat Mat = theTrsf.HVectorialPart();

            for (int i=1; i<=3; i++)
            {
                gp_XYZ Vvec = Mat.Column(i);

                    if(Abs(Vvec.X()) < 1.e-10)
                        Vvec.SetX(0.0);
                    if(Abs(Vvec.Y()) < 1.e-10)
                        Vvec.SetY(0.0);
                    if(Abs(Vvec.Z()) < 1.e-10)
                        Vvec.SetZ(0.0);

                theStream << setw(7)<< " "
                          << setw(15) << Vvec.X() << "  "
                          << setw(15) << Vvec.Y() << "  "
                          << setw(15) << Vvec.Z() << endl;
            }
            theStream.unsetf(ios::right);
        }
    }
}


void McCadCSGGeom_Cell::SetCSG(const Handle(TColStd_HSequenceOfAsciiString)& theCSG)
{
    myCSG = theCSG;
}


void McCadCSGGeom_Cell::AppendToCSG(const TCollection_AsciiString& theCSG)
{
    myCSG->Append(theCSG);
}


Handle(TColStd_HSequenceOfAsciiString) McCadCSGGeom_Cell::GetCSG() const
{
    return myCSG;
}


void McCadCSGGeom_Cell::SetSurfCounter(const Standard_Integer theSCount)
{
    mySurfCounter = theSCount;
}


Standard_Integer McCadCSGGeom_Cell::GetSurfCounter() const
{
    return mySurfCounter;
}


void McCadCSGGeom_Cell::SetTrsfCounter(const Standard_Integer theTCount)
{
    myTrsfCounter = theTCount;
}


Standard_Integer McCadCSGGeom_Cell::GetTrsfCounter() const
{
    return myTrsfCounter;
}


Standard_Integer McCadCSGGeom_Cell::GetNbOfSurf() const
{
    return mySurfaces.Extent();
}


Standard_Integer McCadCSGGeom_Cell::GetNbOfSurfTrsf() const
{
    McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface theIter;
    Standard_Integer iCount=0;

    for (theIter.Initialize(mySurfaces) ; theIter.More() ; theIter.Next())
    {
        if ((theIter.Value())->HaveTransformation())
        {
            iCount++;
        }
    }
    return iCount;
}


void McCadCSGGeom_Cell::SetComment(const TCollection_AsciiString& theComment)
{
    myComment = theComment;
}


TCollection_AsciiString McCadCSGGeom_Cell::GetComment() const
{
    return myComment;
}


void McCadCSGGeom_Cell::PrintComment(Standard_OStream& theStream)
{
    if(myMCType == McCadCSGGeom_TRIPOLI)
    {
        theStream << "// " << (myComment).ToCString() << endl;
    }
    else
        theStream << (myComment).ToCString() << "  " << endl;
}


void McCadCSGGeom_Cell::SetComplementCells(const Handle(TColStd_HSequenceOfInteger)& theIntSeq)
{
    myComplementCells->Clear();
    myComplementCells->Append(theIntSeq);
}

void McCadCSGGeom_Cell::SetVoid(const Standard_Boolean& isVoid)
{
    myIsVoid = isVoid;
}

void McCadCSGGeom_Cell::SetOuterVoid(Standard_Boolean isVoid)
{
    myIsOuterVoid = isVoid;
}

Standard_Boolean McCadCSGGeom_Cell::IsVoid()
{
    return myIsVoid;
}

Handle(TColStd_HSequenceOfInteger) McCadCSGGeom_Cell::GetComplementCells() const
{
    return myComplementCells;
}

void McCadCSGGeom_Cell::SetMCType(const McCadCSGGeom_MCType& theMCType)
{
    myMCType = theMCType;
}

McCadCSGGeom_MCType McCadCSGGeom_Cell::GetMCType()
{
    return myMCType;
}

void McCadCSGGeom_Cell::SetVolume(const Standard_Real& theVol)
{
    myVolume = theVol;
}

Standard_Real McCadCSGGeom_Cell::GetVolume()
{
    return myVolume;
}
