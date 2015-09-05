#include "McCadExtBndFace.hxx"
#include "../McCadTool/McCadGeomTool.hxx"

#include <Geom_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <McCadCSGTool.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>
#include <BRepBndLib.hxx>
#include <omp.h>

#include <BRepAdaptor_Curve2d.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Precision.hxx>

//#include "../McCadTool/McCadConvertConfig.hxx"

McCadExtBndFace::McCadExtBndFace()
{
}

/** ********************************************************************
* @brief Construct function
*
* @param const TopoDS_Face &theFace
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
McCadExtBndFace::McCadExtBndFace(const TopoDS_Face &theFace):McCadExtFace(theFace)
{
    m_bHaveAuxFace = Standard_False;
    m_DiscPntList = new TColgp_HSequenceOfPnt;
    m_EdgePntList = new TColgp_HSequenceOfPnt;
}



/** ********************************************************************
* @brief Copy construct function
*
* @param const McCadExtFace &theExtFace
* @return
*
* @date 14/3/2013
* @author  Lei Lu
************************************************************************/
McCadExtBndFace::McCadExtBndFace(const McCadExtBndFace &theExtFace)
{
    *this = theExtFace;
}


/** ********************************************************************
* @brief Remove the auxiliary face
*
* @param Standard_Integer i
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
McCadExtBndFace::~McCadExtBndFace()
{
   m_DiscPntList->Clear();
   m_EdgePntList->Clear();
   for (unsigned int i = 0; i < m_AuxFaceList.size(); i++)
   {
       McCadExtFace *pFace = m_AuxFaceList.at(i);
       delete pFace;
       pFace = NULL;
   }
}



/** ********************************************************************
* @brief Remove the auxiliary face
*
* @param Standard_Integer i
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadExtBndFace::RemoveAuxFace(Standard_Integer index)
{
   m_AuxFaceList.clear();
}


/** ********************************************************************
* @brief Judge the face has auxiliary face or not.
*
* @param
* @return
*
* @date
* @author
************************************************************************/
Standard_Boolean McCadExtBndFace::HaveAuxSurf()
{
    return m_bHaveAuxFace;
}


/** ********************************************************************
* @brief Judge the face is concave curved face or not
*
* @param
* @return Standard_Boolean
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Boolean McCadExtBndFace::IsConcaveCurvedFace()
{
    /* Judge the face is plane or not, plane needn't to add auxiliary face */
    TopLoc_Location loc;
    Handle_Geom_Surface theGeomSurface = BRep_Tool::Surface(*this,loc);
    GeomAdaptor_Surface theAdaptedSurface(theGeomSurface);
    if(theAdaptedSurface.GetType() == GeomAbs_Plane)
    {
        return Standard_False;
    }

    /* If the curved surface is forward, no need to add auxiliary face */
    if(this->Orientation() == TopAbs_FORWARD)
    {
        return Standard_False;
    }
    else if(this->Orientation() == TopAbs_REVERSED)
    {
        /* If the max and min UV parameter of the face is same, the face is unaviliable. */
        Standard_Real UMin,UMax, VMin, VMax;
        BRepTools::UVBounds(*this, UMin,UMax, VMin, VMax);

        /* gp::Resolution is tolerance to judge the points is same or not,
           So judge the curved face is close or not. */
        if ( fabs(UMax - UMin)<= gp::Resolution() || fabs(VMax - VMin)<= gp::Resolution())
        {
            return Standard_False;
        }
        return Standard_True;
    }
    return Standard_False;
}


/** ********************************************************************
* @brief Get the discreted sample points of face
*
* @param
* @return Handle_TColgp_HSequenceOfPnt
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Handle_TColgp_HSequenceOfPnt McCadExtBndFace::GetDiscPntList()
{
    if (m_DiscPntList->Length() == 0)
    {
        m_DiscPntList = new TColgp_HSequenceOfPnt;
        m_DiscPntList = McCadGeomTool::GetFaceSamplePnt(*this);
        cout<<"Sample points of face ----------------------------- "<<m_DiscPntList->Length()<<endl;
        return m_DiscPntList;
    }
    else
    {
        return m_DiscPntList;
    }
}


/** ********************************************************************
* @brief Get the discreted sample points of the edges
*
* @param
* @return Handle_TColgp_HSequenceOfPnt
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Handle_TColgp_HSequenceOfPnt McCadExtBndFace::GetEdgePntList()
{
    if (m_EdgePntList->Length() == 0)
    {
        m_EdgePntList = new TColgp_HSequenceOfPnt;
        m_EdgePntList = McCadGeomTool::GetEdgeSamplePnt(*this);      
        cout<<"Sample points of edge ----------------------------- "<<m_EdgePntList->Length()<<endl;
        return m_EdgePntList;
    }
    else
    {
        return m_EdgePntList;
    }
}


/** ********************************************************************
* @brief Get auxiliary faces
*
* @param
* @return vector<McCadExtFace *>
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
vector<McCadExtAuxFace *> McCadExtBndFace::GetAuxFaces()
{    
    if (m_bHaveAuxFace == Standard_False)
    {
        Handle_TopTools_HSequenceOfShape tmpFaceList = new TopTools_HSequenceOfShape();
        tmpFaceList = McCadCSGTool::Partials(*this);      // Calculate the auxiliary faces
        if(tmpFaceList->Length() == 0)
        {
            return m_AuxFaceList;
        }
        for (int i = 1; i <= tmpFaceList->Length(); i++)
        {
           TopoDS_Face tmpFace = TopoDS::Face(tmpFaceList->Value(i));
           McCadExtFace * pExtFace = new McCadExtFace(tmpFace);
//qiu            m_AuxFaceList.push_back(pExtFace);
		         m_AuxFaceList.push_back(static_cast <McCadExtAuxFace *> (pExtFace));
        }
        tmpFaceList->Clear();
        m_bHaveAuxFace = Standard_True;                 // The face has auxiliary faces
        return m_AuxFaceList;
    }
    else
    {
        return m_AuxFaceList;
    }
}


/** ********************************************************************
* @brief Add the auxiliary faces
*
* @param
* @return vector<McCadExtFace *>
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadExtBndFace::AddAuxFaces(vector< McCadExtAuxFace* > faces)
{
    //GetAuxFaces();
    for(Standard_Integer i = 0; i < faces.size(); i++)
    {
        for (Standard_Integer j = 0; j < m_AuxFaceList.size(); j++)
        {
            Standard_Integer iAuxFaceNum = m_AuxFaceList.at(j)->GetFaceNum();
            // if the two auxilary faces are same surfaces with different oritation,
            // remove them, because (-1 : 1) means whole space.
            if(faces.at(i)->GetFaceNum() == -iAuxFaceNum)
            {
                McCadExtAuxFace *pAuxFace = m_AuxFaceList.at(j);
                m_AuxFaceList.erase(m_AuxFaceList.begin()+j);
                delete pAuxFace;
                pAuxFace = NULL;
                j--;
                continue;
            }
            // if the auxilary faces are same, do not add
            else if (faces.at(i)->GetFaceNum() == iAuxFaceNum)
            {
                continue;
            }
            // if the auxilary faces are different faces, create a new one
            // and add to the auxilary face lis of one face.
            else
            {
                McCadExtAuxFace *pExtFace = new McCadExtAuxFace(*faces.at(i));
                m_AuxFaceList.push_back(pExtFace);
            }
        }

    }
}


/** ********************************************************************
* @brief Get the same face list
*
* @param
* @return vector<McCadExtFace *>
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
vector< McCadExtBndFace* > McCadExtBndFace::GetSameFaces()
{
    return m_SameFaceList;
}


/** ********************************************************************
* @brief If a solid has two same surfaces, add one of them into the
         samefacelist of another's. and update the mesh points list
         and boundary box for collision detection.
*
* @param int iNum
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadExtBndFace::AddSameFaces( McCadExtBndFace* faces)
{
    m_EdgePntList->Append(faces->GetEdgePntList()); // Merge the discrete edge points of the two same faces
    m_DiscPntList->Append(faces->GetDiscPntList()); // Merge the discrete face points of the two same faces

    GetBndBox();                                    // Merge the boundary box
    m_bBox.Add(faces->GetBndBox());

    AddAuxFaces(faces->GetAuxFaces());              // Merge the auxiliary face
    McCadExtBndFace *pExtFace = new McCadExtBndFace(*faces); // Put one face into sameface list of another face
    m_SameFaceList.push_back(pExtFace);
}


void McCadExtBndFace::AddPntList(Handle_TColgp_HSequenceOfPnt pnt_list)
{
    m_DiscPntList->Append(pnt_list);
}
