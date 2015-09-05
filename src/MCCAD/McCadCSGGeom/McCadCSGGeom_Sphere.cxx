#include <McCadCSGGeom_Sphere.ixx>
#include <McCadGTOOL.hxx>
#include <ElSLib.hxx>
#include <iomanip>

#include <Geom_Surface.hxx>
#include <Geom_SphericalSurface.hxx>

McCadCSGGeom_Sphere::McCadCSGGeom_Sphere()
{
	gp_Ax3 A3;
	SetType(McCadCSGGeom_Spherical);
	SetAxis(A3);

}

McCadCSGGeom_Sphere::McCadCSGGeom_Sphere(const Standard_Real theRadius)
{
	gp_Ax3 A3;
	SetType(McCadCSGGeom_Spherical);
	SetAxis(A3);
	myRadius = theRadius;
}

McCadCSGGeom_Sphere::McCadCSGGeom_Sphere(const gp_Ax3& A3,	const Standard_Real theRadius)
{
	SetType(McCadCSGGeom_Spherical);
	SetAxis(A3);
	myRadius =theRadius;
}

void McCadCSGGeom_Sphere::SetRadius(const Standard_Real theRadius)
{
	myRadius = theRadius;
}

Standard_Real McCadCSGGeom_Sphere::GetRadius() const
{
	return myRadius;
}

void McCadCSGGeom_Sphere::Coefficients(Standard_Real& A1, Standard_Real& A2,
		Standard_Real& A3, Standard_Real& B1, Standard_Real& B2,
		Standard_Real& B3, Standard_Real& C1, Standard_Real& C2,
		Standard_Real& C3, Standard_Real& D) const
{
	gp_Sphere S(myAxis, myRadius);

	S.Coefficients(A1, A2, A3, B1, B2, B3, C1, C2, C3, D);
}

void McCadCSGGeom_Sphere::Print(Standard_OStream& theStream) const
{
	theStream.setf(ios::scientific);
        theStream.precision(myOutputPrecision);
	gp_Pnt theLocation;
	// gp_Dir theDirect;
	// gp_Ax1 MainAx;
	//  Standard_Real A1, A2, A3, B1, B2, B3, C1, C2, C3, D;
	gp_Sphere aSphere;

	// if the main axis is rotated the Trsf matrix is printed by client;
	// here we only print qaudric in major axis position with its TRcard Number.
	// warning it is up to the clien to print the Trsf matrix.

	if (haveTrsf)
	{
		aSphere.SetPosition(myTransformedAxis);
		aSphere.SetRadius(myRadius);
	}
	else
	{
		aSphere.SetPosition(myAxis);
		aSphere.SetRadius(myRadius);
	}

	aSphere.Scale(gp_Pnt(0,0,0), Scale()); // this is to make mcnp happy with its cms.

	theLocation = aSphere.Location();

	if(GetMCType() == McCadCSGGeom_TRIPOLI)
	{
		theStream << setw(8) << "   SURF " << setw(5) << myNumber << " ";
		theStream << setw(7) << "SPHERE";
		theStream.setf(ios::right);
		theStream << setw(15) << theLocation.X() << " "
				  << setw(15) << theLocation.Y() << " "
				  << setw(15) << theLocation.Z() << "\n\t\t"
				  << setw(20) << aSphere.Radius() << endl;
		theStream.unsetf(ios::right);
	}
	else
	{
		if (theLocation.X() == 0 && theLocation.Y() == 0 && theLocation.Z() == 0)
		{
			theStream<< setw(5) << myNumber << " ";
			theStream.setf(ios::right);
				if (haveTrsf)
					theStream << setw(3) << myTNumber << " ";
				else
					theStream << setw(4) << " ";
                                theStream << setw(4) << "SO " << setw(8+myOutputPrecision) << aSphere.Radius() << endl;
			theStream.unsetf(ios::right);
		}
		else if (theLocation.X() != 0 && theLocation.Y() == 0 && theLocation.Z()
				== 0)
		{
			theStream<< setw(5) << myNumber << " ";
			theStream.setf(ios::right);
				if (haveTrsf)
					theStream << setw(3) << myTNumber << " ";
				else
					theStream << setw(4) << " ";
				theStream << setw(4)  << "SX "
                                                  << setw(8+myOutputPrecision) << theLocation.X() << " "
                                                  << setw(8+myOutputPrecision) << aSphere.Radius() << endl;
			theStream.unsetf(ios::right);
		}
		else if (theLocation.X() == 0 && theLocation.Y() != 0 && theLocation.Z()
				== 0)
		{
			theStream<< setw(5) << myNumber << " ";
			theStream.setf(ios::right);
				if (haveTrsf)
					theStream << setw(3) << myTNumber << " ";
				else
					theStream << setw(4) << " ";
				theStream << setw(4)  << "SY "
                                                  << setw(8+myOutputPrecision) << theLocation.Y() << " "
                                                  << setw(8+myOutputPrecision) << aSphere.Radius() << endl;
			theStream.unsetf(ios::right);
		}
		else if (theLocation.X() == 0 && theLocation.Y() == 0 && theLocation.Z()
				!= 0)
		{
			theStream<< setw(5) << myNumber << " ";
			theStream.setf(ios::right);
				if (haveTrsf)
					theStream << setw(3) << myTNumber << " ";
				else
					theStream << setw(4) << " ";
				theStream << setw(4)  << "SZ "
                                                  << setw(8+myOutputPrecision) << theLocation.Z() << " "
                                                  << setw(8+myOutputPrecision) << aSphere.Radius() << endl;
			theStream.unsetf(ios::right);
		}
		else
		{
			theStream << setw(5) << myNumber << " ";
			theStream.setf(ios::right);
				if (haveTrsf)
					theStream << setw(3) << myTNumber << " ";
				else
					theStream << setw(4) << " ";
				theStream << setw(4)  << "S "
                                                  << setw(8+myOutputPrecision) << theLocation.X() << " "
                                                  << setw(8+myOutputPrecision) << theLocation.Y() << " "
                                                  << setw(8+myOutputPrecision) << theLocation.Z() << " " << endl
						  << setw(14) << " "
                                                  << setw(8+myOutputPrecision) << aSphere.Radius() << endl;
			theStream.unsetf(ios::right);
		}
	}
}

Standard_Real McCadCSGGeom_Sphere::Evaluate(const gp_Pnt& thePoint) const
{
	gp_Sphere S(myAxis, myRadius);
	return McCadGTOOL::Evaluate(S, thePoint);
}

gp_Pnt McCadCSGGeom_Sphere::Value(const Standard_Real U, const Standard_Real V) const
{
	gp_Sphere S(myAxis, myRadius);
	return ElSLib::Value(U, V, S);
}

void McCadCSGGeom_Sphere::SetCasSurf(const GeomAdaptor_Surface& theSurf)
{
	if (theSurf.GetType() != GeomAbs_Sphere)
	{
		cout << "Surface Type incorrect !!" << endl;
		return;
	}
	gp_Sphere S = theSurf.Sphere();
	myAxis = S.Position();
	myRadius = S.Radius();
	SetAxis(myAxis);
}

Handle(Geom_Surface) McCadCSGGeom_Sphere::CasSurf() const
{
	gp_Sphere S(myAxis,myRadius);
	Handle(Geom_SphericalSurface) surf = new Geom_SphericalSurface(S);
	return surf;
}

Standard_Boolean McCadCSGGeom_Sphere::IsEqual(const Handle(McCadCSGGeom_Surface)& another) const
{

	if(another->GetType() != myType) return Standard_False;

	Handle(Geom_SphericalSurface) Gsph = Handle_Geom_SphericalSurface::DownCast(another->CasSurf());
	gp_Sphere C = Gsph->Sphere();
	gp_Ax3 ax2 = C.Position();
	gp_Dir dir2 = ax2.Direction();
	gp_XYZ loc2 = (ax2.Location()).XYZ();
	Standard_Real rad2 = C.Radius();
	Standard_Real Precision = 1.e-05;

	gp_Dir dir1 = myAxis.Direction();
	gp_XYZ loc1 = (myAxis.Location()).XYZ();

	if((dir1.IsEqual(dir2,1.e-03)) && (loc1.IsEqual(loc2,1.e-03))
	&& (fabs(myRadius -rad2 )<= Precision))
	return Standard_True;
	else return Standard_False;
}

Standard_Boolean McCadCSGGeom_Sphere::IsGeomEqual(const Handle(McCadCSGGeom_Surface)& another) const
{
	return IsEqual(another);

	if(another->GetType() != myType) return Standard_False;

	Handle(Geom_SphericalSurface) Gsph = Handle_Geom_SphericalSurface::DownCast(another->CasSurf());
	gp_Sphere C = Gsph->Sphere();
	gp_Ax3 ax2 = C.Position();
	gp_Dir dir2 = ax2.Direction();
	gp_XYZ loc2 = (ax2.Location()).XYZ();
	Standard_Real rad2 = C.Radius();
	Standard_Real Precision = 1.e-04;

	gp_Dir dir1 = myAxis.Direction();
	gp_XYZ loc1 = (myAxis.Location()).XYZ();

	if((dir1.IsParallel(dir2,1.e-03)) && (loc1.IsEqual(loc2,1.e-03))
	&& (fabs(myRadius -rad2 )<= Precision))
	return Standard_True;
	else return Standard_False;
}
