#include <McCadCSGAdapt_SolidAnalyser.ixx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>
#include <ShapeFix_Solid.hxx>

McCadCSGAdapt_SolidAnalyser::McCadCSGAdapt_SolidAnalyser()
{
    myIsFixed = Standard_False;
}

McCadCSGAdapt_SolidAnalyser::McCadCSGAdapt_SolidAnalyser(const TopoDS_Solid& theSolid)
{
	Init(theSolid);
}

void McCadCSGAdapt_SolidAnalyser::Init(const TopoDS_Solid& theSolid)
{
	myFirstSolid = theSolid;
        myIsFixed = Standard_False;
	myResultSolid = new TopTools_HSequenceOfShape;
}

TopoDS_Solid McCadCSGAdapt_SolidAnalyser::FixedSolid()
{
        if (!IsFixed())
		Fix();
	return myFirstSolid;
}

void McCadCSGAdapt_SolidAnalyser::Fix()
{
	Handle(ShapeFix_Solid) genericFix = new ShapeFix_Solid;
	genericFix->Init(myFirstSolid);
	genericFix->Perform();
	TopoDS_Solid theSolid =TopoDS::Solid(genericFix->Solid());
	myFirstSolid = theSolid;
        myIsFixed = Standard_True;
}


bool McCadCSGAdapt_SolidAnalyser::IsFixed(){
    return myIsFixed;
}
