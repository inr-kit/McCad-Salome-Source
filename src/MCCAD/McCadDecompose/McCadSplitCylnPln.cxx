#include "McCadSplitCylnPln.hxx"

#include <assert.h>

#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>

#include <STEPControl_Writer.hxx>

#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopExp_Explorer.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>

#include "../McCadTool/McCadMathTool.hxx"
#include "../McCadTool/McCadGeomTool.hxx"


McCadSplitCylnPln::McCadSplitCylnPln()
{

}

McCadSplitCylnPln::~McCadSplitCylnPln()
{

}



/** ***************************************************************************
* @brief  Generate the assisted surfaces and add them to the assisted splitting
*         surface list
* @param  McCadBndSurface input cylinder face, assisted splitting face list
* @return void
*
* @date 13/07/2015
* @modify 13/07/2015
* @author  Lei Lu
******************************************************************************/
void McCadSplitCylnPln::GenSplitSurfaces( McCadDcompSolid *& pSolid)
{
    vector<McCadBndSurface *>  &cylinder_list = pSolid->m_CylinderList;
    vector<McCadBndSurface *>  &plane_list    = pSolid->m_PlaneList;
    vector<McCadAstSurface*>   &AstFaceList   = pSolid->m_AstFaceList;

    m_fLength = pSolid->m_fBoxSqLength; // Set the length of created splitting surface

    for(int i = 0; i < cylinder_list.size(); i++ )
    {
        // The cylinder has common edge with planes or not
        Standard_Boolean bHasStraightEdge = Standard_False;

        McCadBndSurfCylinder *pBndCyln = (McCadBndSurfCylinder*)cylinder_list.at(i);
        ///< if the cylinder is not splitting surface or it is a close cylinder
        ///< need not add assisted splitting surfaces
        if(pBndCyln->GetSplitSurfNum() == 0 || pBndCyln->GetRadian() >= M_PI*2)
        {
            continue;
        }
        ///< if the cylinder is concave, use different streagy to add splitting surface
        if(pBndCyln->Orientation() == TopAbs_REVERSED )
        {
            continue;
        }
        else if(pBndCyln->Orientation() == TopAbs_FORWARD)
        {
            ///< if the cylinder has a common straight edge with a plane, add a plane through
            ///< the edge and axis of cylinder as assisted splitting surface.
            for(int j = 0; j < plane_list.size(); j++ )
            {
                McCadBndSurface *pBndPln = plane_list.at(j);

                if(FindComLineEdge(pBndCyln,pBndPln))
                {
                    bHasStraightEdge = Standard_True;
                }
            }
        }

        /** If the cylinder does not connect with other cylinders or planes with a straight edge
            but it is still a splitting surface, then add assisted splitting all the same.*/
        //TopoDS_Face theCyln = (TopoDS_Face)(*pBndCyln);
        //if (!HasStraightEdge(theCyln))// Lei Lu modification
        if (!bHasStraightEdge)
        {
            // GenAssistSurfaces(pBndCyln,AstFace_list);
            // pBndCyln->AddAstSurfaces(Standard_True);
        }
    }

    for(int i = 0; i < cylinder_list.size(); i++ )
    {
        McCadBndSurfCylinder *pBndCyln = (McCadBndSurfCylinder*)cylinder_list.at(i);
        if(pBndCyln->HasCylnPlnSplitSurf())
        {
            CrtSplitSurfaces(pBndCyln,AstFaceList);
        }
    }

    // Merge the generated assisted splitting surfaces
    if(AstFaceList.size() >= 2)
    {
        MergeSurfaces(AstFaceList);
    }
}




/** ***************************************************************************
* @brief  Combine the splitting surfaces to avoid the generation of thin plate
* @param
* @return
*
* @date 13/07/2015
* @modify 13/07/2015
* @author  Lei Lu
******************************************************************************/
void McCadSplitCylnPln::MergeSurfaces(vector<McCadAstSurface*> & theAstFaceList)
{
    ///< Merge the assisted splitting surfaces
    for(int i = 0; i < theAstFaceList.size()-1; i++ )
    {
        for(int j = i+1; j < theAstFaceList.size(); j++ )
        {
//qiu            McCadAstSurfPlane *pFirFace = theAstFaceList.at(i);
            McCadAstSurfPlane *pFirFace = static_cast<McCadAstSurfPlane *>(theAstFaceList.at(i));
//qiu            McCadAstSurfPlane *pSecFace = theAstFaceList.at(j);
            McCadAstSurfPlane *pSecFace = static_cast<McCadAstSurfPlane *>(theAstFaceList.at(j));

            if(pFirFace->IsSameSurface(pSecFace))
            {
                theAstFaceList.erase(theAstFaceList.begin()+j);
                pFirFace->SetCombined(Standard_True);

                delete pSecFace;
                pSecFace = NULL;

                j--;

                continue;
            }
        }
    }
}



/** ***************************************************************************
* @brief Create the assisted splitting surface from a cylinder which is
*        connected with one or two planes
*
* @param McCadBndSurfCylinder *& pCylnFace,
         vector<McCadAstSurface*> & astFace_list  >> The generated splitting
         surface is added into this face list.
* @return void
*
* @date 13/06/2016
* @modify 13/06/2016
* @author  Lei Lu
******************************************************************************/
void McCadSplitCylnPln::CrtSplitSurfaces(McCadBndSurfCylinder *& pCylnFace,
                                         vector<McCadAstSurface*> & astFace_list)
{
    int iEdgeNum = pCylnFace->GetCylnPlnSplitEdgeList().size();
    if (iEdgeNum  == 1)
    {
         McCadEdge *pEdge = pCylnFace->GetCylnPlnSplitEdgeList().at(0);
         McCadAstSurfPlane *pAstSplitSurf = CrtSplitSurfThroughEdge(pCylnFace,pEdge);
         astFace_list.push_back(pAstSplitSurf);
    }
    else if (iEdgeNum == 2)
    {
        Standard_Real radian = pCylnFace->GetRadian();

        /// if the radian of cylinder is smaller than 180 degree, use two splitting surfaces
        /// to split them
        if (radian < M_PI)
        {
            for(unsigned int i = 0 ; i < pCylnFace->GetCylnPlnSplitEdgeList().size(); i++)
            {
                McCadEdge *pEdge = pCylnFace->GetCylnPlnSplitEdgeList().at(i);
                McCadAstSurfPlane *pAstSplitSurf = CrtSplitSurfThroughEdge(pCylnFace,pEdge);
                astFace_list.push_back(pAstSplitSurf);
            }
        }
        else /// if the radian of cylinder is lager than 120 degree, use one splitting
             /// surfaces which connect two edges
        {
            McCadEdge *pEdgeA = pCylnFace->GetCylnPlnSplitEdgeList().at(0);
            McCadEdge *pEdgeB = pCylnFace->GetCylnPlnSplitEdgeList().at(1);

            McCadAstSurfPlane* pAstSplitSurf = CrtSplitSurfThroughTwoEdges(pEdgeA,pEdgeB);
            astFace_list.push_back(pAstSplitSurf);
        }
    }
}



/** ***************************************************************************
* @brief Generate a splitting surface through one edge.
*
* @param McCadBndSurfCylinder *& pCylnFace,
         McCadEdge *& pEdge
* @return McCadAstSurfPlane *     >> Generated assisted splittin surface
*
* @date 13/06/2016
* @modify 13/06/2016
* @author  Lei Lu
******************************************************************************/
McCadAstSurfPlane* McCadSplitCylnPln::CrtSplitSurfThroughEdge(McCadBndSurfCylinder *& pCylnFace,
                                                              McCadEdge *& pEdge)
{
    BRepTools::Update(*pCylnFace);
    gp_Pnt center = pCylnFace->GetCenter();                 // Assign a point on the axis

    gp_Pnt posStart = pEdge->StartPoint();
    gp_Pnt posEnd = pEdge->EndPoint();

    gp_Vec vec1(center, posStart), vec2(center, posEnd);
    gp_Vec vec = vec1 ^ vec2;
    vec.Normalize();
    gp_Dir dir(vec);

    Standard_Real size = m_fLength;
    gp_Pln pln(posStart, dir);
    TopoDS_Face surf = BRepBuilderAPI_MakeFace(pln,-size,size,-size,size).Face();

    McCadAstSurfPlane *pAstSurfA = new McCadAstSurfPlane(surf);
    pAstSurfA->SetEdge(*pEdge); // Set the edge throught by assisted splitting surface

    return pAstSurfA;
}




/** ***************************************************************************
* @brief Generate assisted splitting surface through two edges
*
* @param McCadEdge *& pEdgeA, McCadEdge *& pEdgeB
* @return McCadAstSurfPlane *     >> Generated assisted splittin surface
*
* @date 13/06/2016
* @modify 13/06/2016
* @author  Lei Lu
******************************************************************************/
McCadAstSurfPlane* McCadSplitCylnPln::CrtSplitSurfThroughTwoEdges(McCadEdge *& pEdgeA, McCadEdge *& pEdgeB)
{
    /// Get the start and end points of the curves, and calculate middle points
    gp_Pnt pntStartB = pEdgeB->StartPoint();
    gp_Pnt pntEndB = pEdgeB->EndPoint();

    gp_Pnt pntMidA = pEdgeA->MidPoint();

    /// Calculate the normal vector or plane through the two edges (three points)
    gp_Vec vec1(pntStartB, pntMidA), vec2(pntEndB, pntMidA);
    gp_Vec vec = vec1 ^ vec2;
    vec.Normalize();
    gp_Dir dir(vec);

    gp_Pln pln(pntMidA, dir);

    Standard_Real size = m_fLength;
    /// Generate the surface according to the normal vector
    TopoDS_Face face = BRepBuilderAPI_MakeFace(pln,-size,size,-size,size).Face();
    McCadAstSurfPlane *pSurf = new McCadAstSurfPlane(face);
    pSurf->SetCombined(Standard_True);

    return pSurf;
}



/** ***************************************************************************
* @brief Calculate the radian of curved surface
* @param McCadBndSurface *& pBndCyln
* @return Standard_Real the radian
*
* @date 12/04/2016
* @modify
* @author  Lei Lu
******************************************************************************/
Standard_Real McCadSplitCylnPln::CalCurveRadian(McCadBndSurface *& pBndCyln)
{
    Standard_Real UMin,UMax,VMin,VMax;
    BRepTools::UVBounds(*pBndCyln,UMin,UMax,VMin,VMax);

    McCadMathTool::ZeroValue(UMin,1.e-7);
    McCadMathTool::ZeroValue(UMax,1.e-7);

    Standard_Real radian = fmod(Abs(UMax-UMin),2*M_PI);

    cout<<"Radian  "<<radian<<endl;
    return radian;
}



/** ***************************************************************************
* @brief  Judge the input two surfaces have common connected edges or not
* @param  McCadBndSurface * pSurfA,
          McCadBndSurface * pSurfB,
          McCadEdge *& pEdge
* @return Standard_Boolean
*
* @date 13/07/2015
* @modify 13/07/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadSplitCylnPln::FindComLineEdge(McCadBndSurfCylinder *& pSurfCyln,
                                        McCadBndSurface *& pSurfPln)
{
    /// Judge the surfaces have the common straight edge
    for(unsigned i = 0; i < pSurfCyln->GetEdgeList().size(); i++)
    {
        McCadEdge *pEdgeA = pSurfCyln->GetEdgeList().at(i);
        if(pEdgeA->GetType() != Line || !pEdgeA->CanAddAstSplitSurf())
        {
            continue;
        }

        for(unsigned j = 0; j < pSurfPln->GetEdgeList().size(); j++)
        {
            McCadEdge *pEdgeB = pSurfPln->GetEdgeList().at(j);
            if(pEdgeB->GetType() != Line )
            {
                continue;
            }

            if (pEdgeA->IsSame(pEdgeB,1.0e-5))
            {
                /// Get the first vertex of edge
                gp_Pnt pntStart = pEdgeA->StartPoint();

                /// Get the normals of each surface
                gp_Dir normalA = McCadGeomTool::NormalOnFace(*pSurfCyln,pntStart);
                gp_Dir normalB = McCadGeomTool::NormalOnFace(*pSurfPln,pntStart);

                Standard_Real angle = normalA.Angle(normalB);

                if(angle < 1.0e-4)
                {
                    pEdgeA->SetConvexity(0);
                    pEdgeB->SetConvexity(0);
                }          

                if(angle >= 0.5*M_PI && angle <= M_PI)
                {
                    continue;
                }
                else
                {                   
                    pSurfCyln->AddCylnPlnSplitEdge(pEdgeA);
                   //pSurfCyln->SetAstSurfaces(Standard_True);
                }                
                return Standard_True;
            }
        }
    }

    return Standard_False;
}




//** ***************************************************************************
//* @brief  Judge the input surface has staight edges or not
//* @param  TopoDS_Face theFace
//* @return Standard_Boolean
//*
//* @date 13/07/2015
//* @modify 13/07/2015
//* @author  Lei Lu
//******************************************************************************/
//Standard_Boolean McCadSplitCylnPln::HasStraightEdge(TopoDS_Face theFace)
//{
//    for(TopExp_Explorer ex1(theFace, TopAbs_EDGE); ex1.More(); ex1.Next())
//    {
//        TopoDS_Edge edge = TopoDS::Edge(ex1.Current());
//        Standard_Real fStart, fEnd;
//        Handle(Geom_Curve) theCurve = BRep_Tool::Curve(edge, fStart, fEnd);
//        if(GeomAdaptor_Curve(theCurve).GetType() == GeomAbs_Line)
//        {
//            return Standard_True;
//        }
//    }
//    return Standard_False;
//}





//** ***************************************************************************
//* @brief  Judge the input two surfaces have common connected edges or not
//* @param  TopoDS_Face &theFaceA,  input surface A
//          TopoDS_Face &theFaceB,  input surface B
//          TopoDS_Edge &edge       return common edge
//* @return Standard_Boolean
//*
//* @date 13/07/2015
//* @modify 13/07/2015
//* @author  Lei Lu
//******************************************************************************/
//Standard_Boolean McCadSplitCylnPln::HasCommonEdge(  TopoDS_Face &theFaceA,
//                                                    TopoDS_Face &theFaceB,
//                                                    TopoDS_Edge &edge)
//{
//    TopLoc_Location loc;
//    Handle_Geom_Surface hGeomA = BRep_Tool::Surface(theFaceA,loc);
//    GeomAdaptor_Surface GemoAdptSurfA(hGeomA);

//    Handle_Geom_Surface hGeomB = BRep_Tool::Surface(theFaceB,loc);
//    GeomAdaptor_Surface GemoAdptSurfB(hGeomB);

//    if( GemoAdptSurfA.GetType() == GeomAbs_Plane
//            && GemoAdptSurfB.GetType() == GeomAbs_Plane)
//    {
//        return Standard_False;
//    }

//    /// Judge the surfaces have the common edge
//    for(TopExp_Explorer ex1(theFaceA, TopAbs_EDGE); ex1.More(); ex1.Next())
//    {
//        TopoDS_Edge e1 = TopoDS::Edge(ex1.Current());
//        for(TopExp_Explorer ex2(theFaceB, TopAbs_EDGE); ex2.More(); ex2.Next())
//        {
//            TopoDS_Edge e2 = TopoDS::Edge(ex2.Current());
//            if (McCadGeomTool::IsSameStraightEdge(e1, e2, 0.5))
//            {
//                edge = e1;
//                return Standard_True;
//            }
//        }
//    }

//    return Standard_False;
//}



//    TopTools_IndexedDataMapOfShapeListOfShape mapEdgeFace;
//    TopExp::MapShapesAndAncestors(m_Solid,TopAbs_EDGE,TopAbs_FACE, mapEdgeFace);
//    Standard_Integer nbEdge = mapEdgeFace.Extent();
//    TopTools_ListOfShape listFace;
//    //TopTools_MapOfShape exMapEdge;

//    ///////////////////////////////////////////////////////////////////////////////////
//    cout << " Nb of F = " << nbEdge << endl;

//    for (int iKey = 1; iKey <= nbEdge; iKey++)
//    {
//        TopoDS_Edge edge =TopoDS::Edge(mapEdgeFace.FindKey(iKey));
//        BRepAdaptor_Curve adaptorCurve;
//        adaptorCurve.Initialize(edge);
//        if (adaptorCurve.GetType() != GeomAbs_Line)
//        {
//            continue;
//        }

//        listFace = mapEdgeFace.FindFromKey(mapEdgeFace.FindKey(iKey));
//        TopTools_ListIteratorOfListOfShape iterFace(listFace);

//        //for(; iterFace.More(); iterFace.Next())
//        //{

//        cout<<"ListFace"<<listFace.Extent()<<endl;
//        if(listFace.Extent() != 2)
//        {
//            continue;
//        }

//        TopoDS_Face FaceA = TopoDS::Face(iterFace.Value());
//        iterFace.Next();
//        TopoDS_Face FaceB = TopoDS::Face(iterFace.Value());

//        BRepAdaptor_Surface BAFaceA(FaceA,Standard_True);
//        GeomAdaptor_Surface GAFaceA = BAFaceA.Surface();

//        BRepAdaptor_Surface BAFaceB(FaceB,Standard_True);
//        GeomAdaptor_Surface GAFaceB = BAFaceB.Surface();

//        if ((GAFaceA.GetType() == GeomAbs_Plane && GAFaceB.GetType() == GeomAbs_Cylinder)
//             ||(GAFaceB.GetType() == GeomAbs_Plane && GAFaceA.GetType() == GeomAbs_Cylinder))
//        {
//            cout<<"yes"<<endl;
//        }
//    }


///** ***************************************************************************
//* @brief  Generate two splitting planes according to the cylinder and
//*         add them into the face list
//* @param  McCadBndSurface input cylinder face, assisted splitting face list
//* @return void
//*
//* @date 13/07/2015
//* @modify 13/07/2015
//* @author  Lei Lu
//******************************************************************************/
//void McCadSplitCylnPln::GenSurfThroughEdge(McCadBndSurfCylinder *& pCylnFace,
//                                           TopoDS_Edge & theEdge,
//                                           vector<McCadAstSurfPlane*> *& AstFace_list)
//{
//    McCadBndSurface *pFace = NULL;
//    pFace = pCylnFace;
//    BRepTools::Update(*pFace);

//    TopLoc_Location loc;
//    Handle(Geom_Surface) surface = BRep_Tool::Surface(*pFace,loc);

//    BRepAdaptor_Surface BS(*pFace, Standard_True);
//    GeomAdaptor_Surface AdpSurf = BS.Surface();

//    gp_Cylinder cyln = AdpSurf.Cylinder();           // Get the geometry of cylinder
//    gp_Ax3 axis = cyln.Position();                   // Get the coordinate system Ax3
//    gp_Pnt center = axis.Location();                 // Assign a point on the axis

//    Standard_Real size = 10000;

//    /// Get the first vertex of edge
//    TopoDS_Vertex vertexStart = TopExp::FirstVertex(theEdge,0);
//    gp_Pnt posStart = BRep_Tool::Pnt(vertexStart);

//    /// Get the second vertex of edge
//    TopoDS_Vertex vertexEnd = TopExp::LastVertex(theEdge,0);
//    gp_Pnt posEnd = BRep_Tool::Pnt(vertexEnd);

//    gp_Vec vec1(center, posStart), vec2(center, posEnd);
//    gp_Vec vec = vec1 ^ vec2;
//    vec.Normalize();
//    gp_Dir dir(vec);

//    gp_Pln pln(posStart, dir);
//    TopoDS_Face surf = BRepBuilderAPI_MakeFace(pln,-size,size,-size,size).Face();

//    McCadAstSurfPlane *pAstSurfA = new McCadAstSurfPlane(surf);
//    pAstSurfA->SetEdge(theEdge); // Set the edge throught by assisted splitting surface
//    AstFace_list->push_back(pAstSurfA);
//}





///** ***************************************************************************
//* @brief  If the cylinder does not connect with a plane, but it is a splitting
//*         surface, then generate two splitting surfaces through the two straight
//*         edges of input cylinder directly.
//* @param  McCadBndSurface input cylinder face, assisted splitting face list
//* @return void
//*
//* @date 13/07/2015
//* @modify 13/07/2015
//* @author  Lei Lu
//******************************************************************************/
//void McCadSplitCylnPln::GenAssistSurfaces(McCadBndSurfCylinder *& pCylnFace,
//                                          vector<McCadAstSurfPlane*> *& AstFace_list)
//{
//    McCadBndSurface *pFace = NULL;
//    pFace = pCylnFace;
//    BRepTools::Update(*pFace);

//    TopLoc_Location loc;
//    Handle(Geom_Surface) surface = BRep_Tool::Surface(*pFace,loc);

//    BRepAdaptor_Surface BS(*pFace, Standard_True);
//    GeomAdaptor_Surface AdpSurf = BS.Surface();

//    gp_Cylinder cyln = AdpSurf.Cylinder();           // Get the geometry of cylinder
//    gp_Ax3 axis = cyln.Position();                   // Get the coordinate system Ax3
//    gp_Pnt center = axis.Location();                 // Assign a point on the axis

//    Standard_Real UMin,UMax,VMin,VMax;
//    BRepTools::UVBounds(*pFace,UMin,UMax,VMin,VMax);

//    McCadMathTool::ZeroValue(UMin,1.e-7);
//    McCadMathTool::ZeroValue(UMax,1.e-7);
//    McCadMathTool::ZeroValue(VMin,1.e-7);
//    McCadMathTool::ZeroValue(VMax,1.e-7);

//    gp_Pnt p1,p2,p3,p4;

//    AdpSurf.D0(UMin, VMin, p1);
//    AdpSurf.D0(UMin, VMax, p2);

//    AdpSurf.D0(UMax, VMin, p3);
//    AdpSurf.D0(UMax, VMax, p4);

//    gp_Vec vec1(center, p1), vec2(center, p2);
//    gp_Vec vecA = vec1 ^ vec2;
//    vecA.Normalize();
//    gp_Dir dirA(vecA);

//    gp_Vec vec3(center, p3), vec4(center, p4);
//    gp_Vec vecB = vec3 ^ vec4;
//    vecB.Normalize();
//    gp_Dir dirB(vecB);

//    Standard_Real size = 10000;

//    gp_Pln plnA(p1, dirA);
//    TopoDS_Face surfA = BRepBuilderAPI_MakeFace(plnA,-size,size,-size,size).Face();

//    gp_Pln plnB(p3, dirB);
//    TopoDS_Face surfB = BRepBuilderAPI_MakeFace(plnB,-size,size,-size,size).Face();

//    McCadAstSurfPlane *pAstSurfA = new McCadAstSurfPlane(surfA);
//    TopoDS_Edge EdgeA = BRepBuilderAPI_MakeEdge(p1,p2).Edge();
//    pAstSurfA->SetEdge(EdgeA); // Set the edge throught by assisted splitting surface
//    AstFace_list->push_back(pAstSurfA);

//    McCadAstSurfPlane *pAstSurfB = new McCadAstSurfPlane(surfB);
//    TopoDS_Edge EdgeB = BRepBuilderAPI_MakeEdge(p3,p4).Edge();
//    pAstSurfB->SetEdge(EdgeB); // Set the edge throught by assisted splitting surface
//    AstFace_list->push_back(pAstSurfB);
//}
