#include "McCadExtFace.hxx"
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

#include "../McCadTool/McCadConvertConfig.hxx"
#include "../McCadTool/McCadGeomTool.hxx"

McCadExtFace::McCadExtFace()
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
McCadExtFace::McCadExtFace(const TopoDS_Face &theFace):TopoDS_Face(theFace)
{
    m_bHaveAuxFace = Standard_False;
    m_DiscPntList = new TColgp_HSequenceOfPnt;
    m_EdgePntList = new TColgp_HSequenceOfPnt;
    m_iAttri = 0;
    m_bFusedFace = Standard_False;
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
McCadExtFace::McCadExtFace(const McCadExtFace &theExtFace):TopoDS_Face(theExtFace)
{
    //m_bHaveAuxFace = Standard_False;
    //m_DiscPntList = new TColgp_HSequenceOfPnt;
    m_iFaceNum = theExtFace.m_iFaceNum;
    //*this = theExtFace;
    m_iAttri = theExtFace.GetAttri();

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
McCadExtFace::~McCadExtFace()
{
   m_DiscPntList->Clear();
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
* @modify 04/09/2014
* @author  Lei Lu
************************************************************************/
void McCadExtFace::RemoveAuxFace(Standard_Integer index)
{
   McCadExtFace * pFace = m_AuxFaceList.at(index);
   m_AuxFaceList.erase(m_AuxFaceList.begin()+index);
   delete pFace;
   pFace = NULL;
}



/** ********************************************************************
* @brief Add attribute to the auxiliary face
*
* @param
* @return
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadExtFace::AddAttri(Standard_Integer iAttri)
{
   m_iAttri = iAttri;
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
Standard_Boolean McCadExtFace::IsConcaveCurvedFace()
{
    /* Judge the face is plane or not, plane needn't to add auxiliary face */
    TopLoc_Location loc;
    Handle_Geom_Surface theGeomSurface = BRep_Tool::Surface(*this,loc);
    GeomAdaptor_Surface theAdaptedSurface(theGeomSurface);

    SetFaceType(theAdaptedSurface.GetType());

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
Handle_TColgp_HSequenceOfPnt McCadExtFace::GetDiscPntList()
{
    if (m_DiscPntList->Length() == 0)
    {
        m_DiscPntList = new TColgp_HSequenceOfPnt;        
        m_DiscPntList = McCadGeomTool::GetFaceSamplePnt(*this);
        cout<<">>> Sample points of face ....................... "<<m_DiscPntList->Length()<<endl;
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
Handle_TColgp_HSequenceOfPnt McCadExtFace::GetEdgePntList()
{
    if (m_EdgePntList->Length() == 0)
    {
        m_EdgePntList = new TColgp_HSequenceOfPnt;
        m_EdgePntList = McCadGeomTool::GetEdgeSamplePnt(*this);
        cout<<">>> Sample points of edge ....................... "<<m_EdgePntList->Length()<<endl;
        //gp_Pnt pos;
       // for (int i = 1 ; i <= m_EdgePntList->Length(); i++)
       // {
       //     pos = m_EdgePntList->Value(i);
       //     cout<<"y"<<pos.Y()<<endl;
      //  }
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
vector<McCadExtFace *> McCadExtFace::GetAuxFaces()
{
    if (m_bHaveAuxFace == Standard_False)
    {
        Handle_TopTools_HSequenceOfShape AuxFaceList = new TopTools_HSequenceOfShape();
        if ( (!McCadCSGTool::AddAuxSurf(*this,AuxFaceList))
                || (AuxFaceList->Length() == 0))      // Calculate the auxiliary faces
        {
            return m_AuxFaceList;
        }

        for (int i = 1; i <= AuxFaceList->Length(); i++)
        {
           TopoDS_Face tmpFace = TopoDS::Face(AuxFaceList->Value(i));
           McCadExtFace * pExtFace = new McCadExtFace(tmpFace);
           m_AuxFaceList.push_back(pExtFace);
        }

        AuxFaceList->Clear();
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
void McCadExtFace::AddAuxFaces(vector< McCadExtFace* > faces)
{
    // Whether the two faces have same auxilary surfaces
    Standard_Boolean bRepeat = Standard_False;
    for(Standard_Integer i = 0; i < faces.size(); i++)
    {
        for (Standard_Integer j = 0; j < m_AuxFaceList.size(); j++)
        {
            Standard_Integer iAuxFaceNum = m_AuxFaceList.at(j)->GetFaceNum();
            // if the two auxilary faces are same surfaces with different oritation,
            // remove them, because (-1 : 1) means whole space.
            if(faces.at(i)->GetFaceNum() == -iAuxFaceNum)
            {
                McCadExtFace *pAuxFace = m_AuxFaceList.at(j);
                m_AuxFaceList.erase(m_AuxFaceList.begin()+j);
                delete pAuxFace;
                pAuxFace = NULL;
                j--;
                bRepeat = Standard_True;
                break;
            }
            // if the auxilary faces are same, do not add
            else if (faces.at(i)->GetFaceNum() == iAuxFaceNum)
            {
                bRepeat = Standard_True;
                break;
            }            

            // if the auxilary faces are different faces, create a new one
            // and add to the auxilary face lis of one face.
            if (!bRepeat)
            {
                McCadExtFace *pExtFace = new McCadExtFace(*faces.at(i));
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
vector< McCadExtFace* > McCadExtFace::GetSameFaces()
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
void McCadExtFace::AddSameFaces( McCadExtFace* faces)
{    
    m_EdgePntList->Append(faces->GetEdgePntList()); // Merge the discrete edge points of the two same faces    
    if (McCadConvertConfig::GenerateVoid())
    {
       m_DiscPntList->Append(faces->GetDiscPntList()); // Merge the discrete face points of the two same faces
    }

    GetBndBox();                                    // Merge the boundary box
    m_bBox.Add(faces->GetBndBox());

    if(faces->HaveAuxSurf())
    {
         AddAuxFaces(faces->GetAuxFaces());              // Merge the auxiliary face
    }

    McCadExtFace *pExtFace = new McCadExtFace(*faces); // Put one face into sameface list of another face
    m_SameFaceList.push_back(pExtFace);
}



/** ********************************************************************
* @brief
*
*
*
* @param int iNum
* @return void
*
* @date 7/2/2014
* @author  Lei Lu
************************************************************************/
void McCadExtFace::MergeDscPnt(TopoDS_Face &theFace)
{
    McCadExtFace *pExtFace = new McCadExtFace(theFace); // Create a McCadExtFace based on original face

   // m_EdgePntList->Append(pExtFace->GetEdgePntList()); // Merge the discrete edge points of the two same faces

    //if (McCadConvertConfig::GenerateVoid())
   // {
   //    m_DiscPntList->Append(pExtFace->GetDiscPntList()); // Merge the discrete face points of the two same faces
  //  }

    //m_bBox.Add(pExtFace->GetBndBox()); // Merge the boundary box
    AddSameFaces(pExtFace);
    m_bFusedFace = Standard_True;

    //delete pExtFace;
    //pExtFace = NULL;
}




Standard_Boolean McCadExtFace::HaveAuxSurf()
{
    return m_bHaveAuxFace;
}




/** ********************************************************************
* @brief Set the number of face according the storing position of face list
*
* @param int iNum
* @return void
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
void McCadExtFace::SetFaceNum(int iNum)
{
    if(this->GetFaceOrientation() == MINUS )
    {
       m_iFaceNum = -1 * iNum ;//* m_Reverse;
    }
    else
    {
       m_iFaceNum = iNum ;//* m_Reverse;
    }
}



/** ********************************************************************
* @brief Get face number which can be used to find the geometry information
*
* @param
* @return Standard_Integer
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
Standard_Integer McCadExtFace::GetFaceNum()
{
    return m_iFaceNum;
}



/** ********************************************************************
* @brief Get the orientation of face, is forward or reversed
*
* @param
* @return ENUM ORIENTATION (PLUS or MINUS)
*
* @date 31/8/2012
* @author  Lei Lu
************************************************************************/
ORIENTATION McCadExtFace::GetFaceOrientation()
{
    TopAbs_Orientation orient = this->Orientation();
    if (orient == TopAbs_FORWARD)
    {        
        return MINUS;                   // Add minus symbol
    }
    else if(orient == TopAbs_REVERSED)
    {        
        return PLUS;                    // Add plus symbol
    }
}


Bnd_Box McCadExtFace::GetBndBox()
{
    if(!m_bBox.IsVoid())
    {
        return m_bBox;
    }

    Bnd_Box tmpBBox;
    BRepBndLib::Add(*this, tmpBBox);   

    tmpBBox.SetGap(0.0);
    Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
    tmpBBox.Get(xMin, yMin, zMin, xMax, yMax, zMax);
    gp_Pnt lower(xMin, yMin, zMin), upper(xMax, yMax, zMax);

    McCadGeomTool::SimplifyPoint(lower);
    McCadGeomTool::SimplifyPoint(upper);

    m_bBox.Add(lower);
    m_bBox.Add(upper);

    return m_bBox;
}


void McCadExtFace::UpdateBndBox(Bnd_Box theBBox)
{
    if(m_bBox.IsVoid())
    {
        GetBndBox();
    }
    m_bBox.Add(theBBox);
}


GeomAbs_SurfaceType McCadExtFace::GetFaceType()
{
    return m_FaceType;
}


void McCadExtFace::SetFaceType(GeomAbs_SurfaceType theFaceType)
{
    m_FaceType = theFaceType;
}


void McCadExtFace::AddPntList(Handle_TColgp_HSequenceOfPnt pnt_list)
{
    m_DiscPntList->Append(pnt_list);
}



/*Handle_TColgp_HSequenceOfPnt McCadExtFace::Init()
{
    BRepClass_FaceClassifier bsc3d;
    Handle(TColgp_HSequenceOfPnt) thePntSeq = new TColgp_HSequenceOfPnt();

    BRepAdaptor_Curve2d c;
    TopoDS_Face theFace = *this;

    BRepAdaptor_Surface BS(theFace, Standard_True);
    GeomAdaptor_Surface theASurface = BS.Surface();

    /*Standard_Real uMin, uMax, vMin, vMax, uMean, vMean;
    uMin = theASurface.FirstUParameter();
    uMax = theASurface.LastUParameter();
    vMin = theASurface.FirstVParameter();
    vMax = theASurface.LastVParameter();
    uMean = (uMin + uMax)/2.0;
    vMean = (vMin + vMax)/2.0;
//

//	Standard_Integer nbBrimPoints(0);
    myMaxNbPnt = 50;
    myMinNbPnt = 10;
    myXlen = 50 ;
    myYlen = 50;
    myRlen = 0.0314;
    myTolerance = 1e-07;

    for (TopExp_Explorer ex(theFace, TopAbs_EDGE); ex.More(); ex.Next())
    {
        c.Initialize(TopoDS::Edge(ex.Current()), theFace);
        Standard_Real f = (!(Precision::IsNegativeInfinite(c.FirstParameter()))) ? c.FirstParameter() : -1.0;
        Standard_Real l = (!(Precision::IsPositiveInfinite(c.LastParameter()))) ? c.LastParameter()	: 1.0;

        gp_Pnt2d pf = c.Value(f);
        gp_Pnt2d pl = c.Value(l);
        gp_Pnt p3f;
        theASurface.D0(pf.X(), pf.Y(), p3f);
        gp_Pnt p3l;
        theASurface.D0(pl.X(), pl.Y(), p3l);
        Standard_Real Len = p3f.Distance(p3l);
        Standard_Integer NUMPNT;
        if (int(Len/myXlen) < myMinNbPnt)
            NUMPNT = myMinNbPnt;
        else if (int(Len/myXlen) > myMaxNbPnt)
            NUMPNT = myMaxNbPnt;
        else
            NUMPNT = int(Len/myXlen);

        for (int t=0; t<=NUMPNT; t++)
        {
            Standard_Real a = Standard_Real(t)/Standard_Real(NUMPNT);
            Standard_Real par = (1-a)*f + a*l;
            gp_Pnt2d p2 = c.Value(par);
            Standard_Real U = p2.X();
            Standard_Real V = p2.Y();

            gp_Pnt p1;
            theASurface.D0(U, V, p1);

            bsc3d.Perform(theFace, p2, myTolerance);

            if (bsc3d.State() == TopAbs_IN || bsc3d.State() == TopAbs_ON)
                thePntSeq->Append(p1);
        }



    }




    cout << "Length------------------------" <<  thePntSeq->Length()  <<endl;

//	nbBrimPoints = thePntSeq->Length();

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    Standard_Real U1, U2, V1, V2;
    BRepTools::UVBounds(theFace, U1, U2, V1, V2);

    if (theASurface.GetType() == GeomAbs_Cone ||theASurface.GetType() == GeomAbs_Cylinder)
    {
        myXlen = myRlen;
    }
    if (theASurface.GetType() == GeomAbs_Sphere ||theASurface.GetType()	== GeomAbs_Torus)
    {
        myXlen = myRlen;
        myYlen = myRlen;
    }

    // cout << "U1 =  " << U1  << " U2 =  " << U2  <<  " V1 =  " << V1  <<  " V2 =  " << V2  << endl;
    Standard_Real du=1, dv=1;

    Standard_Real XLen=U2-U1;
    Standard_Real YLen=V2-V1;
    Standard_Integer XNUMPNT;
    if (int(XLen/myXlen) < myMinNbPnt)
        XNUMPNT = myMinNbPnt;
    else if (int(XLen/myXlen) > myMaxNbPnt)
        XNUMPNT = myMaxNbPnt;
    else
        XNUMPNT = int(XLen/myXlen);
    // cout << "XNUMPNT =  " << XNUMPNT  << endl;
    Standard_Integer YNUMPNT;
    if (int(YLen/myYlen) < myMinNbPnt)
        YNUMPNT = myMinNbPnt;
    else if (int(YLen/myYlen) > myMaxNbPnt)
        YNUMPNT = myMaxNbPnt;
    else
        YNUMPNT = int(YLen/myYlen);
    //  cout << "YNUMPNT =  " << YNUMPNT  << endl;

    Handle(TColgp_HSequenceOfPnt) tmpPntSeq = new TColgp_HSequenceOfPnt();

    if (theASurface.GetType() == GeomAbs_Sphere ||theASurface.GetType()	== GeomAbs_Torus)
    {
        cout << "XLen =  " << XLen  << endl;
        cout << "YLen =  " << YLen  << endl;
        cout << "XNUMPNT =  " << XNUMPNT  << endl;
        cout << "YNUMPNT =  " << YNUMPNT  << endl;
        cout << "myMinNbPnt =  " << myMinNbPnt  << endl;

    }

    Standard_Integer repeat = 0;
    do
    {
        if (repeat > 3)
            break;

        du = XLen/Standard_Real(XNUMPNT);
        dv = YLen/Standard_Real(YNUMPNT);

        tmpPntSeq->Clear();

        for (int j=0; j<=YNUMPNT; j++)
        {
            Standard_Real V = V1+dv*j;

            for (int i=0; i<=XNUMPNT; i++)
            {
                gp_Pnt p1;
                Standard_Real U = U1+du*i;
                theASurface.D0(U, V, p1);
                gp_Pnt2d p2(U, V);

                bsc3d.Perform(theFace, p2, myTolerance);
                if (bsc3d.State() == TopAbs_IN /*|| bsc3d.State() == TopAbs_ON) //MYTEST comment
                    tmpPntSeq->Append(p1);
            }
        }

        XNUMPNT=XNUMPNT*2;
        YNUMPNT=YNUMPNT*2;
        repeat++;

    } while (tmpPntSeq->Length() < myMinNbPnt*myMinNbPnt);

    if (theASurface.GetType() == GeomAbs_Sphere ||theASurface.GetType()	== GeomAbs_Torus)
    {
        cout << "XNUMPNT =  " << XNUMPNT  << endl;
        cout << "YNUMPNT =  " << YNUMPNT  << endl;
        cout << "myMinNbPnt =  " << myMinNbPnt  << endl;

    }



    thePntSeq->Append(tmpPntSeq);



    // we add vetices here
    for (TopExp_Explorer exv(theFace, TopAbs_VERTEX); exv.More(); exv.Next())
    {
        gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(exv.Current()));
        thePntSeq->Prepend(aPnt);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // remove sample points which have been added multiple times
   for (int j=2; j<= thePntSeq->Length(); j++)
    {
        gp_XYZ p1 = (thePntSeq->Value(j)).XYZ();
        for (int jk=j-1; jk >= 1; jk--)
        {
            gp_XYZ p2 = (thePntSeq->Value(jk)).XYZ();

            //cout << p1.X()*p1.X() + p1.Y()*p1.Y() + p1.Z()*p1.Z() + p2.X()*p2.X() + p2.Y()*p2.Y() + p2.Z()*p2.Z() << endl;

            if (p1.IsEqual(p2, 1.e-05))
            {
                thePntSeq->Remove(jk);
                j=jk;
            }
        }
    }


    // cout << "Total Number of face sample points computed = " << thePntSeq->Length() << endl;
    Bnd_Box aBB; // make a tight box!!!!


    BRepBndLib::Add(theFace, aBB);
    aBB.SetGap(0);


    if (aBB.IsWhole() || aBB.IsVoid() || aBB.IsThin(1e-7))
        cout << "_#_McCadDiscretization_Face.cxx :: Face Bounding box computation failed !!" << endl;

    return thePntSeq;

}*/
