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

Standard_EXPORT    McCadSolid();
Standard_EXPORT    ~McCadSolid();

private:

    vector <McCadConvexSolid *> m_ConvexSolidList;          /**< Convex solid list */
    Standard_Integer m_id;
    Standard_Real m_density;
    TCollection_AsciiString m_name;    

public:

Standard_EXPORT    void AddConvexSolid(McCadConvexSolid *& pConvexSolid);  /**< Set convex solid list */
Standard_EXPORT    vector<McCadConvexSolid *> GetConvexSolidList();        /**< Get convex solid list */

Standard_EXPORT    void SetMaterial(int theID, double theDensity);
Standard_EXPORT    Standard_Integer GetID(){return m_id;};
Standard_EXPORT    Standard_Real GetDensity(){return m_density;};
Standard_EXPORT    void SetName(TCollection_AsciiString theName);
Standard_EXPORT    TCollection_AsciiString GetName(){return m_name;};

Standard_EXPORT    vector<McCadTripoliCell *> GetTripoliCellList();

};

#endif // MCCADSOLID_HXX
