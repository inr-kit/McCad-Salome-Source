#include "McCadStitchSurfaces.hxx"

#include <TopAbs_Orientation.hxx>
#include <Handle_Geom_Surface.hxx>
#include <BRep_Tool.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Surface.hxx>


#include "../McCadTool/McCadGeomTool.hxx"

McCadStitchSurfaces::McCadStitchSurfaces()
{
}

McCadStitchSurfaces::~McCadStitchSurfaces()
{
}


/** ********************************************************************
* @brief Judge the two surfaces can be fused or not, from the geometries
*        and whether they have the common edge or not. It is different
*        from combine surface function, this function is only used for
*        process the cylinders which are separated into two pieces. But
*        the combination function is used for remove some internal loop
*        in one surface.
*
* @param
* @return bool
*
* @date 04/01/2014
* @author  Lei Lu
***********************************************************************/
Standard_Boolean McCadStitchSurfaces::CanbeStitched(McCadExtBndFace *& pFaceA, McCadExtBndFace *& pFaceB)
{
    /// Get the geometries of the faces and judge they are same or not.
    TopLoc_Location loc;
    Handle_Geom_Surface hGeomA = BRep_Tool::Surface(*pFaceA,loc);
    GeomAdaptor_Surface GemoAdptSurfA(hGeomA);

    Handle_Geom_Surface hGeomB = BRep_Tool::Surface(*pFaceB,loc);
    GeomAdaptor_Surface GemoAdptSurfB(hGeomB);

    if(GemoAdptSurfA.GetType() != GemoAdptSurfB.GetType())
    {
        return Standard_False;
    }

    /// The planes are no need to add assisted surfaces, so they are no need to be combined
    if( GemoAdptSurfA.GetType() == GeomAbs_Plane)
    {
        return Standard_False;
    }

    TopAbs_Orientation orientA = pFaceA->Orientation();
    TopAbs_Orientation orientB = pFaceB->Orientation();
    if ((orientA != orientB))
    {
        return Standard_False;
    }

    if( GemoAdptSurfA.GetType() == GeomAbs_Torus
            && GemoAdptSurfB.GetType() == GeomAbs_Torus)
    {
        if(IsSameSurface(*pFaceA,*pFaceB))
                //&&HasCommonEdge(*pFaceA,*pFaceB))
        {            
            return Standard_True;
        }
        else
        {
            return Standard_False;
        }
    }
    else
    {
        /// The convex surfaces donot need to be combined, because they donot need to be add assisted surfaces
        if(orientA == TopAbs_FORWARD || orientB == TopAbs_FORWARD)
        {
            return Standard_False;
        }

        if(IsSameSurface(*pFaceA,*pFaceB)
                &&HasCommonEdge(*pFaceA,*pFaceB))
        {
            return Standard_True;
        }
        else
        {
            return Standard_False;
        }
    }

    return false;
}



/** ***************************************************************************
* @brief  Judge the two surfaces are same surface or not
* @param  TopoDS_Face &faceA, TopoDS_Face &faceB
* @return Boolean
*
* @date   09/03/2016
* @modify 27/07/2016
* @author Lei Lu
******************************************************************************/
Standard_Boolean McCadStitchSurfaces::IsSameSurface(TopoDS_Face &faceA, TopoDS_Face &faceB) const
{
    if (McCadGeomTool::IsSameSurfaces(faceA, faceB))
    {
        return Standard_True;
    }
    else
    {
        return Standard_False;
    }
}




/** ***************************************************************************
* @brief  Judge the two surfaces have common edge or not
* @param  TopoDS_Face &faceA, TopoDS_Face &faceB
* @return Boolean
*
* @date   09/03/2016
* @modify 27/07/2016
* @author Lei Lu
******************************************************************************/
Standard_Boolean McCadStitchSurfaces::HasCommonEdge(TopoDS_Face &faceA, TopoDS_Face &faceB) const
{
    /// Judge the surfaces have the common edge
    for(TopExp_Explorer ex1(faceA, TopAbs_EDGE); ex1.More(); ex1.Next())
    {
        TopoDS_Edge edgeA = TopoDS::Edge(ex1.Current());
        for(TopExp_Explorer ex2(faceB, TopAbs_EDGE); ex2.More(); ex2.Next())
        {
            TopoDS_Edge edgeB = TopoDS::Edge(ex2.Current());
            if ( McCadGeomTool::IsSameEdge(edgeA, edgeB, 1.0e-4))
            {                
                return Standard_True;
            }
        }
    }    
    return Standard_False;
}




/** ***************************************************************************
* @brief  Combine the two connected same surfaces and generate a new surface
* @param  McCadExtFace *& pFaceA,McCadExtFace *& pFaceB
* @return McCadExtFace *
*
* @date   27/07/2016
* @modify 27/07/2016
* @author Lei Lu
******************************************************************************/
TopoDS_Face McCadStitchSurfaces::Stitch(McCadExtBndFace *& pFaceA, McCadExtBndFace *& pFaceB )
{
    TopoDS_Face newFace;
    if(pFaceA->GetFaceType() == GeomAbs_Cylinder
            || pFaceA->GetFaceType() == GeomAbs_Cone)
    {
       newFace = McCadGeomTool::FuseCylinders(*pFaceA, *pFaceB);
    }
    else if ((pFaceA->GetFaceType() == GeomAbs_Torus
              || pFaceA->GetFaceType() == GeomAbs_SurfaceOfRevolution))
    {
       newFace = McCadGeomTool::FuseTorus(*pFaceA, *pFaceB);
    }

    return newFace;
}
