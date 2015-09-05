#include "Material.hxx"

Material::Material()
{
}

Material::~Material()
{
}


/** ********************************************************************
* @brief Set material
*
* @param int theID,                  material ID
*        QString theName,            material name
*        double theDensity,          material density
*        QString theMatCard          material card include compositions
* @return
*
* @date 12/11/2013
* @author  Lei Lu
************************************************************************/
Material::Material(int theID, QString theName, double theDensity, QString theMatCard)
{
    m_ID = theID;
    m_Name = theName;
    m_Density = theDensity;
    m_MatCard = theMatCard;
}


/** ********************************************************************
* @brief Set material
*
* @param int theID,                  material ID
*        QString theName,            material name
*        double theDensity,          material density
*        QString theMatCard          material card include compositions
* @return
*
* @date 12/11/2013
* @author  Lei Lu
************************************************************************/
void Material::SetMaterial(int theID, QString theName, double theDensity, QString theMatCard)
{
    m_ID = theID;
    m_Name = theName;
    m_Density = theDensity;
    m_MatCard = theMatCard;
}
