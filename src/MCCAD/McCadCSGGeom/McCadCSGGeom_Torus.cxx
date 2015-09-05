#include <McCadCSGGeom_Torus.ixx>
#include <McCadGTOOL.hxx>
#include <ElSLib.hxx>
#include <iomanip>

#include <Geom_Surface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>

McCadCSGGeom_Torus::McCadCSGGeom_Torus()
{
	gp_Ax3 A3;
	SetType(McCadCSGGeom_Toroidal);
	myIsElliptical = Standard_False;
	SetAxis(A3);

}

McCadCSGGeom_Torus::McCadCSGGeom_Torus(const gp_Ax3& A3, const Standard_Real theMajorRadius, const Standard_Real theMinorRadius)
{
	SetType(McCadCSGGeom_Toroidal);
	SetAxis(A3);
	myMajorRadius = theMajorRadius;
	myMinorRadius = theMinorRadius;
	myMinorEllipsRadius = theMinorRadius;
}

void McCadCSGGeom_Torus::SetMajorRadius(const Standard_Real theMajorRadius)
{
	myMajorRadius = theMajorRadius;
}

void McCadCSGGeom_Torus::SetMinorRadius(const Standard_Real theMinorRadius)
{
	myMinorRadius = theMinorRadius;
}

void McCadCSGGeom_Torus::SetMinorEllipticalRadius(const Standard_Real theMinorEllipsRadius)
{
	myMinorEllipsRadius = theMinorEllipsRadius;
}

Standard_Real McCadCSGGeom_Torus::GetMajorRadius() const
{
	return myMajorRadius;
}

Standard_Real McCadCSGGeom_Torus::GetMinorRadius() const
{
	return myMinorRadius;
}

Standard_Real McCadCSGGeom_Torus::GetMinorEllipticalRadius() const
{
	return myMinorEllipsRadius;
}

Standard_Boolean McCadCSGGeom_Torus::IsElliptical() const
{
	return myIsElliptical;
}

void McCadCSGGeom_Torus::SetElliptical(const Standard_Boolean theState)
{
	myIsElliptical = theState;
}

void McCadCSGGeom_Torus::Coefficients(TColStd_Array1OfReal& Coef) const
{
	gp_Torus T(myAxis, myMajorRadius, myMinorRadius);

	T.Coefficients(Coef);

}

void McCadCSGGeom_Torus::Print(Standard_OStream& theStream) const
{
	theStream.setf(ios::scientific);
        theStream.precision(myOutputPrecision);
	gp_Torus T;
	// if the main axis is rotated the Trsf matrix is printed by client;
	// here we only print qaudric in major axis position with its TRcard Number.
	// warning it is upto the client to print the Trsf matrix.
	gp_Ax3 theAxis;

	gp_Pnt theTorusLocation;
	gp_Dir theTorusDirect;
	gp_Ax1 torusMainAx;

	if (haveTrsf)
		theAxis = myTransformedAxis;
	else
		theAxis = myAxis;

	//cout << "Torus major radius : " << myMajorRadius << " , minor radius : " << myMinorRadius << endl;

	Standard_Real scaledMinorRadius(0.0), scaledMajorRadius(0.0);

	if(myMajorRadius < myMinorRadius) // OCC doesn't seem to like this case !!!
	{
		// make fake torus for main axis and scaling!!!
		gp_Torus dummyTorus;
		dummyTorus.SetPosition(theAxis);
		dummyTorus.SetMajorRadius(10.0);
		dummyTorus.SetMinorRadius(2.0);

		dummyTorus.Scale(gp_Pnt(0,0,0), Scale());
		scaledMinorRadius = myMinorRadius * Scale();
		scaledMajorRadius = myMajorRadius * Scale();

		theTorusLocation = dummyTorus.Location();
		torusMainAx = dummyTorus.Axis();
		theTorusDirect = torusMainAx.Direction();
	}
	else
	{
		T.SetPosition(theAxis);
		T.SetMajorRadius(myMajorRadius);
		T.SetMinorRadius(myMinorRadius);

		T.Scale(gp_Pnt(0,0,0), Scale()); // this is to make mcnp happy with its cms.

		// after Scaling reset radii
		scaledMajorRadius = T.MajorRadius();
		scaledMinorRadius = T.MinorRadius();

		theTorusLocation = T.Location();
		torusMainAx = T.Axis();
		theTorusDirect = torusMainAx.Direction();
	}

	gp_Ax3 stdAx3Z(theAxis.Location(), gp::DZ());
	gp_Ax3 stdAx3X(theAxis.Location(), gp::DX());
	gp_Ax3 stdAx3Y(theAxis.Location(), gp::DY());
	gp_XYZ transl;

	// prepare for tripoli
	if(GetMCType() == McCadCSGGeom_TRIPOLI)
	{
		theStream << setw(8) << "   SURF " << setw(5) << myNumber << " ";
		gp_Ax3 stdAxis;
		theAxis = GetAxis();
                //gp_Dir theDir = theAxis.Direction();
		gp_Trsf tmpTrsf;
		tmpTrsf.SetTransformation(theAxis, stdAxis);
		transl = tmpTrsf.TranslationPart();
		gp_Pnt xyzPnt(transl);
		stdAx3Z.SetLocation(xyzPnt);
		stdAx3Y.SetLocation(xyzPnt);
		stdAx3X.SetLocation(xyzPnt);
	}
	else if(GetMCType() == McCadCSGGeom_MCNP)
		theStream << setw(5) << myNumber << " ";

	if (theAxis.IsCoplanar(stdAx3X, 0.001, 0.001))
	{
		if(GetMCType() == McCadCSGGeom_TRIPOLI)
		{
			theStream << setw(7) << "TORUSX";
			theStream.setf(ios::right);
			theStream << setw(15) << transl.X() << " "
					  << setw(15) << transl.Y() << " "
					  << setw(15) << transl.Z() << "\n\t\t"
					  << setw(20) << scaledMajorRadius << " "
					  << setw(15) << scaledMinorRadius << " "
					  << setw(15) << scaledMinorRadius << endl;
			theStream.unsetf(ios::right);
		}
		else // MCNP
		{
			theStream.setf(ios::right);
				if (haveTrsf)
					theStream << setw(3) << myTNumber << " ";
				else
					theStream << setw(4) << " ";
				theStream << setw(4)  << "TX "
                                                  << setw(8+myOutputPrecision) << theTorusLocation.X() << " "
                                                  << setw(8+myOutputPrecision) << theTorusLocation.Y() << " "
                                                  << setw(8+myOutputPrecision) << theTorusLocation.Z() << endl
						  << setw(14) << " "
                                                  << setw(8+myOutputPrecision) << scaledMajorRadius << " "
                                                  << setw(8+myOutputPrecision) << scaledMinorRadius << " "
                                                  << setw(8+myOutputPrecision) << scaledMinorRadius << endl;
			theStream.unsetf(ios::right);
		}
	}
	else if (theAxis.IsCoplanar(stdAx3Y, 0.001, 0.001))
	{
		if(GetMCType() == McCadCSGGeom_TRIPOLI)
		{
			theStream << setw(7) << "TORUSY";
			theStream.setf(ios::right);
			theStream << setw(15) << transl.X() << " "
					  << setw(15) << transl.Y() << " "
					  << setw(15) << transl.Z() << "\n\t\t"
					  << setw(20) << scaledMajorRadius << " "
					  << setw(15) << scaledMinorRadius << " "
					  << setw(15) << scaledMinorRadius << endl;
			theStream.unsetf(ios::right);
			//theStream << setw(10) << "TORUSY" << transl.X() << " " << transl.Y() << " " << transl.Z() << " "
			//					  << myMajorRadius << " " << myMinorRadius << " " << myMinorRadius << endl;
		}
		else //MCNP
		{
			theStream.setf(ios::right);
				if (haveTrsf)
					theStream << setw(3) << myTNumber << " ";
				else
					theStream << setw(4) << " ";

				theStream << setw(4)  << "TY "
                                                  << setw(8+myOutputPrecision) << theTorusLocation.X() << " "
                                                  << setw(8+myOutputPrecision) << theTorusLocation.Y() << " "
                                                  << setw(8+myOutputPrecision) << theTorusLocation.Z() << endl
						  << setw(14) << " "
                                                  << setw(8+myOutputPrecision) << scaledMajorRadius << " "
                                                  << setw(8+myOutputPrecision) << scaledMinorRadius << " "
                                                  << setw(8+myOutputPrecision) << scaledMinorRadius << endl;
			theStream.unsetf(ios::right);
		}
	}
	else if (theAxis.IsCoplanar(stdAx3Z, 0.001, 0.001))
	{
		if(GetMCType() == McCadCSGGeom_TRIPOLI)
		{
			theStream << setw(7) << "TORUSZ";
			theStream.setf(ios::right);
			theStream << setw(15) << transl.X() << " "
					  << setw(15) << transl.Y() << " "
					  << setw(15) << transl.Z() << "\n\t\t"
					  << setw(20) << scaledMajorRadius << " "
					  << setw(15) << scaledMinorRadius << " "
					  << setw(15) << scaledMinorRadius << endl;
			theStream.unsetf(ios::right);
			//theStream << setw(10) << "TORUSZ" << transl.X() << " " << transl.Y() << " " << transl.Z() << " "
			//					  << myMajorRadius << " " << myMinorRadius << " " << myMinorRadius << endl;
		}
		else // MCNP
		{
			theStream.setf(ios::right);
				if (haveTrsf)
					theStream << setw(3) << myTNumber << " ";
				else
					theStream << setw(4) << " ";
				theStream << setw(4)  << "TZ "
                                                  << setw(8+myOutputPrecision) << theTorusLocation.X() << " "
                                                  << setw(8+myOutputPrecision) << theTorusLocation.Y() << " "
                                                  << setw(8+myOutputPrecision) << theTorusLocation.Z() << endl
						  << setw(14) << " "
                                                  << setw(8+myOutputPrecision) << scaledMajorRadius << " "
                                                  << setw(8+myOutputPrecision) << scaledMinorRadius << " "
                                                  << setw(8+myOutputPrecision) << scaledMinorRadius << endl;
			theStream.unsetf(ios::right);
		}
	}
	else // MCNP
	{
		theStream << "c      McCad Error: A generic Torus transformation."
				<< endl;
		theStream
				<< "c      McCad: The following are the coefficients of the torus:"
				<< endl;

		// The size of the array is upper - lower + 1.

		TColStd_Array1OfReal Coef(1, 31);
		T.Coefficients(Coef);
		if (haveTrsf)
			theStream << myTNumber << "  ";
		theStream << "  " << "GT   " << setw(10);
		for (int it=1; it <= 31; it++)
		{
			theStream << setw(10) << Coef(it) << " ";
		}
		theStream << endl;
	}
}

Standard_Real McCadCSGGeom_Torus::Evaluate(const gp_Pnt& thePoint) const
{
	gp_Torus T(myAxis, myMajorRadius, myMinorRadius);
	return McCadGTOOL::Evaluate(T, thePoint);
}

gp_Pnt McCadCSGGeom_Torus::Value(const Standard_Real U, const Standard_Real V) const
{
	gp_Torus T(myAxis, myMajorRadius, myMinorRadius);
	return ElSLib::Value(U, V, T);
}

void McCadCSGGeom_Torus::SetCasSurf(const GeomAdaptor_Surface& theSurf)
{
	if (theSurf.GetType() != GeomAbs_Torus)
	{
		cout << "Surface Type incorrect !!" << endl;
		return;
	}
	gp_Torus T = theSurf.Torus();
	gp_Ax3 theAxis = T.Position();
// cout << "McCadCSGGeom_Torus :: theAxis = " << theAxis.Location().X() << "  " << theAxis.Location().Y() << "  " << theAxis.Location().Z() << endl;
	myMajorRadius = T.MajorRadius();
	myMinorRadius = T.MinorRadius();
	SetAxis(theAxis);
}

Handle(Geom_Surface) McCadCSGGeom_Torus::CasSurf() const
{
	gp_Torus T(myAxis,myMajorRadius,myMinorRadius);
	Handle(Geom_ToroidalSurface) surf = new Geom_ToroidalSurface(T);
	return surf;
}

Standard_Boolean McCadCSGGeom_Torus::IsEqual(const Handle(McCadCSGGeom_Surface)& another) const
{

	if(another->GetType() != myType) return Standard_False;

	Handle(Geom_ToroidalSurface) Gtor = Handle_Geom_ToroidalSurface::DownCast(another->CasSurf());
	gp_Torus C = Gtor->Torus();
	gp_Ax3 ax2 = C.Position();
	gp_Dir dir2 = ax2.Direction();
	gp_XYZ loc2 = (ax2.Location()).XYZ();
	Standard_Real arad2 = C.MajorRadius();
	Standard_Real irad2 = C.MinorRadius();
	Standard_Real Precision = 1.e-05;

	gp_Dir dir1 = myAxis.Direction();
	gp_XYZ loc1 = (myAxis.Location()).XYZ();

	if((dir1.IsEqual(dir2,1.e-03)) && (loc1.IsEqual(loc2,1.e-03)) &&
	(fabs(myMajorRadius - arad2 )<= Precision) && (fabs(myMinorRadius -irad2 )<= Precision) )
	return Standard_True;
	else return Standard_False;
}

Standard_Boolean McCadCSGGeom_Torus::IsGeomEqual(const Handle(McCadCSGGeom_Surface)& another) const
{
	return IsEqual(another);

	if(another->GetType() != myType) return Standard_False;

	Handle(Geom_ToroidalSurface) Gtor = Handle_Geom_ToroidalSurface::DownCast(another->CasSurf());
	gp_Torus C = Gtor->Torus();
	gp_Ax3 ax2 = C.Position();
	gp_Dir dir2 = ax2.Direction();
	gp_XYZ loc2 = (ax2.Location()).XYZ();
	Standard_Real arad2 = C.MajorRadius();
	Standard_Real irad2 = C.MinorRadius();
	Standard_Real Precision = 1.e-04;

	gp_Dir dir1 = myAxis.Direction();
	gp_XYZ loc1 = (myAxis.Location()).XYZ();

	if((dir1.IsParallel(dir2,1.e-03)) && (loc1.IsEqual(loc2,1.e-03)) &&
	(fabs(myMajorRadius - arad2 )<= Precision) && (fabs(myMinorRadius -irad2 )<= Precision) )
	return Standard_True;
	else return Standard_False;
}
