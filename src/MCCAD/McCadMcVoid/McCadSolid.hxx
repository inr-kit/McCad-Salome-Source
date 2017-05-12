/** @file McCadSolid.hxx
  *
  * @brief  This solid is represent a compound solid of CAD file, which will be wrote as a MCNP or Tripoli
  *         cell. Normally, it contains a convex solid list.
  *
  * @author Lei Lu
  * @date   1st.Aug.2012
  */


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
Standard_EXPORT   ~McCadSolid();

private:

    vector <McCadConvexSolid *> m_ConvexSolidList;  /**< Convex solid list included */
    Standard_Integer m_id;                          /**< The ID of this solid */
    Standard_Real m_density;                        /**< The desity of the solid */
    TCollection_AsciiString m_name;                 /**< The name of the solid */

public:

Standard_EXPORT    void AddConvexSolid(McCadConvexSolid *& pConvexSolid);  /**< Set convex solid list */
Standard_EXPORT    vector<McCadConvexSolid *> GetConvexSolidList();        /**< Get convex solid list */

Standard_EXPORT    void SetMaterial(int theID, double theDensity);         /**< Set the material include matid and desity */
Standard_EXPORT    Standard_Integer GetID();                               /**< Get the solid id */
Standard_EXPORT    Standard_Real GetDensity();                             /**< Get the density */
Standard_EXPORT    void SetName(TCollection_AsciiString theName);          /**< Set the name of solid */
Standard_EXPORT    TCollection_AsciiString GetName();                      /**< Get the name of solid */

Standard_EXPORT    vector<McCadTripoliCell *> GetTripoliCellList();        /**< Get the tripoli cell list */

};

#endif // MCCADSOLID_HXX
