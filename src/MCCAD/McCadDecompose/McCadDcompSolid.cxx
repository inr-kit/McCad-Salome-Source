/******************************************************************************
 *
 * McCadSolid.cxx
 * This class is used for store the solid imported and decomposed child solids
 *
 *  Created on: May, 2015
 *      Author: Lei Lu
 *
 ****************************************************************************/

#include "McCadDcompSolid.hxx"

#include <assert.h>

#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Bnd_Box.hxx>

#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Face.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepGProp.hxx>
#include <BRepBuilderAPI_Sewing.hxx>

#include <ShapeAnalysis_CheckSmallFace.hxx>
#include <STEPControl_Writer.hxx>
#include <ShapeFix_Shape.hxx>

#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopoDS_Vertex.hxx>
#include <TColgp_HSequenceOfPnt.hxx>

#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <GeomAdaptor_Curve.hxx>

#include "../McCadTool/McCadFuseSurface.hxx"
#include "../McCadTool/McCadMathTool.hxx"
#include "../McCadTool/McCadBooleanOpt.hxx"
#include "../McCadTool/McCadGeomTool.hxx"

#include "McCadBndSurfCylinder.hxx"
#include "McCadBndSurfPlane.hxx"
#include "McCadAstSurfPlane.hxx"
#include "McCadSplitCylinders.hxx"
#include "McCadSplitCylnPln.hxx"

#include "McCadEdgeLine.hxx"
#include "McCadEdgeCircle.hxx"
#include "McCadEdgeEllipse.hxx"
#include "McCadEdgeSpline.hxx"

McCadDcompSolid::McCadDcompSolid()
{
}

McCadDcompSolid::McCadDcompSolid(const TopoDS_Solid &theSolid):TopoDS_Solid(theSolid)
{
    m_Solid = theSolid;
    m_HasSplitSurf = Standard_False;                  // Has splitting surface or not

    m_pSelSplitSurf = new McCadSelSplitSurf();

    Bnd_Box bndBox;
    BRepBndLib::Add(m_Solid, bndBox);
    bndBox.SetGap(0.0);
    m_fBoxSqLength = sqrt(bndBox.SquareExtent());
}

McCadDcompSolid::~McCadDcompSolid()
{
    Free();
}


/** ***************************************************************************
* @brief  Free the allocated surface lists
* @param
* @return Void
*
* @date   13/05/2015
* @modify 13/10/2015
* @author Lei Lu
******************************************************************************/
void McCadDcompSolid::Free()
{

    for(unsigned int i = 0; i < m_AstFaceList.size(); i++)
    {
        McCadAstSurface *pSurf = m_AstFaceList.at(i);
        m_AstFaceList.erase(m_AstFaceList.begin()+i);
        delete pSurf;
        pSurf = NULL;
    }

    m_AstFaceList.clear();

//  std::vector<McCadAstSurfPlane *> tmp = *m_pAstFaceList;
//  m_pAstFaceList->swap(tmp);

    for(unsigned int i = 0; i < m_FaceList.size(); i++)
    {
        McCadSurface *pSurf = m_FaceList.at(i);
        m_FaceList.erase(m_FaceList.begin()+i);
        delete pSurf;
        pSurf = NULL;
    }

    m_FaceList.clear();

    if(m_pSelSplitSurf)
    {
        delete m_pSelSplitSurf;
        m_pSelSplitSurf = NULL;
    }
}



/** ***************************************************************************
* @brief  Generate the boundary surfaces list and assisted splitting surfaces
*         Then decompose the solid recursivily
* @param  vector<McCadSolid*> *pSolidList
*         Standard_Integer & iLevel
* @return Void
*
* @date 13/07/2015
* @modify 13/07/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadDcompSolid::Decompose(vector<McCadDcompSolid *> *& pDcompSolidList,
                                            vector<McCadDcompSolid *> *& pErrorSolidList,
                                            Standard_Integer & iLevel, int iSolid)
{
    iLevel++;
    Standard_Integer iRecLevel = iLevel;

//    if(iLevel == 5 && iSolid == 2)
//    {
//       STEPControl_Writer wrt;
//       wrt.Transfer(*this, STEPControl_AsIs);
//       wrt.Write("errorSolid.stp");
//       exit(1);
//    }

    GenSurfaceList();                   /// Generate the boundary surface list
    JudgeDecomposeSurface();            /// Judge which surfaces are decompose surfaces

    // If the solid contains splitting surface which through no boundary surfaces, it
    // is will be used as splitting surface directly.
    if(!m_pSelSplitSurf->HasSurfThroughNoBndSurf(m_SplitFaceList))
    {
        JudgeThroughConcaveEdges(m_FaceList);   /// Judge how many concave edges of each splitting surface through
        // If it has already prioritical splitting surface, skip the assisted surface adding
        if(!m_pSelSplitSurf->HasPlnSplitOnlyPln(m_SplitFaceList))
        {
            GenAssistSurfaces();                /// Generate the assisted surfaces

            JudgeAssistedDecomposeSurfaces();   /// Judge the assisted surfaces are decompose surfaces or not
            JudgeThroughConcaveEdges(m_AstFaceList);
        }
    }

    /// STEPControl_Writer wrt;
    Standard_Integer iSelSurf = 0;

    /// If there are some problem of decomposition, add the solid into error solid list
    if (iLevel >= 25)
    {
        McCadDcompSolid *pErrSolid = this;
        pErrorSolidList->push_back(pErrSolid);
        return Standard_False;
    }

    /// If the solid has splitting surfaces, then perform decomposition
    if (m_HasSplitSurf)
    {
        McCadSurface* pSplitSurf = SelectSplitSurface(iSelSurf);
        assert(pSplitSurf);

        if(NULL == pSplitSurf)
        {
            return Standard_False;
        }

        Handle_TopTools_HSequenceOfShape solid_list = new TopTools_HSequenceOfShape();
        if (!SplitSolid(pSplitSurf,solid_list))
        {
            //pSolidList->push_back(this);
            McCadDcompSolid *pErrSolid = this;
            pErrorSolidList->push_back(pErrSolid);
            return Standard_False;
        }

        cout<<"Level  "<<iLevel<<" - "<<iSolid<<" solid is decomposed"<<endl;        

        for(int i = 1; i <= solid_list->Length(); i++ )
        {
            TopoDS_Solid theSolid = TopoDS::Solid(solid_list->Value(i));           

            McCadDcompSolid *pSolid = new McCadDcompSolid(theSolid);
            pSolid->SetDeflection(m_fDeflection);

            iLevel = iRecLevel;

            //wrt.Transfer(theSolid, STEPControl_AsIs);
            if(pSolid->Decompose(pDcompSolidList,pErrorSolidList,iLevel,i))
            {
                delete pSolid;
                pSolid = NULL;
            }
        }        

        solid_list->Clear();
        return Standard_True;
    }
    else
    {
        cout<<"Level  "<<iLevel<<" - "<<iSolid<<" solid is convex solid"<<endl;
        McCadDcompSolid *pSolid = this;
        pDcompSolidList->push_back(pSolid);
        return Standard_False;
    }

    //wrt.Write("FirstCut.stp");
    //exit(1);
}



/** ***************************************************************************
* @brief  Generate the surface list of solid and categorize the surfaces
*         according to the different surfaces types.
* @param
* @return void
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
void McCadDcompSolid::GenSurfaceList()
{
    Standard_Integer iFaceNum = 0;  // Face number

    TopExp_Explorer exF;            // Trace the face of input solid
    vector<TopoDS_Face> face_list;  // Face list to store the traced bundary surfaces

    /** Find the faces with small areas, they will not be added into face list */
    for (exF.Init(m_Solid,TopAbs_FACE); exF.More(); exF.Next())
    {
        TopoDS_Face face = TopoDS::Face(exF.Current());
        BRepTools::Update(face);

        ShapeAnalysis_CheckSmallFace saf;
        TopoDS_Edge e1, e2;
        if( saf.CheckSpotFace(face,0.0001)||saf.CheckStripFace(face,e1,e2,0.0001))
        {
            GProp_GProps GPface;
            BRepGProp::SurfaceProperties(face, GPface);
            cout<<"# There are small faces are found, the area is "<<GPface.Mass()<<endl;
            continue;
        }

        //BRepCheck_Analyzer BA5(face, Standard_True);
        //if(!BA5.IsValid())
        face_list.push_back(face);
    }

    /** Generate the edge list of each face */
    CalEdgeConvexity();  ///< Calculate the convexity of edges

    for(unsigned int i = 0; i < face_list.size(); i++ )
    {
        iFaceNum++;
        TopoDS_Face face = TopoDS::Face(face_list.at(i));

        McCadBndSurface *pBndSurf = GenSurface(face,0);     // Lei Lu need modification
        pBndSurf->SetSurfNum(iFaceNum);                     // Set the surface number        

        if (pBndSurf->GenTriangles(m_fDeflection))          // Mesh the face and generate a list of triangle
        {
            GenEdges(pBndSurf);
            if(pBndSurf->GetSurfType() == Plane)            // If the boundary surface is plane
            {
                m_PlaneList.push_back(pBndSurf);
            }
            else if(pBndSurf->GetSurfType() == Cylinder)    // If the boundary surface is cylinder
            {
                m_CylinderList.push_back(pBndSurf);
            }
        }
        else
        {
            cout<<"Error...Mesh Face "<<iFaceNum<<endl;
        }
    }

    MergeSurfaces(m_PlaneList);     ///<  Merge the planes which have same geometries and common edges
    MergeSurfaces(m_CylinderList);  ///<  Merge the curved surfaces with same geometries in the solid

    /** Add the cylinders and planes into face list */
    m_FaceList.insert(m_FaceList.end(),m_PlaneList.begin(),m_PlaneList.end());
    m_FaceList.insert(m_FaceList.end(),m_CylinderList.begin(),m_CylinderList.end());
}




/** ***************************************************************************
* @brief  According to the input face, generate the bundary surface objects
* @param
* @return void
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
McCadBndSurface* McCadDcompSolid::GenSurface(TopoDS_Face face,Standard_Integer iSurfType)
{
    ////**********************************************************************/
    //! The created new face always has geometry error.
    //! So it needs the healing before using.
    Handle(ShapeFix_Shape) shpFixer = new ShapeFix_Shape(face);
    shpFixer->SetPrecision(1.0e-7);
    shpFixer->SetMaxTolerance(1.0e-3);

    shpFixer->FixWireTool()->FixRemovePCurveMode() = 1;
    shpFixer->FixWireTool()->FixConnected();

    shpFixer->Perform();
    ///***********************************************************************/
    face = TopoDS::Face(shpFixer->Shape());

    if (iSurfType == 1) //wait for modification
    {
        //pBndSurf = new McCadAstSurfPlane(face);
    }
    else
    {
        BRepAdaptor_Surface BS(face, Standard_True);
        GeomAdaptor_Surface AdpSurf = BS.Surface();

        if( AdpSurf.GetType() == GeomAbs_Cylinder)
        {            
            McCadBndSurfCylinder *pBndCylnSurf = new McCadBndSurfCylinder(face);  // Generate the cylinder bundary object
            pBndCylnSurf->GenExtCylinder(m_fBoxSqLength);

            assert(pBndCylnSurf);
            return pBndCylnSurf;
        }
        else if (AdpSurf.GetType() == GeomAbs_Plane)
        {
            McCadBndSurfPlane *pBndPlnSurf = new McCadBndSurfPlane(face);     // Generate the plane bundary object
            pBndPlnSurf->GenExtPlane(m_fBoxSqLength);

            assert(pBndPlnSurf);
            return pBndPlnSurf;
        }
    } 
}





/** ***************************************************************************
* @brief  Judge the boundary surfaces in the solid are splitting surface or not
* @param
* @return void
*
* @date 13/05/2015
* @modify 29/03/2016
* @author Lei Lu
******************************************************************************/
void McCadDcompSolid::JudgeDecomposeSurface()
{
    McCadBndSurface* pFirFace = NULL;
    McCadBndSurface* pSecFace = NULL;

    //STEPControl_Writer wrt;

    /** Judge the boundary surfaces of solid are splitting surfaces or not */
    for (int i = 0; i <  m_FaceList.size(); i++)
    {
        Standard_Integer iPosFace = 0;  // Number of the faces on the positive side
        Standard_Integer iNegFace = 0;  // Number of the faces on the negative side

        pFirFace = m_FaceList.at(i);
        assert(pFirFace);

        //wrt.Transfer(*pFirFace, STEPControl_AsIs);

        Standard_Boolean bIsSplitFace = Standard_False; // If it is splitting face
        Standard_Integer iSplitSurfNum = 0;             // How many surfaces have collision with this face
        Standard_Integer iSplitCurvSurfNum = 0;         // How many curved surfaces have collision with it

        for (int j = 0; j < m_FaceList.size(); j++)
        {
            pSecFace = m_FaceList.at(j);

            if((i == j) || (pFirFace->GetSurfNum() == pSecFace->GetSurfNum())) //If the two compared surface are not same surface
            {
                continue;
            }

            if(pFirFace->IsSameSurface(pSecFace))
            {
               continue;
            }

            // The position relationship between faces,
            // 1 and -1 means the face location totally on positive or minor side of another face.
            POSITION eSide = MIDDLE/*0*/; //qiu add void convertion

            // Detect the face hace collision with another face, or located on one side of face.
            if (pFirFace->FaceCollision(pSecFace,eSide))
            {
                iSplitSurfNum ++;
                bIsSplitFace = Standard_True;

                if (pSecFace->GetSurfType() != Plane) // if the surface is not plane
                {
                    iSplitCurvSurfNum++;
                }
            }
            else
            {
                if (eSide == POSITIVE)    // The face locates on positive side of another face
                {
                    iPosFace++;
                }
                else if(eSide == NEGATIVE)// The face locates on negative side of another face
                {
                    iNegFace++;
                }
            }
        }

        // if the boundary surfaces located at both the positive and negative spaces of this
        // boundary surface, even there are no surface splitted by this face, it means  ths face
        // is still splitting face which can split the solid into two parts
        if (iPosFace > 0 && iNegFace > 0)
        {
            bIsSplitFace = Standard_True;
        }

        if (bIsSplitFace) // if surface is splitting surface
        {
            pFirFace->SetSplitSurfNum(iSplitSurfNum);
            pFirFace->SetSplitCurvSurfNum(iSplitCurvSurfNum);
            m_SplitFaceList.push_back(pFirFace);
        }
    }

    //wrt.Write("Faces.stp");
    if (!m_SplitFaceList.empty()) // if a solid has no splitting surface
    {
        m_HasSplitSurf = Standard_True;
    }
}




/** ***************************************************************************
* @brief  Judge how many concave edges each face of solid go through
* @param
* @return void
*
* @date 13/06/2016
* @author Lei Lu
******************************************************************************/
void McCadDcompSolid::JudgeThroughConcaveEdges( vector<McCadBndSurface*> & theFaceList )
{
    McCadBndSurface* pFirFace = NULL;
    McCadBndSurface* pSecFace = NULL;

    for (int i = 0; i <  theFaceList.size(); i++)
    {
        pFirFace = theFaceList.at(i);
        assert(pFirFace);

        Standard_Integer iConcaveEdges = 0;             // How many surfaces have collision with this face
        for (int j = 0; j < theFaceList.size(); j++)
        {
            pSecFace = theFaceList.at(j);
            if((i == j) || (pFirFace->GetSurfNum() == pSecFace->GetSurfNum())) //If the two compared surface are not same surface
            {
                continue;
            }

            for(int k = 0; k < pSecFace->GetEdgeList().size(); k++)
            {
                McCadEdge *pEdge = pSecFace->GetEdgeList().at(k);
                if(pEdge->IsConcaveEdge() && pFirFace->IsEdgeOnFace(pEdge))
                {
                    iConcaveEdges++;
                }
            }
        }

        pFirFace->SetThroughConcaveEdges(iConcaveEdges);
        //cout<<"iConvexity   "<<iConcaveEdges<<endl;
    }
}




/** ***************************************************************************
* @brief  Judge how many concave edges each face of solid go through
* @param
* @return void
*
* @date 13/06/2016
* @author Lei Lu
******************************************************************************/
void McCadDcompSolid::JudgeThroughConcaveEdges( vector<McCadAstSurface*> & theFaceList )
{
    McCadAstSurface* pFirFace = NULL;
    McCadBndSurface* pSecFace = NULL;

    for (unsigned int i = 0; i <  theFaceList.size(); i++)
    {
        pFirFace = theFaceList.at(i);
        assert(pFirFace);       

        Standard_Integer iConcaveEdges = 0;             // How many surfaces have collision with this face
        for (unsigned int j = 0; j < m_FaceList.size(); j++)
        {
            pSecFace = m_FaceList.at(j);
            for(unsigned int k = 0; k < pSecFace->GetEdgeList().size(); k++)
            {
                McCadEdge *pEdge = pSecFace->GetEdgeList().at(k);
                if(pEdge->IsConcaveEdge() && pFirFace->IsEdgeOnFace(pEdge))
                {
                    iConcaveEdges++;
                }
            }
        }  

        pFirFace->SetThroughConcaveEdges(iConcaveEdges);       
    }
}


/** ***************************************************************************
* @brief Generate the assisted splitting surfaces
* @param
* @return void
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
void McCadDcompSolid::GenAssistSurfaces()
{
    McCadDcompSolid *pSolid = this;

    ///< Find the assistant surface for seprating connected cylinders
    if(m_CylinderList.size() >= 2 )
    {
        McCadSplitCylinders * split_cylinders = new McCadSplitCylinders();
        split_cylinders->GenSplitSurfaces(pSolid);
        delete split_cylinders;
        split_cylinders = NULL;
    }

    ///< Find the assistant surface for seprating the connected cylinder and plane
    if(!m_CylinderList.empty() && !m_PlaneList.empty())
    {
        McCadSplitCylnPln *split_CylnPln = new McCadSplitCylnPln();
        split_CylnPln->GenSplitSurfaces(pSolid);
        delete split_CylnPln;
        split_CylnPln = NULL;
    }

//    if(!m_AstFaceList.empty())
//    {
//        STEPControl_Writer wrt;
//        for(unsigned int i = 0; i < m_AstFaceList.size(); i++)
//        {
//            McCadAstSurface *pSurf = m_AstFaceList.at(i);
//            wrt.Transfer(*pSurf, STEPControl_AsIs);
//            wrt.Write("SplittingFaces.stp");
//        }
//        exit(1);
//    }
}




/** ***************************************************************************
* @brief  Judge the added assisted surfaces in the solid are splitting surface
*         or not
* @param
* @return void
*
* @date 13/05/2015
* @modify 29/03/2016 define this function as a independent subroutine function
* @author Lei Lu
******************************************************************************/
void McCadDcompSolid::JudgeAssistedDecomposeSurfaces()
{
    /** Judge the added assisted surfaces are splitting surfaces or not */
    for(int i = 0; i < m_AstFaceList.size(); i++ )
    {
//qiu        McCadAstSurfPlane *pFirFace = m_AstFaceList.at(i);
        McCadAstSurfPlane *pFirFace = static_cast<McCadAstSurfPlane *> (m_AstFaceList.at(i));

        Standard_Integer iPosFace = 0;  // Number of the faces on the positive side
        Standard_Integer iNegFace = 0;  // Numb5er of the faces on the negative side

        Standard_Boolean bIsDecomposeFace = Standard_False; // If it is decomposition face
        Standard_Integer iSplitSurfNum = 0; // How many other faces has collision with this face
        Standard_Integer iSplitCurvSurfNum = 0;         // How many curved surfaces have collision with it

        for (int j = 0; j < m_FaceList.size(); j++)
        {
            McCadBndSurface *pSecFace = m_FaceList.at(j);

            POSITION eSide = MIDDLE/*0*/; // The relationship between faces

            if (pFirFace->FaceCollision(pSecFace,eSide))
            {
                iSplitSurfNum ++;
                bIsDecomposeFace = Standard_True;

                if (pSecFace->GetSurfType() != Plane) // if the surface is not plane
                {
                    iSplitCurvSurfNum++;
                }
            }
            else
            {
                if (eSide == POSITIVE)
                {
                    iPosFace++;
                }
                else if(eSide == NEGATIVE)
                {
                    iNegFace++;
                }
            }
        }

        if (iPosFace > 0 && iNegFace > 0)
        {
            bIsDecomposeFace = Standard_True;
        }

        if (bIsDecomposeFace)
        {
            pFirFace->SetSplitSurfNum(iSplitSurfNum);// wait for modification
            pFirFace->SetSplitCurvSurfNum(iSplitCurvSurfNum);
            m_SplitFaceList.push_back(pFirFace);
        }
    }

    if (!m_SplitFaceList.empty()) // if a solid has no splitting surface
    {
        m_HasSplitSurf = Standard_True;
    }
}



/** ***************************************************************************
* @brief  Merge the two surfaces which have same geometry and common edge
* @param
* @return Void
*
* @date 13/07/2015
* @modify 13/07/2015
* @author  Lei Lu
******************************************************************************/
void McCadDcompSolid::MergeSurfaces(vector<McCadBndSurface*> & faceList)
{
    if(faceList.size() <= 1)
    {
        return;
    }
    for(unsigned int i = 0; i < faceList.size()-1; i++ )
    {
        McCadBndSurface *pSurfA = faceList.at(i);
        for(unsigned int j = i+1; j < faceList.size(); j++ )
        {
            McCadBndSurface *pSurfB = faceList.at(j);

            if(pSurfA->IsSameSurface(pSurfB))
            {
                pSurfA->SetSurfNum(pSurfB->GetSurfNum()); // Set the two surfaces are same surfaces.
            }
            else
            {
                continue;
            }

            if(pSurfA->CanFuseSurf(pSurfB))
            {
                TopoDS_Face fused_face = pSurfA->FuseSurfaces(pSurfB);
                McCadBndSurface *pBndSurf = GenSurface(fused_face,0);

                pBndSurf->AddTriangles(pSurfA);
                pBndSurf->AddTriangles(pSurfB);

                //pBndSurf->SetLoopNum(pSurfA->GetLoopsNum()+pSurfB->GetLoopsNum());

                pBndSurf->SetSurfNum(pSurfA->GetSurfNum());
                pBndSurf->CombineEdges(pSurfA, pSurfB);

                faceList.erase(faceList.begin()+j);
                faceList.erase(faceList.begin()+i);     // Delete the original surfaces

                faceList.push_back(pBndSurf);             // Add the new generated surface

                delete pSurfA;
                pSurfA = NULL;
                delete pSurfB;
                pSurfB = NULL;

                i--;
                j--;
                break;
            }
        }
    }
}




/** ***************************************************************************
* @brief  Sort the splitting surface, choose the best splitting surface
* @param
* @return Void
*
* @date 13/07/2015
* @modify 13/07/2015
* @author  Lei Lu
******************************************************************************/
McCadSurface * McCadDcompSolid::SelectSplitSurface(Standard_Integer iSel)
{
    McCadSurface* pSelectSurf = NULL;

    ///< Merge the splitting surfaces, remove the repeated surfaces
    m_pSelSplitSurf->MergeSplitSurfaces(m_SplitFaceList);    
    ///< Select the splitting surfaces to generate the selected splitting surfaces list
    m_pSelSplitSurf->GenSplitSurfList(m_SplitFaceList,m_SelSplitFaceList);

    if (!m_SelSplitFaceList.empty())
    {
        pSelectSurf = m_SelSplitFaceList.at(0);
        //cout<<"Through  "<<pSelectSurf->GetThroughConcaveEdges()<<"  "<<pSelectSurf->GetSplitSurfNum()<<endl;
    }

    assert(pSelectSurf);
    return pSelectSurf;
}




/** ***************************************************************************
* @brief  Set the deflection for surface meshing
* @param
* @return void
*
* @date 06/04/2016
* @modify 06/04/2016
* @author  Lei Lu
******************************************************************************/
void McCadDcompSolid::SetDeflection(Standard_Real deflection)
{
    m_fDeflection = deflection;
}



/** ***************************************************************************
* @brief Calculate the convexities of each edge, and change the Convex() of edge
* @param
* @return void
*
* @date 06/06/2016
* @modify 06/06/2016
* @author  Lei Lu
******************************************************************************/
void McCadDcompSolid::CalEdgeConvexity()
{
    //STEPControl_Writer wrt;

    // Judge the convexities of edges
    TopTools_IndexedDataMapOfShapeListOfShape mapEdgeFace;
    TopExp::MapShapesAndAncestors(m_Solid,TopAbs_EDGE,TopAbs_FACE, mapEdgeFace);

    Standard_Integer nbEdge = mapEdgeFace.Extent();
    TopTools_ListOfShape listFace;

    for (int iKey = 1; iKey <= nbEdge; iKey++)
    {
        TopoDS_Edge edge = TopoDS::Edge(mapEdgeFace.FindKey(iKey));

        BRepAdaptor_Curve adaptorCurve;
        adaptorCurve.Initialize(edge);

        listFace = mapEdgeFace.FindFromKey(edge);
        TopTools_ListIteratorOfListOfShape iterFace(listFace);

        if(listFace.Extent() != 2)
        {
           continue;
        }

        TopoDS_Face FaceA = TopoDS::Face(iterFace.Value());
        iterFace.Next();
        TopoDS_Face FaceB = TopoDS::Face(iterFace.Value());

        /// Get the first vertex of edge
        TopoDS_Vertex vtxStart = TopExp::FirstVertex(edge,0);
        gp_Pnt pntStart = BRep_Tool::Pnt(vtxStart);
        TopoDS_Vertex vtxEnd = TopExp::LastVertex(edge,0);
        gp_Pnt pntEnd = BRep_Tool::Pnt(vtxEnd);

        gp_Vec vec(pntStart,pntEnd);
        gp_Dir dir(vec);

        /// Get the normals of each surface
        gp_Dir normalA = McCadGeomTool::NormalOnFace(FaceA,pntStart);
        gp_Dir normalB = McCadGeomTool::NormalOnFace(FaceB,pntStart);

        BRepAdaptor_Curve baCrv;
        baCrv.Initialize(edge);

        Standard_Real fStartB, fEndB;
        Handle(Geom_Curve) theCurve = BRep_Tool::Curve(edge, fStartB, fEndB);

        Standard_Real angle = normalA.AngleWithRef(normalB,dir);

        if(Abs(angle) < 1.0e-4 )
        {
           angle = 0;
        }

        /** The edge is concave */
        if( angle > 0 && edge.Orientation() == TopAbs_REVERSED)
        {
            //wrt.Transfer(edge, STEPControl_AsIs);
            edge.Convex(1);
        }
        else if(edge.Orientation() == TopAbs_FORWARD && angle < 0)
        {
            //wrt.Transfer(edge, STEPControl_AsIs);
            edge.Convex(1);
        }
    }
}



/** ***************************************************************************
* @brief Generate the edges of each face, and set the convexities of edges
* @param
* @return void
*
* @date 06/06/2016
* @modify 06/06/2016
* @author  Lei Lu
******************************************************************************/
void McCadDcompSolid::GenEdges(McCadBndSurface *& pBndSurf)
{
    for(TopExp_Explorer ex(*pBndSurf, TopAbs_EDGE); ex.More(); ex.Next())
    {
        TopoDS_Edge edge = TopoDS::Edge(ex.Current());
        McCadEdge *pEdge = NULL;

        Standard_Real fStart, fEnd;
        Handle(Geom_Curve) theCurve = BRep_Tool::Curve(edge, fStart, fEnd);

        gp_Pnt pntStart, pntEnd;
        theCurve->D0(fStart, pntStart);
        theCurve->D0(fEnd, pntEnd);

        switch (GeomAdaptor_Curve(theCurve).GetType())
        {
            case GeomAbs_Line:
            {
                pEdge = new McCadEdgeLine(edge);
                break;
            }
            case GeomAbs_Circle:
            {
                pEdge = new McCadEdgeCircle(edge);
                break;
            }
            case GeomAbs_Ellipse:
            {
                pEdge = new McCadEdgeEllipse(edge);
                break;
            }
            default:
                pEdge = new McCadEdgeSpline(edge);
                break;
        }

        /** Set the convexity of edge */
        if(edge.Convex())
        {
            pEdge->SetConvexity(-1);
        }

        /** Set the edge can be used for adding assisted splitting surfaces */
        if( pBndSurf->GetSurfType() == Cylinder && pEdge->GetType() == Line )
        {
            Standard_Real UMin,UMax,VMin,VMax;
            Standard_Real UMin1,UMax1,VMin1,VMax1;
            BRepTools::UVBounds(*pBndSurf,edge,UMin,UMax,VMin,VMax);
            BRepTools::UVBounds(*pBndSurf,UMin1,UMax1,VMin1,VMax1);

            if( Abs(UMin - UMin1) < 1.0e-3 || Abs(UMax-UMax1) < 1.0e-3)
            {
                // The edge can be used for adding assisted splitting surface
                pEdge->AddAstSplitSurf(Standard_True);
            }
//            else
//            {
//                cout<<"UMin  "<<UMin<<"   UMin1   "<<UMin1<<"  "<<Abs(UMin - UMin1)<<endl;
//                cout<<"UMax  "<<UMax<<"   UMax1   "<<UMax1<<"  "<<Abs(UMax-UMax1)<<endl;
//            }
        }

        pBndSurf->AddEdge(pEdge); // Add the edge into edge list of the surface
    }
}





//cout<<"UMin  "<<UMin<<"   UMin1   "<<UMin1<<endl;
//cout<<"UMax  "<<UMax<<"   UMax1   "<<UMax1<<endl;
//cout<<"VMin  "<<VMin<<"   VMin1   "<<VMin1<<endl;
//cout<<"VMax  "<<VMax<<"   VMax1   "<<VMax1<<endl;

//    for(int i = 0; i < m_FaceList.size()-1; i++ )
//    {
//        McCadBndSurface *pSurfA = (McCadBndSurface*)m_FaceList.at(i);
//        for(int j = i+1; j < m_FaceList.size(); j++ )
//        {
//            McCadBndSurface *pSurfB = m_FaceList.at(j);

//            for(TopExp_Explorer exA(*pSurfA, TopAbs_EDGE); exA.More(); exA.Next())
//            {
//                TopoDS_Edge edgeA = TopoDS::Edge(exA.Current());

//                for(TopExp_Explorer exB(*pSurfB, TopAbs_EDGE); exB.More(); exB.Next())
//                {
//                    TopoDS_Edge edgeB = TopoDS::Edge(exB.Current());

//                    if (edgeA.IsSame(edgeB))
//                    {
//                        BRepAdaptor_Curve adaptorCurve;
//                        adaptorCurve.Initialize(edgeA);

//                        /// Get the first vertex of edge
//                        TopoDS_Vertex vtxStart = TopExp::FirstVertex(edgeA,0);
//                        gp_Pnt pntStart = BRep_Tool::Pnt(vtxStart);

//                        TopoDS_Vertex vtxEnd = TopExp::LastVertex(edgeB,0);
//                        gp_Pnt pntEnd = BRep_Tool::Pnt(vtxEnd);


//                      //  if(edgeA.Orientation() == TopAbs_REVERSED)
//                       // {
//                            gp_Vec v1( pntEnd,pntStart);
//                            gp_Dir dir(v1);

//                            if(edgeA.Orientation() == TopAbs_REVERSED)
//                            {
//                                cout<<"ewrwerrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr"<<endl;

//                                gp_Vec v2( pntStart,pntEnd);
//                                dir.SetX(v2.X());
//                                dir.SetY(v2.Y());
//                                dir.SetZ(v2.Z());
//                            }

//                       // }
//                      //  else
//                        {
//                         //   gp_Vec v1( pntStart,pntEnd);
//                          //  dir.SetX(v1.X());
//                          //  dir.SetY(v1.Y());
//                          //  dir.SetZ(v1.Z());
//                        }




//                        /// Get the normals of each surface
//                        gp_Dir normalA = McCadCSGTool::Normal(*pSurfA,pntStart);
//                        gp_Dir normalB = McCadCSGTool::Normal(*pSurfB,pntStart);

//                        cout<<"normalA "<<normalA.X()<<"  "<<normalA.Y()<<"   "<<normalA.Z()<<endl;
//                        cout<<"normalB "<<normalB.X()<<"  "<<normalB.Y()<<"   "<<normalB.Z()<<endl;

//                        BRepAdaptor_Curve baCrv;
//                        baCrv.Initialize(edgeA);

//                        Standard_Real fStartB, fEndB;
//                        Handle(Geom_Curve) theCurve = BRep_Tool::Curve(edgeA, fStartB, fEndB);

//                        if(GeomAdaptor_Curve(theCurve).GetType() == GeomAbs_Line)
//                        {
//                            //gp_Lin line = baCrv.Line();
//                            //gp_Dir dir = line.Direction();

//                            if(normalA.IsEqual(normalB,1.0e-3))
//                            {

//                            }
//                            else
//                            {
//                                gp_Dir newDir =  normalB^normalA ;
//                               // Standard_Real angle = dir.Angle(newDir);
//                               // cout<< angle * 180 / M_PI <<"   Degree "<<endl;
//                                if(newDir.IsEqual(dir,1.0e-3))
//                                {
//                                    wrt.Transfer(edgeA, STEPControl_AsIs);
//                                }
//                            }
//                        }

//                        wrt.Write("ConvexEdges3.stp");
//                    }
//                }
//            }
//        }
//    }


/** ***************************************************************************
* @brief  Split the model with the splitting surfaces
* @param  McCadSurface *& pSplitSurf  Splitting surface
*         Handle_TopTools_HSequenceOfShape &solid_list The solid list after splitting
* @return Void
*
* @date 13/07/2015
* @modify 13/07/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadDcompSolid::SplitSolid(McCadSurface *& pSplitSurf,Handle_TopTools_HSequenceOfShape &solid_list )
{
    McCadBooleanOpt *BoolOpt = new McCadBooleanOpt(m_Solid, *pSplitSurf);
    Standard_Boolean result;
    //McCadBooleanOpt BoolOpt(m_Solid, pSplitSurf->GetTopoFace());
    if (BoolOpt->Perform())
    {
        result = Standard_True;
    }
    else
    {
        result = Standard_False;
    }

    BoolOpt->GetResultSolids(solid_list);

    delete BoolOpt;
    BoolOpt = NULL;

    return result;
}
