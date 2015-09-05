#include <McCadCSGTool_TrivialCheck.hxx>

#include <Geom_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>  
#include <TopoDS.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>

McCadCSGTool_TrivialCheck::McCadCSGTool_TrivialCheck() :
	myElementary(Standard_False)
{
    myIsDone = false;
}

McCadCSGTool_TrivialCheck::McCadCSGTool_TrivialCheck(const TopoDS_Shape& theSolid) : myElementary(Standard_False)
{
    myIsDone = false;
        Init(theSolid);
}

void McCadCSGTool_TrivialCheck::Init(const TopoDS_Shape& theSolid)
{
	NotDone();
	mySolid = theSolid;
	if (LinearConvex())
		myElementary = Standard_True;
	else if (SingleSphere())
		myElementary = Standard_True;
	else if (SingleTorus())
		myElementary = Standard_True;
	else
		myElementary = Standard_False;
	Done();
}

Standard_Boolean McCadCSGTool_TrivialCheck::IsDone() const
{
	return myIsDone;
}

void McCadCSGTool_TrivialCheck::Done()
{
	myIsDone = Standard_True;
}

void McCadCSGTool_TrivialCheck::NotDone()
{
	myIsDone = Standard_False;
}

Standard_Boolean McCadCSGTool_TrivialCheck::LinearConvex() const
{
	Standard_Integer linType=1;
	TopExp_Explorer ex;

	for (ex.Init(mySolid, TopAbs_FACE); ex.More(); ex.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(ex.Current());
		BRepAdaptor_Surface BS(aFace, Standard_True);
		GeomAdaptor_Surface aSurface = BS.Surface();
		linType = linType * (Standard_Integer(aSurface.GetType() == GeomAbs_Plane));
	}

	if (linType && mySolid.Convex())
		return Standard_True;
	else
		return Standard_False;
}

Standard_Boolean McCadCSGTool_TrivialCheck::SingleSphere() const
{
	Standard_Integer i;
	TopExp_Explorer ex;
	GeomAdaptor_Surface aSurface;

        //TODO:: counting the amount of used faces is not sufficient
        //       use U-V-range test instead
        // trac #9

	for (ex.Init(mySolid, TopAbs_FACE), i=1; ex.More(); ex.Next(), i++)
        {
                TopoDS_Face aFace = TopoDS::Face(ex.Current());
		BRepAdaptor_Surface BS(aFace, Standard_True);
		aSurface = BS.Surface();
        }
        if ((i==1) && aSurface.GetType() == GeomAbs_Sphere)
                return Standard_True;
        else
                return Standard_False;

}

Standard_Boolean McCadCSGTool_TrivialCheck::SingleTorus() const
{
    // TODO: trac #9, see SingleSphere above
	Standard_Integer i;
	TopExp_Explorer ex;
	GeomAdaptor_Surface aSurface;
	for (ex.Init(mySolid, TopAbs_FACE), i=1; ex.More(); ex.Next(), i++)
	{
		TopoDS_Face aFace = TopoDS::Face(ex.Current());
		BRepAdaptor_Surface BS(aFace, Standard_True);
		aSurface = BS.Surface();
	}
	if ((i==1) && aSurface.GetType() == GeomAbs_Torus)
		return Standard_True;
	else
		return Standard_False;

}

Standard_Boolean McCadCSGTool_TrivialCheck::Elementary() const
{
	return myElementary;
}

