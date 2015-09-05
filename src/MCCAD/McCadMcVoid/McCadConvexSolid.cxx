#include "McCadConvexSolid.hxx"
#include "McCadExtFace.hxx"

#include <McCadCSGTool.hxx>
#include <McCadGTOOL.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBndLib.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>

#include "McCadVoidCellManager.hxx"
#include "../McCadTool/McCadGeomTool.hxx"
#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadFuseSurface.hxx"



/** ********************************************************************
* @brief Construct function of McCadConvexSolid
*
* @param const TopoDS_Solid & theSolid
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
McCadConvexSolid::McCadConvexSolid(const TopoDS_Solid & theSolid):TopoDS_Solid(theSolid)
{
    //m_FaceList = new TopTools_HSequenceOfShape;           // New a sequence to store the faces
    m_DiscPntList = new TColgp_HSequenceOfPnt;      // New a sequence to store the descrete points
    m_EdgePntList = new TColgp_HSequenceOfPnt;      // New a sequence to store the descrete points

    GenFacesList(theSolid);     // Trace the boundary faces of solid, store in the list of faces.
    GenEdgePoints();            // Generate the discrete points of each edges.
    AddAuxSurfaces();           // Add assisted surfaces for concave curved surfaces.
}


McCadConvexSolid::~McCadConvexSolid()
{
    //m_FaceList->Clear();
    m_DiscPntList->Clear();
    m_EdgePntList->Clear();
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
//    TopExp_Explorer exp;    // Traverse the faces of input solid
//    for(exp.Init(theSolid, TopAbs_FACE); exp.More(); exp.Next())
//    {
//        TopoDS_Face theFace = TopoDS::Face(exp.Current());
//        McCadExtFace *pExtFace = new McCadExtFace(theFace);  // Create the extended faces
//        m_STLFaceList.push_back(pExtFace);
//        m_iNumOfFaces++;
//    }


    TopExp_Explorer exp;    // Traverse the faces of input solid

    vector<TopoDS_Face> face_list;
   // vector<TopoDS_Face> fused_face_list;
    vector<McCadExtFace*> fused_face_list;

    for(exp.Init(theSolid, TopAbs_FACE); exp.More(); exp.Next())
    {
        TopoDS_Face theFace = TopoDS::Face(exp.Current());
        face_list.push_back(theFace);
    }

    McCadFuseSurface Fuser;     // Fuse the surfaces with same geomtries and common edge
    /// Traverse the surfaces, find the same surfaces
    for(int i = 0; i < face_list.size(); i++ )
    {
        for(int j = i+1; j < face_list.size(); j++ )
        {            
            Fuser.SetSurfaces(face_list.at(i),face_list.at(j));
            if(Fuser.Fuse())
            {                
                McCadExtFace *pExtFace = new McCadExtFace(Fuser.GetNewSurf());
                pExtFace->MergeDscPnt(face_list.at(i));
                pExtFace->MergeDscPnt(face_list.at(j));

                face_list.erase(face_list.begin()+j);
                face_list.erase(face_list.begin()+i);
                fused_face_list.push_back(pExtFace);

                j -= 1;
                i -= 1;
                break;
            }
        }
    }

    /// Based on the new boundary surfaces, generate the McCadExtFace and add into surface list.
    for(int i = 0; i < face_list.size(); i++ )
    {
        TopoDS_Face theFace = face_list.at(i);
        McCadExtFace *pExtFace = new McCadExtFace(theFace);  // Create the extended faces
        m_STLFaceList.push_back(pExtFace);
        m_iNumOfFaces++;
    }

    for(int i = 0; i < fused_face_list.size(); i++ )
    {
        McCadExtFace *pExtFace = fused_face_list.at(i);
        m_STLFaceList.push_back(pExtFace);
        m_iNumOfFaces++;
    }

    face_list.clear();
    fused_face_list.clear();
}



/** ********************************************************************
* @brief Add Auxiliary Surface to each concave curved surface
*
* @param
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadConvexSolid::AddAuxSurfaces()
{
    for (int i = 0; i < m_STLFaceList.size(); i++)
    {
        McCadExtFace *pExtFace = m_STLFaceList.at(i);
        /* Judge whether need to add auxiliary faces when there are curved surfaces */
        if( pExtFace->IsConcaveCurvedFace() )      // Judge the face is concave curve surface or not
        {
            /* Get the auxiliary faces list */
            vector<McCadExtFace*> theAuxFaceList = pExtFace->GetAuxFaces();
            /* Judge the auxiliary face whether can be add into auxiliary face list.*/
            JudgeAuxFaces(theAuxFaceList,pExtFace);
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
TopoDS_Shape McCadConvexSolid::GetBntBoxShape()
{
    if(m_bHaveBndBox)                                       // If already have boundary box
    {
        return m_bBoxShape;
    }
    else
    {
        GetBntBox();
        return m_bBoxShape;
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
                McCadExtFace *pExtFace = m_STLFaceList[i];
                pExtFace->GetDiscPntList();   // Discrete each face of convex solid.
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
            McCadExtFace *pExtFace = m_STLFaceList[i];
            m_EdgePntList->Append(pExtFace->GetEdgePntList());    // Add the discrete point list of face into the one of solid
        }
    }
}



/** ********************************************************************
* @brief Judge the auxiliary face can be add into auxiliary face list
of this face
*
* @param vector<McCadExtFace*> & theAuxFaceList
* @param McCadExtFace *& theFace
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
void McCadConvexSolid::JudgeAuxFaces(vector<McCadExtFace*> & theAuxFaceList, McCadExtFace *& theFace)
{
    /* If the auxiliary face is sign-constant,it can be added into auxiliary face list */
    for (Standard_Integer i = 0; i < theAuxFaceList.size(); i++)
    {
        TopoDS_Face theAuxFace = *(theAuxFaceList[i]);

        BRepAdaptor_Surface BS(theAuxFace, Standard_True);
        GeomAdaptor_Surface theAdpSurface = BS.Surface();

        Standard_Integer iPosPnt = 0;
        Standard_Integer iNegPnt = 0;

        //Standard_Real aVal = 1.0
        for (Standard_Integer j = 1; j <= m_EdgePntList->Length(); j++)
        {
            /* Distinguish which side does the point located.*/
            Standard_Real aVal = McCadGTOOL::Evaluate(theAdpSurface, m_EdgePntList->Value(j));

            if (aVal > 1.0e-5)              // Point located on the positive side of face
            {
                iPosPnt ++;
            }
            else if (aVal < -1.0e-5)        // Point located on the negative side of face
            {
                iNegPnt ++;
            }

            if (iPosPnt > 0 && iNegPnt > 0) // i.e sign changing
            {
                (theAuxFaceList[i])->AddAttri(1);
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
vector<McCadExtFace*> McCadConvexSolid::GetSTLFaceList()
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


void McCadConvexSolid::DeleteRepeatFaces(Standard_Boolean bMergeDiscPnt)
{
    MergeBndFaces(bMergeDiscPnt);
    MergeBndAuxFaces();

    for (Standard_Integer i = 0; i < m_STLFaceList.size()-1; i++)
    {
        McCadExtFace *pLeftFace = m_STLFaceList.at(i);
        for (Standard_Integer j = i+1; j < m_STLFaceList.size(); j++)
        {
           McCadExtFace *pRightFace = m_STLFaceList.at(j);
           if (pLeftFace->HaveAuxSurf() && pRightFace->HaveAuxSurf())
           {
               MergeAuxFaces(pLeftFace,pRightFace);
           }
        }
    }
}


void McCadConvexSolid::MergeBndFaces(Standard_Boolean bMergeDiscPnt)
{
    for (Standard_Integer i = 0; i < m_STLFaceList.size()-1; i++)
    {
        McCadExtFace *pLeftFace = m_STLFaceList.at(i);
        for (Standard_Integer j = i+1; j < m_STLFaceList.size(); j++)
        {
            McCadExtFace *pRightFace = m_STLFaceList.at(j);
            if(pLeftFace->GetFaceNum() == pRightFace->GetFaceNum())
            {
                if (bMergeDiscPnt)
                {
                    pLeftFace->AddSameFaces(pRightFace);      // Add the same face into samefacelist for collision detection
                }

                m_STLFaceList.erase(m_STLFaceList.begin()+j);
                delete pRightFace;
                pRightFace = NULL;
                j--;
                continue;
            }
        }
    }
}


void McCadConvexSolid::MergeAuxFaces(McCadExtFace *& pLeftFace, McCadExtFace *& pRightFace)
{
    for (Standard_Integer i = 0; i < pLeftFace->GetAuxFaces().size(); i++)
    {
        McCadExtFace *pLeftAuxFace = pLeftFace->GetAuxFaces().at(i);
        for (Standard_Integer j = 0; j < pRightFace->GetAuxFaces().size(); j++)
        {
            McCadExtFace *pRightAuxFace = pRightFace->GetAuxFaces().at(j);
            if(pLeftAuxFace->GetFaceNum() == pRightAuxFace->GetFaceNum())
            {
                pRightFace->RemoveAuxFace(j);
                j--;
            }           
        }
    }
}

void McCadConvexSolid::MergeBndAuxFaces()
{
    for (Standard_Integer i = 0; i < m_STLFaceList.size(); i++)
    {        
        for (Standard_Integer j = 0; j < m_STLFaceList.size(); j++)
        {
            if (i == j)
            {
                continue;
            }
            McCadExtFace *pLeftFace = m_STLFaceList.at(i);
            McCadExtFace *pRightFace = m_STLFaceList.at(j);
            for (Standard_Integer k = 0; k < pRightFace->GetAuxFaces().size(); k++)
            {
                McCadExtFace *pAuxFace = pRightFace->GetAuxFaces().at(k);
                if(pLeftFace->GetFaceNum() == pAuxFace->GetFaceNum())
                {                    
                    pRightFace->RemoveAuxFace(k);
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

    TCollection_AsciiString szAuxFaceUni = "";
    TCollection_AsciiString szAuxFaceSub = "";

    int iInitSurfNum = McCadConvertConfig::GetInitSurfNum()-1;

    for (Standard_Integer i = 0; i < m_STLFaceList.size(); i++)
    {

        McCadExtFace * pExtFace = m_STLFaceList[i];
        /* if the orientation of face is minus,add "-" before the face number */
        //if(pExtFace->GetFaceOrientation() == MINUS)
        //{
           //m_szExpression += "-";
        //}
        int iFaceNum = pExtFace->GetFaceNum();
        iFaceNum > 0 ? iFaceNum += iInitSurfNum : iFaceNum -= iInitSurfNum;
        m_szExpression += TCollection_AsciiString(iFaceNum);
        m_szExpression += " ";

        for(Standard_Integer j = 0; j < pExtFace->GetAuxFaces().size(); j++)
        {
            McCadExtFace *pAuxFace = pExtFace->GetAuxFaces().at(j);
            int iAuxFaceNum = pAuxFace->GetFaceNum();
            iAuxFaceNum > 0 ? iAuxFaceNum += iInitSurfNum : iAuxFaceNum -= iInitSurfNum;
            if (pAuxFace->GetAttri() == 1)
            {
                szAuxFaceUni += TCollection_AsciiString(iAuxFaceNum);
                szAuxFaceUni += ":";
            }
            else
            {
                szAuxFaceSub += TCollection_AsciiString(iAuxFaceNum);
                szAuxFaceSub += " ";
            }
        }
    }

    if (!szAuxFaceSub.IsEmpty())
    {
        szAuxFaceSub.Remove(szAuxFaceSub.Length());
        if (szAuxFaceUni.Search(" ") != -1 )
        {
            m_szExpression += " (";
            m_szExpression += szAuxFaceSub;
            m_szExpression += ") ";
        }
        else
        {
            m_szExpression += szAuxFaceSub;
            m_szExpression += " ";
        }
    }

    if (!szAuxFaceUni.IsEmpty())
    {
        szAuxFaceUni.Remove(szAuxFaceUni.Length());
        if (szAuxFaceUni.Search(":") != -1 )
        {
            m_szExpression += "(";
            m_szExpression += szAuxFaceUni;
            m_szExpression += ") ";
        }
        else
        {
            m_szExpression += szAuxFaceUni;
        }
    }

    return m_szExpression;
}


Standard_Real McCadConvexSolid::GetVolume()
{
    return m_fVolume;
}


void McCadConvexSolid::SetVolume(Standard_Real fVolume)
{
    m_fVolume = fVolume;
}


void McCadConvexSolid::ChangeFaceNum(McCadGeomData * pData)
{   
    for (Standard_Integer i = 0; i < m_STLFaceList.size(); i++)
    {
        McCadExtFace * pExtFace = m_STLFaceList[i];

        Standard_Integer iFaceNumOld = pExtFace->GetFaceNum();
        Standard_Integer iFaceNumNew = pData->GetNewFaceNum(abs(iFaceNumOld));

        pExtFace->SetFaceNum(iFaceNumNew);

        if (pExtFace->HaveAuxSurf())
        {
            for(Standard_Integer j = 0; j < pExtFace->GetAuxFaces().size(); j++)
            {
                McCadExtFace *pAuxFace = pExtFace->GetAuxFaces().at(j);
                iFaceNumOld = pAuxFace->GetFaceNum();
                iFaceNumNew = pData->GetNewFaceNum(abs(iFaceNumOld));

                pAuxFace->SetFaceNum(iFaceNumNew);

                /* Test Code */
                //cout<<"old:"<<iFaceNumOld << "   new:"<<iFaceNumNew<<endl;
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
vector<McCadExtFace*> McCadConvexSolid::GetFaces()
{
    return m_STLFaceList;
}
