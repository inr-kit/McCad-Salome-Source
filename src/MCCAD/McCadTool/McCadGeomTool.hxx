#ifndef MCCADGEOMTOOL_HXX
#define MCCADGEOMTOOL_HXX

#include <Handle_TColgp_HSequenceOfPnt.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Handle_TopTools_HSequenceOfShape.hxx>

class McCadGeomTool
{

public:

    void* operator new(size_t,void* anAddress)
      {
        return anAddress;
      }
    void* operator new(size_t size)
      {
        return Standard::Allocate(size);
      }
    void  operator delete(void *anAddress)
      {
        if (anAddress) Standard::Free((Standard_Address&)anAddress);
      }

public:
    McCadGeomTool();

    Standard_EXPORT static Handle_TColgp_HSequenceOfPnt GetFaceSamplePnt(const TopoDS_Face& theFace);
    Standard_EXPORT static Handle_TColgp_HSequenceOfPnt GetEdgeSamplePnt(const TopoDS_Face& theFace);

    static void DiscreteEdge(   const TopoDS_Edge& theEdge,
                                const TopoDS_Face& theFace,
                                const GeomAdaptor_Surface & theSurf,
                                Handle(TColgp_HSequenceOfPnt) & thePntSeq);

    static void ScaleSampleNum(int iCyc, int &xNum, int &yNum );
    static void RemoveRepeatPnt(Handle_TColgp_HSequenceOfPnt & thePntList);
    static Standard_Integer AdjustSampleNum(Standard_Real dLen, Standard_Real dPreDefLen);
    static Standard_Integer AdjustEdgeSampleNum(Standard_Real dLen, Standard_Real dPdfLen);


    Standard_EXPORT static Standard_Real GetVolume(const TopoDS_Shape &theShape);
    static void SimplifyPoint(gp_Pnt &pnt3d);
    static void SetPrmt();

    //Standard_EXPORT static IsFaceCutSolid(const GeomAdaptor_Surface& theSurf, const gp_Pnt& thePoint)

private:

    static Standard_Real m_MaxNbPnt;
    static Standard_Real m_MinNbPnt;
    static Standard_Real m_Xlen;
    static Standard_Real m_Ylen;
    static Standard_Real m_Rlen;
    static Standard_Real m_Tolerance;

};

#endif // MCCADGEOMTOOL_HXX
