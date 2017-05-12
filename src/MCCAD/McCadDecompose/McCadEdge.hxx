#ifndef MCCADEDGE_HXX
#define MCCADEDGE_HXX

#include <Standard.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include <gp_Pnt.hxx>

enum CURVE{
    Line = 1,
    Circle = 2,
//qiu use lower case to avoid conflict with Ellipse in wingdi.h
//    Ellipse = 3,
    ellipse = 3,
    Spline = 4
};

class McCadEdge : public TopoDS_Edge
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
    McCadEdge();
    McCadEdge(const TopoDS_Edge &theEdge);
    virtual ~McCadEdge();

public:

    Standard_Boolean IsSame(McCadEdge *&pEdge, Standard_Real dis);  /**< The two edges are same edges or not */
    void CopyAttri(McCadEdge *&theEdge);                            /**< Copy the attribution of the edge */

    void AddAstSplitSurf(Standard_Boolean bSplit);     /**< Set the edge is the one can be added assisted splitting surface */
    Standard_Boolean CanAddAstSplitSurf();             /**< The edge can be used to add splitting surface */

    void SetConvexity(Standard_Integer bConvexity);             /**< Set the convexity of the edge */
    Standard_Boolean IsConcaveEdge();                           /**< The edge is concave edge or not */
    CURVE GetType();                                            /**< Get the type of edge curve */

    gp_Pnt StartPoint();    /**< Get the start point of edge */
    gp_Pnt EndPoint();      /**< Get the end point of edge */
    gp_Pnt MidPoint();      /**< Get the middle point of edge */
    gp_Pnt ExtraPoint();    /**< Get the auxilary point of edge */


protected:

    void CalPoints();       /**< Calculate the points of edge */

protected:

    CURVE m_CurveType;                  /**< The type of curve */
    Standard_Boolean m_bCanSplit;       /**< The edge can be used for adding splitting surface or not */
    Standard_Boolean m_bIsConcave;      /**< The edge is concave edge or not */
    Standard_Integer m_iConvexity;      /**< The convexity of edge, -1 is concave, 0 is flat, 1 is convex */
    Standard_Boolean m_bCalPoints;      /**< The points of edge has been calculated or not */

    gp_Pnt m_pntStart;  /**< The start point */
    gp_Pnt m_pntEnd;    /**< The end point */
    gp_Pnt m_pntMid;    /**< The middle point */
    gp_Pnt m_pntExtra;  /**< The extra point */
};

#endif // MCCADDCMPEDGE_HXX
