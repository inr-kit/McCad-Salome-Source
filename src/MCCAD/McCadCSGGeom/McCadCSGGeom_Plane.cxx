#include <McCadCSGGeom_Plane.ixx>
#include <McCadGTOOL.hxx>
#include <ElSLib.hxx>
#include <iomanip>

#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
McCadCSGGeom_Plane::McCadCSGGeom_Plane()
{
	gp_Ax3 axis;
	SetType(McCadCSGGeom_Planar);
	SetAxis(axis);
}

McCadCSGGeom_Plane::McCadCSGGeom_Plane(const Standard_Real A,
		const Standard_Real B, const Standard_Real C, const Standard_Real D)
{
	gp_Pln aPln(A, B, C, D);
	SetType(McCadCSGGeom_Planar);
	SetAxis(aPln.Position());
}

void McCadCSGGeom_Plane::Coefficients(Standard_Real& A, Standard_Real& B,
		Standard_Real& C, Standard_Real& D) const
{
	gp_Pln aPln(myAxis);
	aPln.Coefficients(A, B, C, D);
}

void McCadCSGGeom_Plane::Print(Standard_OStream& theStream) const
{
	theStream.setf(ios::scientific);
        theStream.precision(myOutputPrecision);
	  Standard_Real A, B, C, D;
	  gp_Pln aPln;
	  // if the main axis is rotated the Trsf matrix is printed by client;
	  // here we only print qaudric in major axis position with its TRcard Number.
	  // warning it is up to the clien to print the Trsf matrix.

	  if (haveTrsf)
	    {
	      aPln.SetPosition(myTransformedAxis);
	    }
	  else
	    {
	      aPln.SetPosition(GetAxis());
	    }

	  aPln.Coefficients(A, B, C, D);
	  if(Abs(A) < 1.0e-07)
		  A = 0.0;
	  if(Abs(B) < 1.0e-07)
		  B = 0.0;
	  if(Abs(C) < 1.0e-07)
		  C = 0.0;
	  if(Abs(D) < 1.0e-07)
		  D = 0.0;

	  //aPln.Scale(gp_Pnt(0,0,0), Scale()); // this is to make mcnp happy with its cms.


	  // note that mcnp uses the plane eq. as follows AX+BY+CZ-D = 0
	  // Cascade uses AX+BY+CZ+D. Therefore D must be corrected accordingly.
	  // only globaly right handed oriented PX, PY and PZ are written;
	  // others are written as generic P;

	  if(GetMCType() == McCadCSGGeom_TRIPOLI)
	  {
		  theStream << setw(8) << "   SURF " << setw(5) <<  myNumber << " ";

		  if ( A > 1.e-7 && fabs(B) < 1.e-7 && fabs(C) < 1.e-7 )
		  {
			  theStream <<  setw(7) << "PLANEX";
			  theStream.setf(ios::right);
			  theStream << setw(15) << -D << endl;
			  theStream.unsetf(ios::right);
		  }
		  else if (B > 1.e-7 && fabs(A) < 1.e-7 && fabs(C) < 1.e-7)
		  {
			  theStream <<  setw(7) << "PLANEY";
			  theStream.setf(ios::right);
			  theStream << setw(15) << -D << endl;
			  theStream.unsetf(ios::right);
		  }
		  else if ( C > 1.e-7 && fabs(A) < 1.e-7 && fabs(B) < 1.e-7)
		  {
			  theStream <<  setw(7) << "PLANEZ";
			  theStream.setf(ios::right);
			  theStream << setw(15) << -D << endl;
			  theStream.unsetf(ios::right);
		  }
		  else
		  {
			  if (fabs(A) < 1.e-9)
				  A = 0.0 ;
			  if (fabs(B) < 1.e-9)
				  B = 0.0;
			  if (fabs(C) < 1.e-9 )
				  C = 0.0;

			  theStream << setw(7) << "PLANE";
			  theStream.setf(ios::right);
			  theStream << setw(15) << A << " "
					    << setw(15) << B << " "
					    << setw(15) << C << "\n\t\t"
					    << setw(20) << D << endl;
			  theStream.unsetf(ios::right);
		   }
	  }
	  else // MCNP is default
          {
                  if ( A > 1.e-7 && fabs(B) < 1.e-7 && fabs(C) < 1.e-7 )
                  {
                        theStream << setw(5) << myNumber << " " ;

                        theStream.setf(ios::right);
                                if (haveTrsf)
                                        theStream << setw(3) << myTNumber << " ";
                                else
                                        theStream << setw(4) << " ";
                                theStream << setw(4) << "PX "<< setw(8+myOutputPrecision) << -D << endl;
                        theStream.unsetf(ios::right);
                  }
                  else if (B > 1.e-7 && fabs(A) < 1.e-7 && fabs(C) < 1.e-7)
                  {
                         theStream<< setw(5) << myNumber << " " ;

                         theStream.setf(ios::right);
                                 if (haveTrsf)
                                         theStream  << setw(3) << myTNumber << " ";
                                 else
                                         theStream << setw(4) << " ";
                                 theStream <<  setw(4) << "PY "<< setw(8+myOutputPrecision) << -D << endl;
                         theStream.unsetf(ios::right);
                  }
                  else if ( C > 1.e-7 && fabs(A) < 1.e-7 && fabs(B) < 1.e-7)
                  {
                         theStream << setw(5) << myNumber << " " ;
                         theStream.setf(ios::right);
                                 if (haveTrsf)
                                         theStream << setw(3) << myTNumber << " " ;
                                 else
                                         theStream << setw(4) << " ";
                                 theStream << setw(4) << "PZ " << setw(8+myOutputPrecision) << -D << endl;
                         theStream.unsetf(ios::right);
                  }
                  else
                  {
                          theStream<< setw(5) << myNumber << " " ;

                          theStream.setf(ios::right);
                                  if (haveTrsf)
                                          theStream  << setw(3) << myTNumber << " ";
                                  else
                                          theStream << setw(4) << " ";
                                  theStream << setw(4)  << "P "
                                                        << setw(8+myOutputPrecision) << A << " "
                                                        << setw(8+myOutputPrecision) << B << " "
                                                        << setw(8+myOutputPrecision) << C << endl
                                                        << setw(14) << " "
                                                        << setw(8+myOutputPrecision) << -D << endl;
                          theStream.unsetf(ios::right);
                   }
	  }
}


Standard_Real McCadCSGGeom_Plane::Evaluate(const gp_Pnt& thePoint) const
{
	gp_Pln aPln(myAxis);
	return McCadGTOOL::Evaluate(aPln, thePoint);
}


gp_Pnt McCadCSGGeom_Plane::Value(const Standard_Real U, const Standard_Real V) const
{
	gp_Pln aPln(myAxis);
	return ElSLib::Value(U, V, aPln);
}


void McCadCSGGeom_Plane::SetCasSurf(const GeomAdaptor_Surface& theSurf)
{
	if (theSurf.GetType() != GeomAbs_Plane)
	{
		cout << "_#_McCadCSGGeom_Plane.cxx :: Surface Type incorrect !!" << endl;
		return;
	}
	gp_Pln aPln = theSurf.Plane();
	gp_Ax3 ax3 = aPln.Position();
	SetAxis(ax3);
}


Handle(Geom_Surface) McCadCSGGeom_Plane::CasSurf() const
{
	gp_Pln aPln(myAxis);
	Handle(Geom_Plane) surf = new Geom_Plane(aPln);
	return surf;
}


Standard_Boolean McCadCSGGeom_Plane::IsEqual(const Handle(McCadCSGGeom_Surface)& another) const
{
	if(another->GetType() != myType)
		return Standard_False;

	gp_Pln aPln(myAxis);
	gp_Ax3 pos = aPln.Position();
//	gp_Dir dir = pos.Direction();
//	gp_Pnt loc = pos.Location ();

	Standard_Real sA,sB,sC,sD, oA, oB, oC, oD;
	aPln.Coefficients(sA,sB,sC,sD);
//	Standard_Real D1 = dir.X()* loc.X() + dir.Y()* loc.Y() + dir.Z()*loc.Z();

	Handle(Geom_Plane) Gpln = Handle_Geom_Plane::DownCast(another->CasSurf());
	gp_Pln other = Gpln->Pln();
	other.Coefficients(oA,oB,oC,oD);

	if(Abs(oA)<1e-7)
		oA = 0.0;
	if(Abs(oB)<1e-7)
		oB = 0.0;
	if(Abs(oC)<1e-7)
		oC = 0.0;
	if(Abs(oD)<1e-7)
		oD = 0.0;
	if(Abs(sA)<1e-7)
		sA = 0.0;
	if(Abs(sB)<1e-7)
		sB = 0.0;
	if(Abs(sC)<1e-7)
		sC = 0.0;
	if(Abs(sD)<1e-7)
		sD = 0.0;

        /*cout << "\n" << oA << " " << oB << " " << oC << " " << oD;
        cout << "\n" << sA << " " << sB << " " << sC << " " << sD;*/

        if(Abs(Abs(oA) - Abs(sA)) > 1e-5 ||
           Abs(Abs(oB) - Abs(sB)) > 1e-5 ||
           Abs(Abs(oC) - Abs(sC)) > 1e-5 ||
           Abs(Abs(oD) - Abs(sD)) > 1e-5 )
		return Standard_False;

        /*cout << "\n" << Abs(oA-sA) << " " << Abs(oB-sB) << " " << Abs(oC-sC) << " " << Abs(oD-sD) << endl;*/

        if( Abs(oA-sA) < 1.e-5 &&
            Abs(oB-sB) < 1.e-5 &&
            Abs(oC-sC) < 1.e-5 &&
            Abs(oD-sD) < 1.e-5 )
		return Standard_True;

	/*cout << "\n" << oA << " " << oB << " " << oC << " " << oD;
	cout << "\n" << sA << " " << sB << " " << sC << " " << sD;*/

        /*gp_Ax3 otherPos = other.Position();
	gp_Dir otherDir = otherPos.Direction();
	gp_Pnt otherLoc = otherPos.Location();
	Standard_Real D2 = otherDir.X()* otherLoc.X() + otherDir.Y()* otherLoc.Y() + otherDir.Z()* otherLoc.Z();

	if( (dir.IsEqual(otherDir, 1.0e-03)  && fabs(D1 - D2) < 1.0e-03) )
	{
		return Standard_True;
        }*/

	return Standard_False;
}


Standard_Boolean McCadCSGGeom_Plane::IsGeomEqual(const Handle(McCadCSGGeom_Surface)& another) const
{
	if(another->GetType() != myType)
		return Standard_False;

	gp_Pln aPln(myAxis);
	gp_Ax3 pos = aPln.Position();
	gp_Dir dir = pos.Direction();
	gp_Pnt loc = pos.Location ();

	Standard_Real sA,sB,sC,sD, oA, oB, oC, oD;
	aPln.Coefficients(sA,sB,sC,sD);
	Standard_Real D1 = dir.X()* loc.X() + dir.Y()* loc.Y() + dir.Z()*loc.Z();

	Handle(Geom_Plane) Gpln = Handle_Geom_Plane::DownCast(another->CasSurf());
	gp_Pln other = Gpln->Pln();
	other.Coefficients(oA,oB,oC,oD);

	if(Abs(oA)<1e-7)
		oA = 0.0;
	if(Abs(oB)<1e-7)
		oB = 0.0;
	if(Abs(oC)<1e-7)
		oC = 0.0;
	if(Abs(oD)<1e-7)
		oD = 0.0;
	if(Abs(sA)<1e-7)
		sA = 0.0;
	if(Abs(sB)<1e-7)
		sB = 0.0;
	if(Abs(sC)<1e-7)
		sC = 0.0;
	if(Abs(sD)<1e-7)
		sD = 0.0;

        if(Abs(Abs(oA) - Abs(sA)) > 1e-5 ||
           Abs(Abs(oB) - Abs(sB)) > 1e-5 ||
           Abs(Abs(oC) - Abs(sC)) > 1e-5 ||
           Abs(Abs(oD) - Abs(sD)) > 1e-5 )
		return Standard_False;

        if( ( Abs(oA-sA) < 1.e-5 && Abs(oB-sB) < 1.e-5 && Abs(oC-sC) < 1.e-5 && Abs(oD-sD) < 1.e-5 )  ||
            ( Abs(oA+sA) < 1.e-5 && Abs(oB+sB) < 1.e-5 && Abs(oC+sC) < 1.e-5 && Abs(oD+sD) < 1.e-5 )  )
	{
		return Standard_True;
	}


	gp_Ax3 otherPos = other.Position();
	gp_Dir otherDir = otherPos.Direction();
	gp_Pnt otherLoc = otherPos.Location();
	Standard_Real D2 = otherDir.X()* otherLoc.X() + otherDir.Y()* otherLoc.Y() + otherDir.Z()* otherLoc.Z();

	if( (dir.IsEqual(otherDir, 1.0e-03)  && fabs(D1 - D2) < 1.0e-03) )
		return Standard_True;


	return Standard_False;

	/*if(another->GetType() != myType)
		return Standard_False;

	gp_Pln aPln(myAxis);
	gp_Ax3 pos = aPln.Position();
	gp_Dir dir = pos.Direction();
	gp_Pnt loc = pos.Location ();
	Standard_Real D1 = dir.X()* loc.X() + dir.Y()* loc.Y() + dir.Z()*loc.Z();

	Handle(Geom_Plane) Gpln = Handle_Geom_Plane::DownCast(another->CasSurf());
	gp_Pln other = Gpln->Pln();

	gp_Ax3 otherPos = other.Position();
	gp_Dir otherDir = otherPos.Direction();
	gp_Pnt otherLoc = otherPos.Location();
	Standard_Real D2 = otherDir.X()* otherLoc.X() + otherDir.Y()* otherLoc.Y() + otherDir.Z()* otherLoc.Z();

	if (dir.IsParallel(otherDir, 1.0e-03) && fabs(D1 - D2) < 1.0e-03)
	{
		return Standard_True;
	}
	return Standard_False;*/
}
