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
#include <McCadMessenger_Singleton.hxx>
#include <TopoDS.hxx>
#include <STEPControl_Writer.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools.hxx>

#include "../McCadMcVoid/IGeomFace.hxx"
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
bool McCadFuseSurface::IsSameSurfaces(GeomAdaptor_Surface &SurfA, GeomAdaptor_Surface &SurfB)
{
    IGeomFace * pGeomFaceA = NULL;
    IGeomFace * pGeomFaceB = NULL;
    switch (SurfA.GetType())
    {
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
*        and whether they have the common edge or not.
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
        if(orientA == TopAbs_REVERSED || orientB == TopAbs_REVERSED)
        {
            return false;
        }

        if(IsSameSurfaces(GemoAdptSurfA,GemoAdptSurfB))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if(orientA == TopAbs_FORWARD || orientB == TopAbs_FORWARD)
        {
            return false;
        }

        if(IsSameSurfaces(GemoAdptSurfA,GemoAdptSurfB)&&HasCommonEdge(GemoAdptSurfA,GemoAdptSurfB))
        {
            cout<<"Two cylinders with same geometries are merged."<<endl;
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
***********************************************************************/
TopoDS_Face McCadFuseSurface::FuseSurfaces()
{
    Standard_Real UMin1,UMax1,VMin1,VMax1;
    BRepTools::UVBounds(m_surfA,UMin1,UMax1,VMin1,VMax1);
    Standard_Real UMin2,UMax2,VMin2,VMax2;
    BRepTools::UVBounds(m_surfB,UMin2,UMax2,VMin2,VMax2);

//    cout<<UMax1<<" "<<UMin1<<endl;
//    cout<<VMax1<<" "<<VMin1<<endl;
//    cout<<UMax2<<" "<<UMin2<<endl;
//    cout<<VMax2<<" "<<VMin2<<endl;

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

//    if( Abs(UMin1-UMax2) < 1.0e-2 || ( UMin1 == 0 && Abs(UMax2-2*M_PI) < 1e-7 ))
//    {
//        UMin = UMin2;
//        UMax = UMax1;
//    }
//    else if( Abs(UMin2-UMax1) < 1.0e-2 || ( UMin2 == 0 && Abs(UMax1-2*M_PI) < 1e-7 ) )
//    {
//        UMin = UMin1;
//        UMax = UMax2;
//    }


    if(UMax1 > 2*M_PI)
    {
       UMax1 -= 2*M_PI;
    }
    if(UMax2 > 2*M_PI)
    {
       UMax2 -= 2*M_PI;
    }
    if(UMin1 > 2*M_PI)
    {
       UMin1 -= 2*M_PI;
    }
    if(UMin2 > 2*M_PI)
    {
       UMin2 -= 2*M_PI;
    }

    if( Abs(UMin1-UMax2) < Abs(UMin2-UMax1))
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

    if(isClose)
    {
        UMin = 0;
        UMax = 2*M_PI;
    }

    (VMin1 <= VMin2) ? VMin = VMin1 : VMin = VMin2;
    (VMax1 >= VMax2) ? VMax = VMax1 : VMax = VMax2;

    TopLoc_Location loc;
    const Handle(Geom_Surface)& aS1 = BRep_Tool::Surface(m_surfA,loc);

    TopoDS_Face face  = BRepBuilderAPI_MakeFace(aS1, UMin,UMax, VMin, VMax, 1.e-7).Face();

    Standard_Real UMin3,UMax3,VMin3,VMax3;
    BRepTools::UVBounds(face,UMin3,UMax3,VMin3,VMax3);

    return face;

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
TopoDS_Wire McCadFuseSurface::MakeWire(vector<TopoDS_Edge> &edge_list)
{
    BRepBuilderAPI_MakeWire wireMaker;
    for(int i = 0; i < edge_list.size(); i++)
    {
        wireMaker.Add(edge_list.at(i));
    }

    TopoDS_Wire theWire = wireMaker.Wire();
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
bool McCadFuseSurface::Fuse()
{
    if (!CanbeFused())
    {
        return false;
    }

    McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();

    try
    {
        m_newSurf = FuseSurfaces();
    }
    catch(...)
    {
        msgr->Message("_#_McCadFuseSurface :: Boolean Fuse Operation crashed!!!\n",
                              McCadMessenger_WarningMsg);
        return false;
    }

    BRepCheck_Analyzer face_analyzer(m_newSurf, true);  // check validity of Result
    if(!face_analyzer.IsValid())
    {
        msgr->Message("_#_McCadFuseSurface :: The fused surface is not available\n",
                              McCadMessenger_WarningMsg);
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
