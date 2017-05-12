#ifndef MCCADGDMLWRITER_HXX
#define MCCADGDMLWRITER_HXX

#include "IMcCadWriter.hxx"

#include "TopoDS_Shape.hxx"

#include <QtXml/QDomComment>
#include <QFile>
#include <QTextStream>
#include <QString>

#include <vector>

//using namespace std;
class McCadGDMLWriter : public IMcCadWriter
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
Standard_EXPORT    McCadGDMLWriter();
Standard_EXPORT    ~McCadGDMLWriter();

    //main function to print the gdml file
Standard_EXPORT    virtual void    PrintFile() ;

    //##### Print the <define> Element
    //Print <define> element, means print the polyhedron for visualization purpose
Standard_EXPORT    virtual void    PrintDefine(QDomElement & ParentElement);
    //print one polyhedron: a close shell form by facets
Standard_EXPORT    virtual void    PrintPolyhedron(QDomElement & ParentElement, const TopoDS_Shape * aShape, const QString & aName);
    //method to print the polyhedron for a McCadSolid. McCadSolid is not inherit from TopoDS_Shape,
    //we should therefore implemenet independently
Standard_EXPORT    virtual void    PrintPolyhedron(QDomElement & ParentElement, McCadSolid * pSolid, const QString & aName);
    //meshing the shape with
Standard_EXPORT    void    MeshShape(const TopoDS_Shape theShape, double &theDeflection);


    //##### Print the <materials> Element

Standard_EXPORT    virtual void    PrintMaterial(QDomElement & ParentElement);

    //##### Print the <solids> Element
    //print the <solids > element, where the solid is built by HalfSpaceSolid and HalfSpaceBoolean
Standard_EXPORT    virtual void    PrintSolids (QDomElement & ParentElement);
    //Print the whole world box
Standard_EXPORT    virtual void    PrintWorldBox (QDomElement & ParentElement);
    //print a HalfSpaceSolid
Standard_EXPORT    virtual void    PrintHalfSpaceSolid(QDomElement & ParentElement,  McCadConvexSolid * pConvexSolid, const QString & aName);
//Standard_EXPORT    virtual void    PrintHalfSpaceSolid(QDomElement & ParentElement,  McCadVoidCell * pVoid, const QString & aName);
    //print the surface list
Standard_EXPORT    virtual void    PrintSurfaces(QDomElement & ParentElement,  McCadConvexSolid * pConvexSolid);
//Standard_EXPORT    virtual void    PrintSurfaces(QDomElement & ParentElement,  McCadVoidCell * pVoid);
    //find the surface according to the surface ID
Standard_EXPORT    IGeomFace *     findSurface(const int & aIdx);
    //print a surface
Standard_EXPORT    virtual void    PrintASurface(QDomElement & ParentElement, IGeomFace * pFace, const int & theSense);
    //print the boundary box
Standard_EXPORT    virtual void    PrintBoundaryBox(QDomElement & ParentElement,  TopoDS_Solid * pConvexSolid);
    //print other values (volume size, surface area, polyhedron)
Standard_EXPORT    virtual void    PrintVolumeArea (QDomElement & ParentElement,  TopoDS_Solid * pConvexSolid, const QString & aName);
Standard_EXPORT    virtual void    PrintPolyhedronRef (QDomElement & ParentElement, const QString & aName);
    //we create the boundary box but not using the one from McCadGeomData because we might goes without void generation.
Standard_EXPORT    virtual McCadVoidCell * getWholeBntBox();

    //Print the HalfSpaceBoolean
    //if isPrintPoly == true, then print the polyhedron
Standard_EXPORT    virtual void    PrintHalfSpaceBoolean(QDomElement & ParentElement, const QString SolidName,
                                          const int & SubSolidCnt, const bool isPrintPoly = true);


    //#####print the <structure> element
Standard_EXPORT    virtual void    PrintStructure(QDomElement & ParentElement);
    //print a volume, noted that the name of the volume is set to "LV_" + SolidName
Standard_EXPORT    virtual void    PrintVolume(QDomElement & ParentElement, const QString MaterialName, const QString SolidName);

    //print a physical volume, noted that the name of the volume is set to "LV_" + SolidName
Standard_EXPORT    virtual void    PrintPhysicalVolume(QDomElement & ParentElement, const QString SolidName);

    //#####print the <setup> element
Standard_EXPORT    virtual void    PrintSetup (QDomElement & ParentElement);

    //to check repetition of the name, if repeated then rename it with a number
    //Because the GDML use the name as references, therefore each name should be
    //unique
Standard_EXPORT    virtual void    CheckSolidNames();
Standard_EXPORT    virtual void    CheckMaterialNames();
protected:
Standard_EXPORT    virtual void    CheckNames(vector<QString> aNameList);
Standard_EXPORT    QString         RoundDouble(const double & aDouble);
// for checking repeated surface in one solid
Standard_EXPORT    void            checkRepeatFaces(McCadConvexSolid *& pSolid, vector<Standard_Integer> & IntSurfList, vector<Standard_Integer> UniSurfList);

protected:
    QDomDocument m_Doc;


};

#endif // MCCADGDMLWRITER_HXX
