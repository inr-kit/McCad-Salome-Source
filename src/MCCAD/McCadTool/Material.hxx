#ifndef MATERIAL_HXX
#define MATERIAL_HXX

#include <TCollection_AsciiString.hxx>
#include <QString>

class Material
{
public:
    Material();
    Material(int theID, QString theName,
             double theDensity,QString theMatCard);
    ~Material();

private:
    int m_ID;               // Material ID
    QString m_Name;         // Material Name
    double m_Density;       // Material Density
    QString m_MatCard;      // Composition of Material

public:
    //void SetMatID(Standard_Integer theID);
    int GetMatID(){return m_ID;};

    //void SetMatName(TCollection_AsciiString theName);
    QString GetMatName(){return m_Name;};
    QString GetMatCard(){return m_MatCard;};

    //void SetMatDensity(Standard_Real theDensity);
    double GetMatDensity(){return m_Density;};
    void SetMaterial(int theID, QString theName, double theDensity, QString theMatCard);
//qiu add new function
    void SetMatName(QString theName) {m_Name = theName;}
};

#endif // MATERIAL_HXX
