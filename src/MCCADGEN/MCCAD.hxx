// Copyright (C) 2014-2015  KIT-INR/NK
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//


#ifndef _MCCAD_HXX_
#define _MCCAD_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MCCAD)
#include CORBA_CLIENT_HEADER(MEDCouplingCorbaServant)
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include <SALOME_Component_i.hxx>
#include <TopoDS.hxx>
#include <TopTools_HSequenceOfShape.hxx>

//to defined EXPORT for these class
//#ifdef WIN32
//#  ifdef CONTAINER_EXPORT
//#    define MCCADGEN_EXPORT CONTAINER_EXPORT
//#  else 
//#    define MCCADGEN_EXPORT Standard_EXPORT
//#  endif 
//#endif


#include <map>
#include <vector>
using namespace std;

class BaseObj;
class Part;
class Material;
class Group;
class Component;


//############### Base object ####################


class Standard_EXPORT BaseObj: public virtual POA_MCCAD_ORB::BaseObj
{
protected:

    int             myID; // an unique integer for identification, assign when part instance is generated
    char *          myName;  //Name for displaying in the GUI
    char *          myRemark; //description for this part
    TopoDS_Shape    myShape;  //OCC instance for this part
    MCCAD_ORB::MColor myColor;

public:
    virtual char*           getName()           {return CORBA::string_dup(myName);}
    virtual void            setName(const char* Name) ;
    virtual CORBA::Long     getID()             {return myID ; }
    virtual char*           getRemark()    {return CORBA::string_dup(myRemark);}
    virtual void            setRemark(const char* aRemark) ;
    virtual TopoDS_Shape    getShape();
    virtual CORBA::LongLong getShapePointer();          //in the same host
    virtual SALOMEDS::TMPFile*  getShapeStream();
    virtual void            getColor(MCCAD_ORB::MColor  aColor);
//    virtual void            setColor(const MCCAD_ORB::MColor aColor);

};


//############### Part ####################

class Standard_EXPORT Part: public virtual BaseObj, public virtual POA_MCCAD_ORB::Part
{

public:

    Part (const char * Name,const CORBA::Long id);
    virtual         ~Part();

//    char*           getName()           {return CORBA::string_dup(myName);}
//    void            setName(const char* Name) ;
//    CORBA::Long     getID()             {return myID ; }
//    char*           getDescription()    {return CORBA::string_dup(myDescription);}
//    void            setRemark(const char* aDescription) ;
    void            setShape(TopoDS_Shape & aShape) ;
    void            setShapeStream(const SALOMEDS::TMPFile& aShapeStream) ;
    virtual TopoDS_Shape    getShape()          {return myShape;}
    void            recoverShape();
    void            setIsDecomposed (CORBA::Boolean  isDecomposed) ;
    CORBA::Boolean  getIsDecomposed ();


//    CORBA::LongLong getShapePointer()          {return (CORBA::LongLong)(&myShape) ;} //in the same host
//    SALOMEDS::TMPFile*  getShapeStream();
    void            setMesh(SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr aMesh)  ;
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr getMesh() ;
    void            clearMesh();

    void            setImportance(const ::MCCAD_ORB::FixArray aIMP) ;
    MCCAD_ORB::FixArray_slice* getImportance() ;
    char*           getAdditive() ;
    void            setAdditive(const char* aAdditive) ;

    CORBA::Double   getVolume();
    CORBA::Double   getMeshVolume();

    CORBA::Boolean  hasBackupShape() { return myShapeBackup.IsNull() ? false : true;}
    CORBA::Boolean  hasMesh() {return myMesh->_is_nil()?  false : true; }
    //    SALOMEDS::TMPFile *  getShapeStream();
    void            setColor(const MCCAD_ORB::MColor aColor);
//    void            getColor(MCCAD_ORB::MColor aColor);
    CORBA::Boolean  isExplodible();


private:

//    int             myID; // an unique integer for identification, assign when part instance is generated
//    char *          myName;  //Name for displaying in the GUI
//    char *          myDescription; //description for this part
//    TopoDS_Shape    myShape;  //OCC instance for this part
    MCCAD_ORB::FixArray  myImportance; //importances N,P,S: -1:empty, no importance
    char *          myAdditive;  //additive card
    CORBA::Boolean  myIsDecomposed;
    TopoDS_Shape    myShapeBackup;  //Backup of last modification
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr myMesh;   // MED unstrcutred mesh for MCNP6
};


//############### Material ####################
//material property, not yet implement

class  Standard_EXPORT Material: public virtual POA_MCCAD_ORB::Material
{
public:

    Material (const char * Name,const CORBA::Long id);
    virtual         ~Material();

    char*           getName()           {return CORBA::string_dup(myName);}
    void            setName(const char* Name) ;
    CORBA::Long     getID()             {return myID ; }
    void            setID(CORBA::Long id) ;

    CORBA::Double     getDensity()             {return myDensity ; }
    void            setDensity(CORBA::Double aDensity) ;

    char*           getMatCard()           {return CORBA::string_dup(myMatCard);}
    void            setMatCard(const char* MatCard) ;

    void            getColor(MCCAD_ORB::MColor  aColor);
    void            setColor(const MCCAD_ORB::MColor aColor);


private:

    int             myID; // an unique integer for identification. difference is that this ID is given by User, each time should check
    char *          myName;  //name of the material
    double          myDensity; //atom or mass density
    char *          myMatCard; //MCNP material card or TRIPOLI card
    MCCAD_ORB::MColor myColor;

};



//############### Group ####################
//Group of parts which have ethe same material
class Standard_EXPORT Group: public virtual BaseObj, public virtual POA_MCCAD_ORB::Group
{
public:

    Group(const char * Name,  const CORBA::Long id);
    virtual         ~Group();

//    char*           getName() ;
//    void            setName(const char* Name) ;
//    CORBA::Long     getID()              {return myID; }
    MCCAD_ORB::PartList*  getPartList()    {return myList; }
    CORBA::Boolean  setPartList(const MCCAD_ORB::PartList& aPartList) ;
    CORBA::Long     getNumParts() {return myList->length();}
    MCCAD_ORB::Material_ptr     getMaterial();
    CORBA::Long     getMaterialID();
    void            setMaterial(MCCAD_ORB::Material_ptr aMaterial);
    MCCAD_ORB::Part_ptr        getPart(CORBA::Long PartID) ;
    CORBA::Boolean  appendPart(MCCAD_ORB::Part_ptr aPart) ;
    CORBA::Boolean  deletePart(CORBA::Long PartID) ;
    CORBA::Boolean  insertPart(MCCAD_ORB::Part_ptr aPart, CORBA::Long PartID) ;
    CORBA::Boolean  clearParts() ;
    void            appendPartInGroup(MCCAD_ORB::Group_ptr aGroup);
    virtual TopoDS_Shape    getShape();
//    CORBA::LongLong getShapePointer();
//    SALOMEDS::TMPFile *  getShapeStream();
    void            setColor(const MCCAD_ORB::MColor aColor);
//    void            getColor(MCCAD_ORB::MColor aColor);
    CORBA::Boolean  hasMaterial() {return myMaterial->_is_nil() ? false : true ;}



private:

//    CORBA::Long     myID;
//    char *          myName;
//    TopoDS_Shape    myShape;
    MCCAD_ORB::PartList *   myList; //PartList is a CORBA sequence of Part type
    MCCAD_ORB::Material_ptr myMaterial;// material


};



//############### Component ####################
//Component of Groups which have the same material
//Naming to Component (McCad Component) because the
//word Component clashed with CORBA key word
class Standard_EXPORT Component: public virtual BaseObj, public virtual POA_MCCAD_ORB::Component
{
public:

    Component(const char * Name,  const CORBA::Long id);
    virtual         ~Component();

//    char*           getName() ;
//    void            setName(const char* Name) ;
//    CORBA::Long     getID()              {return myID; }
    MCCAD_ORB::GroupList*  getGroupList()    {return myList; }
    CORBA::Boolean  setGroupList(const MCCAD_ORB::GroupList& aGroupList) ;
//    MCCAD_ORB::Group_ptr        generateGroup(const char* Name);
    MCCAD_ORB::Group_ptr        getGroup(CORBA::Long GroupID) ;
    CORBA::Long     getNumGroups()  {return myList->length();}
    CORBA::Long     getNumParts() ;
    CORBA::Boolean  appendGroup(MCCAD_ORB::Group_ptr aGroup) ;
    CORBA::Boolean  deleteGroup(CORBA::Long GroupID) ;
    CORBA::Boolean  insertGroup(MCCAD_ORB::Group_ptr aGroup, CORBA::Long GroupID) ;
    CORBA::Boolean  clearGroups() ;
    void            appendGroupInComponent(MCCAD_ORB::Component_ptr aComponent);
    virtual TopoDS_Shape    getShape();
//    CORBA::LongLong getShapePointer();
//    SALOMEDS::TMPFile *  getShapeStream();
    void            setColor(const MCCAD_ORB::MColor aColor);
//    void            getColor(MCCAD_ORB::MColor aColor);
    void            setEnvelop(TopoDS_Shape & aShape){myEnvelop = aShape;}
    void            setEnvelopStream(const SALOMEDS::TMPFile& aShapeStream);
    TopoDS_Shape    getEnvelop(){return myEnvelop;}
    SALOMEDS::TMPFile*  getEnvelopStream();
    CORBA::LongLong getEnvelopPointer();
    CORBA::Boolean  hasEnvelop() {return myEnvelop.IsNull() ? false : true ;}
    void            clearEnvelop() {myEnvelop = TopoDS_Shape();}


private:

//    CORBA::Long  myID;
//    char * myName;
//    TopoDS_Shape    myShape;
    MCCAD_ORB::GroupList * myList; //GroupList is a CORBA sequence of Group type
    TopoDS_Shape    myEnvelop;  //Envelop of this part, simplest case is the boundary box, for MCNP6


};


//############### Engine ####################

class Standard_EXPORT MCCAD: public  virtual POA_MCCAD_ORB::MCCAD_Gen,
        public Engines_Component_i
{

public:
    MCCAD(CORBA::ORB_ptr orb,
               PortableServer::POA_ptr poa,
               PortableServer::ObjectId * contId,
               const char *instanceName,
               const char *interfaceName);
    MCCAD();
    virtual ~MCCAD();
    CORBA::Long genId ();

    CORBA::Boolean             setData(CORBA::Long studyID, const MCCAD_ORB::ComponentList& theData) ;
    MCCAD_ORB::ComponentList*  getData(CORBA::Long studyID) ;
    MCCAD_ORB::MaterialList*   getMaterialList(CORBA::Long studyID) ;

    MCCAD_ORB::Component_ptr   generateComponent(const char* Name)                                     throw (SALOME::SALOME_Exception) ;
    MCCAD_ORB::Group_ptr       generateGroup(const char* Name)                                     throw (SALOME::SALOME_Exception) ;
    MCCAD_ORB::Part_ptr        generatePart(const char* Name )                                      throw (SALOME::SALOME_Exception);
    MCCAD_ORB::Material_ptr    generateMaterial (const char* Name , const CORBA::Long MaterialID , const CORBA::Long studyID)      throw (SALOME::SALOME_Exception) ;
    MCCAD_ORB::Component_ptr   getComponent(CORBA::Long studyID, CORBA::Long ComponentID) ;
    MCCAD_ORB::Group_ptr       getGroup(CORBA::Long studyID, CORBA::Long GroupID) ;
    MCCAD_ORB::Part_ptr        getPart(CORBA::Long studyID, CORBA::Long PartID) ;
    MCCAD_ORB::Material_ptr    getMaterial(CORBA::Long studyID, CORBA::Long MaterialID) ;

    MCCAD_ORB::Part_ptr        deepCopyPart (MCCAD_ORB::Part_ptr aPart)                       throw (SALOME::SALOME_Exception);
    MCCAD_ORB::Group_ptr       deepCopyGroup (MCCAD_ORB::Group_ptr aGroup)                       throw (SALOME::SALOME_Exception);
    CORBA::Boolean             appendComponent(CORBA::Long studyID,
                                                MCCAD_ORB::Component_ptr aComponent) ;//                  throw (SALOME::SALOME_Exception);
    CORBA::Boolean             insertComponent(CORBA::Long studyID,
                                                MCCAD_ORB::Component_ptr aComponent, CORBA::Long ComponentID);//      throw (SALOME::SALOME_Exception);
    CORBA::Boolean             deleteComponent(CORBA::Long studyID,
                                                CORBA::Long ComponentID);//                                    throw (SALOME::SALOME_Exception) ;
    CORBA::Boolean             clearComponent(CORBA::Long studyID) ;
    CORBA::Boolean             appendMaterial(CORBA::Long studyID,
                                                MCCAD_ORB::Material_ptr aMaterial) ;//                  throw (SALOME::SALOME_Exception);
    CORBA::Boolean             deleteMaterial(CORBA::Long studyID,
                                                CORBA::Long MaterialID);//

    void                       unittest();
    TopoDS_Shape               getShape(CORBA::Long studyID);
    CORBA::LongLong            getShapePointer(CORBA::Long studyID);
    SALOMEDS::TMPFile *        getShapeStream(CORBA::Long studyID);
    MCCAD_ORB::PartList*       ImportGeom(const char* FileName/*,CORBA::Boolean ifExplode*/) throw (SALOME::SALOME_Exception);
    MCCAD_ORB::PartList*       explodePart(MCCAD_ORB::Part_ptr aPart) throw (SALOME::SALOME_Exception);
    MCCAD_ORB::Part_ptr        fuseParts(const MCCAD_ORB::PartList& aPartList, const char* aName) throw (SALOME::SALOME_Exception);

    CORBA::Boolean             decomposePart(MCCAD_ORB::Part_ptr aPart);
    CORBA::Boolean             decomposeEnvelop(MCCAD_ORB::Component_ptr aComponent);
    CORBA::Boolean              decomposeShape (const TopoDS_Shape & aShape, Handle_TopTools_HSequenceOfShape & OutputSolids, Handle_TopTools_HSequenceOfShape & ErrorSolids);
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr importMED(const char* MEDFileName)                                  throw (SALOME::SALOME_Exception);
    void                       export2MED(SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr aMesh, const char* FileName)    throw (SALOME::SALOME_Exception);
    void                       exportAllMesh2Abaqus(CORBA::Long studyID, const char* FileName)   throw (SALOME::SALOME_Exception);
//    CORBA::Boolean             generateTetMesh(MCCAD_ORB::Part_ptr aPart, CORBA::Double aDeflection, CORBA::Double aCoefficient, CORBA::Double aMeshQuality) throw (SALOME::SALOME_Exception);
//    CORBA::Boolean             generateTetgenMesh(MCCAD_ORB::Part_ptr aPart, CORBA::Double aDeflection, CORBA::Double aCoefficient, CORBA::Double aMeshQuality) throw (SALOME::SALOME_Exception);
//    CORBA::Boolean             generateNetgenMesh(MCCAD_ORB::Part_ptr aPart, CORBA::Double aDeflection, CORBA::Double aCoefficient) throw (SALOME::SALOME_Exception);
    CORBA::Boolean             generateTetMesh(MCCAD_ORB::Part_ptr aPart, CORBA::Double aDeflection, CORBA::Double aVolThreshold, CORBA::Double aMeshQuality) throw (SALOME::SALOME_Exception);
    CORBA::Boolean             generateTetgenMesh(MCCAD_ORB::Part_ptr aPart, CORBA::Double aDeflection, CORBA::Double aVolThreshold, CORBA::Double aMeshQuality) throw (SALOME::SALOME_Exception);
    CORBA::Boolean             generateNetgenMesh(MCCAD_ORB::Part_ptr aPart, CORBA::Double aDeflection, CORBA::Double aVolThreshold) throw (SALOME::SALOME_Exception);
    void                       MeshShape(const TopoDS_Shape aShape, double & theDeflection);
    void                       TessellateShape(const TopoDS_Shape theShape, vector <double> &PointList, vector <int> &FacetList, double & theDeflection);


    void exportGeom(const MCCAD_ORB::BaseObjList & aBaseObjList, const char* FileName) ;




private:
    std::map<long, MCCAD_ORB::ComponentList*> myData;
    std::map<long, MCCAD_ORB::MaterialList*> myMaterials;

    CORBA::Long myMaxId;
    TopoDS_Shape    myShape;
};



extern "C"
Standard_EXPORT PortableServer::ObjectId * MCCADEngine_factory(CORBA::ORB_ptr orb,
                                                    PortableServer::POA_ptr poa,
                                                    PortableServer::ObjectId * contId,
                                                    const char *instanceName,
                                                    const char *interfaceName);

#endif
