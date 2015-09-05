#include <McCadCSGGeom_Cylinder.ixx>
#include <McCadGTOOL.hxx>
#include <ElSLib.hxx>
#include <iomanip>

#include <Geom_Surface.hxx>
#include <Geom_CylindricalSurface.hxx>

McCadCSGGeom_Cylinder::McCadCSGGeom_Cylinder()
{
	gp_Ax3 A3;
	SetType(McCadCSGGeom_Cylindrical);
	SetAxis(A3);

}

McCadCSGGeom_Cylinder::McCadCSGGeom_Cylinder(const gp_Ax3& A3,
		const Standard_Real theRadius)
{
	SetType(McCadCSGGeom_Cylindrical);
	SetAxis(A3);
	myRadius =theRadius;

}

void McCadCSGGeom_Cylinder::SetRadius(const Standard_Real theRadius)
{

	myRadius = theRadius;

}

Standard_Real McCadCSGGeom_Cylinder::GetRadius() const
{

	return myRadius;

}

void McCadCSGGeom_Cylinder::Coefficients(Standard_Real& A1, Standard_Real& A2,
		Standard_Real& A3, Standard_Real& B1, Standard_Real& B2,
		Standard_Real& B3, Standard_Real& C1, Standard_Real& C2,
		Standard_Real& C3, Standard_Real& D) const
{
	gp_Cylinder C(myAxis, myRadius);

	C.Coefficients(A1, A2, A3, B1, B2, B3, C1, C2, C3, D);

}

void McCadCSGGeom_Cylinder::Print(Standard_OStream& theStream) const
{
	theStream.setf(ios::scientific);
        theStream.precision(myOutputPrecision);
	gp_Pnt theLocation;
	gp_Dir theDir;
	Standard_Real A1, A2, A3, B1, B2, B3, C1, C2, C3, D;
	gp_Cylinder aCylinder;
	gp_Trsf tmpTrsf;
	gp_XYZ transl;

	// if the main axis is rotated the Trsf matrix is printed by client;
	// here we only print quadric in major axis position with its TRcard Number.
	// warning it is up to the client to print the Trsf matrix.
	gp_Ax3 theAxis;

	if (haveTrsf)
		theAxis = myTransformedAxis;
	else
		theAxis = GetAxis();//myAxis;

	aCylinder.SetPosition(theAxis);
	aCylinder.SetRadius(myRadius);
	theDir = theAxis.Direction();
	theLocation = theAxis.Location();

	Standard_Real locX(theLocation.X()), locY(theLocation.Y()), locZ(theLocation.Z());
	if( Abs(locX) < 1.0e-07 )
		locX = 0.0;
	if( Abs(locY) < 1.0e-07 )
		locY = 0.0;
	if( Abs(locZ) < 1.0e-07 )
		locZ = 0.0;


	aCylinder.Scale(gp_Pnt(0,0,0), Scale()); // this is to make mcnp happy with its cms.
	Standard_Real aRadius = aCylinder.Radius();
	if(Abs(aRadius) < 1.0e-07)
		aRadius = 0.0;

	gp_Ax3 stdAx3Z(theAxis.Location(), gp::DZ());
	gp_Ax3 stdAx3X(theAxis.Location(), gp::DX());
	gp_Ax3 stdAx3Y(theAxis.Location(), gp::DY());

	if(GetMCType()==McCadCSGGeom_TRIPOLI) // we don't need transformation cards
	{
		theStream << setw(8) <<"   SURF " << setw(5) << myNumber << " ";
		gp_Ax3 stdAxis;
		theAxis = GetAxis();
		theDir = theAxis.Direction();
		tmpTrsf.SetTransformation(theAxis, stdAxis);
		transl = tmpTrsf.TranslationPart();
		gp_Pnt xyzPnt(transl);
		stdAx3Z.SetLocation(xyzPnt);
		stdAx3Y.SetLocation(xyzPnt);
		stdAx3X.SetLocation(xyzPnt);
	}

	if (theAxis.IsCoplanar(stdAx3X, 0.001, 0.001))
	{
		if(GetMCType()==McCadCSGGeom_TRIPOLI)
		{
			theStream << setw(7) << "CYLX";
			theStream.setf(ios::right);
                                theStream << setw(15) << transl.Y() << " "
                                                  << setw(15) << transl.Z() << "   "
                                                  << setw(15) << aRadius << endl;
			theStream.unsetf(ios::right);
		}
		else // DEFAULT: MCNP
		{
			if (fabs(locY) <= 1.e-07 && fabs(locZ) <= 1.e-07)
			{
				theStream<< setw(5) << myNumber << " ";

				theStream.setf(ios::right);
					if (haveTrsf)
						theStream << setw(3) << myTNumber << " ";
					else
						theStream << setw(4) << " ";
                                        theStream << setw(4) << "CX " << setw(8+myOutputPrecision)
                                                  << aRadius << endl;
				theStream.unsetf(ios::right);
			}
			else
			{
				theStream<< setw(5) << myNumber << " ";

				theStream.setf(ios::right);
					if (haveTrsf)
						theStream << setw(3) << myTNumber << " ";
					else
						theStream << setw(4) << " ";
					theStream << setw(4)  << "C/X "
                                                          << setw(8+myOutputPrecision) << locY << " "
                                                          << setw(8+myOutputPrecision) << locZ << " "
                                                          << setw(8+myOutputPrecision) << aRadius << endl;
				theStream.unsetf(ios::right);
			}
		}
	}
	else if (theAxis.IsCoplanar(stdAx3Y, 0.001, 0.001))
	{
		if(GetMCType()==McCadCSGGeom_TRIPOLI)
		{
			theStream << setw(7) << "CYLY";
			theStream.setf(ios::right);
				theStream << setw(15) << transl.X() << " "
						  << setw(15) << transl.Z() << "   "
						  << setw(15) << aRadius << endl;
			theStream.unsetf(ios::right);
			// theStream << setw(10) << "CYLY" << transl.X() << " " << transl.Z() << " " << aCylinder.Radius() << endl;
		}
		else // DEFAULT: MCNP
		{
			if (fabs(locX)<= 1.e-07 && fabs(locZ) <= 1.e-07)
			{
				theStream << setw(5) << myNumber << " ";

				theStream.setf(ios::right);
					if (haveTrsf)
						theStream << setw(3) << myTNumber << " ";
					else
						theStream << setw(4) << " ";
                                        theStream << setw(4) << "CY " << setw(8+myOutputPrecision) << aRadius << endl;
				theStream.unsetf(ios::right);
			}
			else
			{

				theStream<< setw(5) << myNumber << " ";

				theStream.setf(ios::right);
					if (haveTrsf)
						theStream << setw(3) << myTNumber << " ";
					else
						theStream << setw(4) << " ";
					theStream << setw(4)  << "C/Y "
                                                          << setw(8+myOutputPrecision) << locX << " "
                                                          << setw(8+myOutputPrecision) << locZ << " "
                                                          << setw(8+myOutputPrecision) << aRadius << endl;
				theStream.unsetf(ios::right);
			}
		}
	}
	else if (theAxis.IsCoplanar(stdAx3Z, 0.001, 0.001))
	{

		//cout << "_#_McCadCSGGeom_Cylinder:: theAxis = " << theAxis.Location().X() << "  " << theAxis.Location().Y() << "  " << theAxis.Location().Z() << endl;

		if(GetMCType()==McCadCSGGeom_TRIPOLI)
		{
			theStream << setw(7) << "CYLZ";
			theStream.setf(ios::right);
				theStream << setw(15) << transl.X() << " "
						  << setw(15) << transl.Y() << " "
						  << setw(15) << aRadius << endl;
			theStream.unsetf(ios::right);
			//theStream << setw(10) << "CYLZ" << transl.X() << " " << transl.Y() << " " << aCylinder.Radius() << endl;
		}
		else // DEFAULT: MCNP
		{
			if (fabs(locX) <= 1.e-07 && fabs(locY) <= 1.e-07 )
			{
				//cout << "haveTrsf = " << haveTrsf << endl;
				theStream<< setw(5) << myNumber << " ";

				theStream.setf(ios::right);
					if (haveTrsf)
						theStream << setw(3) << myTNumber << " ";
					else
						theStream << setw(4) << " ";
                                        theStream << setw(4) << "CZ " << setw(8+myOutputPrecision) << aRadius << endl;
				theStream.unsetf(ios::right);
			}
			else
			{
				theStream<< setw(5) << myNumber << " ";

				theStream.setf(ios::right);
					if (haveTrsf)
						theStream << setw(4) << myTNumber << " ";
					else
							theStream << setw(4) << " ";
					theStream << setw(4)  << "C/Z "
                                                          << setw(8+myOutputPrecision) << locX << " "
                                                          << setw(8+myOutputPrecision) << locY << " "
                                                          << setw(8+myOutputPrecision) << aRadius << endl;
				theStream.unsetf(ios::right);
			}
		}
	}
	else
	{
		if(GetMCType()==McCadCSGGeom_TRIPOLI)
		{
			theStream << setw(7) << "CYL";
			theStream.setf(ios::right);
			theStream << setw(15) << transl.X() << " "
					  << setw(15) << transl.Y() << " "
					  << setw(15) << transl.Z() << "\n\t\t"
					  << setw(20) << aRadius << "\n\t\t"
					  << setw(20) << theDir.X() << " "
					  << setw(15) << theDir.Y() << " "
					  << setw(15) << theDir.Z() << endl;
			theStream.unsetf(ios::right);
			//theStream << setw(10) << "CYL" << transl.X() << " " << transl.Y() << " " << transl.Z() << " " << aCylinder.Radius() << "\n\t\t"
			//		  << theDir.X() << " " << theDir.Y() << " " << theDir.Z() << endl;
		}
		else // DEFAULT: MCNP
		{
			theStream << "c      McCad: cylindrical surface written as quadric!!" 	<< endl;
			aCylinder.Coefficients(A1, A2, A3, B1, B2, B3, C1, C2, C3, D);
			theStream << setw(5)<< myNumber << " ";
			if (haveTrsf)
				theStream << setw(3) << myTNumber << " ";
			else
				theStream << setw(4) << " ";

			theStream << "GQ   "
                                          << setw(8+myOutputPrecision) << A1 << " "
                                          << setw(8+myOutputPrecision) << A2 << " "
                                          << setw(8+myOutputPrecision) << A3	<< endl
					  << setw(14) << " "
                                          << setw(8+myOutputPrecision) << 2*B1 << " "
                                          << setw(8+myOutputPrecision) << 2*B3 << " "
                                          << setw(8+myOutputPrecision) << 2*B2 << endl
			          << setw(14) << " "
                                  << setw(8+myOutputPrecision) << 2*C1 << " "
                                  << setw(8+myOutputPrecision) << 2*C2 << " "
                                  << setw(8+myOutputPrecision) << 2*C3 << endl
			          << setw(14) << " "
                                  << setw(8+myOutputPrecision) << D << endl;
		}
	}

}

Standard_Real McCadCSGGeom_Cylinder::Evaluate(const gp_Pnt& thePoint) const
{

	gp_Cylinder C(myAxis, myRadius);
	return McCadGTOOL::Evaluate(C, thePoint);

}

gp_Pnt McCadCSGGeom_Cylinder::Value(const Standard_Real U, const Standard_Real V) const
{
	gp_Cylinder C(myAxis, myRadius);
	return ElSLib::Value(U, V, C);
}

void McCadCSGGeom_Cylinder::SetCasSurf(const GeomAdaptor_Surface& theSurf)
{
	if (theSurf.GetType() != GeomAbs_Cylinder)
	{
		cout << "Surface Type incorrect !!" << endl;
		return;
	}
	gp_Cylinder C = theSurf.Cylinder();
	gp_Ax3 theAxis = C.Position();

	//cout << "McCadCSGGeom_Cylinder:: theAxis = " << theAxis.Location().X() << "  " << theAxis.Location().Y() << "  " << theAxis.Location().Z() << endl;

	myRadius = C.Radius();
	SetAxis(theAxis);
}

Handle(Geom_Surface) McCadCSGGeom_Cylinder::CasSurf() const
{
	gp_Cylinder C(myAxis,myRadius);
	Handle(Geom_CylindricalSurface) surf = new Geom_CylindricalSurface(C);
	return surf;
}

Standard_Boolean McCadCSGGeom_Cylinder::IsEqual(const Handle(McCadCSGGeom_Surface)& another) const
{

	if(another->GetType() != myType)
		return Standard_False;

	Handle(Geom_CylindricalSurface) Gcyl = Handle_Geom_CylindricalSurface::DownCast(another->CasSurf());
	gp_Cylinder other = Gcyl->Cylinder();

	Standard_Real aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD;
	Standard_Real bA1, bA2, bA3, bB1, bB2, bB3, bC1, bC2, bC3, bD;

	Coefficients(aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD);
	other.Coefficients (bA1, bA2, bA3, bB1, bB2, bB3, bC1, bC2, bC3, bD);
	  /*cout << "\n ****  " <<  aA1 << "   " <<   aA2 << " " <<  aA3 << " " <<
	 aB1 << " " <<  aB2 << " " <<  aB3 << " " <<
	 aC1 << " " << aC2  << " " << aC3  << " " << aD << endl;
	 cout << " ****  " <<  bA1 << "   " <<   bA2 << " " <<  bA3 << " " <<
	 bB1 << " " <<  bB2 << " " <<  bB3 << " " <<
	 bC1 << " " << bC2  << " " << bC3  << " " << bD << endl;*/

	if ( fabs(aA1 - bA1 )<= 1.e-04 &&
	fabs(aA2 - bA2 )<= 1.e-04 &&
	fabs(aA3 - bA3 )<= 1.e-04 &&
	fabs(aB1 - bB1 )<= 1.e-04 &&
	fabs(aB2 - bB2 )<= 1.e-04 &&
	fabs(aB3 - bB3 )<= 1.e-04 &&
	fabs(aC1 - bC1 )<= 1.e-04 &&
	fabs(aC2 - bC2 )<= 1.e-04 &&
	fabs(aC3 - bC3 )<= 1.e-04 &&
	fabs(aD - bD )<= 1.e-04 )
	{
		return Standard_True;
	}
	return Standard_False;
}

Standard_Boolean McCadCSGGeom_Cylinder::IsGeomEqual(const Handle(McCadCSGGeom_Surface)& another) const
{
	return IsEqual(another);

	if(another->GetType() != myType) return Standard_False;

	Handle(Geom_CylindricalSurface) Gcyl = Handle_Geom_CylindricalSurface::DownCast(another->CasSurf());
	gp_Cylinder other = Gcyl->Cylinder();

	Standard_Real aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD;
	Standard_Real bA1, bA2, bA3, bB1, bB2, bB3, bC1, bC2, bC3, bD;

	Coefficients(aA1, aA2, aA3, aB1, aB2, aB3, aC1, aC2, aC3, aD);
	other.Coefficients(bA1, bA2, bA3, bB1, bB2, bB3, bC1, bC2, bC3, bD);

	if ( fabs(aA1 - bA1 )<= 1.e-04 &&
	fabs(aA2 - bA2 )<= 1.e-04 &&
	fabs(aA3 - bA3 )<= 1.e-04 &&
	fabs(aB1 - bB1 )<= 1.e-04 &&
	fabs(aB2 - bB2 )<= 1.e-04 &&
	fabs(aB3 - bB3 )<= 1.e-04 &&
	fabs(aC1 - bC1 )<= 1.e-04 &&
	fabs(aC2 - bC2 )<= 1.e-04 &&
	fabs(aC3 - bC3 )<= 1.e-04 &&
	fabs(aD - bD )<= 1.e-04 )
	{
		return Standard_True;
	}
	return Standard_False;
}
