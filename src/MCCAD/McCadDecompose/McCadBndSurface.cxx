#include "McCadBndSurface.hxx"
//qiu add SALOME definition
#ifndef SALOME
#include <McCadMessenger_Singleton.hxx>
#endif
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColgp_Array1OfPnt.hxx>

#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <BRepMesh.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAlgoAPI_Section.hxx>

#include <Poly_Triangulation.hxx>

#include <Handle_TColgp_HSequenceOfPnt.hxx>

#include <gp_Pln.hxx>
#include <gp_Ax3.hxx>
#include <GeomAdaptor_Curve.hxx>

#include <STEPControl_Writer.hxx>
#include <ShapeFix_Shape.hxx>

#include "McCadEdgeLine.hxx"
#include "McCadEdgeCircle.hxx"
#include "McCadEdgeEllipse.hxx"
#include "McCadEdgeSpline.hxx"

#include "../McCadTool/McCadMathTool.hxx"
#include <assert.h>

McCadBndSurface::McCadBndSurface()
{
}

McCadBndSurface::McCadBndSurface(const TopoDS_Face &theFace):McCadSurface(theFace)
{
    m_iInternalLoops = CountInternalLoops();
}

McCadBndSurface::~McCadBndSurface()
{
    Free(); // Free the triangle and edge list
}



/** ***************************************************************************
* @brief  Free the triangle and edge list, when the object is destroied
* @param
* @return void
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
void McCadBndSurface::Free()
{
    // Free the triangle list
    for (int i = 0; i < m_TriangleList.size(); i++)
    {
        McCadTriangle *pTriangle = m_TriangleList.at(i);
        m_TriangleList.erase(m_TriangleList.begin()+i);

        delete pTriangle;
        pTriangle = NULL;
    }
    m_TriangleList.clear();

    // Free the edge list
    for(int i = 0; i < m_EdgeList.size(); i++)
    {
        McCadEdge *pEdge = m_EdgeList.at(i);
        m_EdgeList.erase(m_EdgeList.begin()+i);
        delete pEdge;
        pEdge = NULL;
    }

    m_EdgeList.clear();

    //std::vector<McCadTriangle*> tmp = m_TriangleList;
    //m_TriangleList.swap(tmp);
}


/** ***************************************************************************
* @brief  Calculate the position relationship between faces
* @param  McCadBndSurface *& pBndFace  Compared face
*         POSITION & ePosition         Which side of the face the triangle locate
* @return Standard_Boolean             The faces are collied or not
*
* @date 13/05/2015
* @modify 21/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadBndSurface::FaceCollision(McCadBndSurface *& pBndFace,
                                                POSITION & eSide)
{
    Standard_Boolean bCollision = Standard_False; // If there are collision between two faces.

    Standard_Integer iPosTriNum = 0;   // The number of triangles locating at the positive side of face
    Standard_Integer iNegTriNum = 0;   // The number of triangles locating at the negative side of face

    for (int i = 0; i < pBndFace->GetTriangleList().size(); i++)
    {
        POSITION eTriPosition =MIDDLE /*0*/; // The positional relationship between triangle and face

        McCadTriangle *pTriangle = pBndFace->GetTriangleList().at(i);
        if (this->TriangleCollision(pTriangle,eTriPosition)) // The triangle is collied with face
        {
            bCollision = Standard_True;           
            break;
        }
        else
        {
            if (eTriPosition == POSITIVE)       // The triangle is on the positive side of face
            {
                iPosTriNum++;
            }
            else if(eTriPosition == NEGATIVE)   // The triangle is on the negative side of face
            {
                iNegTriNum++;
            }
        }

        if (iPosTriNum > 0 && iNegTriNum > 0) // If the triangles locate at the both sides of face
        {
            bCollision = Standard_True;
            break;
        }
    }

    if (iPosTriNum > 0 && iNegTriNum == 0)
    {
        eSide = POSITIVE; // Compared face locate completly at the positive side of this face
    }
    else if (iPosTriNum == 0 && iNegTriNum >0)
    {
        eSide = NEGATIVE;// Compared face locate completly at the negative side of this face
    }   

    return bCollision;
}



/** ***************************************************************************
* @brief  Generate the triangles of surface,  and add into the list.
* @param
* @return If the mesh function is sucessful, return true, or it return false
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
Standard_Boolean McCadBndSurface::GenTriangles(Standard_Real aDeflection)
{
    if (GetTriangleList().size() != 0)
    {
        return Standard_True;
    }  

    Handle(Poly_Triangulation) mesh;
    BRepAdaptor_Surface BS(*this,Standard_True);
    gp_Trsf T = BS.Trsf();

    /** Generate the meshes of face **/
    try
    {
        TopLoc_Location loc;
        BRepMesh::Mesh(*this, aDeflection);
        mesh = BRep_Tool::Triangulation(*this,loc);
    }
    catch(...)
    {
#ifndef SALOME
        McCadMessenger_Singleton *msgr = McCadMessenger_Singleton::Instance();
        msgr->Message("McCadBndSurface::GenTriangles :: Error: Face meshing has errors",
                      McCadMessenger_DefaultMsg);
#else
        cout <<"McCadBndSurface::GenTriangles :: Error: Face meshing has errors!" <<endl;
#endif
        return Standard_False;
    }

    if (!mesh.IsNull())
    {
        Standard_Integer nNodes = mesh->NbNodes();
        TColgp_Array1OfPnt meshPnts(1,nNodes);
        meshPnts = mesh->Nodes();

        Standard_Integer nbTriangles = mesh->NbTriangles();

        Standard_Integer n1, n2, n3;                                // For getting mesh points
        const Poly_Array1OfTriangle& Triangles = mesh->Triangles(); // Get the triangle

        for (int i = 1; i <= nbTriangles; i++)
        {
            Poly_Triangle tri = Triangles(i);
            tri.Get(n1, n2, n3);
            gp_Pnt P1 = (meshPnts(n1)).Transformed(T);
            gp_Pnt P2 = (meshPnts(n2)).Transformed(T);
            gp_Pnt P3 = (meshPnts(n3)).Transformed(T);

            /** Generate a new face using the points of triangle **/
            TopoDS_Wire wire = BRepBuilderAPI_MakePolygon(P1,P2,P3,Standard_True);
            TopoDS_Face face = BRepBuilderAPI_MakeFace(wire,Standard_True);

            McCadTriangle *pTri = new McCadTriangle(face);
            pTri->AddVertex(P1);
            pTri->AddVertex(P2);
            pTri->AddVertex(P3);           

            pTri->SetSurfNum(m_iSurfNum);           
            m_TriangleList.push_back(pTri);      
        }
    }
    else
    {       
        return Standard_False;
    }

    return Standard_True;
}



/** ***************************************************************************
* @brief  Return the triangles list of face
* @param
* @return vector<McCadTriangle *>
*
* @date 13/05/2015
* @modify 13/05/2015
* @author  Lei Lu
******************************************************************************/
vector<McCadTriangle *> McCadBndSurface::GetTriangleList()
{
    return m_TriangleList;
}




/** ***************************************************************************
* @brief Trace the triangles of a face and add them into the triangle into the
*        list of current face.
* @param McCadBndSurface *& pFace
* @return void
*
* @date 13/03/2016
* @modify 13/03/2015
* @author  Lei Lu
******************************************************************************/
void McCadBndSurface::AddTriangles(McCadBndSurface *& pFace)
{
    for (int i = 0; i < pFace->GetTriangleList().size(); i++)
    {
        McCadTriangle *pTriangle = pFace->GetTriangleList().at(i);
        McCadTriangle *pNewTriangle = new McCadTriangle(*pTriangle); // Copy the triangles
        m_TriangleList.push_back(pNewTriangle);
    }
}



/** ***************************************************************************
* @brief  Count how many internal loops in the surface
* @param
* @return void
*
* @date 29/03/2016
* @modify 29/03/2016
* @author  Lei Lu
******************************************************************************/
Standard_Integer McCadBndSurface::CountInternalLoops()
{
    TopExp_Explorer exW;            // Trace the face of input solid
    int iLoopNum = 0;
    for (exW.Init(*this,TopAbs_WIRE); exW.More(); exW.Next())
    {
        iLoopNum++;
        TopoDS_Wire wire = TopoDS::Wire(exW.Current());
        BRepTools::Update(wire);
    }
    return iLoopNum-1;
}


void McCadBndSurface::SetLoopNum(Standard_Integer iLoopNum)
{
    m_iInternalLoops = iLoopNum;
}


/** ***************************************************************************
* @brief  Get the edge list
* @param
* @return vector<McCadDcompEdge*>
*
* @date 02/05/2016
* @modify
* @author  Lei Lu
******************************************************************************/
vector<McCadEdge*> McCadBndSurface::GetEdgeList() const
{
    if(!m_EdgeList.empty())
    {
        return m_EdgeList;
    }
}



/** ***************************************************************************
* @brief  Copy edge and create a object
* @param  McCadEdge * pEdge
* @return McCadEdge *
*
* @date 02/05/2016
* @modify 06/06/2016
* @author  Lei Lu
******************************************************************************/
McCadEdge* McCadBndSurface::CopyEdge(McCadEdge *& pEdge)
{
    McCadEdge *pCopyEdge = NULL;
    switch (pEdge->GetType())
    {
        case Line:
        {
            pCopyEdge = new McCadEdgeLine(*pEdge);
            break;
        }
        case Circle:
        {
            pCopyEdge = new McCadEdgeCircle(*pEdge);
            break;
        }
        case ellipse:
        {
            pCopyEdge = new McCadEdgeEllipse(*pEdge);
            break;
        }
        default:
            pCopyEdge = new McCadEdgeSpline(*pEdge);
            break;
    }

    pCopyEdge->CopyAttri(pEdge);  // Copy the attributions of edges
    return pCopyEdge;
}



/** ***************************************************************************
* @brief  Fuse the surfaces, and add the edges of each one into the new fused
*         surface
* @param  McCadBndSurface * pSurfA
*         McCadBndSurface * pSurfB
* @return void
*
* @date 02/05/2016
* @modify
* @author  Lei Lu
******************************************************************************/
void McCadBndSurface::CombineEdges(McCadBndSurface *& pSurfA,McCadBndSurface *& pSurfB )
{
    m_EdgeList.clear(); // Remove the orignal edge list
    for(unsigned int i = 0; i < pSurfA->GetEdgeList().size(); i++ )
    {
        McCadEdge *pEdgeA = pSurfA->GetEdgeList().at(i);
        McCadEdge *pNewEdgeA = CopyEdge(pEdgeA);
        m_EdgeList.push_back(pNewEdgeA);
    }

    Standard_Boolean bHasSameEdge;
    for(unsigned int j = 0; j < pSurfB->GetEdgeList().size(); j++ )
    {
        McCadEdge *pEdgeB = pSurfB->GetEdgeList().at(j);
        McCadEdge *pNewEdgeB = CopyEdge(pEdgeB);      

        bHasSameEdge = Standard_False;
        int iSame = 0; // The index of edge list which are repeated.

        for(unsigned int k = 0; k < m_EdgeList.size(); k++ )
        {
            McCadEdge *pEdge = m_EdgeList.at(k);
            if(pNewEdgeB->IsSame(pEdge,1.0e-5)) // If the edge are same, then remove the common edge
            {
                bHasSameEdge = Standard_True;
                iSame = k;
                break;
            }
        }       

        // If the two surfaces to be merged, if they have same edge, remove the same edge.
        if (!bHasSameEdge)
        {
            m_EdgeList.push_back(pNewEdgeB);   // Add into the edge list
        }
        else
        {
            m_EdgeList.erase(m_EdgeList.begin()+ iSame); // Remove from the edge list
        }
    }
}


/** ***************************************************************************
* @brief  Add the edge into the edge list
* @param  McCadEdge *& pEdge
* @return void
*
* @date 02/06/2016
* @author  Lei Lu
******************************************************************************/
void McCadBndSurface::AddEdge(McCadEdge *& pEdge)
{
    assert(pEdge);
    m_EdgeList.push_back(pEdge);
}

