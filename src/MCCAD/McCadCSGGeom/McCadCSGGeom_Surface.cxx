#include <McCadCSGGeom_Surface.ixx>
#include <Handle_Geom_Transformation.hxx>
#include <Geom_Transformation.hxx>
#include <iomanip>

McCadCSGGeom_Surface::McCadCSGGeom_Surface()
{
  myNumber   =-1;
  myType     = McCadCSGGeom_OtherSurface;
  myMCType   = McCadCSGGeom_MCNP;
  myTransformedAxis= myAxis;
  myTNumber = 0;
  haveTrsf = Standard_False;
  myUnit = McCadCSGGeom_CM;
  myIsMacroBody = Standard_False;
  myMacroBodySurfaceHSequence = NULL;
}

Standard_Boolean McCadCSGGeom_Surface::HaveTransformation() const
{
  return haveTrsf;

}

void McCadCSGGeom_Surface::SetTrsfNumber(const Standard_Integer theTNumber)
{
  myTNumber = theTNumber;
}

Standard_Integer McCadCSGGeom_Surface::GetTrsfNumber() const
{

  return  myTNumber;
}

void McCadCSGGeom_Surface::SetTrsf(const gp_Trsf& theTrsf)
{
	myTrsf =  theTrsf;
}

gp_Trsf McCadCSGGeom_Surface::GetTrsf() const
{

  return myTrsf;

}

void McCadCSGGeom_Surface::SetLocation(const gp_Pnt& Loc)
{

  myAxis.SetLocation(Loc);

}

gp_Pnt McCadCSGGeom_Surface::GetLocation() const
{
 return myAxis.Location();
}

gp_Ax3 McCadCSGGeom_Surface::GetAxis() const
{
	gp_Ax3 scaledAxis = myAxis;
	scaledAxis.Scale(gp_Pnt(0,0,0), Scale());
    return scaledAxis;
}

void McCadCSGGeom_Surface::SetAxis(const gp_Ax3& theAx)
{
  myAxis = theAx;

  /////////////////////////////////////////////////////////////////////////////////////////
  if (!myAxis.Direct())
  {
	  cout << " IndirectSurface found !!!" << endl;
  }
   /////////////////////////////////////////////////////////////////////////////////////////

  if(myType == McCadCSGGeom_Planar ||
     myType ==  McCadCSGGeom_Spherical ||
     myType ==  McCadCSGGeom_Quadric ||
     myType == McCadCSGGeom_OtherSurface)
  {
      haveTrsf = Standard_False;
      return ;
  }

 // cout << "McCadCSGGeom_Surface :: " << myAxis.Location().X() << " " << myAxis.Location().Y() << " " << myAxis.Location().Z() << endl;

  gp_Ax3 stdAx(gp::Origin(),gp::DZ());

  gp_Ax3 stdAx3Z(myAxis.Location(),gp::DZ());
  gp_Ax3 stdAx3X(myAxis.Location(),gp::DX());
  gp_Ax3 stdAx3Y(myAxis.Location(),gp::DY());

  /*cout << "stdAx:   " << stdAx.Location().X() << " " << stdAx.Location().Y() << " " << stdAx.Location().Z() << endl;
  cout << "stdAx3X: " << stdAx3X.Location().X() << " " << stdAx3X.Location().Y() << " " << stdAx3X.Location().Z() << endl;
  cout << "stdAx3Y: " << stdAx3Y.Location().X() << " " << stdAx3Y.Location().Y() << " " << stdAx3Y.Location().Z() << endl;
  cout << "stdAx3Z: " << stdAx3Z.Location().X() << " " << stdAx3Z.Location().Y() << " " << stdAx3Z.Location().Z() << endl;*/

 // Standard_Boolean isCentered = Standard_False;

  /*if( stdAx.Location().X() == myAxis.Location().X() && //because the IsCoplanar function does not test if the Locations match (although
	  stdAx.Location().Y() == myAxis.Location().Y() && //it is supposed to do so)
	  stdAx.Location().Z() == myAxis.Location().Z()  )
	  isCentered = Standard_True;*/

/*  if(myType == McCadCSGGeom_Toroidal) // prevent a skewed transformation if parallel to z-Axis..
  {
	  gp_Dir torDir = myAxis.Direction();
	  gp_Dir stdDir = stdAx.Direction();

	  if(torDir.IsParallel(stdDir, 1e-5))
	  {
		  gp_Ax3 newAx(myAxis.Location(), gp::DZ());
		  myAxis = newAx;
	  }
  }*/

  /*if (isCentered && (stdAx.IsCoplanar(stdAx3Z, 0.001,0.001) || stdAx.IsCoplanar(stdAx3X, 0.001,0.001)
        || stdAx.IsCoplanar(stdAx3Y, 0.001,0.001)))*/
 // if(isCentered && stdAx.IsCoplanar(myAxis, 0.001, 0.001))
  if (myAxis.IsCoplanar(stdAx3Z, 0.001,0.001) || myAxis.IsCoplanar(stdAx3X, 0.001,0.001) || myAxis.IsCoplanar(stdAx3Y, 0.001,0.001))
  {
	  haveTrsf = Standard_False;
      return ;
  }
  else
  {
      // we draw on the translated Z-Axis
      myTransformedAxis.SetAxis(stdAx.Axis());
      haveTrsf  = Standard_True;
      return;
  }
}

McCadCSGGeom_SurfaceType McCadCSGGeom_Surface::GetType() const
{
  return myType;
}

void McCadCSGGeom_Surface::SetType(const McCadCSGGeom_SurfaceType theType)
{
  myType = theType;
}

Standard_Integer McCadCSGGeom_Surface::GetNumber() const
{
  return myNumber;
}

void McCadCSGGeom_Surface::SetNumber(const Standard_Integer theNumber)
{
  myNumber = theNumber;
}

void McCadCSGGeom_Surface::SetUnits(McCadCSGGeom_Unit theUnit)
{
	myUnit = theUnit;
}

McCadCSGGeom_Unit McCadCSGGeom_Surface::GetUnits() const
{
	return myUnit;
}

Standard_Real McCadCSGGeom_Surface::Scale() const
{
	Standard_Real scaleFactor(1.0);
	switch(myUnit)
	{
		case McCadCSGGeom_CM:   scaleFactor = 1.0; break;
		case McCadCSGGeom_MM:   scaleFactor = 0.1; break;
		case McCadCSGGeom_Inch: scaleFactor = 2.54; break;
		default : scaleFactor = 1.0; break;
	}
	return scaleFactor;
}

void McCadCSGGeom_Surface::SetMCType(const McCadCSGGeom_MCType& theMCType)
{
	myMCType = theMCType;
}


McCadCSGGeom_MCType McCadCSGGeom_Surface::GetMCType() const
{
	return myMCType;
}

///////// MACROBODY RELATED STUFF

void McCadCSGGeom_Surface::SetIsMacroBody()
{
	myIsMacroBody = Standard_True;
}

Standard_Boolean McCadCSGGeom_Surface::IsMacroBody()
{
	return myIsMacroBody;
}

void McCadCSGGeom_Surface::AddMacroBodySurface(const Standard_Integer& surfNum)
{
	if(myMacroBodySurfaceHSequence == NULL)
		myMacroBodySurfaceHSequence = new TColStd_HSequenceOfInteger();
	myMacroBodySurfaceHSequence->Append(surfNum);
}

Handle(TColStd_HSequenceOfInteger) McCadCSGGeom_Surface::GetMacroBodySurfaces()
{
	return myMacroBodySurfaceHSequence;
}


void McCadCSGGeom_Surface::SetMacroBodySurfaceNumbers(const Handle(TColStd_HSequenceOfInteger)& mbsn)
{
	myMacroBodySurfaceHSequence = mbsn;
}
