#include "McCadBndSurfCylinder.hxx"

#include <assert.h>

#include <Extrema_ExtPS.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <Bnd_Box2d.hxx>
#include <BRepTools.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>

#include <Geom_CylindricalSurface.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <Geom_Curve.hxx>

#include <STEPControl_Writer.hxx>

#include <Handle_TColgp_HSequenceOfPnt.hxx>

#include <TopAbs_Orientation.hxx>
#include <TopLoc_Location.hxx>
#include <TopOpeBRep_ShapeIntersector.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <IntTools_EdgeFace.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_CommonPrt.hxx>

//qiu add SALOME definition
#ifndef SALOME
#include <McCadMessenger_Singleton.hxx>
#endif

#include "McCadEdge.hxx"
#include <math.h>
//qiu #include <McCadGTOOL.hxx>

#include "../McCadTool/McCadMathTool.hxx"
#include "../McCadTool/McCadGeomTool.hxx"
#include "../McCadTool/McCadEvaluator.hxx"

McCadBndSurfCylinder::McCadBndSurfCylinder()
{
}

McCadBndSurfCylinder::McCadBndSurfCylinder(const TopoDS_Face &theFace):McCadBndSurface(theFace)
{
    m_SurfType = Cylinder;                          // Set the surface type
    m_bHasAstSplitSurfaces = Standard_False;        // Set the cylinder has not splitting surface
}

McCadBndSurfCylinder::~McCadBndSurfCylinder()
{
    // Free the edge list
//    for(int i = 0; i < m_SplitEdgeList.size(); i++)
//    {
//        McCadEdge *pEdge = m_SplitEdgeList.at(i);
//        m_SplitEdgeList.erase(m_SplitEdgeList.begin()+i);
//        delete pEdge;
//        pEdge = NULL;
//    }

//    m_SplitEdgeList.clear();
}



/** ***************************************************************************
* @brief  Calculate the position relationship between face and triangle
* @param  McCadTriangle *& triangle Input triangle
*         POSITION & ePosition      which side of the face the triangle locate
* @return Standard_Boolean          Triangle and face is collied or not
*
* @date 13/05/2015
* @modify 21/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadBndSurfCylinder::TriangleCollision(McCadTriangle *& triangle,
                                                         POSITION & eSide)
{
    Standard_Boolean bCollision = Standard_False; // The triangle is collied with face or not
    Bnd_Box bndBoxTri  = triangle->GetBndBox();

    if(bndBoxTri.IsOut(m_CylnBndBox))
    {
        eSide = POSITIVE;
        return bCollision;
    }

    Standard_Integer iPosPnt = 0;   // The number of vertex of triangle locating at positive side of face
    Standard_Integer iNegPnt = 0;   // The number of vertex of triangle locating at negative side of face

    Handle_TColgp_HSequenceOfPnt pnt_list = triangle->GetVexList();
    for (Standard_Integer i = 1; i <= pnt_list->Length(); i++)
    {
        // if the point is on the surface, do not calculate the position between them.
        gp_Pnt point = pnt_list->Value(i);
        if (IsPntOnSurf(point,1.0e-2))
        {            
            continue;
        }

        /* Distinguish which side does the point located.*/
        Standard_Real aVal = McCadEvaluator::Evaluate(m_AdpSurface, point);

        if (aVal > 1.0e-1)              // Point located on the positive side of face
        {            
            iPosPnt ++;
        }
        else if (aVal < -1.0e-1)        // Point located on the negative side of face
        {
            iNegPnt ++;
        }

        if (iPosPnt > 0 && iNegPnt > 0) // The points are on both sides of face
        {            
            bCollision = Standard_True;
            break;
        }
        else
        {
            continue;
        }
    }

    if (bCollision)
    {
        return bCollision;
    }

    /// When the vertex of triangle locate at the positive side the cylinder,
    /// it does not mean that the triangle has no collition with surface,
    /// the edges of triangle may collied with surface
    if (iPosPnt > 0 && iNegPnt == 0)
    {
        //eSide = NEGATIVE;
        bCollision = FurtherCollisionDetect(triangle,eSide,iPosPnt);
        //bCollision = BooleanCollisionDetect(triangle,eSide);
    }  
    else if (iNegPnt > 0 && iPosPnt == 0)   // The triangle on negative side of face
    {
        eSide = NEGATIVE;
    }
    else if(iNegPnt == 0 && iPosPnt == 0)
    {
        eSide = NEGATIVE;   // Need to be checked Lei Lu
    }

   // bCollision = Standard_False;
    return bCollision;
}



/** ******************************************************************************
* @brief Detect the collision between cylinder and triangles
*        Calculate the intersect points of the cylinder and edges of triangle
*        The compare the vertexes out the cylinder and the intersect points.
* @param McCadTriangle *& triangle     The triangle to be detected
*        POSITION & eSide              The position of triangle locate
*        Standard_Integer posPnt       How many vertexes on the outside of cylinder
*        Standard_Integer negPnt       How many vertexes on the inside of cylinder
*
* @return Standard_Boolean      The cylinder and triangle have collision or not
*
* @date 13/05/2015
* @modify 05/04/2016
* @author Lei Lu
*********************************************************************************/
Standard_Boolean McCadBndSurfCylinder::FurtherCollisionDetect(McCadTriangle *& triangle,
                                                              POSITION & eSide,
                                                              Standard_Integer posPnt)
{
    Standard_Boolean bCollision = Standard_False; // The triangle is collied with face or not

    int iNumVer = 0;
    // How many intersected pointed do the triangle and cylinder have.
    for (TopExp_Explorer ex(*triangle, TopAbs_EDGE); ex.More(); ex.Next())
    {
        TopoDS_Edge edge = TopoDS::Edge(ex.Current());
        IntTools_EdgeFace efInt;

        efInt.SetEdge(edge);
        efInt.SetFace(m_cylSurf);

        efInt.SetRange(0.0,1.0);
        efInt.Perform();

        IntTools_SequenceOfCommonPrts cprts;
        cprts = efInt.CommonParts();
        if(cprts.Length()!=0)
        {
            iNumVer += cprts.Length();
        }
    }

    //Judge the triangle is collied with cylinder or not
    Standard_Integer iNumVertex = triangle->GetVexList()->Length();
    if(posPnt == 2) // if two vertexes locate at the outside of cylinder
    {
        if(iNumVertex-posPnt < iNumVer)
        {
            bCollision = Standard_True;
            return bCollision;
        }
        else
        {
            eSide = POSITIVE;
            bCollision = Standard_False;
            return bCollision;
        }
    }
    else if(posPnt == 3) // if three vertexes locate at the outside of cylinder
    {
        if(iNumVertex-posPnt < iNumVer)
        {
            bCollision = Standard_True;
            return bCollision;
        }
        else
        {           
            bCollision = BooleanCollisionDetect(triangle,eSide);
        }
    }
    else // if one vertexes locate at the outside of cylinder
    {
        eSide = POSITIVE;
        bCollision = Standard_False;
       // bCollision = CollisionDetectOnePosPoint(triangle,eSide);
    }
    return bCollision;
}


/** ******************************************************************************
* @brief Detect the collision between cylinder and triangles if only one vertex
*        is located at the outside of cylinder,calculate the middle point of the
*        edge who has two vertexes on the surface. if the middle point is close
*        to cylinder and the distance is less than tolerence, the triangle is
*        thought has no collision of cylinder.
* @param McCadTriangle *& triangle     The triangle to be detected
*        POSITION & eSide              The position of triangle locate
*
* @return Standard_Boolean      The cylinder and triangle have collision or not
*
* @date 13/06/2016
* @modify 05/07/2016
* @author Lei Lu
*********************************************************************************/
Standard_Boolean McCadBndSurfCylinder::CollisionDetectOnePosPoint(McCadTriangle *& triangle,
                                                                  POSITION & eSide)
{
    Standard_Boolean bCollision = Standard_False; // The triangle is collied with face or not

    Handle_TColgp_HSequenceOfPnt pnt_list = triangle->GetVexList();

    vector<gp_Pnt> PntOnFaceList;
    for(int i = 1; i <= pnt_list->Length(); i++)
    {
        gp_Pnt point = pnt_list->Value(i);
        if (IsPntOnSurf(point,1.0e-2))
        {
            PntOnFaceList.push_back(point);
        }
    }

    if(PntOnFaceList.size() == 2)
    {
        gp_Pnt pntStart = PntOnFaceList.at(0);
        gp_Pnt pntEnd   = PntOnFaceList.at(1);

        gp_Pnt pntMid((pntStart.X()+pntEnd.X())/2.0,(pntStart.Y()+pntEnd.Y())/2.0,(pntStart.Z()+pntEnd.Z())/2.0);

        //calculate the distance between the middle point and cylinder.
        Standard_Real aVal = McCadEvaluator::Evaluate(m_AdpSurface, pntMid);

        if(aVal<m_radius/50.0) // if the distance is less than the radius/50. the triangle is located at the outside
        {
            eSide = POSITIVE;
            bCollision = Standard_False;
            return bCollision;
        }
    }

    return bCollision;
}


/** ******************************************************************************
* @brief Detect the collision between cylinder and triangles with further Boolean
*        operation, if there are intersected section between them, calculate the
*        position of intersected vertexes, the triangles could be located in, out
*        or on the cylinders.
* @param McCadTriangle *& triangle     The triangle to be detected
*        POSITION & eSide              The position of triangle locate
*
* @return Standard_Boolean      The cylinder and triangle have collision or not
*
* @date 13/05/2015
* @modify 05/04/2016
* @author Lei Lu
*********************************************************************************/
Standard_Boolean McCadBndSurfCylinder::BooleanCollisionDetect(McCadTriangle *& triangle,
                                                              POSITION & eSide)
{
    Standard_Boolean bCollision = Standard_False; // The triangle is collied with face or not

    int nPoint = 0; // The number of collied points between triangle and cylinder
    TopoDS_Shape section_shape;
    try             // Calculate the intersected section
    {
        BRepAlgoAPI_Section section(m_cylSurf, *triangle);
        section.ComputePCurveOn1(Standard_True);
        section.Approximation(Standard_True);
        section.Build();
        section_shape = section.Shape();
    }
    catch(...)
    {
#ifndef SALOME
        McCadMessenger_Singleton* msgr = McCadMessenger_Singleton::Instance();
        msgr->Message("_#_McCadBndSurfCylinder.cxx :: Section computering of triangle and surface is fail!!!",
                      McCadMessenger_WarningMsg);
#else
        cout <<"_#_McCadBndSurfCylinder.cxx :: Section computering of triangle and surface is fail!!!" <<endl;
#endif
        eSide = POSITIVE;
        bCollision = Standard_False;
        return bCollision;
    }

    // How many intersected pointed do the triangle and cylinder have.
    for (TopExp_Explorer ex(section_shape, TopAbs_VERTEX); ex.More(); ex.Next())
    {
        nPoint++;
    }  

    if (nPoint > 0)
    {
        bCollision = Standard_True;
    }

    return bCollision;
}


//    if(nPoint != 0)
//    {
//        TopoDS_Edge edge;
//        for (TopExp_Explorer ex(section_shape, TopAbs_EDGE); ex.More(); ex.Next())
//        {
//            edge = TopoDS::Edge(ex.Current());
//        }

//        Standard_Real fStart, fEnd;
//        Handle(Geom_Curve) theCurve = BRep_Tool::Curve(edge, fStart, fEnd);

//        GeomAdaptor_Curve gp_Curve(theCurve);
//        if(gp_Curve.GetType() == GeomAbs_Line)
//        {
//           eSide = POSITIVE;
//           bCollision = Standard_False;
//           return bCollision;
//        }

////        gp_Pnt pntMid;
////        Standard_Real fMid = (fStart+fEnd)/2.0;
////        theCurve->D0(fMid, pntMid);

//        gp_Pnt pntStart;
//        theCurve->D0(fStart, pntStart);
//        gp_Pnt pntEnd;
//        theCurve->D0(fEnd, pntEnd);

//        gp_Pnt pntLineMid((pntStart.X()+pntEnd.X())/2.0, (pntStart.Y()+pntEnd.Y())/2.0, (pntStart.Z()+pntEnd.Z())/2.0);

//        //Standard_Real len = pntLineMid.Distance(pntMid);

//        Standard_Real tol = m_radius/50.0;
//        if(IsPntOnSurf(pntLineMid,tol))
//        {
//            eSide = POSITIVE;
//            bCollision = Standard_False;
//            return bCollision;
//        }
//        else
//        {
//            bCollision = Standard_True;
//            return bCollision;
//        }

////        // if the intersected edge is too small and less than the radius too much, the triangle
////        // is not consided that has collision with cylinder.
////        if((len/m_radius) >= 0.05)
////        {
////            bCollision = Standard_True;
////            return bCollision;
////        }
////        else
////        {
////            eSide = POSITIVE;
////        }
//    }
//    else
//    {
//        eSide = POSITIVE;
//    }





/** ******************************************************************************
* @brief Set the length the generated a close extended cylinder for triangles
*        collision detecting
* @param Standard_Real length
* @return void
*
* @date 25/04/2016
* @modify
* @author Lei Lu
*********************************************************************************/
void McCadBndSurfCylinder::GenExtCylinder(Standard_Real length)
{
    m_CylnLength = length;

    TopLoc_Location loc;
    Handle(Geom_Surface) surface = BRep_Tool::Surface(*this,loc);
    //TopAbs_Orientation ori = this->Orientation();

    Standard_Real UMin,UMax, VMin, VMax;
    BRepTools::UVBounds(*this,UMin,UMax, VMin, VMax);

    m_Radian = Abs(UMax-UMin);   // Set the radian of cylinder

    UMin = 0.0;
    UMax = 2*M_PI;

    Standard_Real VMid = (VMin + VMax)/2.0;
    Standard_Real ext_length = m_CylnLength/2.0;

    VMin = VMid - ext_length;
    VMax = VMid + ext_length;

    m_cylSurf = BRepBuilderAPI_MakeFace(surface,UMin,UMax,VMin,VMax,1.e-7);

    gp_Cylinder gCyl = m_AdpSurface.Cylinder();
    m_radius = gCyl.Radius();                   // Get the radius of cylinder
    gp_Ax3 axis = gCyl.Position();              // Get the coordinate system Ax3
    m_Center = axis.Location();                 // Assign a point on the axis

    BRepBndLib::Add(m_cylSurf, m_CylnBndBox);
    m_CylnBndBox.SetGap(0.0);
}



/** ******************************************************************************
* @brief The two boundary cylinders can be fused or not, if they have same
*        geometries and a common edges, they can be fused.
* @param McCadBndSurface * pSurf
* @return Standard_Boolean
*
* @date 02/05/2016
* @modify
* @author Lei Lu
*********************************************************************************/
Standard_Boolean McCadBndSurfCylinder::CanFuseSurf(McCadBndSurface *& pSurf)
{
    if(!this->IsSameSurface(pSurf))
    {
       return Standard_False;
    }    

    /// Judge the surfaces have the common edge
    for(unsigned int i = 0; i < this->GetEdgeList().size(); i++)
    {
        McCadEdge *pEdgeA = GetEdgeList().at(i);

        for(unsigned int j = 0; j < pSurf->GetEdgeList().size(); j++)
        {
            McCadEdge *pEdgeB = pSurf->GetEdgeList().at(j);

            if(pEdgeA->IsSame(pEdgeB, 1.e-5))
            {
                return Standard_True;
            }
        }
    }

    return Standard_False;
}



/** ***************************************************************************
* @brief  Add the edge connect cylinders for adding assited split surfaces
* @param  McCadEdge *& pEdge
* @return void
*
* @date 02/06/2016
* @author  Lei Lu
******************************************************************************/
void McCadBndSurfCylinder::AddCylnCylnSplitEdge(McCadEdge *& pEdge)
{
    assert(pEdge);
    m_CylnCylnSplitEdgeList.push_back(pEdge);
}



/** ***************************************************************************
* @brief  Add the edge which connect plane and cylinder for adding assited
*         split surfaces
* @param  McCadEdge *& pEdge
* @return void
*
* @date 02/06/2016
* @author  Lei Lu
******************************************************************************/
void McCadBndSurfCylinder::AddCylnPlnSplitEdge(McCadEdge *& pEdge)
{
    assert(pEdge);
    m_CylnPlnSplitEdgeList.push_back(pEdge);
}




/** ******************************************************************************
* @brief Fuse two same cylinders and generate a new cylinder
* @param McCadBndSurface *& pSurf
* @return TopoDS_Face
*
* @date 02/05/2016
* @modify
* @author Lei Lu
*********************************************************************************/
TopoDS_Face McCadBndSurfCylinder::FuseSurfaces(McCadBndSurface *& pSurf)
{
    TopoDS_Face face = McCadGeomTool::FuseCylinders(*this, *pSurf);
    return face;
}





/** ******************************************************************************
* @brief The cylinder has assisted splitting surface or not
* @param
* @return Standard_Boolean
*
* @date 09/06/2016
* @modify
* @author Lei Lu
*********************************************************************************/
Standard_Boolean McCadBndSurfCylinder::HasAstSplitSurfaces()
{
    if(m_bHasAstSplitSurfaces)
    {
        return m_bHasAstSplitSurfaces;
    }
    else
    {
        if(m_CylnPlnSplitEdgeList.empty() && m_CylnCylnSplitEdgeList.empty())
        {
            m_bHasAstSplitSurfaces = Standard_False;
        }
        else
        {
            m_bHasAstSplitSurfaces = Standard_True;
        }
    }

    return m_bHasAstSplitSurfaces;
}



/** ******************************************************************************
* @brief The cylinder has assisted splitting surface for separating cylinder and
*        plane.
* @param
* @return Standard_Boolean
*
* @date 09/06/2016
* @modify
* @author Lei Lu
*********************************************************************************/
Standard_Boolean McCadBndSurfCylinder::HasCylnPlnSplitSurf()
{
    if(m_CylnPlnSplitEdgeList.empty() )
    {
        return Standard_False;
    }
    else
    {
        return Standard_True;
    }
}



/** ******************************************************************************
* @brief Get the splitting edge list, the edge can be used for add splitting
*        surface
* @param
* @return vector<McCadEdge*>
*
* @date 27/06/2016
* @modify
* @author Lei Lu
*********************************************************************************/
vector<McCadEdge*> McCadBndSurfCylinder::GetCylnPlnSplitEdgeList() const
{
    if(!m_CylnPlnSplitEdgeList.empty())
    {       
        return m_CylnPlnSplitEdgeList;
    }   
}



/** ******************************************************************************
* @brief Get the radian of cylinder
* @param
* @return Standard_Real
*
* @date 27/06/2016
* @modify
* @author Lei Lu
*********************************************************************************/
Standard_Real McCadBndSurfCylinder::GetRadian() const
{
    return m_Radian;
}


/** ******************************************************************************
* @brief Get the center of cylindr which is one point on the axis
* @param
* @return gp_Pnt
*
* @date 27/06/2016
* @modify
* @author Lei Lu
*********************************************************************************/
gp_Pnt McCadBndSurfCylinder::GetCenter() const
{
    return m_Center;
}



/** ***************************************************************************
* @brief  Calculate the distance between point and surface, if it less than
*         distance tolerence, the point is consided on the surface.
* @param  point(gp_Pnt), tolerence(Standard_Real)
* @return
*
* @date 08/10/2015
* @modify 08/10/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadBndSurfCylinder::IsPntOnSurf(gp_Pnt &thePnt, Standard_Real disTol)
{
    BRepAdaptor_Surface BS(m_cylSurf, Standard_True);
    Standard_Real uvTol = BS.Tolerance();

    Standard_Real fu = BS.FirstUParameter();
    Standard_Real lu = BS.LastUParameter();
    Standard_Real fv = BS.FirstVParameter();
    Standard_Real lv = BS.LastVParameter();

    Extrema_ExtPS extPS(thePnt,BS,fu,lu,fv,lv,uvTol,uvTol);
    if(extPS.IsDone() && extPS.NbExt() != 0)
    {
        gp_Pnt pnt  = thePnt;
        gp_Pnt pntSurf = extPS.Point(1).Value(); // The nearest point on the surface
        // Calculate the distance between surface and point
        Standard_Real dis = Sqrt(pow(pnt.X()-pntSurf.X(),2)+pow(pntSurf.Y()-pnt.Y(),2)+pow(pnt.Z()-pntSurf.Z(),2));

        if(dis < disTol)
        {
            return Standard_True;
        }
        else
        {
            return Standard_False;
        }
    }

    return Standard_False;
}


//vector<McCadEdge*> McCadBndSurfCylinder::GetEdgeList()
//{

//    STEPControl_Writer wrt;
//    if(m_EdgeList.empty())
//    {
//        for(TopExp_Explorer ex(*this, TopAbs_EDGE); ex.More(); ex.Next())
//        {
//            TopoDS_Edge edge = TopoDS::Edge(ex.Current());

//            Standard_Real fStartB, fEndB;
//            Handle(Geom_Curve) theCurve = BRep_Tool::Curve(edge, fStartB, fEndB);

//            McCadEdge *pEdge = NULL;

//            switch (GeomAdaptor_Curve(theCurve).GetType())
//            {
//                case GeomAbs_Line:
//                {
//                    pEdge = new McCadEdgeLine(edge);
//                    break;
//                }
//                case GeomAbs_Circle:
//                {
//                    pEdge = new McCadEdgeCircle(edge);
//                    break;
//                }
//                case GeomAbs_Ellipse:
//                {
//                    pEdge = new McCadEdgeEllipse(edge);
//                    break;
//                }
//                default:
//                    pEdge = new McCadEdgeSpline(edge);
//                    break;
//            }



//            if (pEdge->GetType() == Line)
//            {
//                Standard_Real UMin,UMax,VMin,VMax;
//                Standard_Real UMin1,UMax1,VMin1,VMax1;
//                BRepTools::UVBounds(*this,edge,UMin,UMax,VMin,VMax);
//                BRepTools::UVBounds(*this,UMin1,UMax1,VMin1,VMax1);

//                if( Abs(UMin - UMin1) < 1.0e-7 || Abs(UMin-UMax1) < 1.0e-7)
//                {
//                    if( Abs(VMin - VMin1) < 1.0e-7 || Abs(VMax-VMax1) < 1.0e-7)
//                    {
//                        cout<<"UMin  "<<UMin<<"   UMin1   "<<UMin1<<endl;
//                        cout<<"UMax  "<<UMax<<"   UMax1   "<<UMax1<<endl;

//                        cout<<"VMin  "<<VMin<<"   VMin1   "<<VMin1<<endl;
//                        cout<<"VMax  "<<VMax<<"   VMax1   "<<VMax1<<endl;

//                        pEdge->SetSplit(1);

//                        wrt.Transfer(edge, STEPControl_AsIs);
//                    }
//                }
//            }

//            m_EdgeList.push_back(pEdge);
//        }

//        wrt.Write("Edges.stp");
//    }

//    return m_EdgeList;
//}

