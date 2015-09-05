#include <McCadCSGGeom_Transformation.ixx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax3.hxx>

McCadCSGGeom_Transformation::McCadCSGGeom_Transformation()
{
  myID = 0;
  myTrsf.SetValues(1,0,0,0,0,1,0,0,0,0,1,0,0,0);
}

McCadCSGGeom_Transformation::McCadCSGGeom_Transformation(const Standard_Real a14,const Standard_Real a24,const Standard_Real a34,
                                                         const Standard_Real a11,const Standard_Real a12,const Standard_Real a13,
                                                         const Standard_Real a21,const Standard_Real a22,const Standard_Real a23,
                                                         const Standard_Real a31,const Standard_Real a32,const Standard_Real a33)
{

  // myTrsf.SetValues(1,0,0,0,0,1,0,0,0,0,1,0,1e-5,1e-5);

  //  myTrsf.SetValues(a11,a12,a13,a14,a21,a22,a23,a24, a31,a32,a33,a34,1e-5,1e-5);
  myTrsf.SetValues(a11,a21,a31,a14,a12,a22,a32,a24,a13,a23,a33,a34,1e-5,1e-5);

  // gp_Ax3  stdAxis;
  // gp_Ax3  theAxis  = stdAxis.Transformed(myTrsf);
  //gp_Trsf theTrsf;
  //theTrsf.SetTransformation(theAxis,stdAxis);
  //myTrsf = theTrsf;
}
void McCadCSGGeom_Transformation::SetValues(const Standard_Real a14,const Standard_Real a24,const Standard_Real a34,
                                            const Standard_Real a11,const Standard_Real a12,const Standard_Real a13,
                                            const Standard_Real a21,const Standard_Real a22,const Standard_Real a23,
                                            const Standard_Real a31,const Standard_Real a32,const Standard_Real a33)
{

  myTrsf.SetValues(a11,a21,a31,a14,a12,a22,a32,a24,a13,a23,a33,a34,1e-5,1e-5);
}

void McCadCSGGeom_Transformation::SetVValues(const Standard_Real a14,const Standard_Real a24,const Standard_Real a34)
{
  gp_Vec V(a14,a24,a34);
  myTrsf.SetTranslation(V);

}

void McCadCSGGeom_Transformation::SetIdNum(const Standard_Integer theId)
{
  myID = theId;
}

Standard_Integer McCadCSGGeom_Transformation::GetIdNum() const
{
  return myID;
}

void McCadCSGGeom_Transformation::SetTrsf(const gp_Trsf& theTrsf)
{
  myTrsf = theTrsf;
}

gp_Trsf McCadCSGGeom_Transformation::GetTrsf() const
{
  return myTrsf;
}

