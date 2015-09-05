#include <McCadCSGGeom_SurfaceOfRevolution.ixx>
#include <McCadGTOOL.hxx>
#include <ElSLib.hxx>
#include <iomanip>

#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <TopoDS_Shell.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Geom_Ellipse.hxx>

McCadCSGGeom_SurfaceOfRevolution::McCadCSGGeom_SurfaceOfRevolution()
{
	SetType(McCadCSGGeom_SurfOfRev);
}

McCadCSGGeom_SurfaceOfRevolution::McCadCSGGeom_SurfaceOfRevolution(gp_Ax1& revAxis, Handle(Geom_Curve)& basisCurve)
{
	SetType(McCadCSGGeom_SurfOfRev);
	myBasisCurve = basisCurve;
	myRevAxis = revAxis;
	gp_Ax2 the2Axis(revAxis.Location(), revAxis.Direction());
	//the2Axis.SetAxis(revAxis);
	gp_Ax3 the3Axis(the2Axis);
	SetAxis(the3Axis);
	myRealSeq = new TColStd_HSequenceOfReal();
}

McCadCSGGeom_SurfaceOfRevolution::McCadCSGGeom_SurfaceOfRevolution(gp_Ax1& revAxis, Handle(Geom_Curve)& basisCurve, Handle(TColStd_HSequenceOfReal)& realSeq)
{
	SetType(McCadCSGGeom_SurfOfRev);
	myBasisCurve = basisCurve;
	myRevAxis = revAxis;
	gp_Ax2 the2Axis(revAxis.Location(), revAxis.Direction());
	//the2Axis.SetAxis(revAxis);
	gp_Ax3 the3Axis(the2Axis);
	SetAxis(the3Axis);
	myRealSeq = realSeq;
}

/*void McCadCSGGeom_SurfaceOfRevolution::Coefficients(Standard_Real& A, Standard_Real& B,
		Standard_Real& C, Standard_Real& D) const
{
	gp_Pln aPln(myAxis);
	aPln.Coefficients(A, B, C, D);
}*/

void McCadCSGGeom_SurfaceOfRevolution::Print(Standard_OStream& theStream) const
{
	theStream.setf(ios::scientific);

	// Calculate Parameters R, A, B
	Standard_Real R, A, B;

	gp_Pnt p1 = myRevAxis.Location();
	Handle(Geom_Ellipse) e = Handle(Geom_Ellipse)::DownCast(myBasisCurve);
	gp_Pnt p2 = e->Location();
	R = p1.Distance(p2);
	A = e->MajorRadius();
	B = e->MinorRadius();

	gp_Ax3 theAxis;

	if (haveTrsf)
		theAxis = myTransformedAxis;
	else
		theAxis = myAxis;

        gp_Pnt theTorusLocation = theAxis.Location();

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
                                      << setw(20) << R << " "
                                      << setw(15) << A << " "
                                      << setw(15) << B << endl;
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
                                              << setw(8+myOutputPrecision) << R << " "
                                              << setw(8+myOutputPrecision) << A << " "
                                              << setw(8+myOutputPrecision) << B << endl;
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
                                      << setw(20) << R << " "
                                      << setw(15) << A << " "
                                      << setw(15) << B << endl;
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
                                              << setw(8+myOutputPrecision) << R << " "
                                              << setw(8+myOutputPrecision) << A << " "
                                              << setw(8+myOutputPrecision) << B << endl;
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
                                      << setw(20) << R << " "
                                      << setw(15) << A << " "
                                      << setw(15) << B << endl;
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
                                              << setw(8+myOutputPrecision) << R << " "
                                              << setw(8+myOutputPrecision) << A << " "
                                              << setw(8+myOutputPrecision) << B << endl;
                    theStream.unsetf(ios::right);
            }
	}
	else
	{
		if(haveTrsf)
			theStream << myTNumber << " -- " << endl;
		else
			theStream << "NO TRSFNBR" << endl;
	}
}


Standard_Real McCadCSGGeom_SurfaceOfRevolution::Evaluate(const gp_Pnt& thePoint) const
{
	GeomAPI_ProjectPointOnSurf projector(thePoint, mySurf);
	gp_Pnt nearestPnt = projector.NearestPoint();

	Standard_Real dist = nearestPnt.Distance(thePoint);
	if(dist < 1e-5)
		return 0.0;

	// Make solid and check if thePoint lies on the inside or the outside
	BRepBuilderAPI_MakeShell shellMaker(mySurf, Standard_False);
	TopoDS_Shell theShell = shellMaker.Shell();
	BRepBuilderAPI_MakeSolid solidMaker(theShell);
	TopoDS_Shape theSolid = solidMaker.Shape();
	Standard_Integer sgn(1);

	BRepClass3d_SolidClassifier clssfr(theSolid, thePoint, 1e-7);

	if(clssfr.State() == TopAbs_IN)
		sgn = -1;
	else if(clssfr.State() == TopAbs_ON)
		dist = 0.0;

	return dist*sgn;
}


gp_Pnt McCadCSGGeom_SurfaceOfRevolution::Value(const Standard_Real U, const Standard_Real V) const
{
	return gp_Pnt(0,0,0);
}


void McCadCSGGeom_SurfaceOfRevolution::SetCasSurf(const GeomAdaptor_Surface& theSurf)
{
	if (theSurf.GetType() != GeomAbs_SurfaceOfRevolution)
	{
		cout << "Surface Type incorrect !!" << endl;
		return;
	}

	mySurf = theSurf.Surface();
}


Handle(Geom_Surface) McCadCSGGeom_SurfaceOfRevolution::CasSurf() const
{
	Handle(Geom_SurfaceOfRevolution) retSurf = new Geom_SurfaceOfRevolution(myBasisCurve, myRevAxis);
	return retSurf;
}


Standard_Boolean McCadCSGGeom_SurfaceOfRevolution::IsEqual(const Handle(McCadCSGGeom_Surface)& another) const
{
	return Standard_False;
}


Standard_Boolean McCadCSGGeom_SurfaceOfRevolution::IsGeomEqual(const Handle(McCadCSGGeom_Surface)& another) const
{
	return Standard_False;
}

Handle(Geom_SurfaceOfRevolution) McCadCSGGeom_SurfaceOfRevolution::SurfaceOfRevolution() const
{
	Handle(Geom_SurfaceOfRevolution) retSurf = new Geom_SurfaceOfRevolution(myBasisCurve, myRevAxis);
	return retSurf;
}

void McCadCSGGeom_SurfaceOfRevolution::SetParameter(Handle(TColStd_HSequenceOfReal)& realSeq)
{
	myRealSeq = realSeq;
}

void McCadCSGGeom_SurfaceOfRevolution::GetParameter(Handle(TColStd_HSequenceOfReal)& realSeq) const
{
	realSeq = myRealSeq;
}
