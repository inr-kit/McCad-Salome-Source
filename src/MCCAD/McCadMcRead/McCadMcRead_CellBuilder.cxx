#include <McCadMcRead_CellBuilder.ixx>
#include <McCadBoolExp_PostFixer.hxx>

#include <string>

#include <TColgp_HSequenceOfPnt.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <gp_Pnt.hxx>

#include <McCadMcRead_TrsfBuilder.hxx>
#include <McCadBoolExp_PostFixer.hxx>
#include <McCadMcRead_FillBuilder.hxx>
#include <McCadMcRead_Fill.hxx>
#include <McCadMcRead_HSequenceOfFill.hxx>
#include <McCadCSGGeom_Transformation.hxx>

#include <McCadMessenger_Singleton.hxx>

McCadMcRead_CellBuilder::McCadMcRead_CellBuilder()
{

	myCell = new McCadMcRead_Cell;
	NotDone();

}


Standard_Boolean McCadMcRead_CellBuilder::IsDone() const
{
	return myIsDone;
}


void McCadMcRead_CellBuilder::Done()
{
	myIsDone = Standard_True;
}


void McCadMcRead_CellBuilder::NotDone()
{
	myIsDone = Standard_False;
}


void McCadMcRead_CellBuilder::BuildCell(const TCollection_AsciiString& sDesc)
{
        McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();
	if (!Parse(sDesc))
	{
                msgr->Message("_#_McCadMcRead_CellBuilder.cxx :: Can not build Cell for this Card : ",
                              McCadMessenger_ErrorMsg);
                msgr->Message(sDesc.ToCString(), McCadMessenger_ErrorMsg);
		NotDone();
                return;
	}



	Done();
}


Standard_Boolean McCadMcRead_CellBuilder::Parse(const TCollection_AsciiString& sDesc)
{
	//	cout << "1\n";
	if (!ExtractNo(sDesc)) //Extract Cell No;
		return false;

	//	cout << "2\n";
	if (!ExtractLIKE(sDesc)) //Extract like but card;
		return false;

	//	cout << "3\n";
	if (!myCell->IsLike())
		if (!ExtractMat(sDesc))
			return false;

	//	cout << "4\n";
	if (!ExtractTRCL(sDesc))
		return false;

	//	cout << "5\n";
	if (!ExtractUCard(sDesc))
		return false;

	cout << "6\n";
	if (!ExtractFill(sDesc)) // pure fill cards are extracted !!
		return false;

	cout << "7\n";
	if (!ExtractLatFill(sDesc)) // lat with fill cards are extracted !!
		return false;

	cout << "8\n";
	if (!myCell->IsLike())
		if (!ExtractBOOLEXPR(sDesc))
			return false;
	//	cout << "...done\n";

	return true;
}


Standard_Boolean McCadMcRead_CellBuilder::ExtractNo(const TCollection_AsciiString& sDesc)
{
        McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
	TCollection_AsciiString tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search(" "));

	Standard_Integer num = tmpStr.IntegerValue();
	if (num <= 0)
	{
                TCollection_AsciiString errormsg("_#_McCadMcRead_CellBuilder.cxx :: The cell number for this Card is less than 0! : ");
                errormsg += sDesc;
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

		return false;
	}
	else
		myCell->SetCellNumber(num);



	return true;
}

Standard_Boolean McCadMcRead_CellBuilder::ExtractMat(const TCollection_AsciiString& sDesc)
{
        McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();
	TCollection_AsciiString tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search(" "));
	sEnd.LeftAdjust();
	sEnd.RightAdjust();
	TCollection_AsciiString sEnd2 = sEnd.Split(sEnd.Search(" "));


	// sEnd is now the mat number
	int iMat = sEnd.IntegerValue();
	if (iMat < 0)
	{
                TCollection_AsciiString errormsg("_#_McCadMcRead_CellBuilder.cxx :: The material number for this Card is less than 0! : ");
                errormsg += sDesc;
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

		return false;
	}
	else if (iMat > 0 ) // we have density
	{
		myCell->SetMaterialNumber(iMat);
		// we go for the density
		sEnd.Clear();
		sEnd2.LeftAdjust();
		sEnd2.RightAdjust();
		sEnd = sEnd2.Split(sEnd2.Search(" "));
		// sEnd2 is now the density number
		if (!sEnd2.IsRealValue())
		{
//			msgr << "Missing density for material cell ... setting density to 1e-9 for cell : ";
//			msgr << myCell->GetCellNumber();
			myCell->SetDensity(0.000000001);
		}
		else
		{
			if (sEnd2.RealValue() == 0 )
			{
                                        TCollection_AsciiString warningmsg("Warning: The material density is 0 for cell : ");
                                        warningmsg += myCell->GetCellNumber();
                                        msgr->Message(warningmsg.ToCString(), McCadMessenger_WarningMsg);
			}
			myCell->SetDensity(sEnd2.RealValue());
		}
	}
	else // iMat = 0  void case
		myCell->SetMaterialNumber(iMat);



	return true;
}


Standard_Boolean McCadMcRead_CellBuilder::ExtractTRCL(const TCollection_AsciiString& sDesc)
{
        McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
	TCollection_AsciiString tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();

	int iTRCL = tmpStr.Search("TRCL");

	if (iTRCL > 0)
	{
		myCell->SetHaveTRCL(true);
		// we get either num or matrix
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("TRCL")+4);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();

		TCollection_AsciiString sEnd3;
		// sEnd3 is a number or (
		if (sEnd.Search("(") < 0)
		{
			// we have a number here
			if(sEnd.Search(" ") > 0)
				sEnd3 = sEnd.SubString(1, sEnd.Search(" "));
			else
				sEnd3 = sEnd;

			sEnd3.LeftAdjust();
			sEnd3.RightAdjust();

			if (sEnd3.IntegerValue() <= 0)
			{
                                TCollection_AsciiString warningmsg("TRCL number for this Card is less than 0 for cell : ");
                                warningmsg += myCell->GetCellNumber();
                                msgr->Message(warningmsg.ToCString(), McCadMessenger_WarningMsg);

				return false;
			}
			else
			{
				myCell->SetTRCLNumber(sEnd3.IntegerValue());

				return true;
			}
		}
		else // we have a matrix
		{
			sEnd3 = sEnd.SubString(sEnd.Search("(")+1,sEnd.Search(")")-1);
			// sEnd3 contains the vector
			if(sDesc.Search("*TRCL") > 0 )
				sEnd3.InsertBefore(1, TCollection_AsciiString("*TR1000  "));
			else
				sEnd3.InsertBefore(1, TCollection_AsciiString("TR1000  "));

			// bulid the matrix
			McCadMcRead_TrsfBuilder TrBuilder;
			TrBuilder.BuildTrsf(sEnd3);
			myCell->SetTRCLNumber(1000); // 1000 is a nonnumber of TR cards
			myCell->SetTrsf(TrBuilder.GetTrsf());

			return true;
		}
	}
	else myCell->SetHaveTRCL(false);



	return true;
}

Standard_Boolean McCadMcRead_CellBuilder::ExtractBOOLEXPR(const TCollection_AsciiString& sDesc)
{
        McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();
	// extract macros first!
	TCollection_AsciiString tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	TCollection_AsciiString sEnd=(" ");

	if (tmpStr.Search("$") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("$")-1);
	if (tmpStr.Search("IMP") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("IMP")-1);
	if (tmpStr.Search("VOL") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("VOL")-1);
	if (tmpStr.Search("PWT") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("PWT")-1);
	if (tmpStr.Search("EXT") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("EXT")-1);
	if (tmpStr.Search("FCL") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("FCL")-1);
	if (tmpStr.Search("WWN") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("WWN")-1);
	if (tmpStr.Search("DXC") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("DXC")-1);
	if (tmpStr.Search("NONU") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("NONU")-1);
	if (tmpStr.Search("PD") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("PD")-1);
	if (tmpStr.Search("TMP") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("TMP")-1);
	if (tmpStr.Search("U") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("U")-1);
	if (tmpStr.Search("TRCL") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("TRCL")-1);
	if (tmpStr.Search("LAT") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("LAT")-1);
	if (tmpStr.Search("FILL") > 0)
		sEnd = tmpStr.Split(tmpStr.Search("FILL")-1);

	sEnd.Clear();
	sEnd = tmpStr.Split(tmpStr.Search(" ")); //tmpStr is cellnum
	sEnd.LeftAdjust();
	sEnd.RightAdjust();
	TCollection_AsciiString sEnd2 = sEnd.Split(sEnd.Search(" ")); // sEnd is matNum
	sEnd2.LeftAdjust();
	sEnd2.RightAdjust();
	tmpStr.Clear();
	tmpStr = sEnd2;
	if (sEnd.IntegerValue() > 0) // we have density
	{
		// we go for the density
		tmpStr.Clear();
		tmpStr = sEnd2.Split(sEnd2.Search(" "));
		// sEnd2 is density number
	}
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	// at this point we should have only geometry discription on the card tmpStr.
	// we check this
	int i =1;
	while (i <= tmpStr.Length())
	{
		Standard_Character aC = tmpStr.Value(i);
		if (isdigit(aC) || isspace(aC) || (aC == '-') || (aC == '+') || (aC == ':') || (aC == '#') || (aC == '(') || (aC == ')') || aC == '*')
		{
			i++;
			continue;
		}
		else
		{
                        TCollection_AsciiString errormsg("Wrong cell geometry description !! : ");
                        errormsg += tmpStr;
                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

			return false;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////
	// macro cell extraction
	// #num is a macro
	sEnd.Clear();
	sEnd2.Clear();
	sEnd = tmpStr;

	if(sEnd.Search("#") > 0 )
	{
		Handle(TColStd_HSequenceOfInteger) CompsNum = new TColStd_HSequenceOfInteger;
		while (sEnd.Search("#") > 0 )
		{
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
			sEnd2 = sEnd.Split(sEnd.Search("#"));
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
			sEnd2.LeftAdjust();
			sEnd2.RightAdjust();
			if(sEnd2.Search(" ") < 0 && sEnd2.IsIntegerValue() ) //single word
			{
				//  cout << "TEEEEEEE" << endl;
				//  cout << sEnd2.ToCString() << endl;
				CompsNum->Append(sEnd2.IntegerValue());
				sEnd.Remove(sEnd.Search("#"),1);
				tmpStr.Clear();
				tmpStr = sEnd;
			}
			else
			{
				sEnd = sEnd2.Split(sEnd2.Search(" "));
				if (sEnd2.IsIntegerValue()) // we have a number
				{
					CompsNum->Append(sEnd2.IntegerValue());
					sEnd2.Clear();
					sEnd.Clear();
					sEnd = tmpStr;
					sEnd2 = sEnd.Split(sEnd.Search("#"));
					sEnd.Remove(sEnd.Search("#"),1);
					sEnd2.LeftAdjust();
					sEnd2.RightAdjust();
					TCollection_AsciiString sEnd3 = sEnd2.Split(sEnd2.Search(" "));
					tmpStr.Clear();
					tmpStr = sEnd + " " + sEnd3;
					sEnd = tmpStr;
				}
				else // we have a bracket we mark the comp. sign
				{
					int id1 = tmpStr.Search("#");
					tmpStr.Remove(id1,1);
					tmpStr.Insert(id1,"!");
					sEnd = tmpStr;
				}
			}
			sEnd2.Clear();
		}
		//replace the exclamation mark
		/////////////////////////////////////////////////////////////////////
		while (tmpStr.Search("!") > 0 )
		{
			int id1 = tmpStr.Search("!");
			tmpStr.Remove(id1,1);
			tmpStr.Insert(id1,"#");
		}
		if(CompsNum->Length() > 0 )
		{
			myCell->SetMacroIndex(CompsNum);
			myCell->SetHaveMacro(true);
		}
		else
		{
			myCell->SetHaveMacro(false);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////
	// post fix boolean forms
	if(tmpStr.Length()<1)
		return Standard_True;

	McCadBoolExp_PostFixer thePostFixer;
	thePostFixer.Init(tmpStr);
	if(!thePostFixer.IsDone())
	{
                TCollection_AsciiString errormsg("_#_McCadMcRead_CellBuilder.cxx :: Generating postfix CSG expression failed for the following input: ");
                errormsg += tmpStr;
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

		return false;
	}
	Handle(TColStd_HSequenceOfAsciiString) theInPutCSG = thePostFixer.GetInput();
	myCell->SetCSG(theInPutCSG); // this is the raw mcnp csg!!

	//cout << "THE INPUT CSG : ";  for(int i=1; i<=theInPutCSG->Length(); i++) cout << theInPutCSG->Value(i) << " "; cout << endl;
	Handle(TColStd_HSequenceOfAsciiString) theInFixCSG = thePostFixer.GetInFix();
	myCell->SetInFixCSG(theInFixCSG); // this is the raw csg in infix with intersection added!!
	Handle(TColStd_HSequenceOfAsciiString) thePostFixCSG = thePostFixer.GetPostFix();
	myCell->SetPostFixCSG(thePostFixCSG);



	return true;
}


Standard_Boolean McCadMcRead_CellBuilder::ExtractLIKE(const TCollection_AsciiString& sDesc)
{
    McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
	TCollection_AsciiString tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	int iLike = tmpStr.Search("LIKE");

	if (iLike > 0) // we have like but
	{
		myCell->SetHaveLike(true);
		TCollection_AsciiString sEnd = tmpStr.SubString(
				tmpStr.Search("LIKE")+4, tmpStr.Search("BUT")-1);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		int num = sEnd.IntegerValue();
		if (num <= 0)
		{
                        TCollection_AsciiString errormsg("The LIKE cell number is less than 0 for cell : ");
                        errormsg += myCell->GetCellNumber();
                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

			return false;
		}
		else myCell->SetLikeNum(num);
		// set also MATnum and RHO here if available !!
		sEnd.Clear();

		TCollection_AsciiString tmptmpStr = tmpStr;

		if (tmpStr.Search("MAT") > 0 ) // mnemonics is MAT = num and RHO = num
		{
			sEnd = tmpStr.Split(tmpStr.Search("MAT")+3);
			sEnd.LeftAdjust(); sEnd.RightAdjust();
			// now the first element should be = sign.

			if(tmpStr.Search("=") < 0 )
			{
                                TCollection_AsciiString errormsg("MAT number of the LIKE_BUT Card not available for cell : ");
                                errormsg += myCell->GetCellNumber();
                                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

				return false;
			}
			else
			{
				TCollection_AsciiString sStr;
				sEnd.LeftAdjust();
				sEnd.RightAdjust();
				sStr = sEnd;

				if(sEnd.Search(" ") > 0)
					sStr = sStr.SubString(1, sEnd.Search(" "));

				if (sStr.IntegerValue() <=0)
				{
                                        TCollection_AsciiString errormsg("MAT number of the LIKE_BUT Card is less than 0 for cell : ");
                                        errormsg += myCell->GetCellNumber();
                                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

					return false;
				}
				else
					myCell->SetMaterialNumber(sStr.IntegerValue());
			}
		}
		else
			myCell->SetMaterialNumber(-1);

		// RHO
		sEnd.Clear();
		tmpStr = tmptmpStr;

		if (tmpStr.Search("RHO") > 0) // mnemonics is MAT = num and RHO = num
		{
			sEnd = tmpStr.Split(tmpStr.Search("RHO")+3);
			sEnd.LeftAdjust(); sEnd.RightAdjust();
			// now the first element should be = sign.
			if(tmpStr.Search("=") < 0 )
			{
                                TCollection_AsciiString errormsg("RHO number of the LIKE_BUT Card is not available for cell : ");
                                errormsg += myCell->GetCellNumber();
                                msgr->Message(errormsg.ToCString(),McCadMessenger_ErrorMsg);

				return false;
			}
			else
			{
				TCollection_AsciiString sStr=sEnd;
				if(sEnd.Search(" ") > 0)
					sStr = sStr.SubString(1, sEnd.Search(" "));

				if (sStr.RealValue() == 0)
				{
                                        TCollection_AsciiString errormsg("MAT number of the LIKE_BUT Card is equal to 0 for cell : ");
                                        errormsg += myCell->GetCellNumber();
                                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

					return false;
				}
				else
					myCell->SetDensity(sStr.RealValue());
			}
		}
		else
			myCell->SetDensity(0.0);
	}
	else
	{

		myCell->SetHaveLike(false);
		return true;
	}




	return true;
}


Standard_Boolean McCadMcRead_CellBuilder::ExtractFill(const TCollection_AsciiString& sDesc)
{
        McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();

	TCollection_AsciiString tmpStr = sDesc;

	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	int iFill = tmpStr.Search("FILL");
	int iLat = tmpStr.Search("LAT");
	if (iFill > 0 && iLat > 0)
		return true; // we have lat fill will be processed otherwise.

	if (iFill <= 0) // we don't have a Fill card at all.
	{
		myCell->SetHaveFill(false);
		return true;
	}
	// process simple fill card
	myCell->SetHaveFill(true);
	// create a fill card
	TCollection_AsciiString fCard;
	if (tmpStr.Search("*FILL") > 0)
		fCard = TCollection_AsciiString("*FILL");
	else
		fCard = TCollection_AsciiString("FILL");

	TCollection_AsciiString theRest = tmpStr.Split(tmpStr.Search("FILL")+3);

	theRest.LeftAdjust();
	theRest.RightAdjust();
	// the first string should be = sign
	if (theRest.Search("=") != 1)
	{
                TCollection_AsciiString errormsg("Syntax error for cell : ");
                errormsg += myCell->GetCellNumber();
                errormsg += " !! Fill keyword should be followed by a '=' ";
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
		return false;
	}
	else
	{
		fCard += TCollection_AsciiString("=");
		theRest.Remove(1,1);
		// know we expect a ( or a number!
		theRest.LeftAdjust();
		theRest.RightAdjust();
		if (theRest.Search("(") > 0 )
		{ // we have trsfnum or matrix
			TCollection_AsciiString sMid = theRest.SubString(theRest.Search("("),theRest.Search(")"));
			fCard += sMid;

			// get the fill number
			sMid.Clear();
			sMid = theRest.Split(theRest.Search("(")+1);
			TCollection_AsciiString sMid2 = theRest.Split(sMid.Search(" "));

			if (theRest.IntegerValue() <= 0 )
			{
                                TCollection_AsciiString errormsg("Fill Number <= 0 for cell : ");
                                errormsg += myCell->GetCellNumber();
                                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
				return false;
			}
			else fCard += theRest;
		}
		else
		{
			if (theRest.IntegerValue() <= 0 )
			{
                            TCollection_AsciiString errormsg("Fill Number <= 0 for cell : ");
                                errormsg += myCell->GetCellNumber();
                                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
				return false;
			}
			else fCard += theRest;
		}
	}



	McCadMcRead_FillBuilder BuildFill;
	if(BuildFill.Build(fCard))
	{
		Handle(McCadMcRead_Fill) theFill = BuildFill.GetFiller();
		myCell->SetFill(theFill);
		return true;
	}
	else
		return false;
}


Standard_Boolean McCadMcRead_CellBuilder::ExtractLatFill(const TCollection_AsciiString& sDesc)
{
        McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();
	TCollection_AsciiString tmpStr = sDesc;

	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	int iFill = tmpStr.Search("FILL");
	int iLat = tmpStr.Search("LAT");
	if (iLat <= 0 || iFill <= 0)
	{
		myCell->SetHaveLat(false);
		return Standard_True; // simple cell
	}

//	cout << sDesc << endl;

	// now we know that we have lat keywords;
	TCollection_AsciiString theRest = tmpStr.Split(tmpStr.Search("LAT")+2);
	theRest.LeftAdjust();
	theRest.RightAdjust();

//	cout << "THE REST: " << theRest.ToCString() << "  TMP STR: " << tmpStr.ToCString() << endl;

	// the first string should be = sign
	if (theRest.Search("=") != 1 )
	{
                TCollection_AsciiString errormsg("Syntax error for cell : ");
                errormsg += myCell->GetCellNumber();
                errormsg += " !! Lat keyword should be followed by a '='!";
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
		return false;
	}
	theRest.Remove(1,1); // this removes the equal sign.
	theRest.LeftAdjust();
	theRest.RightAdjust();
	TCollection_AsciiString theRest2 = theRest.Split(theRest.Search(" "));

	//cout << "THE REST2: " << theRest2.ToCString() << "  THE REST: " << theRest.ToCString() << endl;

	if (theRest.IntegerValue() <= 0 )
	{
                TCollection_AsciiString errormsg("Lat Number <= 0 for cell : ");
                errormsg += myCell->GetCellNumber();
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
		return false;
	}
	myCell->SetHaveLat(Standard_True);
	myCell->SetLat(theRest.IntegerValue());

	///////////////////////////////////////////////////////
	//we go here for the fill card which is multi
	tmpStr.Clear();
	theRest.Clear();
	theRest2.Clear();
	tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();

	// process simple fill card
	//cout << "----------\n";
	myCell->SetHaveFill(true);
	// create a fill card
	TCollection_AsciiString fCard(" ");
	if (tmpStr.Search("*FILL") > 0 )
		fCard.Insert(fCard.Length(),"*FILL");
	else
		fCard.Insert(fCard.Length(),"FILL");
// cout << "1\n";
	theRest = tmpStr.Split(tmpStr.Search("FILL")+3);
	theRest.LeftAdjust();
	theRest.RightAdjust();
	// the first string should be = sign
	if (theRest.Search("=") != 1 )
	{
                TCollection_AsciiString errormsg("Syntax error for cell : ");
                errormsg += myCell->GetCellNumber();
                errormsg += " !! Fill keyword should be followed by a '='!";
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
		return false;
	}
// cout << "2\n";
	fCard.Insert(fCard.Length(),"=");
	theRest.Remove(1,1);

	// know we expect a ( or a number!
	theRest.LeftAdjust();
	theRest.RightAdjust();
	if (theRest.Search("(") == 1 )
	{ // with trsfnum or matrix
		TCollection_AsciiString sTrsf = theRest.SubString(theRest.Search("("),theRest.Search(")"));
		fCard.Insert(fCard.Length(),sTrsf);
	}
	//the fCard is now "*Fill=(num) and theRest is the rest.
	// we get dimension or compute it.
	theRest.LeftAdjust();
	theRest.RightAdjust();
	//////////////////////////////////////////////////
	// we check if we have the three index pairs!!
	// xindex

	Standard_Boolean infTest(Standard_False);

	if(theRest.Search(":") < 0)// infinite lattice or error
		infTest = Standard_True;
	else
	{
		TCollection_AsciiString tstStr = theRest;
		TCollection_AsciiString trash = tstStr.Split(tstStr.Search(":") - 1);

		tstStr.LeftAdjust();
		tstStr.RightAdjust();

		if(tstStr.Search(" ") > 0) // more than one integer between '=' and ':'
			infTest = Standard_True;
		else if(!tstStr.IsIntegerValue())
		{
                        TCollection_AsciiString errormsg("Expecting integer value after 'Fill=' in cell : ");
                        errormsg += myCell->GetCellNumber();
                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
			return Standard_False;
		}
	}

	if(infTest) // Infinite Lattice
	{
		cout << "INFINITE LATTICE FOUND\n\n";
		TCollection_AsciiString trash;
		if(theRest.Search(" ") > 0)
			trash = theRest.Split(theRest.Search(" "));
		else
			trash = theRest;

		theRest.LeftAdjust();
		theRest.RightAdjust();

		if(!theRest.IsIntegerValue())
		{
                        TCollection_AsciiString errormsg("Integer value after Fill statement expected!! Error in cell : ");
                        errormsg += myCell->GetCellNumber();
                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
			return Standard_False;
		}

		Standard_Integer fillNum = theRest.IntegerValue();
		Handle(McCadMcRead_Fill) fill = new McCadMcRead_Fill(fillNum);

		myCell->SetFill(fill);
		myCell->SetInfiniteLattice();

		return Standard_True;
	}

	TCollection_AsciiString sEnd = theRest.Split(theRest.Search(":")-1); // this is : 1 3:4 etc. and sTmp is an index;

	int xLindex = theRest.IntegerValue();

	sEnd.LeftAdjust();
	Standard_Character aC = sEnd.Value(1);
	if (aC != ':')
	{
                TCollection_AsciiString errormsg("Fill card x index entry is wrong for cell : ");
                errormsg += myCell->GetCellNumber();
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
		return false;
	}
	sEnd.Remove(1,1); // this removes ":"
	sEnd.LeftAdjust();
	int xUindex = (sEnd.SubString(1,sEnd.Search(" "))).IntegerValue();

	sEnd.Remove(1,sEnd.Search(" ")-1);
	sEnd.LeftAdjust();
	theRest.Clear();
	theRest = sEnd;
	sEnd.Clear();
	//y index
	sEnd = theRest.Split(theRest.Search(":")-1); // this is : 1 3:4 etc. and sTmp is an index;
	int yLindex = theRest.IntegerValue();
	sEnd.LeftAdjust();
	aC = sEnd.Value(1);
	if (aC != ':')
	{
                TCollection_AsciiString errormsg("Fill card y index entry is wrong for cell : ");
                errormsg += myCell->GetCellNumber();
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
		return false;
	}
	sEnd.Remove(1,1); // this removes ":"
	sEnd.LeftAdjust();
	int yUindex = (sEnd.SubString(1,sEnd.Search(" "))).IntegerValue();
	sEnd.Remove(1,sEnd.Search(" ")-1);
	sEnd.LeftAdjust();
	theRest.Clear();
	theRest = sEnd;
	sEnd.Clear();
	//z index
	sEnd = theRest.Split(theRest.Search(":")-1); // this is : 1 3:4 etc. and sTmp is an index;
	int zLindex = theRest.IntegerValue();
	sEnd.LeftAdjust();
	aC = sEnd.Value(1);
	if (aC != ':')
	{
                TCollection_AsciiString errormsg("Fill card z index entry is wrong for cell : ");
                errormsg += myCell->GetCellNumber();
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
		return false;
	}
	sEnd.Remove(1,1); // this removes ":"
	sEnd.LeftAdjust();
	int zUindex = (sEnd.SubString(1,sEnd.Search(" "))).IntegerValue();
	sEnd.Remove(1,sEnd.Search(" ")-1);
	sEnd.LeftAdjust();
	theRest.Clear();
	theRest = sEnd;
	sEnd.Clear();
	// we have now the dimension indices.
	Handle(TColgp_HSequenceOfPnt) theFillPlaceSeq = new TColgp_HSequenceOfPnt;

	Standard_Integer zCnt((zUindex - zLindex)+1), yCnt((yUindex - yLindex)+1), xCnt((xUindex - xLindex)+1);
	Standard_Integer dim = xCnt*yCnt*zCnt;
	// gp_Pnt place;

	Standard_Integer plcCnt(0);

	for(Standard_Integer k=1; k <= zCnt; k++)
	{
		for (Standard_Integer j=1;j <= yCnt; j++)
		{
			for (Standard_Integer i=1 ;i <= xCnt; i++)
			{
				plcCnt++;
				gp_Pnt place(xLindex+i,yLindex+j,zLindex+k);
				theFillPlaceSeq->Append(place);
			}
		}
	}
/*cout << "number of places : " << plcCnt << endl;
cout << "##### > " << theFillPlaceSeq->Length() << endl;*/

	myCell->SetFillPlaceSeq(theFillPlaceSeq);

	//////////////////////////////////////////////////////////////////////////////////////
	// we create the fill card for each entry and append it to fCard and make a fill object for each;
	//  theRest contain the vector and fCard is *Fill = ( trsf )
	// if we find a trsf we replace trsf after fill
	Handle(McCadMcRead_HSequenceOfFill) theFillSeq = new McCadMcRead_HSequenceOfFill;
	theRest.LeftAdjust();
	theRest.RightAdjust();
	theRest += " ";

	// resolve repeat operator R
	theRest.UpperCase();

	if(theRest.Search("R") > 0)
	{
		TCollection_AsciiString newRest, previousFillNumber(-1);
		while(true)
		{
			TCollection_AsciiString tail;

			if(theRest.Search(" ") > 0)
			{
				tail = theRest.Split(theRest.Search(" "));
				tail.LeftAdjust();

				if(theRest.Search("R") < 0)
				{
					previousFillNumber = theRest;
					newRest += previousFillNumber;
				}
				else
				{
					TCollection_AsciiString theR = theRest.Split(theRest.Search("R"));
					if(!theRest.IsIntegerValue())
					{
						cout << "ERROR :  integer value expected before repeate operator nR in cell " << myCell->GetCellNumber() << endl;
						return false;
					}

					Standard_Integer numberOfRepeats = theRest.IntegerValue();

					for(int i=1; i<=numberOfRepeats; i++)
					{
						newRest += previousFillNumber;
					}
				}
			}
			else
			{
				if(theRest.Search("R") > 0)
				{
					TCollection_AsciiString theR = theRest.Split(theRest.Search("R"));
					if(!theRest.IsIntegerValue())
					{
						cout << "ERROR :  integer value expected before repeate operator nR in cell " << myCell->GetCellNumber() << endl;
						return false;
					}

					Standard_Integer numberOfRepeats = theRest.IntegerValue();

					for(int i=1; i<=numberOfRepeats; i++)
					{
						newRest += previousFillNumber;
					}
				}
				else
					newRest.Cat(theRest);
				break;
			}

			theRest = tail;
		}

		theRest = newRest + TCollection_AsciiString(" ");
	}

	//cout << "Dimension : " << dim << endl;
	for (int l = 1; l <= dim; l++)
	{
		TCollection_AsciiString fCard2 = fCard;
		TCollection_AsciiString sTrsf(" ");
		TCollection_AsciiString stmp, sMid;
		sMid = theRest;

		theRest  = sMid.Split(sMid.Search(" "));
		theRest.LeftAdjust();

		// sMid can be num or  num( or num(trsf or num(trsf)
		if(sMid.Search("(") > 0 )
		{
			sTrsf += theRest.SubString(theRest.Search("("),theRest.Search(")"));
			sMid = theRest.SubString(1,theRest.Search("(")-1);
			stmp = theRest.Split(sMid.Search(" "));
			theRest.Clear();
			theRest = stmp;
		}

		if (sMid.IntegerValue() <= 0 )
		{
                        TCollection_AsciiString errormsg("_#_McCadMcRead_CellBuilder.cxx :: Fill Number <= 0 for cell : ");
                        errormsg += myCell->GetCellNumber();
                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

			return false;
		}

		fCard2.Insert(fCard2.Length(),sTrsf);
		fCard2.Insert(fCard2.Length(),sMid);


		McCadMcRead_FillBuilder BuildFill;
		if(BuildFill.Build(fCard2))
		{
			Handle(McCadMcRead_Fill) theFill = BuildFill.GetFiller();
			theFillSeq->Append(theFill);
		}
		else
		{
                    TCollection_AsciiString errormsg("Fill Card building failed for cell : ");
                        errormsg += myCell->GetCellNumber();
                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

			return false;
		}
	}



	myCell->SetFillSeq(theFillSeq);
	return true;
}


Standard_Boolean McCadMcRead_CellBuilder::ExtractUCard(const TCollection_AsciiString& sDesc)
{
        McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
	TCollection_AsciiString tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	Standard_Integer iU = tmpStr.Search(" U");
	if (iU > 0) // we are looking for U = num mnemonics
	{
		myCell->SetHaveU(true);
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("U"));
		sEnd.LeftAdjust();
		sEnd.RightAdjust();

		TCollection_AsciiString sEnd2;

		if(sEnd.Search(" ")>0)
			sEnd2 = sEnd.SubString(sEnd.Search("=")+1, sEnd.Search(" "));
		else
			sEnd2 = sEnd.SubString(sEnd.Search("=")+1, sEnd.Length());

		if (sEnd2.IntegerValue() <= 0)
		{
                    TCollection_AsciiString errormsg("Universe Card with non positive entry in cell : ");
                        errormsg += myCell->GetCellNumber();
                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

			return false;
		}
		else
		{
			myCell->SetUNum(sEnd2.IntegerValue());
			return true;
		}
	}
	else
	myCell->SetHaveU(false);

	return true;
}


Handle(McCadMcRead_Cell) McCadMcRead_CellBuilder::GetCell() const
{
	return myCell;
}

