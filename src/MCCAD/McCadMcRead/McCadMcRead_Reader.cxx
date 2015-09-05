#include <McCadMcRead_Reader.ixx>

#include <string>
#include <algorithm>
#include <iterator>

#include <McCadMcRead_Cell.hxx>
#include <McCadMcRead_DataMapOfIntegerCell.hxx>
#include <McCadMcRead_DataMapIteratorOfDataMapOfIntegerCell.hxx>
#include <McCadMcRead_CellBuilder.hxx>
#include <McCadMcRead_TrsfBuilder.hxx>
#include <McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface.hxx>
#include <McCadCSGGeom_Transformation.hxx>
#include <McCadMcRead_SurfaceFactory.hxx>
#include <McCadCSGGeom_Surface.hxx>
#include <McCadMcRead_Fill.hxx>
#include <McCadMcRead_HSequenceOfCell.hxx>
#include <McCadMcRead_HSequenceOfFill.hxx>
#include <McCadBoolExp_PostFixer.hxx>

#include <McCadCSGGeom_SurfaceType.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>

#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

#include <GeomAdaptor_Surface.hxx>

#include <TColgp_HSequenceOfPnt.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

#include <McCadMessenger_Singleton.hxx>


int NbyteRead;
Standard_Integer maxCNum, maxSNum, maxTNum;

McCadMcRead_Reader::McCadMcRead_Reader()
{
	myTitleSkipped = Standard_False;
	myUnsorted = Standard_True;
	myMacroBodySurfNumCnt = 1000000;
}

Standard_Boolean McCadMcRead_Reader::Parse(OSD_File& theStream)
{
        McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();
	//-----processing Infomation....
//::	cout<<"Parsing message card......"<<endl;

	//	std::ifstream
	//		in(sFileName.c_str());         //Open input file;
	if(!ParseMessage(theStream))
	{
		theStream.Close();
		return false;
	}
	//-----processing Infomation....

	if(!ParseCell(theStream))
	{
		theStream.Close();
		return false;
	}
	//-----processing Infomation....

	maxSNum = 0;
	if(!ParseSurface(theStream))
	{
		theStream.Close();
		return false;
	}
	//-----processing Infomation....

	if(!ParseDataCard(theStream))
	{
		theStream.Close();
		return false;
	}

	theStream.Close();

	//----- estimate Numbering ....
	//estimate max numberig!!
	Standard_Integer oldCNum=0;
	McCadMcRead_DataMapIteratorOfDataMapOfIntegerCell cIter;
	cIter.Initialize(myCellMap);

	maxTNum = 1000;
	for (; cIter.More();cIter.Next())
	{
		Handle(McCadMcRead_Cell) aCell = cIter.Value();
		maxCNum = aCell->GetCellNumber();

		Handle(TColStd_HSequenceOfAsciiString) curCSG = aCell->GetCSG();
		//cout << " CSG for Cell : " << aCell->GetCellNumber() << "\t _ " << aCell->GetMaterialNumber() << " _\t " << "\t -- \t";
		/*for(int r=1; r<=curCSG->Length(); r++)
			cout << curCSG->Value(r) << " " ;
		cout << endl;*/

		if (maxCNum > oldCNum)
			oldCNum = maxCNum;

		if(aCell->IsTRCL())
		{
			if(aCell->GetTRCLNumber()==1000) // now all transformations should be bound to myTrsfMap
			{
				Handle(McCadCSGGeom_Transformation) newTrafo = aCell->GetTrsf();
				newTrafo->SetIdNum(++maxTNum);
				myTrsfMap.Bind(maxTNum, newTrafo);
				aCell->SetTRCLNumber(maxTNum);

				/*Handle(McCadCSGGeom_Transformation) testTrsf = aCell->GetTrsf();
				gp_Trsf atrsf = testTrsf->GetTrsf();*/
			}
		}
	}
	maxCNum = oldCNum;

	//cout << "===============================================================\n";

	//-----processing Infomation....
/*	cout<<"Expanding  Repeated structures  ......"<<endl;
	cout << "Filler\n";*/
	ResolveFiller();
	cout << "Lattices\n" << endl;
	ResolveLattices();

	//-----processing Macro cells....
	cout<<"Resolving Macro cells  ......"<<endl;
	cIter.Initialize(myCellMap);

	for (; cIter.More();cIter.Next())
	{
		Handle(McCadMcRead_Cell) aCell = cIter.Value();

		//if(!aCell->IsPseudo())
	/*	{
			Handle(TColStd_HSequenceOfAsciiString) curCSG = aCell->GetCSG();
			cout << " CSG for Cell : " << aCell->GetCellNumber() << "\t _ " << aCell->GetMaterialNumber() << " _\t " << "\t -- \t";
			for(int r=1; r<=curCSG->Length(); r++)
				cout << curCSG->Value(r) << " " ;
			if(aCell->GetPseudoNum()!=0)
				cout << "  ... has Pseudo Number " << aCell->GetPseudoNum();

			cout << endl;
		}*/

		//append transformation to the cells:
		if(aCell->IsTRCL())
		{
		//	cout << "ASSIGNING Trsf " << aCell->GetTRCLNumber() << " to Cell " << aCell->GetCellNumber()<<endl;
			aCell->SetTrsf(myTrsfMap(aCell->GetTRCLNumber()));
		}

		if(aCell->HaveMacro())
		{
	//		cout << "Resolving macro cell for cell number:  ";
	//		cout << aCell->GetCellNumber() << endl;
	//		cout << "========================================" << endl;
			Handle(TColStd_HSequenceOfAsciiString) ownCSG = aCell->GetCSG();
			Handle(TColStd_HSequenceOfAsciiString) aCSG = new TColStd_HSequenceOfAsciiString;
			aCSG->Append(ownCSG);
			Handle(TColStd_HSequenceOfInteger) compCells = aCell->GetMacroIndex();
			for(int i=1; i<= compCells->Length(); i++)
			{
				Standard_Integer cellNum = compCells->Value(i);
				if(!myCellMap.IsBound(cellNum))
				{
                                    TCollection_AsciiString errormsg("McCadError: Missing macro cell for cell number:  ");
                                        errormsg += aCell->GetCellNumber();
                                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
					return false;
				}
				else
				{
					Handle(McCadMcRead_Cell) coCell = myCellMap(cellNum);
					Handle(TColStd_HSequenceOfAsciiString) coCSG = coCell->GetCSG();

					if(coCell->IsTRCL()) // transformed cell -> copy surfaces and apply transformation to surfcopy
					{
						Handle(TColStd_HSequenceOfAsciiString) tmpCSG = new TColStd_HSequenceOfAsciiString;
						if(coCSG->Length()<1)
							break;

						for(int l=1; l<=coCSG->Length(); l++)
						{
							if(coCSG->Value(l).IsIntegerValue())
							{
								//create new surface
								Standard_Integer surfNum = coCSG->Value(l).IntegerValue();
								Standard_Integer sign = 1;
								if(surfNum<0)
								{
									surfNum *= -1;
									sign = -1;
								}

								Handle(McCadCSGGeom_Surface) oldSurf = mySurfMap(surfNum);
								//Handle(McCadCSGGeom_Surface) newSurf = ;
								McCadMcRead_SurfaceFactory copySurf(oldSurf);
								Handle(McCadCSGGeom_Surface) newSurf = copySurf.GetSurface();
								newSurf->SetNumber(++maxSNum);
								gp_Trsf theTrsf = newSurf->GetTrsf();
								theTrsf = myTrsfMap(coCell->GetTRCLNumber())->GetTrsf() * theTrsf;
								newSurf->SetTrsf(theTrsf);
								newSurf->SetTrsfNumber(++maxTNum);

								Handle(McCadCSGGeom_Transformation) newTrsf = new McCadCSGGeom_Transformation;
								newTrsf->SetTrsf(theTrsf);
								newTrsf->SetIdNum(maxTNum);
								myTrsfMap.Bind(maxTNum, newTrsf);
								mySurfMap.Bind(maxSNum, newSurf);

								//newSurf->SetNumber(++maxSNum);
								tmpCSG->Append(TCollection_AsciiString(sign * maxSNum));

							}
							else
								tmpCSG->Append(coCSG->Value(l));
						}
						coCSG = tmpCSG;
					}

					aCSG->Append("#");
					aCSG->Append("(");
					aCSG->Append(coCSG);
					aCSG->Append(")");
				}
			}
			//  aCell->SetCSG(aCSG);

			TCollection_AsciiString tmpStr;
			for(int i=1; i<= aCSG->Length(); i++)
			{
				tmpStr.InsertAfter(tmpStr.Length(),aCSG->Value(i));
				tmpStr.InsertAfter(tmpStr.Length()," ");
			}
			/////////////////////////////////////////////////////////////////////////////////
			// update boolean forms
			McCadBoolExp_PostFixer thePostFixer;
			thePostFixer.Init(tmpStr);

			if(!thePostFixer.IsDone())
			{
                            TCollection_AsciiString errormsg("Error :: Generating postfix CSG expression failed for the following input:\n");
                                errormsg += tmpStr;
                                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
				return false;
			}
			Handle(TColStd_HSequenceOfAsciiString) theInPutCSG = thePostFixer.GetInput();
			aCell->SetCSG(theInPutCSG); // this is the raw mcnp csg!!
			Handle(TColStd_HSequenceOfAsciiString) theInFixCSG = thePostFixer.GetInFix();
			aCell->SetInFixCSG(theInFixCSG); // this is the raw csg in infix with intersection added!!
			Handle(TColStd_HSequenceOfAsciiString) thePostFixCSG = thePostFixer.GetPostFix();
			aCell->SetPostFixCSG(thePostFixCSG);
			aCell->SetHaveMacro(false);
			//////////////////////////////////////////////////////////////
		//	cout << endl;
		//	cout << "Macro cell resolved for cell number:  ";
		//	cout << aCell->GetCellNumber() << endl;
		//	cout << tmpStr.ToCString() << endl;
		//	cout << "========================================" << endl;
		}
	}

	//------ if a cell is TRCL we need to add transformed surfaces for that cell
/*	cIter.Initialize(myCellMap);

	for(; cIter.More(); cIter.Next())
	{
		Handle(McCadMcRead_Cell) curCell = cIter.Value();

		if(curCell->IsPseudo())
			continue;

		if(curCell->IsTRCL())
		{
			cout << "NEED TO APPEND TRANSFORMED SURFACES FOR CELL " << curCell->GetCellNumber() << endl;
		}
	}
*/

	////////////////////////////////////////////////
	//----- Transforming Surfaces
	McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface sIter;
	sIter.Initialize(mySurfMap);
	int trsfCount = 0;
	for (; sIter.More();sIter.Next())
	{
		Handle(McCadCSGGeom_Surface) aSurf = sIter.Value();
		Standard_Integer trsfNum = aSurf->GetTrsfNumber();
	//	Standard_Integer surfNum = aSurf->GetNumber();

		if( trsfNum == 0 ) // no trsf matrix
			continue;
		else
			trsfCount++;

		if(!myTrsfMap.IsBound(trsfNum))
		{
                        TCollection_AsciiString errormsg("McCadError: a missing transformation card for surface number: ");
                        errormsg += aSurf->GetNumber();
                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
			continue;
		}

		Handle(McCadCSGGeom_Transformation) aTrsf = myTrsfMap(trsfNum);
		gp_Trsf aCTrsf = aTrsf->GetTrsf();
		Handle(Geom_Surface) theSurface = aSurf->CasSurf();

		////////////////////////////////////////////////////////////////////////////////////////////
		switch (aSurf->GetType())
		{
			case McCadCSGGeom_Planar:
			{
				//cout << surfNum << "   <<S       T>>  " << trsfNum << ":McCadCSGGeom_Planar:  ";
				Handle(Geom_Plane) aCSurf = Handle(Geom_Plane)::DownCast(theSurface);
				gp_Ax3 aaAxis = aCSurf->Position();
		//		if (aaAxis.Direct());
		//		else cout << "Before: IndirectSurface found !!!" << endl;
				aCSurf->Transform(aCTrsf);
				aaAxis = aCSurf->Position();
		//		if (aaAxis.Direct());
		//		else cout << "After: IndirectSurface found !!!" << endl;
				GeomAdaptor_Surface theAdapSurface(aCSurf);
				aSurf->SetCasSurf(theAdapSurface);
				break;
			}
			case McCadCSGGeom_Conical:
			{
				//		cout << surfNum << "   <<S       T>>  " << trsfNum<< "McCadCSGGeom_Conical: ";
				Handle(Geom_ConicalSurface) aCSurf = Handle(Geom_ConicalSurface)::DownCast(theSurface);
				gp_Ax3 aaAxis = aCSurf->Position();
		//		if (aaAxis.Direct());
				//	else cout << "Before: IndirectSurface found !!!" << endl;
				aCSurf->Transform(aCTrsf);
				aaAxis = aCSurf->Position();
		//		if (aaAxis.Direct());
				//	else cout << "After: IndirectSurface found !!!" << endl;
				GeomAdaptor_Surface theAdapSurface(aCSurf);
				aSurf->SetCasSurf(theAdapSurface);
				break;
			}
			case McCadCSGGeom_Cylindrical:
			{
				//cout << surfNum << "   <<S       T>>  " << trsfNum<< "    McCadCSGGeom_Cylindrical: \n";
				Handle(Geom_CylindricalSurface) aCSurf = Handle(Geom_CylindricalSurface)::DownCast(theSurface);
				gp_Ax3 aaAxis = aCSurf->Position();
		//		if (aaAxis.Direct());
				//	else cout << "Before: IndirectSurface found !!!" << endl;
				aCSurf->Transform(aCTrsf);
				aaAxis = aCSurf->Position();
		//		if (aaAxis.Direct());
				//	else cout << "After: IndirectSurface found !!!" << endl;
				GeomAdaptor_Surface theAdapSurface(aCSurf);
				aSurf->SetCasSurf(theAdapSurface);
				break;
			}
			case McCadCSGGeom_Spherical:
			{
				//cout<< "McCadMcRead_Reader.cxx :: " << surfNum << "   <<S       T>>  " << trsfNum << "McCadCSGGeom_Spherical: ";
				Handle(Geom_SphericalSurface) aCSurf = Handle(Geom_SphericalSurface)::DownCast(theSurface);
				aCSurf->Transform(aCTrsf);
				GeomAdaptor_Surface theAdapSurface(aCSurf);
				aSurf->SetCasSurf(theAdapSurface);
				break;
			}
			case McCadCSGGeom_Toroidal:
			{
				//cout << surfNum << "   <<S       T>>  " << trsfNum<< "McCadCSGGeom_Toroidal: ";
				Handle(Geom_ToroidalSurface) aCSurf = Handle(Geom_ToroidalSurface)::DownCast(theSurface);
				aCSurf->Transform(aCTrsf);
				GeomAdaptor_Surface theAdapSurface(aCSurf);
				aSurf->SetCasSurf(theAdapSurface);
				break;
			}
			case McCadCSGGeom_Quadric:
			{
				//cout << "McCadError:  McCadCSGGeom_Qaudric surface type for surface number: ";
				//cout << aSurf->GetNumber() << endl;
				break;
			}
			case McCadCSGGeom_OtherSurface:
			{
				//cout << "McCadError: unimplemented surface type for surface number: ";
				//	cout << aSurf->GetNumber() << endl;
				break;
			}
			default :
                                msgr->Message("!!! unknown type of surface !!!\n", McCadMessenger_ErrorMsg);
		}
		//cout << endl;
	}


	//////////////////////////////////////////////
	//----- append surfacemaps to cells
	cIter.Initialize(myCellMap);
	for (; cIter.More();cIter.Next())
	{
		Handle(McCadMcRead_Cell) aCell = cIter.Value();
		Handle(TColStd_HSequenceOfAsciiString) aCSGSeq = aCell->GetCSG();
		Handle(TColStd_HSequenceOfAsciiString) mcrbdyCSGSubstSeq = new TColStd_HSequenceOfAsciiString;

		McCadCSGGeom_DataMapOfIntegerSurface surfMap;
		Standard_Boolean isMade = Standard_True;
		Standard_Boolean hasMB = Standard_False;

		for ( int it = 1; it <= aCSGSeq->Length(); it++ )
		{
			TCollection_AsciiString aVal = aCSGSeq->Value(it);

			Standard_Integer sNum(0);
			Standard_Integer signum(1);
			if(aVal.IsIntegerValue())
			{
				sNum = aVal.IntegerValue();

				if (sNum < 0 )
				{
					sNum = sNum*-1;
					signum = -1;
				}

				if(!mySurfMap.IsBound(sNum))
				{
                                        TCollection_AsciiString errormsg("McCadError: a missing surface card for the inputcard: ");
                                        errormsg += aVal;
                                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
					isMade = Standard_False;
					break;
				}
				else
				{
					if(mySurfMap(sNum)->IsMacroBody())
					{
						cout << "IS MACROBODY\n";
						hasMB = Standard_True;
						Handle(TColStd_HSequenceOfInteger) surfSeq = mySurfMap(sNum)->GetMacroBodySurfaces();

						for(Standard_Integer i = 1; i<=surfSeq->Length(); i++)
						{
							Standard_Integer j = surfSeq->Value(i);

							if(!mySurfMap.IsBound(j))
							{
                                                                msgr->Message("_#_McCadMcRead_Reader.cxx :: Macrobody surface not bound!!!",
                                                                              McCadMessenger_ErrorMsg);
								isMade = Standard_False;
								break;
							}

							surfMap.Bind(j, mySurfMap(j));
							mcrbdyCSGSubstSeq->Append(signum*j);
						}
					}
					else
					{
						surfMap.Bind(sNum,mySurfMap(sNum));
						mcrbdyCSGSubstSeq->Append(signum*sNum);
					}
				}
			}
		}

		if(isMade)
		{
			aCell->SetSurface(surfMap);

			if(hasMB)
			{

				TCollection_AsciiString tmpStr;
				cout << "Before fix :";
				for(int i=1; i<= mcrbdyCSGSubstSeq->Length(); i++)
				{
					cout << mcrbdyCSGSubstSeq->Value(i).ToCString() << " ";
					tmpStr.InsertAfter(tmpStr.Length(), mcrbdyCSGSubstSeq->Value(i));
					tmpStr.InsertAfter(tmpStr.Length()," ");
				}
				cout << endl;

				McCadBoolExp_PostFixer thePostFixer;

				thePostFixer.Init(tmpStr);

				if(!thePostFixer.IsDone())
				{
                                        TCollection_AsciiString errormsg("Error :: Generating postfix CSG expression failed for the following input: ");
                                        errormsg += tmpStr;
                                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
					continue;
				}

				Handle(TColStd_HSequenceOfAsciiString) theInPutCSG = thePostFixer.GetInput();
				aCell->SetCSG(theInPutCSG); // this is the raw mcnp csg!!
				Handle(TColStd_HSequenceOfAsciiString) theInFixCSG = thePostFixer.GetInFix();
				aCell->SetInFixCSG(theInFixCSG); // this is the raw csg in infix with intersection added!!
				Handle(TColStd_HSequenceOfAsciiString) thePostFixCSG = thePostFixer.GetPostFix();
				aCell->SetPostFixCSG(thePostFixCSG);
			}
			//	cout << "Number of surfaces of the cell =  " << surfMap.Extent() << endl;
		}
		else
		{
                        msgr->Message("McCadError: surface map construction failed due to missing surface card!\n",
                                      McCadMessenger_ErrorMsg);
		}

		Handle(TColStd_HSequenceOfAsciiString) pfCSG = aCell->GetPostFixCSG();
		/*cout << "POSTFIX: ";
		for(int kl=1; kl<=pfCSG->Length(); kl++)
		{
			cout << pfCSG->Value(kl).ToCString() << " ";
		}
		cout << endl;*/
	}



	return true;
}

Standard_Boolean McCadMcRead_Reader::ParseMessage(OSD_File& theStream)
{
	char theBuff[256];

	TCollection_AsciiString sBuff(theBuff);
	sBuff.Clear();

	theStream.ReadLine(sBuff, Standard_Integer(256), NbyteRead); //Read first line of the file;
	TCollection_AsciiString sline(sBuff);
	sline.UpperCase();
	int nIndex = sline.Search("MESSAGE:");

	if (nIndex<0 || nIndex>5 || IsComment(sline)) //NO message block
	{
		if(!IsComment(sline))//then it must be the title
			myTitleSkipped = Standard_True;

		return true; //could be the title
	}
	/********Skip message block********************/
	while (!theStream.IsAtEnd())
	{
		char theBuff[256];
		TCollection_AsciiString sBuff(theBuff);
		TCollection_AsciiString sline;
		theStream.ReadLine(sBuff, 256, NbyteRead);
		sline += sBuff;

		if (IsBlankDeliminter(sline)) // end of message block
			break;
	}
	//Now, in is at the begin of the next block;
	return true;
}

Standard_Boolean McCadMcRead_Reader::ParseTitle(OSD_File& theStream)
{
	// OSD_File in = theStream;

	if (theStream.IsAtEnd()) //"Title card" is necessary;
	{
		return false;
	}

	if(myTitleSkipped)//title has been found in ParseMessage
	{
		cout << "Title skipped\n";
		return true;
	}

	while (!theStream.IsAtEnd())
	{
		char theBuff[256];
		TCollection_AsciiString sBuff(theBuff);
		TCollection_AsciiString sline;

		//Read one line title card;
		theStream.ReadLine(sBuff, 256, NbyteRead);
		sline = sBuff;

	//	::cout<<sline.ToCString()<<endl;

		if(IsComment(sline))
		{
			continue;
		}
		if(IsBlankDeliminter(sline)) //"title card" is obligatory
		{
			return false;
		}
		//The next line should be "blank delimintor";
		if(theStream.IsAtEnd())
		{
			return false;
		}
		return true;
	}
	return false;
}

Standard_Boolean McCadMcRead_Reader::ParseCell(OSD_File& theStream)
{
	char theBuffer[256];
	TCollection_AsciiString sBuffer(theBuffer);
	sBuffer.Clear();
	TCollection_AsciiString sCellBuff; //Keep descripition of the cell being reading
	TCollection_AsciiString sline;
	// OSD_File in = theStream;
	cout << "Cell Building Started\n";
	cout << "======================\n";
	while (true)
	{
		if(theStream.IsAtEnd()) //Reach end of the cell card;
		{
			if(!sCellBuff.IsEmpty())
			{
				if(!CreatCellInst(sCellBuff))
					return false;
			}
			break; //Exit the loop
		}

		sBuffer.Clear();
		theStream.ReadLine(sBuffer,256,NbyteRead); //Read in a line;
		sline = sBuffer;

		sline.UpperCase();
		sline.ChangeAll('\t', ' '); //substitute all tabs by spaces

		if(IsBlankDeliminter(sline)) //Blank deliminter, the end of the cell card is found;
		{
			if(sCellBuff.Length() != 0)
			{
				if(!CreatCellInst(sCellBuff))
					return false;
			}
			break; //Exit the loop
		}

		if(IsComment(sline)) //Ignore the comment line;
			continue;

		//Last Char;
		sCellBuff.RightAdjust();

		if(sCellBuff.Search("&") == sCellBuff.Length())
		{
			if(sCellBuff.Length() == 0)//It is the first line, it should be the begin of the new cell card;
				return false;

			sCellBuff.RemoveAll('&',Standard_True);
			RemoveComment(sline); //Reomve subtring after '$', and '$' itself;
			sCellBuff.InsertAfter(sCellBuff.Length(),sline);
		}
		else if(IsContinuation(sline)) //Append to the last cell card;
		{
			if(sCellBuff.Length() == 0)//It is the first line, it should be the begin of the new cell card;
				return false;

			RemoveComment(sline); //Reomve subtring after '$', and '$' itself;
			sCellBuff.InsertAfter(sCellBuff.Length(),sline);
		}
		else //Begin of the new cell card;
		{
			//test if sline is title, i.e. doesn't begin with an integer value
			//if message is given, title seems not to be obligatory any longer
			sline.LeftAdjust();
			TCollection_AsciiString firstChar = sline;
			firstChar.Remove(2,firstChar.Length()-1);
			if(!firstChar.IsIntegerValue())
				continue;

			//finish reading the last cell card,
			if(sCellBuff.Length() != 0 )
			{
				if(!CreatCellInst(sCellBuff))
					return false;
			}

			RemoveComment(sline); //Reomve subtring after '$', and '$' itself;
			sCellBuff=sline; //Begin to read the new cell;
		}
	}

	cout << "Number of Cells Build : " << myCellMap.Extent() << endl;
	cout << "======================\n";
	cout << "End Of Cell Building\n\n";
	return true;
}

Standard_Boolean McCadMcRead_Reader::ParseSurface(OSD_File& theStream)
{
	char theBuffer[256];
	TCollection_AsciiString sBuffer(theBuffer);
	TCollection_AsciiString sSurfBuff; //Keeps the descripition of the cell thats being read
	TCollection_AsciiString sline;
	// OSD_File in = theStream;
	cout << "\nSurface Building Started\n";
	cout << "==========================\n";
	while (true)
	{
		if(theStream.IsAtEnd()) //Reach end of the cell card;
		{
			if(!sSurfBuff.IsEmpty())
			{
				if(!CreatSurfInst(sSurfBuff))
					return false;
			}
			break; //Exit the loop
		}
		sBuffer.Clear();
		theStream.ReadLine(sBuffer,256,NbyteRead); //Read in a line;
		sline=sBuffer;
		sline.UpperCase();
		sline.ChangeAll('\t', ' '); //tabs cause errors; substitute tabs with spaces

		if(IsBlankDeliminter(sline)) //Blank deliminter, the end of the surface card is found;
		{
			if(sSurfBuff.Length() != 0)
			{
				if(!CreatSurfInst(sSurfBuff))
				{
					return false;
				}
			}
			break; //Exit the loop
		}

		if(IsComment(sline)) //Ignore the comment line;
		{
			continue;
		}
		//Last Char;
		sSurfBuff.RightAdjust();
		sSurfBuff.LeftAdjust();

		if(sSurfBuff.Search("&") == sSurfBuff.Length())
		{
			if(sSurfBuff.Length() == 0)//It is the first line, it should be the begin of the new cell card;
			{
				return false;
			}
			sSurfBuff.RemoveAll('&',Standard_True);
			RemoveComment(sline); //Reomve subtring after '$', and '$' itself;
			sSurfBuff.InsertAfter(sSurfBuff.Length(),sline);
		}
		else if(IsContinuation(sline)) //Append to the last surf card;
		{
			if(sSurfBuff.Length() == 0)//It is the first line, it should be the begin of the new cell card;
			{
				return false;
			}
			RemoveComment(sline); //Reomve subtring after '$', and '$' itself;
			sSurfBuff.InsertAfter(sSurfBuff.Length(),sline);
		}
		else //Begin of the new cell card;
		{
			//finish reading the last cell card,
			if(sSurfBuff.Length() != 0)
			{
				if(!CreatSurfInst(sSurfBuff))
				{
					return false;
				}
			}

			RemoveComment(sline); //Reomve subtring after '$', and '$' itself;
			sSurfBuff=sline; //Begin to read the new cell;
		}
	}

	cout << "==========================\n";
	cout << "End Of Surface Building\n";

	return true;
}


Standard_Boolean McCadMcRead_Reader::ParseDataCard(OSD_File& theStream) //formerly ParseTRC
{
        McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();
	char theBuffer[256];
	TCollection_AsciiString sBuffer(theBuffer);
	TCollection_AsciiString sTRBuff; //Keep descripition of the cell being reading
	TCollection_AsciiString sline;
	Standard_Boolean isMCard = Standard_False;
	Standard_Boolean isTRC = Standard_False;

	// OSD_File in = theStream;
	cout << "\nParsing Data Card \n";
	cout << "=======================\n";
	while (true)
	{
		if (theStream.IsAtEnd()) //Reach end of the surf card;
		{
			if (sTRBuff.Length() != 0)
			{
				if(isTRC)
				{
					if (!CreatTRInst(sTRBuff))
					{
						cout<<"False 1"<<endl;
						return false;
					}
				}
				else if(isMCard)
				{
					if(!CreatMCardInst(sTRBuff))
					{
						cout <<"Material Card Failed\n";
						return false;
					}
				}
				else
                                        msgr->Message("McCadMcRead_Reader.cxx :: Error: neither MCard nor TransfromationCard",
                                                      McCadMessenger_WarningMsg);
			}
			break; //Exit the loop
		}
		sBuffer.Clear();
		theStream.ReadLine(sBuffer,256,NbyteRead); //Read in a line;
		sline = sBuffer;
		sline.UpperCase();

		if(IsBlankDeliminter(sline)) //Blank deliminter, the end of the cell card is found;
		{
			if(sTRBuff.Length() != 0)
			{
				if(isTRC)
				{
					if(!CreatTRInst(sTRBuff))
					{
						cout<<"False 2"<<endl;
						return false;
					}
				}
				else if(isMCard)
				{
					if(!CreatMCardInst(sTRBuff))
					{
                                                msgr->Message("Material Card Failed", McCadMessenger_ErrorMsg);
						return false;
					}
				}
			}
			break; //Exit the loop
		}

		if(IsComment(sline)) //Ignore the comment line;
			continue;

		//Last Char;
		sTRBuff.RightAdjust();

		if(sTRBuff.Search("&") == sTRBuff.Length())
		{
			if(sTRBuff.Length() == 0)//It is the first line, it should be the begin of the new cell card;
			{
				cout<<"False 3"<<endl;
				return false;
			}

			if(isTRC)
			{
				sTRBuff.RemoveAll('&',Standard_True);
				RemoveComment(sline); //Reomve subtring after '$', and '$' itself;
			}

			sTRBuff.InsertAfter(sTRBuff.Length(),sline);
		}
		else if(IsContinuation(sline)) //Append to the last surf card;
		{
			if(sTRBuff.Length() == 0)//It is the first line, it should be the begin of the new cell card;
				break;

			if(isTRC)
				RemoveComment(sline); //Reomve subtring after '$', and '$' itself;

			sline.RemoveAll('\n');

			if(isMCard)
				sline.Prepend("&");

			sTRBuff.InsertAfter(sTRBuff.Length(),sline);
			continue;
		}
		else //Begin of the new trsf card;
		{
			//finish reading the last TRSF card,
			if(sTRBuff.Length() != 0)
			{
				if(isTRC)
				{
					if(!CreatTRInst(sTRBuff))
					{
						cout<<"False 5"<<endl;
						return false;
					}
					sTRBuff.Clear();
				}
				else if(isMCard)
				{
					if(!CreatMCardInst(sTRBuff))
					{
                                                msgr->Message("MCard failed",McCadMessenger_ErrorMsg);
						return false;
					}
					sTRBuff="";
				}
			}

			int nIndex = sline.Search("TR"); // TRANSFORMATION CARD
			int mIndex = sline.Search("M"); //Possible MCard
			if(nIndex>=1 && nIndex<=5)
			{
				isTRC = Standard_True;
				isMCard = Standard_False;
				if(nIndex>1)
				{
					if(sline.Value(nIndex-1) == '*' || sline.Value(nIndex-1) == ' ')
					{
						RemoveComment(sline);
						sTRBuff = sline; //Begin to read the new cell;
					}
				}
				else
				{
					RemoveComment(sline); //Reomve subtring after '$', and '$' itself;
					sTRBuff = sline; //Begin to read the new Trsf;
				}
			}
			else if(mIndex>=1 && mIndex<=5)
			{
				TCollection_AsciiString mCardCheck = sline;
				mCardCheck.LeftAdjust();
				mCardCheck.Remove(1,1); //leading M is deleted
				mCardCheck.Split(1);
				if(mCardCheck.IsIntegerValue()) //MCard
				{
					//cout << "MCard : " << sline.ToCString();
					isTRC = Standard_False;
					isMCard = Standard_True;
					sTRBuff = sline;
				}
			}
		}
	}
	cout << "=======================\n";
	cout << "End Of Data Card\n\n";



	return true;
}

void McCadMcRead_Reader::RemoveComment(TCollection_AsciiString& sline)
{
	if (sline.Length() > 0 && sline.Search("$") > 0)
		TCollection_AsciiString sEnd = sline.Split(sline.Search("$")-1);

}

Standard_Boolean McCadMcRead_Reader::IsBlankDeliminter(
		const TCollection_AsciiString& sline)
{
	TCollection_AsciiString tmpStr = sline;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	if (tmpStr.Length() == 0) //A empty line;
		return true;
	else
		return false;
}

Standard_Boolean McCadMcRead_Reader::IsContinuation(
		const TCollection_AsciiString& sline)
{
	TCollection_AsciiString tmpStr = sline;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();

	if (tmpStr.Length() == 0) //An empty line;
		return false;
	tmpStr.Clear();
	tmpStr = sline;

	if (tmpStr.Search("     ")==1)
		return true;
	else
		return false;
}

Standard_Boolean McCadMcRead_Reader::IsComment(const TCollection_AsciiString& sline)
{
	TCollection_AsciiString tmpStr = sline;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	tmpStr.UpperCase();
	if (tmpStr.Length() == 0) //A empty line;
	{
		return false;
	}
	if (tmpStr.Value(1) == 'C')
		return true;
	else
		return false;
}

Standard_Boolean McCadMcRead_Reader::CreatCellInst(	const TCollection_AsciiString& sline)
{
        McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
	//cout<< endl;
	//cout<< " Cell Building started " << endl;
	cout << "Building : " << sline.ToCString() << endl;

	McCadMcRead_CellBuilder aBuilder;
	Handle(McCadMcRead_Cell) aCell;
	aBuilder.BuildCell(sline);

	if(aBuilder.IsDone())
	{
		aCell = aBuilder.GetCell();
		cout << " Cell Building finished for Cell number : " << aCell->GetCellNumber() << endl;
		cout << " cell is : ";
		if(aCell->IsFill())
			cout << "Fill ";
		if(aCell->IsLat())
			cout << "Lat ";
		if(aCell->IsU())
			cout << "U ";
		cout << endl;
	}
	else
	{
                TCollection_AsciiString errormsg("Cell Building failed for Card: ");
                errormsg += sline;
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

		return false;
	}
	myCellMap.Bind(aCell->GetCellNumber(),aCell);

	TCollection_AsciiString asciiName(aCell->GetCellNumber());
	myMDReader.SetMaterial(asciiName, aCell->GetMaterialNumber());
	myMDReader.SetDensity(asciiName, aCell->GetDensity());
	//cout<< " End of Cell Building " << endl;
	//cout<< "=========================================" << endl;
	//cout<< endl;

	return true;
}


Standard_Boolean McCadMcRead_Reader::CreatSurfInst(	const TCollection_AsciiString& sline)
{
        McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
	McCadMcRead_SurfaceFactory theFactory;

	Handle(McCadCSGGeom_Surface) aSurf;
	//cout<< endl;
	//cout<< "=========================================" << endl;
	//cout<< " Surface Building started " << endl;

	if(theFactory.MakeSurface(sline) && theFactory.IsDone())
	{
		aSurf = theFactory.GetSurface();
		if(aSurf->IsMacroBody())
		{
			cout << "MACROBODY!!!\n";
			Handle(McCadCSGGeom_HSequenceOfSurface) mcrBdySrfs = theFactory.GetMacroBodySurfaces();
			Handle(TColStd_HSequenceOfInteger) surfNumSeq = new TColStd_HSequenceOfInteger;

			for(Standard_Integer i=1; i<=mcrBdySrfs->Length(); i++)
			{
				Handle(McCadCSGGeom_Surface) curSrf = mcrBdySrfs->Value(i);
				curSrf->SetNumber(++myMacroBodySurfNumCnt);
				surfNumSeq->Append(myMacroBodySurfNumCnt);
				mySurfMap.Bind(myMacroBodySurfNumCnt, curSrf);
			}

			aSurf->SetMacroBodySurfaceNumbers(surfNumSeq);
		}
		//cout<<"Building Surface number :  "<< aSurf->GetNumber() << "  is finished." << endl;
	}
	else
	{
                TCollection_AsciiString errormsg("Surface Building failed for Card: ");
                errormsg += sline;
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

		return false;
	}

	Standard_Integer curNum = aSurf->GetNumber();

	if(maxSNum < curNum)
		maxSNum = curNum;

	mySurfMap.Bind(curNum,aSurf);
	//cout<< " End of Surface Building " << endl;
	//cout<< "=========================================" << endl;
	//cout<< endl;

	return true;
}


Standard_Boolean McCadMcRead_Reader::CreatTRInst(const TCollection_AsciiString& sline)
{
        McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
	//cout<< endl;
	//cout<< "=========================================" << endl;
	//cout<< " Building Transformation card started " << endl;

	//cout<< sline.ToCString() << endl;

	McCadMcRead_TrsfBuilder aBuilder;
	Handle(McCadCSGGeom_Transformation) aTrsf = new McCadCSGGeom_Transformation;
	aBuilder.BuildTrsf(sline);
	if(aBuilder.IsDone())
	{
		aTrsf = aBuilder.GetTrsf();
		//cout<<" Building Trsf number : "<< aTrsf->GetIdNum() << "  finished." << endl;
	}
	else
	{
                TCollection_AsciiString errormsg("Trsf Building failed for Card: ");
                errormsg += sline;
                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);

		return false;

	}
	myTrsfMap.Bind(aTrsf->GetIdNum(),aTrsf);

	/*cout << "Binding " << aTrsf->GetIdNum() << " to " << endl;
	for(int i=1; i<=3; i++)
	{
		cout << "\t\t";
		for(int j=1;j<=4; j++)
			cout << aTrsf->GetTrsf().Value(i,j) << " ";
		cout << endl;
	}*/

	//cout<< " End of Transformation card building " << endl;
	//cout<< "=========================================" << endl;
	//cout<< endl;

	// cout << myTrsfMap.Extent() << endl;


	return true;
}


Standard_Boolean McCadMcRead_Reader::CreatMCardInst(const TCollection_AsciiString& sline)
{
	TCollection_AsciiString tmpMC = sline;
	TCollection_AsciiString aNum = sline;
	Standard_Integer mNum(0);

	aNum.LeftAdjust();
	aNum.Remove(1,1);
	aNum.Split(aNum.Search(" ")-1);

	if(!aNum.IsIntegerValue())
		return Standard_False;

	mNum = aNum.IntegerValue();
	tmpMC.ChangeAll('&','\n');

	myMDReader.SetMCard(mNum, tmpMC);

	return Standard_True;
}

void McCadMcRead_Reader::ResolveLattices()
{
	//this call follows always the ResolveFiller();

	/*Standard_Integer maxTR(0);
	McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerTransformation trsfIter;
	for(trsfIter.Initialize(mySurfMap); trsfIter.More(); trsfIter.Next())
	{
		if(trsfIter.Value()->GetIdNum() > maxTR)
			maxTR = trsfIter.Value()->GetIdNum();
	}
	maxTR++;*/
        McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();

//	cout << "New Transformations begin with : " << maxTNum << endl;

	McCadMcRead_DataMapIteratorOfDataMapOfIntegerCell cIter;
	cIter.Initialize(myCellMap);

	Handle(McCadMcRead_HSequenceOfCell) cellSeq = new McCadMcRead_HSequenceOfCell;

	for (; cIter.More() ; cIter.Next())
	{
		/*cout << "Cell " << cIter.Value()->GetCellNumber() << " is ... ";
		if(cIter.Value()->IsU())
			cout << "U ";
		if(cIter.Value()->IsFill())
			cout << "Fill ";
		if(cIter.Value()->IsInfiniteLattice())
			cout << "Inf_Lattice";
			cout << endl;*/
		cellSeq->Append(cIter.Value());
	}

	for(Standard_Integer c1=1; c1<=cellSeq->Length(); c1++)
	{
		Handle(McCadMcRead_Cell) aCell = cellSeq->Value(c1);

		if (aCell->IsFill() && aCell->IsLat())
		{
			// make fill cell pseudo
			Standard_Integer latType = aCell->GetLatNum();
			if(latType != 2 && latType != 1)
			{
                                TCollection_AsciiString warningmsg("_#_McCadMcRead_Reader::ResolveLattices :: unknow lattice type : ");
                                warningmsg += latType;
                                msgr->Message(warningmsg.ToCString(), McCadMessenger_WarningMsg);

				return;
			}

		//	cout << "LAT TYPE : " << latType << endl;

			if(aCell->IsPseudo())
				continue;

			aCell->SetPseudo(true);

			// compute for each fill card  trsf using the placement vector and
			// mirroring. We have to read the surface and work with them.
			// The algoritm is as follows.
			// get +x,-x, +y,-y, +z,-z surfaces, this ordering is given on the cell card.
			// get the placement vector.
			// mirror along non zero component of the vector, say nx,  all surfaces n-times along (x).
			// example. pv=(-1,-1,0) ==> mirror all surfaces along -x once, then mirror all along -y once.
			// ****get the trsf by calculating trsf from (orgaxis,currentaxis); this is a translation.
			// the mirrored surfaces are set in the csg description.!! no trsf.
			// that is we create new surfaces, give them number and change the csg accordinly.
			Handle(McCadMcRead_Fill) aFill = aCell->GetFill();
			Handle(McCadMcRead_HSequenceOfFill) aFillSeq = aCell->GetFillSeq();
			Handle(TColgp_HSequenceOfPnt) placeSeq = aCell->GetFillPlaceSeq();
			//cout << "\t fill length: " << aFillSeq->Length() << "  place length: " << placeSeq->Length() << endl;

			if (aFillSeq->Length() != placeSeq->Length())
			{
                                TCollection_AsciiString errormsg("McCad_Error: Dimension mismatch on the fill card !  \nCell Number ");
                                errormsg += aCell->GetUNum();
                                errormsg += "  will not be constructed. ";
                                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
				continue;
			}
			Handle(TColStd_HSequenceOfAsciiString) cellCSG = new TColStd_HSequenceOfAsciiString;
			cellCSG->Append(aCell->GetCSG());
			// we extract the lattice surfaces: these can be upto 8 surfaces,
			// depending on lattice type and fill index dimension.
			// use a routine to find them
		//	cout << placeSeq->Length() << endl;
			if(placeSeq->Length() < 1)
			{
				if(aCell->IsInfiniteLattice())
					cout << "INIFINITE!!!\n";

				cout << aCell->GetCellNumber() << endl;

				// construct finite boundary for infinit lattice from the

				continue;
			}


			// estimate dimension using extrem vectors.
			gp_Pnt P1 = placeSeq->Value(1);
			gp_Pnt P2 = placeSeq->Value(placeSeq->Length());
			int xdim = int (P2.X() - P1.X());
			int ydim = int (P2.Y() - P1.Y());
			int zdim = int (P2.Z() - P1.Z());
			// minimum number of orienting surfaces: os =min(1,xdim-1)*2 + min(1,ydim-1)*2 + min(1,zdim-1)*2
			// total number of surfaces is determined by reading from the seq upto parenthesis.
			// get a the of surfaces
			//
			cellCSG->Reverse(); // now the lattice surface numbers are at the from till the parenthesis.
			Handle(TColStd_HSequenceOfInteger) surfNums = new TColStd_HSequenceOfInteger;
			for (int i = 1; i<= cellCSG->Length(); i++)
			{
				TCollection_AsciiString cStr = cellCSG->Value(i);

				if(!cStr.IsIntegerValue())
				{
					cellCSG->Remove(1, i-1);
					cellCSG->Reverse();
					break; // it breaks at "("
				}
				surfNums->Append(cStr.IntegerValue());
			}

			// make the mirroring directions.
			Standard_Integer dim=0;
			if (xdim > 0 && ydim > 0 && zdim > 0)
			{
				//the first two surface are x-mirroring directions.
				dim=3;
			}
			else if ((xdim > 0 && ydim > 0 && zdim == 0) ||
					(xdim > 0 && zdim > 0 && ydim == 0) ||
					(ydim > 0 && zdim > 0 && xdim == 0))
			{
				dim=2;
			}
			else
			{
                                TCollection_AsciiString errormsg("_#_McCadMcRead_Reader.cxx :: Dimension Error on the fill card !");
                                errormsg += "Cell Number ";
                                errormsg += aCell->GetUNum();
                                errormsg += "  will not be constructed. ";
                                msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
				continue;
			}

			for(int i=1; i<=placeSeq->Length(); i++)
			{
				//continue here the for loop is not yet closed!!!!
				//the problem is to modify the csg of the cell now......
				gp_Pnt place = placeSeq->Value(i);

		//		cout << "\t place = (" << place.X() << ", " << place.Y() << ", " << place.Z() << ")\n";
				MakeLatticeCellTransformation(dim, surfNums, place, latType);

				Handle(McCadMcRead_Fill) aFill = aFillSeq->Value(i);
				Standard_Integer uNum = aFill->GetFillNum();

				for (Standard_Integer c2=1; c2<=cellSeq->Length(); c2++)
				{
					Handle(McCadMcRead_Cell) uCell = cellSeq->Value(c2);
					if(uCell->IsU() && uCell->GetUNum() == uNum)
					{
						// make u cell pseudo ...............
						uCell->SetPseudo(true);

						Handle(McCadMcRead_Cell) newCell = new McCadMcRead_Cell;
						// we merge the fill and u card to a new cell and set both pseudo.
						maxCNum++; // free index in the map will serv as new cell number.
						newCell->SetCellNumber(maxCNum);
						newCell->SetMaterialNumber(uCell->GetMaterialNumber());
						newCell->SetDensity(uCell->GetDensity());
						// set MCNP csg
						Handle(TColStd_HSequenceOfAsciiString) aCSG = new TColStd_HSequenceOfAsciiString;
						// for the favor of  latter calculation we parenthesize the fill cell description.
						aCSG->Append(aCell->GetCSG());
						aCSG->Append(uCell->GetCSG());

						TCollection_AsciiString tmpStr;
						for(int i=1; i<= aCSG->Length(); i++)
						{
							tmpStr.InsertAfter(tmpStr.Length(), aCSG->Value(i));
							tmpStr.InsertAfter(tmpStr.Length()," ");
						}

						McCadBoolExp_PostFixer thePostFixer;

						thePostFixer.Init(tmpStr);

						if(!thePostFixer.IsDone())
						{
                                                        TCollection_AsciiString errormsg("Error :: Generating postfix CSG expression failed for the following input:");
                                                        errormsg += tmpStr;
                                                        msgr->Message(errormsg.ToCString(), McCadMessenger_ErrorMsg);
							continue;
						}

						Handle(TColStd_HSequenceOfAsciiString) theInPutCSG = thePostFixer.GetInput();
						newCell->SetCSG(theInPutCSG); // this is the raw mcnp csg!!
						Handle(TColStd_HSequenceOfAsciiString) theInFixCSG = thePostFixer.GetInFix();
						newCell->SetInFixCSG(theInFixCSG); // this is the raw csg in infix with intersection added!!
						Handle(TColStd_HSequenceOfAsciiString) thePostFixCSG = thePostFixer.GetPostFix();
						newCell->SetPostFixCSG(thePostFixCSG);

						// set fill card;
						if(uCell->IsFill())
						{
							newCell->SetHaveFill(true);
							newCell->SetFill(uCell->GetFill());
						}
						// copy macro cells
						if(uCell->HaveMacro())

						{
							newCell->SetHaveMacro(true);
							newCell->SetMacroIndex(uCell->GetMacroIndex());
						}
						//trsf card must be computed;
						if (aCell->IsTRCL())
						{
							int fillTNum = aCell->GetTRCLNumber();
							if (uCell->IsTRCL())
							{
								int uTNum = uCell->GetTRCLNumber();
								//both have trsf so we have to compute new
								Handle(McCadCSGGeom_Transformation) newTrsf = new McCadCSGGeom_Transformation;
								Handle(McCadCSGGeom_Transformation) fillMcCadTrsf = myTrsfMap(fillTNum);
								Handle(McCadCSGGeom_Transformation) uMcCadTrsf = myTrsfMap(uTNum);
								gp_Trsf fillTrsf = fillMcCadTrsf->GetTrsf();
								gp_Trsf uTrsf = uMcCadTrsf->GetTrsf();
								newTrsf->SetIdNum(++maxTNum);
								newTrsf->SetTrsf(fillTrsf*uTrsf);
								// bind this in the trsfmap
								myTrsfMap.Bind(newTrsf->GetIdNum(),newTrsf);
								newCell->SetTRCLNumber(maxTNum);
							}
							else
							{
								// only the fill has a trsf. set this number to new cell.
								newCell->SetTRCLNumber(fillTNum);
							}
						}
						else
						{
							newCell->SetTRCLNumber(maxTNum);
							newCell->SetHaveTRCL(maxTNum);
						}
						// bind the new cell into the map;
						myCellMap.Bind(newCell->GetCellNumber(),newCell);
					}
				}
			}
		}
	}

}

void McCadMcRead_Reader::ResolveFiller()
{
        McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
	//we resolve first the Like cards by copy.
	McCadMcRead_DataMapIteratorOfDataMapOfIntegerCell cIter;

	cIter.Initialize(myCellMap);

	//cout << "\nResolve Filler\n------------------------------------\n";
	for (; cIter.More() ; cIter.Next())
	{
		Handle(McCadMcRead_Cell) aCell = cIter.Value();

		if (aCell->IsLike())
		{
		//	cout << "Cell '" << aCell->GetCellNumber() << "' is like Cell '" << aCell->GetLikeNum() << "'\n";

			//switch like now
			aCell->SetHaveLike(false);
			if (!myCellMap.IsBound(aCell->GetLikeNum()))
			{
                                msgr->Message("McCadError: a companion like cell missing!",
                                              McCadMessenger_ErrorMsg);

				return;
			}
			Handle(McCadMcRead_Cell) aLikeCell = myCellMap(aCell->GetLikeNum());

			// we copy aLikeCell to aCell;
			if(aCell->GetMaterialNumber() < 0) // else MAT has been declared in cell
				aCell->SetMaterialNumber(aLikeCell->GetMaterialNumber());
			if(aCell->GetDensity() == 0.0)
				aCell->SetDensity(aLikeCell->GetDensity());

			// set MCNP csg
			aCell->SetCSG(aLikeCell->GetCSG());

			// set fill card;
			if(aLikeCell->IsFill() && !aLikeCell->IsLat())
			{
				// simple fill cell
				aCell->SetHaveFill(true);
				aCell->SetFill(aLikeCell->GetFill());
			}
			else if (aLikeCell->IsFill() && aLikeCell->IsLat())
			{
				// a lat-fill cell
				aCell->SetHaveFill(true);
				aCell->SetHaveLat(true);
				aCell->SetLat(aLikeCell->GetLatNum());
				// we put a multi fill card
				aCell->SetFillPlaceSeq(aLikeCell->GetFillPlaceSeq());
				aCell->SetFillSeq(aLikeCell->GetFillSeq());
			}
			else
			{
				// dor error check here!
			}

			if(aLikeCell->IsU())
			{
				aCell->SetHaveU(true);
				aCell->SetUNum(aLikeCell->GetUNum());
			}

			if(aLikeCell->HaveMacro())
			{
				aCell->SetHaveMacro(true);
				aCell->SetMacroIndex(aLikeCell->GetMacroIndex());
			}

			//trsf is retained;
			if(aCell->IsTRCL()) // like ...  but TRCL=(dadada);
			{
				gp_Trsf newTrsf = myTrsfMap(aCell->GetTRCLNumber())->GetTrsf();
				Handle(McCadCSGGeom_Transformation) mcTrsf = new McCadCSGGeom_Transformation;
				mcTrsf->SetTrsf(newTrsf);
				mcTrsf->SetIdNum(++maxTNum);
				myTrsfMap.Bind(mcTrsf->GetIdNum(),mcTrsf);
				aCell->SetTRCLNumber(mcTrsf->GetIdNum());
			}

			Handle(TColStd_HSequenceOfAsciiString) aCSG = new TColStd_HSequenceOfAsciiString;
			aCSG->Append(aCell->GetCSG());
			TCollection_AsciiString theCSG;

			for(int k=1; k<=aCSG->Length(); k++)
				theCSG += aCSG->Value(k) + TCollection_AsciiString(" ");

			McCadBoolExp_PostFixer csgPostFixer(theCSG);
			aCell->SetPostFixCSG(csgPostFixer.GetPostFix());

			//update the cell in the map
			myCellMap.UnBind(aCell->GetCellNumber());
			myCellMap.Bind(aCell->GetCellNumber(),aCell);
		}
	}

	//we resolve here only simple fill cards
	cIter.Initialize(myCellMap);
	Handle(TColStd_HSequenceOfInteger) cellSeq = new TColStd_HSequenceOfInteger;
	for (; cIter.More();cIter.Next())
		cellSeq->Append(cIter.Value()->GetCellNumber());

	for(int j=1; j<=cellSeq->Length(); j++)
	{
		Handle(McCadMcRead_Cell) fCell = myCellMap(cellSeq->Value(j)); //cIter.Value();

		if(fCell->IsFill() && !fCell->IsLat())
		{
			cout << "Cell '" << fCell->GetCellNumber() << "' is filled with universe : " << fCell->GetFill()->GetFillNum() << endl;
			// make fill cell pseudo
			fCell->SetPseudo(true);

			Handle(McCadMcRead_Fill) aFill = fCell->GetFill();
			Standard_Integer uNum = aFill->GetFillNum();

			McCadMcRead_DataMapIteratorOfDataMapOfIntegerCell cIter2;

			//cout << "FCell : " << fCell->GetCellNumber() << endl;
			//cout << " Universe : " << uNum <<  " CITER Extent : " << myCellMap.Extent() << endl;

			cIter2.Initialize(myCellMap);

			// we can't use the cIter2 directly in the for loop, because we change the
			// content of the map!
			Handle(McCadMcRead_HSequenceOfCell) curCells = new McCadMcRead_HSequenceOfCell;
			for(; cIter2.More(); cIter2.Next())
				curCells->Append(cIter2.Value());

			for(Standard_Integer k=1; k<=curCells->Length(); k++)
			{
				Handle(McCadMcRead_Cell) uCell = curCells->Value(k);
				if(uCell->IsU() && uCell->GetUNum() == uNum)
				{
					uCell->SetPseudo(Standard_True);
					//cout << "CREATE NEW CELL " << maxCNum+1 << " FOR UNIVERSE CELL " << uCell->GetCellNumber() << endl;
					Handle(McCadMcRead_Cell) newCell = new McCadMcRead_Cell;
					// we merge the fill and u card to a new cell and set both pseudo.
					maxCNum++; // free index in the map will serv as new cell number.
					newCell->SetCellNumber(maxCNum);
					newCell->SetMaterialNumber(uCell->GetMaterialNumber());
					newCell->SetDensity(uCell->GetDensity());
					newCell->SetPseudoNum(uCell->GetCellNumber()); // this will be the cells name
					// set MCNP csg
					Handle(TColStd_HSequenceOfAsciiString) aCSG = new TColStd_HSequenceOfAsciiString;
					// for the favor of  latter calculation we parenthesize the fill cell description.

					aCSG->Append(TCollection_AsciiString("("));
					aCSG->Append(fCell->GetCSG());
					aCSG->Append(TCollection_AsciiString(")"));
					aCSG->Append(uCell->GetCSG());

					newCell->SetCSG(aCSG);
					// set fill card;
					if(uCell->IsFill())
					{
						newCell->SetHaveFill(true);
						newCell->SetFill(uCell->GetFill());
						for(int l=1; l<=cellSeq->Length(); l++)
						{
							if(cellSeq->Value(l) == uCell->GetCellNumber())
							{
								cellSeq->Remove(l);
								break;
							}
						}
					}

					if(uCell->IsLat())
					{
						// a lat-fill cell
						newCell->SetHaveFill(true);
						newCell->SetLat(uCell->GetLatNum());
						newCell->SetHaveLat(Standard_True);
						newCell->SetFill(uCell->GetFill());
						newCell->SetFillPlaceSeq(uCell->GetFillPlaceSeq());
						newCell->SetFillSeq(uCell->GetFillSeq());

						if(uCell->IsInfiniteLattice())
							newCell->SetInfiniteLattice();
					}

					if(fCell->IsU())
					{
						newCell->SetPseudo(Standard_True);
					}

					// copy macro cells
					if(uCell->HaveMacro())
					{
						newCell->SetHaveMacro(true);
						newCell->SetMacroIndex(uCell->GetMacroIndex());
					}
					if(uCell->IsTRCL())
					{
						newCell->SetTRCLNumber(uCell->GetTRCLNumber());
						newCell->SetTrsf(uCell->GetTrsf());
						newCell->SetHaveTRCL(Standard_True);
					}

					//trsf card must be computed;
					if (fCell->IsTRCL())
					{
						int fillTNum = fCell->GetTRCLNumber();

						if (uCell->IsTRCL())
						{
							int uTNum = uCell->GetTRCLNumber();
							//both have trsf so we have to compute new
							Handle(McCadCSGGeom_Transformation) newTrsf = new McCadCSGGeom_Transformation;
							Handle(McCadCSGGeom_Transformation) fillMcCadTrsf = myTrsfMap(fillTNum);
							Handle(McCadCSGGeom_Transformation) uMcCadTrsf = myTrsfMap(uTNum);

							gp_Trsf fillTrsf = fillMcCadTrsf->GetTrsf();
							gp_Trsf uTrsf = uMcCadTrsf->GetTrsf();

							newTrsf->SetIdNum(++maxTNum);
							newTrsf->SetTrsf(fillTrsf*uTrsf);
							// bind this in the trsfmap

							myTrsfMap.Bind(newTrsf->GetIdNum(),newTrsf);
							newCell->SetTRCLNumber(newTrsf->GetIdNum());
							newCell->SetHaveTRCL(Standard_True);
						}
						else
						{
							// only the fill has a trsf. set this number to new cell.
							newCell->SetTRCLNumber(fillTNum);
							newCell->SetTrsf(fCell->GetTrsf());
							newCell->SetHaveTRCL(Standard_True);
						}
					}

					// set CSG for new cell
					TCollection_AsciiString theCSG;

					for(int k=1; k<=aCSG->Length(); k++)
						theCSG += aCSG->Value(k) + TCollection_AsciiString(" ");

					McCadBoolExp_PostFixer csgPostFixer(theCSG);
					newCell->SetPostFixCSG(csgPostFixer.GetPostFix());
					// bind the new cell into the map;
					myCellMap.Bind(newCell->GetCellNumber(),newCell);
					cellSeq->Append(maxCNum);
				}
			}
		}
	}

//	cout << "-----------------------------------------------\n#Resolve Filler\n\n";

}

void McCadMcRead_Reader::GetCells(McCadMcRead_DataMapOfIntegerCell& theMap)
{
	theMap = myCellMap;
}

void McCadMcRead_Reader::GetSurfaces(McCadCSGGeom_DataMapOfIntegerSurface& theMap)
{
	theMap = mySurfMap;
}

void McCadMcRead_Reader::GetTrsfs(McCadCSGGeom_DataMapOfIntegerTransformation& theMap)
{
	theMap = myTrsfMap;
}

void McCadMcRead_Reader::GetMDReader(McCadMDReader_Reader& theMDReader)
{
	theMDReader = myMDReader;
}


void McCadMcRead_Reader::MakeLatticeCellTransformation(const Standard_Integer& dim, const Handle_TColStd_HSequenceOfInteger& surfNum,
													   const gp_Pnt& thePlace, const Standard_Integer& latType)
{
        McCadMessenger_Singleton * msgr = McCadMessenger_Singleton::Instance();
	Handle(McCadCSGGeom_Transformation) newTrsf = new McCadCSGGeom_Transformation;

        Standard_Real deltX, deltY, deltZ;
	Handle(McCadCSGGeom_Surface) x0Surf, x1Surf, y0Surf, y1Surf, z0Surf, z1Surf, y0PSurf, y1PSurf;

	if(surfNum->Length() < 4)
	{
                msgr->Message("_#_McCadMcRead_Reader.cxx :: not enough surfaces for lattice construction!!!",
                              McCadMessenger_ErrorMsg);

		return;
	}

	// Get planes
	x0Surf = mySurfMap(Abs(surfNum->Value(1)));
	x1Surf = mySurfMap(Abs(surfNum->Value(2)));
	y0Surf = mySurfMap(Abs(surfNum->Value(3)));
	y1Surf = mySurfMap(Abs(surfNum->Value(4)));

	if(dim > 2 || latType == 2) // hexagonal or threedimensional lattice
	{
		if(dim > 2) // three dimensional
		{
			if(latType == 1) // hexahedral lattice
			{
				z0Surf = mySurfMap(Abs(surfNum->Value(5)));
				z1Surf = mySurfMap(Abs(surfNum->Value(6)));
			}
			else // hexagonal lattice
			{
				y0PSurf = mySurfMap(Abs(surfNum->Value(5)));
				y1PSurf = mySurfMap(Abs(surfNum->Value(6)));
				z0Surf = mySurfMap(Abs(surfNum->Value(7)));
				z1Surf = mySurfMap(Abs(surfNum->Value(8)));
			}
		}
		else // two dimensional hexagonal lattice
		{
			y0PSurf = mySurfMap(Abs(surfNum->Value(5)));
			y1PSurf = mySurfMap(Abs(surfNum->Value(6)));
		}
	}

	// get oriented location of planes
	gp_Ax3 x0Ax, x1Ax, y0Ax, y1Ax, z0Ax, z1Ax; //, y0PAx, y1PAx;
	x0Ax = x0Surf->GetAxis();
	x1Ax = x1Surf->GetAxis();
	y0Ax = y0Surf->GetAxis();
	y1Ax = y1Surf->GetAxis();

	if(dim>2) // three dimensional lattice
	{
		z0Ax = z0Surf->GetAxis();
		z1Ax = z1Surf->GetAxis();
	}
/*	if(latType == 2) // hexagonal lattice
	{
		y0PAx = y0PSurf->GetAxis();
		y1PAx = y1PSurf->GetAxis();
	}*/

	// compute step width along the lattice vectors
	deltX = Sqrt( (x1Ax.Location().X()-x0Ax.Location().X()) * (x1Ax.Location().X()-x0Ax.Location().X()) +
				  (x1Ax.Location().Y()-x0Ax.Location().Y()) * (x1Ax.Location().Y()-x0Ax.Location().Y()) +
				  (x1Ax.Location().Z()-x0Ax.Location().Z()) * (x1Ax.Location().Z()-x0Ax.Location().Z()) );
	deltY = Sqrt( (y1Ax.Location().X()-y0Ax.Location().X()) * (y1Ax.Location().X()-y0Ax.Location().X()) +
				  (y1Ax.Location().Y()-y0Ax.Location().Y()) * (y1Ax.Location().Y()-y0Ax.Location().Y()) +
				  (y1Ax.Location().Z()-y0Ax.Location().Z()) * (y1Ax.Location().Z()-y0Ax.Location().Z()) );
	deltZ = Sqrt( (z1Ax.Location().X()-z0Ax.Location().X()) * (z1Ax.Location().X()-z0Ax.Location().X()) +
				  (z1Ax.Location().Y()-z0Ax.Location().Y()) * (z1Ax.Location().Y()-z0Ax.Location().Y()) +
				  (z1Ax.Location().Z()-z0Ax.Location().Z()) * (z1Ax.Location().Z()-z0Ax.Location().Z()) );
	/*deltYP = Sqrt((y1PAx.Location().X()-y0PAx.Location().X()) * (y1PAx.Location().X()-y0PAx.Location().X()) +
				  (y1PAx.Location().Y()-y0PAx.Location().Y()) * (y1PAx.Location().Y()-y0PAx.Location().Y()) +
				  (y1PAx.Location().Z()-y0PAx.Location().Z()) * (y1PAx.Location().Z()-y0PAx.Location().Z()) );*/

	// compute new oriented locations
	Standard_Real x(0),y(0),z(0);
	gp_Pnt newPnt;
	gp_Trsf finalTrsf;

	//if(latType == 1)
	//{
		//set newX0
		x = x0Ax.Location().X() + deltX * thePlace.X() * x0Ax.Direction().X();
		y = x0Ax.Location().Y() + deltX * thePlace.X() * x0Ax.Direction().Y();
		z = x0Ax.Location().Z() + deltX * thePlace.X() * x0Ax.Direction().Z();

		newPnt = gp_Pnt(x,y,z);
		gp_Trsf xTrsf;
		xTrsf.SetTranslation(x0Ax.Location(), newPnt);

		//set newY0
		x = y0Ax.Location().X() + deltY * thePlace.Y() * y0Ax.Direction().X();
		y = y0Ax.Location().Y() + deltY * thePlace.Y() * y0Ax.Direction().Y();
		z = y0Ax.Location().Z() + deltY * thePlace.Y() * y0Ax.Direction().Z();

		newPnt = gp_Pnt(x,y,z);
		gp_Trsf yTrsf;
		yTrsf.SetTranslation(y0Ax.Location(), newPnt);

		finalTrsf = xTrsf*yTrsf;
	/*}
	else // calculate new position for hexagonal lattice
	{
		//set newX0
		x = x0Ax.Location().X() + deltX * thePlace.X() * x0Ax.Direction().X();
		y = x0Ax.Location().Y() + deltX * thePlace.X() * x0Ax.Direction().Y();
		z = x0Ax.Location().Z() + deltX * thePlace.X() * x0Ax.Direction().Z();

		newPnt = gp_Pnt(x,y,z);
		gp_Trsf xTrsf;
		xTrsf.SetTranslation(x0Ax.Location(), newPnt);

		//set newY0
		x = y0Ax.Location().X() + deltY * thePlace.Y() * y0Ax.Direction().X();
		y = y0Ax.Location().Y() + deltY * thePlace.Y() * y0Ax.Direction().Y();
		z = y0Ax.Location().Z() + deltY * thePlace.Y() * y0Ax.Direction().Z();

		newPnt = gp_Pnt(x,y,z);
		gp_Trsf yTrsf;
		yTrsf.SetTranslation(y0Ax.Location(), newPnt);

		finalTrsf = xTrsf*yTrsf;
	}*/

	if(dim > 2) // three dimensions
	{
		//set newZ0
		x = z0Ax.Location().X() + deltZ * thePlace.Z() * z0Ax.Direction().X();
		y = z0Ax.Location().Y() + deltZ * thePlace.Z() * z0Ax.Direction().Y();
		z = z0Ax.Location().Z() + deltZ * thePlace.Z() * z0Ax.Direction().Z();
		newPnt = gp_Pnt(x,y,z);
		gp_Trsf zTrsf;
		zTrsf.SetTranslation(z0Ax.Location(), newPnt);

		finalTrsf *= zTrsf;
	}

	newTrsf->SetTrsf(finalTrsf);
	newTrsf->SetIdNum(maxTNum++);

	myTrsfMap.Bind(maxTNum, newTrsf);

}
