#include "McCadSolid.hxx"
#include <assert.h>

McCadSolid::McCadSolid()
{
    m_id = 0;
    m_density = 0;
}

McCadSolid::~McCadSolid()
{
}


/** ********************************************************************
* @brief Add convex solids into convex solid list. A solid is consist of
*        many convex solids commonly.
*
* @param McCadConvexSolid *pConvexSolid
* @return void
*
* @date 2/4/2013
* @author  Lei Lu
***********************************************************************/
void McCadSolid::AddConvexSolid(McCadConvexSolid *& pConvexSolid)
{
    m_ConvexSolidList.push_back(pConvexSolid);
}



/** ********************************************************************
* @brief Get a convex solid list of a solid
*
* @param
* @return vector<McCadConvexSolid *>
*
* @date 2/4/2013
* @author  Lei Lu
***********************************************************************/
vector<McCadConvexSolid *> McCadSolid::GetConvexSolidList()
{
    return m_ConvexSolidList;
}



/** ********************************************************************
* @brief Set Material Id and Density
*
* @param double fDensity
* @return void
*
* @date 05/11/2013
* @author  Lei Lu
***********************************************************************/
void McCadSolid::SetMaterial(int theID, double theDensity)
{
    m_id = theID;
    m_density = theDensity;
}



/** ********************************************************************
* @brief Set Material Name
*
* @param TCollection_AsciiString theMatName
* @return void
*
* @date 05/11/2013
* @author  Lei Lu
***********************************************************************/
void McCadSolid::SetName(TCollection_AsciiString theMatName)
{
   m_name = theMatName;
}
