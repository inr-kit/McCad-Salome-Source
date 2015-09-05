#ifndef MCCADSOLID_HXX
#define MCCADSOLID_HXX

#include <TCollection_AsciiString.hxx>
#include "McCadConvexSolid.hxx"

class McCadSolid
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

    McCadSolid();
    ~McCadSolid();

private:

    vector <McCadConvexSolid *> m_ConvexSolidList;          /**< Convex solid list */
    Standard_Integer m_id;
    Standard_Real m_density;
    TCollection_AsciiString m_name;    

public:

    void AddConvexSolid(McCadConvexSolid *& pConvexSolid);  /**< Set convex solid list */
    vector<McCadConvexSolid *> GetConvexSolidList();        /**< Get convex solid list */

    void SetMaterial(int theID, double theDensity);
    Standard_Integer GetID(){return m_id;};
    Standard_Real GetDensity(){return m_density;};
    void SetName(TCollection_AsciiString theName);
    TCollection_AsciiString GetName(){return m_name;};

    vector<McCadTripoliCell *> GetTripoliCellList();

};

#endif // MCCADSOLID_HXX
