#include <McCadMcWrite_TripoliInputGenerator.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>

#include <McCadCSGGeom_Surface.hxx>
#include <McCadCSGGeom_Cell.hxx>
#include <McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <OSD_Path.hxx>
#include <fstream>
#include <McCadCSGGeom_MCType.hxx>


McCadMcWrite_TripoliInputGenerator::McCadMcWrite_TripoliInputGenerator()
{
	myNbMatCells = 0;
	myNbVoidCells = 0;
	myNbOuterCells = 0;
	myNbTrsfCards = 0;
	myNbSurfCards = 0;
	myIsHeaded = false;
	myIsNumbered = false;
	myHeader = new TColStd_HSequenceOfAsciiString;
	myCellSeq = NULL;
	myInitSurfNb = 0;
	myInitCellNb = 0;
}


McCadMcWrite_TripoliInputGenerator::McCadMcWrite_TripoliInputGenerator(const Handle(McCadCSGGeom_HSequenceOfCell)& theCellSeq)
{
	myCellSeq = theCellSeq;
	myNbMatCells = 0;
	myNbVoidCells = 0;
	myNbOuterCells = 0;
	myNbTrsfCards = 0;
	myNbSurfCards = 0;
	myIsHeaded = false;
	myIsNumbered = false;
	myHeader = new TColStd_HSequenceOfAsciiString;
	myInitSurfNb = 0;
	myInitCellNb = 0;
}


void McCadMcWrite_TripoliInputGenerator::SetInitialSurfaceNumber(Standard_Integer& theInt)
{
	myInitSurfNb = theInt;
}

void McCadMcWrite_TripoliInputGenerator::SetInitialCellNumber(Standard_Integer& theInt)
{
	myInitCellNb = theInt;
}


void McCadMcWrite_TripoliInputGenerator::SetCells(const Handle(McCadCSGGeom_HSequenceOfCell)& theCells)
{
	myCellSeq = theCells;
}


Handle(McCadCSGGeom_HSequenceOfCell) McCadMcWrite_TripoliInputGenerator::GetCells() const
{
	return myCellSeq;
}


void McCadMcWrite_TripoliInputGenerator::SetNbOfMatCells(const Standard_Integer theNumber)
{
	myNbMatCells = theNumber;
}


Standard_Integer McCadMcWrite_TripoliInputGenerator::GetNbOfMatCells() const
{
	return myNbMatCells;
}

void McCadMcWrite_TripoliInputGenerator::SetNbOfVoidCells(const Standard_Integer theNumber)
{
	myNbVoidCells = theNumber;
}


Standard_Integer McCadMcWrite_TripoliInputGenerator::GetNbOfVoidCells() const
{
	return myNbVoidCells;
}


void McCadMcWrite_TripoliInputGenerator::SetNbOfOuterCells(	const Standard_Integer theNumber)
{
	myNbOuterCells = theNumber;
}


Standard_Integer McCadMcWrite_TripoliInputGenerator::GetNbOfOuterCells() const
{
	return myNbOuterCells;
}


void McCadMcWrite_TripoliInputGenerator::MakeHeader()
{
	if (myHeader->Length() == 0)
	{
		myHeader->Append("COMMENT");
		myHeader->Append("          ------ Volumes  ------- ");
		myHeader->Append(TCollection_AsciiString("          ------ Material ---------- ") + TCollection_AsciiString(myNbMatCells));
		myHeader->Append(TCollection_AsciiString("          ------ Void -------------- ") + TCollection_AsciiString(myNbVoidCells));
		myHeader->Append("COMMENT \n\n");
	}
	myIsHeaded = true;
}


void McCadMcWrite_TripoliInputGenerator::AppendToHeader(const TCollection_AsciiString& theText)
{
	myHeader->Append(theText);
}


void McCadMcWrite_TripoliInputGenerator::SurfNumbering()
{
	myIsNumbered = true;
	Standard_Integer icell=0, isurf=0;

	//modify isurf to manipulate start value of surface counter
	isurf = myInitSurfNb;
	icell = myInitCellNb;
	//if cell counter is modified take also care of the complement cells in (this)->PrintCell()

	McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface gloIter;
	ofstream volOut(".cadVolumes");
	if(!volOut)
		cout << "Could not open file \".cadVolumes\" to save the CAD-Cell volumes \n\n";
	volOut << "Solid" << "   " << "Volume\n==========================\n";

	///////////////////////////////////////////////////////////////////
	for (Standard_Integer i=1; i<= myCellSeq->Length(); i++) //For all cells ...
	{
		McCadCSGGeom_DataMapOfIntegerSurface locSurfMap;
		TColStd_DataMapOfIntegerInteger surfNumMap;
		McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface locIter;

		Handle(McCadCSGGeom_Cell) curCell = myCellSeq->Value(i);

		curCell->SetCellNumber(icell+i);
		curCell->SetSurfCounter(0);

		if(!curCell->IsVoid())
			volOut << icell+i << "  \t"  << setw(10) << curCell->GetVolume() << endl;
		///////////////////////////////////////////////////////////////////
		locIter = curCell->GetSurface();
		Standard_Integer gloSurfNum, locSurfNum;
		Standard_Boolean isFound = Standard_False;

		for (; locIter.More() ; locIter.Next())//For all surfaces of current cell
		{
			Handle(McCadCSGGeom_Surface) locSurf = locIter.Value();//current surface

			locSurfNum = locIter.Key();//get local surface number

			for (gloIter.Initialize(myGlSurfaceMap); gloIter.More(); gloIter.Next())
			{
				Handle(McCadCSGGeom_Surface) gloSurf = gloIter.Value();
				if(gloSurf->IsEqual(locSurf))
				{
					isFound = Standard_True;
					gloSurfNum = gloIter.Key();
					break;
				}
				else isFound = Standard_False;
			}
			if (isFound)
			{
				surfNumMap.Bind(locSurfNum,gloSurfNum);
			}
			else
			{
				isurf++;
				gloSurfNum = isurf;
				locSurf->SetNumber(gloSurfNum);
				myGlSurfaceMap.Bind(gloSurfNum,locSurf);

				surfNumMap.Bind(locSurfNum,gloSurfNum);
			}
		}
		////////////////////////////////////////////////////////////////////
		// update the CSG expression
		Handle(TColStd_HSequenceOfAsciiString) orCSG;
		Handle(TColStd_HSequenceOfAsciiString) modCSG = new TColStd_HSequenceOfAsciiString;

		orCSG = curCell->GetCSG();

		for(int j=1;j<= orCSG->Length();j++)
		{
			if((orCSG->Value(j)).IsIntegerValue())
			{
				Standard_Integer orSurfNum = (orCSG->Value(j)).IntegerValue();
				Standard_Integer absOrSurfNum = abs(orSurfNum);
				Standard_Integer glSurfNum = surfNumMap(absOrSurfNum);

				glSurfNum = abs(glSurfNum)*(orSurfNum/absOrSurfNum); // sign setting
				modCSG->Append(glSurfNum);
			}
			else modCSG->Append(orCSG->Value(j));
		}
		/////////////////////////////////////////////////////////////
		curCell->SetCSG(modCSG);
	}
}


void McCadMcWrite_TripoliInputGenerator::PrintHeader(Standard_OStream& theStream)
{
	if (myIsHeaded) {
		for (Standard_Integer i=1; i<= myHeader->Length(); i++)
		{
			theStream << myHeader->Value(i) << endl;
		}
	} else {
		MakeHeader();
		PrintHeader(theStream);
	}
}


void McCadMcWrite_TripoliInputGenerator::PrintCell(Standard_OStream& theStream)
{
	if (myIsNumbered)
	{
		Standard_Boolean haveVoidCommentPrinted = Standard_False;

		theStream << "// ------ Volumes --------" << endl;

		for (Standard_Integer i=1; i<= myCellSeq->Length(); i++)
		{
			Handle(McCadCSGGeom_Cell) theCell = myCellSeq->Value(i);
			theCell->SetMCType(McCadCSGGeom_TRIPOLI);

			Handle(TColStd_HSequenceOfInteger) theComplementCells = theCell->GetComplementCells();
			if(theComplementCells->Length() > 0 )
			{
				TCollection_AsciiString aCSG(" ");
				for (Standard_Integer ii=1; ii<= theComplementCells->Length(); ii++)
				{
					TCollection_AsciiString tmpCSG(" ");
					tmpCSG.InsertAfter(tmpCSG.Length(), "#");
					tmpCSG.InsertAfter(tmpCSG.Length(), TCollection_AsciiString(myInitCellNb +  theComplementCells->Value(ii)));
					tmpCSG.InsertAfter(tmpCSG.Length(), " "); // we close if com.
					aCSG.InsertAfter(aCSG.Length(),tmpCSG);
				}
				theCell->AppendToCSG(aCSG);
			}

			if(!haveVoidCommentPrinted)
				theCell->PrintComment(theStream); //print comment of cell

			if(theCell->IsVoid())
				haveVoidCommentPrinted = Standard_True;
			else
				haveVoidCommentPrinted = Standard_False;

			theStream << setw(8) << "   VOLU " << theCell->GetCellNumber() << " EQUA";
			theCell->PrintCell(theStream);
			theStream << "\tENDV\n";
		}
	}
	else
		cout << "_#_McCadMcWrite_TripoliInputGenerator.cxx :: Can not print cells due to missing numbering !!"<< endl;
}


void McCadMcWrite_TripoliInputGenerator::PrintSurface(Standard_OStream& theStream)
{
	if (myIsNumbered)
	{
		//////////////////////////////////////////////////////////////////////////////
		theStream << "                         "<< endl;
		theStream << "// ------ Surfaces -------- "<< endl;
		int isurf=0;
		// we put the global surface map into the first cell use its print function
		(myCellSeq->Value(1))->SetSurface(myGlSurfaceMap);
		(myCellSeq->Value(1))->SetSurfCounter(isurf);
		(myCellSeq->Value(1))->SetMCType(McCadCSGGeom_TRIPOLI);
		(myCellSeq->Value(1))->PrintSurfaces(theStream);
	}
	else
		cout << "_#_McCadMcWrite_TripoliInputGenerator.cxx :: Can not print surface due to missing numbering !!"<< endl;

}


void McCadMcWrite_TripoliInputGenerator::PrintTrsf(Standard_OStream& theStream)
{
	if (myIsNumbered)
	{
	//	theStream << "                         "<< endl;
		int itrsf=0;
		// we print here the trsf matrices
		(myCellSeq->Value(1))->SetTrsfCounter(itrsf);
		itrsf += (myCellSeq->Value(1))->GetNbOfSurfTrsf(); // good for nothing
		(myCellSeq->Value(1))->PrintSurfaceTrsfMat(theStream);
	//	theStream << "                         "<< endl;
	} else
		cout
				<< "_#_McCadMcWrite_TripoliInputGenerator.cxx :: Can not print surface transformation cards due to missing numbering !!" << endl;
}


void McCadMcWrite_TripoliInputGenerator::PrintAllCells(Standard_OStream& theStream)
{
	theStream << "imp:n " << myNbMatCells + myNbVoidCells + myNbOuterCells << endl;
}


void McCadMcWrite_TripoliInputGenerator::PrintAll(Standard_OStream& theStream)
{
	PrintHeader(theStream);
	PrintSurface(theStream);
	PrintCell(theStream);
	//PrintTrsf(theStream);
}


void McCadMcWrite_TripoliInputGenerator::PrintAll(OSD_File& theFile)
{
	OSD_Path thePath;
	theFile.Path(thePath);
	TCollection_AsciiString theName = thePath.Name() + thePath.Extension();
        const char* strName = theName.ToCString();
	ofstream theStream(strName);

	PrintHeader(theStream);
	theStream << "GEOMETRY\n";
	theStream << "\t  TITLE \t >>> McCad generated Input <<<\n"; // TITLE is mandatory!!!
	PrintSurface(theStream);
	PrintCell(theStream);
	theStream << "ENDGEOM\n";

	/*PrintMaterial(theStream);
	PrintTrsf(theStream);
	PrintAllCells(theStream);*/
}

void McCadMcWrite_TripoliInputGenerator::PrintMaterial(Standard_OStream& theStream)
{
	Handle(TColStd_HSequenceOfInteger) numSeq = myMDReader.GetMCardNumbers();
	theStream << "                         "<< endl;

	//rank numbers and delete multiple MCards
	/////////////////////////////////////////
	for(Standard_Integer i=1; i < numSeq->Length(); i++)
	{
		Standard_Integer iVal = numSeq->Value(i);

		for(Standard_Integer j=numSeq->Length(); j > i; j--)
		{
			Standard_Integer jVal = numSeq->Value(j);

			if(iVal == jVal)
				numSeq->Remove(j);

			if(iVal > jVal)
			{
				numSeq->Exchange(i,j);
				i--;
				break;
			}

		}
	}

	//print material cards
	////////////////////////////////////////
	for(Standard_Integer i=1; i<=numSeq->Length(); i++)
	{
		TCollection_AsciiString curMCard;
		myMDReader.GetMCard(numSeq->Value(i), curMCard);
		theStream << curMCard.ToCString();
		theStream << endl;
	}
}

void McCadMcWrite_TripoliInputGenerator::SetMDReader(const McCadMDReader_Reader& theMDReader)
{
	myMDReader = theMDReader;
}
