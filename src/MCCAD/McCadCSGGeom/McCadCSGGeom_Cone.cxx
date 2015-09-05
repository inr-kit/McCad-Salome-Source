#include <McCadCSGGeom_Cone.ixx>
#include <McCadGTOOL.hxx>
#include <ElSLib.hxx>
#include <iomanip>


#include <Geom_Surface.hxx>
#include <Geom_ConicalSurface.hxx>



McCadCSGGeom_Cone::McCadCSGGeom_Cone()
{
  gp_Ax3 A3;
  SetType(McCadCSGGeom_Conical);
  SetAxis(A3);
  mySheet = 0;
  myApex = gp_Pnt(0,0,0);
}

McCadCSGGeom_Cone::McCadCSGGeom_Cone(const gp_Ax3& A3,const Standard_Real Ang,const Standard_Real theRadius)
{
  SetType(McCadCSGGeom_Conical);
  SetAxis(A3);
  mySemiAngle = Ang;
  myRadius =theRadius;
  mySheet = 0;
  myApex = gp_Pnt(0,0,0);
}

McCadCSGGeom_Cone::McCadCSGGeom_Cone(const gp_Ax3& A3,const Standard_Real Ang,const Standard_Real theRadius, const int sheet)
{
	  SetType(McCadCSGGeom_Conical);
	  SetAxis(A3);
	  mySemiAngle = Ang;
	  myRadius =theRadius;
	  mySheet = sheet;
          myApex = gp_Pnt(0,0,0);
}

int McCadCSGGeom_Cone::Sheet()
{
	if(abs(mySheet) > 1)
		mySheet = int (mySheet / abs(mySheet));

	return mySheet;
}

void McCadCSGGeom_Cone::SetRadius(const Standard_Real theRadius)
{
  myRadius = theRadius;
}

void McCadCSGGeom_Cone::SetSemiAngle(const Standard_Real Ang)
{
  mySemiAngle = Ang;
}

Standard_Real McCadCSGGeom_Cone::GetRadius() const
{

  return myRadius;

}

Standard_Real McCadCSGGeom_Cone::GetSemiAngle() const
{

  return mySemiAngle ;

}

void McCadCSGGeom_Cone::Coefficients(Standard_Real& A1,Standard_Real& A2,Standard_Real& A3,Standard_Real& B1,Standard_Real& B2,Standard_Real& B3,Standard_Real& C1,Standard_Real& C2,Standard_Real& C3,Standard_Real& D) const
{

  gp_Cone C(myAxis, mySemiAngle,myRadius);

  C.Coefficients(  A1,  A2,  A3,  B1,  B2,  B3,  C1,  C2,  C3,  D);

}

void McCadCSGGeom_Cone::Print(Standard_OStream& theStream) const
{

	theStream.setf(ios::scientific);
        theStream.precision(myOutputPrecision);

	gp_Pnt theApex;
	gp_Dir theDir;
	gp_Ax1 coneMainAx;
	Standard_Real A1, A2, A3, B1, B2, B3, C1, C2, C3, D;
	gp_Cone C;
	gp_Trsf tmpTrsf;
	gp_XYZ transl;
	// if the main axis is rotated the Trsf matrix is printed by client;
	// here we only print qaudric in major axis position with its TRcard Number.
        // warning it is up to the clien to print the Trsf matrix.
	gp_Ax3 theAxis;

	if (haveTrsf)
        {
//          cout << "HAVE TRANSFORMED CONE AXIS!!!\n\n\n";
//            cout << myAxis.Direction().X() << " " << myAxis.Direction().Z() << " -- " << myTransformedAxis.Direction().X() << " " << myTransformedAxis.Direction().Z() << endl;
            //cout << myAxis.Location().X() << " " << myAxis.Location().Z() << " -- " << myTransformedAxis.Location().X() << " " << myTransformedAxis.Location().Z() << endl;
            theAxis = myTransformedAxis;
        }
	else
            theAxis = myAxis;

	C.SetPosition(theAxis);
	C.SetRadius(myRadius);
	C.SetSemiAngle(mySemiAngle);

	C.Scale(gp_Pnt(0,0,0), Scale()); // this is to make mcnp happy with its cms.

	coneMainAx = C.Axis();
	theDir =  coneMainAx.Direction();

	theApex = C.Apex();

	Standard_Real apexX(theApex.X()), apexY(theApex.Y()), apexZ(theApex.Z());
	if(Abs(apexX) < 1.0e-07)
		apexX = 0.0;
	if(Abs(apexY) < 1.0e-07)
		apexY = 0.0;
	if(Abs(apexZ) < 1.0e-07)
		apexZ = 0.0;

	gp_Ax3 stdAx3Z(theAxis.Location(),gp::DZ());
	gp_Ax3 stdAx3X(theAxis.Location(),gp::DX());
	gp_Ax3 stdAx3Y(theAxis.Location(),gp::DY());

	if(GetMCType()==McCadCSGGeom_TRIPOLI) // we don't need transformation cards
	{
		theStream << setw(8) << "   SURF " << setw(5) << myNumber << " ";
		gp_Ax3 stdAxis;
                theAxis = GetAxis();
                theDir = theAxis.Direction();
		tmpTrsf.SetTransformation(theAxis, stdAxis);
		transl = tmpTrsf.TranslationPart();
                cout << "TRANSLATION : " << tmpTrsf.TranslationPart().X() << " " << tmpTrsf.TranslationPart().Y() << " " << tmpTrsf.TranslationPart().Z() << endl;
		gp_Pnt xyzPnt(transl);
		stdAx3Z.SetLocation(xyzPnt);
		stdAx3Y.SetLocation(xyzPnt);
		stdAx3X.SetLocation(xyzPnt);
	}

	if ( theAxis.IsCoplanar(stdAx3X, 0.001,0.001))
	{
		if(GetMCType() == McCadCSGGeom_TRIPOLI)
		{
			theStream << setw(7) << "CONEX";
			theStream.setf(ios::right);
			theStream << setw(15) << apexX << " "
					  << setw(15) << apexY << " "
					  << setw(15) << apexZ << "\n\t\t"
                      << setw(20) << C.SemiAngle()*(180./M_PI) << endl;
			theStream.unsetf(ios::right);
		}
		else
		{
			if (fabs(apexX) >= 1.e-07 && fabs(apexY) <= 1.e-07 && fabs(apexZ) <= 1.e-07)
			{
				theStream << setw(5) << myNumber << " " ;

				theStream.setf(ios::right);
					if (haveTrsf)
						theStream << setw(3) << myTNumber << " ";
					else
						theStream << setw(4) << " ";

					theStream  << setw(4) << "KX "
                                                           << setw(8+myOutputPrecision) << apexX << " "
                                                           << setw(8+myOutputPrecision) << pow(tan(C.SemiAngle()),2) << endl ;
				theStream.unsetf(ios::right);
			}
			else
			{
				theStream << setw(5) << myNumber << " " ;
				theStream.setf(ios::right);
					if (haveTrsf)
						theStream << setw(3) << myTNumber << " ";
					else
						theStream << setw(4) << " ";
					theStream   << setw(4)  << "K/X "
                                                                << setw(8+myOutputPrecision) << apexX << " "
                                                                << setw(8+myOutputPrecision) << apexY << " "
                                                                << setw(8+myOutputPrecision) << apexZ << " " << endl
								<< setw(14) << " "
                                                                << setw(8+myOutputPrecision)	<< pow(tan(C.SemiAngle()),2) << endl ;
				theStream.unsetf(ios::right);
			}
		}
	}
	else if ( theAxis.IsCoplanar(stdAx3Y, 0.001,0.001))
	{
		if(GetMCType() == McCadCSGGeom_TRIPOLI)
		{
			theStream << setw(7) << "CONEY";
			theStream.setf(ios::right);
			theStream << setw(15) << apexX << " "
					  << setw(15) << apexY << " "
					  << setw(15) << apexZ << "\n\t\t"
                      << setw(20) << C.SemiAngle()*(180./M_PI) << endl;
			theStream.unsetf(ios::right);
		}
		else
		{
			if (fabs(apexX) <= 1.e-07 && fabs(apexY) >= 1.e-07 && fabs(apexZ) <= 1.e-07)
			{
				theStream<< setw(5)  << myNumber << " " ;
				theStream.setf(ios::right);
					if (haveTrsf)
						theStream << setw(3) << myTNumber << " " ;
					else
						theStream << setw(4) << " ";

					theStream  << setw(4)  << "KY "
                                                           << setw(8+myOutputPrecision) << apexY  << " "
                                                           << setw(8+myOutputPrecision) << pow(tan(C.SemiAngle()),2) << endl ;
				theStream.unsetf(ios::right);
			}
			else
			{
				theStream << setw(5) << myNumber << " " ;
				theStream.setf(ios::right);
				if (haveTrsf)
					theStream << setw(3) << myTNumber << " ";
				else
					theStream << setw(4) << " ";

				theStream   << setw(4)  << "K/Y "
                                                    << setw(8+myOutputPrecision) << apexX << " "
                                                    << setw(8+myOutputPrecision) << apexY << " "
                                                    << setw(8+myOutputPrecision) << apexZ << " " << endl
						    << setw(14) << " "
                                                    << setw(8+myOutputPrecision) << pow(tan(C.SemiAngle()),2) << endl ;
				theStream.unsetf(ios::right);
			}
		}
	}
	else if  ( theAxis.IsCoplanar(stdAx3Z, 0.001,0.001))
	{
		if(GetMCType() == McCadCSGGeom_TRIPOLI)
		{
			theStream << setw(7) << "CONEZ";
			theStream.setf(ios::right);
			theStream << setw(15) << apexX << " "
					  << setw(15) << apexY << " "
					  << setw(15) << apexZ << "\n\t\t"
                      << setw(20) << C.SemiAngle()*(180./M_PI) << endl;
			theStream.unsetf(ios::right);
		}
		else
		{
			if (fabs(apexX) <= 1.e-07 && fabs(apexY) <= 1.e-07 && fabs(apexZ) >= 1.e-07)
			{
				theStream << setw(5) << myNumber << " " ;
				theStream.setf(ios::right);
					if (haveTrsf)
						theStream << setw(3) << myTNumber << " " ;
					else
						theStream << setw(4) << " ";
					theStream  << setw(4)  << "KZ "
                                                           << setw(8+myOutputPrecision) << apexZ << " "
                                                           << setw(8+myOutputPrecision) << pow(tan(C.SemiAngle()),2) << endl ;
				theStream.unsetf(ios::right);
			}
			else
			{
				theStream << setw(5) << myNumber << " " ;
				theStream.setf(ios::right);
					if (haveTrsf)
						theStream << setw(3) << myTNumber << " " ;
					else
						theStream << setw(4) << " ";

					theStream  << setw(4)  << "K/Z "
                                                           << setw(8+myOutputPrecision) << apexX << " "
                                                           << setw(8+myOutputPrecision) << apexY << " "
                                                           << setw(8+myOutputPrecision) << apexZ << " " << endl
							   << setw(14) << "  "
                                                           << setw(8+myOutputPrecision) << pow(tan(C.SemiAngle()),2) << endl;
				theStream.unsetf(ios::right);
			}
		}
	}
	else
	{
		if(GetMCType() == McCadCSGGeom_TRIPOLI)
		{
			theStream << setw(7) << "CONE";
			theStream.setf(ios::right);
                        theStream << setw(15) << myApex.X() << " "
                                  << setw(15) << myApex.Y() << " "
                                  << setw(15) << myApex.Z() << "\n\t\t"
                                          << setw(20) << C.SemiAngle()*(180./M_PI) << "\n\t\t"
					  << setw(20) << theDir.X() << " "
					  << setw(15) << theDir.Y() << " "
					  << setw(15) << theDir.Z() << endl;
			theStream.unsetf(ios::right);
		}
		else
		{
			theStream << "c        McCad:  Warning a conical surface may be written as a generic quadric!!" << endl;

			C.Coefficients (A1, A2, A3, B1, B2, B3, C1, C2, C3, D);

			theStream << setw(5) << myNumber << " " ;
			theStream.setf(ios::right);
				if (haveTrsf)
					theStream << setw(3) << myTNumber << " ";
				else
					theStream << setw(4) << " ";
				theStream  << setw(4)  << "GQ "
                                                   << setw(8+myOutputPrecision) << A1 << " "
                                                   << setw(8+myOutputPrecision) << A2 << " "
                                                   << setw(8+myOutputPrecision) << A3 << endl
						   << setw(14) << " "
                                                   << setw(8+myOutputPrecision) << 2*B1 << " "
                                                   << setw(8+myOutputPrecision) << 2*B2 << " "
                                                   << setw(8+myOutputPrecision) << 2*B3 << endl
						   << setw(14) << " "
                                                   << setw(8+myOutputPrecision) << 2*C1 << " "
                                                   << setw(8+myOutputPrecision) << 2*C2 << " "
                                                   << setw(8+myOutputPrecision) << 2*C3 << " " << endl
						   << setw(14) << " "
                                                   << setw(8+myOutputPrecision) <<  D  << endl ;
			theStream.unsetf(ios::right);
		}
	}
}


Standard_Real McCadCSGGeom_Cone::Evaluate(const gp_Pnt& thePoint) const
{
  gp_Cone C(myAxis,mySemiAngle,myRadius);
  return McCadGTOOL::Evaluate(C,thePoint);
}

gp_Pnt McCadCSGGeom_Cone::Value(const Standard_Real U,const Standard_Real V) const
{
   gp_Cone C(myAxis,mySemiAngle,myRadius);
   return ElSLib::Value(U,V,C);
}


void McCadCSGGeom_Cone::SetCasSurf(const GeomAdaptor_Surface& theSurf)
{
  if (theSurf.GetType() != GeomAbs_Cone) {cout << "Surface Type incorrect !!" << endl ; return;}
  gp_Cone C = theSurf.Cone();
  myAxis = C.Position();

  myApex = C.Apex();
  //cout << "SETCASSURFACE:POSITION :: " << myAxis.Location().X() << " < " << myAxis.Location().Z() << endl;
  //cout << "THE APEX : " << C.Apex().X() << " " << C.Apex().Y() << " " << C.Apex().Z() <<  endl;

  mySemiAngle = C.SemiAngle();
  myRadius =  C.RefRadius();
  SetAxis(myAxis);
}

Handle(Geom_Surface) McCadCSGGeom_Cone::CasSurf() const
{
   gp_Cone C(myAxis,mySemiAngle,myRadius);
   Handle(Geom_ConicalSurface) Gcone = new Geom_ConicalSurface(C);
   return Gcone;
}

Standard_Boolean McCadCSGGeom_Cone::IsEqual(const Handle(McCadCSGGeom_Surface)& another) const
{

  if(another->GetType() != myType)
	  return Standard_False;

  Handle(Geom_ConicalSurface) Gcone;

  try
  {
	  Gcone = Handle_Geom_ConicalSurface::DownCast(another->CasSurf());
  }
  catch(...)
  {
	  return Standard_False;
  }
  gp_Cone C = Gcone->Cone();

  Standard_Real aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD;
  Standard_Real bA1, bA2, bA3, bB1, bB2, bB3, bC1, bC2, bC3, bD;

  Coefficients (aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD);

// cout << "CONE1 : " << aA1 << " " << aA2 << " " << aA3<< " " << aB1<< " " << aB2<< " " << aB3<< " " << aC1<< " " << aC2<< " " << aC3<< " " << aD << endl;

  C.Coefficients (bA1, bA2, bA3, bB1, bB2, bB3, bC1, bC2, bC3, bD);

//  cout << "CONE2 : " << bA1<< " " << bA2<< " " << bA3<< " " << bB1<< " " << bB2<< " " << bB3<< " " << bC1<< " " << bC2<< " " << bC3<< " " << bD << endl;


  if ( fabs(aA1 - bA1 )<= 1.e-04 &&
       fabs(aA2 - bA2 )<= 1.e-04 &&
       fabs(aA3 - bA3 )<= 1.e-04 &&
       fabs(aB1 - bB1 )<= 1.e-04 &&
       fabs(aB2 - bB2 )<= 1.e-04 &&
       fabs(aB3 - bB3 )<= 1.e-04 &&
       fabs(aC1 - bC1 )<= 1.e-04 &&
       fabs(aC2 - bC2 )<= 1.e-04 &&
       fabs(aC3 - bC3 )<= 1.e-04 &&
       fabs(aD  -  bD )<= 1.e-04 )
    {
      return Standard_True;
    }
  else return Standard_False;
}

Standard_Boolean McCadCSGGeom_Cone::IsGeomEqual(const Handle(McCadCSGGeom_Surface)& another) const
{

	return IsEqual(another);

  if(another->GetType() != myType) return Standard_False;

  Handle(Geom_ConicalSurface) Gcone = Handle_Geom_ConicalSurface::DownCast(another->CasSurf());
  gp_Cone C = Gcone->Cone();

  Standard_Real aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD;
  Standard_Real bA1, bA2, bA3, bB1, bB2, bB3, bC1, bC2, bC3, bD;

  Coefficients (aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD);
  C.Coefficients (bA1, bA2, bA3, bB1, bB2, bB3, bC1, bC2, bC3, bD);

  if ( fabs(aA1 - bA1 )<= 1.e-04 &&
       fabs(aA2 - bA2 )<= 1.e-04 &&
       fabs(aA3 - bA3 )<= 1.e-04 &&
       fabs(aB1 - bB1 )<= 1.e-04 &&
       fabs(aB2 - bB2 )<= 1.e-04 &&
       fabs(aB3 - bB3 )<= 1.e-04 &&
       fabs(aC1 - bC1 )<= 1.e-04 &&
       fabs(aC2 - bC2 )<= 1.e-04 &&
       fabs(aC3 - bC3 )<= 1.e-04 &&
       fabs(aD  -  bD )<= 1.e-04 )
    {
      return Standard_True;
    }
  else return Standard_False;
}
