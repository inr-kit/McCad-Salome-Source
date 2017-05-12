#include "McCadFuseSurface.hxx"

#include <TopTools_HSequenceOfShape.hxx>
#include <TopAbs_Orientation.hxx>
#include <Handle_Geom_Surface.hxx>
#include <BRep_Tool.hxx>

#include <BRepCheck_Analyzer.hxx>
#include <BRepAlgo_Fuse.hxx>
#include <gp_Pnt.hxx>
#include <gp_Cylinder.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
//qiu add SALOME definition
#ifndef SALOME
#include <McCadMessenger_Singleton.hxx>
#endif
#include <TopoDS.hxx>
#include <STEPControl_Writer.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>

#include <ShapeFix_Shape.hxx>

#include "../McCadMcVoid/IGeomFace.hxx"
#include "../McCadMcVoid/McCadGeomPlane.hxx"
#include "../McCadMcVoid/McCadGeomCylinder.hxx"
#include "../McCadMcVoid/McCadGeomCone.hxx"
#include "../McCadMcVoid/McCadGeomSphere.hxx"
#include "../McCadMcVoid/McCadGeomTorus.hxx"
#include "../McCadMcVoid/McCadGeomRevolution.hxx"
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadMathTool.hxx"

McCadFuseSurface::McCadFuseSurface()
{
}

McCadFuseSurface::~McCadFuseSurface()
{
}

/** ********************************************************************
* @brief Judge the two edges are same or not, from the geometries of
*        curves and start and end vertices
*
* @param edgeA(TopoDS_Edge&) edgeB(TopoDS_Edge&)
* @return bool
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
bool McCadFuseSurface::IsSameEdge(TopoDS_Edge & edgeA, TopoDS_Edge & edgeB )
{
    /// Get the start and end points of the curves
    Standard_Real fStartA, fEndA;
    Handle(Geom_Curve) theCurveA = BRep_Tool::Curve(edgeA, fStartA, fEndA);

    gp_Pnt posStartA, posEndA;
    theCurveA->D0(fStartA, posStartA);
    theCurveA->D0(fEndA, posEndA);

    Standard_Real fStartB, fEndB;
    Handle(Geom_Curve) theCurveB = BRep_Tool::Curve(edgeB, fStartB, fEndB);

    gp_Pnt posStartB, posEndB;
    theCurveB->D0(fStartB, posStartB);
    theCurveB->D0(fEndB, posEndB);

    /// If the type of curves are not same, return false.
    if(GeomAdaptor_Curve(theCurveA).GetType() != GeomAdaptor_Curve(theCurveB).GetType())
    {
        return false;
    }

    /// If the start and end points of the two curves are same, then they are same curves
    if ( ( posStartA.IsEqual(posEndB,1.e-5) && posEndA.IsEqual(posStartB,1.e-5) )
         || ( posStartA.IsEqual(posStartB,1.e-5) && posEndB.IsEqual(posEndA,1.e-5)) )
    {
        return true;
    }
    else
    {
        if((posStartA.Distance(posEndB) <= 0.5 && posEndA.Distance(posStartB)<= 0.5)
                || (posStartA.Distance(posStartB) <= 0.5 && posEndB.Distance(posEndA)<= 0.5))
        {
            return true;
        }
        else
        {
             return false;
        }
    }
}


//        cout<<" X "<<posStartA.X()<<" Y  "<<posStartA.Y()<<" Z   "<<posStartA.Z()<<endl;
//        cout<<" X "<<posEndA.X()<<" Y  "<<posEndA.Y()<<" Z   "<<posEndA.Z()<<endl<<endl;

//        cout<<" X "<<posEndB.X()<<" Y  "<<posEndB.Y()<<" Z   "<<posEndB.Z()<<endl;
//        cout<<" X "<<posStartB.X()<<" Y  "<<posStartB.Y()<<" Z   "<<posStartB.Z()<<endl<<endl;


/** ********************************************************************
* @brief .
*
* @param
* @return bool
*
* @date 13/03/2014
* @author  Lei Lu
***********************************************************************/
bool McCadFuseSurface::IsSameSurfaces(GeomAdaptor_Surface &SurfA, GeomAdaptor_Surface &SurfB)
{
    IGeomFace * pGeomFaceA = NULL;
    IGeomFace * pGeomFaceB = NULL;  
    switch (SurfA.GetType())
    {
        case GeomAbs_Plane:
        {
            pGeomFaceA = new McCadGeomPlane(SurfA);
            pGeomFaceB = new McCadGeomPlane(SurfB);
            break;
        }
        case GeomAbs_Cylinder:
        {
            pGeomFaceA = new McCadGeomCylinder(SurfA);
            pGeomFaceB = new McCadGeomCylinder(SurfB);            
            break;           
        }
        case GeomAbs_Cone:
        {
            pGeomFaceA = new McCadGeomCone(SurfA);
            pGeomFaceB = new McCadGeomCone(SurfB);
            cout<<"GeomAbs_Cone"<<endl;
            break;
        }
        case GeomAbs_Sphere:
        {
            pGeomFaceA = new McCadGeomSphere(SurfA);
            pGeomFaceB = new McCadGeomSphere(SurfB);
            cout<<"GeomAbs_Sphere"<<endl;
            break;
        }
        case GeomAbs_Torus:
        {
            pGeomFaceA = new McCadGeomTorus(SurfA);
            pGeomFaceB = new McCadGeomTorus(SurfB);
            cout<<"GeomAbs_Torus"<<endl;
            break;
        }
        case GeomAbs_SurfaceOfRevolution:
        {
            pGeomFaceA = new McCadGeomRevolution(SurfA);
            pGeomFaceB = new McCadGeomRevolution(SurfB);
            cout<<"GeomAbs_SurfaceOfRevolution"<<endl;
            break;
        }
        default:break;
    }   

    if(NULL != pGeomFaceA && NULL != pGeomFaceB)
    {
        if(pGeomFaceA->IsEqual(pGeomFaceB))
        {
            delete pGeomFaceA;
            delete pGeomFaceB;

            pGeomFaceA = NULL;
            pGeomFaceB = NULL;

            return true;
        }
        else
        {
            delete pGeomFaceA;
            delete pGeomFaceB;

            pGeomFaceA = NULL;
            pGeomFaceB = NULL;

            return false;
        }
    }
    else
    {
        return false;
    }
}


/** ********************************************************************
* @brief .
*
* @param
* @return bool
*
* @date 13/03/2014
* @author  Lei Lu
***********************************************************************/
bool McCadFuseSurface::HasCommonEdge(GeomAdaptor_Surface &SurfA, GeomAdaptor_Surface &SurfB)
{

//    Standard_Real UMin1,UMax1,VMin1,VMax1;
//    BRepTools::UVBounds(m_surfA,UMin1,UMax1,VMin1,VMax1);

//    Standard_Real UMin2,UMax2,VMin2,VMax2;
//    BRepTools::UVBounds(m_surfB,UMin2,UMax2,VMin2,VMax2);

//    McCadMathTool::ZeroValue(UMin1,1.e-7);
//    McCadMathTool::ZeroValue(UMin2,1.e-7);
//    McCadMathTool::ZeroValue(UMax1,1.e-7);
//    McCadMathTool::ZeroValue(UMax2,1.e-7);
//    McCadMathTool::ZeroValue(VMin1,1.e-7);
//    McCadMathTool::ZeroValue(VMin2,1.e-7);
//    McCadMathTool::ZeroValue(VMax1,1.e-7);
//    McCadMathTool::ZeroValue(VMax2,1.e-7);

    /// Judge the surfaces have the common edge
    for(TopExp_Explorer ex1(m_surfA, TopAbs_EDGE); ex1.More(); ex1.Next())
    {
        TopoDS_Edge e1 = TopoDS::Edge(ex1.Current());
        for(TopExp_Explorer ex2(m_surfB, TopAbs_EDGE); ex2.More(); ex2.Next())
        {
            TopoDS_Edge e2 = TopoDS::Edge(ex2.Current());
            if (IsSameEdge(e1, e2))
            {
               return true;
            }
        }
    }
    return false;
}



/** ********************************************************************
* @brief Judge the two surfaces can be fused or not, from the geometries
*        and whether they have the common edge or not. It is different
*        from combine surface function, this function is only used for
*        process the cylinders which are separated into two pieces. But
*        the combination function is used for remove some internal loop
*        in one surface.
*
* @param
* @return bool
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
bool McCadFuseSurface::CanbeFused()
{
    /// Get the geometries of the faces and judge they are same or not.

    TopLoc_Location loc;
    Handle_Geom_Surface hGeomA = BRep_Tool::Surface(m_surfA,loc);
    GeomAdaptor_Surface GemoAdptSurfA(hGeomA);

    Handle_Geom_Surface hGeomB = BRep_Tool::Surface(m_surfB,loc);
    GeomAdaptor_Surface GemoAdptSurfB(hGeomB);

    /// The planes are no need to add assisted surfaces, so they are no need to be fused
    if( GemoAdptSurfA.GetType() == GeomAbs_Plane
            || GemoAdptSurfB.GetType() == GeomAbs_Plane)
    {
        return false;
    }

    if(GemoAdptSurfA.GetType() != GemoAdptSurfB.GetType())
    {
        return false;
    }

    TopAbs_Orientation orientA = m_surfA.Orientation();
    TopAbs_Orientation orientB = m_surfB.Orientation();
    if ((orientA != orientB))
    {
        return false;
    }

    if( GemoAdptSurfA.GetType() == GeomAbs_Torus
            && GemoAdptSurfB.GetType() == GeomAbs_Torus)
    {
       // if(orientA == TopAbs_REVERSED || orientB == TopAbs_REVERSED)
        {           
            if(IsSameSurfaces(GemoAdptSurfA,GemoAdptSurfB))
            {
                return true;
            }
            else
            {
                return false;
            }
        }       
    }
    else
    {
        if(IsSameSurfaces(GemoAdptSurfA,GemoAdptSurfB)&&HasCommonEdge(GemoAdptSurfA,GemoAdptSurfB))
        {
            cout<<"Two cylinders with same geometries are fused."<<endl;
            return true;
        }
        else
        {          
            return false;
        }
    }

    return false;
}



/** ********************************************************************
* @brief Judge two surfaces can be combined or not
*        Normally judge if the two surfaces have same geometries and
*        common edges, sometimes. It used for remove the internal loops
*        of one surface.
*
* @param
* @return
*
* @date    18/03/2016
* @author  Lei Lu
***********************************************************************/
bool McCadFuseSurface::CanbeCombined()
{
    /// Get the geometries of the faces and judge they are same or not.
    TopLoc_Location loc;
    Handle_Geom_Surface hGeomA = BRep_Tool::Surface(m_surfA,loc);
    GeomAdaptor_Surface GemoAdptSurfA(hGeomA);

    Handle_Geom_Surface hGeomB = BRep_Tool::Surface(m_surfB,loc);
    GeomAdaptor_Surface GemoAdptSurfB(hGeomB);

    if(GemoAdptSurfA.GetType() != GemoAdptSurfB.GetType())
    {
        return false;
    }

    TopAbs_Orientation orientA = m_surfA.Orientation();
    TopAbs_Orientation orientB = m_surfB.Orientation();
    if ((orientA != orientB))
    {
        return false;
    }

    if(IsSameSurfaces(GemoAdptSurfA,GemoAdptSurfB) && HasCommonEdge(GemoAdptSurfA,GemoAdptSurfB))
    {
        cout<<"Combined two surfaces"<<endl;
        return true;
    }

    return false;
}




/** ********************************************************************
* @brief Create a new surface in stead of the original two connected
*        surface with same geometries
*
* @param
* @return TopoDS_Face
*
* @date 01/11/2015
* @author  Lei Lu
***********************************************************************/
TopoDS_Face McCadFuseSurface::CrtNewSurf()
{    
    /// Trace the edges or the first surface
    vector<TopoDS_Edge> edge_list;
    for(TopExp_Explorer ex1(m_surfA, TopAbs_EDGE); ex1.More(); ex1.Next())
    {
        TopoDS_Edge e1 = TopoDS::Edge(ex1.Current());
        edge_list.push_back(e1);
    }

    /// remove the common edge of the first surface and second surface
    Standard_Boolean bHasSameEdge;
    for(TopExp_Explorer ex2(m_surfB, TopAbs_EDGE); ex2.More(); ex2.Next())
    {
        bHasSameEdge = Standard_False;

        int k = 0;
        TopoDS_Edge e2 = TopoDS::Edge(ex2.Current());
        for(int i = 0; i < edge_list.size(); i++ )
        {
            TopoDS_Edge e3 = edge_list.at(i);
            if (IsSameEdge(e2, e3))
            {
                bHasSameEdge = Standard_True;
                k = i;
                break;
            }
        }

        if (!bHasSameEdge)
        {
            edge_list.push_back(e2);
        }
        else
        {
            edge_list.erase(edge_list.begin()+k);
        }

    }


//        for(int i = 0; i < edge_list.size()-1; i++)
//        {
//            for(int j = i+1; j < edge_list.size(); j++)
//            {
//                TopoDS_Edge edgeA = edge_list.at(i);
//                TopoDS_Edge edgeB = edge_list.at(j);
//                TopoDS_Edge new_edge;

//                if (JointEdges(edgeA,edgeB,new_edge))
//                {
//                    edge_list.erase(edge_list.begin()+j);
//                    edge_list.erase(edge_list.begin()+i);

//                    edge_list.push_back(new_edge);
//                    i--;
//                    j--;
//                }
//            }
//        }

//        STEPControl_Writer wrt;

//        cout<<"edge list size"<<edge_list.size()<<endl;
//        for(int i = 0; i < edge_list.size(); i++)
//        {
//            TopoDS_Edge edgeA = edge_list.at(i);
//            wrt.Transfer(edgeA,STEPControl_AsIs);
//        }

//        wrt.Write("connectedEdge.stp");

    /// Make a wire with the edges, the common edge of two surfaces is removed
    TopLoc_Location loc;
    const Handle(Geom_Surface)& aS1 = BRep_Tool::Surface(m_surfA,loc);
    TopoDS_Wire wire = MakeWire(edge_list);

    //return wire;

//    /// Create the new face with the wire
    TopoDS_Face newFace = BRepBuilderAPI_MakeFace(aS1,wire).Face();

    ////**********************************************************************/
    //! The created new face always has geometry error.
    //! So it needs the healing before using.
    Handle(ShapeFix_Shape) shpFixer = new ShapeFix_Shape(newFace);
    shpFixer->SetPrecision(1.0e-7);
    shpFixer->SetMaxTolerance(1.0e-3);

    shpFixer->FixWireTool()->FixRemovePCurveMode() = 1;
    shpFixer->FixWireTool()->FixConnected();

    shpFixer->Perform();









    newFace = TopoDS::Face(shpFixer->Shape());
    return newFace;
}



/** ********************************************************************
* @brief Joint the two edges with same geometries and connected at a vertex
*
* @param TopoDS_Edge edgeA,TopoDS_Edge edgeB,
*        TopoDS_Edge new_edge (new edge connects edgeA and edgeB)
* @return Standard_Boolean
*
* @date 01/11/2015
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadFuseSurface::JointEdges(TopoDS_Edge & edgeA,
                                              TopoDS_Edge & edgeB,
                                              TopoDS_Edge & new_edge)
{
    Standard_Real fStartA, fEndA;
    Handle(Geom_Curve) theCurveA = BRep_Tool::Curve(edgeA, fStartA, fEndA);

    Standard_Real fStartB, fEndB;
    Handle(Geom_Curve) theCurveB = BRep_Tool::Curve(edgeB, fStartB, fEndB);

    // The types of curves of edges
    GeomAdaptor_Curve gp_CurveA(theCurveA);
    GeomAdaptor_Curve gp_CurveB(theCurveB);

    if(gp_CurveA.GetType() != gp_CurveB.GetType())
    {
        return Standard_False;
    }

    gp_Pnt posVexA, posVexB;
    if (gp_CurveA.GetType() == 1)  // If the edge is a circle
    {
        if (CompareVertex(edgeA,edgeB,posVexA,posVexB)) // calculate start and end point of new edge
        {
            gp_Circ circA = gp_CurveA.Circle();
            gp_Circ circB = gp_CurveB.Circle();

            if( (circA.Radius() == circB.Radius())     // Radius and point of two circles are same
                &&(circA.Location().IsEqual(circB.Location(),1.e-5)) )
            {
                new_edge = BRepBuilderAPI_MakeEdge(circA,posVexA,posVexB);
                return Standard_True;
            }
        }
    }
    else if(gp_CurveA.GetType() == 2) // If the edge is a ellipse circle
    {
        if (CompareVertex(edgeA,edgeB,posVexA,posVexB))
        {
            gp_Elips elipsA = gp_CurveA.Ellipse();
            gp_Elips elipsB = gp_CurveB.Ellipse();

            if((elipsA.MinorRadius() == elipsB.MinorRadius())      // Major and Minor radius are same
               && (elipsA.MajorRadius() == elipsB.MajorRadius()) )
            {
                new_edge = BRepBuilderAPI_MakeEdge(elipsB,posVexA,posVexB);
                return Standard_True;
            }
        }
    }
    else if(gp_CurveA.GetType() == GeomAbs_Line) // If the edge is a ellipse circle
    {
        if (CompareVertex(edgeA,edgeB,posVexA,posVexB))
        {
            gp_Lin lineA = gp_CurveA.Line();
            gp_Lin lineB = gp_CurveB.Line();

            if((lineA.Direction().IsEqual(lineB.Direction(),1.0e-7)))  // Major and Minor radius are same
            {
                new_edge = BRepBuilderAPI_MakeEdge(lineA,posVexB,posVexA);
                return Standard_True;
            }
        }
    }
    else
    {
        return Standard_False;
    }

    return Standard_False;
}


/** ********************************************************************
* @brief Joint the two edges with same geometries and connected at a vertex
*
* @param TopoDS_Edge edgeA,TopoDS_Edge edgeB,
*        TopoDS_Edge new_edge (new edge connects edgeA and edgeB)
* @return Standard_Boolean
*
* @date 01/11/2015
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadFuseSurface::CompareVertex(TopoDS_Edge &edgeA,
                                                 TopoDS_Edge &edgeB,
                                                 gp_Pnt &posVexA,
                                                 gp_Pnt &posVexB)
{
    Standard_Real fStartA, fEndA;
    Handle(Geom_Curve) theCurveA = BRep_Tool::Curve(edgeA, fStartA, fEndA);

    gp_Pnt posStartA, posEndA;
    theCurveA->D0(fStartA, posStartA);
    theCurveA->D0(fEndA, posEndA);

    Standard_Real fStartB, fEndB;
    Handle(Geom_Curve) theCurveB = BRep_Tool::Curve(edgeB, fStartB, fEndB);

    gp_Pnt posStartB, posEndB;
    theCurveB->D0(fStartB, posStartB);
    theCurveB->D0(fEndB, posEndB);

    if (posStartA.IsEqual(posStartB,1.e-5))
    {
        posVexA = posEndB;
        posVexB = posEndA;
        return Standard_True;
    }
    else if (posStartA.IsEqual(posEndB,1.e-5))
    {
        posVexA = posStartB;
        posVexB = posEndA;
        return Standard_True;
    }
    else if (posEndA.IsEqual(posStartB,1.e-5))
    {
        posVexA = posStartA;
        posVexB = posEndB;
        return Standard_True;
    }
    else if (posEndA.IsEqual(posEndB,1.e-5))
    {
        posVexA = posStartB;
        posVexB = posStartA;
        return Standard_True;
    }
    else
    {        
        return Standard_False;
    }

    return Standard_False;
}






/** ********************************************************************
* @brief  Create a fused surface based on the original surfaces.
*
* @param
* @return bool
*
* @modify 30/04/2014 repair some u values of cylinder are large than
*                    2*M_PI. And some u values of common edge in two faces
*                    are not same.
*
* @date 04/01/2014
* @author  Lei Lu
* @modify 13/04/2016 use the fmod to find the connected U coordination
***********************************************************************/
TopoDS_Face McCadFuseSurface::FuseSurfaces()
{
    Standard_Real UMin1,UMax1,VMin1,VMax1;
    BRepTools::UVBounds(m_surfA,UMin1,UMax1,VMin1,VMax1);
    Standard_Real UMin2,UMax2,VMin2,VMax2;
    BRepTools::UVBounds(m_surfB,UMin2,UMax2,VMin2,VMax2);

    cout<<"UMin1 "<<UMin1<<" UMax1 "<<UMax1<<endl;
    cout<<"UMin2 "<<UMin2<<" UMax2 "<<UMax2<<endl;
    cout<<"VMin1 "<<VMin1<<" VMax1 "<<VMax1<<endl;
    cout<<"VMin2 "<<VMin2<<" VMax2 "<<VMax2<<endl;

    McCadMathTool::ZeroValue(UMin1,1.e-7);
    McCadMathTool::ZeroValue(UMin2,1.e-7);
    McCadMathTool::ZeroValue(UMax1,1.e-7);
    McCadMathTool::ZeroValue(UMax2,1.e-7);
    McCadMathTool::ZeroValue(VMin1,1.e-7);
    McCadMathTool::ZeroValue(VMin2,1.e-7);
    McCadMathTool::ZeroValue(VMax1,1.e-7);
    McCadMathTool::ZeroValue(VMax2,1.e-7);

    Standard_Boolean isClose = Standard_False;
    if ( Abs(UMax1-UMin1)+Abs(UMax2-UMin2) >= 2*M_PI )
    {
        isClose = Standard_True;
    }

    Standard_Real UMin(0.0),UMax(0.0), VMin(0.0), VMax(0.0);

    Standard_Real UDis1 = fmod(Abs(UMin1-UMax2),2*M_PI);
    Standard_Real UDis2 = fmod(Abs(UMin2-UMax1),2*M_PI);

    if( UDis1 < UDis2)
    {
        UMin = UMin2;
        UMax = UMax1;
    }
    else
    {
        UMin = UMin1;
        UMax = UMax2;
    }

    if(isClose)
    {
        UMin = 0;
        UMax = 2*M_PI;
    }

   // cout<<"VMin1 "<<VMin1<<" VMax1 "<<VMax1<<endl;
   // cout<<"VMin2 "<<VMin2<<" VMax2 "<<VMax2<<endl;

    (VMin1 <= VMin2) ? VMin = VMin1 : VMin = VMin2;
    (VMax1 >= VMax2) ? VMax = VMax1 : VMax = VMax2;




    TopLoc_Location loc;
    const Handle(Geom_Surface)& aS1 = BRep_Tool::Surface(m_surfA,loc);

    TopoDS_Face face  = BRepBuilderAPI_MakeFace(aS1, UMin,UMax, VMin, VMax, 1.e-7).Face();

    return face;
}




/** ********************************************************************
* @brief  Create a fused surface based on the original surfaces.
*
* @param
* @return bool
*
* @modify 30/04/2014 repair some u values of cylinder are large than
*                    2*M_PI. And some u values of common edge in two faces
*                    are not same.
*
* @date 04/01/2014
* @author  Lei Lu
* @modify 13/04/2016 use the fmod to find the connected U coordination
***********************************************************************/
TopoDS_Face McCadFuseSurface::FuseTorus()
{
    Standard_Real UMin1,UMax1,VMin1,VMax1;
    BRepTools::UVBounds(m_surfA,UMin1,UMax1,VMin1,VMax1);
    Standard_Real UMin2,UMax2,VMin2,VMax2;
    BRepTools::UVBounds(m_surfB,UMin2,UMax2,VMin2,VMax2);

    McCadMathTool::ZeroValue(UMin1,1.e-7);
    McCadMathTool::ZeroValue(UMin2,1.e-7);
    McCadMathTool::ZeroValue(UMax1,1.e-7);
    McCadMathTool::ZeroValue(UMax2,1.e-7);
    McCadMathTool::ZeroValue(VMin1,1.e-7);
    McCadMathTool::ZeroValue(VMin2,1.e-7);
    McCadMathTool::ZeroValue(VMax1,1.e-7);
    McCadMathTool::ZeroValue(VMax2,1.e-7);

    cout<<"UMin1 "<<UMin1<<" UMax1 "<<UMax1<<endl;
    cout<<"UMin2 "<<UMin2<<" UMax2 "<<UMax2<<endl;

    cout<<"VMin1 "<<VMin1<<" VMax1 "<<VMax1<<endl;
    cout<<"VMin2 "<<VMin2<<" VMax2 "<<VMax2<<endl;

    Standard_Boolean isClose = Standard_False;
    if ( Abs(UMax1-UMin1)+Abs(UMax2-UMin2) >= 2*M_PI )
    {
        isClose = Standard_True;
    }

    Standard_Real UMin(0.0),UMax(0.0), VMin(0.0), VMax(0.0);

    Standard_Real UDis1 = fmod(Abs(UMin1-UMax2),2*M_PI);
    Standard_Real UDis2 = fmod(Abs(UMin2-UMax1),2*M_PI);

    if( UDis1 < UDis2)
    {
        UMin = UMin2;
        UMax = UMax1;
    }
    else
    {
        UMin = UMin1;
        UMax = UMax2;
    }

    if(isClose)
    {
        UMin = 0;
        UMax = 2*M_PI;
    }

    (VMin1 <= VMin2) ? VMin = VMin1 : VMin = VMin2;
    (VMax1 >= VMax2) ? VMax = VMax1 : VMax = VMax2;

    cout<<"UMin "<<UMin<<" UMax "<<UMax<<endl;
    cout<<"VMin "<<VMin<<" VMax "<<VMax<<endl;

    TopLoc_Location loc;
    const Handle(Geom_Surface)& aS1 = BRep_Tool::Surface(m_surfA,loc);

    TopoDS_Face face  = BRepBuilderAPI_MakeFace(aS1, UMin,UMax, VMin, VMax, 1.e-7).Face();

    return face;
}



/** ********************************************************************
* @brief connect the edges to be a wire .
*
* @param  edge_list(vector<TopoDS_Edge>)
* @return TopoDS_Wire
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
TopoDS_Wire McCadFuseSurface::MakeWire(vector<TopoDS_Edge> edge_list)
{   
    BRepBuilderAPI_MakeWire wireMaker;

    for(int i = 0; i < edge_list.size(); i++)
    {
        wireMaker.Add(edge_list.at(i));
    }

    TopoDS_Wire theWire = wireMaker.Wire();   

    if(!theWire.Closed())
    {
        BRepBuilderAPI_Sewing Sew(1.e-04);
        for(int i = 0; i < edge_list.size(); i++)
        {
            Sew.Add(edge_list.at(i));
        }

        TopoDS_Shape sewedShape;
        try
        {
            Sew.Perform();
            sewedShape = Sew.SewedShape();
        }
        catch(...)
        {
            cout << "sewing failed\n";
        }

        BRepBuilderAPI_MakeWire wireMaker2;

        for(TopExp_Explorer ex(sewedShape,TopAbs_EDGE); ex.More(); ex.Next())
        {           
            TopoDS_Edge edge = TopoDS::Edge(ex.Current());
            wireMaker2.Add(edge);
        }

        theWire = wireMaker2.Wire();
    }

    return theWire;
}



/** ********************************************************************
* @brief Fuse faces on the same surface that share an common edge .
*
* @param
* @return bool
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
bool McCadFuseSurface::FuseCylinders()
{
    if (!CanbeFused())
    {        
        return false;
    }
#ifndef SALOME
    McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
#endif
    try
    {
        //m_newSurf = FuseSurfaces();
        m_newSurf = FuseTorus();
    }
    catch(...)
    {
#ifndef SALOME
        msgr->Message("_#_McCadFuseSurface :: Boolean Fuse Operation crashed!!!\n",
                              McCadMessenger_WarningMsg); 
#else
        cout <<"_#_McCadFuseSurface :: Boolean Fuse Operation crashed!!!\n" <<endl;
#endif
        return false;
    }

    BRepCheck_Analyzer face_analyzer(m_newSurf, true);  // check validity of Result
    if(!face_analyzer.IsValid())
    {
#ifndef SALOME
        msgr->Message("_#_McCadFuseSurface :: The fused surface is not available\n",
                              McCadMessenger_WarningMsg);
#else
        cout <<"_#_McCadFuseSurface :: The fused surface is not available\n" <<endl;
#endif

        return false;
    }

    /// adjust orientation if neccessary - in some cases this is needed (even for fusion of planar faces)!!!
    if(m_newSurf.Orientation() != m_surfA.Orientation())
    {
        m_newSurf.Orientation(m_surfA.Orientation());
    }

    return true;

//    try
//    {
//        BRepAlgo_Fuse Fuser(m_surfA, m_surfB);
//        if(!Fuser.IsDone())
//        {
//            msgr->Message("_#_McCadFuseSurface :: Boolean Fuse Operation failed for two faces!",
//                                      McCadMessenger_WarningMsg);
//            return false;
//        }

//        fused_shape = Fuser.Shape();
//    }
//    catch(...)
//    {
//        msgr->Message("_#_McCadFuseSurface :: Boolean Fuse Operation crashed!!!\n",
//                              McCadMessenger_WarningMsg);
//        return false;
//    }
//    for(TopExp_Explorer ex(fused_shape, TopAbs_FACE); ex.More(); ex.Next())
//    {
//        m_newSurf = TopoDS::Face(ex.Current());
//    }

}


/** ********************************************************************
* @brief Combine two surfaces with same geometries and common edges
*
* @param
* @return bool
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
bool McCadFuseSurface::CombineSurfs()
{
    if (!CanbeCombined())
    {        
        return false;
    }   
#ifndef SALOME
    McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
#endif
    try
    {
        m_combSurf = CrtNewSurf();

        //BRepAlgo_Fuse Fuser(m_surfA, m_surfB);
//        if(!Fuser.IsDone())
//        {
//            msgr->Message("_#_McCadFuseSurface :: Boolean Fuse Operation failed for two faces!",
//                                      McCadMessenger_WarningMsg);
//            return false;
//        }

        //m_combSurf = TopoDS::Face(Fuser.Shape());
    }
    catch(...)
    {
#ifndef SALOME
        msgr->Message("_#_McCadFuseSurface :: Boolean combination operation crashed!!!\n",
                              McCadMessenger_WarningMsg);
#else
        cout <<"_#_McCadFuseSurface :: Boolean combination operation crashed!!!" <<endl;
#endif
        return false;
    }

    BRepCheck_Analyzer face_analyzer(m_combSurf, true);  // check validity of Result
    if(!face_analyzer.IsValid())
    {
#ifndef SALOME
        msgr->Message("_#_McCadFuseSurface :: The combined surface is not available\n",
                              McCadMessenger_WarningMsg);
#else
        cout <<"_#_McCadFuseSurface :: The combined surface is not available" <<endl;
#endif
        return false;
    }

    /// adjust orientation if neccessary - in some cases this is needed (even for fusion of planar faces)!!!
    if(m_combSurf.Orientation() != m_surfA.Orientation())
    {
        m_combSurf.Orientation(m_surfA.Orientation());
    }

    return true;
}




/** ********************************************************************
* @brief Set the surfaces to be fused .
*
* @param surfA(TopoDS_Face) surfB(TopoDS_Face)
* @return void
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
void McCadFuseSurface::SetSurfaces(TopoDS_Face surfA, TopoDS_Face surfB)
{
    m_surfA = surfA;
    m_surfB = surfB;
}



/** ********************************************************************
* @brief Get the new fused surface.
*
* @param
* @return TopoDS_Face
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
TopoDS_Face McCadFuseSurface::GetNewSurf()
{
    return m_newSurf;
}



/** ********************************************************************
* @brief Get the new fused surface.
*
* @param
* @return TopoDS_Face
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
TopoDS_Face McCadFuseSurface::GetCombSurf()
{
    return m_combSurf;
}



/** ********************************************************************
***********************************************************************/
bool McCadFuseSurface::FindCutEdge(TopoDS_Edge &edge)
{
    TopLoc_Location loc;
    Handle_Geom_Surface hGeomA = BRep_Tool::Surface(m_surfA,loc);
    GeomAdaptor_Surface GemoAdptSurfA(hGeomA);

    Handle_Geom_Surface hGeomB = BRep_Tool::Surface(m_surfB,loc);
    GeomAdaptor_Surface GemoAdptSurfB(hGeomB);

    if( GemoAdptSurfA.GetType() == GeomAbs_Plane
            || GemoAdptSurfB.GetType() == GeomAbs_Plane)
    {
        return false;
    }

    if(GemoAdptSurfA.GetType() != GemoAdptSurfB.GetType())
    {
        return false;
    }

    if( GemoAdptSurfA.GetType() == GeomAbs_Cylinder
            && GemoAdptSurfB.GetType() == GeomAbs_Cylinder)
    {
        if(FindCommonEdge(GemoAdptSurfA,GemoAdptSurfB,edge))
        {
            return true;
        }
    }

    return false;
}


/** ********************************************************************
* @brief .
*
* @param
* @return bool
*
* @date 13/03/2014
* @author  Lei Lu
***********************************************************************/
bool McCadFuseSurface::FindCommonEdge(GeomAdaptor_Surface &SurfA,
                                      GeomAdaptor_Surface &SurfB,
                                      TopoDS_Edge &edge)
{

    /// Judge the surfaces have the common edge
    for(TopExp_Explorer ex1(m_surfA, TopAbs_EDGE); ex1.More(); ex1.Next())
    {
        TopoDS_Edge e1 = TopoDS::Edge(ex1.Current());
        for(TopExp_Explorer ex2(m_surfB, TopAbs_EDGE); ex2.More(); ex2.Next())
        {
            TopoDS_Edge e2 = TopoDS::Edge(ex2.Current());
            if (IsSameEdge(e1, e2))
            {
               edge = e2;
               return true;
            }
        }
    }
    return false;
}


//    vector<TopoDS_Edge> edge_listA;
//    vector<TopoDS_Edge> edge_listB;
//    vector<TopoDS_Edge> edge_listF;

//    /// Judge the surfaces have the common edge
//    for(TopExp_Explorer ex1(m_surfA, TopAbs_EDGE); ex1.More(); ex1.Next())
//    {
//        TopoDS_Edge e1 = TopoDS::Edge(ex1.Current());
//        edge_listA.push_back(e1);
//    }

//    for(TopExp_Explorer ex2(m_surfB, TopAbs_EDGE); ex2.More(); ex2.Next())
//    {
//        TopoDS_Edge e2 = TopoDS::Edge(ex2.Current());
//        edge_listB.push_back(e2);
//    }

//    for(int i = 0; i < edge_listA.size(); i++)
//    {
//        bool bRepeat = false;
//        TopoDS_Edge e1 = edge_listA.at(i);
//        for (int j = 0; j < edge_listB.size(); j++)
//        {
//            TopoDS_Edge e2 = edge_listB.at(j);
//            if (IsSameEdge(e1, e2))
//            {
//               bRepeat = true;

//               edge_listB.erase(edge_listB.begin()+j);
//               break;
//            }
//        }

//        if (!bRepeat)
//        {
//            edge_listF.push_back(e1);
//        }
//    }

//    TopoDS_Edge new_edge;
//    vector<TopoDS_Edge> edge_list2;
//    for(int i = 0; i < edge_list.size(); i++)
//    {
//        TopoDS_Edge edgeA = edge_list.at(i);

//        Standard_Real fStartA, fEndA;
//        Handle(Geom_Curve) theCurveA = BRep_Tool::Curve(edgeA, fStartA, fEndA);
//        GeomAdaptor_Curve gp_CurveA(theCurveA);

//        if (gp_CurveA.GetType() == 1)
//        {
//            gp_Circ circA = gp_CurveA.Circle();
//            new_edge = BRepBuilderAPI_MakeEdge(circA,fStartA,fEndA);
//        }
//        else if(gp_CurveA.GetType() == 2)
//        {
//            gp_Elips elipsA = gp_CurveA.Ellipse();
//            new_edge = BRepBuilderAPI_MakeEdge(elipsA,fStartA,fEndA);
//        }
//        edge_list2.push_back(new_edge);
//    }


//    TopLoc_Location loc;
//    Handle(Geom_Surface) surface = BRep_Tool::Surface(m_surfA,loc);

//    BRepAdaptor_Surface BS(m_surfA, Standard_True);
//    GeomAdaptor_Surface AdpSurf = BS.Surface();
//    gp_Cylinder cyln = AdpSurf.Cylinder();

//    gp_Ax3 ax3 = cyln.Position();
//    Standard_Real radius = cyln.Radius();

//    gp_Cylinder NewCyln(ax3,radius);

//    BRepBuilderAPI_Sewing Sew(1.e-04);
//    for(TopExp_Explorer ex3(wire, TopAbs_EDGE); ex3.More(); ex3.Next())
//    {
//        TopoDS_Edge e3 = TopoDS::Edge(ex3.Current());
//        Sew.Add(e3);
//    }

//    Sew.Perform();
//    TopoDS_Shape sShape = Sew.SewedShape();
//    TopoDS_Wire sWire = TopoDS::Wire(sShape);

//TopoDS_Face face = FuseSurfaces();
