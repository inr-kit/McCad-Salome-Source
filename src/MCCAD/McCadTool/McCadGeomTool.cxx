#include "McCadGeomTool.hxx"

#include <BRepClass_FaceClassifier.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <ElSLib.hxx>
#include <CSLib_DerivativeStatus.hxx>
#include <CSLib.hxx>

#include <Precision.hxx>

#include <TopoDS.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <GProp_GProps.hxx>

#include "McCadConvertConfig.hxx"
#include "McCadMathTool.hxx"

#include "../McCadMcVoid/IGeomFace.hxx"
#include "../McCadMcVoid/McCadGeomPlane.hxx"
#include "../McCadMcVoid/McCadGeomCylinder.hxx"
#include "../McCadMcVoid/McCadGeomCone.hxx"
#include "../McCadMcVoid/McCadGeomSphere.hxx"
#include "../McCadMcVoid/McCadGeomTorus.hxx"
#include "../McCadMcVoid/McCadGeomRevolution.hxx"

Standard_Real McCadGeomTool::m_MaxNbPnt;
Standard_Real McCadGeomTool::m_MinNbPnt;
Standard_Real McCadGeomTool::m_Xlen;
Standard_Real McCadGeomTool::m_Ylen;
Standard_Real McCadGeomTool::m_Rlen;
Standard_Real McCadGeomTool::m_Tolerance;

McCadGeomTool::McCadGeomTool()
{

}


/** ***************************************************************************
* @brief  Set the parameters for getting the sample points, which are stored in
*         the config file.
* @param  Object: McCadConvertConfig file storing the input parameters
* @return void
*
* @date 23/06/2013
* @modify 22/04/2016
* @author  Lei Lu
******************************************************************************/
void McCadGeomTool::SetPrmt()
{
    m_MaxNbPnt = McCadConvertConfig::GetMaxSmplPntNum();
    m_MinNbPnt = McCadConvertConfig::GetMinSmplPntNum();
    m_Xlen = McCadConvertConfig::GetXResolution();
    m_Ylen = McCadConvertConfig::GetYResolution();
    m_Rlen = McCadConvertConfig::GetRResolution();
    m_Tolerance = McCadConvertConfig::GetTolerence();
}



/** ***************************************************************************
* @brief  Get the sample points on the input face, including the edges of face
* @param  TopoDS_Face theFace     The face which edge pretains
* @return Handle_TColgp_HSequenceOfPnt  The generated sample point list
*
* @date 23/06/2013
* @modify 22/04/2016
* @author  Lei Lu
******************************************************************************/
Handle_TColgp_HSequenceOfPnt McCadGeomTool::GetFaceSamplePnt(const TopoDS_Face& theFace)
{
    SetPrmt();                                          // Set the basic prameters for the mesh
    Handle(TColgp_HSequenceOfPnt) thePntList = new TColgp_HSequenceOfPnt();

    BRepClass_FaceClassifier bsc3d;                     // Classifier of face

    BRepAdaptor_Surface bSurf(theFace, Standard_True);  // Create the face adaptor
    GeomAdaptor_Surface gSurf = bSurf.Surface();        // Get the geometry surface

    for (TopExp_Explorer exv(theFace, TopAbs_VERTEX); exv.More(); exv.Next())
    {
        gp_Pnt pnt_vtx = BRep_Tool::Pnt(TopoDS::Vertex(exv.Current()));
        thePntList->Prepend(pnt_vtx);
    }

    /* Traverse the edges of face, add them into sample points list */
    for (TopExp_Explorer ex(theFace, TopAbs_EDGE); ex.More(); ex.Next())
    {
        DiscreteEdge(TopoDS::Edge(ex.Current()),theFace,gSurf,thePntList);
    }

    RemoveRepeatPnt(thePntList);

    /* Get the points on face, add them into sample points list */
    Standard_Real U1, U2, V1, V2;
    BRepTools::UVBounds(theFace, U1, U2, V1, V2);

    Standard_Real du = 1, dv = 1;
    Standard_Real xLen = U2 - U1;
    Standard_Real yLen = V2 - V1;
    Standard_Integer xNumPnt;
    Standard_Integer yNumPnt;

    if (gSurf.GetType() == GeomAbs_Cone ||gSurf.GetType() == GeomAbs_Cylinder)
    {
        m_Xlen = m_Rlen;
    }
    if (gSurf.GetType() == GeomAbs_Sphere ||gSurf.GetType()	== GeomAbs_Torus)
    {
        m_Xlen = m_Rlen;
        m_Ylen = m_Rlen;
    }

    xNumPnt = AdjustSampleNum(xLen,m_Xlen);
    yNumPnt = AdjustSampleNum(yLen,m_Ylen);

    ScaleSampleNum(3, xNumPnt, yNumPnt);

    du = xLen/Standard_Real(xNumPnt);
    dv = yLen/Standard_Real(yNumPnt);

//    if (gSurf.GetType() == GeomAbs_Sphere ||gSurf.GetType()	== GeomAbs_Torus)
//    {
//        /*cout << "XLen =  " << xLen  << endl;
//        cout << "YLen =  " << yLen  << endl;
//        cout << "XNUMPNT =  " << xNumPnt  << endl;
//        cout << "YNUMPNT =  " << yNumPnt  << endl;
//        cout << "myMinNbPnt =  " << m_MinNbPnt  << endl;*/
//    }

    for ( int j = 0; j <= yNumPnt; j++ )
    {
        Standard_Real V = V1 + dv*j;
        for ( int i = 0; i <= xNumPnt; i++ )
        {
            gp_Pnt SamplePnt;
            Standard_Real U = U1 + du*i;
            gSurf.D0(U, V, SamplePnt);
            gp_Pnt2d pnt2d(U, V);

            bsc3d.Perform(theFace, pnt2d, m_Tolerance);
            if (bsc3d.State() == TopAbs_IN /*|| bsc3d.State() == TopAbs_ON*/) //MYTEST comment
            {
                thePntList->Append(SamplePnt);
            }
        }
    }

    return thePntList;
}



/** ***************************************************************************
* @brief  Discrete the edge
* @param  TopoDS_Edge theEdge     The input edge
          TopoDS_Face theFace     The face which edge pretains
          GeomAdaptor_Surface theSurf     The adaptor surface of input face
          Handle_TColgp_HSequenceOfPnt  thePntSeq  The generated sample point list
* @return Handle_TColgp_HSequenceOfPnt  thePntSeq  The generated sample point list
*
* @date 23/06/2013
* @modify 22/04/2016
* @author  Lei Lu
******************************************************************************/
void McCadGeomTool::DiscreteEdge(const TopoDS_Edge& theEdge,
                                 const TopoDS_Face& theFace,
                                 const GeomAdaptor_Surface & theSurf,
                                 Handle_TColgp_HSequenceOfPnt & thePntSeq)
{
    BRepAdaptor_Curve2d curve;                      // 2D curve to define the edge
    curve.Initialize(theEdge, theFace);             // Get the curve

    Standard_Real prmtFst =  -1.0;                  // The first point parameter of curve
    Standard_Real prmtLst =  1.0;                   // The last point parameter of curve

    if ( !(Precision::IsNegativeInfinite(curve.FirstParameter())) )
    {
        prmtFst = curve.FirstParameter();
    }

    if ( !(Precision::IsPositiveInfinite(curve.LastParameter())) )
    {
        prmtLst = curve.LastParameter();
    }

    gp_Pnt2d pnt2dFst = curve.Value(prmtFst);
    gp_Pnt2d pnt2dLst = curve.Value(prmtLst);
    gp_Pnt pnt3dFst = theSurf.Value(pnt2dFst.X(), pnt2dFst.Y());
    gp_Pnt pnt3dLst = theSurf.Value(pnt2dLst.X(), pnt2dLst.Y());

    Standard_Real EgdeLen = pnt3dFst.Distance(pnt3dLst);
    Standard_Integer NumPnt;  

    NumPnt = AdjustEdgeSampleNum(EgdeLen,m_Ylen);

    BRepClass_FaceClassifier bsc3d;
    for (int i = 0; i <= NumPnt; i++ )
    {
        Standard_Real dPrmt = Standard_Real(i)/Standard_Real(NumPnt);
        Standard_Real vPrmt = prmtFst + dPrmt*(prmtLst - prmtFst);

        gp_Pnt2d pnt2d = curve.Value(vPrmt);
        gp_Pnt pnt3d = theSurf.Value(pnt2d.X(), pnt2d.Y());

        bsc3d.Perform(theFace, pnt2d, m_Tolerance);

        if (bsc3d.State() == TopAbs_IN || bsc3d.State() == TopAbs_ON)
        {
            SimplifyPoint(pnt3d);
            thePntSeq->Append(pnt3d);            
        }
    }
}




/** ***************************************************************************
* @brief  Discrete the edges of a input face, get the sample points on the
*         wire of face
* @param  const TopoDS_Face theFace
* @return Handle_TColgp_HSequenceOfPnt  Sample point list
*
* @date 23/06/2013
* @modify 22/04/2016
* @author  Lei Lu
******************************************************************************/
Handle_TColgp_HSequenceOfPnt McCadGeomTool::GetWireSamplePnt(const TopoDS_Face& theFace)
{
    SetPrmt();                                          // Set the basic prameters for the mesh
    Handle(TColgp_HSequenceOfPnt) thePntList = new TColgp_HSequenceOfPnt();

    //BRepClass_FaceClassifier bsc3d;                     // Classifier of face

    BRepAdaptor_Surface bSurf(theFace, Standard_True);  // Create the face adaptor
    GeomAdaptor_Surface gSurf = bSurf.Surface();        // Get the geometry surface

    for (TopExp_Explorer exv(theFace, TopAbs_VERTEX); exv.More(); exv.Next())
    {
        gp_Pnt pnt_vtx = BRep_Tool::Pnt(TopoDS::Vertex(exv.Current()));
        thePntList->Prepend(pnt_vtx);
    }

    /* Traverse the edges of face, add them into sample points list */
    for (TopExp_Explorer ex(theFace, TopAbs_EDGE); ex.More(); ex.Next())
    {
        DiscreteEdge(TopoDS::Edge(ex.Current()),theFace,gSurf,thePntList);
    }

    RemoveRepeatPnt(thePntList);
    return thePntList;
}





/** ***************************************************************************
* @brief  There are some redundant digital of the coordination value which
*         make no sense and will make the collisition detecting errors,
*         so remove them when get the sample points.
* @param  gp_Pnt pnt3d     original point
* @return gp_Pnt pnt3d     modified point
*
* @date 23/06/2013
* @modify 22/04/2016
* @author  Lei Lu
******************************************************************************/
void McCadGeomTool::SimplifyPoint(gp_Pnt & pnt3d)
{
    Standard_Real x = pnt3d.X();
    Standard_Real y = pnt3d.Y();
    Standard_Real z = pnt3d.Z();

#ifdef WIN32
    x = ((long long int)(x*10000000.0))/10000000.0;
    y = ((long long int)(y*10000000.0))/10000000.0;
    z = ((long long int)(z*10000000.0))/10000000.0;
#else //linux
    x = ((long int)(x*10000000.0))/10000000.0;
    y = ((long int)(y*10000000.0))/10000000.0;
    z = ((long int)(z*10000000.0))/10000000.0;
#endif

    pnt3d.SetX(x);
    pnt3d.SetY(y);
    pnt3d.SetZ(z);
}




/** ***************************************************************************
* @brief  If the number of sample points less than 100, then increase the number
*         of sample points
* @param  int iCycle     the times for cycle
*         int &xNum       the number of points on x direction
*         int &yNum       the number of points on y direction
* @return void
*
* @date 23/06/2013
* @modify 22/04/2016
* @author  Lei Lu
******************************************************************************/
void McCadGeomTool::ScaleSampleNum(int iCycle, int &xNum, int &yNum )
{
    for(int i = 0; i < iCycle; i++)
    {
        if(( (xNum-1)*(yNum-1) )<( 10*10 )) //xiugai  lei
        {
            xNum *= 2;
            yNum *= 2;
        }
        else
        {
            break;
        }
    }
}



/** ***************************************************************************
* @brief  Remove the repeated points in the sample point list
* @param  Handle_TColgp_HSequenceOfPnt   thePntList
* @return void
*
* @date 23/06/2013
* @modify 22/04/2016
* @author  Lei Lu
******************************************************************************/
void McCadGeomTool::RemoveRepeatPnt(Handle_TColgp_HSequenceOfPnt & thePntList)
{
    for (int i = 1; i < thePntList->Length(); i++)
    {
         gp_XYZ pA = ( thePntList->Value(i) ).XYZ();
         for (int j = i+1 ; j <= thePntList->Length(); j++)
         {
             gp_XYZ pB = ( thePntList->Value(j) ).XYZ();
             if (pA.IsEqual(pB, 1.e-05))
             {
                 thePntList->Remove(j);
             }
         }
    }
}



/** ***************************************************************************
* @brief  Adjust the number of sample points according to the given minimum
*         lengths of UV coordinations of input edge, and the max and min number
*         of points on each coordinations. The final number of points should be
*         less than maximum value and less than the minimum value.
* @param  Standard_Real dLen        The length of input edge
*         Standard_Real dPdfLen     The minimum length requiremnt on x or y directions
* @return Standard_Integer          The number of points should be layed on the direction
*
* @date 23/06/2013
* @modify 22/04/2016
* @author  Lei Lu
******************************************************************************/
Standard_Integer McCadGeomTool::AdjustSampleNum(Standard_Real dLen, Standard_Real dPdfLen)
{
    Standard_Integer iNumPnt;

    if (int(dLen/dPdfLen) < m_MinNbPnt)
    {
        iNumPnt = m_MinNbPnt;
    }
    else if (int(dLen/dPdfLen) > m_MaxNbPnt)
    {
        iNumPnt = m_MaxNbPnt;
    }
    else
    {
        iNumPnt = (int(dLen/dPdfLen));
    }
    return iNumPnt;
}





/** ***************************************************************************
* @brief  Adjust the number of sample points according to the given minimum
*         length and input edge, and the max and min number of points on each
*         coordinations.
* @param  Standard_Real dLen        The length of input edge
*         Standard_Real dPdfLen     The minimum length requiremnt on x or y directions
* @return Standard_Integer          The number of points should be layed on the direction
*
* @date 23/06/2013
* @modify 22/04/2016
* @author  Lei Lu
******************************************************************************/
Standard_Integer McCadGeomTool::AdjustEdgeSampleNum(Standard_Real dLen, Standard_Real dPdfLen)
{
    Standard_Integer iNumPnt;
    if (int(dLen/dPdfLen) < m_MinNbPnt)
    {
        iNumPnt = m_MinNbPnt*10;
    }
    else if (int(dLen/dPdfLen) > m_MaxNbPnt)
    {
        iNumPnt = m_MaxNbPnt*10;
    }
    else
    {
        iNumPnt = (int(dLen/dPdfLen))*10;
    }
    return iNumPnt;
}




/** ********************************************************************
* @brief Judge the two surfaces are same surfaces or not
*
* @param TopoDS_Face &SurfA, TopoDS_Face &SurfB
* @return Standard_Boolean
*
* @date 13/03/2014
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadGeomTool::IsSameSurfaces(const TopoDS_Face &SurfA,
                                               const TopoDS_Face &SurfB)
{
    TopLoc_Location loc;
    Handle_Geom_Surface hGeomA = BRep_Tool::Surface(SurfA,loc);
    GeomAdaptor_Surface GeomSurfA(hGeomA);

    Handle_Geom_Surface hGeomB = BRep_Tool::Surface(SurfB,loc);
    GeomAdaptor_Surface GeomSurfB(hGeomB);

    if(GeomSurfA.GetType() != GeomSurfB.GetType())
    {
        return Standard_False;
    }

    TopAbs_Orientation orientA = SurfA.Orientation();
    TopAbs_Orientation orientB = SurfB.Orientation();

    if ((orientA != orientB))
    {
        return Standard_False;
    }

    IGeomFace * pGeomFaceA = NULL;
    IGeomFace * pGeomFaceB = NULL;

    switch (GeomSurfA.GetType())
    {
        case GeomAbs_Plane:
        {
            pGeomFaceA = new McCadGeomPlane(GeomSurfA);
            pGeomFaceB = new McCadGeomPlane(GeomSurfB);
            break;
        }
        case GeomAbs_Cylinder:
        {
            pGeomFaceA = new McCadGeomCylinder(GeomSurfA);
            pGeomFaceB = new McCadGeomCylinder(GeomSurfB);
            break;
        }
        case GeomAbs_Cone:
        {
            pGeomFaceA = new McCadGeomCone(GeomSurfA);
            pGeomFaceB = new McCadGeomCone(GeomSurfB);
            break;
        }
        case GeomAbs_Sphere:
        {
            pGeomFaceA = new McCadGeomSphere(GeomSurfA);
            pGeomFaceB = new McCadGeomSphere(GeomSurfB);
            break;
        }
        case GeomAbs_Torus:
        {
            pGeomFaceA = new McCadGeomTorus(GeomSurfA);
            pGeomFaceB = new McCadGeomTorus(GeomSurfB);
            break;
        }
        case GeomAbs_SurfaceOfRevolution:
        {
            pGeomFaceA = new McCadGeomRevolution(GeomSurfA);
            pGeomFaceB = new McCadGeomRevolution(GeomSurfB);            
            break;
        }
        default:break;
    }

    if(NULL != pGeomFaceA && NULL != pGeomFaceB)
    {
        if(pGeomFaceA->IsEqual(pGeomFaceB))
        {
            delete pGeomFaceA;
            delete pGeomFaceB;

            pGeomFaceA = NULL;
            pGeomFaceB = NULL;

            return Standard_True;
        }
        else
        {
            delete pGeomFaceA;
            delete pGeomFaceB;

            pGeomFaceA = NULL;
            pGeomFaceB = NULL;

            return Standard_False;
        }
    }
    else
    {
        return Standard_False;
    }
}



/** ***************************************************************************
* @brief  Calculate the volume of given solid
* @param  TopoDS_Shape theShape*
* @return Standard_Real
*
* @date 14/09/2014
* @modify 22/04/2016
* @author  Lei Lu
******************************************************************************/
Standard_Real McCadGeomTool::GetVolume(const TopoDS_Shape &theShape)
{
    if(theShape.ShapeType() == TopAbs_SOLID)
    {
        GProp_GProps GP;
        BRepGProp::VolumeProperties(theShape,GP);
        return GP.Mass();
    }
    else
    {
        cout<<"Error....Calculate the volume, the input shape is not solid!"<<endl;
    }
}


/** ***************************************************************************
* @brief  Fuse two planes with common straight edge and same geometries
* @param  TopoDS_Face & faceA    input face A
*         TopoDS_Face & faceB    input face B
* @return Standard_Boolean       if there are same edge return true, vis return false
*
* @date 02/05/2016
* @modify 02/05/2016
* @author  Lei Lu
******************************************************************************/
TopoDS_Face McCadGeomTool::FusePlanes(TopoDS_Face &faceA, TopoDS_Face &faceB)
{
    Standard_Real UMin1,UMax1,VMin1,VMax1;
    BRepTools::UVBounds(faceA,UMin1,UMax1,VMin1,VMax1);

    Standard_Real UMin2,UMax2,VMin2,VMax2;
    BRepTools::UVBounds(faceB,UMin2,UMax2,VMin2,VMax2);

    McCadMathTool::ZeroValue(UMin1,1.e-7);
    McCadMathTool::ZeroValue(UMin2,1.e-7);
    McCadMathTool::ZeroValue(UMax1,1.e-7);
    McCadMathTool::ZeroValue(UMax2,1.e-7);
    McCadMathTool::ZeroValue(VMin1,1.e-7);
    McCadMathTool::ZeroValue(VMin2,1.e-7);
    McCadMathTool::ZeroValue(VMax1,1.e-7);
    McCadMathTool::ZeroValue(VMax2,1.e-7);

    Standard_Real UMin,UMax,VMin,VMax;

    (UMin1 <= UMin2) ? UMin = UMin1 : UMin = UMin2;
    (UMax1 >= UMax2) ? UMax = UMax1 : UMax = UMax2;

    (VMin1 <= VMin2) ? VMin = VMin1 : VMin = VMin2;
    (VMax1 >= VMax2) ? VMax = VMax1 : VMax = VMax2;

    TopLoc_Location loc;
    const Handle(Geom_Surface)& aS1 = BRep_Tool::Surface(faceA,loc);

    TopoDS_Face face  = BRepBuilderAPI_MakeFace(aS1, UMin,UMax, VMin, VMax, 1.e-7).Face();

    /// adjust orientation if neccessary - in some cases this is needed (even for fusion of planar faces)!!!
    if(face.Orientation() != faceA.Orientation())
    {
        face.Orientation(faceA.Orientation());
    }

    return face;
}



/** ***************************************************************************
* @brief  Fuse two cylinders with common straight edge and same geometries
* @param  TopoDS_Face & faceA    input face A
*         TopoDS_Face & faceB    input face B
* @return Standard_Boolean       if there are same edge return true, vis return false
*
* @date 02/05/2016
* @modify 02/05/2016
* @author  Lei Lu
******************************************************************************/
TopoDS_Face McCadGeomTool::FuseCylinders(TopoDS_Face &faceA, TopoDS_Face &faceB)
{
    Standard_Real UMin1,UMax1,VMin1,VMax1;
    BRepTools::UVBounds(faceA,UMin1,UMax1,VMin1,VMax1);

    Standard_Real UMin2,UMax2,VMin2,VMax2;
    BRepTools::UVBounds(faceB,UMin2,UMax2,VMin2,VMax2);

    McCadMathTool::ZeroValue(UMin1,1.e-7);
    McCadMathTool::ZeroValue(UMin2,1.e-7);
    McCadMathTool::ZeroValue(UMax1,1.e-7);
    McCadMathTool::ZeroValue(UMax2,1.e-7);
    McCadMathTool::ZeroValue(VMin1,1.e-7);
    McCadMathTool::ZeroValue(VMin2,1.e-7);
    McCadMathTool::ZeroValue(VMax1,1.e-7);
    McCadMathTool::ZeroValue(VMax2,1.e-7);

    Standard_Boolean isClose = Standard_False;
    if ( Abs(UMax1-UMin1)+Abs(UMax2-UMin2) >= 2*M_PI )
    {
        isClose = Standard_True;
    }

    Standard_Real UMin(0.0),UMax(0.0), VMin(0.0), VMax(0.0);

    if(isClose)
    {
        UMin = 0;
        UMax = 2*M_PI;
    }
    else
    {
        Standard_Real UDis1 = fmod(Abs(UMin1-UMax2),2*M_PI);
        Standard_Real UDis2 = fmod(Abs(UMin2-UMax1),2*M_PI);

        if( UDis1 < UDis2)
        {
            UMin = UMin2;
            UMax = UMax1;
        }
        else
        {
            UMin = UMin1;
            UMax = UMax2;
        }
    }

    (VMin1 <= VMin2) ? VMin = VMin1 : VMin = VMin2;
    (VMax1 >= VMax2) ? VMax = VMax1 : VMax = VMax2;

    TopLoc_Location loc;
    const Handle(Geom_Surface)& aS1 = BRep_Tool::Surface(faceA,loc);

    TopoDS_Face face  = BRepBuilderAPI_MakeFace(aS1, UMin,UMax, VMin, VMax, 1.e-7).Face();

    /// adjust orientation if neccessary - in some cases this is needed (even for fusion of planar faces)!!!
    if(face.Orientation() != faceA.Orientation())
    {
        face.Orientation(faceA.Orientation());
    }

    return face;
}



/** ***************************************************************************
* @brief  Fuse two cylinders with common straight edge and same geometries
* @param  TopoDS_Face & faceA    input face A
*         TopoDS_Face & faceB    input face B
* @return Standard_Boolean       if there are same edge return true, vis return false
*
* @date 02/05/2016
* @modify 02/05/2016
* @author  Lei Lu
******************************************************************************/
TopoDS_Face McCadGeomTool::FuseTorus(TopoDS_Face &faceA, TopoDS_Face &faceB)
{
    Standard_Real UMin1,UMax1,VMin1,VMax1;
    BRepTools::UVBounds(faceA,UMin1,UMax1,VMin1,VMax1);

    Standard_Real UMin2,UMax2,VMin2,VMax2;
    BRepTools::UVBounds(faceB,UMin2,UMax2,VMin2,VMax2);

    McCadMathTool::ZeroValue(UMin1,1.e-7);
    McCadMathTool::ZeroValue(UMin2,1.e-7);
    McCadMathTool::ZeroValue(UMax1,1.e-7);
    McCadMathTool::ZeroValue(UMax2,1.e-7);
    McCadMathTool::ZeroValue(VMin1,1.e-7);
    McCadMathTool::ZeroValue(VMin2,1.e-7);
    McCadMathTool::ZeroValue(VMax1,1.e-7);
    McCadMathTool::ZeroValue(VMax2,1.e-7);

    /// Set the new Umin and Umax at the U direction
    Standard_Boolean isUClose = Standard_False;
    if ( Abs(UMax1-UMin1)+Abs(UMax2-UMin2) >= 2*M_PI )
    {
        isUClose = Standard_True;
    }

    Standard_Real UMin(0.0),UMax(0.0), VMin(0.0), VMax(0.0);

    if(isUClose)
    {
        UMin = 0;
        UMax = 2*M_PI;
    }
    else
    {
        Standard_Real UDis1 = fmod(Abs(UMin1-UMax2),2*M_PI);
        Standard_Real UDis2 = fmod(Abs(UMin2-UMax1),2*M_PI);

        if( UDis1 < UDis2)
        {
            UMin = UMin2;
            UMax = UMax1;
        }
        else
        {
            UMin = UMin1;
            UMax = UMax2;
        }
    }

    /// Set the new Vmin and Vmax at the V direction
    Standard_Boolean isVClose = Standard_False;
    if ( Abs(VMax1-VMin1)+Abs(VMax2-VMin2) >= 2*M_PI )
    {
        isVClose = Standard_True;
    }

    if(isVClose)
    {
        VMin = 0;
        VMax = 2*M_PI;
    }
    else
    {
        Standard_Real VDis1 = fmod(Abs(VMin1-VMax2),2*M_PI);
        Standard_Real VDis2 = fmod(Abs(VMin2-VMax1),2*M_PI);

        if( VDis1 < VDis2)
        {
            VMin = VMin2;
            VMax = VMax1;
        }
        else
        {
            VMin = VMin1;
            VMax = VMax2;
        }
    }

    TopLoc_Location loc;
    const Handle(Geom_Surface)& aS1 = BRep_Tool::Surface(faceA,loc);

    TopoDS_Face face  = BRepBuilderAPI_MakeFace(aS1, UMin,UMax, VMin, VMax, 1.e-7).Face();

    /// adjust orientation if neccessary - in some cases this is needed (even for fusion of planar faces)!!!
    if(face.Orientation() != faceA.Orientation())
    {
        face.Orientation(faceA.Orientation());
    }

    return face;
}




/** ***************************************************************************
* @brief  If the two edges are same or not
* @param  TopoDS_Edge &edgeA    $ Input edge A
          TopoDS_Edge &edgeB    $ Input edge B
          Standard_Real dis     $ Distance tolerance
* @return Standard_Boolean
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadGeomTool::IsSameEdge(const TopoDS_Edge &edgeA,
                                           const TopoDS_Edge &edgeB,
                                           Standard_Real dis)
{
    /// Get the start and end points of the curves
    Standard_Real fStartA, fEndA;
    Handle(Geom_Curve) theCurveA = BRep_Tool::Curve(edgeA, fStartA, fEndA);

    Standard_Real fStartB, fEndB;
    Handle(Geom_Curve) theCurveB = BRep_Tool::Curve(edgeB, fStartB, fEndB);

    GeomAdaptor_Curve gp_CurveA(theCurveA);
    GeomAdaptor_Curve gp_CurveB(theCurveB);

    /// If the type of curves are not same, return false.
    if(gp_CurveA.GetType() != gp_CurveB.GetType())
    {
        return Standard_False;
    }

    Standard_Boolean bIsSame = Standard_False;

    if(gp_CurveA.GetType() == GeomAbs_Ellipse)
    {
        bIsSame = IsSameEllipseEdge(edgeA, edgeB, dis);
    }
    else if(gp_CurveA.GetType() == GeomAbs_Circle)
    {
        bIsSame = IsSameCircleEdge(edgeA, edgeB, dis);
    }
    else if(gp_CurveA.GetType() == GeomAbs_Line)
    {
        bIsSame = IsSameLineEdge(edgeA, edgeB, dis);
    }
    else
    {
        return Standard_False;
    }

    return bIsSame;
}





/** ***************************************************************************
* @brief  If the two cylinders have common edge or not
* @param
* @return Bool
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadGeomTool::IsSameEllipseEdge(const TopoDS_Edge &edgeA,
                                                  const TopoDS_Edge &edgeB,
                                                  Standard_Real dis)
{
    /// Get the start and end points of the curves
    Standard_Real fStartA, fEndA;
    Handle(Geom_Curve) theCurveA = BRep_Tool::Curve(edgeA, fStartA, fEndA);

    Standard_Real fStartB, fEndB;
    Handle(Geom_Curve) theCurveB = BRep_Tool::Curve(edgeB, fStartB, fEndB);

    GeomAdaptor_Curve gp_CurveA(theCurveA);
    GeomAdaptor_Curve gp_CurveB(theCurveB);

    gp_Pnt posStartA, posEndA;
    theCurveA->D0(fStartA, posStartA);
    theCurveA->D0(fEndA, posEndA);

    gp_Pnt posStartB, posEndB;
    theCurveB->D0(fStartB, posStartB);
    theCurveB->D0(fEndB, posEndB);   

    gp_Pnt pntMidA;
    Standard_Real fMidA = (fStartA+fEndA)/2.0;
    theCurveA->D0(fMidA, pntMidA);

    gp_Pnt pntMidB;
    Standard_Real fMidB = (fStartB+fEndB)/2.0;
    theCurveA->D0(fMidB, pntMidB);

    // If the middle points are not same, the two ellipses are not same
    if(!pntMidA.IsEqual(pntMidB,1.0e-5))
    {
        return Standard_False;
    }

    gp_Elips elipsA = gp_CurveA.Ellipse();
    gp_Elips elipsB = gp_CurveB.Ellipse();

    // Major and Minor radius are same
    if( Abs(elipsA.MinorRadius() - elipsB.MinorRadius()) <= 1.0e-5
        && Abs(elipsA.MajorRadius() - elipsB.MajorRadius()) <= 1.0e-5 )
    {
        if((posStartA.Distance(posEndB) <= dis && posEndA.Distance(posStartB)<= dis)
            || (posStartA.Distance(posStartB) <= dis && posEndB.Distance(posEndA)<= dis))
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



/** ***************************************************************************
* @brief  If the two cylinders have common edge or not
* @param
* @return Bool
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadGeomTool::IsSameCircleEdge(const TopoDS_Edge &edgeA,
                                                 const TopoDS_Edge &edgeB,
                                                 Standard_Real dis)
{
    /// Get the start and end points of the curves
    Standard_Real fStartA, fEndA;
    Handle(Geom_Curve) theCurveA = BRep_Tool::Curve(edgeA, fStartA, fEndA);

    Standard_Real fStartB, fEndB;
    Handle(Geom_Curve) theCurveB = BRep_Tool::Curve(edgeB, fStartB, fEndB);

    GeomAdaptor_Curve gp_CurveA(theCurveA);
    GeomAdaptor_Curve gp_CurveB(theCurveB);

    gp_Pnt posStartA, posEndA;
    theCurveA->D0(fStartA, posStartA);
    theCurveA->D0(fEndA, posEndA);

    gp_Pnt posStartB, posEndB;
    theCurveB->D0(fStartB, posStartB);
    theCurveB->D0(fEndB, posEndB);

    gp_Pnt pntMidA;
    Standard_Real fMidA = (fStartA+fEndA)/2.0;
    theCurveA->D0(fMidA, pntMidA);

    gp_Pnt pntMidB;
    Standard_Real fMidB = (fStartB+fEndB)/2.0;
    theCurveA->D0(fMidB, pntMidB);

    // If the middle points are not same, the two ellipses are not same
    if(!pntMidA.IsEqual(pntMidB,1.0e-5))
    {
        return Standard_False;
    }

    gp_Circ circA = gp_CurveA.Circle();
    gp_Circ circB = gp_CurveB.Circle();

    // Major and Minor radius are same
    if( Abs(circA.Radius() - circB.Radius()) <= dis
            && circA.Location().IsEqual(circB.Location(),1.0e-5))
    {
        if((posStartA.Distance(posEndB) <= dis && posEndA.Distance(posStartB)<= dis)
            || (posStartA.Distance(posStartB) <= dis && posEndB.Distance(posEndA)<= dis))
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



/** ***************************************************************************
* @brief  Judge the input two edges are same straight edges or not
* @param  TopoDS_Edge & edgeA    input edge A
*         TopoDS_Edge & edgeB    input edge B
* @return Standard_Boolean       if there are same edge return true, vis return false
*
* @date 13/07/2015
* @modify 13/07/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadGeomTool::IsSameLineEdge(const TopoDS_Edge & edgeA,
                                               const TopoDS_Edge & edgeB,
                                               Standard_Real dis)
{
    /// Get the start and end points of the curves
    Standard_Real fStartA, fEndA;
    Handle(Geom_Curve) theCurveA = BRep_Tool::Curve(edgeA, fStartA, fEndA);

    Standard_Real fStartB, fEndB;
    Handle(Geom_Curve) theCurveB = BRep_Tool::Curve(edgeB, fStartB, fEndB);

    gp_Pnt posStartB, posEndB;
    theCurveB->D0(fStartB, posStartB);
    theCurveB->D0(fEndB, posEndB);

    gp_Pnt posStartA, posEndA;
    theCurveA->D0(fStartA, posStartA);
    theCurveA->D0(fEndA, posEndA);

    /// If the start and end points of the two curves are same, then they are same curves
    if ( ( posStartA.IsEqual(posEndB,1.e-5) && posEndA.IsEqual(posStartB,1.e-5) )
         || ( posStartA.IsEqual(posStartB,1.e-5) && posEndB.IsEqual(posEndA,1.e-5)) )
    {
        return Standard_True;
    }
    else
    {
        if((posStartA.Distance(posEndB) <= dis && posEndA.Distance(posStartB)<= dis)
                || (posStartA.Distance(posStartB) <= dis && posEndB.Distance(posEndA)<= dis))
        {
            return Standard_True;
        }
        else
        {
             return Standard_False;
        }
    }
}



gp_Dir McCadGeomTool::NormalOnFace(const TopoDS_Face& F,const gp_Pnt& P)
{
    Standard_Real U, V;
    gp_Pnt pt;
    BRepAdaptor_Surface AS(F, Standard_True);

    switch (AS.GetType())
    {
        case GeomAbs_Plane:
            ElSLib::Parameters(AS.Plane(), P, U, V);
            break;

        case GeomAbs_Cylinder:
            ElSLib::Parameters(AS.Cylinder(), P, U, V);
            break;

        case GeomAbs_Cone:
            ElSLib::Parameters(AS.Cone(), P, U, V);
            break;

        case GeomAbs_Torus:
            ElSLib::Parameters(AS.Torus(), P, U, V);
            break;

        default:
        {
            return gp_Dir(1., 0., 0.);
        }
    }

    gp_Vec D1U, D1V;
    AS.D1(U, V, pt, D1U, D1V);
    gp_Dir N;
    CSLib_DerivativeStatus St;
    CSLib::Normal(D1U, D1V, Precision::Confusion(), St, N);
    if (F.Orientation() == TopAbs_FORWARD)
        N.Reverse();
    return N;
}

