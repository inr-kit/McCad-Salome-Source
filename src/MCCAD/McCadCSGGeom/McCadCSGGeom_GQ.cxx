#include <McCadCSGGeom_GQ.ixx>
#include <math_Matrix.hxx>
#include <iomanip>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pln.hxx>
#include <Geom_Plane.hxx>

McCadCSGGeom_GQ::McCadCSGGeom_GQ()
{
	//empty ctor
	MakeSurface(); // dummy surface
	SetType(McCadCSGGeom_Quadric);
}

McCadCSGGeom_GQ::McCadCSGGeom_GQ(const Standard_Real A,const Standard_Real B,const Standard_Real C,const Standard_Real D,const Standard_Real E,const Standard_Real F,const Standard_Real G,const Standard_Real H,const Standard_Real I,const Standard_Real J)
{
	myA = A;
	myB = B;
	myC = C;
	myD = D;
	myE = E;
	myF = F;
	myG = G;
	myH = H;
	myI = I;
	myJ = J;
	SetType(McCadCSGGeom_Quadric);

	MakeSurface(); //dummy surface
}

void McCadCSGGeom_GQ::Coefficients(Standard_Real& A,Standard_Real& B,Standard_Real& C,Standard_Real& D,Standard_Real& E,Standard_Real& F,Standard_Real& G,Standard_Real& H,Standard_Real& I,Standard_Real& J) const
{
	A = myA;
	B = myB;
	C = myC;
	D = myD;
	E = myE;
	F = myF;
	G = myG;
	H = myH;
	I = myI;
	J = myJ;
}

void McCadCSGGeom_GQ::Print(Standard_OStream& theStream) const
{
	Standard_Real s=Scale();//0.1; //This is to make mcnp happy with its cm
	Standard_Real t=s*s;

	//No transformation cards needed
	theStream << setw(5) << myNumber << " ";
	theStream << "GQ    " << setw(10) << myA/t << " " << myB/t << " " << myC/t << " " << myD/t << " " << myE/t << endl;
	theStream << "      " << setw(10) << myF/t << " " << myG/s << " " << myH/s << " " << myI/s << " " << myJ << endl;
}

Standard_Real McCadCSGGeom_GQ::Evaluate(const gp_Pnt& thePoint) const
{
	Standard_Real x = thePoint.X();
	Standard_Real y = thePoint.Y();
	Standard_Real z = thePoint.Z();

	return (myA*x*x + myB*y*y + myC*z*z + myD*x*y + myE*y*z + myF*z*x + myG*x + myH*y + myI*z + myJ);
}

gp_Pnt McCadCSGGeom_GQ::Value(const Standard_Real U,const Standard_Real V) const
{
	return mySurface->Value(U, V);
}

void McCadCSGGeom_GQ::SetCasSurf(const GeomAdaptor_Surface& theSurf)
{
	mySurface = theSurf.Surface();
}

Handle(Geom_Surface) McCadCSGGeom_GQ::CasSurf() const
{
	return mySurface;
}

Standard_Boolean McCadCSGGeom_GQ::IsEqual(const Handle(McCadCSGGeom_Surface)& another) const
{//TODO
	/*
	 * This function is not yet implemented. If the same surface occurce multiple times, we will not know!!! But MCNP will
	 * sort them out so it's not that bad right now!
	 * */

	if(another->GetType()!=myType)
		return Standard_False;

	return Standard_False;
}


Standard_Boolean McCadCSGGeom_GQ::IsGeomEqual(const Handle(McCadCSGGeom_Surface)& another) const
{//TODO
	return IsEqual(another);
}


Standard_Integer McCadCSGGeom_GQ::ComputeMatrixRank(gp_Mat m)
{
	Standard_Integer i(1), j(1), s(1), p(0), rank(3);
	Standard_Real f(0.0), max(0.0);
	Standard_Boolean done(Standard_True);


	do
	{
		/*cout << "m[ {" << m(1,1) << ", " << m(1,2) << ", " << m(1,3) << " } \n   {" <<
					      m(2,1) << ", " << m(2,2) << ", " << m(2,3) << " } \n   {" <<
					      m(3,1) << ", " << m(3,2) << ", " << m(3,3) << " } ]" << endl;*/

		max = Abs(m(s,s));
		p = s;

		for (i = s+1; i <= 3; i++)
		{
			if( Abs(m(i,s)) > max)
			{
				max = Abs(m(i,s)) ;
				p = i;
			}
		}

		if (max < 1e-5)
		{
			done = Standard_False;
			break;
		}

		if (p != s)  // pivoting
		{
			Standard_Real h;
			for (j = s ; j <= 3; j++)
			{
				h = m(s,j);
				m(s,j) = m(p,j);
				m(p,j) = h;
			}
		}

		// gauss elimination
		for (i = s+1; i <= 3; i++ )
		{
			f = -(m(i,s)/m(s,s));
			m(i,s) = 0.0;
			for (j = s+1; j <= 3 ; j++)
				m(i,j) += f*m(s,j);
		}
		s++;
	} while ( s <= 3 ) ;

	if(Abs(m(3,3)) < 1e-5)
		rank = 2;
	if(Abs(m(2,2)) < 1e-5)
		rank = 1;
	if(!done)
		rank = s-1;

	return rank;
}


void McCadCSGGeom_GQ::MakeSurface()
{ // make cassurface in the given boundary
	//gather points

	mySurface = NULL;

	/*gp_Mat mat(myA   , myD/2., myF/2.,
			   myD/2., myB   , myE/2.,
			   myF/2., myE/2., myC    );

	Standard_Integer rank = ComputeMatrixRank(mat);

	if(rank == 0) // plane
	{
		Standard_Real A(myG),B(myH),C(myI),D(myJ);

		gp_Pln pln(A,B,C,D);
		Handle(Geom_Plane) geomPln = new Geom_Plane(pln);
		mySurface = geomPln;
	}
	else if(rank == 1) // paraboloid, plane,
	{

	}
	else if(rank == 2)
	{

	}
	else if(rank == 3)
	{

	}*/

/*	Standard_Integer xRng(0), yRng(0);

	xRng = (double(xMax-xMin)/100. <= 0.1) ? 100 : 1000;
	yRng = (double(yMax-yMin)/100. <= 0.1) ? 100 : 1000;

	TColgp_Array2OfPnt pnt2Array(1,xRng,1,yRng);
	pnt2Array.Init(gp_Pnt(-999999,-999999,-999999));

	Standard_Real dX = double(xMax-xMin)/double(xRng);
	Standard_Real dY = double(yMax-yMin)/double(xRng);
	Standard_Real x(xMin-dX),y,z;
	Standard_Integer xCnt(0), yCnt(0), xHit(0), yHit(0), pntCnt(0);
	gp_Pnt tstPnt;
	Standard_Boolean firstHit = Standard_True;

	for(int i=1; i<=xRng; i++)
	{
		x += dX;
		y = yMin-dY;
		for(int j=1; j<=yRng; j++)
		{
			y+=dY;

			Standard_Real Q = 0.5 * (myE*y + myF*x + myI) / myC;
			Standard_Real T = (myA*x*x + myB*y*y + myD*x*y + myG*x + myH*y + myJ) / myC;

			if((Q*Q - T) > 0)
			{
				if(firstHit)
				{
					xHit = i;
					firstHit = Standard_False;
				}
				xCnt = (i > xCnt) ? i : xCnt;
				yCnt = (j > yCnt) ? j : yCnt;
				pntCnt++;

				z = -Q + Sqrt(Q*Q - T);
				tstPnt = gp_Pnt(x,y,z);
				if(Abs(Evaluate(tstPnt))<1e-7)
					pnt2Array.SetValue(i,j,tstPnt);
				else
				{
					z = Q - Sqrt(Q*Q - T);
					tstPnt.SetZ(z);
					if(Abs(Evaluate(tstPnt))<1e-7)
						pnt2Array.SetValue(i,j,tstPnt);
					else
						pntCnt--;
				}
				pnt2Array.SetValue(i,j,tstPnt);
				//cout << "(i , j) = (" << i << " , " << j << ")\t\t(x, y, z) = (" << tstPnt.X() << ", " << tstPnt.Y() << ", " << tstPnt.Z() << ") \n";
			}
		}
	}

	//cout << "\n\n==========================================================================\n\n";

	tstPnt = gp_Pnt(-999999,-999999,-999999);
	Standard_Integer curY(0), maxY(0), minY(999999), curX(0), minX(0), maxX(0);

	for(int i=xHit; i<=xCnt; i++)
	{
		curY=0;
		firstHit = Standard_True;
		for(int j=1; j<= yRng; j++)
		{
			if(!pnt2Array(i,j).IsEqual(tstPnt,1e-7))
			{
				if(firstHit)
				{
					minY = ( j < minY ) ? j : minY;
					firstHit = Standard_False;
				}
				curY++;
				if(j=yRng)
					maxY = j;
				//cout << "(i,j) = " << i << "  " << j << endl;
			}
			else
			{
				if(curY>0)
				{
					maxY = j;
					j = yRng;
					continue;
				}
			}
		}
		//cout << "YVals for x: " << i << "\t\t" << minY << " " << maxY << endl;
	}

	//cout << "minY, maxY : " << minY << " , " << maxY << endl;

	firstHit = Standard_True;

	for(int i=xHit; i<=xCnt; i++)
	{
		if(firstHit)
		{
			if(!pnt2Array(i,minY).IsEqual(tstPnt,1e-7) && !pnt2Array(i,maxY).IsEqual(tstPnt,1e-7))
			{
				firstHit = Standard_False;
				minX = (i>minX) ? i : minX;
			}
		}
		else
		{
			if(pnt2Array(i,minY).IsEqual(tstPnt,1e-7) || pnt2Array(i,maxY).IsEqual(tstPnt,1e-7))
			{
				maxX = (i<maxX) ? i : maxX;
				firstHit = Standard_True;
			}
			else
				if(i==xCnt)
					maxX=xCnt;
		}
	}

	//cout << maxX-minX << "  "  << maxY - minY+1 << endl;

	TColStd_Array2OfReal finalPnts(1,maxX-minX+1, 1, maxY-minY+1);


	//cout << "VARS : " << minX << " " << maxX << "  " << minY << " " << maxY << endl;

	for(int i=1; i<=maxX-minX+1; i++)
		for(int j=1; j<=maxY-minY+1; j++)
		{
			finalPnts.SetValue(i,j,pnt2Array(minX-1+i,minY-1+j).Z());
		//	cout << i << " " << j << " \t : (" << pnt2Array(minX-1+i,minY-1+j).X() << " , " << pnt2Array(minX-1+i,minY-1+j).Y() << " , " << finalPnts(i,j) << ")" << endl;
		}

	//cout << xMin+xHit*dX << "  " << dX << "  "  << yMin+yHit*dY << "  " << dY << "  \n\n";

	//Use final points to create the bspline surface
	GeomAPI_PointsToBSplineSurface bSSMaker(finalPnts, xMin+xHit*dX, dX, yMin+yHit*dY, dY);*/

	//mySurface = bSSMaker.Surface();
}
