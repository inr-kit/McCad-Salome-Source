#include <McCadMcRead_SurfaceFactory.ixx>

#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>

#include <TColStd_HSequenceOfReal.hxx>

#include <algorithm>
#include <string>

#include <McCadCSGGeom_Plane.hxx>
#include <McCadCSGGeom_Cylinder.hxx>
#include <McCadCSGGeom_Cone.hxx>
#include <McCadCSGGeom_Sphere.hxx>
#include <McCadCSGGeom_Torus.hxx>
#include <McCadCSGGeom_GQ.hxx>
#include <McCadCSGGeom_SurfaceOfRevolution.hxx>
#include <TColStd_ListOfReal.hxx>
#include <TColStd_ListIteratorOfListOfReal.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Curve.hxx>
#include <Geom_SurfaceOfRevolution.hxx>

#define PI 3.1415926535897932384626433832795

McCadMcRead_SurfaceFactory::McCadMcRead_SurfaceFactory()
{
	myMacroBodySurfaces = NULL;
	myIsMacroBody = Standard_False;
	NotDone();
}

// copy existing McCadCSGGeom_Surface
McCadMcRead_SurfaceFactory::McCadMcRead_SurfaceFactory(const Handle_McCadCSGGeom_Surface& oldSurf)
{
	switch(oldSurf->GetType())
	{
		case McCadCSGGeom_Planar:
		{
			Standard_Real a,b,c,d;
			Handle(McCadCSGGeom_Plane) thePlane = Handle(McCadCSGGeom_Plane)::DownCast(oldSurf);
			thePlane->Coefficients(a,b,c,d);
			mySurface = new McCadCSGGeom_Plane(a,b,c,d);
			mySurface->SetNumber(thePlane->GetNumber());

			if(thePlane->HaveTransformation())
			{
				mySurface->SetTrsf(thePlane->GetTrsf());
				mySurface->SetTrsfNumber(thePlane->GetTrsfNumber());
			}

			break;
		}
		case McCadCSGGeom_Cylindrical:
		{
			Standard_Real r;
			Handle(McCadCSGGeom_Cylinder) theCylinder = Handle(McCadCSGGeom_Cylinder)::DownCast(oldSurf);
			r = theCylinder->GetRadius();
			gp_Ax3 axis = theCylinder->GetAxis();

			mySurface = new McCadCSGGeom_Cylinder(axis, r);
			mySurface->SetNumber(theCylinder->GetNumber());

			if(theCylinder->HaveTransformation())
			{
				mySurface->SetTrsf(theCylinder->GetTrsf());
				mySurface->SetTrsfNumber(theCylinder->GetTrsfNumber());
			}

			break;
		}
		case McCadCSGGeom_Conical:
		{
			Standard_Real r, phi;
			Handle(McCadCSGGeom_Cone) theCone = Handle(McCadCSGGeom_Cone)::DownCast(oldSurf);
			r = theCone->GetRadius();
			phi = theCone->GetSemiAngle();
			gp_Ax3 axis = theCone->GetAxis();

			mySurface = new McCadCSGGeom_Cone(axis, phi, r);
			mySurface->SetNumber(theCone->GetNumber());

			if(theCone->HaveTransformation())
			{
				mySurface->SetTrsf(theCone->GetTrsf());
				mySurface->SetTrsfNumber(theCone->GetTrsfNumber());
			}

			break;
		}
		case McCadCSGGeom_Spherical:
		{
			Standard_Real r;
			Handle(McCadCSGGeom_Sphere) theSphere = Handle(McCadCSGGeom_Sphere)::DownCast(oldSurf);
			r = theSphere->GetRadius();
			gp_Ax3 axis = theSphere->GetAxis();

			mySurface = new McCadCSGGeom_Sphere(axis, r);
			mySurface->SetNumber(theSphere->GetNumber());

			if(theSphere->HaveTransformation())
			{
				mySurface->SetTrsf(theSphere->GetTrsf());
				mySurface->SetTrsfNumber(theSphere->GetTrsfNumber());
			}

			break;
		}
		case McCadCSGGeom_Toroidal:
		{
			Standard_Real rMin, rMaj;
			Handle(McCadCSGGeom_Torus) theTorus = Handle(McCadCSGGeom_Torus)::DownCast(oldSurf);
			rMin = theTorus->GetMinorRadius();
			rMaj = theTorus->GetMajorRadius();
			gp_Ax3 axis = theTorus->GetAxis();

			mySurface = new McCadCSGGeom_Torus(axis, rMin, rMaj);
			mySurface->SetNumber(theTorus->GetNumber());

			if(theTorus->HaveTransformation())
			{
				mySurface->SetTrsf(theTorus->GetTrsf());
				mySurface->SetTrsfNumber(theTorus->GetTrsfNumber());
			}

			break;
		}
		case McCadCSGGeom_Quadric:
		{

			Standard_Real c1, c2, c3, c4, c5, c6, c7, c8, c9, c10;
			Handle(McCadCSGGeom_GQ) theGQ = Handle(McCadCSGGeom_GQ)::DownCast(oldSurf);
			theGQ->Coefficients(c1, c2, c3, c4, c5, c6, c7, c8, c9, c10);
			mySurface = new McCadCSGGeom_GQ(c1, c2, c3, c4, c5, c6, c7, c8, c9, c10);
			mySurface->SetNumber(theGQ->GetNumber());

			if(theGQ->HaveTransformation())
			{
				mySurface->SetTrsf(theGQ->GetTrsf());
				mySurface->SetTrsfNumber(theGQ->GetTrsfNumber());
			}

			break;
		}
		case McCadCSGGeom_OtherSurface:
		{
			cout << "_#_McCadMcRead_SurfaceFactory :: unknown surface could not be initialized\n";
			break;
		}
		default:
			cout << "_#_McCadMcRead_SurfaceFactory :: unknown surface type\n";
	}

	Done();
}


Standard_Boolean McCadMcRead_SurfaceFactory::IsDone() const
{
	return myIsDone;
}


void McCadMcRead_SurfaceFactory::Done()
{
	myIsDone = Standard_True;
}


void McCadMcRead_SurfaceFactory::NotDone()
{
	myIsDone = Standard_False;
}


Standard_Boolean McCadMcRead_SurfaceFactory::MakeSurface(const TCollection_AsciiString& sDesc)
{
	if (sDesc.Search(" P") > 0)
	{
		if (MakePlane(sDesc))
			Done();
		else
		{
			::cout << "Making a Plane surface failed!!" << endl;
			NotDone();
		}
	}
	else if (sDesc.Search(" S") > 0  && sDesc.Search(" SQ") < 0) //
	{
		if(MakeSphere(sDesc))
		Done();
		else
		{
			::cout << "Making a Sphere surface failed!!" << endl;
			NotDone();
		}
	}
	else if (sDesc.Search(" C") > 0 )
	{
		if(MakeCylinder(sDesc))
		Done();
		else
		{
			::cout << "Making a Cylinder surface failed!!" << endl;
			NotDone();
		}
	}
	else if (sDesc.Search(" K") > 0 )
	{
		if(MakeCone(sDesc))
		Done();
		else
		{
			::cout << "Making a Cone surface failed!!" << endl;
			NotDone();
		}
	}
	else if (sDesc.Search(" T") > 0 )
	{
		if(MakeTorus(sDesc))
		Done();
		else
		{
			::cout << "Making a Torus surface failed!!" << endl;
			NotDone();
		}
	}
	else if (sDesc.Search(" GQ") > 0 )
	{
		if(MakeGQ(sDesc))
			Done();
		else
		{
			::cout << "Making a GQ surface failed!!" << endl;
			NotDone();
		}
	}
	else if (sDesc.Search(" SQ") > 0 )
	{
		cout << "SQ not yet implemented \n";
	}
	else if (sDesc.Search(" X") > 0 || sDesc.Search(" Y") > 0 || sDesc.Search(" Z") > 0) // axissymmetric surfaces defined by points
	{
		if(MakeAxissymmetricSurfaceByPoints(sDesc))
			Done();
		else
		{
			::cout << "Making an Axissymmetric Surface defined by Points failed for : " << sDesc.ToCString() << endl;
			NotDone();
		}
	}
	else if(sDesc.Search(" BOX ") > 0)
	{
		InitMacroBodySurfaces();

		if(MakeBOX(sDesc))
			Done();
		else
		{
			cout << "Making a BOX failed for : " << sDesc.ToCString() << endl;
			NotDone();
		}
	}
	else if(sDesc.Search(" RPP ") > 0)
	{
		InitMacroBodySurfaces();
		cout << "RPP -- MACRO BODY!!!\n";
		if(MakeRPP(sDesc))
			Done();
		else
		{
			cout << "Making a RPP failed for : " << sDesc.ToCString() << endl;
			NotDone();
		}
	}
	else if(sDesc.Search(" RCC ") > 0)
	{
		InitMacroBodySurfaces();
		cout << "RCC -- MACRO BODY!!!\n";
		if(MakeRCC(sDesc))
			Done();
		else
		{
			cout << "Making a RCC failed for : " << sDesc.ToCString() << endl;
			NotDone();
		}
	}
	else if(sDesc.Search(" RHP ") > 0 || sDesc.Search(" HEX ") > 0 )
	{
		InitMacroBodySurfaces();
		cout << "HEX/RHP -- MACRO BODY!!!\n";

		if(MakeHEX(sDesc))
			Done();
		else
		{
			cout << "Making a RHP/HEX failed for : " << sDesc.ToCString() << endl;
			NotDone();
		}
	}
	else if(sDesc.Search(" REC ") > 0)
	{
		InitMacroBodySurfaces();
		cout << "REC -- MACRO BODY!!!\n";
		if(MakeREC(sDesc))
			Done();
		else
		{
			cout << "Making a REC failed for : " << sDesc.ToCString() << endl;
			NotDone();
		}
	}
	else if(sDesc.Search(" TRC ") > 0)
	{
		InitMacroBodySurfaces();
		cout << "TRC -- MACRO BODY!!!\n";
		if(MakeTRC(sDesc))
			Done();
		else
		{
			cout << "Making a TRC failed for : " << sDesc.ToCString() << endl;
			NotDone();
		}
	}
	else if(sDesc.Search(" ELL ") > 0)
	{
		InitMacroBodySurfaces();
		cout << "ELL -- MACRO BODY!!!\n";
		if(MakeELL(sDesc))
			Done();
		else
		{
			cout << "Making a ELL failed for : " << sDesc.ToCString() << endl;
			NotDone();
		}
	}
	else if(sDesc.Search(" WED ") > 0)
	{
		InitMacroBodySurfaces();
		cout << "WED -- MACRO BODY!!!\n";
		if(MakeWED(sDesc))
			Done();
		else
		{
			cout << "Making a WED failed for : " << sDesc.ToCString() << endl;
			NotDone();
		}
	}
	else if(sDesc.Search(" ARB ") > 0)
	{
		InitMacroBodySurfaces();
		cout << "ARB -- MACRO BODY!!!\n";
		if(MakeARB(sDesc))
			Done();
		else
		{
			cout << "Making a ARB failed for : " << sDesc.ToCString() << endl;
			NotDone();
		}
	}
	else
	{
		NotDone();
		::cout << "Making a surface failed!!" << endl;
		::cout << "Unknown surface type!!" << endl;
	}


	if (IsDone())
	{
		// set surface number
		// set TrsfNumber
		// set trsf;
		TCollection_AsciiString tmpStr = sDesc;
		tmpStr.LeftAdjust();
		tmpStr.RightAdjust();
		TCollection_AsciiString sEnd;
		if( tmpStr.Search("*") > 0 )
		{
                        //cout << "Reflecting Surface found. This information is not retained. " << endl;
			sEnd = tmpStr.Split(tmpStr.Search("*"));
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
			tmpStr.Clear();
			tmpStr = sEnd;
		}
		tmpStr.LeftAdjust();
		tmpStr.RightAdjust();
		sEnd = tmpStr.Split(tmpStr.Search(" "));
		// tmpStr is SurfNum
		if(tmpStr.IsIntegerValue())
		{
			if(tmpStr.IntegerValue() > 0 && tmpStr.IntegerValue() < 99999)
				mySurface->SetNumber(tmpStr.IntegerValue());
                        /*else
			{
                                cout << "Surface Number out of range:  " << tmpStr.IntegerValue() << endl;
                        }*/
		}
		else
		{
			cout << "Surface Number missing on the surface card!!" << tmpStr.IntegerValue() << endl;
		}
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		tmpStr.Clear();
		tmpStr = sEnd;
		sEnd = tmpStr.Split(tmpStr.Search(" "));

		//////////////////////////////////////
		// tmpStr is TrsfNum or period
		if(tmpStr.IsIntegerValue())
		{
			if(tmpStr.IntegerValue() > 0)
			{
				mySurface->SetTrsfNumber(tmpStr.IntegerValue());

				if(mySurface->IsMacroBody())
				{
					Standard_Integer sInt = tmpStr.IntegerValue();

					for(Standard_Integer i=1; i<=myMacroBodySurfaces->Length(); i++)
						myMacroBodySurfaces->Value(i)->SetTrsfNumber(sInt);
				}
			}
			else
			{
				cout << "Periodic Surface found. This information is not retained. " << endl;
			}
		}
	}

	return IsDone();

}

Standard_Boolean McCadMcRead_SurfaceFactory::MakePlane(	const TCollection_AsciiString& sDesc)
{
	//cout << "Making a planar Surface !!" << endl;
	TCollection_AsciiString tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	Standard_Real a=0, b=0, c=0, d=0;

	if (tmpStr.Search(" PX") > 0)
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("PX")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		a = 1;
		d = sEnd.RealValue();
		d=d*-1;
	}
	else if (tmpStr.Search(" PY") > 0)
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("PY")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		b = 1;
		d = sEnd.RealValue();
		d=d*-1;
	}
	else if (tmpStr.Search(" PZ") > 0)
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("PZ")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		c = 1;
		d = sEnd.RealValue();
		d=d*-1;
	}
	else if (tmpStr.Search(" P ") > 0)
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("P")+1);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while (true)
		{
			TCollection_AsciiString sEnd2;
			if (sEnd.Search(" ") > 1)
				sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if (sEnd.IsRealValue())
				realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if (realSeq->Length() == 4)
		{
			a = realSeq->Value(1);
			b = realSeq->Value(2);
			c = realSeq->Value(3);
			d = realSeq->Value(4);
			d=d*-1;
		}
		else if (realSeq->Length() == 9)
		{
			gp_Pnt P1(realSeq->Value(1), realSeq->Value(2), realSeq->Value(3));
			gp_Pnt P2(realSeq->Value(4), realSeq->Value(5), realSeq->Value(6));
			gp_Pnt P3(realSeq->Value(7), realSeq->Value(8), realSeq->Value(9));
			gp_Vec v1(P1, P2), v2(P1, P3);
			if (v1.IsParallel(v2, 0.0001))
			{
::				cout << "Colinear points: can not make a Plane !" << endl;
				return false;
			}
			gp_Vec v3 = v1 ^ v2;
			v3.Normalize();
			gp_Dir D1(v3);
			gp_Pln aPln(P1,D1);
			// orient the plane
			Standard_Real a1=0, b1=0, c1=0, d1=0;
			aPln.Coefficients(a1,b1,c1,d1);

			//cout << "GENERAL PLANE DEFINED BY POINTS --> Coefficients :  " << a1 << " " << b1 << " " << c1 << " " << d1 << endl;

			d1*=-1; // what follows is a test for the planes orientation
					// for this reason we have to multiply d1 by -1 since the
					// plane equation is Ax+By+Cz -D =0

			if(d1<0)
			{
				a = -a1;
				b = -b1;
				c = -c1;
				d = d1;
			}
			else if ( d1 > 0 )
			{
				a = a1;
				b = b1;
				c = c1;
				d = -d1;
			}
			else
			{
				if(c1 == 0)
				{
					if(b1 == 0) //D=0 C=0 B=0;

					{
						if(a1 < 0 )
						{
							a= -a1; b = -b1; c = -c1;
							d = -d1;
						}
					}
					else
					{
						if(b1<0) //D= 0; C =0;

						{
							a= -a1; b = -b1; c = -c1;
							d = -d1;
						}
					}
				}
				else
				{
					if(c1 < 0) //D =0;

					{
						a= -a1; b = -b1; c = -c1;
						d = -d1;
					}
				}

			}
		}
	}
	else
	{
		::cout << "Can not make a Plane: insufficient number of points !" << endl;
		return false;
	}
	mySurface = new McCadCSGGeom_Plane(a,b,c,d);
	return true;
}

Standard_Boolean McCadMcRead_SurfaceFactory::MakeCylinder(
		const TCollection_AsciiString& sDesc)
{
	TCollection_AsciiString tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	Standard_Real r=0;
	gp_Ax3 a3X;
	if (tmpStr.Search(" C/X") > 0)
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("C/X")+3);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while (true)
		{
			TCollection_AsciiString sEnd2;
			if (sEnd.Search(" ") > 1)
				sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if (sEnd.IsRealValue())
				realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if (realSeq->Length() == 3 && realSeq->Value(3) > 0)
		{
			gp_Dir aDir = gp::DX();
			gp_Pnt aPnt(0, realSeq->Value(1), realSeq->Value(2));
			gp_Ax3 t3X(aPnt, aDir);
			a3X = t3X;
			r = realSeq->Value(3);
		}
		else
		{
::			cout << "Error in Cylinder Surface Card Parameter!" << endl; return false;
		}
	}
	else if ( tmpStr.Search(" C/Y") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("C/Y")+3);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while(true)
		{
			TCollection_AsciiString sEnd2;
			if(sEnd.Search(" ") > 1)
			sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if(sEnd.IsRealValue()) realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if(realSeq->Length() == 3 && realSeq->Value(3) > 0 )
		{
			gp_Ax3 t3X(gp_Pnt(realSeq->Value(1),0,realSeq->Value(2)), gp::DY());
			a3X = t3X;
			r = realSeq->Value(3);
		}
		else
		{
			::cout << "Error in Cylinder Surface Card Parameter!" << endl; return false;
		}
	}
	else if ( tmpStr.Search(" C/Z") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("C/Z")+3);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while(true)
		{
			TCollection_AsciiString sEnd2;
			if(sEnd.Search(" ") > 1)
			sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if(sEnd.IsRealValue()) realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if(realSeq->Length() == 3 && realSeq->Value(3) > 0 )
		{
			gp_Ax3 t3X(gp_Pnt(realSeq->Value(1),realSeq->Value(2),0), gp::DZ());
			a3X = t3X;
			r = realSeq->Value(3);
		}
		else
		{
			::cout << "Error in Cylinder Surface Card Parameter!" << endl; return false;
		}
	}
	else if ( tmpStr.Search(" CX") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("CX")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		if(sEnd.RealValue() > 0 )
		{
			gp_Ax3 t3X(gp_Pnt(0,0,0), gp::DX());
			a3X = t3X;
			r = sEnd.RealValue();
		}
		else
		{
			::cout << "Error in Cylinder Surface Card Parameter!" << endl; return false;
		}
	}
	else if ( tmpStr.Search(" CY") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("CY")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		if(sEnd.RealValue() > 0 )
		{
			gp_Ax3 t3X(gp_Pnt(0,0,0), gp::DY());
			a3X = t3X;
			r = sEnd.RealValue();
		}
		else
		{
			::cout << "Error in Cylinder Surface Card Parameter!" << endl;
			return false;

		}
	}
	else if ( tmpStr.Search(" CZ") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("CZ")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		if(sEnd.RealValue() > 0 )
		{
			gp_Ax3 t3X(gp_Pnt(0,0,0), gp::DZ());
			a3X = t3X;
			r = sEnd.RealValue();
		}
		else
		{
			::cout << "Error in Cylinder Surface Card Parameter!" << endl;
			return false;

		}
	}
	else
	{
		::cout << "Error in Cylinder Surface Card Parameter!" << endl;
		return false;
	}

	mySurface = new McCadCSGGeom_Cylinder (a3X,r);
	return true;
}


Standard_Boolean McCadMcRead_SurfaceFactory::MakeCone(const TCollection_AsciiString& sDesc)
{
	int theSheet(0);

	TCollection_AsciiString tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	Standard_Real ang=0, rad=0;
	gp_Ax3 a3X;
	if (tmpStr.Search(" K/X") > 0)
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("K/X")+3);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while (true)
		{
			TCollection_AsciiString sEnd2;
			if (sEnd.Search(" ") > 1)
				sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if (sEnd.IsRealValue())
				realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if ((realSeq->Length() == 4 || realSeq->Length() == 5) && realSeq->Value(4) > 0 )
		{
			if(realSeq->Length() == 5)
				theSheet = int(realSeq->Value(5));

			gp_Ax3 t3X(gp_Pnt(realSeq->Value(1), realSeq->Value(2),
					realSeq->Value(3)), gp::DX());
			a3X = t3X;
			Standard_Real tanAng = sqrt(realSeq->Value(4));
			ang = atan(tanAng);
			if (realSeq->Length() == 5)
			{
				ang = -1 * ang*realSeq->Value(5);
			}
			//a half angle Ang should be  between -Pi/2. and Pi/2.
			if (ang > PI/2 || ang < -PI/2.)
			{
::				cout << "The semi-angle of the cone is not  between -Pi/2. and Pi/2.!!" << endl;
				return false;
			}
		}
		else
		{
			::cout << "Error in Cone Surface Card Parameter!" << endl;
			return false;
		}
	}
	else if ( tmpStr.Search(" K/Y") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("K/Y")+3);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while(true)
		{
			TCollection_AsciiString sEnd2;
			if(sEnd.Search(" ") > 1)
			sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if(sEnd.IsRealValue()) realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if((realSeq->Length() == 4 ||realSeq->Length() == 5) && realSeq->Value(4) > 0 )
		{
			if(realSeq->Length() == 5)
				theSheet = int(realSeq->Value(5));

			gp_Ax3 t3X(gp_Pnt(realSeq->Value(1),realSeq->Value(2),realSeq->Value(3)), gp::DY());
			a3X = t3X;
			Standard_Real tanAng = sqrt(realSeq->Value(4));
			ang = atan(tanAng);
			if(realSeq->Length() == 5)
			{
				ang = -1 * ang*realSeq->Value(5);
			}
			//a half angle Ang should be  between -Pi/2. and Pi/2.
			if(ang > PI/2 || ang < -PI/2.)
			{
				::cout << "The semi-angle of the cone is not  between -Pi/2. and Pi/2.!!" << endl;
				return false;
			}

		}
		else
		{
			::cout << "Error in Cone Surface Card Parameter!" << endl;
			return false;
		}
	}
	else if ( tmpStr.Search(" K/Z") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("K/Z")+3);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while(true)
		{
			TCollection_AsciiString sEnd2;
			if(sEnd.Search(" ") > 1 )
			sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if(sEnd.IsRealValue()) realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if((realSeq->Length() == 4 ||realSeq->Length() == 5) && realSeq->Value(4) > 0 )
		{
			if(realSeq->Length() == 5)
				theSheet = int(realSeq->Value(5));

			gp_Ax3 t3X(gp_Pnt(realSeq->Value(1),realSeq->Value(2),realSeq->Value(3)), gp::DZ());
			a3X = t3X;
			Standard_Real tanAng = sqrt(realSeq->Value(4));
			ang = atan(tanAng);
			if(realSeq->Length() == 5)
			{
				ang = -1 * ang*realSeq->Value(5);
			}
			//a half angle Ang should be  between -Pi/2. and Pi/2.
			if(ang > PI/2 || ang < -PI/2.)
			{
				::cout << "The semi-angle of the cone is not  between -Pi/2. and Pi/2.!!" << endl;
				return false;
			}

		}
		else
		{
			::cout << "Error in Cone Surface Card Parameter!" << endl;
			return false;
		}
	}
	else if ( tmpStr.Search(" KX") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("KX")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while(true)
		{
			TCollection_AsciiString sEnd2;
			if(sEnd.Search(" ") > 1)
			sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if(sEnd.IsRealValue()) realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if(realSeq->Length() == 2 || realSeq->Length() == 3)
		{
			if(realSeq->Length() == 3)
				theSheet = int(realSeq->Value(3));

			gp_Ax3 t3X(gp_Pnt(realSeq->Value(1),0,0), gp::DX());
			a3X = t3X;
			Standard_Real tanAng = sqrt(realSeq->Value(2));
			ang = atan(tanAng);
			//a half angle Ang should be  between -Pi/2. and Pi/2.
			if(realSeq->Length() == 3)
			{
				ang = -1 * ang*realSeq->Value(3);
			}
			if(ang > PI/2 || ang < -PI/2.)
			{
				::cout << "The semi-angle of the cone is not  between -Pi/2. and Pi/2.!!" << endl;
				return false;
			}

		}
		else
		{
			::cout << "Error in Cone Surface Card Parameter!" << endl;
			return false;
		}
	}
	else if ( tmpStr.Search(" KY") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("KY")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while(true)
		{
			TCollection_AsciiString sEnd2;
			if(sEnd.Search(" ") > 1)
			sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if(sEnd.IsRealValue()) realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if(realSeq->Length() == 2 || realSeq->Length() == 3)
		{
			if(realSeq->Length() == 3)
				theSheet = int(realSeq->Value(3));

			gp_Ax3 t3X(gp_Pnt(0,realSeq->Value(1),0), gp::DY());
			a3X = t3X;
			Standard_Real tanAng = sqrt(realSeq->Value(2));
		//	cout << "TANGENT ALPHA : " << tanAng << endl;
			ang = atan(tanAng);
			if(realSeq->Length() == 3)
			{
				ang = -1 * ang*realSeq->Value(3);
			}
			//a half angle Ang should be  between -Pi/2. and Pi/2.
			if(ang > PI/2 || ang < -PI/2.)
			{
				::cout << "The semi-angle of the cone is not  between -Pi/2. and Pi/2.!!" << endl;
				return false;
			}

		}
		else
		{
			::cout << "Error in Cone Surface Card Parameter!" << endl;
			return false;
		}
	}
	else if ( tmpStr.Search(" KZ") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("KZ")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;

		while(true)
		{
			TCollection_AsciiString sEnd2;
			if(sEnd.Search(" ") > 1 )
			{
				sEnd2 = sEnd.Split(sEnd.Search(" "));
			}
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}

			if(sEnd.IsRealValue())
				realSeq->Append(sEnd.RealValue());

			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}

		if(realSeq->Length() == 2 || realSeq->Length() == 3)
		{
			if(realSeq->Length() == 3)
				theSheet = int(realSeq->Value(3));

			gp_Ax3 t3X(gp_Pnt(0,0,realSeq->Value(1)), gp::DZ());
			a3X = t3X;
			Standard_Real tanAng = sqrt(realSeq->Value(2));
			ang = atan(tanAng);

			if(realSeq->Length() == 3)
			{
				ang = -1 * ang * realSeq->Value(3);
			}

			//a half angle Ang should be  between -Pi/2. and Pi/2.
			if(ang > PI/2 || ang < -PI/2.)
			{
				::cout << "The semi-angle of the cone is not  between -Pi/2. and Pi/2.!!" << endl;
				return false;
			}

		}
		else
		{
			::cout << "Error in Cone Surface Card Parameter!" << endl;
			return false;
		}
	}
	else
	{
		::cout << "Error in Cone Surface Card Parameter!" << endl;
		return false;
	}

	mySurface = new McCadCSGGeom_Cone(a3X,ang,rad, theSheet);
	return true;
}


Standard_Boolean McCadMcRead_SurfaceFactory::MakeSphere(const TCollection_AsciiString& sDesc)
{
	TCollection_AsciiString tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();

	Standard_Real rad=0.0;
	gp_Ax3 a3X;
	gp_Pnt origin(0,0,0);

	if (tmpStr.Search(" S ") > 0 || tmpStr.Search(" SPH ") > 0) // latter is macrobody but has the same definition
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("S")+1);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while (true)
		{
			TCollection_AsciiString sEnd2;
			if (sEnd.Search(" ") > 1)
				sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if (sEnd.IsRealValue())
				realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if (realSeq->Length() == 4 && realSeq->Value(4) > 0)
		{
			gp_Ax3 t3X(gp_Pnt(realSeq->Value(1), realSeq->Value(2),	realSeq->Value(3)), gp::DZ());
			a3X = t3X;
			rad = realSeq->Value(4);

			if (rad <=0)
			{
				cout << "The radius of a shpere should be grater than zero!!" << endl;
				return false;
			}
		}
		else
		{
			cout << "Error in Sphere Surface Card Parameter!" << endl;
			return false;
		}
	}
	else if ( tmpStr.Search(" SX") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("SX")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		TCollection_AsciiString sEnd2;
		if(sEnd.Search(" ") > 1)
		sEnd2 = sEnd.Split(sEnd.Search(" "));
		Standard_Real x = sEnd.RealValue();
		rad = sEnd2.RealValue();
		gp_Ax3 t3X(gp_Pnt(x,0,0), gp::DZ());
		a3X = t3X;
		if(rad <=0 )
		{
			::cout << "The radius of a shpere should be grater than zero!!" << endl;
			return false;
		}
	}
	else if ( tmpStr.Search(" SY") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("SY")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		TCollection_AsciiString sEnd2;
		if(sEnd.Search(" "))
		sEnd2 = sEnd.Split(sEnd.Search(" "));
		Standard_Real y = sEnd.RealValue();
		rad = sEnd2.RealValue();
		gp_Ax3 t3X(gp_Pnt(0,y,0), gp::DZ());
		a3X = t3X;
		if(rad <=0 )
		{
			::cout << "The radius of a shpere should be grater than zero!!" << endl;
			return false;
		}
	}
	else if ( tmpStr.Search(" SZ") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("SZ")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		TCollection_AsciiString sEnd2;
		if(sEnd.Search(" ") > 1)
		sEnd2 = sEnd.Split(sEnd.Search(" "));
		Standard_Real z = sEnd.RealValue();
		rad = sEnd2.RealValue();
		gp_Ax3 t3X(gp_Pnt(0,0,z), gp::DZ());
		a3X = t3X;
		if(rad <=0 )
		{
			::cout << "The radius of a shpere should be grater than zero!!" << endl;
			return false;
		}
	}
	else if ( tmpStr.Search(" SO") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("SO")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		TCollection_AsciiString sEnd2;
		if(sEnd.Search(" ") > 1)
			sEnd2 = sEnd.Split(sEnd.Search(" "));

		rad = sEnd.RealValue();
		gp_Ax3 t3X(gp_Pnt(0,0,0), gp::DZ());
		a3X = t3X;
		if(rad <= 0 )
		{
			::cout << "The radius of a shpere should be grater than zero!!" << endl;
			return false;
		}
	}
	else
	{
		::cout << "Error in Sphere Surface Card Parameter!" << endl;
		return false;
	}

	mySurface = new McCadCSGGeom_Sphere(a3X,rad);
	return true;
}


Standard_Boolean McCadMcRead_SurfaceFactory::MakeTorus(const TCollection_AsciiString& sDesc)
{
	TCollection_AsciiString tmpStr = sDesc;
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();
	Standard_Real majrad=0, minrad=0;
	gp_Ax3 a3X;
	if (tmpStr.Search(" TX") > 0)
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("TX")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while (true)
		{
			TCollection_AsciiString sEnd2;
			if (sEnd.Search(" ") > 1)
				sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if (sEnd.IsRealValue())
				realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if (realSeq->Length() == 6 && realSeq->Value(4) > 0	&& realSeq->Value(5) > 0 && realSeq->Value(6) > 0)
		{
			if (realSeq->Value(5) != realSeq->Value(6))
			{
				//Make Surface Of Reveolution
				//Create Ax2
			//	cout << "ELLIPTICAL TORUS - X\n";

				gp_Pnt ellOri(realSeq->Value(1),realSeq->Value(2)+realSeq->Value(4),realSeq->Value(3));
				gp_Pnt revOri(realSeq->Value(1), realSeq->Value(2), realSeq->Value(3));
				gp_Dir ellipseNormal(0,0,1);
				gp_Dir ellipseMajAxisDir;

				gp_Ax2 ax2;
				Standard_Real rMaj, rMin;
				if(realSeq->Value(5) >= realSeq->Value(6)) // orient ax2 of ellipse according to input
				{
					ellipseMajAxisDir = gp_Dir(1,0,0);
					rMin = realSeq->Value(6);
					rMaj = realSeq->Value(5);
				}
				else
				{
					ellipseMajAxisDir = gp_Dir(0,1,0);
					rMin = realSeq->Value(5);
					rMaj = realSeq->Value(6);
				}

				ax2 = gp_Ax2(ellOri, ellipseNormal, ellipseMajAxisDir);

				Handle(Geom_Ellipse) anEllipse = new Geom_Ellipse(ax2, rMaj, rMin);
				//make surface of revolution
				gp_Dir rotAxis(1,0,0);
				gp_Ax1 revAx(revOri, rotAxis);
				Handle(Geom_Curve) basisCurve = Handle(Geom_Curve)::DownCast(anEllipse);

				mySurface = new McCadCSGGeom_SurfaceOfRevolution(revAx, basisCurve, realSeq);

				return true;
			}
			gp_Ax3 t3X(gp_Pnt(realSeq->Value(1),realSeq->Value(2),realSeq->Value(3)), gp::DX());
			a3X = t3X;
			majrad = realSeq->Value(4);
			minrad = realSeq->Value(5);
		}
		else
		{
			::cout << "Error in Sphere Surface Card Parameter!" << endl;
			return false;
		}
	}
	else if ( tmpStr.Search(" TY") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("TY")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while(true)
		{
			TCollection_AsciiString sEnd2;
			if(sEnd.Search(" ") > 1)
			sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if(sEnd.IsRealValue()) realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if(realSeq->Length() == 6 && realSeq->Value(4) > 0 && realSeq->Value(5) > 0 && realSeq->Value(6) > 0)
		{
			if (realSeq->Value(5) != realSeq->Value(6))
			{
				//cout << "ELLIPTICAL TORUS - Y\n";

				gp_Pnt ellOri(realSeq->Value(1),realSeq->Value(2),realSeq->Value(3)+realSeq->Value(4));
				gp_Pnt revOri(realSeq->Value(1), realSeq->Value(2), realSeq->Value(3));
				gp_Dir ellipseNormal(1,0,0);
				gp_Dir ellipseMajAxisDir;

				gp_Ax2 ax2;
				Standard_Real rMaj, rMin;
				if(realSeq->Value(5) >= realSeq->Value(6)) // orient ax2 of ellipse according to input
				{
					ellipseMajAxisDir = gp_Dir(0,1,0);
					rMin = realSeq->Value(6);
					rMaj = realSeq->Value(5);
				}
				else
				{
					ellipseMajAxisDir = gp_Dir(0,0,1);
					rMin = realSeq->Value(5);
					rMaj = realSeq->Value(6);
				}

				ax2 = gp_Ax2(ellOri, ellipseNormal, ellipseMajAxisDir);

				Handle(Geom_Ellipse) anEllipse = new Geom_Ellipse(ax2, rMaj, rMin);
				//make surface of revolution
				gp_Dir rotAxis(0,1,0);
				gp_Ax1 revAx(revOri, rotAxis);
				Handle(Geom_Curve) basisCurve = Handle(Geom_Curve)::DownCast(anEllipse);

				mySurface = new McCadCSGGeom_SurfaceOfRevolution(revAx, basisCurve, realSeq);

				return true;
			}
			gp_Ax3 t3X(gp_Pnt(realSeq->Value(1),realSeq->Value(2),realSeq->Value(3)), gp::DY());
			a3X = t3X;
			majrad = realSeq->Value(4);
			minrad = realSeq->Value(5);
		}
		else
		{
			::cout << "Error in Torus Surface Card Parameter!" << endl;
			return false;
		}
	}
	else if ( tmpStr.Search(" TZ") > 0 )
	{
		TCollection_AsciiString sEnd = tmpStr.Split(tmpStr.Search("TZ")+2);
		sEnd.LeftAdjust();
		sEnd.RightAdjust();
		Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;
		while(true)
		{
			TCollection_AsciiString sEnd2;
			if(sEnd.Search(" ") > 1)
			sEnd2 = sEnd.Split(sEnd.Search(" "));
			else
			{
				realSeq->Append(sEnd.RealValue());
				break;
			}
			if(sEnd.IsRealValue()) realSeq->Append(sEnd.RealValue());
			sEnd.Clear();
			sEnd = sEnd2;
			sEnd.LeftAdjust();
			sEnd.RightAdjust();
		}
		if(realSeq->Length() == 6 && realSeq->Value(4) > 0 && realSeq->Value(5) > 0 && realSeq->Value(6) > 0)
		{
			if (realSeq->Value(5) != realSeq->Value(6))
			{
			//	cout << "ELLIPTICAL TORUS - Z\n";

				gp_Pnt ellOri(realSeq->Value(1),realSeq->Value(2)+realSeq->Value(4),realSeq->Value(3));
				gp_Pnt revOri(realSeq->Value(1), realSeq->Value(2), realSeq->Value(3));
				gp_Dir ellipseNormal(1,0,0);
				gp_Dir ellipseMajAxisDir;

				gp_Ax2 ax2;
				Standard_Real rMaj, rMin;
				if(realSeq->Value(5) >= realSeq->Value(6)) // orient ax2 of ellipse according to input
				{
					ellipseMajAxisDir = gp_Dir(0,0,1);
					rMin = realSeq->Value(6);
					rMaj = realSeq->Value(5);
				}
				else
				{
					ellipseMajAxisDir = gp_Dir(0,1,0);
					rMin = realSeq->Value(5);
					rMaj = realSeq->Value(6);
				}

				ax2 = gp_Ax2(ellOri, ellipseNormal, ellipseMajAxisDir);

				Handle(Geom_Ellipse) anEllipse = new Geom_Ellipse(ax2, rMaj, rMin);
				//make surface of revolution
				gp_Dir rotAxis(0,0,1);
				gp_Ax1 revAx(revOri, rotAxis);
				Handle(Geom_Curve) basisCurve = Handle(Geom_Curve)::DownCast(anEllipse);

				mySurface = new McCadCSGGeom_SurfaceOfRevolution(revAx, basisCurve, realSeq);

				return true;

			}
			gp_Ax3 t3X(gp_Pnt(realSeq->Value(1),realSeq->Value(2),realSeq->Value(3)), gp::DZ());
			a3X = t3X;
			majrad = realSeq->Value(4);
			minrad = realSeq->Value(5);
		}
		else
		{
			::cout << "Error in Torus Surface Card Parameter!" << endl;
			return false;
		}
	}
	else
	{
		::cout << "Error in Sphere Surface Card Parameter!" << endl;
		return false;
	}

	mySurface = new McCadCSGGeom_Torus(a3X,majrad, minrad);

	return true;
}

Standard_Boolean McCadMcRead_SurfaceFactory::MakeGQ(const TCollection_AsciiString& sDesc)
{
	// cylinder
	//cout << sDesc.ToCString() << endl;
	//cout << "Can not make a GQ yet!!" << endl;

	//extract values
	TCollection_AsciiString coeff = sDesc;
	coeff.LeftAdjust();
	coeff.Remove(1, coeff.Search("GQ")+2);
	coeff.LeftAdjust();
	TColStd_ListOfReal coeffList;

	while(true)
	{
		if(coeff.Search(" ")>0)
		{
			TCollection_AsciiString aVal = coeff;
			coeff = aVal.Split(aVal.Search(" "));

			if(!aVal.IsRealValue()) // skip all spaces
				continue;

			coeffList.Append(aVal.RealValue());
		}
		else
		{
			coeffList.Append(coeff.RealValue());
			break;
		}
	}

	if(coeffList.Extent()!=10)
	{
		cout << "GQ :: has not 10 coefficients!!!\n";
		return Standard_False;
	}

	Standard_Real a,b,c,d,e,f,g,h,j,k;
	a = coeffList.First();	coeffList.RemoveFirst();
	b = coeffList.First();	coeffList.RemoveFirst();
	c = coeffList.First();	coeffList.RemoveFirst();
	d = coeffList.First();	coeffList.RemoveFirst();
	e = coeffList.First();	coeffList.RemoveFirst();
	f = coeffList.First();	coeffList.RemoveFirst();
	g = coeffList.First();	coeffList.RemoveFirst();
	h = coeffList.First();	coeffList.RemoveFirst();
	j = coeffList.First();	coeffList.RemoveFirst();
	k = coeffList.First();	coeffList.RemoveFirst();



        // cout << "\n_#_McCadMcRead_SurfaceFactory.cxx :: WARNING !!!  Generic Quadrics not yet implemented !!! \n\n";

	mySurface = new McCadCSGGeom_GQ(a,b,c,d,e,f,g,h,j,k);

	return Standard_True;
}


Standard_Boolean McCadMcRead_SurfaceFactory::MakeAxissymmetricSurfaceByPoints(const TCollection_AsciiString& sDesc)
{
	TCollection_AsciiString tmpStr = sDesc;


//	cout << "Axissymmetric by points...\n\n";

	gp_Dir axeOfSym;
	gp_Dir X=gp::DX(), Y=gp::DY(), Z=gp::DZ();

	if(sDesc.Search(" X") > 0)
	{
		axeOfSym = gp::DX();
		tmpStr.Remove(1,sDesc.Search(" X")+1);
	}
	else if(sDesc.Search(" Y") > 0)
	{
		axeOfSym = gp::DY();
		tmpStr.Remove(1,sDesc.Search(" Y")+1);
	}
	else if(sDesc.Search(" Z") > 0)
	{
		axeOfSym = gp::DZ();
		tmpStr.Remove(1,sDesc.Search(" Z")+1);
	}
	else
	{
		cout << "_#_McCadMcRead_SurfaceFactory.cxx :: Not an axissymmetric surface defined by points!!!" << endl;
		return Standard_False;
	}

	// retrieve Points
	tmpStr.LeftAdjust();
	tmpStr.RightAdjust();

	Handle(TColStd_HSequenceOfReal) pntSeq = new TColStd_HSequenceOfReal;

	while(true)
	{
		if(tmpStr.Search(" ") < 0) // final point
		{
			if(tmpStr.IsRealValue())
				pntSeq->Append(tmpStr.RealValue());
			else
				cout << "_#_ McCadMcRead_SurfaceFactory.cxx :: Point is not a real value!\n";

			break;
		}

		TCollection_AsciiString pntStr = tmpStr.Split(tmpStr.Search(" ")-1);

		if(tmpStr.IsRealValue())
			pntSeq->Append(tmpStr.RealValue());
		else
			cout << "_#_ McCadMcRead_SurfaceFactory.cxx :: Point is not a real value!\n";

		tmpStr = pntStr;
		tmpStr.LeftAdjust();
	}

	Standard_Integer numPnts = pntSeq->Length();

	//Create Surfaces
	switch(numPnts)
	{
		case 2:
		{ // a Plane
			return PointPlane(axeOfSym, pntSeq);
		}
		break;
		case 4: // a Plane, Cylinder or Cone
		{
		//	cout << "FOUR PARAMETERS\n";
			if(pntSeq->Value(1) == pntSeq->Value(3))//we assume a plane
			{
				return PointPlane(axeOfSym, pntSeq);
			}
			else if(pntSeq->Value(2) == pntSeq->Value(4)) //cylinder
			{
				return PointCylinder(axeOfSym, pntSeq);
			}
			else //Cone
			{
				return PointCone(axeOfSym, pntSeq);
			}
		}
		break;
		case 6: // a Plane, Cylinder, Cone, Sphere or Special Quadric
		{
			if(pntSeq->Value(1) == pntSeq->Value(3) && pntSeq->Value(1) == pntSeq->Value(5))//we assume a plane
			{
				return PointPlane(axeOfSym, pntSeq);
			}
			else if(pntSeq->Value(2) == pntSeq->Value(4) && pntSeq->Value(2) == pntSeq->Value(6)) //cylinder
			{
				return PointCylinder(axeOfSym, pntSeq);
			}
			else //Cone
			{
				cout << "!!!!!!!! WARNING : Axissymmetric Surface with six pairs of points defined. SQ and spheres are not yet implemented.\n";
				cout << "                   assuming a cone\n";

				//TODO: implement Sx, with x in {Q,X,Y,Z,O,\X,\Y,\Z}

				return PointCone(axeOfSym, pntSeq);
			}
		}
		break;
		default:
			cout << "Number of point pairs not valid!!!\n";
			return Standard_False;
	}


	return Standard_False;
}


Standard_Boolean McCadMcRead_SurfaceFactory::MakeBOX(const TCollection_AsciiString& sDesc)
{
	mySurface = new McCadCSGGeom_Plane(0,0,1,0); // fake surface
	mySurface->SetIsMacroBody();

	// resolve box planes

	TCollection_AsciiString tmpStr = sDesc;

	if(tmpStr.Search(" BOX") < 0)
		return Standard_False;

	tmpStr.Remove(1, tmpStr.Search(" BOX")+4);
	Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;

	while(true)
	{
		tmpStr.LeftAdjust();
		tmpStr.RightAdjust();

		if(tmpStr.Search(" ") > 0)
		{
			TCollection_AsciiString beg = tmpStr;
			tmpStr = beg.Split(beg.Search(" "));
			beg.LeftAdjust();
			beg.RightAdjust();
			if(beg.IsRealValue())
				realSeq->Append(beg.RealValue());
			else
			{
				cout << "Real Value expected!\n";
				return Standard_False;
			}
		}
		else
		{
			if(tmpStr.IsRealValue())
				realSeq->Append(tmpStr.RealValue());
			else
			{
				cout << "Real Value expected!\n";
				return Standard_False;
			}

			break;
		}
	}

	if(realSeq->Length() != 12)
	{
		cout << "Wrong number of entries in box definition : " << realSeq->Length() << endl;
		cout << sDesc.ToCString() << endl;
		return Standard_False;
	}

	gp_Pnt ori(realSeq->Value(1), realSeq->Value(2), realSeq->Value(3));
	gp_Vec a1(realSeq->Value(4), realSeq->Value(5), realSeq->Value(6));
	gp_Vec a2(realSeq->Value(7), realSeq->Value(8), realSeq->Value(9));
	gp_Vec a3(realSeq->Value(10), realSeq->Value(11), realSeq->Value(12));

	//////////////////////////////////////
	// Build new Surfaces

	gp_Pln pl1(ori, a1.Reversed());
	gp_Pln pl2(ori, a2.Reversed());
	gp_Pln pl3(ori, a3.Reversed());
	gp_Pnt pnt4(ori.X()+a3.X(), ori.Y()+a3.Y(), ori.Z()+a3.Z());
	gp_Pln pl4(pnt4, a3);
	gp_Pnt pnt5(ori.X()+a2.X(), ori.Y()+a2.Y(), ori.Z()+a2.Z());
	gp_Pln pl5(pnt5, a2);
	gp_Pnt pnt6(ori.X()+a1.X(), ori.Y()+a1.Y(), ori.Z()+a1.Z());
	gp_Pln pl6(pnt6, a1);

	Standard_Real a,b,c,d;
	pl1.Coefficients(a,b,c,d);
	Handle(McCadCSGGeom_Surface) appSurf1 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf1);
	pl2.Coefficients(a,b,c,d);
	Handle(McCadCSGGeom_Surface) appSurf2 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf2);
	pl3.Coefficients(a,b,c,d);
	Handle(McCadCSGGeom_Surface) appSurf3 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf3);
	pl4.Coefficients(a,b,c,d);
	Handle(McCadCSGGeom_Surface) appSurf4 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf4);
	pl5.Coefficients(a,b,c,d);
	Handle(McCadCSGGeom_Surface) appSurf5 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf5);
	pl6.Coefficients(a,b,c,d);
	Handle(McCadCSGGeom_Surface) appSurf6 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf6);

	return Standard_True;
}

Standard_Boolean McCadMcRead_SurfaceFactory::MakeRPP(const TCollection_AsciiString& sDesc)
{
	mySurface = new McCadCSGGeom_Plane(0,0,1,0); // fake surface
	mySurface->SetIsMacroBody();
	return Standard_False;
}

Standard_Boolean McCadMcRead_SurfaceFactory::MakeRCC(const TCollection_AsciiString& sDesc)
{
	mySurface = new McCadCSGGeom_Plane(0,0,1,0); // fake surface
	mySurface->SetIsMacroBody();

	gp_Pnt basePnt(0.0,0.0,0.0);
	gp_Dir theDir(0,0,1);
	gp_Ax1 theAxis;
	Standard_Real radius(0.0);

	TCollection_AsciiString tmpStr = sDesc;
	TCollection_AsciiString rest = tmpStr.Split(sDesc.Search("RCC") + 3);
	rest.LeftAdjust();

	//extract basis
	Handle(TColStd_HSequenceOfReal) realValues = new TColStd_HSequenceOfReal;
	while(true){
		if(rest.Search(" ") < 0) {
			rest.RightAdjust();
			if(!rest.IsRealValue())	{
				cout << "RCC : Real value expected\n";
				return Standard_False;
			}

			realValues->Append(rest.RealValue());
			break;
		}

		tmpStr = rest.Split(rest.Search(" "));
		rest.RightAdjust();

		if(!rest.IsRealValue()){
			cout << "RCC : Real Value expected!\n";
			return Standard_False;
		}

		realValues->Append(rest.RealValue());
		rest = tmpStr;
		rest.LeftAdjust();
		rest.RightAdjust();
	}

	if(realValues->Length() < 7){
		cout << "RCC : 7 real values expected; found : " << realValues->Length() << endl;
		return Standard_False;
	}

	basePnt.SetCoord(realValues->Value(1), realValues->Value(2), realValues->Value(3));


	radius = realValues->Value(7);

	// make surfaces
	gp_Pln basePlane;
	basePlane.SetLocation(basePnt);
	basePlane.SetAxis(theAxis);

/*	Standard_Real a,b,c,d;
	pl1.Coefficients(a,b,c,d);
	Handle(McCadCSGGeom_Surface) appSurf1 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf1);*/

// TODO !!!

	return Standard_False;
}

Standard_Boolean McCadMcRead_SurfaceFactory::MakeHEX(const TCollection_AsciiString& sDesc)
{
	mySurface = new McCadCSGGeom_Plane(0,0,1,0); // fake surface
	mySurface->SetIsMacroBody();

	// resolve hex planes

	TCollection_AsciiString tmpStr = sDesc;
	if(tmpStr.Search(" RHP") > 0)
	{
		TCollection_AsciiString grbg = tmpStr.Split(tmpStr.Search(" RHP") + 4);
		tmpStr = " HEX ";
		tmpStr += grbg;
	}

	if(tmpStr.Search(" HEX") < 0)
		return Standard_False;

	tmpStr.Remove(1, tmpStr.Search(" HEX")+4);
	Handle(TColStd_HSequenceOfReal) realSeq = new TColStd_HSequenceOfReal;

	while(true)
	{
		tmpStr.LeftAdjust();
		tmpStr.RightAdjust();

		if(tmpStr.Search(" ") > 0)
		{
			TCollection_AsciiString beg = tmpStr;
			tmpStr = beg.Split(beg.Search(" "));
			beg.LeftAdjust();
			beg.RightAdjust();
			if(beg.IsRealValue())
				realSeq->Append(beg.RealValue());
			else
			{
				cout << "Real Value expected!\n";
				return Standard_False;
			}
		}
		else
		{
			if(tmpStr.IsRealValue())
				realSeq->Append(tmpStr.RealValue());
			else
			{
				cout << "Real Value expected!\n";
				return Standard_False;
			}

			break;
		}
	}

	if(realSeq->Length() < 9)
	{
		cout << "Expected at least 9 Parameters!!\n\n";
		return(Standard_False);
	}

	// base
	gp_Pnt ori(realSeq->Value(1), realSeq->Value(2), realSeq->Value(3)); // bottom center
	gp_Vec dir(realSeq->Value(4), realSeq->Value(5), realSeq->Value(6));
	gp_Pln basePln(ori, dir.Reversed());
	Standard_Real a,b,c,d;
	basePln.Coefficients(a,b,c,d);
	Handle(McCadCSGGeom_Surface) appSurf1 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf1);

	// top
	gp_Pnt topCenter(ori.X()+dir.X(), ori.Y()+dir.Y(), ori.Z()+dir.Z());
	gp_Pln topPln(topCenter, dir);
	topPln.Coefficients(a,b,c,d);
	Handle(McCadCSGGeom_Surface) appSurf2 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf2);

	// first facet
	gp_Pnt firstFacetCenter(ori.X()+realSeq->Value(7), ori.Y()+realSeq->Value(8), ori.Z()+realSeq->Value(9));
	gp_Vec firstFacetDir(realSeq->Value(7), realSeq->Value(8), realSeq->Value(9));
	gp_Pln firstFacetPln(firstFacetCenter, firstFacetDir);
	firstFacetPln.Coefficients(a, b, c, d);
	Handle(McCadCSGGeom_Surface) appSurf3 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf3);

	// first facet opponent
	gp_Pnt ffOCntr(ori.X()-realSeq->Value(7), ori.Y()-realSeq->Value(8), ori.Z()-realSeq->Value(9));
	gp_Pln ffOPln(ffOCntr, firstFacetDir.Reversed());
	ffOPln.Coefficients(a, b, c, d);
	Handle(McCadCSGGeom_Surface) appSurf4 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf4);

	// second facet
	gp_Vec sfDir;
	if(realSeq->Length() < 12) // calculate other facets
	{
		gp_Ax1 rotAxis(ori, dir);
		gp_Pnt fcMid = firstFacetCenter.Rotated(rotAxis, PI/3.0);
		sfDir = gp_Vec(ori, fcMid);
	}
	else // read dir
	{
		sfDir = gp_Vec(realSeq->Value(10), realSeq->Value(11), realSeq->Value(12));
	}
	gp_Pnt sfCntr(ori.X()+sfDir.X(), ori.Y()+sfDir.Y(), ori.Z()+sfDir.Z());
	gp_Pln sfPln(sfCntr, sfDir);
	sfPln.Coefficients(a, b, c, d);
	Handle(McCadCSGGeom_Surface) appSurf5 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf5);

	// second facet opponent
	gp_Pnt sfOCntr(ori.X()-sfDir.X(), ori.Y()-sfDir.Y(), ori.Z()-sfDir.Z());
	gp_Pln sfOPln(sfOCntr, sfDir.Reversed());
	sfOPln.Coefficients(a, b, c, d);
	Handle(McCadCSGGeom_Surface) appSurf6 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf6);

	// third facet
	gp_Vec tfDir;
	if(realSeq->Length() < 15) // calculate other facets
	{
		gp_Ax1 rotAxis(ori, dir);
		gp_Pnt fcMid = firstFacetCenter.Rotated(rotAxis, -1*PI/3.0);
		tfDir = gp_Vec(ori, fcMid);
	}
	else // read dir
	{
		tfDir = gp_Vec(realSeq->Value(13), realSeq->Value(14), realSeq->Value(15));
	}
	gp_Pnt tfCntr(ori.X()+tfDir.X(), ori.Y()+tfDir.Y(), ori.Z()+tfDir.Z());
	gp_Pln tfPln(tfCntr, tfDir);
	tfPln.Coefficients(a, b, c, d);
	Handle(McCadCSGGeom_Surface) appSurf7 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf7);

	// third facet opponent
	gp_Pnt tfOCntr(ori.X()-tfDir.X(), ori.Y()-tfDir.Y(), ori.Z()-tfDir.Z());
	gp_Pln tfOPln(tfOCntr, tfDir.Reversed());
	tfOPln.Coefficients(a, b, c, d);
	Handle(McCadCSGGeom_Surface) appSurf8 = new McCadCSGGeom_Plane(a,b,c,d);
	myMacroBodySurfaces->Append(appSurf8);

	return Standard_True;
}

Standard_Boolean McCadMcRead_SurfaceFactory::MakeREC(const TCollection_AsciiString& sDesc)
{
	mySurface = new McCadCSGGeom_Plane(0,0,1,0); // fake surface
	mySurface->SetIsMacroBody();
	return Standard_False;
}

Standard_Boolean McCadMcRead_SurfaceFactory::MakeTRC(const TCollection_AsciiString& sDesc)
{
	mySurface = new McCadCSGGeom_Plane(0,0,1,0); // fake surface
	mySurface->SetIsMacroBody();
	return Standard_False;
}

Standard_Boolean McCadMcRead_SurfaceFactory::MakeELL(const TCollection_AsciiString& sDesc)
{
	mySurface = new McCadCSGGeom_Plane(0,0,1,0); // fake surface
	mySurface->SetIsMacroBody();
	return Standard_False;
}

Standard_Boolean McCadMcRead_SurfaceFactory::MakeWED(const TCollection_AsciiString& sDesc)
{
	mySurface = new McCadCSGGeom_Plane(0,0,1,0); // fake surface
	mySurface->SetIsMacroBody();
	return Standard_False;
}

Standard_Boolean McCadMcRead_SurfaceFactory::MakeARB(const TCollection_AsciiString& sDesc)
{
	mySurface = new McCadCSGGeom_Plane(0,0,1,0); // fake surface
	mySurface->SetIsMacroBody();
	return Standard_False;
}


Standard_Boolean McCadMcRead_SurfaceFactory::PointPlane(const gp_Dir& dir, Handle_TColStd_HSequenceOfReal& thePntSeq)
{
	gp_Dir X=gp::DX(), Y=gp::DY(), Z=gp::DZ();
	TCollection_AsciiString planeStr;

	if(dir.IsEqual(X, 1e-7))
		planeStr = TCollection_AsciiString("0 PX ");
	if(dir.IsEqual(Y, 1e-7))
		planeStr = TCollection_AsciiString("0 PY ");
	if(dir.IsEqual(Z, 1e-7))
		planeStr = TCollection_AsciiString("0 PZ ");

	planeStr +=  TCollection_AsciiString(thePntSeq->Value(1));

	return MakePlane(planeStr);
}


Standard_Boolean McCadMcRead_SurfaceFactory::PointCylinder(const gp_Dir& dir, Handle_TColStd_HSequenceOfReal& thePntSeq)
{
	gp_Dir X=gp::DX(), Y=gp::DY(), Z=gp::DZ();
	TCollection_AsciiString cStr;

	if(dir.IsEqual(X, 1e-7))
		cStr = TCollection_AsciiString("0 CX ");
	if(dir.IsEqual(Y, 1e-7))
		cStr = TCollection_AsciiString("0 CY ");
	if(dir.IsEqual(Z, 1e-7))
		cStr = TCollection_AsciiString("0 CZ ");

	cStr +=  TCollection_AsciiString(thePntSeq->Value(2)); // append the radius

	return MakeCylinder(cStr);
}


Standard_Boolean McCadMcRead_SurfaceFactory::PointCone(const gp_Dir& dir, Handle_TColStd_HSequenceOfReal& thePntSeq)
{
	//cout << "POINT CONE\n";
	gp_Dir X=gp::DX(), Y=gp::DY(), Z=gp::DZ();
	TCollection_AsciiString cStr;

	if(dir.IsEqual(X, 1e-7))
		cStr = TCollection_AsciiString("0 KX ");
	if(dir.IsEqual(Y, 1e-7))
		cStr = TCollection_AsciiString("0 KY ");
	if(dir.IsEqual(Z, 1e-7))
		cStr = TCollection_AsciiString("0 KZ ");


	//Standard_Integer sheet(-1);
	Standard_Real r2 = thePntSeq->Value(2);
	Standard_Real r1,x1,x2;
	if(r2 < thePntSeq->Value(4))
	{
		x2 = thePntSeq->Value(3);
		r2 = thePntSeq->Value(4);
		x1 = thePntSeq->Value(1);
		r1 = thePntSeq->Value(2);
	}
	else
	{
		x2 = thePntSeq->Value(1);
		x1 = thePntSeq->Value(3);
		r1 = thePntSeq->Value(4);
	}

	/*if(x2<x1)
		sheet = 1;*/
	Standard_Real m = (x2-x1)/(r2-r1);

	Standard_Real apex = x2 - r2 * m;
	Standard_Real t = 1/m;

	cStr += TCollection_AsciiString(apex);
	cStr += " ";
	cStr += TCollection_AsciiString(t*t);

//	cout << "Cone : " << cStr.ToCString() << endl;

	return MakeCone(cStr);
}


Handle(McCadCSGGeom_Surface) McCadMcRead_SurfaceFactory::GetSurface() const
{
	return mySurface;
}

Handle(McCadCSGGeom_HSequenceOfSurface) McCadMcRead_SurfaceFactory::GetMacroBodySurfaces()
{
	return myMacroBodySurfaces;
}

void McCadMcRead_SurfaceFactory::InitMacroBodySurfaces()
{
	myMacroBodySurfaces = new McCadCSGGeom_HSequenceOfSurface;
	myIsMacroBody = Standard_True;
}
