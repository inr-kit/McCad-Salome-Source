#include "McCadConvexSolid.hxx"
#include "McCadExtBndFace.hxx"

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBndLib.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>

#include "McCadStitchSurfaces.hxx"
#include "McCadVoidCellManager.hxx"

#include "../McCadTool/McCadGeomTool.hxx"
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadEvaluator.hxx"


McCadConvexSolid::McCadConvexSolid(const TopoDS_Solid & theSolid):TopoDS_Solid(theSolid)
{
    //m_FaceList = new TopTools_HSequenceOfShape;           // New a sequence to store the faces
    m_DiscPntList = new TColgp_HSequenceOfPnt;      // New a sequence to store the descrete points
    m_EdgePntList = new TColgp_HSequenceOfPnt;      // New a sequence to store the descrete points

    GenFacesList(theSolid);     // Trace the boundary faces of solid, store in the list of faces.    
    GenEdgePoints();            // Generate the discrete points of each edges.   
    AddAstSurfaces();           // Add assisted surfaces for concave curved surfaces.  
}


McCadConvexSolid::~McCadConvexSolid()
{    
    m_EdgePntList->Clear();
    m_DiscPntList->Clear();
    vector<McCadExtBndFace *>::iterator iterFace;
    for (iterFace = m_STLFaceList.begin(); iterFace != m_STLFaceList.end(); ++iterFace)
    {
        if(*iterFace != NULL)
        {
            delete *iterFace;
            *iterFace = NULL;
        }
    }
    m_STLFaceList.clear();
}



/** ********************************************************************
* @brief Generate the face list
*
* @param const TopoDS_Solid & theSolid
* @return void
*
* @date 31/8/2012
* @modify  4/2/2014  fuse the same boundary surfaces
* @author  Lei Lu
************************************************************************/
void McCadConvexSolid::GenFacesList(const TopoDS_Solid & theSolid)
{
    TopExp_Explorer exp;    // Traverse the faces of input solid

    /// Based on the boundary surfaces, generate the McCadExtFace and add into surface list.
    for(exp.Init(theSolid, TopAbs_FACE); exp.More(); exp.Next())
    {
        TopoDS_Face theFace = TopoDS::Face(exp.Current());

        McCadExtBndFace *pExtFace = new McCadExtBndFace(theFace);  // Create the extended faces
        m_STLFaceList.push_back(pExtFace);
        m_iNumOfFaces++;
    }

    McCadStitchSurfaces tailor;     // stitch the surfaces with same geomtries and common edge
    /// Traverse the surfaces, find the same surfaces
    for( unsigned int i = 0; i < m_STLFaceList.size()-1; i++ )
    {
        McCadExtBndFace *pExtFaceA = m_STLFaceList.at(i);
        for(unsigned int j = i+1; j < m_STLFaceList.size(); j++ )
        {
            McCadExtBndFace *pExtFaceB = m_STLFaceList.at(j);
            if(tailor.CanbeStitched(pExtFaceA,pExtFaceB))
            {
                /// TopoDS_Face newGenFace = Fuser.Fuse(pExtFaceA,pExtFaceB);
                TopoDS_Face newGenFace = tailor.Stitch(pExtFaceA,pExtFaceB);
                McCadExtBndFace *pExtFace = new McCadExtBndFace(newGenFace);

                ///Add the discreted points of original faces into new surface created.
                pExtFace->Merge(pExtFaceA);
                pExtFace->Merge(pExtFaceB);

                m_STLFaceList.push_back(pExtFace);

                m_STLFaceList.erase(m_STLFaceList.begin()+j);
                m_STLFaceList.erase(m_STLFaceList.begin()+i);

                j --;
                i --;
                m_iNumOfFaces--;
                break;
            }
        }
    }
}



/** ********************************************************************
* @brief Add Assisted Surface to each concave curved surface
*
* @param
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadConvexSolid::AddAstSurfaces()
{
    for (int i = 0; i < m_STLFaceList.size(); i++)
    {
        McCadExtBndFace *pExtFace = m_STLFaceList.at(i);
        /// Judge whether need to add assisted faces when there are curved surfaces */
        if( pExtFace->IsConcaveCurvedFace() )      // Judge the face is concave curve surface or not
        {
            /// Get the auxiliary faces list
            vector<McCadExtAstFace*> theAstFaceList = pExtFace->GetAstFaces();
            /// Judge the auxiliary face whether can be add into auxiliary face list.*/
            JudgeAssistFaces(theAstFaceList);
        }
    }
}




/** ********************************************************************
* @brief Get the boundary box of solid
*
* @param
* @return TopoDS_Shape
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Bnd_Box McCadConvexSolid::GetBntBox()
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

        gp_Pnt MaxPnt(fXmax, fYmax, fZmax);                         // The max point of the boundary box
        gp_Pnt MinPnt(fXmin, fYmin, fZmin);                         // The min point of the boundary box

        m_bBoxShape = BRepPrimAPI_MakeBox(MinPnt,MaxPnt).Shape();   // Create the shape of boundary box
        m_bHaveBndBox = Standard_True;                              // Already have boundary box

        return m_bBox;
    }
}




/** ********************************************************************
* @brief Traverse the faces of solid and get the discreted poins list
*
* @param
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadConvexSolid::GenDescPoints(Standard_Boolean bGenVoid)
{
    /* Generate sample points of the each face */
    if (m_DiscPntList->IsEmpty())
    {
        if (bGenVoid)
        {
            for(Standard_Integer i = 0; i < m_STLFaceList.size(); i++)
            {
                McCadExtBndFace *pExtFace = m_STLFaceList[i];
                m_DiscPntList->Append(pExtFace->GetEdgePntList());    // Add the discrete point list of face into the one of solid
            }
        }
    }    
}







/** ********************************************************************
* @brief Traverse the faces of solid and get the discreted poins list
*
* @param
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadConvexSolid::GenEdgePoints()
{
    if (m_EdgePntList->IsEmpty())
    {
        for(Standard_Integer i = 0; i < m_STLFaceList.size(); i++)
        {
            McCadExtBndFace *pExtFace = m_STLFaceList[i];
            m_EdgePntList->Append(pExtFace->GetEdgePntList());    // Add the discrete point list of edge into the one of solid
        }
    }
}




/** ********************************************************************
* @brief Judge the assisted face can be add into auxiliary face list
         of this face
*
* @param vector<McCadExtFace*> & theAuxFaceList
* @param
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadConvexSolid::JudgeAssistFaces(vector<McCadExtAstFace*> & theAstFaceList)
{
    /// If the auxiliary face is not splitting surface,it can be added into auxiliary face list directly
    for (Standard_Integer i = 0; i < theAstFaceList.size(); i++)
    {
        McCadExtAstFace *pExtAstFace = theAstFaceList.at(i);

        Standard_Integer iPosPnt = 0;
        Standard_Integer iNegPnt = 0;

        //Standard_Real aVal = 1.0
        for (Standard_Integer j = 1; j <= m_EdgePntList->Length(); j++)
        {
            /* Distinguish which side does the point located.*/
            Standard_Real aVal = McCadEvaluator::Evaluate(pExtAstFace->GetAdpFace(), m_EdgePntList->Value(j));

            if (aVal > 1.0e-3)              // Point located on the positive side of face
            {
                iPosPnt ++;
            }
            else if (aVal < -1.0e-3)        // Point located on the negative side of face
            {
                iNegPnt ++;
            }

            if (iPosPnt > 0 && iNegPnt > 0) // i.e sign changing, the assited surface can split solid
            {
                cout<<"Splitting assisted surface"<<endl;
                pExtAstFace->SetSplit(Standard_True);
                break;
            }
        }        
    }
    return;
}


/** ********************************************************************
* @brief Get the face list of solid
*
* @param
* @return vector<McCadExtFace*>
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
vector<McCadExtBndFace*> McCadConvexSolid::GetSTLFaceList()
{
    return m_STLFaceList;
}



/** ********************************************************************
* @brief Discrete the faces and get the list of discreted points.
*
* @param
* @return Handle_TColgp_HSequenceOfPnt
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
Handle_TColgp_HSequenceOfPnt McCadConvexSolid::GetDiscPntList()
{
    return m_DiscPntList;
}





/** ********************************************************************
* @brief Traversal the boundary surfaces and remove the repeated surfaces
*        if do not need generate the void space, then it is unnecssary to
*        combine the sample points or each surface, if need to generate
*        void, then combine the sample points of two repeated surfaces.
* @note  This function is not be used again. Instead,the surfaces will be
*        merged when they are wrote into input file.
* @param Standard_Boolean bMergeDiscPnt
* @return void
*
* @date 31/8/2012
* @modify 29/07/2016
* @author  Lei Lu
***********************************************************************/
void McCadConvexSolid::DeleteRepeatFaces(Standard_Boolean bMergeDiscPnt)
{
    MergeBndFaces(bMergeDiscPnt);   /// Find the repeated boundary surfaces
    MergeBndAstFaces();             /// Find the same boundary surface and assisted surface

    /// Remove the repeated assisted surfaces
    for (Standard_Integer i = 0; i < m_STLFaceList.size()-1; i++)
    {
        McCadExtBndFace *pLeftFace = m_STLFaceList.at(i);
        for (Standard_Integer j = i+1; j < m_STLFaceList.size(); j++)
        {
           McCadExtBndFace *pRightFace = m_STLFaceList.at(j);
           if (pLeftFace->HaveAstSurf() && pRightFace->HaveAstSurf())
           {
               MergeAstFaces(pLeftFace,pRightFace);
           }
        }
    }
}



/** ********************************************************************
* @brief If two boundary surfaces are same surfaces, then merge them, and
*        transmit the sample points of one surface to another.
* @param Standard_Boolean bMergeDiscPnt
* @return void
*
* @date 31/8/2012
* @modify 29/07/2016
* @author  Lei Lu
***********************************************************************/
void McCadConvexSolid::MergeBndFaces(Standard_Boolean bMergeDiscPnt)
{
    for (Standard_Integer i = 0; i < m_STLFaceList.size()-1; i++)
    {
        McCadExtBndFace *pLeftFace = m_STLFaceList.at(i);
        for (Standard_Integer j = i+1; j < m_STLFaceList.size(); j++)
        {
            McCadExtBndFace *pRightFace = m_STLFaceList.at(j);
            if(pLeftFace->GetFaceNum() == pRightFace->GetFaceNum())
            {
               // Add the same face into samefacelist for collision detection
               pLeftFace->AddSameFaces(pRightFace);
               m_STLFaceList.erase(m_STLFaceList.begin()+j);
               j--;
               continue;
            }
        }
    }
}




/** ********************************************************************
* @brief Merge the assisted surface of two surfaces. If the two assisted
*        surfaces are same surfaces then remove one of them.
* @param
* @return void
*
* @date 31/8/2012
* @modify 29/07/2016
* @author  Lei Lu
***********************************************************************/
void McCadConvexSolid::MergeAstFaces(McCadExtBndFace *& pLeftFace, McCadExtBndFace *& pRightFace)
{
    for (Standard_Integer i = 0; i < pLeftFace->GetAstFaces().size(); i++)
    {
        McCadExtAstFace *pLeftAstFace = pLeftFace->GetAstFaces().at(i);
        for (Standard_Integer j = 0; j < pRightFace->GetAstFaces().size(); j++)
        {
            McCadExtAstFace *pRightAstFace = pRightFace->GetAstFaces().at(j);
            if(pLeftAstFace->GetFaceNum() == pRightAstFace->GetFaceNum())
            {
                pRightFace->RemoveAstFace(j);
                j--;
            }           
        }
    }
}




/** ********************************************************************
* @brief If a boundary surface is same as an assisted surface, then remove
*        the assisted surface.
* @param
* @return void
*
* @date 31/8/2012
* @modify 29/07/2016
* @author  Lei Lu
***********************************************************************/
void McCadConvexSolid::MergeBndAstFaces()
{
    for (unsigned int i = 0; i < m_STLFaceList.size(); i++)
    {        
        for (unsigned int  j = 0; j < m_STLFaceList.size(); j++)
        {
            if (i == j)
            {
                continue;
            }
            McCadExtBndFace *pLeftFace = m_STLFaceList.at(i);
            McCadExtBndFace *pRightFace = m_STLFaceList.at(j);
            for (unsigned int  k = 0; k < pRightFace->GetAstFaces().size(); k++)
            {
                McCadExtAstFace *pAstFace = pRightFace->GetAstFaces().at(k);
                if(pLeftFace->GetFaceNum() == pAstFace->GetFaceNum())
                {                    
                    pRightFace->RemoveAstFace(k);
                    k--;
                    continue;
                }
            }
        }
    }
}



/** ********************************************************************
* @brief Get the MCNP cell expression of solid
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
TCollection_AsciiString McCadConvexSolid::GetExpression()
{
    if (!m_szExpression.IsEmpty())
    {
        return m_szExpression;
    }

    TCollection_AsciiString szAstFaceUni = "";
    TCollection_AsciiString szAstFaceSub = "";

    int iInitSurfNum = McCadConvertConfig::GetInitSurfNum()-1;

    for (Standard_Integer i = 0; i < m_STLFaceList.size(); i++)
    {

        McCadExtBndFace * pExtFace = m_STLFaceList[i];
        /* if the orientation of face is minus,add "-" before the face number */
        //if(pExtFace->GetFaceOrientation() == MINUS)
        //{
           //m_szExpression += "-";
        //}
        int iFaceNum = pExtFace->GetFaceNum();
        iFaceNum > 0 ? iFaceNum += iInitSurfNum : iFaceNum -= iInitSurfNum;
        m_szExpression += TCollection_AsciiString(iFaceNum);
        m_szExpression += " ";

        for(Standard_Integer j = 0; j < pExtFace->GetAstFaces().size(); j++)
        {
            McCadExtAstFace *pAstFace = pExtFace->GetAstFaces().at(j);
            int iAstFaceNum = pAstFace->GetFaceNum();
            iAstFaceNum > 0 ? iAstFaceNum += iInitSurfNum : iAstFaceNum -= iInitSurfNum;
            if (pAstFace->IsSplitFace())
            {
                szAstFaceUni += TCollection_AsciiString(iAstFaceNum);
                szAstFaceUni += ":";
            }
            else
            {
                szAstFaceSub += TCollection_AsciiString(iAstFaceNum);
                szAstFaceSub += " ";
            }
        }
    }

    if (!szAstFaceSub.IsEmpty())
    {
        szAstFaceSub.Remove(szAstFaceSub.Length());
        if (szAstFaceUni.Search(" ") != -1 )
        {
            m_szExpression += " (";
            m_szExpression += szAstFaceSub;
            m_szExpression += ") ";
        }
        else
        {
            m_szExpression += szAstFaceSub;
            m_szExpression += " ";
        }
    }

    if (!szAstFaceUni.IsEmpty())
    {
        szAstFaceUni.Remove(szAstFaceUni.Length());
        if (szAstFaceUni.Search(":") != -1 )
        {
            m_szExpression += "(";
            m_szExpression += szAstFaceUni;
            m_szExpression += ") ";
        }
        else
        {
            m_szExpression += szAstFaceUni;
        }
    }

    return m_szExpression;
}




/** ********************************************************************
* @brief Get the volume of solid
*
* @param
* @return Standard_Real
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
Standard_Real McCadConvexSolid::GetVolume()
{
    return m_fVolume;
}



/** ********************************************************************
* @brief Set the volume of solid
*
* @param Standard_Real fVolume
* @return
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadConvexSolid::SetVolume(Standard_Real fVolume)
{
    m_fVolume = fVolume;
}

/** ********************************************************************
* @brief After the surfaces sorting, the surface numbers will be changed
*        according to the original surface number, change them to be new
*        one. The new surface list is stored at McCadGeomData with map,
*        give the old surface number, a new number will be given back.
* @param McCadGeomData * pData
* @return
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadConvexSolid::ChangeFaceNum(McCadGeomData * pData)
{   
    for (Standard_Integer i = 0; i < m_STLFaceList.size(); i++)
    {
        McCadExtBndFace * pExtFace = m_STLFaceList[i];

        Standard_Integer iFaceNumOld = pExtFace->GetFaceNum();
        Standard_Integer iFaceNumNew = pData->GetNewFaceNum(abs(iFaceNumOld));

        pExtFace->SetFaceNum(iFaceNumNew);

        if (pExtFace->HaveAstSurf())
        {
            for(Standard_Integer j = 0; j < pExtFace->GetAstFaces().size(); j++)
            {
                McCadExtAstFace *pAstFace = pExtFace->GetAstFaces().at(j);
                iFaceNumOld = pAstFace->GetFaceNum();
                iFaceNumNew = pData->GetNewFaceNum(abs(iFaceNumOld));

                pAstFace->SetFaceNum(iFaceNumNew);
            }
        }
    }
}



/** ********************************************************************
* @brief Get the boundary surface list
*
* @param
* @return vector<McCadExtBndFace*>
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
vector<McCadExtBndFace*> McCadConvexSolid::GetFaces()
{
    return m_STLFaceList;
}
