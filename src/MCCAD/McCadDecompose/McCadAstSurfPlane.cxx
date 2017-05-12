#include "McCadAstSurfPlane.hxx"

//qiu #include <McCadGTOOL.hxx>
#include <Handle_TColgp_HSequenceOfPnt.hxx>
#include <TColgp_HSequenceOfPnt.hxx>

#include <gp_Pln.hxx>
#include <gp_Lin.hxx>
#include <BRepAdaptor_Curve.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>

#include <Extrema_ExtPS.hxx>
#include <assert.h>

#include <STEPControl_Writer.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>

#include "../McCadTool/McCadEvaluator.hxx"

McCadAstSurfPlane::McCadAstSurfPlane(const TopoDS_Face &theFace):McCadAstSurface(theFace)
{
    m_SurfType = Plane; //wait for modification

    gp_Pln plane = m_AdpSurface.Plane();    // Get the geometry plane
    gp_Ax3 axis = plane.Position();         // Get the coordinate system Ax3
    m_Dir = axis.Direction();               // Get the direction of plane normal

    m_bCombined = Standard_False;           // The surfaces has already been combined or not?
}

McCadAstSurfPlane::~McCadAstSurfPlane()
{
    if(m_pEdgeLine)
    {
        delete m_pEdgeLine;
        m_pEdgeLine = NULL;
    }
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
Standard_Boolean McCadAstSurfPlane::TriangleCollision(McCadTriangle *& triangle,
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
        Standard_Real aVal = McCadEvaluator::Evaluate(m_AdpSurface, pnt_list->Value(i));

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
* @brief  Judge the two surfaces can be merged or not, if the distance between
*         two surfaces is less than 10.0, and the angle between them is less than
*         0.01*PI, means they are paralle, then they can be merged. Created a new
*         splitting surface through the two edges of merged surfaces.
* @param  McCadAstSurfPlane *pFace (Compared assisted surface)
* @return Boolean
*
* @date   13/05/2015
* @modify 21/05/2015
* @author Lei Lu
******************************************************************************/
//Standard_Boolean McCadAstSurfPlane::CanbeCombined(McCadAstSurfPlane *pFace)
//{
//    if(m_bCombined || pFace->IsCombined())
//    {
//        return Standard_False;
//    }

//    assert(m_pEdge);

//    Standard_Real dis_tol = 50.0;          // Distance between two surfaces, wait modified Lei
//    Standard_Real angle_tol = 0.001*M_PI;  // McCadConvertConfig::GetTolerence();

//    /// If the two edges of plane are not parallel, it can not generate a plane through two edges.
//    McCadEdgeLine *pEdge = pFace->GetEdge();
//    assert(pEdge);

//    if(!m_pEdge->IsParallel(pEdge,angle_tol))
//    {
//         return Standard_False;
//    }

//    /// Calculate the distance between two edges
//    BRepExtrema_DistShapeShape theExtDist(*m_pEdge,*pFace);
//    theExtDist.Perform();
//    if(theExtDist.IsDone())
//    {
//        Standard_Real dist = theExtDist.Value();
//        if(dist > dis_tol) // If the minmum distance between edge and face is large than tolerence
//        {
//           return Standard_False;
//        }
//    }

//    /// Judge the two planes are parallel or not
////    if(m_Dir.IsEqual(pFace->GetDir(),angle_tol) || m_Dir.IsOpposite(pFace->GetDir(),angle_tol))
////    {
////        return Standard_True;
////    }

//    return Standard_True;
//}




/** ***************************************************************************
* @brief Get the direction of assited plane
* @param
*
* @return gp_Dir
*
* @date   13/05/2015
* @modify 21/05/2015
* @author Lei Lu
******************************************************************************/
gp_Dir McCadAstSurfPlane::GetDir()
{
    return m_Dir;
}



/** ***************************************************************************
* @brief Set the edge which plane through and the direction of the edge
* @param TopoDS_Edge
*
* @return void
*
* @date   13/05/2015
* @modify 21/05/2015
* @author Lei Lu
******************************************************************************/
void McCadAstSurfPlane::SetEdge(TopoDS_Edge edge)
{
    m_pEdgeLine = new McCadEdgeLine(edge);
}




/** ***************************************************************************
* @brief  Get the edge of the splitting plane
* @param
*
* @return TopoDS_Edge
*
* @date   13/05/2015
* @modify 21/05/2015
* @author Lei Lu
******************************************************************************/
McCadEdgeLine* McCadAstSurfPlane::GetEdge()const
{
    return m_pEdgeLine;
}




/** ***************************************************************************
* @brief    The plane has been already merged or not
* @param
*
* @return   Standard_Boolean
*
* @date     13/05/2015
* @modify   21/05/2015
* @author   Lei Lu
******************************************************************************/
Standard_Boolean McCadAstSurfPlane::CanbeCombined()
{
    return m_bCombined;
}




/** ***************************************************************************
* @brief   If the plane has already been combined, set the symble as true
* @param   Standard_Boolean bCombined
*
* @return  void
*
* @date    13/05/2015
* @modify  21/05/2015
* @author  Lei Lu
******************************************************************************/
void McCadAstSurfPlane::SetCombined(Standard_Boolean bCombined)
{
    m_bCombined = bCombined;
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
Standard_Boolean McCadAstSurfPlane::IsPntOnSurf(gp_Pnt &thePnt, Standard_Real disTol)
{
    BRepAdaptor_Surface BS(*this, Standard_True);
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

