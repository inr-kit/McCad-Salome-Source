#include "McCadAstSurface.hxx"

//qiu #include <McCadGTOOL.hxx>

#include <Handle_TColgp_HSequenceOfPnt.hxx>

#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <BRepMesh.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <TopTools_HSequenceOfShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>

#include <gp_Pln.hxx>
#include <gp_Ax3.hxx>

#include "../McCadTool/McCadMathTool.hxx"

McCadAstSurface::McCadAstSurface(const TopoDS_Face &theFace):McCadSurface(theFace)
{
    m_bIsAstSurf = Standard_True;
}

McCadAstSurface::~McCadAstSurface()
{

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
Standard_Boolean McCadAstSurface::FaceCollision(McCadBndSurface *& pBndFace,
                                             POSITION & eSide)
{
    Standard_Integer iPosTriNum = 0;   // The number of triangles locating at the positive side of face
    Standard_Integer iNegTriNum = 0;   // The number of triangles locating at the negative side of face

    Standard_Boolean bCollision = Standard_False; // If there are collision between two faces.

    for (int i = 0; i < pBndFace->GetTriangleList().size(); i++)
    {
        POSITION eTriPosition = MIDDLE/*0*/; // The positional relationship between triangle and face

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
