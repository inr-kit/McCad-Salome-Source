#ifndef MCCADTRIANGLE_HXX
#define MCCADTRIANGLE_HXX

#include <TopoDS_Face.hxx>
#include <gp_Pnt.hxx>

#include <vector>
#include <Handle_TColgp_HSequenceOfPnt.hxx>

#include <Standard.hxx>
#include <Bnd_Box.hxx>

using namespace std;

class McCadTriangle : public TopoDS_Face
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

    McCadTriangle(const McCadTriangle & theTriangle);
    McCadTriangle(const TopoDS_Face & theFace);
    //McCadTriangle(const McCadTriangle* );
    ~McCadTriangle();

    void AddVertex(gp_Pnt pnt);                     /**< Add vertex of the triangle */
    void SetSurfNum(Standard_Integer iSurfNum);     /**< The triangle belong to which surface */
    Standard_Integer GetSurfNum() const;            /**< Get the contained surface number */
    Handle_TColgp_HSequenceOfPnt GetVexList() const;/**< Get the vertex list */
    Bnd_Box GetBndBox() const;                      /**< Get the boundary box of triangle */

private:
    //vector<gp_Pnt> m_PntList;
    Handle_TColgp_HSequenceOfPnt m_PntList;         /**< vertex of triangle */    
    Standard_Integer m_iSurfNum;                    /**< The surface triangle belong to */
    Standard_Integer m_iNumOfPnt;                   /**< How many vertex in the triangle */

    Bnd_Box m_BndBox;
    };

#endif // MCCADTRIANGLE_HXX
