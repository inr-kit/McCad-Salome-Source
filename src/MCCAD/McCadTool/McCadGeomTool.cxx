#include "McCadGeomTool.hxx"

#include <BRepClass_FaceClassifier.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRep_Tool.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <TopoDS.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>

#include <TopTools_HSequenceOfShape.hxx>

#include "McCadConvertConfig.hxx"

Standard_Real McCadGeomTool::m_MaxNbPnt;
Standard_Real McCadGeomTool::m_MinNbPnt;
Standard_Real McCadGeomTool::m_Xlen;
Standard_Real McCadGeomTool::m_Ylen;
Standard_Real McCadGeomTool::m_Rlen;
Standard_Real McCadGeomTool::m_Tolerance;

McCadGeomTool::McCadGeomTool()
{

}


void McCadGeomTool::SetPrmt()
{
    m_MaxNbPnt = McCadConvertConfig::GetMaxSmplPntNum();
    m_MinNbPnt = McCadConvertConfig::GetMinSmplPntNum();
    m_Xlen = McCadConvertConfig::GetXResolution();
    m_Ylen = McCadConvertConfig::GetYResolution();
    m_Rlen = McCadConvertConfig::GetRResolution();
    m_Tolerance = McCadConvertConfig::GetTolerence();
}


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

    if (gSurf.GetType() == GeomAbs_Sphere ||gSurf.GetType()	== GeomAbs_Torus)
    {
        /*cout << "XLen =  " << xLen  << endl;
        cout << "YLen =  " << yLen  << endl;
        cout << "XNUMPNT =  " << xNumPnt  << endl;
        cout << "YNUMPNT =  " << yNumPnt  << endl;
        cout << "myMinNbPnt =  " << m_MinNbPnt  << endl;*/
    }

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


void McCadGeomTool::DiscreteEdge(const TopoDS_Edge& theEdge,
                                 const TopoDS_Face& theFace,
                                 const GeomAdaptor_Surface & theSurf,
                                 Handle(TColgp_HSequenceOfPnt) & thePntSeq)
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



Handle_TColgp_HSequenceOfPnt McCadGeomTool::GetEdgeSamplePnt(const TopoDS_Face& theFace)
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
    return thePntList;
}




void McCadGeomTool::SimplifyPoint(gp_Pnt & pnt3d)
{
    Standard_Real x = pnt3d.X();
    Standard_Real y = pnt3d.Y();
    Standard_Real z = pnt3d.Z();

    //qiu because in windows system OCC 32bit is used.
    //it cause the problem on precisions, and RemoveRepeatPnt delete a lot of points
    //the code is update to solve the problem
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



void McCadGeomTool::ScaleSampleNum(int iCyc, int &xNum, int &yNum )
{
    for(int i = 0; i < iCyc; i++)
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



Standard_Real McCadGeomTool::GetVolume(const TopoDS_Shape &theShape)
{
    GProp_GProps GP;
    BRepGProp::VolumeProperties(theShape,GP);
    return GP.Mass();
    //return 0;
}



