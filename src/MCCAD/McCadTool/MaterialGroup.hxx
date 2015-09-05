#ifndef MATERIALGROUP_HXX
#define MATERIALGROUP_HXX

#include "Material.hxx"

class MaterialGroup
{
public:
    MaterialGroup();
    MaterialGroup(int iStartNum, int iEndNum, QString theName, Material theMaterial);
    ~MaterialGroup();

private:
    int m_iStartNum;        // the start number of material group
    int m_iEndNum;          // the end number of material group

    int m_iCellNum;         // the number of cells in this group
    Material m_Material;    // the material of this group

    QString m_strGroupName; // the name of this group


public:
    int GetStartNum(){return m_iStartNum;};
    int GetEndNum(){return m_iEndNum;};
    int GetCellNum(){return m_iCellNum;};
    QString GetGroupName(){return m_strGroupName;};

    QString GetMaterialName();
    QString GetMaterialCard();
    double GetDensity();
    int GetMaterialID();
};

#endif // MATERIALGROUP_HXX
