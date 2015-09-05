#include "McCadVoidCell.hxx"

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepBndLib.hxx>
#include <TopoDS.hxx>
#include <assert.h>
#include <omp.h>
#include <Standard.hxx>
#include <McCadGTOOL.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <STEPControl_Writer.hxx>
#include <McCadCSGUtil_SolidBuilder.hxx>

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
    GetBntBox();
    GetGeomFaceList();
    m_VertexList = new TColgp_HSequenceOfPnt;
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
void McCadVoidCell::AddColliedSolidNum(Standard_Integer iNum)
{
    m_CollidedSolidNumList.push_back(iNum);
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
vector<int> McCadVoidCell::GetColliedSolidList()
{
    return m_CollidedSolidNumList;
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
* @param
* @return TopoDS_Shape
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Bnd_Box McCadVoidCell::SetBntBox(Standard_Real fXmin, Standard_Real fYmin, Standard_Real fZmin,
                                 Standard_Real fXmax, Standard_Real fYmax, Standard_Real fZmax)
{
    m_bBox.Update(fXmin, fYmin, fZmin, fXmax, fYmax, fZmax);
    m_bHaveBndBox = Standard_True;                              // Already have boundary box

    m_MaxPnt.SetCoord(fXmax, fYmax, fZmax);                     // The max point of the boundary box
    m_MinPnt.SetCoord(fXmin, fYmin, fZmin);                     // The min point of the boundary box

    return m_bBox;
}


/** ********************************************************************
* @brief Set the boundary box of solid
*
* @param
* @return TopoDS_Shape
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadVoidCell::CalColliedFaces(McCadGeomData * pData)
{
    int b = 0;

    for (unsigned int i = 0; i < m_CollidedSolidNumList.size(); i++)
    {
        int index = m_CollidedSolidNumList.at(i);
        McCadConvexSolid * pSolid = (pData->GetConvexSolid()).at(index);
        assert(pSolid);

        Bnd_Box bbox_solid = pSolid->GetBntBox();
        Bnd_Box bbox_void = this->GetBntBox();

        if(!CalColliedBox(bbox_solid))
        {
            m_CollidedSolidNumList.erase(m_CollidedSolidNumList.begin()+i);
            i--;
            continue;
        }

        //if(bbox_void.IsOut(bbox_solid))
        //{
        //    m_CollidedSolidNumList.erase(m_CollidedSolidNumList.begin()+i);
        //    i--;
        //    continue;
        //}

        COLLISION tmpCollision;
        tmpCollision.SolidNum = index+1;
        vector<int> tmpAuxFaceList;

        for (unsigned int j = 0; j < pSolid->GetSTLFaceList().size(); j++)
        {
            McCadExtFace *pFace = pSolid->GetSTLFaceList()[j];
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

            if (!bCollied)
            {
                continue;
            }

            tmpCollision.FaceList.push_back(pFace->GetFaceNum());            
            if(pFace->HaveAuxSurf())
            {                
                vector<int> auxface_list_solid;
                for (Standard_Integer k = 0; k < pFace->GetAuxFaces().size(); k++)
                {
                    McCadExtFace *pAuxFace = pFace->GetAuxFaces().at(k);                    
                    if(pAuxFace->GetAttri() == 1)
                    {
                        auxface_list_solid.push_back(pAuxFace->GetFaceNum());
                    }
                    else
                    {
                        tmpCollision.FaceList.push_back(pAuxFace->GetFaceNum());
                    }
                }

                if (auxface_list_solid.size() == 1)
                {
                    tmpAuxFaceList.push_back(auxface_list_solid.at(0));
                }
                else if (auxface_list_solid.size() > 1)
                {
                    tmpCollision.AuxFaceList.push_back(auxface_list_solid);
                }
            }            
        }

        if ( tmpAuxFaceList.size() != 0)
        {
            tmpCollision.AuxFaceList.push_back(tmpAuxFaceList);
        }

        if(tmpCollision.FaceList.size() != 0)
        {
            m_Collision.push_back(tmpCollision);
            continue;
        }

        // Advanced collision detect.Judge the vertex of void cell is in the solid or not.
        if(IsVertexInSolid(pSolid))
        {            
            vector<int> tmpAuxFaceList;
            for (unsigned int j = 0; j < pSolid->GetSTLFaceList().size(); j++)
            {
                McCadExtFace *pFace = pSolid->GetSTLFaceList().at(j);
                tmpCollision.FaceList.push_back(pFace->GetFaceNum());                
                if(pFace->HaveAuxSurf())
                {
                   vector<int> auxface_list_solid;
                   for (Standard_Integer k = 0; k < pFace->GetAuxFaces().size(); k++)
                   {
                       McCadExtFace *pAuxFace = pFace->GetAuxFaces().at(k);
                       if(pAuxFace->GetAttri() == 1)
                       {
                           auxface_list_solid.push_back(pAuxFace->GetFaceNum());
                       }
                       else
                       {
                           tmpCollision.FaceList.push_back(pAuxFace->GetFaceNum());
                       }
                   }

                   if (auxface_list_solid.size() == 1)
                   {
                       tmpAuxFaceList.push_back(auxface_list_solid.at(0));
                   }
                   else if (auxface_list_solid.size() > 1)
                   {
                       tmpCollision.AuxFaceList.push_back(auxface_list_solid);
                   }
                }               
            }

            if ( tmpAuxFaceList.size() != 0)
            {
                tmpCollision.AuxFaceList.push_back(tmpAuxFaceList);
            }

            if(tmpCollision.FaceList.size() != 0)
            {
                m_Collision.push_back(tmpCollision);
            }
        }
    }

    /* Print the collision information */
  /*  if (m_Collision.size() == 0)
    {
        cout<<" null collision"<<endl;
        return;
    }

    for (unsigned int i = 0; i < m_Collision.size(); i++)
    {
       cout<<endl;
       cout<<"Solid "<<(m_Collision[i]).SolidNum<<":";
       for (unsigned int j = 0; j < m_Collision[i].FaceList.size(); j++)
       {
           cout<<"  Face "<<Abs((m_Collision[i]).FaceList[j]);
       }
    }
    cout<<endl;*/
}



/** ********************************************************************
* @brief Calculate the relationship between points and void boxes, if the
*        the point is in the box, it means the face and void box have
*        collision.
*
* @param
* @return TopoDS_Shape
*
* @date 20/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadVoidCell::CalColliedPoints(Handle_TColgp_HSequenceOfPnt point_list)
{
    //Handle_TColgp_HSequenceOfPnt point_list = theFace->GetDiscPntList();
    Standard_Boolean bInVoid = Standard_False;

    for (unsigned int i = 1; i <= point_list->Length(); i++)
    {
        gp_Pnt pnt = point_list->Value(i);
        //if(m_bBox.IsOut(pnt))
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
* @brief Calculate the two boxes are connected and next to each other
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

//    gp_Pnt v1(xMin, yMin, zMin);
//    gp_Pnt v2(xMin, yMin, zMax);
//    gp_Pnt v3(xMin, yMax, zMax);
//    gp_Pnt v4(xMax, yMax, zMax);
//    gp_Pnt v5(xMax, yMin, zMax);
//    gp_Pnt v6(xMax, yMax, zMin);
//    gp_Pnt v7(xMax, yMin, zMin);
//    gp_Pnt v8(xMin, yMax, zMin);

    m_bBox.SetGap(0.0);
    Standard_Real xMin2, yMin2, zMin2, xMax2, yMax2, zMax2;
    m_bBox.Get(xMin2, yMin2, zMin2, xMax2, yMax2, zMax2);


    if ( Abs(xMin1-xMax2)<1.0e-7 || Abs(xMax1-xMin2)<1.0e-7)
    {
        bConnected = Standard_True;
    }

    if ( Abs(yMin1-yMax2)<1.0e-2 || Abs(yMax1 - yMin2)<1.0e-2)
    {
        //cout<<yMin1<<" "<<yMax1<<" "<<yMin2<<" "<<yMax2<<endl;
        bConnected = Standard_True;
    }

    if ( Abs(zMin1-zMax2)<1.0e-7 || Abs(zMax1 - zMin2)<1.0e-7)
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


Standard_Boolean McCadVoidCell::PntInBox(gp_Pnt &pnt, Bnd_Box & box)
{
    box.SetGap(0.0);
    Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
    box.Get(xMin, yMin, zMin, xMax, yMax, zMax);

    McCadGeomTool::SimplifyPoint(pnt);

    Standard_Real px = pnt.X();
    Standard_Real py = pnt.Y();
    Standard_Real pz = pnt.Z();

    if ((px < xMax) && (px > xMin))
    {
        if ((py < yMax) && (py > yMin))
        {
            if ((pz < zMax) && (pz > zMin))
            {
                return Standard_True;
            }
        }
    }

    return Standard_False;
}



/*Standard_Boolean McCadVoidCell::CalColliedVertex(McCadExtFace *& theFace)
{
    GetVertexList();   // Get the vertex of void cell
    Standard_Boolean bInSolid = Standard_False;
    int iSide = 1;
    for (unsigned int i = 1; i <= m_VertexList->Length(); i++)
    {
        gp_Pnt pnt = m_VertexList->Value(i);
        if (i == 1)
        {
            iSide = SideofFace(pnt,theFace);
            continue;
        }

        if(iSide != SideofFace(pnt,theFace))
        {
            bInSolid = Standard_True;
            break;
        }
    }
    return bInSolid;
}*/



/** ********************************************************************
* @brief Further collision detection, use the boolean operation between
*        face and void box, if they have common section, then they are
*        collied.
*        Because same faces have been merged, so the merged same surfaces
*        also have to be used for detecting the collision.
*
* @param
* @return TopoDS_Shape
*
* @date 20/3/2013
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadVoidCell::CalColliedFace(McCadExtFace *& theFace)
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
            //cout<<"   "<<nCount<<endl
            //STEPControl_Writer wrt;
            //wrt.Transfer(section_shape, STEPControl_AsIs);
            //wrt.Write("problem.stp");
            return Standard_True;
        }


    }

   // if(theFace->IsFusedFace())
    if(theFace->GetSameFaces().size()!=0)
    {
        Standard_Boolean bCollision = Standard_False;
        for (Standard_Integer i = 0 ; i < theFace->GetSameFaces().size(); i++)
        {
            McCadExtFace *pFace;
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




Standard_Integer McCadVoidCell::SideofFace(gp_Pnt pnt, McCadExtFace *& theFace)
{
    TopLoc_Location loc;
    Handle(Geom_Surface) geom_surface = BRep_Tool::Surface(*theFace, loc);
    GeomAdaptor_Surface surf_adoptor(geom_surface);
    Standard_Real aVal = McCadGTOOL::Evaluate(surf_adoptor,pnt);

    if(aVal > 0.0 )
    {
        return 1;
    }
    else if (aVal < 0.0)
    {
        return -1;
    }
    else if (fabs(aVal - 0.0) < 1.0e-7)
    {
        return 0;
    }
}



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
* @brief Get the MCNP cell expression of solid
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
    //m_szExpression += "(Defined Boudary Box)&(";
    m_szExpression += "(";
    int iInitSurfNum = McCadConvertConfig::GetInitSurfNum()-1;
    for (Standard_Integer i = 0; i < m_FaceList.size(); i++)
    {
        McCadExtFace * pExtFace = m_FaceList[i];
        /* if the orientation of face is minus,add "-" before the face number */
        /*if(pExtFace->GetFaceOrientation() == MINUS)
        {
           //m_szExpression += "-";
        }*/

        int iFaceNum = pExtFace->GetFaceNum();
        iFaceNum > 0 ? iFaceNum += iInitSurfNum : iFaceNum -= iInitSurfNum;

        m_szExpression += TCollection_AsciiString(iFaceNum);
        m_szExpression += " ";
    }
    m_szExpression.Remove(m_szExpression.Length());     // Remove the last character " "
    m_szExpression += ")";
    m_szExpression += "&";

    for (Standard_Integer i = 0; i < m_Collision.size(); i++)
    {
        COLLISION collision = m_Collision[i];
        m_szExpression += "(";
        for (Standard_Integer j = 0; j < (collision.FaceList).size(); j++)
        {
            int iCollidFaceNum = collision.FaceList.at(j);
            iCollidFaceNum > 0 ? iCollidFaceNum += iInitSurfNum : iCollidFaceNum -= iInitSurfNum;

            int iNum = -1*(iCollidFaceNum);
            m_szExpression += TCollection_AsciiString(iNum);
            m_szExpression += ":";
        }

        for (Standard_Integer k = 0; k < (collision.AuxFaceList).size(); k++)
        {
            vector<int> auxiliary_face_list = collision.AuxFaceList.at(k);           
            if ( auxiliary_face_list.size() > 1)
            {
                m_szExpression += "(";
            }

            for (Standard_Integer k = 0; k < auxiliary_face_list.size(); k++)
            {
                int iAuxFaceNum = auxiliary_face_list.at(k);
                iAuxFaceNum > 0 ? iAuxFaceNum += iInitSurfNum : iAuxFaceNum -= iInitSurfNum;

                int iNum = -1*(iAuxFaceNum);
                m_szExpression += TCollection_AsciiString(iNum);
                m_szExpression += " ";
            }

            m_szExpression.Remove(m_szExpression.Length()); // Remove the last character
            if (auxiliary_face_list.size() > 1)
            {
                m_szExpression += ")";
            }
            m_szExpression += ":";
        }

        m_szExpression.Remove(m_szExpression.Length());     // Remove the last character ":"
        m_szExpression += ")";
        m_szExpression += "&";
    }    
    return m_szExpression;
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
TCollection_AsciiString McCadVoidCell::GetOutVoidExpression()
{
    m_szExpression.Clear();
    m_szExpression += "(";

    int iInitSurfNum = McCadConvertConfig::GetInitSurfNum()-1;
    for (Standard_Integer i = 0; i < m_FaceList.size(); i++)
    {
        McCadExtFace * pExtFace = m_FaceList[i];

        int iFaceNum = pExtFace->GetFaceNum();
        iFaceNum > 0 ? iFaceNum += iInitSurfNum : iFaceNum -= iInitSurfNum;
        iFaceNum *= -1;

        m_szExpression += TCollection_AsciiString(iFaceNum);
        m_szExpression += ":";
    }
    m_szExpression.Remove(m_szExpression.Length());
    m_szExpression += ")";

    return m_szExpression;
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
vector<McCadExtFace*> McCadVoidCell::GetGeomFaceList()
{
    if(!m_FaceList.empty())
    {
        return m_FaceList;
    }

    TopExp_Explorer exp;

    TopoDS_Solid void_solid = BRepPrimAPI_MakeBox(m_MinPnt,m_MaxPnt).Solid();
    for (exp.Init(void_solid, TopAbs_FACE); exp.More(); exp.Next())
    {
        TopoDS_Face theFace = TopoDS::Face(exp.Current());
        McCadExtFace *pExtFace = new McCadExtFace(theFace);
        pExtFace->SetFaceNum(-1);
        m_FaceList.push_back(pExtFace);
    }
    return m_FaceList;
}


/** ********************************************************************
* @brief
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadVoidCell::SplitVoidCell(vector<McCadVoidCell*> & void_list,
                                              McCadGeomData *pData)
{
    //cout<<"+++"<<"VoidDecomposeDepth:"<<McCadConvertConfig::GetVoidDecomposeDepth()<<endl;
    if (m_iSplitDepth == McCadConvertConfig::GetVoidDecomposeDepth())
    {
        return Standard_False;
    }    
    //cout<<"MaxDiscLength()"<<McCadConvertConfig::GetMaxDiscLength()<<endl;
    if (this->GetExpression().Length() > McCadConvertConfig::GetMaxDiscLength())
    {
        //cout<<"GetExpression():"<<this->GetExpression().Length()<<endl;
        //SplitAxis split_axis;

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

            //split_axis = YAxis;

        }
        else if(this->m_SplitAxis == YAxis)
        {
            Standard_Integer yMid = (m_MaxPnt.Y()+ m_MinPnt.Y())/2;

            max_pntA.SetCoord(m_MaxPnt.X(),yMid,m_MaxPnt.Z());
            min_pntA.SetCoord(m_MinPnt.X(),m_MinPnt.Y(),m_MinPnt.Z());

            max_pntB.SetCoord(m_MaxPnt.X(),m_MaxPnt.Y(),m_MaxPnt.Z());
            min_pntB.SetCoord(m_MinPnt.X(),yMid,m_MinPnt.Z());

            //split_axis = ZAxis;

        }
        else if(this->m_SplitAxis == ZAxis)
        {
            Standard_Integer zMid = (m_MaxPnt.Z()+ m_MinPnt.Z())/2;

            max_pntA.SetCoord(m_MaxPnt.X(),m_MaxPnt.Y(),zMid);
            min_pntA.SetCoord(m_MinPnt.X(),m_MinPnt.Y(),m_MinPnt.Z());

            max_pntB.SetCoord(m_MaxPnt.X(),m_MaxPnt.Y(),m_MaxPnt.Z());
            min_pntB.SetCoord(m_MinPnt.X(),m_MinPnt.Y(),zMid);

            //split_axis = XAxis;
        }

       /* cout<<m_MaxPnt.X()<<","<<m_MaxPnt.Y()<<","<<m_MaxPnt.Z()<<endl;
        cout<<m_MinPnt.X()<<","<<m_MinPnt.Y()<<","<<m_MinPnt.Z()<<endl;
        cout<<endl;

        cout<<min_pntA.X()<<","<<min_pntA.Y()<<","<<min_pntA.Z()<<endl;
        cout<<max_pntA.X()<<","<<max_pntA.Y()<<","<<max_pntA.Z()<<endl;
        cout<<endl;

        cout<<min_pntB.X()<<","<<min_pntB.Y()<<","<<min_pntB.Z()<<endl;
        cout<<max_pntB.X()<<","<<max_pntB.Y()<<","<<max_pntB.Z()<<endl;*/

        McCadVoidCell * pVoidA = new McCadVoidCell(BRepPrimAPI_MakeBox(min_pntA,max_pntA).Solid());
        McCadVoidCell * pVoidB = new McCadVoidCell(BRepPrimAPI_MakeBox(min_pntB,max_pntB).Solid());

        pVoidA->SetCollidedSolidNumList(m_CollidedSolidNumList);
        pVoidB->SetCollidedSolidNumList(m_CollidedSolidNumList);

        pVoidA->CalColliedFaces(pData);
        pVoidB->CalColliedFaces(pData);

        //pVoidA->SetSplitAxis(split_axis);
        //pVoidB->SetSplitAxis(split_axis);
        pVoidA->SetSplitDepth(m_iSplitDepth+1);
        pVoidB->SetSplitDepth(m_iSplitDepth+1);

        if(!pVoidA->SplitVoidCell(void_list,pData))
        {
            void_list.push_back(pVoidA);
            //cout<<"level"<<m_iSplitDepth+1<<"   A"<<endl;
        }

        if(!pVoidB->SplitVoidCell(void_list,pData))
        {
            void_list.push_back(pVoidB);
            //cout<<"level"<<m_iSplitDepth+1<<"   B"<<endl;
        }
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}


void McCadVoidCell::SetSplitDepth(Standard_Integer iSplitDepth)
{
    m_iSplitDepth = iSplitDepth;
}



void McCadVoidCell::SetSplitAxis(SplitAxis split_axis)
{
    m_SplitAxis = split_axis;
}



void McCadVoidCell::SetCollidedSolidNumList(vector<int> theCollidedSolidNumList )
{
    m_CollidedSolidNumList = theCollidedSolidNumList;
}


void McCadVoidCell::ChangeFaceNum(McCadGeomData *pData)
{
    for (Standard_Integer i = 0; i < m_FaceList.size(); i++)
    {
        McCadExtFace * pExtFace = m_FaceList[i];

        Standard_Integer iFaceNumOld = pExtFace->GetFaceNum();
        Standard_Integer iFaceNumNew = pData->GetNewFaceNum(abs(iFaceNumOld));
        pExtFace->SetFaceNum(iFaceNumNew);
    }

    for (Standard_Integer i = 0; i < m_Collision.size(); i++)
    {
        COLLISION collision = m_Collision[i];
        for (Standard_Integer j = 0; j < (collision.FaceList).size(); j++)
        {
            Standard_Integer iFaceNumOld = collision.FaceList.at(j);
            Standard_Integer iFaceNumNew = pData->GetNewFaceNum(abs(iFaceNumOld));
            if(iFaceNumOld < 0)
            {
                iFaceNumNew *= -1;
            }
            m_Collision[i].FaceList.at(j) = iFaceNumNew;
        }

        for (Standard_Integer k = 0; k < (collision.AuxFaceList).size(); k++)
        {
            vector<int> auxiliary_face_list = collision.AuxFaceList.at(k);
            for (Standard_Integer l = 0; l < auxiliary_face_list.size(); l++)
            {
                Standard_Integer iFaceNumOld = auxiliary_face_list.at(l);
                Standard_Integer iFaceNumNew = pData->GetNewFaceNum(abs(iFaceNumOld));
                if(iFaceNumOld < 0)
                {
                    iFaceNumNew *= -1;
                }
                m_Collision[i].AuxFaceList.at(k).at(l) = iFaceNumNew;
            }
        }
    }
}



/** ********************************************************************
* @brief Get the expression of the cubic box represented the void space
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @modify 16/12/2013
* @author  Lei Lu
***********************************************************************/
vector<McCadExtFace*> McCadVoidCell::GetFaces()
{
    return m_FaceList;
}




/** ********************************************************************
* @brief Get the collision information of void box
*
* @param
* @return TCollection_AsciiString
*
* @date 31/8/2012
* @modify 16/12/2013
* @author  Lei Lu
***********************************************************************/
vector<COLLISION> McCadVoidCell::GetCollision()
{
    return m_Collision;
}



//for (int iVoidFace = 0; iVoidFace < m_FaceList.size(); iVoidFace++)
//{
//    if (b==1)
//    {
//        break;
//    }

//    McCadExtFace *pVoidFace = m_FaceList.at(iVoidFace);
//    McCadExtFace *pMatFace = NULL;
//    for (int iMatFace = 0; iMatFace < pSolid->GetSTLFaceList().size();iMatFace++)
//    {
//        pMatFace = pSolid->GetSTLFaceList().at(iMatFace);

//        TopLoc_Location loc;
//        Handle(Geom_Surface) geom_surface = BRep_Tool::Surface(*pMatFace, loc);
//        GeomAdaptor_Surface surf_adoptor(geom_surface);

//        Handle(Geom_Surface) geom_surface2 = BRep_Tool::Surface(*pVoidFace, loc);
//        GeomAdaptor_Surface surf_adoptor2(geom_surface2);

//        if (surf_adoptor.GetType() == GeomAbs_Plane
//                && surf_adoptor2.GetType() == GeomAbs_Plane)
//        {
//            gp_Pln plane1 = surf_adoptor.Plane();      // Get the geometry plane

//             Standard_Real PrmtA1;
//             Standard_Real PrmtB1;
//             Standard_Real PrmtC1;
//             Standard_Real PrmtD1;



//            plane1.Coefficients(PrmtA1,PrmtB1,PrmtC1,PrmtD1);
//            // If the parameter is less than 1.0e-07, it can be take as zero
//            if(McCadMathTool::IsEqualZero(PrmtA1))
//               PrmtA1 = 0.0;
//            if(McCadMathTool::IsEqualZero(PrmtB1))
//               PrmtB1 = 0.0;
//            if(McCadMathTool::IsEqualZero(PrmtC1))
//               PrmtC1 = 0.0;
//            if(McCadMathTool::IsEqualZero(PrmtD1))
//               PrmtD1 = 0.0;


//            gp_Dir Dir1;
//            Dir1.SetCoord(PrmtA1,PrmtB1,PrmtC1);
//           // cout<<Dir1.X()<<"     "<<Dir1.Y()<<"      "<<Dir1.Z()<<endl;

//            gp_Pln plane2 = surf_adoptor2.Plane();      // Get the geometry plane

//             Standard_Real PrmtA2;
//             Standard_Real PrmtB2;
//             Standard_Real PrmtC2;
//             Standard_Real PrmtD2;

//            plane2.Coefficients(PrmtA2,PrmtB2,PrmtC2,PrmtD2);



//            if(McCadMathTool::IsEqualZero(PrmtA2))
//               PrmtA2 = 0.0;
//            if(McCadMathTool::IsEqualZero(PrmtB2))
//               PrmtB2 = 0.0;
//            if(McCadMathTool::IsEqualZero(PrmtC2))
//               PrmtC2 = 0.0;
//            if(McCadMathTool::IsEqualZero(PrmtD2))
//               PrmtD2 = 0.0;

//            gp_Dir Dir2;
//            Dir2.SetCoord(PrmtA2,PrmtB2,PrmtC2);

//            cout<<Dir2.X()<<"     "<<Dir2.Y()<<"      "<<Dir2.Z()<<"      "<<PrmtD2<<endl;

//            if(pVoidFace->GetFaceOrientation() == MINUS)
//            {
//                cout<<"----"<<endl;
//            }
//            else
//            {
//                cout<<"+++++"<<endl;
//            }

//            if(Dir1.IsEqual(Dir2,1.0e-3)
//                    && (fabs(PrmtD1 - PrmtD2) < 1.0e-5)
//                    && (pVoidFace->GetFaceOrientation() != pMatFace->GetFaceOrientation())
//                    )
//            {
//                cout<<PrmtA1<<" "<<PrmtB1<<" "<<PrmtC1<<endl;
//                cout<<PrmtA2<<" "<<PrmtB2<<" "<<PrmtC2<<endl;
//                cout<<PrmtD1<<" "<<PrmtD2<<endl<<endl;
//                cout<<"return_______________________________"<<endl;
//                b = 1;
//                break;
//            }
//        }

//        if (b==1)
//        {
//            break;
//        }
//    }
//}


