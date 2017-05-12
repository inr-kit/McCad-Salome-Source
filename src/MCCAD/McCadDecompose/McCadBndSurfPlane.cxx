#include "McCadBndSurfPlane.hxx"

//qiu #include <McCadGTOOL.hxx>
#include <Handle_TColgp_HSequenceOfPnt.hxx>

#include <gp_Pln.hxx>

#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <TopExp_Explorer.hxx>
#include <TColgp_HSequenceOfPnt.hxx>

#include <Extrema_ExtPS.hxx>

#include "../McCadTool/McCadGeomTool.hxx"
#include "../McCadTool/McCadEvaluator.hxx"

McCadBndSurfPlane::McCadBndSurfPlane()
{
}

McCadBndSurfPlane::McCadBndSurfPlane(const TopoDS_Face &theFace):McCadBndSurface(theFace)
{
    m_SurfType = Plane;
}

McCadBndSurfPlane::~McCadBndSurfPlane()
{

}


/** ***************************************************************************
* @brief  Calculate the position relationship between face and triangle
* @param  McCadTriangle *& triangle Input triangle
*         POSITION & ePosition      which side of the face the triangle locate
* @return Standard_Boolean          Triangle and face is collied or not
*
* @date 13/05/2015
* @modify 21/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadBndSurfPlane::TriangleCollision(McCadTriangle *& triangle,
                                                        POSITION & eSide)
{
    Standard_Boolean bCollision = Standard_False; // The triangle is collied with face or not

    Standard_Integer iPosPnt = 0;   // The number of vertex of triangle locating at positive side of face
    Standard_Integer iNegPnt = 0;   // The number of vertex of triangle locating at negative side of face

    Handle_TColgp_HSequenceOfPnt pnt_list = triangle->GetVexList();
    for (Standard_Integer i = 1; i <= pnt_list->Length(); i++)
    {
        gp_Pnt point = pnt_list->Value(i);
        if (IsPntOnSurf(point,1.0e-4)) // if the point is on the surface, do not calculate position
        {
            continue;
        }

        /* Distinguish which side does the point located.*/
        Standard_Real aVal = McCadEvaluator::Evaluate(m_AdpSurface, point);

        if (aVal > 1.0e-4)              // Point located on the positive side of face
        {
            iPosPnt ++;
        }
        else if (aVal < -1.0e-4)        // Point located on the negative side of face
        {
            iNegPnt ++;
        }

        if (iPosPnt > 0 && iNegPnt > 0) // The points are on both sides of face
        {
            bCollision = Standard_True;
            break;
        }
        else
        {
            continue;
        }
    } 

    if (iPosPnt > 0 && iNegPnt == 0)        // The triangle on positive side of face
    {
        eSide = POSITIVE;
    }
    else if (iNegPnt > 0 && iPosPnt == 0)   // The triangle on negative side of face
    {
        eSide = NEGATIVE;
    }

    return bCollision;
}



/** ***************************************************************************
* @brief The two planes can be fused or not (currently set they can not be fused,
*        will be modified)
* @param McCadBndSurface * pSurf
* @return Standard_Boolean
*
* @date 12/05/2016
* @modify 12/05/2016
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadBndSurfPlane::CanFuseSurf(McCadBndSurface *& pSurf)
{
    if(!this->IsSameSurface(pSurf))
    {
       return Standard_False;
    }

    /// Judge the surfaces have the common edge
    for(unsigned int i = 0; i < this->GetEdgeList().size(); i++)
    {
        McCadEdge *pEdgeA = GetEdgeList().at(i);

        for(unsigned int j = 0; j < pSurf->GetEdgeList().size(); j++)
        {
            McCadEdge *pEdgeB = pSurf->GetEdgeList().at(j);

            if(pEdgeA->IsSame(pEdgeB, 1.e-5))
            {
                return Standard_True;
            }
        }
    }

    return Standard_False;
}


/** ******************************************************************************
* @brief Set the length the generated extended plane
* @param Standard_Real length
* @return void
*
* @date 25/04/2016
* @modify
* @author Lei Lu
*********************************************************************************/
void McCadBndSurfPlane::GenExtPlane(Standard_Real length)
{
    m_ExtLength = length;

    TopLoc_Location loc;
    Handle(Geom_Surface) surface = BRep_Tool::Surface(*this,loc);

    Standard_Real UMin,UMax, VMin, VMax;
    BRepTools::UVBounds(*this,UMin,UMax, VMin, VMax);

    Standard_Real VMid = (VMin + VMax)/2.0;
    Standard_Real UMid = (UMin + UMax)/2.0;
    Standard_Real ext_length = m_ExtLength/2.0;

    VMin = VMid - ext_length;
    VMax = VMid + ext_length;
    UMin = UMid - ext_length;
    UMax = UMid + ext_length;

    m_ExtSurf = BRepBuilderAPI_MakeFace(surface,UMin,UMax,VMin,VMax,1.e-7);
}




/** ***************************************************************************
* @brief  Calculate the distance between point and surface, if it less than
*         distance tolerence, the point is consided on the surface.
* @param  point(gp_Pnt), tolerence(Standard_Real)
* @return
*
* @date 08/10/2015
* @modify 08/10/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadBndSurfPlane::IsPntOnSurf(gp_Pnt &thePnt, Standard_Real disTol)
{
    BRepAdaptor_Surface BS(m_ExtSurf, Standard_True);
    Standard_Real uvTol = BS.Tolerance();

    Standard_Real fu = BS.FirstUParameter();
    Standard_Real lu = BS.LastUParameter();
    Standard_Real fv = BS.FirstVParameter();
    Standard_Real lv = BS.LastVParameter();

    Extrema_ExtPS extPS(thePnt,BS,fu,lu,fv,lv,uvTol,uvTol);
    if(extPS.IsDone() && extPS.NbExt() != 0)
    {
        gp_Pnt pnt  = thePnt;
        gp_Pnt pntSurf = extPS.Point(1).Value(); // The nearest point on the surface
        // Calculate the distance between surface and point
        Standard_Real dis = Sqrt(pow(pnt.X()-pntSurf.X(),2)+pow(pntSurf.Y()-pnt.Y(),2)+pow(pnt.Z()-pntSurf.Z(),2));

        if(dis < disTol)
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
* @brief Fuse the two planes
*
* @param McCadBndSurface *& pSurf
* @return TopoDS_Face face
*
* @date 12/05/2016
* @author 02/05/2016
*********************************************************************************/
TopoDS_Face McCadBndSurfPlane::FuseSurfaces(McCadBndSurface *& pSurf)
{
    TopoDS_Face face = McCadGeomTool::FusePlanes(*this, *pSurf);
    return face;
}



//Standard_Boolean McCadBndSurfPlane::FaceCollision(McCadBndSurface *& pBndFace,
//                                                POSITION & eSide)
//{
//    Standard_Boolean bCollision = Standard_False; // If there are collision between two faces.
//    if(pBndFace->GetSurfType() == Cylinder)
//    {
//        Bnd_Box bndBox  = pBndFace->GetBndBox();
//        if(!BoxCollision(bndBox))
//        {
//            eSide = POSITIVE;
//            return bCollision;
//        }
//    }
//    else
//    {
//        bCollision = McCadBndSurface::FaceCollision(pBndFace,eSide);
//        return bCollision;
//    }
//}


//Standard_Boolean McCadBndSurfPlane::BoxCollision(Bnd_Box &bndBox)
//{
//    Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
//    bndBox.Get(xMin, yMin, zMin, xMax, yMax, zMax);

//    TopoDS_Shape Box = BRepPrimAPI_MakeBox(gp_Pnt(xMin,yMin,zMin),
//                                           gp_Pnt(xMax,yMax,zMax)).Shape();

//    Standard_Integer iPosPnt = 0;   // The number of vertex of triangle locating at positive side of face
//    Standard_Integer iNegPnt = 0;   // The number of vertex of triangle locating at negative side of face

//    Standard_Boolean bCollision = Standard_False; // If there are collision between two faces.

//    for(TopExp_Explorer ex(Box, TopAbs_VERTEX); ex.More(); ex.Next())
//    {
//        TopoDS_Vertex vertex = TopoDS::Vertex(ex.Current());

//        // if the point is on the surface, do not calculate the position between them.
//        gp_Pnt point = BRep_Tool::Pnt(vertex);
//        if (IsPntOnSurf(point,1.0e-4))
//        {
//            continue;
//        }

//        /* Distinguish which side does the point located.*/
//        Standard_Real aVal = McCadGTOOL::Evaluate(m_AdpSurface, point);

//        if (aVal > 1.0e-1)              // Point located on the positive side of face
//        {
//            iPosPnt ++;
//        }
//        else if (aVal < -1.0e-1)        // Point located on the negative side of face
//        {
//            iNegPnt ++;
//        }

//        if (iPosPnt > 0 && iNegPnt > 0) // The points are on both sides of face
//        {
//            bCollision = Standard_True;
//            break;
//        }
//        else
//        {
//            continue;
//        }
//    }

//    return bCollision;
//}
