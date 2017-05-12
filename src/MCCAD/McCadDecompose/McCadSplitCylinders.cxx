#include "McCadSplitCylinders.hxx"

#include <TopTools_HSequenceOfShape.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>

#include <gp_Pnt.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <gp_Elips.hxx>

#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>

#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepCheck_Analyzer.hxx>

#include <STEPControl_Writer.hxx>
#include <Handle_Geom_Surface.hxx>

#include "../McCadTool/McCadGeomTool.hxx"

McCadSplitCylinders::McCadSplitCylinders()
{

}

McCadSplitCylinders::~McCadSplitCylinders()
{

}


/** ***************************************************************************
* @brief  If the two cylinders have common straight edge or not
* @param  McCadBndSurface * pSurfA,
          McCadBndSurface * pSurfB,
          Handle_TopTools_HSequenceOfShape edge_list  The common edges
* @return Standard_Boolean
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadSplitCylinders::HasComLineEdge(McCadBndSurfCylinder *& pSurfA,
                                                     McCadBndSurfCylinder *& pSurfB,
                                                     vector<McCadEdge*> *& pEdgeList)
{
    /// Judge the surfaces have the common straight edge
    for(unsigned i = 0; i < pSurfA->GetEdgeList().size(); i++)
    {
        McCadEdge *pEdgeA = pSurfA->GetEdgeList().at(i);
        if(pEdgeA->GetType() != Line || !pEdgeA->CanAddAstSplitSurf())
        {
            continue;
        }

        for(unsigned j = 0; j < pSurfB->GetEdgeList().size(); j++)
        {
            McCadEdge *pEdgeB = pSurfB->GetEdgeList().at(j);
            if(pEdgeB->GetType() != Line || !pEdgeB->CanAddAstSplitSurf())
            {
                continue;
            }

            if (pEdgeA->IsSame(pEdgeB,1.0e-5))
            {
                pEdgeA->SetConvexity(0);    /// Set the convexities of edgeA and edgeB
                pEdgeB->SetConvexity(0);

                pEdgeList->push_back(pEdgeA);
            }
        }
    }

    if (!pEdgeList->empty())
    {
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }

}



/** ***************************************************************************
* @brief  If the two cylinders have common curved edge(Ellipse circle) or not
* @param  McCadBndSurface * pSurfA,
          McCadBndSurface * pSurfB,
          McCadEdge *& pEdge
* @return Bool
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadSplitCylinders::HasComCurvEdge(McCadBndSurfCylinder *&pSurfA,
                                                     McCadBndSurfCylinder *&pSurfB,
                                                     McCadEdge *& pEdge)
{
    for(unsigned i = 0; i < pSurfA->GetEdgeList().size(); i++)
    {
        McCadEdge *pEdgeA = pSurfA->GetEdgeList().at(i);
        if(pEdgeA->GetType() != ellipse)
        {
            continue;
        }

        for(unsigned j = 0; j < pSurfB->GetEdgeList().size(); j++)
        {
            McCadEdge *pEdgeB = pSurfB->GetEdgeList().at(j);
            if(pEdgeA->GetType() != ellipse)
            {
                continue;
            }

            if (pEdgeA->IsSame(pEdgeB,1.0e-5))
            {
                pEdgeA->SetConvexity(0);  // Set the convexity of two edges
                pEdgeB->SetConvexity(0);
                pEdge = pEdgeA;
                return Standard_True;
            }
        }
    }
    return Standard_False;
}




/** ***************************************************************************
* @brief  Generate a splitting surface through common edge
* @param  TopoDS_Face theFaceA & theFaceB, common edge
* @return Bool
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
McCadAstSurfPlane * McCadSplitCylinders::GenSurfThroughEdge(TopoDS_Face &theFaceA,
                                                            TopoDS_Face &theFaceB,
                                                            McCadEdge *& pEdge)
{
    gp_Pnt pntStart = pEdge->StartPoint();  /// Get the start vertex of edge
    gp_Pnt pntEnd   = pEdge->EndPoint();    /// Get the end vertex of edge

    /// Get the normals of each surface
    gp_Dir normalA = McCadGeomTool::NormalOnFace(theFaceA,pntStart);
    gp_Dir normalB = McCadGeomTool::NormalOnFace(theFaceB,pntStart);

    /// Get the middle normal of two cylinders
    gp_Dir normalMid(0,0,1);
    normalMid.SetX((normalA.X()+normalB.X())/2.0);
    normalMid.SetY((normalA.Y()+normalB.Y())/2.0);
    normalMid.SetZ((normalA.Z()+normalB.Z())/2.0);

    gp_Vec vec(pntStart,pntEnd);
    vec.Normalize();
    gp_Dir normalAB(vec);
    gp_Dir normalSplitSurf = normalAB.Crossed(normalMid); // the normal of splitting plane

    gp_Pln pln(pntStart, normalSplitSurf);

    Standard_Real size = m_fLength;
    TopoDS_Face split_face = BRepBuilderAPI_MakeFace(pln,-size,size,-size,size).Face();

    McCadAstSurfPlane *pAstFace = new McCadAstSurfPlane(split_face);
    pAstFace->SetEdge(*pEdge); // Set the edge throught by assisted splitting surface

    return pAstFace;
}




/** ***************************************************************************
* @brief  Generate a splitting surface through two paralle edges
* @param  McCadEdge *& pEdgeA,McCadEdge *& pEdgeB
* @return Bool
*
* @date 08/04/2016
* @modify 08/04/2015
* @author  Lei Lu
******************************************************************************/
McCadAstSurfPlane * McCadSplitCylinders::GenSurfThroughTwoEdges( McCadEdgeLine *& pEdgeA,
                                                                 McCadEdgeLine *& pEdgeB)
{
    /// Get the start and end points of the curves, and calculate middle points
    gp_Pnt pntMidA = pEdgeA->MidPoint();
    gp_Pnt pntStartB = pEdgeB->StartPoint();
    gp_Pnt pntEndB = pEdgeB->EndPoint();

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
    pSurf->SetEdge(*pEdgeA);

    return pSurf;
}




/** ***************************************************************************
* @brief Generate a splitting surface through the connected curve
* @param TopoDS_Edge &edge
* @return McCadAstSurfPlane
*
* @date 08/04/2016
* @modify 08/04/2015
* @author  Lei Lu
******************************************************************************/
McCadAstSurfPlane * McCadSplitCylinders::GenSurfThroughCurves(McCadEdge *& pEdge)
{
    gp_Pnt pntStart = pEdge->StartPoint();
    gp_Pnt pntEnd = pEdge->EndPoint();
    gp_Pnt pntMid = pEdge->MidPoint();

    /// Calculate the normal vector or plane through the two edges (three points)
    gp_Vec vec1(pntStart, pntMid), vec2(pntEnd, pntMid);
    gp_Vec vec = vec1 ^ vec2;
    vec.Normalize();
    gp_Dir dir(vec);

    gp_Pln pln(pntMid, dir);

    Standard_Real size = m_fLength;

    /// Generate the surface according to the normal vector
    TopoDS_Face face = BRepBuilderAPI_MakeFace(pln,-size,size,-size,size).Face();
    McCadAstSurfPlane *pSurf = new McCadAstSurfPlane(face);
    pSurf->SetCombined(Standard_True);

    return pSurf;
}





/** ***************************************************************************
* @brief  Generate the splitting surface thought the edge connect the
*         two cylinders
* @param  Input cylinder face list and generated assisted splitting surface
*         list
* @return void
*
* @date 13/07/2015
* @modify 13/07/2015
* @author  Lei Lu
******************************************************************************/
void McCadSplitCylinders::GenSplitSurfaces( McCadDcompSolid *& pSolid)
{
    vector<McCadBndSurface *>  &face_list    = pSolid->m_CylinderList;
    vector<McCadAstSurface*>   &AstFaceList  = pSolid->m_AstFaceList;

    m_fLength = pSolid->m_fBoxSqLength; 

    //STEPControl_Writer wrt2;
    for(unsigned int i = 0; i < face_list.size()-1; i++ )
    {
        McCadBndSurfCylinder *pBndSurfA = (McCadBndSurfCylinder*)face_list.at(i);
        for(unsigned int j = i+1; j < face_list.size(); j++ )
        {            
            McCadBndSurfCylinder *pBndSurfB = (McCadBndSurfCylinder*)face_list.at(j);

            // They are not same cylinders
            if (pBndSurfA->GetSurfNum() == pBndSurfB->GetSurfNum())
            {
                continue;
            }

            if(pBndSurfA->GetSplitSurfNum() == 0 && pBndSurfB->GetSplitSurfNum() == 0)
            {
                continue;                
            }

            //Handle_TopTools_HSequenceOfShape edge_list = new TopTools_HSequenceOfShape();
            vector<McCadEdge*> *pEdgeList = new vector<McCadEdge*>;

            if(HasComLineEdge(pBndSurfA,pBndSurfB,pEdgeList))
            {
                if (pEdgeList->size() == 1 )
                {
                    McCadEdge *pEdge = pEdgeList->at(0);
                    McCadAstSurfPlane * pAstFace = GenSurfThroughEdge(*pBndSurfA,*pBndSurfB,pEdge);
                    AstFaceList.push_back(pAstFace);

                    pBndSurfA->AddCylnCylnSplitEdge(pEdge);
                    pBndSurfB->AddCylnCylnSplitEdge(pEdge);
                }
                else if(pEdgeList->size() == 2)
                {
                    //McCadEdgeLine *pEdgeA = (McCadEdgeLine*)pEdgeList->at(0);
                    McCadEdge * pEdgeA = (McCadEdge*)pEdgeList->at(0);
                    McCadEdge * pEdgeB = (McCadEdge*)pEdgeList->at(1);

                    if(pEdgeA->GetType() == Line && pEdgeB->GetType() == Line)
                    {
                        McCadEdgeLine *pEdgeLineA = (McCadEdgeLine*)pEdgeA;
                        McCadEdgeLine *pEdgeLineB = (McCadEdgeLine*)pEdgeB;

                        Standard_Real angle_tol = 0.01*M_PI; // McCadConvertConfig::GetTolerence();

                        /// If the two edges are not parallel, it can not generate a plane through two edges.
                        if(pEdgeLineA->IsParallel(pEdgeLineB,angle_tol))
                        {
                            McCadAstSurfPlane * pAstFace = GenSurfThroughTwoEdges(pEdgeLineA,pEdgeLineB);
                            AstFaceList.push_back(pAstFace);

                            pBndSurfA->AddCylnCylnSplitEdge(pEdgeA);
                            pBndSurfB->AddCylnCylnSplitEdge(pEdgeA);
                            pBndSurfA->AddCylnCylnSplitEdge(pEdgeB);
                            pBndSurfB->AddCylnCylnSplitEdge(pEdgeB);

                            // pBndSurfA->SetAstSurfaces(Standard_True);
                            // pBndSurfB->SetAstSurfaces(Standard_True);
                        }
                    }
                }
            }

            delete pEdgeList;
            pEdgeList = NULL;

            ///< Detect the two cylinders are connected with ellipse circle curve
            McCadEdge *pCurveEdge = NULL;            
            if(HasComCurvEdge(pBndSurfA,pBndSurfB,pCurveEdge))
            {                
                McCadAstSurfPlane * pAstFace = GenSurfThroughCurves(pCurveEdge);
                pAstFace->SetCombined(Standard_True); // The assisted surface can not be merged
                AstFaceList.push_back(pAstFace);

                pBndSurfA->AddCylnCylnSplitEdge(pCurveEdge);
                pBndSurfB->AddCylnCylnSplitEdge(pCurveEdge);
            }
        }
    }

    // Merge the generated assisted splitting surfaces
    if(AstFaceList.size() >= 2)
    {
        CombAndMergeSurfaces(AstFaceList);
    }
}



/** ***************************************************************************
* @brief  Combine the splitting surfaces to avoid the generation of thin plate
* @param  vector<McCadAstSurface*> & theAstFaceList
* @return void
*
* @date 13/07/2015
* @modify 14/07/2016
* @author  Lei Lu
******************************************************************************/
void McCadSplitCylinders::CombAndMergeSurfaces(vector<McCadAstSurface*> & theAstFaceList)
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

    ///< Combine the assisted splitting surfaces
    for(int i = 0; i < theAstFaceList.size()-1; i++ )
    {
        for(int j = i+1; j < theAstFaceList.size(); j++ )
        {
//qiu            McCadAstSurfPlane *pFirFace = theAstFaceList.at(i);
            McCadAstSurfPlane *pFirFace =  static_cast<McCadAstSurfPlane *> (theAstFaceList.at(i));
//qiu            McCadAstSurfPlane *pSecFace = theAstFaceList.at(j);
            McCadAstSurfPlane *pSecFace = static_cast<McCadAstSurfPlane *> (theAstFaceList.at(j));

            Standard_Real dis_tol = 50.0;          // Distance between two surfaces, wait modified Lei
            Standard_Real angle_tol = 0.001*M_PI;  // McCadConvertConfig::GetTolerence();

            if(CanCombTwoAstSurfaces(pFirFace,pSecFace,dis_tol,angle_tol))
            {
                // Remove the original combined splitting surfaces
                // and generate a new splitting surface through
                // the two edges of them
                McCadAstSurfPlane *pNewFace = CombSurfaces(pFirFace,pSecFace);
                theAstFaceList.push_back(pNewFace);

                theAstFaceList.erase(theAstFaceList.begin()+i);
                theAstFaceList.erase(theAstFaceList.begin()+j-1);

                delete pFirFace;
                pFirFace = NULL;

                delete pSecFace;
                pSecFace = NULL;

                i--;
                j--;
                break;
            }
        }
    }
}


/** ***************************************************************************
* @brief Two two assisted splitting surfaces can be combined or not
* @param McCadAstSurfPlane *& pFaceA
*        McCadAstSurfPlane *& pFaceB
*        Standard_Real dis_tol
*        Standard_Real angle_tol
* @return Standard_Boolean
*
* @date 13/07/2016
* @modify 14/07/2016
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadSplitCylinders::CanCombTwoAstSurfaces(McCadAstSurfPlane *& pFaceA,
                                                            McCadAstSurfPlane *& pFaceB,
                                                            Standard_Real DisTolerence,
                                                            Standard_Real AngleTolerence)
{

    if(pFaceA->CanbeCombined() || pFaceB->CanbeCombined())
    {
        return Standard_False;
    }

    /// If the two edges of plane are not parallel, it can not generate a plane through two edges.
    McCadEdgeLine *pEdgeA = pFaceA->GetEdge();
    McCadEdgeLine *pEdgeB = pFaceB->GetEdge();

    if(!pEdgeA->IsParallel(pEdgeB,AngleTolerence))
    {
         return Standard_False;
    }

    /// Calculate the distance between two edges
    BRepExtrema_DistShapeShape theExtDist(*pEdgeA,*pFaceB);
    theExtDist.Perform();
    if(theExtDist.IsDone())
    {
        Standard_Real dist = theExtDist.Value();
        if(dist > DisTolerence) // If the minmum distance between edge and face is large than tolerence
        {
           return Standard_False;
        }
    }

    /// Judge the two planes are parallel or not
//    if(m_Dir.IsEqual(pFace->GetDir(),angle_tol) || m_Dir.IsOpposite(pFace->GetDir(),angle_tol))
//    {
//        return Standard_True;
//    }

    return Standard_True;
}






/** ***************************************************************************
* @brief  Combine the two assisted splitting planes, generate a new plane through
*         the two edges of original planes.
* @param  McCadAstSurfPlane *pFace
*
* @return McCadAstSurfPlane *
*
* @date   13/05/2015
* @modify 21/05/2015
* @author Lei Lu
******************************************************************************/
McCadAstSurfPlane * McCadSplitCylinders::CombSurfaces(McCadAstSurfPlane *& pFaceA,
                                                      McCadAstSurfPlane *& pFaceB)
{
    /// Get the start and end points of the curves, and calculate middle points
    gp_Pnt pntMidA   = pFaceA->GetEdge()->MidPoint();

    gp_Pnt pntStartB = pFaceB->GetEdge()->StartPoint();
    gp_Pnt pntEndB   = pFaceB->GetEdge()->EndPoint();

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

