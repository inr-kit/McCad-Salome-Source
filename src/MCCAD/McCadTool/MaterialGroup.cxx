#include "MaterialGroup.hxx"

MaterialGroup::MaterialGroup()
{
}

MaterialGroup::MaterialGroup(int iStartNum, int iEndNum, QString theName, Material theMaterial)
{
    m_iStartNum = iStartNum;
    m_iEndNum = iEndNum;
    m_iCellNum = iEndNum - iStartNum + 1;
    m_strGroupName = theName;
    m_Material = theMaterial;
}

MaterialGroup::~MaterialGroup()
{
}


QString MaterialGroup::GetMaterialName()
{
    return m_Material.GetMatName();
}

double MaterialGroup::GetDensity()
{
    return m_Material.GetMatDensity();
}

int MaterialGroup::GetMaterialID()
{
    return m_Material.GetMatID();
}

QString MaterialGroup::GetMaterialCard()
{
    return m_Material.GetMatCard();
}
