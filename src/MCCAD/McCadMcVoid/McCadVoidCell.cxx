#include "McCadVoidCell.hxx"

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepBndLib.hxx>
#include <TopoDS.hxx>
#include <assert.h>
#include <omp.h>
#include <Standard.hxx>

#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <STEPControl_Writer.hxx>
//qiu no use #include <McCadCSGUtil_SolidBuilder.hxx>

#include "McCadVoidCellManager.hxx"
#include "McCadGeomPlane.hxx"
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadGeomTool.hxx"

#include <gp_Pln.hxx>
#include "../McCadTool/McCadMathTool.hxx"

McCadVoidCell::McCadVoidCell()
{
}

McCadVoidCell::McCadVoidCell(const TopoDS_Solid & theSolid):TopoDS_Solid(theSolid)
{
    m_SplitAxis = XAxis;
    m_VertexList = new TColgp_HSequenceOfPnt;

    GetBntBox();
    GetGeomFaceList();
}


McCadVoidCell::~McCadVoidCell()
{
    m_CollidedSolidNumList.clear();
    m_VertexList->Clear();

    for (unsigned int i = 0; i < m_CollisionList.size(); i++)
    {
        McCadVoidCollision *pCollision = m_CollisionList.at(i);
        delete pCollision;
        pCollision = NULL;
    }
    m_CollisionList.clear();

    for (unsigned int i = 0; i < m_BndFaceList.size(); i++)
    {
        McCadExtBndFace *pFace = m_BndFaceList.at(i);
        delete pFace;
        pFace = NULL;
    }
    m_BndFaceList.clear();
}



/** ********************************************************************
* @brief Add the index of collied material solid with void box in the
*        convexsolid list
*
* @param Standard_Integer iNum The index of collied solid
* @return TopoDS_Shape
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadVoidCell::AddColliedSolidNum(Standard_Integer index)
{
    m_CollidedSolidNumList.push_back(index);
}



/** ********************************************************************
* @brief Get the boundary box of solid
*
* @param
* @return Bnd_Box
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Bnd_Box McCadVoidCell::GetBntBox()
{
    if(m_bHaveBndBox)                                       // If already have boundary box
    {
        return m_bBox;
    }
    else
    {
        /* Get Boundary box of current solid */
        BRepBndLib::Add(*this, m_bBox);
        m_bBox.SetGap(0.0);

        /* Set the max and min position of the boundary box */
        Standard_Real fXmin, fYmin, fZmin, fXmax, fYmax, fZmax;
        m_bBox.Get(fXmin, fYmin, fZmin, fXmax, fYmax, fZmax);

        m_MaxPnt.SetCoord(fXmax, fYmax, fZmax);                     // The max point of the boundary box
        m_MinPnt.SetCoord(fXmin, fYmin, fZmin);                     // The min point of the boundary box

        //m_bBoxShape = BRepPrimAPI_MakeBox(MinPnt,MaxPnt).Shape(); // Create the shape of boundary box
        m_bHaveBndBox = Standard_True;                              // Already have boundary box

        return m_bBox;
    }
}



/** ********************************************************************
* @brief Set the boundary box of solid
*
* @param Standard_Real fXmin, Standard_Real fYmin, Standard_Real fZmin,
         Standard_Real fXmax, Standard_Real fYmax, Standard_Real fZmax
* @return Bnd_Box
*
* @date 31/8/2012
* @modify 1/8/2016
* @author Lei Lu
************************************************************************/
void McCadVoidCell::SetBntBox(Standard_Real fXmin, Standard_Real fYmin, Standard_Real fZmin,
                              Standard_Real fXmax, Standard_Real fYmax, Standard_Real fZmax)
{
    m_bBox.Update(fXmin, fYmin, fZmin, fXmax, fYmax, fZmax);
    m_bHaveBndBox = Standard_True;                              // Already have boundary box

    m_MaxPnt.SetCoord(fXmax, fYmax, fZmax);                     // The max point of the boundary box
    m_MinPnt.SetCoord(fXmin, fYmin, fZmin);                     // The min point of the boundary box
}




/** ********************************************************************
* @brief Calculate the collied faces of material solid with void box,
*        The convex solids are stored at the McCadGeomData.
*
* @param McCadGeomData * pData
* @return void
*
* @date 31/8/2012
* @modify 28/7/2016
* @author  Lei Lu
************************************************************************/
void McCadVoidCell::CalColliedFaces(McCadGeomData * pData)
{ 
    for (unsigned int i = 0; i < m_CollidedSolidNumList.size(); i++)
    {
        int index = m_CollidedSolidNumList.at(i);
        McCadConvexSolid * pSolid = (pData->GetConvexSolid()).at(index);
        assert(pSolid);

        Bnd_Box bbox_solid = pSolid->GetBntBox();
        if(!CalColliedBox(bbox_solid))
        {
            m_CollidedSolidNumList.erase(m_CollidedSolidNumList.begin()+i);
            i--;      
            continue;
        }

        McCadVoidCollision *pCollision = new McCadVoidCollision();
        Standard_Boolean bSolidCollied = Standard_False; // If the void collied with this solid
        for (unsigned int j = 0; j < pSolid->GetSTLFaceList().size(); j++)
        {
            McCadExtBndFace *pFace = pSolid->GetSTLFaceList().at(j);
            Bnd_Box bbox_face = pFace->GetBndBox();

            Standard_Boolean bCollied = Standard_False;
            if( !CalColliedBox(bbox_face))
            {              
                continue;
            }

            if (CalColliedPoints(pFace->GetEdgePntList()))
            {
                bCollied = Standard_True;
            }
            else if (CalColliedPoints(pFace->GetDiscPntList()))
            {              
                bCollied = Standard_True;
            }
            else if (CalColliedFace(pFace))
            {                
                bCollied = Standard_True;
            }
            else
            {
                bCollied = Standard_False;
            }


            if (bCollied)
            {                
                pCollision->AddColliedFace(pFace->GetFaceNum());                
                if(pFace->HaveAstSurf())
                {
                    for (unsigned int k = 0; k < pFace->GetAstFaces().size(); k++)
                    {
                        McCadExtAstFace *pAstFace = pFace->GetAstFaces().at(k);
                        if(pAstFace->IsSplitFace())
                        {
                             pCollision->AddColliedAstFace(pAstFace->GetFaceNum());
                        }
                        else
                        {
                             pCollision->AddColliedFace(pAstFace->GetFaceNum());
                        }
                    }
                }
                /// If there are boundary surface of solid is collied with void box,
                /// then the void box is collied with this solid.
                bSolidCollied = Standard_True;
            }
        }

        if(bSolidCollied)
        {
            pCollision->SetColliedSolidNum(index+1);
            m_CollisionList.push_back(pCollision);
            continue;
        }

        // Advanced collision detect.Judge the vertex of void cell is in the solid or not.
        if(IsVertexInSolid(pSolid))
        {
            //cout<< "Yes  there are collision with vertex++++++++++++++++++++++++++++++++"<<endl;
            for (unsigned int j = 0; j < pSolid->GetSTLFaceList().size(); j++)
            {
               McCadExtBndFace *pFace = pSolid->GetSTLFaceList().at(j);
               pCollision->AddColliedFace(pFace->GetFaceNum());

               if(pFace->HaveAstSurf())
               {
                   for (unsigned int k = 0; k < pFace->GetAstFaces().size(); k++)
                   {
                       McCadExtAstFace *pAstFace = pFace->GetAstFaces().at(k);
                       if(pAstFace->IsSplitFace())
                       {
                            pCollision->AddColliedAstFace(pAstFace->GetFaceNum());
                       }
                       else
                       {
                            pCollision->AddColliedFace(pAstFace->GetFaceNum());
                       }
                   }
               }
           }

           bSolidCollied = Standard_True;
        }


        if(bSolidCollied)
        {
            pCollision->SetColliedSolidNum(index+1);
            m_CollisionList.push_back(pCollision);
        }
        else
        {
            m_CollidedSolidNumList.erase(m_CollidedSolidNumList.begin()+i);
            i--;
            delete pCollision;
            pCollision == NULL;
        }
    }
}




/** ********************************************************************
* @brief Calculate the relationship between points and void boxes, if
*        one of the points is located in the box, it means the face and
*        void box have collision.
*
* @param Handle_TColgp_HSequenceOfPnt point_list
* @return Standard_Boolean
*
* @date 20/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadVoidCell::CalColliedPoints(Handle_TColgp_HSequenceOfPnt point_list)
{
    Standard_Boolean bInVoid = Standard_False;

    for (unsigned int i = 1; i <= point_list->Length(); i++)
    {
        gp_Pnt pnt = point_list->Value(i);       
        if (!IsPointInBBox(pnt))
        {
            continue;
        }
        else
        {
            bInVoid = Standard_True;
            break;
        }
    }
    return bInVoid;
}



/** ********************************************************************
* @brief Calculate if the two boxes are connected and touched with
*        each other
*
* @param
* @return Standard_Boolean
*
* @date 15/3/2014
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadVoidCell::CalColliedBox(Bnd_Box & box)
{
    Standard_Boolean bConnected = Standard_False;

    box.SetGap(0.0);
    Standard_Real xMin1, yMin1, zMin1, xMax1, yMax1, zMax1;
    box.Get(xMin1, yMin1, zMin1, xMax1, yMax1, zMax1);

    m_bBox.SetGap(0.0);
    Standard_Real xMin2, yMin2, zMin2, xMax2, yMax2, zMax2;
    m_bBox.Get(xMin2, yMin2, zMin2, xMax2, yMax2, zMax2);


    if ( Abs(xMin1-xMax2)<1.0e-5 || Abs(xMax1-xMin2)<1.0e-5)
    {
        bConnected = Standard_True;
    }

    if ( Abs(yMin1-yMax2)<1.0e-5 || Abs(yMax1 - yMin2)<1.0e-5)
    {
        //cout<<yMin1<<" "<<yMax1<<" "<<yMin2<<" "<<yMax2<<endl;
        bConnected = Standard_True;
    }

    if ( Abs(zMin1-zMax2)<1.0e-5 || Abs(zMax1 - zMin2)<1.0e-5)
    {
        bConnected = Standard_True;
    }

    if (m_bBox.IsOut(box) || bConnected)
    {
        return Standard_False;
    }
    else
    {
        return Standard_True;
    }

}



/** ********************************************************************
* @brief Further collision detection, use the boolean operation between
*        face and void box, if they have common section, then they are
*        collied.
*        Because same faces have been merged, and in some case, the merged
*        surfaces are large the original surfaces, so the origianl same
*        surfaces should be used for detecting the collision instead of
*        the merged surface.
*
* @param
* @return TopoDS_Shape
*
* @date 20/3/2013
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadVoidCell::CalColliedFace(McCadExtBndFace *& theFace)
{     
    Standard_Boolean PerformNow = Standard_False;
    int nCount = 0;    

    if(!theFace->IsFusedFace())
    {
        BRepAlgoAPI_Section section(*this,*theFace,PerformNow);
        section.ComputePCurveOn1(Standard_True);
        section.Approximation(Standard_True);
        section.Build();
        TopoDS_Shape section_shape = section.Shape();

        for (TopExp_Explorer ex(section_shape, TopAbs_VERTEX); ex.More(); ex.Next())
        {
            nCount++;
        }
        if (nCount > 2)
        {   
            return Standard_True;
        }
    }

    /// If it is fused surface, use the original surfaces for collision detecting
    if(theFace->GetSameFaces().size()!=0)
    {
        Standard_Boolean bCollision = Standard_False;
        for (Standard_Integer i = 0 ; i < theFace->GetSameFaces().size(); i++)
        {
            McCadExtBndFace *pFace;
            pFace = theFace->GetSameFaces().at(i);
            bCollision = CalColliedFace(pFace);
            if (!bCollision)
            {
                continue;
            }
            else
            {
                return bCollision;
            }
        }
    }

    return Standard_False;
}



/** ********************************************************************
* @brief The vertexes of void box are in the convex solid or not, if
*        then means they have collision.
*
* @param McCadConvexSolid *& pSolid
* @return Standard_Boolean
*
* @date 20/3/2013
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadVoidCell::IsVertexInSolid(McCadConvexSolid *& pSolid)
{
    SetVertexList();   // Set the vertex of void cell
    BRepClass3d_SolidClassifier bsc3d(*pSolid);
    for (unsigned int i = 1; i <= m_VertexList->Length(); i++)
    {
        gp_Pnt pnt = m_VertexList->Value(i);
        bsc3d.Perform(pnt,1.0e-05);
        if ( bsc3d.State() == TopAbs_IN)
        {
            return Standard_True;
        }
    }
    return Standard_False;
}





/** ********************************************************************
* @brief Travelse the vertexes of void box and add into vertex list
*
* @param
* @return void
*
* @date 20/3/2013
* @author  Lei Lu
************************************************************************/
void McCadVoidCell::SetVertexList()
{
    if(m_VertexList->Length()!=0)
    {
        return;
    }

    Standard_Real dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
    m_bBox.Get(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);

    gp_Pnt Pnt1(dXmax,dYmax,dZmax);
    gp_Pnt Pnt2(dXmax,dYmax,dZmin);
    gp_Pnt Pnt3(dXmax,dYmin,dZmax);
    gp_Pnt Pnt4(dXmax,dYmin,dZmin);

    gp_Pnt Pnt5(dXmin,dYmin,dZmin);
    gp_Pnt Pnt6(dXmin,dYmax,dZmin);
    gp_Pnt Pnt7(dXmin,dYmin,dZmax);
    gp_Pnt Pnt8(dXmin,dYmax,dZmax);

    m_VertexList->Append(Pnt1);
    m_VertexList->Append(Pnt2);
    m_VertexList->Append(Pnt3);
    m_VertexList->Append(Pnt4);
    m_VertexList->Append(Pnt5);
    m_VertexList->Append(Pnt6);
    m_VertexList->Append(Pnt7);
    m_VertexList->Append(Pnt8);
}




/** ********************************************************************
* @brief If the point in void box or not
*
* @param gp_Pnt pnt
* @return Standard_Boolean
*
* @date 20/3/2013
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadVoidCell::IsPointInBBox(gp_Pnt pnt)
{
    Standard_Real dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
    m_bBox.Get(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);

    McCadGeomTool::SimplifyPoint(pnt);

    if (( pnt.X()>dXmin && pnt.X()<dXmax )
        &&( pnt.Y()>dYmin && pnt.Y()<dYmax )
        &&( pnt.Z()>dZmin && pnt.Z()<dZmax ))
    {        
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}




/** ********************************************************************
* @brief Get the MCNP cell expression of solid, this function is used
*        for calculating the length of void cell. If it is too long, then
*        split it into two parts.
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
TCollection_AsciiString McCadVoidCell::GetExpression()
{
    m_szExpression.Clear();
    m_szExpression += "(";
    int iInitSurfNum = McCadConvertConfig::GetInitSurfNum()-1;
    for (Standard_Integer i = 0; i < m_BndFaceList.size(); i++)
    {
        McCadExtBndFace * pExtFace = m_BndFaceList[i];

        int iFaceNum = pExtFace->GetFaceNum();
        iFaceNum > 0 ? iFaceNum += iInitSurfNum : iFaceNum -= iInitSurfNum;

        m_szExpression += TCollection_AsciiString(iFaceNum);
        m_szExpression += " ";
    }
    m_szExpression.Remove(m_szExpression.Length());     // Remove the last character " "
    m_szExpression += ")";
    m_szExpression += "&";

    for (Standard_Integer i = 0; i < m_CollisionList.size(); i++)
    {
        McCadVoidCollision *pCollision = m_CollisionList.at(i);
        m_szExpression += "(";
        for (Standard_Integer j = 0; j < pCollision->GetFaceNumList().size(); j++)
        {
            int iCollidFaceNum = pCollision->GetFaceNumList().at(j);
            iCollidFaceNum > 0 ? iCollidFaceNum += iInitSurfNum : iCollidFaceNum -= iInitSurfNum;

            int iNum = -1*(iCollidFaceNum);
            m_szExpression += TCollection_AsciiString(iNum);
            m_szExpression += ":";
        }

        if ( pCollision->GetAstFaceNumList().size() > 1)
        {
            m_szExpression += "(";

            for (Standard_Integer k = 0; k < pCollision->GetAstFaceNumList().size(); k++)
            {
                int iAstFaceNum  = pCollision->GetAstFaceNumList().at(k);
                iAstFaceNum > 0 ? iAstFaceNum += iInitSurfNum : iAstFaceNum -= iInitSurfNum;

                int iNum = -1*(iAstFaceNum);
                m_szExpression += TCollection_AsciiString(iNum);
                m_szExpression += " ";
            }

            m_szExpression.Remove(m_szExpression.Length()); // Remove the last character
            m_szExpression += ")";
        }

        m_szExpression += ")";
        m_szExpression += "&";
    }
    return m_szExpression;
}




/** ********************************************************************
* @brief Get the MCNP cell expression of out space, this is used for the
*        last outside void cell.
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
TCollection_AsciiString McCadVoidCell::GetOutVoidExpression()
{
    m_szExpression.Clear();
    m_szExpression += "(";

    for (Standard_Integer i = 0; i < m_BndFaceList.size(); i++)
    {
        McCadExtBndFace * pExtFace = m_BndFaceList[i];

        int iFaceNum = pExtFace->GetFaceNum();     
        iFaceNum *= -1;

        m_szExpression += TCollection_AsciiString(iFaceNum);
        m_szExpression += ":";
    }
    m_szExpression.Remove(m_szExpression.Length());
    m_szExpression += ")";

    return m_szExpression;
}



/** ********************************************************************
* @brief Get boundary faces of void box
*
* @param
* @return vector<McCadExtBndFace*>
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
vector<McCadExtBndFace*> McCadVoidCell::GetGeomFaceList()
{
    if(!m_BndFaceList.empty())
    {
        return m_BndFaceList;
    }

    TopExp_Explorer exp;

    TopoDS_Solid void_solid = BRepPrimAPI_MakeBox(m_MinPnt,m_MaxPnt).Solid();
    for (exp.Init(void_solid, TopAbs_FACE); exp.More(); exp.Next())
    {
        TopoDS_Face theFace = TopoDS::Face(exp.Current());
        McCadExtBndFace *pExtFace = new McCadExtBndFace(theFace);
        pExtFace->SetFaceNum(-1);
        m_BndFaceList.push_back(pExtFace);
    }
    return m_BndFaceList;
}




/** ********************************************************************
* @brief Split the void cell recursed if the length of expression is too
*        long.
*
* @param vector<McCadVoidCell*> & void_list
         McCadGeomData *pData
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadVoidCell::SplitVoidCell(vector<McCadVoidCell*> & void_list,
                                              McCadGeomData *pData)
{    
    /// In order to prevent the splitting from running inifinitly,
    /// If the split depth larger than the given limitation, stop the recursion.
    if (m_iSplitDepth == McCadConvertConfig::GetVoidDecomposeDepth())
    {
        return Standard_False;
    }

    //cout<<"Expression   "<<this->GetExpression().Length()<<"    "<<McCadConvertConfig::GetMaxDiscLength()<<endl;
    /// If the expression length is longer than limitation.
    if (this->GetExpression().Length() > McCadConvertConfig::GetMaxDiscLength())
    {
        /// Split the void into two parts with X, Y or Z plane, calculate the
        /// lengthes in three direction, choose the longest as splitting plane.
        gp_Pnt max_pntA,min_pntA;
        gp_Pnt max_pntB,min_pntB;

        double xLength = Abs(m_MaxPnt.X() - m_MinPnt.X());
        double yLength = Abs(m_MaxPnt.Y() - m_MinPnt.Y());
        double zLength = Abs(m_MaxPnt.Z() - m_MinPnt.Z());

        if (xLength >= yLength && xLength >= zLength)
        {
            m_SplitAxis = XAxis;
        }
        else if (yLength >= zLength)
        {
            m_SplitAxis = YAxis;
        }
        else
        {
            m_SplitAxis = ZAxis;
        }

        if(this->m_SplitAxis == XAxis)
        {
            Standard_Integer xMid = (m_MaxPnt.X()+ m_MinPnt.X())/2;

            max_pntA.SetCoord(xMid,m_MaxPnt.Y(),m_MaxPnt.Z());
            min_pntA.SetCoord(m_MinPnt.X(),m_MinPnt.Y(),m_MinPnt.Z());

            max_pntB.SetCoord(m_MaxPnt.X(),m_MaxPnt.Y(),m_MaxPnt.Z());
            min_pntB.SetCoord(xMid,m_MinPnt.Y(),m_MinPnt.Z());       
        }
        else if(this->m_SplitAxis == YAxis)
        {
            Standard_Integer yMid = (m_MaxPnt.Y()+ m_MinPnt.Y())/2;

            max_pntA.SetCoord(m_MaxPnt.X(),yMid,m_MaxPnt.Z());
            min_pntA.SetCoord(m_MinPnt.X(),m_MinPnt.Y(),m_MinPnt.Z());

            max_pntB.SetCoord(m_MaxPnt.X(),m_MaxPnt.Y(),m_MaxPnt.Z());
            min_pntB.SetCoord(m_MinPnt.X(),yMid,m_MinPnt.Z());

        }
        else if(this->m_SplitAxis == ZAxis)
        {
            Standard_Integer zMid = (m_MaxPnt.Z()+ m_MinPnt.Z())/2;

            max_pntA.SetCoord(m_MaxPnt.X(),m_MaxPnt.Y(),zMid);
            min_pntA.SetCoord(m_MinPnt.X(),m_MinPnt.Y(),m_MinPnt.Z());

            max_pntB.SetCoord(m_MaxPnt.X(),m_MaxPnt.Y(),m_MaxPnt.Z());
            min_pntB.SetCoord(m_MinPnt.X(),m_MinPnt.Y(),zMid);
        }

        /// Create two new splitted box with given splitting face.
        McCadVoidCell * pVoidA = new McCadVoidCell(BRepPrimAPI_MakeBox(min_pntA,max_pntA).Solid());
        McCadVoidCell * pVoidB = new McCadVoidCell(BRepPrimAPI_MakeBox(min_pntB,max_pntB).Solid());

        /// Transfer the collied solids to the child void boxes.
        pVoidA->SetCollidedSolidNumList(m_CollidedSolidNumList);
        pVoidB->SetCollidedSolidNumList(m_CollidedSolidNumList);

        pVoidA->CalColliedFaces(pData);
        pVoidB->CalColliedFaces(pData);

        pVoidA->SetSplitDepth(m_iSplitDepth+1);
        pVoidB->SetSplitDepth(m_iSplitDepth+1);

        if(!pVoidA->SplitVoidCell(void_list,pData)) /// If the void box can not be split further
        {
            void_list.push_back(pVoidA);
        }
        else
        {
            delete pVoidA;
            pVoidA = NULL;
        }

        if(!pVoidB->SplitVoidCell(void_list,pData)) /// If the void box can not be split further
        {
            void_list.push_back(pVoidB);           
        }
        else
        {
            delete pVoidB;
            pVoidB = NULL;
        }
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}



/** ********************************************************************
* @brief Set the split depth.
*
* @param Standard_Integer iSplitDepth
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadVoidCell::SetSplitDepth(Standard_Integer iSplitDepth)
{
    m_iSplitDepth = iSplitDepth;
}



/** ********************************************************************
* @brief Transfer the collision solid index list to the child void box.
*        thus reduce the calculate the collision with whole convex solids
*        again.
*
* @param vector<int> theCollidedSolidNumList
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadVoidCell::SetCollidedSolidNumList(vector<int> theCollidedSolidNumList )
{
    for(unsigned int i = 0; i < theCollidedSolidNumList.size(); i++)
    {
        m_CollidedSolidNumList.push_back(theCollidedSolidNumList.at(i));
    }
}





/** ********************************************************************
* @brief After sorting the surfaces, renumber the surfaces.
*        And meanwhile change the number according to the initial
*        surface number.
*
* @param McCadGeomData *pData
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadVoidCell::ChangeFaceNum(McCadGeomData *pData)
{
    for (Standard_Integer i = 0; i < m_BndFaceList.size(); i++)
    {
        McCadExtBndFace * pExtFace = m_BndFaceList[i];

        Standard_Integer iFaceNumOld = pExtFace->GetFaceNum();
        Standard_Integer iFaceNumNew = pData->GetNewFaceNum(abs(iFaceNumOld));
        pExtFace->SetFaceNum(iFaceNumNew);
    }

    for (Standard_Integer i = 0; i < m_CollisionList.size(); i++)
    {       
        McCadVoidCollision *pCollision = m_CollisionList.at(i);
        for (Standard_Integer j = 0; j < pCollision->GetFaceNumList().size(); j++)
        {
            Standard_Integer iFaceNumOld = pCollision->GetFaceNumList().at(j);
            Standard_Integer iFaceNumNew = pData->GetNewFaceNum(abs(iFaceNumOld));
            if(iFaceNumOld < 0)
            {
                iFaceNumNew *= -1;
            }            
            pCollision->ChangeFaceNum(j,iFaceNumNew);

        }

        for (Standard_Integer k = 0; k < pCollision->GetAstFaceNumList().size(); k++)
        {
            Standard_Integer iAstFaceNumOld = pCollision->GetAstFaceNumList().at(k);
            Standard_Integer iAstFaceNumNew = pData->GetNewFaceNum(abs(iAstFaceNumOld));
            if(iAstFaceNumOld < 0)
            {
               iAstFaceNumNew *= -1;
            }
            pCollision->ChangeAstFaceNum(k,iAstFaceNumNew);
        }
    }
}


/** ********************************************************************
* @brief Get the 6 boundary surfaces of void box
*
* @param
* @return vector<McCadExtBndFace*>
*
* @date 31/8/2012
* @modify 16/12/2013
* @author  Lei Lu
***********************************************************************/
vector<McCadExtBndFace*> McCadVoidCell::GetBndFaces()
{
    return m_BndFaceList;
}



/** ********************************************************************
* @brief Get the collisions with convex solids and their boundary
*        surfaces
*
* @param
* @return vector<McCadVoidCollision*>
*
* @date 31/8/2012
* @modify 16/12/2013
* @author  Lei Lu
***********************************************************************/
vector<McCadVoidCollision*> McCadVoidCell::GetCollisions()
{
    return m_CollisionList;
}
