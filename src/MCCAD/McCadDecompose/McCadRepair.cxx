#include "McCadRepair.hxx"
#include <ShapeFix_FixSmallFace.hxx>
#include <ShapeFix_Solid.hxx>
#include <TopoDS.hxx>

McCadRepair::McCadRepair()
{
}

McCadRepair::~McCadRepair()
{
}

TopoDS_Shape McCadRepair::RemoveSmallFaces(TopoDS_Shape &theShape)
{
    Handle(ShapeFix_FixSmallFace) sff = new ShapeFix_FixSmallFace();
    sff->Init(theShape);
    sff->SetPrecision(0.001);
    sff->SetMaxTolerance(0.001);

    sff->Perform();
    TopoDS_Shape newShape = sff->FixShape();
    theShape.Free();

    return newShape;
}


TopoDS_Solid McCadRepair::RepairSolid(TopoDS_Solid &theSolid)
{
    Handle(ShapeFix_Solid) genericFix = new ShapeFix_Solid;
    genericFix->Init(theSolid);
    genericFix->Perform();

    theSolid.Free();
    TopoDS_Solid newSolid = TopoDS::Solid(genericFix->Solid());
    return newSolid;
}
