#include <McCadCSGAdapt_ShapeGeomAnalyser.ixx>

#include <TCollection_AsciiString.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Mat.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Circ2d.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Transformation.hxx>
#include <Geom_CartesianPoint.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <GCE2d_MakeLine.hxx>

#include <GProp_GProps.hxx>
#include <TopLoc_Location.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <BRep_Tool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <ShapeCustom_Surface.hxx> 
#include <GeomTools_SurfaceSet.hxx>
#include <GeomLib.hxx>

#include <McCadCSGAdapt_ShapeAnalyser.hxx>

McCadCSGAdapt_ShapeGeomAnalyser::McCadCSGAdapt_ShapeGeomAnalyser()
{

	myConvertibly = false;
	myIsDone = false;

	myNbPlane= 0;
	myNbCylinder= 0;
	myNbCone= 0;
	myNbSphere= 0;
	myNbTorus= 0;
	myNbBezierSurface= 0;
	myNbBSplineSurface= 0;
	myNbSurfaceOfRevolution= 0;
	myNbSurfaceOfExtrusion= 0;
	myNbOffsetSurface= 0;
	myNbUnspecifiedSurf= 0;

	myNbLine= 0;
	myNbCircle= 0;
	myNbEllipse= 0;
	myNbHyperbola= 0;
	myNbParabola= 0;
	myNbBezierCurve= 0;
	myNbBSplineCurve= 0;
	myNbUnspecifiedCurves= 0;
}

McCadCSGAdapt_ShapeGeomAnalyser::McCadCSGAdapt_ShapeGeomAnalyser(
		const TopoDS_Shape& theShape)
{
	myShape = theShape;
	myConvertibly = false;
	myIsDone = false;

	myNbPlane= 0;
	myNbCylinder= 0;
	myNbCone= 0;
	myNbSphere= 0;
	myNbTorus= 0;
	myNbBezierSurface= 0;
	myNbBSplineSurface= 0;
	myNbSurfaceOfRevolution= 0;
	myNbSurfaceOfExtrusion= 0;
	myNbOffsetSurface= 0;
	myNbUnspecifiedSurf= 0;

	myNbLine= 0;
	myNbCircle= 0;
	myNbEllipse= 0;
	myNbHyperbola= 0;
	myNbParabola= 0;
	myNbBezierCurve= 0;
	myNbBSplineCurve= 0;
	myNbUnspecifiedCurves= 0;

}
void McCadCSGAdapt_ShapeGeomAnalyser::SetShape(const TopoDS_Shape& theShape)
{
	myShape = theShape;
}

TopoDS_Shape McCadCSGAdapt_ShapeGeomAnalyser::GetShape() const
{
	return myShape;
}

void McCadCSGAdapt_ShapeGeomAnalyser::Init()
{

	if (myShape.IsNull())
	{
		cout << "Set the shape first!" << endl;
		myIsDone = false;
		return;
	}

	Standard_Integer fCount = 0;
	Handle_Geom_Surface theSurface;
	GeomAbs_SurfaceType theSurfType;
	Handle_Geom_Curve theCurve;
	GeomAbs_CurveType theCurveType;
	Handle_Geom_TrimmedCurve trimCurve;
	Handle_Geom_Curve basisCurve;
	Handle(Geom_Surface) myNewSurf;

	// ShapeCustom_Surface Convert;
	Handle(Geom_BSplineSurface) myBS;

	for (TopExp_Explorer exp(myShape, TopAbs_FACE); exp.More(); exp.Next())
	{
		fCount++;
		TopoDS_Face theFace = TopoDS::Face(exp.Current());
		theSurface = BRep_Tool::Surface(theFace);
		theSurfType = GeomAdaptor_Surface(theSurface).GetType();
		switch (theSurfType)
		{
		case GeomAbs_Plane:
			myNbPlane++;
			break;
		case GeomAbs_Cylinder:
			myNbCylinder++;
			break;
		case GeomAbs_Cone:
			myNbCone++;
			break;
		case GeomAbs_Sphere:
			myNbSphere++;
			break;
		case GeomAbs_Torus:
			myNbTorus++;
			break;
		case GeomAbs_BezierSurface:
			myNbBezierSurface++;
			break;
		case GeomAbs_BSplineSurface:
			myNbBSplineSurface++;
			break;
		case GeomAbs_SurfaceOfRevolution:
			myNbSurfaceOfRevolution++;
			break;
		case GeomAbs_SurfaceOfExtrusion:
			myNbSurfaceOfExtrusion++;
			break;
		case GeomAbs_OffsetSurface:
			myNbOffsetSurface++;
			break;
		default:
			myNbUnspecifiedSurf++;
			break;
		}

	}

	for (TopExp_Explorer exp(myShape, TopAbs_EDGE); exp.More(); exp.Next())
	{

		Standard_Real first, last;

		theCurve = BRep_Tool::Curve(TopoDS::Edge(exp.Current()), first, last);

		if (!theCurve.IsNull())
		{
			if (theCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
			{
				cout << " Might be a trimmed curve!!" << endl;
				trimCurve = Handle_Geom_TrimmedCurve::DownCast(theCurve);
				basisCurve = trimCurve->BasisCurve();
				theCurveType = GeomAdaptor_Curve(basisCurve).GetType();
			}
			else
			{
				theCurveType = GeomAdaptor_Curve(theCurve).GetType();

			}

			switch (theCurveType)
			{
			case GeomAbs_Line:
				myNbLine++;
				break;
			case GeomAbs_Circle:
				myNbCircle++;

				break;
			case GeomAbs_Ellipse:
				myNbEllipse++;

				break;
			case GeomAbs_Hyperbola:
				myNbHyperbola++;

				break;
			case GeomAbs_Parabola:
				myNbParabola++;

				break;
			case GeomAbs_BezierCurve:
				myNbBezierCurve++;

			case GeomAbs_BSplineCurve:
				myNbBSplineCurve++;

				break;
			default:
				myNbUnspecifiedCurves++;

				break;
			}
		}
	}
	int nonAnalytic = 0;
	nonAnalytic = myNbBezierSurface + myNbBSplineSurface
			+ myNbSurfaceOfRevolution + myNbSurfaceOfExtrusion
			+ myNbOffsetSurface + myNbUnspecifiedSurf;
	if (nonAnalytic == 0)
		myConvertibly = true;
	else
		myConvertibly = false;
	myIsDone = true;
}

void McCadCSGAdapt_ShapeGeomAnalyser::PrintSurfaceInfo(
		Standard_OStream& theStream)
{
	int NumberOfAllSurf = myNbPlane + myNbCylinder + myNbCone + myNbSphere
			+ myNbTorus + myNbBezierSurface + myNbBSplineSurface
			+ myNbSurfaceOfRevolution + myNbSurfaceOfExtrusion
			+ myNbOffsetSurface + myNbUnspecifiedSurf;

	int NumberOfAllCurves = myNbLine + myNbCircle + myNbEllipse + myNbHyperbola
			+ myNbParabola + myNbBezierCurve + myNbBSplineCurve
			+ myNbUnspecifiedCurves;

	theStream << " Summary of solid geometric content " << endl;
	theStream
			<< "============================================================ "
			<< endl;
	theStream << endl;
	theStream << endl;
	theStream << " Surface Type  " <<"Number of Surfaces" << endl;
	theStream << " Plane         " << myNbPlane << endl;
	theStream << " Cylinder      " << myNbCylinder << endl;
	theStream << " Cone          " << myNbCone << endl;
	theStream << " Sphere        " << myNbSphere << endl;
	theStream << " Torus         " << myNbTorus << endl;
	theStream << " Bezier        " << myNbBezierSurface << endl;
	theStream << " BSpline       " << myNbBSplineSurface << endl;
	theStream << " Revolution    " << myNbSurfaceOfRevolution << endl;
	theStream << " Extrusion     " << myNbSurfaceOfExtrusion << endl;
	theStream << " Offset        " << myNbOffsetSurface << endl;
	theStream << " Unspecified   " << myNbUnspecifiedSurf << endl;
	theStream << endl;
	theStream << " Total Number of Surfaces = " << NumberOfAllSurf << endl;
	theStream
			<< "============================================================ "
			<< endl;
	theStream << endl;
	theStream << endl;
	theStream << " Curve Type   " <<"Number of Curves" << endl;
	theStream << " Linear        " << myNbLine << endl;
	theStream << " Circular      " << myNbCircle << endl;
	theStream << " Elliptic      " << myNbEllipse << endl;
	theStream << " Hyperbolic    " << myNbHyperbola << endl;
	theStream << " Parabolic     " << myNbParabola << endl;
	theStream << " Bezier        " << myNbBezierCurve << endl;
	theStream << " BSpline       " << myNbBSplineCurve << endl;
	theStream << " Unspecified   " << myNbUnspecifiedCurves << endl;

	theStream << " Total Number of Curves = " << NumberOfAllCurves << endl;
	theStream
			<< "============================================================ "
			<< endl;

	theStream << endl;

}

void McCadCSGAdapt_ShapeGeomAnalyser::PrintShapeInfo(Standard_OStream& theStream) const
{

	McCadCSGAdapt_ShapeAnalyser theShapeAnalyser(myShape);

	theShapeAnalyser.PrintContent(theStream);
	theShapeAnalyser.PrintInfo(theStream);
}

Standard_Boolean McCadCSGAdapt_ShapeGeomAnalyser::Convertibly() const
{
	return myConvertibly;
}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbPlane() const
{
	return myNbPlane;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbCylinder() const
{
	return myNbCylinder;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbCone() const
{
	return myNbCone;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbSphere() const
{

	return myNbSphere;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbTorus() const
{

	return myNbTorus;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbBezierSurface() const
{

	return myNbBezierSurface;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbBSplineSurface() const
{
	return myNbBSplineSurface;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbSurfaceOfRevolution() const
{

	return myNbSurfaceOfRevolution;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbSurfaceOfExtrusion() const
{

	return myNbSurfaceOfExtrusion;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbOffsetSurface() const
{
	return myNbOffsetSurface;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbUnspecifiedSurf() const
{
	return myNbUnspecifiedSurf;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbLine() const
{
	return myNbLine;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbCircle() const
{
	return myNbCircle;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbEllipse() const
{
	return myNbEllipse;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbHyperbola() const
{
	return myNbHyperbola;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbParabola() const
{

	return myNbParabola;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbBezierCurve() const
{
	return myNbBezierCurve;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbBSplineCurve() const
{
	return myNbBSplineCurve;

}

Standard_Integer McCadCSGAdapt_ShapeGeomAnalyser::GetNbUnspecifiedCurves() const
{
	return myNbUnspecifiedCurves;
}

Standard_Boolean McCadCSGAdapt_ShapeGeomAnalyser::IsDone() const
{
	return myIsDone;
}

