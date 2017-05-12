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



#include "MCCAD.hxx"
#include "MCMESHTRAN_AbaqusMeshWriter.hxx"

//MEDCoupling
#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingFieldTemplateClient.hxx"
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCouplingUMeshServant.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"
#include "MEDCouplingFieldTemplateServant.hxx"
#include "MEDCouplingAutoRefCountObjectPtr.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingRemapper.hxx"
#include "MEDLoader.hxx"
#include "MEDFileMesh.hxx"
#include "MEDFileField.hxx"

//OCC inlucde
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Solid.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <StlAPI_Writer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
//#include <Handle_TopTools_HSequenceOfShape.hxx> //no need to include this
#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Vec.hxx>

//MCCAD includes
//#include <McCadConvertTools_Convertor.hxx>
#include "../MCCAD/McCadDecompose/McCadDecompose.hxx"


//SALOME
#include "Utils_CorbaException.hxx"
#include "utilities.h"


#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <math.h>
#include <string>
#include <vector>
#include <algorithm>
#include <QFile>
#include <QFileInfo>

//#include <thread>

using namespace std;
//TETGEN
#ifdef WITH_TETGEN
    #include "../TETGEN/tetgen.h"
#endif
//Netgen
namespace nglib {
#include "nglib.h"
}
/*!
 * \brief convert the Shape stream to shape
 * \param aShapeStream  the octet stream with OCC shape
 * \param theShape OCC shape
 */
TopoDS_Shape Stream2Shape(const SALOMEDS::TMPFile& aShapeStream)
{
    //the shape stream is a 8bit octet(similar with char) array,
    //convert the stream to char and then read it
    TopoDS_Shape tmpShape;
    char* buf;
    buf = (char*) &aShapeStream[0];
    std::istringstream streamBrep(buf);
    BRep_Builder aBuilder;
    BRepTools::Read(tmpShape, streamBrep, aBuilder);
    return tmpShape;
}



/*!
 * \brief convert the Shape to octet stream
 * \param aShape the shape to be convert
 * \return  return octet stream contains shape info
 */
SALOMEDS::TMPFile* Shape2Stream(const TopoDS_Shape& aShape)
{
    std::ostringstream streamShape;
    //Write TopoDS_Shape in ASCII format to the stream
    BRepTools::Write(aShape, streamShape);
    //Returns the number of bytes that have been stored in the stream's buffer.
    int size = streamShape.str().size();
    //Allocate octect buffer of required size
    CORBA::Octet* OctetBuf = SALOMEDS::TMPFile::allocbuf(size);
    //Copy ostrstream content to the octect buffer
    memcpy(OctetBuf, streamShape.str().c_str(), size);
    //Create and return TMPFile
    SALOMEDS::TMPFile_var SeqFile = new SALOMEDS::TMPFile(size,size,OctetBuf,1);
    return SeqFile._retn();
}

MCCAD::MCCAD(CORBA::ORB_ptr orb,
                       PortableServer::POA_ptr poa,
                       PortableServer::ObjectId * contId,
                       const char *instanceName,
                       const char *interfaceName) :
    Engines_Component_i(orb, poa, contId, instanceName, interfaceName)
{
    MESSAGE("activate object");
    _thisObj = this ;
    _id = _poa->activate_object(_thisObj);
    myMaxId = 0;
    myShape = TopoDS_Shape();

}

MCCAD::~MCCAD()
{
    //delete all the pointers. neccessary?
    for (std::map<long, MCCAD_ORB::ComponentList*>::iterator it=myData.begin(); it!=myData.end(); ++it)
        delete it->second;
}

/*!
 * \brief generate a new id
 * \return  new id
 */
CORBA::Long MCCAD::genId()
{
    return ++myMaxId ;
}

/*!
 * \brief set data to the specific study
 * \param studyID study ID
 * \param theData a List of Component
 * \return \c true if success
 */
CORBA::Boolean MCCAD::setData ( CORBA::Long studyID, const MCCAD_ORB::ComponentList & theData)
{
    const int n = theData.length();
    if (n == 0) return false;
    MCCAD_ORB::ComponentList_var aComponentList = new MCCAD_ORB::ComponentList();
    aComponentList->length(n);

    for (int i=0; i<n; i++)
        aComponentList[i] = theData[i];  //deep copy

    myData[studyID] = aComponentList._retn();
    return true;
}

/*!
 * \brief get the assemblies according to the \a studyID
 * \param studyID identifier of the study
 * \param outData output component list
 * \return the component list
 */
MCCAD_ORB::ComponentList *  MCCAD::getData (CORBA::Long studyID)
{
    if ( myData.find( studyID ) != myData.end() ) //find the list according to the list
    {
        return myData[ studyID ];
    }
    myData[ studyID ] = new MCCAD_ORB::ComponentList;  //if the list was not created, create it
    return myData[ studyID ];
}

/*!
 * \brief MCCAD::getMaterialList
 * get a list of material in this study
 * \param studyID
 * \return a MaterialList of this study
 */
MCCAD_ORB::MaterialList*   MCCAD::getMaterialList(CORBA::Long studyID)
{
    if ( myMaterials.find( studyID ) != myMaterials.end() ) //find the list according to the list
    {
        return myMaterials[ studyID ];
    }
    myMaterials[ studyID ] = new MCCAD_ORB::MaterialList;  //if the list was not created, create it
    return myMaterials[ studyID ];
}


/*!
 * \brief generate a new component
 * \param Name
 * \param ComponentId
 * \return the new component
 */
MCCAD_ORB::Component_ptr MCCAD::generateComponent(const char* Name) throw (SALOME::SALOME_Exception)
{
    if (Name[0] == 0)
        THROW_SALOME_CORBA_EXCEPTION("Invalid Component name", SALOME::BAD_PARAM);
    MESSAGE("Generate Component: " <<Name);
    Component * aComponent = new Component(Name, genId());
    return aComponent->_this();
}

MCCAD_ORB::Group_ptr MCCAD::generateGroup(const char* Name) throw (SALOME::SALOME_Exception)
{
    if (Name[0] == 0)
        THROW_SALOME_CORBA_EXCEPTION("Invalid Group name", SALOME::BAD_PARAM);
    MESSAGE("Generate Group: " <<Name);
    Group * aGroup = new Group(Name, genId());
    return aGroup->_this();
}

/*!
 * \brief generate a new part
 * \param Name part name
 * \return
 */
MCCAD_ORB::Part_ptr MCCAD::generatePart(const char* Name) throw (SALOME::SALOME_Exception)
{
    if (Name[0] == 0 )
        THROW_SALOME_CORBA_EXCEPTION("Invalid part name ", SALOME::BAD_PARAM);
    MESSAGE("Generate Part: " <<Name);
    Part * aPart = new Part(Name, genId());
    return aPart->_this();
}

/*!
 * \brief MCCAD::generateMaterial
 *  generate a material instance and return the reference
 * \param Name material name
 * \param MatID material id
 * \return a new material with reference
 */
MCCAD_ORB::Material_ptr    MCCAD::generateMaterial (const char* Name , const CORBA::Long MaterialID, const CORBA::Long studyID )
 throw (SALOME::SALOME_Exception)
{
    if (Name[0] == 0 )
        THROW_SALOME_CORBA_EXCEPTION("Invalid part name ", SALOME::BAD_PARAM);
    MESSAGE("Generate Material: " <<Name);
    //first check the material list
    MCCAD_ORB::MaterialList * aMaterialList = getMaterialList(studyID);
    int n = aMaterialList->length();
    for (int i=0; i< n; i++) {
        MCCAD_ORB::Material_var aMaterial = (*aMaterialList) [i];
        if (MaterialID == aMaterial->getID())  {
            THROW_SALOME_CORBA_EXCEPTION("Already have a material with the same id! ", SALOME::BAD_PARAM);
            return MCCAD_ORB::Material::_nil();
        }
    }

    Material * aMaterial = new Material(Name, MaterialID);
    return aMaterial->_this();
}

/*!
 * \brief get a component from the study according to the \a ComponentID
 * \param studyID
 * \param ComponentID
 * \return a Component, duplicated, should release outside
 */
MCCAD_ORB::Component_ptr   MCCAD::getComponent(CORBA::Long studyID, CORBA::Long ComponentID)
{
    if ( myData.find( studyID ) != myData.end() ) //find the list according to the list
    {
        MCCAD_ORB::ComponentList * aComponentList = myData[studyID];
        const int n = aComponentList->length();
        for (int i = 0; i < n; i++)
        {
            MCCAD_ORB::Component_var  aComponent = (*aComponentList)[i];
            if (ComponentID == aComponent->getID())
                return aComponent._retn(); //if find the Component, return the pointer
        }
        return MCCAD_ORB::Component::_nil();  //else return NULL
    }
    return MCCAD_ORB::Component::_nil();
}

/*!
 * \brief get a Group from the component list of the study
 * \param studyID
 * \param ComponentId
 * \param GroupID
 * \return the group, duplicated, should release outside
 */
MCCAD_ORB::Group_ptr MCCAD::getGroup(CORBA::Long studyID, CORBA::Long GroupID)
{
    MCCAD_ORB::ComponentList* aComponentList = getData(studyID)    ;
    int n = aComponentList->length();
    for (int i=0; i< n; i++) {
        MCCAD_ORB::Component_var aComponent = (*aComponentList) [i];
        MCCAD_ORB::Group_var  aGroup = aComponent->getGroup(GroupID);
        if (!aGroup->_is_nil()) return aGroup._retn(); //if the group is not nil return it.
    }
    return MCCAD_ORB::Group::_nil();
}
/*!
 * \brief get the part from the group
 * \param studyID
 * \param ComponentID
 * \param GroupID
 * \param PartID
 * \return the part, duplicated, should release outside
 */
MCCAD_ORB::Part_ptr MCCAD::getPart(CORBA::Long studyID, CORBA::Long PartID)
{
    MCCAD_ORB::ComponentList* aComponentList = getData(studyID)    ;
    for (int i=0; i< aComponentList->length(); i++) {
        MCCAD_ORB::Component_var aComponent = (*aComponentList) [i];
        MCCAD_ORB::GroupList* aGroupList = aComponent->getGroupList();
        for (int j=0; j<aGroupList->length(); j++) {
            MCCAD_ORB::Group_var aGroup = (*aGroupList) [j];
            MCCAD_ORB::Part_var aPart = aGroup->getPart(PartID);
            if (!aPart->_is_nil()) return aPart._retn();
        }
    }
    return MCCAD_ORB::Part::_nil();
}


/*!
 * \brief MCCAD::getMaterial
 *  get the material in this study
 * \param studyID
 * \param MaterialID
 * \return a material, duplicated, should release outside
 */
MCCAD_ORB::Material_ptr   MCCAD::getMaterial(CORBA::Long studyID, CORBA::Long MaterialID)
{
    if ( myMaterials.find( studyID ) != myMaterials.end() ) //find the list according to the list
    {
        MCCAD_ORB::MaterialList * aMaterialList = myMaterials[studyID];
        const int n = aMaterialList->length();
        for (int i = 0; i < n; i++)
        {
            MCCAD_ORB::Material_var  aMaterial = (*aMaterialList)[i];
            if (MaterialID == aMaterial->getID())
                return aMaterial._retn(); //if find the Material, return the pointer
        }
        return MCCAD_ORB::Material::_nil();  //else return NULL
    }
    return MCCAD_ORB::Material::_nil();
}


/*!
 * \brief deep copy of a part
 * \param aPart part to be copy
 * \return a new part just different in ID
 */
MCCAD_ORB::Part_ptr MCCAD::deepCopyPart (MCCAD_ORB::Part_ptr aPart)
throw (SALOME::SALOME_Exception)
{
    if (aPart->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Copy a nil part", SALOME::BAD_PARAM);
    MESSAGE("Copy Part: " <<aPart->getName());
    Part * aNewPart = new Part(aPart->getName(), genId());
    aNewPart->setShapeStream(*aPart->getShapeStream());  //getShapeStream() will return a pointer of TMPFile
    aNewPart->setRemark(aPart->getRemark());
    aNewPart->setAdditive(aPart->getAdditive());  //write descriptions
    MCCAD_ORB::MColor aMColor;
    aPart->getColor(aMColor);
    aNewPart->setColor(aMColor);
    MCCAD_ORB::FixArray_var aIMP = aPart->getImportance();
    aNewPart->setImportance(aIMP);
//    if (aPart->hasEnvelop()) aNewPart->setEnvelopStream(*aPart->getEnvelopStream());
    if (aPart->hasMesh()) aNewPart->setMesh(aPart->getMesh());
    return aNewPart->_this();
}

/*!
 * \brief MCCAD::deepCopyGroup
 *  deep copy a group
 * due to the MCCADGUI_DataObject mechanism, the object with the same id is consider as the same object
 * deep copy it can avoid this problem.
 * \param aGroup the group to be copy
 * \return a new group
 */
MCCAD_ORB::Group_ptr MCCAD::deepCopyGroup (MCCAD_ORB::Group_ptr aGroup)                       throw (SALOME::SALOME_Exception)
{
    if (aGroup->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Copy a nil group!", SALOME::BAD_PARAM);
    MESSAGE("Copy Group: " <<aGroup->getName());
    Group * aNewGroup = new Group(aGroup->getName(), genId());
    if (aGroup->hasMaterial())
        aNewGroup->setMaterial(aGroup->getMaterial());
    if (aGroup->getPartList()->length() > 0) {
        for (int i=0; i<aGroup->getPartList()->length(); i++)
            aNewGroup->appendPart(deepCopyPart((*aGroup->getPartList())[i]));
    }
    MCCAD_ORB::MColor aMColor ;
    aGroup->getColor(aMColor);
    aNewGroup->setColor(aMColor);
    return aNewGroup->_this();
}


/*!
 * \brief append new Component into the Component List
 * \param aComponent
 * \return
 */
CORBA::Boolean MCCAD::appendComponent(CORBA::Long studyID, MCCAD_ORB::Component_ptr aComponent)// throw (SALOME::SALOME_Exception)
{
    if (aComponent->_is_nil())         return false;
    MESSAGE("Append a Component: " << aComponent->getName());
    if (myData.find(studyID) == myData.end())  //if no Componentlist in this study, create one
        myData[studyID] = new MCCAD_ORB::ComponentList;
    const int n = myData[studyID]->length() + 1;
    myData[studyID]->length(n);

    //duplicate because the aComponent might be destroy outside
    MCCAD_ORB::Component_var aComponent_var = MCCAD_ORB::Component::_duplicate(aComponent);
    (*myData[studyID])[n-1] = aComponent_var._retn();
    return true;
}

/*!
 * \brief insert a Component into a study
 * \param studyID the study ID
 * \param aComponent the Component to be insert
 * \param ComponentID insert before the Component with ID=ComponentID
 * \return \c true if succeed
 */
CORBA::Boolean  MCCAD::insertComponent(CORBA::Long studyID, MCCAD_ORB::Component_ptr aComponent, CORBA::Long ComponentID)// throw (SALOME::SALOME_Exception)
{
    if (aComponent->_is_nil()) return false;
    MESSAGE("Insert a Component: " << aComponent->getName());
    //if no data in this study, create a Component list and append the Component into the list
    if (myData.find(studyID) == myData.end())
    {
        myData[studyID] = new MCCAD_ORB::ComponentList;
    }
    if (0 == myData[studyID]->length())
    {
        appendComponent(studyID, aComponent);
        return true;  //just append
    }
    //if can not find the Component, then append it
    if (getComponent(studyID, ComponentID)->_is_nil())
    {
        appendComponent(studyID, aComponent);
        return true; //just append
    }

    const int n = myData[studyID]->length();
    myData[studyID]->length(n+1) ;  //increase the length with one element
    int i;
    for (i=0; i<n; i++)  //search for Component
    {
        if (!CORBA::is_nil((*myData[studyID])[i]))
        {
            MCCAD_ORB::Component_var  aComponent_var = (*myData[studyID])[i];
            if (ComponentID == aComponent_var->getID())
            {
                for (int j=n; j>i; j--)
                {
                    (*myData[studyID])[j] = (*myData[studyID])[j-1]; //see the detail design documents
                }
                break; //important
            }
        }
    }
    if (i == n)// cannot find the part
    {
        myData[studyID]->length(n);//recover the length if not insert
        return false;
    }

    // duplicate because aComponent might be delete outside
    (*myData[studyID])[i] = MCCAD_ORB::Component::_duplicate(aComponent);  //copy the part to be insert into the list
    return true;
}

CORBA::Boolean  MCCAD::deleteComponent(CORBA::Long studyID, CORBA::Long ComponentID) //throw (SALOME::SALOME_Exception)
{
    //similar with deletePart()
    MCCAD_ORB::Component_var aTmpComponent = getComponent(studyID, ComponentID);
    if (aTmpComponent->_is_nil()) return false;
    MESSAGE("Delete  Component: " << aTmpComponent->getName());

    const int n = myData[studyID]->length();
    int i;
    for (i=0; i<n; i++)
    {
        if (!CORBA::is_nil((*myData[studyID])[i]))
        {
            MCCAD_ORB::Component_var  aComponent_var = (*myData[studyID])[i];
            if (ComponentID == aComponent_var->getID())
            {
                for (int j=i; j<n-1; j++)
                {
                    (*myData[studyID])[j] = (*myData[studyID])[j+1]; //see the detail design documents
                }
                break;//important
            }
        }
    }
    if (i == n)// cannot find the part
        return false;

    myData[studyID]->length(n-1);  //reduce the lenght therefore delete on item
    return true;

}


CORBA::Boolean  MCCAD::clearComponent(CORBA::Long studyID)
{
    myData[studyID]->length(0);
    return true;
}

/*!
 * \brief append new Material into the Material List
 * \param aMaterial
 * \return
 */
CORBA::Boolean MCCAD::appendMaterial(CORBA::Long studyID, MCCAD_ORB::Material_ptr aMaterial)// throw (SALOME::SALOME_Exception)
{
    if (aMaterial->_is_nil())         return false;
    MESSAGE("Append a Material: " << aMaterial->getName());
    if (myMaterials.find(studyID) == myMaterials.end())  //if no Materiallist in this study, create one
        myMaterials[studyID] = new MCCAD_ORB::MaterialList;
    const int n = myMaterials[studyID]->length() + 1;
    myMaterials[studyID]->length(n);

    //duplicate because the aMaterial might be destroy outside
    MCCAD_ORB::Material_var aMaterial_var = MCCAD_ORB::Material::_duplicate(aMaterial);
    (*myMaterials[studyID])[n-1] = aMaterial_var._retn();
    return true;
}


CORBA::Boolean  MCCAD::deleteMaterial(CORBA::Long studyID, CORBA::Long MaterialID) //throw (SALOME::SALOME_Exception)
{
    //similar with deletePart()
    MCCAD_ORB::Material_var aTmpMaterial = getMaterial(studyID, MaterialID);
    if (aTmpMaterial->_is_nil()) return false;
    MESSAGE("Delete  Material: " << aTmpMaterial->getName());

    const int n = myMaterials[studyID]->length();
    int i;
    for (i=0; i<n; i++)
    {
        if (!CORBA::is_nil((*myMaterials[studyID])[i]))
        {
            MCCAD_ORB::Material_var  aMaterial_var = (*myMaterials[studyID])[i];
            if (MaterialID == aMaterial_var->getID())
            {
                for (int j=i; j<n-1; j++)
                {
                    (*myMaterials[studyID])[j] = (*myMaterials[studyID])[j+1]; //see the detail design documents
                }
                break;//important
            }
        }
    }
    if (i == n)// cannot find the material
        return false;

    myMaterials[studyID]->length(n-1);  //reduce the lenght therefore delete on item
    return true;

}


/*!
 * \brief get all the shape in this component
 * \return TopoDS_shape, as compound
 */
TopoDS_Shape MCCAD::getShape(CORBA::Long studyID)
{
    TopoDS_Compound aCompound;
    BRep_Builder aBuilder;
    //make a compound and return
    aBuilder.MakeCompound (aCompound);

    const int n = myData[studyID]->length();
    for (int i = 0; i < n; i++)
    {
        if (!CORBA::is_nil((*myData[studyID])[i]))
        {
            TopoDS_Shape* aShape = (TopoDS_Shape*)( (*myData[studyID])[i]->getShapePointer());
            if (aShape != NULL)
            aBuilder.Add (aCompound, *aShape);
        }
    }
    return aCompound; //return compound as a shape
}

/*!
 * \brief return the shape as a memory pointer
 * \return
 */
CORBA::LongLong MCCAD::getShapePointer(CORBA::Long studyID)
{
    myShape = getShape(studyID);
    if (myShape.IsNull()) return NULL;
    return (CORBA::LongLong)(&myShape) ;
}

/*!
 * \brief wrap the shape of the part into a stream
 * \return the shape stream
 */
SALOMEDS::TMPFile*  MCCAD::getShapeStream(CORBA::Long studyID)
{
    TopoDS_Shape aShape = getShape(studyID);
    if (aShape.IsNull()) return NULL;//ok to return NULL?
    return  Shape2Stream(aShape);
}

/*!
 * \brief Import a OCC file, can be in brep, step and iges format
 * \param FileName import file name
 * \param ifExplode ifExplode is true , then explode the compound
 * \return a list of parts, should be released after using the list
 */
MCCAD_ORB::PartList*  MCCAD::ImportGeom(const char* FileName/*,CORBA::Boolean ifExplode*/) throw (SALOME::SALOME_Exception)
{
    if (*FileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("Invalid input file name", SALOME::BAD_PARAM);

    TopoDS_Shape aShape ;

//    QStringList aStringList = QString(FileName).split("/", QString::SkipEmptyParts);
//    QString aTmpName = aStringList[aStringList.size() -1].split(".").at(0); //get the file name without path, only works in Linux
//    QString aTmpExt =  aStringList[aStringList.size() -1].split(".").last();

    QFileInfo aGeomFile (FileName);
//    if (!aGeomFile.open(QIODevice::ReadOnly)) {
//        THROW_SALOME_CORBA_EXCEPTION("Cannot open file for reading", SALOME::BAD_PARAM);
//    }

    QString aTmpName = aGeomFile.fileName();
    QString aTmpExt =  aTmpName.split(".").last();

//    int len = strlen( FileName );


//    strcpy( exten, FileName + (len - 4));
    if(aTmpExt == "brep" ){
        BRep_Builder aBuilder;
        BRepTools::Read(aShape,FileName,aBuilder);
    }
    else if( aTmpExt == "iges" || aTmpExt == "igs"){
        IGESControl_Reader Reader;
        Standard_Integer status = Reader.ReadFile( FileName );
        if (status != IFSelect_RetDone )
            THROW_SALOME_CORBA_EXCEPTION("Cannot read this file as iges format", SALOME::BAD_PARAM);
        Reader.TransferRoots();
        aShape = Reader.OneShape();
    }
    else if( aTmpExt == "step" || aTmpExt == "stp"){

        STEPControl_Reader Reader;
        Standard_Integer status = Reader.ReadFile( FileName );
        if (status != IFSelect_RetDone )
            THROW_SALOME_CORBA_EXCEPTION("Cannot read this file as step format", SALOME::BAD_PARAM);
        Reader.TransferRoots();
        aShape = Reader.OneShape();
    }
    else
        THROW_SALOME_CORBA_EXCEPTION("Unrecognizable file with this extension", SALOME::BAD_PARAM);

//    //to see if explode
//    if (ifExplode)
//    {
//        int cnt= 0;
//        MCCAD_ORB::PartList * aPartList = new MCCAD_ORB::PartList;

//        TopExp_Explorer ex;
//        for (ex.Init(aShape, TopAbs_SOLID); ex.More(); ex.Next())
//        {
//            cnt ++;
//            TopoDS_Solid tmpSol = TopoDS::Solid(ex.Current());
//            QString aTmpStr = aTmpName + QString("-Part%1").arg(cnt);
//            Part * aPart = new Part(aTmpStr.toLatin1(), genId());
//            aPart->setShape(tmpSol);
////            aPart->setRemark(QString("Import from %1").arg(FileName).toLatin1());  //write descriptions
//            aPartList->length(cnt); //increase
//            (*aPartList)[cnt-1] = aPart->_this(); //append the part corba reference
//        }
//        if (cnt == 0)
//            THROW_SALOME_CORBA_EXCEPTION("No solid inside this file", SALOME::BAD_PARAM);

//        return aPartList;
//    }
//    else
//    {
        int cnt= 0;
        TopExp_Explorer ex;
        for (ex.Init(aShape, TopAbs_SOLID); ex.More(); ex.Next()) //check if any solid
            cnt ++;
        if (cnt == 0)
            THROW_SALOME_CORBA_EXCEPTION("No solid inside this file", SALOME::BAD_PARAM);
        QString aTmpStr = aTmpName.split(".").first() ;
        Part * aPart = new Part(aTmpStr.toLatin1(), genId());
        aPart->setShape(aShape);
//        aPart->setRemark(QString("Import from %1").arg(FileName).toLatin1());  //write descriptions
        MCCAD_ORB::PartList * aPartList = new MCCAD_ORB::PartList;
        aPartList->length(1); //increase
        (*aPartList)[0] = aPart->_this(); //append the part corba reference
        return aPartList;
//    }

}

/*!
 * \brief MCCAD::explodePart
 *  explode the compound and comsolid in this part
 *  into parts with solids
 * \param aPart part to be explode
 * \return a list of part with a single solid
 */
MCCAD_ORB::PartList*   MCCAD::explodePart(MCCAD_ORB::Part_ptr aPart) throw (SALOME::SALOME_Exception)
{
    MCCAD_ORB::PartList * aPartList = new MCCAD_ORB::PartList;
    if (!aPart->isExplodible()) {  //if not explodible, return this part
        aPartList->length(1); //increase
        (*aPartList)[0] = MCCAD_ORB::Part::_duplicate(aPart); //append the part corba reference
    }
    else {
        int cnt= 0;
        TopoDS_Shape * aShape = (TopoDS_Shape *)(aPart->getShapePointer());
        TopExp_Explorer ex;
        for (ex.Init(*aShape, TopAbs_SOLID); ex.More(); ex.Next())        {
            cnt ++;
            TopoDS_Solid tmpSol = TopoDS::Solid(ex.Current());
            QString aTmpStr = QString(aPart->getName()) + QString("-Part%1").arg(cnt);
            Part * aNewPart = new Part(aTmpStr.toLatin1(), genId());
//            MCCAD_ORB::Part_var aNewPart = deepCopyPart(aPart);
            //deep copy the infomation from apart
            aNewPart->setRemark(aPart->getRemark());
            aNewPart->setAdditive(aPart->getAdditive());
            MCCAD_ORB::MColor aMColor;
            aPart->getColor(aMColor);
            aNewPart->setColor(aMColor);
            MCCAD_ORB::FixArray_var aIMP = aPart->getImportance();
            aNewPart->setImportance(aIMP);
            aNewPart->setShape(tmpSol);;
            aNewPart->setIsDecomposed(aPart->getIsDecomposed());

            aPartList->length(cnt); //increase
//            (*aPartList)[cnt-1] = aNewPart._retn(); //append the part corba reference
            (*aPartList)[cnt-1] = aNewPart->_this(); //append the part corba reference
        }
        if (cnt == 0) {
//            CORBA::release(aPartList);
            delete aPartList;
            THROW_SALOME_CORBA_EXCEPTION("No solid inside this file", SALOME::BAD_PARAM);
        }
    }
    return aPartList;
}


/*!
 * \brief MCCAD::fuseParts
 *  fuse several parts into one part
 * \param aPartList  a list of parts
 * \return a new part which contains all shape of the part list
 */
MCCAD_ORB::Part_ptr MCCAD::fuseParts(const MCCAD_ORB::PartList& aPartList, const char* aName) throw (SALOME::SALOME_Exception)
{
    if (aPartList.length() <= 1)  {
        THROW_SALOME_CORBA_EXCEPTION("Need more than two parts to fuse!", SALOME::BAD_PARAM);
    }

    MCCAD_ORB::Part_var aNewPart ;
    if (aPartList[0]->_is_nil())
        aNewPart = generatePart(aName);
    else
        aNewPart = deepCopyPart(aPartList[0]);  //deep copy the info in the first part

    //discard the non-appliable info
    aNewPart->clearMesh();
    aNewPart->setName(aName);

    TopoDS_Compound aCompound;
    BRep_Builder aBuilder;
    //make a compound and return
    aBuilder.MakeCompound (aCompound);
    bool isDecomposed = true;
    for (int i = 0; i < aPartList.length(); i++)
    {
        if (!CORBA::is_nil(aPartList[i]))
        {
            TopoDS_Shape* aShape = (TopoDS_Shape*)( aPartList[i]->getShapePointer());
            if (aShape != NULL)
                aBuilder.Add (aCompound, *aShape);

            if (aPartList[i]->getIsDecomposed() == false)
                isDecomposed = false;
        }
    }
    aNewPart->setShapeStream(*Shape2Stream(aCompound));
    aNewPart->setIsDecomposed(isDecomposed);
    return  aNewPart._retn();
}


/*!
 * \brief MCCAD::decomposePart
 *  decompose a part using McCad functionalities,
 *  the shape of the part will be decomposed into convex solids
 *  these solid(s) is merge again as a compound and assign for this part,
 *  the old shape will be push to backup
 * \param aPart the part to be decomposed
 * \return false if error
 */

CORBA::Boolean MCCAD::decomposePart(MCCAD_ORB::Part_ptr aPart)
{
    //check the part
    if(aPart->_is_nil()) {
        MESSAGE ("the Part is nil!") ;
        return false;
    }
    if(aPart->getIsDecomposed()) //if already decomposed, do not decompose again
        return true;

//    TopoDS_Shape aShape = * ((TopoDS_Shape *)aPart->getShapePointer()); //in the same host: get the shape pointer and then get the shape
    TopoDS_Shape * aShape = (TopoDS_Shape *)aPart->getShapePointer();
    if (aShape == NULL || aShape->IsNull()) {
        MESSAGE ("the shape in the part is nill!") ;
        return false;
    }

    //decompose the shape
    Handle_TopTools_HSequenceOfShape aShapeList = new TopTools_HSequenceOfShape();
    Handle_TopTools_HSequenceOfShape aErrShapeList = new TopTools_HSequenceOfShape();

//    aShapeList = decomposeShape(*aShape) ;
//    if (aShapeList == NULL) {
    bool isOk;
   if (!decomposeShape(*aShape, aShapeList, aErrShapeList) ) {
//        aPart->setIsDecomposed(false);
//        return false;
        aShapeList->Append(aErrShapeList);
        isOk = false;
    }
   else    {   //set the decomposed flag as true
//       aPart->setIsDecomposed(true);
       isOk = true;
   }



    //make as a compound and set it for the part as new shape
    TopoDS_Compound aCompound;
    BRep_Builder aBuilder;
    //make a compound and return
    aBuilder.MakeCompound (aCompound);
    for(int i=1; i<=aShapeList->Length(); i++){
        aBuilder.Add (aCompound, aShapeList->Value(i));
    }
    if (!aCompound.IsNull()) {
        aPart->setShapeStream(*Shape2Stream(aCompound));
    }

    aPart->setIsDecomposed(isOk);//20160829 this should be done at the end!! because setShapeStream overwrites myIsDecomposed.

    return isOk;
}


/*!
 * \brief MCCAD::decomposeEnvelop
 *  decompose the mesh envelop of the component
 * \sa decomposePart
 * \param aComponent a component reference
 * \return false if error
 */
CORBA::Boolean MCCAD::decomposeEnvelop(MCCAD_ORB::Component_ptr aComponent)
{
    //check the Component
    if(aComponent->_is_nil()) {
        MESSAGE ("the Component is nil!") ;
        return false;
    }

    TopoDS_Shape * aShape = (TopoDS_Shape *)aComponent->getEnvelopPointer(); //in the same host: get the shape pointer and then get the shape
    if (aShape == NULL || aShape->IsNull()) {
        MESSAGE ("the shape in the Component is nill!") ;
        return false;
    }

    //decompose the shape
    Handle_TopTools_HSequenceOfShape aShapeList = new TopTools_HSequenceOfShape();
    Handle_TopTools_HSequenceOfShape aErrShapeList = new TopTools_HSequenceOfShape();
//    aShapeList = decomposeShape(*aShape) ;
    if (!decomposeShape(*aShape, aShapeList, aErrShapeList) ) {
        return false;
    }
    //make as a compound and set it for the Component as new shape
    TopoDS_Compound aCompound;
    BRep_Builder aBuilder;
    //make a compound and return
    aBuilder.MakeCompound (aCompound);
    for(int i=1; i<=aShapeList->Length(); i++){ //i must start from 1 !!
        aBuilder.Add (aCompound, aShapeList->Value(i));
    }
    if (!aCompound.IsNull()) {
        aComponent->setEnvelopStream(*Shape2Stream(aCompound));
    }
    return true;
}


/*!
 * \brief MCCAD::decomposeShape
 *  decompose a shape
 * \param aShape a topods_shape
 * \param OutputSolids output decomposed solids
 * \param ErrorSolids output decomposed solids
 * \return a list of shape
 */
CORBA::Boolean MCCAD::decomposeShape (const TopoDS_Shape & aShape,
                                                        Handle_TopTools_HSequenceOfShape & OutputSolids,
                                                        Handle_TopTools_HSequenceOfShape & ErrorSolids)
{
    //put the shape into the Handle_squenceOfShape
    Handle_TopTools_HSequenceOfShape aShapeList = new TopTools_HSequenceOfShape();
    aShapeList->Append(aShape);
    //BEGIN####CALL THE MCCAD FOR DECOMPOSITION####
/*     these are old decomposition algorithm
    McCadConvertTools_Convertor convertor(aShapeList);
    convertor.Convert();
    if(!convertor.IsConverted()){
        MESSAGE ("decompose the shape failed!") ;
        return NULL;
    }
    aShapeList = convertor.GetConvertedModel(); // get the list of decompose shape
*/
    //new decomposition algorithm by Lei Lu
    McCadDecompose * pDecompose = new McCadDecompose();
    pDecompose->InputData(aShapeList,"");//dummy file name because we don't export file
    pDecompose->SetCombine(1); //1 means the the decomposed solids which belong to one original solid will be combined
    pDecompose->Decompose();
    OutputSolids= pDecompose->getOutCompSolids();
    ErrorSolids = pDecompose->getErrCompSolids();

    if (ErrorSolids->IsEmpty()) return true; //if no error solids
    else return false;

    //END  ####
}


/*!
 * \brief MCCAD::importMED
 *  import a MED mesh, generate a CORBA MEDCoupling
 * \param MEDFileName
 * \return
 */
SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr MCCAD::importMED(const char* MEDFileName)
throw (SALOME::SALOME_Exception)
{
    if (*MEDFileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    MESSAGE ("Import Meshes from: " << MEDFileName);
    QStringList atmpList = QString(MEDFileName).split('/');
    atmpList = atmpList.at(atmpList.size()-1).split('.');

    //import the MED file
    try
    {
        //first check the file
        MEDLoader::CheckFileForRead(MEDFileName);
        //get the mesh name
        vector <string> MeshNames = MEDLoader::GetMeshNames(MEDFileName);
        if (MeshNames.size() > 1)
            MESSAGE ("More than one mesh is discovered, only the first one will be used");

        //using MEDloader Advance API for importing mesh
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDFileUMesh>  aNewFileUMesh
                = ParaMEDMEM::MEDFileUMesh::New(MEDFileName);
        if (aNewFileUMesh ->getMeshDimension() != 3 ) //if not 3D mesh
            THROW_SALOME_CORBA_EXCEPTION("Not a 3D mesh!", SALOME::BAD_PARAM);
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDCouplingUMesh> aNewUMesh
                = aNewFileUMesh ->getMeshAtLevel(0);
        if (aNewUMesh->getType() != ParaMEDMEM::UNSTRUCTURED)
            THROW_SALOME_CORBA_EXCEPTION("Not a Unstructured mesh!", SALOME::BAD_PARAM);
        aNewUMesh->setName(atmpList.at(0).toStdString().c_str());
        //create field IOR
        ParaMEDMEM::MEDCouplingUMeshServant *aUMeshI=new ParaMEDMEM::MEDCouplingUMeshServant(aNewUMesh);
        SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr myUMeshIOR = aUMeshI->_this();
        return myUMeshIOR;

    }
    catch (INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION( ex.what(), SALOME::BAD_PARAM);
    }
    return SALOME_MED::MEDCouplingUMeshCorbaInterface::_nil();
}

/*!
 * \brief MCCAD::export2MED
 *  export a MEDCouplingUMesh to a MED file
 * \param aMesh the MEDCouplingUmesh
 * \param FileName file name
 * \return false if error
 */
void             MCCAD::export2MED(SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr aMesh, const char* FileName)
throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("The mesh is nil!", SALOME::BAD_PARAM);
    if (*FileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    MESSAGE ("Write Mesh: " << aMesh->getName() << " to " << FileName);

    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingUMesh> aUMesh_ptr
            = ParaMEDMEM::MEDCouplingUMeshClient::New(aMesh);
    try
    {
        //see http://docs.salome-platform.org/salome_6_6_0/gui/MED/MEDLoaderAdvancedAPIPage.html#AdvMEDLoaderAPIFieldWC
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDFileUMesh > aFileUMesh
                = ParaMEDMEM::MEDFileUMesh::New();
        aFileUMesh->setMeshAtLevel(0, aUMesh_ptr);
        aFileUMesh->write(FileName, 2);
    }
    catch (INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::BAD_PARAM);
    }
}

/*!
 * \brief exportAllMesh2Abaqus
 *  export all mesh exist inside parts into a abaqus file
 * \param FileName
 */
void   MCCAD:: exportAllMesh2Abaqus(CORBA::Long studyID, const char* FileName)   throw (SALOME::SALOME_Exception)
{
    if (*FileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    vector <const ParaMEDMEM::MEDCouplingUMesh *> aUMeshList ;

    //loop all part, if has mesh, append the mesh
    if ( myData.find( studyID ) != myData.end() ) //find the list according to the list
    {
        MCCAD_ORB::ComponentList * aComponentList = myData[studyID];
        for (int i = 0; i <  aComponentList->length(); i++)
        {
            MCCAD_ORB::Component_var  aComponent = (*aComponentList)[i];
            if (!aComponent->_is_nil()) {
                MCCAD_ORB::GroupList * aGroupList = aComponent->getGroupList();
                for (int j = 0; j <  aGroupList->length(); j++)
                {
                    MCCAD_ORB::Group_var  aGroup = (*aGroupList)[j];
                    if (!aGroup->_is_nil()) {
                        MCCAD_ORB::PartList * aPartList = aGroup->getPartList();
                        for (int k = 0; k <  aPartList->length(); k++)
                        {
                            MCCAD_ORB::Part_var  aPart = (*aPartList)[k];
                            if (!aPart->_is_nil()) {
                                if (aPart->hasMesh()) {
                                ParaMEDMEM::MEDCouplingUMesh * aUMesh =  ParaMEDMEM::MEDCouplingUMeshClient::New(aPart->getMesh());
                                aUMesh->setName(aPart->getName());
                                aUMeshList.push_back(aUMesh);
                                }
                            }
                        }
                    }
                }
            }
        }

        MCMESHTRAN_AbaqusMeshWriter aAbaqusWriter;
        aAbaqusWriter.setMeshes(aUMeshList);
        bool isOK = aAbaqusWriter.exportFile(FileName);

        for (int i=0; i<aUMeshList.size(); i++) {
            aUMeshList[i]->decrRef();  //should increase reference
        }
        if (!isOK)
            THROW_SALOME_CORBA_EXCEPTION("Export meshes to Abaqus file failed!", SALOME::BAD_PARAM);
    }
}

CORBA::Boolean   MCCAD:: generateTetMesh(MCCAD_ORB::Part_ptr aPart, CORBA::Double aDeflection,
                                         CORBA::Double /*aCoefficient*/aVolThreshold, CORBA::Double aMeshQuality)  throw (SALOME::SALOME_Exception)
{
    if (aPart->_is_nil()) {
        MESSAGE("A nil part, we skip it!");
        return false;
    }
    bool isOk = false;
#ifdef WITH_TETGEN
    isOk = generateTetgenMesh(aPart, aDeflection, /*aCoefficient*/aVolThreshold, aMeshQuality);
#endif
    if (!isOk)
    {
        //use Netgen for mesh generation
        isOk = generateNetgenMesh(aPart, aDeflection, /*aCoefficient*/aVolThreshold);

    }
}

/*!
 * \brief generateTetMesh
 *  generate tetrahedral mesh using Tetgen,
 *  the Solid is firstly tessellated and export to STL file
 *  and then import by Tetgen and generate the mesh,
 *  the mesh is then import into MED mesh
 * \param aPartList  a list of parts which would be meshed
 * \return  false if error
 */
//int op_decrease (int i) { return --i; }
//CORBA::Boolean   MCCAD:: generateTetgenMesh(MCCAD_ORB::Part_ptr aPart, CORBA::Double aDeflection,
//                                         CORBA::Double aCoefficient, CORBA::Double aMeshQuality)  throw (SALOME::SALOME_Exception)
//{
//#ifdef WITH_TETGEN
////    //check the list
////    if (aPartList.length() == 0) {
////        MESSAGE("No part to generate mesh!");
////        return false;
////    }

////    //for each part,
////    const int n = aPartList.length();
////    for (int i=0; i<n; i++)
////    {
//        //get the part and the shape
////        MCCAD_ORB::Part_var  aPart = aPartList[i];
////        if (aPart->_is_nil()) {
////            MESSAGE("A nil part, we skip it!");
////            return false;
////        }
//    if (aPart->_is_nil()) {
//        MESSAGE("A nil part, we skip it!");
//        return false;
//    }
//        TopoDS_Shape * aShape = (TopoDS_Shape *)aPart->getShapePointer(); //we use this method in the same host
//        if (aShape->IsNull()) {
//            MESSAGE("This part contains no shape, we skip it!");
//            return false;
//        }
//        //write the shape into stl
//        QString aTmpName = QString("tmp_") + QString(aPart->getName()) + QString(".stl");
//        StlAPI_Writer aSTLWriter ;
//        aSTLWriter.ASCIIMode() = Standard_True; // if ASCII output
//        aSTLWriter.RelativeMode() = Standard_True; //force to use user parameter
//        aSTLWriter.SetDeflection((Standard_Real)aDeflection);  //set parameters
//        aSTLWriter.SetCoefficient((Standard_Real)aCoefficient);//set parameters
//        aSTLWriter.Write(*aShape, aTmpName.toStdString().c_str());

//        //load the file with tetgen
//        //see also CADMesh source code
//        tetgenio aTetgenIn, aTetgenOut ;
//        aTetgenIn.load_stl(const_cast <char *>(aTmpName.toStdString().c_str())); //load the temp file
//        QString aConfig = "pCCV";//
////        QString aConfig = "pY";  //qiu add Y to preserved the surface mesh
////        if (aMeshQuality > 0) //set the Tetgen meshing quality
////            aConfig = aConfig + QString("q") + QString("%1").arg(double (aMeshQuality));

//        //we try to catch the exception, and safely skip this part.
//        try {
//        tetrahedralize(const_cast <char *>(aConfig.toStdString().c_str()), &aTetgenIn, &aTetgenOut);
//        }
//        catch(int & ) { //the tetgen throw an integer number
//            MESSAGE("failed to generated mesh for "<< aPart->getName()<<" !");
//            STEPControl_Writer aWriter  ;
//            aWriter.Transfer(*aShape, STEPControl_AsIs);
//            QString aTmpStepName = QString("tmp_") + QString(aPart->getName()) + QString(".stp");
//            aWriter.Write(aTmpStepName.toLatin1());
//            return false;
//        }

//        //get the mesh from aTetgenOut, and make a MED mesh
//        //see also http://docs.salome-platform.org/salome_6_6_0/gui/MED/medcouplingcppexamplesUmeshStdBuild1.html
//        //first copy the node list
//        ParaMEDMEM::DataArrayDouble *aCoords=ParaMEDMEM::DataArrayDouble::New();
//        aCoords->alloc(aTetgenOut.numberofpoints,3);
//        std::copy(aTetgenOut.pointlist , aTetgenOut.pointlist + aTetgenOut.numberofpoints*3 , aCoords->getPointer());
//        ParaMEDMEM::MEDCouplingUMesh *aMesh=ParaMEDMEM::MEDCouplingUMesh::New(aPart->getName(),3);
//        aMesh->setCoords(aCoords);//set the point list
//        aCoords->decrRef();

//        if (aTetgenOut.numberoftetrahedra == 0){
//            MESSAGE("The mesh "<<aPart->getName()<<" has no cell!");
//            aMesh->decrRef();
//            STEPControl_Writer aWriter  ;
//            aWriter.Transfer(*aShape, STEPControl_AsIs);
//            QString aTmpStepName = QString("tmp_") + QString(aPart->getName()) + QString(".stp");
//            aWriter.Write(aTmpStepName.toLatin1());
//            return false;
//        }

//        //then copy the connectivities and insert cells
//        aMesh->allocateCells(aTetgenOut.numberoftetrahedra);
//        int atmpArray [10];
//        if (aTetgenOut.numberofcorners == 4) // if first-order tetrahedron
//        {
//            for (int j=0; j<aTetgenOut.numberoftetrahedra; j++) {
//                //because in connectivities, Tetgen start from 1, MED start from 0
//                std::transform(aTetgenOut.tetrahedronlist + j*4, aTetgenOut.tetrahedronlist + j*4 + 4, atmpArray, op_decrease);
//                aMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4, 4,atmpArray );
//            }
//            aMesh->finishInsertingCells();
//        }
//        else if (aTetgenOut.numberofcorners == 10) // if second-order tetrahedron
//        {
//            for (int j=0; j<aTetgenOut.numberoftetrahedra; j++) {
//                //because in connectivities, Tetgen start from 1, MED start from 0
//                std::transform(aTetgenOut.tetrahedronlist + j*10, aTetgenOut.tetrahedronlist + j*10 + 10, atmpArray, op_decrease);
//                aMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA10, 10,atmpArray );
//            }
//            aMesh->finishInsertingCells();
//        }


////        //reverse the nodal connectivities because MED mesh has inverted connectivities compared with Tetgen mesh!
////        ParaMEDMEM::DataArrayInt *revNodal=DataArrayInt::New();
////        ParaMEDMEM::DataArrayInt *revNodalIndx=DataArrayInt::New();
////        aMesh->getReverseNodalConnectivity(revNodal,revNodalIndx);
////        aMesh->setConnectivity(revNodal,revNodalIndx );
////        revNodal->decrRef();
////        revNodalIndx->decrRef();
////        aMesh->rearrange2ConsecutiveCellTypes();
////        vector <int> aRevCellId;
////        for (int i=aTetgenOut.numberoftetrahedra-1; i>=0; i--)
////            aRevCellId.push_back(i);
////        aMesh->renumberCells(aRevCellId.data());

//        //assign this mesh to the parat
//        ParaMEDMEM::MEDCouplingUMeshServant *aMeshServant=new ParaMEDMEM::MEDCouplingUMeshServant(aMesh);
//        SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr aMeshPtr = aMeshServant->_this();
//        aMesh->decrRef();
//        aPart->setMesh(aMeshPtr);
//        QFile::remove(aTmpName);


////        //for testing
////        aTetgenOut.save_elements("/mnt/shared/Tessellated2Tetrahedral/Alite80/Problem_mesh");
////        aTetgenOut.save_nodes("/mnt/shared/Tessellated2Tetrahedral/Alite80/Problem_mesh");
////        aTetgenOut.save_poly("/mnt/shared/Tessellated2Tetrahedral/Alite80/Problem_mesh");


////    }
//#endif
//    return true;
//}


//!!!!!!!Experimental work!!!!!!!!!!!!
//! try to delete those cells with very small volume!
//! they might affect the simulation
int op_decrease (int i) { return --i; }
CORBA::Boolean   MCCAD:: generateTetgenMesh(MCCAD_ORB::Part_ptr aPart, CORBA::Double aDeflection,
                                         CORBA::Double /*aCoefficient*/aVolThreshold, CORBA::Double aMeshQuality)  throw (SALOME::SALOME_Exception)
{
#ifdef WITH_TETGEN
//    //check the list
//    if (aPartList.length() == 0) {
//        MESSAGE("No part to generate mesh!");
//        return false;
//    }

//    //for each part,
//    const int n = aPartList.length();
//    for (int i=0; i<n; i++)
//    {
        //get the part and the shape
//        MCCAD_ORB::Part_var  aPart = aPartList[i];
//        if (aPart->_is_nil()) {
//            MESSAGE("A nil part, we skip it!");
//            return false;
//        }
    if (aPart->_is_nil()) {
        MESSAGE("A nil part, we skip it!");
        return false;
    }
        TopoDS_Shape * aShape = (TopoDS_Shape *)aPart->getShapePointer(); //we use this method in the same host
        if (aShape->IsNull()) {
            MESSAGE("This part contains no shape, we skip it!");
            return false;
        }

        //comment out because we use OCC tessellation to replace STL file
        //STL has only 32 bit, this might cause problems
        /*
        //write the shape into stl
        QString aTmpName = QString("tmp_") + QString(aPart->getName()) + QString(".stl");
        StlAPI_Writer aSTLWriter ;
        aSTLWriter.ASCIIMode() = Standard_True; // if ASCII output
        aSTLWriter.RelativeMode() = Standard_True; //force to use user parameter
        aSTLWriter.SetDeflection((Standard_Real)aDeflection);  //set parameters
        aSTLWriter.SetCoefficient((Standard_Real)aCoefficient);//set parameters
        aSTLWriter.Write(*aShape, aTmpName.toStdString().c_str());

        //load the file with tetgen
        //see also CADMesh source code
        tetgenio aTetgenIn, aTetgenOut ;
        aTetgenIn.load_stl(const_cast <char *>(aTmpName.toStdString().c_str())); //load the temp file
        */

        //USING OCC TESSELLATION TO REPLACE STL FILE
        vector <double > aPointList;
        vector <int > aFacetList;
        //get the tessellation
        TessellateShape(*aShape, aPointList, aFacetList,aDeflection);
        //put the points and facets into the tetgen
        tetgenio aTetgenIn, aTetgenOut ;
        tetgenio::facet *f;
        tetgenio::polygon *p;
        //copy from tetgenio::load_stl
        aTetgenIn.numberofpoints = aPointList.size()/3;
        aTetgenIn.pointlist = new REAL[aPointList.size()];
//        cout <<"size of a point: "<<sizeof(aTetgenIn.pointlist[0] )<<endl;

        for (int i = 0; i < aPointList.size(); i++) {
            aTetgenIn.pointlist[i] =  aPointList[i]; //copy the nodes
//               cout<<setprecision(12)<<aTetgenIn.pointlist[i]<<endl;
        }

        int nfaces = aFacetList.size()/3;
        aTetgenIn.numberoffacets = nfaces;
        aTetgenIn.facetlist = new tetgenio::facet[nfaces];

        // Default use '1' as the array starting index.
        aTetgenIn.firstnumber = 1;
        for (int i = 0; i < nfaces; i++) {
          f = &aTetgenIn.facetlist[i];
          aTetgenIn.init(f);
          // In .stl format, each facet has one polygon, no hole.
          f->numberofpolygons = 1;
          f->polygonlist = new tetgenio::polygon[1];
          p = &f->polygonlist[0];
          aTetgenIn.init(p);
          // Each polygon has three vertices.
          p->numberofvertices = 3;
          p->vertexlist = new int[p->numberofvertices];
          p->vertexlist[0] = aFacetList[i*3];
          p->vertexlist[1] = aFacetList[i*3 + 1];
          p->vertexlist[2] = aFacetList[i*3 + 2];

//          cout<<"Point\t"<<p->vertexlist[0]<<"\t"<<p->vertexlist[1]<<"\t"<<p->vertexlist[2]<<"\t"<<endl;
        }


        QString aConfig = "p";//
//        QString aConfig = "pY";  //qiu add Y to preserved the surface mesh
//        if (aMeshQuality > 0) //set the Tetgen meshing quality
//            aConfig = aConfig + QString("q") + QString("%1").arg(double (aMeshQuality));

        //we try to catch the exception, and safely skip this part.
        try {
        tetrahedralize(const_cast <char *>(aConfig.toStdString().c_str()), &aTetgenIn, &aTetgenOut);
        }
        catch(int & ) { //the tetgen throw an integer number
            MESSAGE("failed to generated mesh for "<< aPart->getName()<<" !");
            STEPControl_Writer aWriter  ;
            aWriter.Transfer(*aShape, STEPControl_AsIs);
            QString aTmpStepName = QString("tmp_") + QString(aPart->getName()) + QString(".stp");
            aWriter.Write(aTmpStepName.toLatin1());
            return false;
        }

        //get the mesh from aTetgenOut, and make a MED mesh
        //see also http://docs.salome-platform.org/salome_6_6_0/gui/MED/medcouplingcppexamplesUmeshStdBuild1.html
        //first copy the node list
        ParaMEDMEM::DataArrayDouble *aCoords=ParaMEDMEM::DataArrayDouble::New();
        aCoords->alloc(aTetgenOut.numberofpoints,3);
        std::copy(aTetgenOut.pointlist , aTetgenOut.pointlist + aTetgenOut.numberofpoints*3 , aCoords->getPointer());
        ParaMEDMEM::MEDCouplingUMesh *aMesh=ParaMEDMEM::MEDCouplingUMesh::New(aPart->getName(),3);
        aMesh->setCoords(aCoords);//set the point list

        if (aTetgenOut.numberoftetrahedra == 0){
            MESSAGE("The mesh "<<aPart->getName()<<" has no cell!");
            aMesh->decrRef();
            STEPControl_Writer aWriter  ;
            aWriter.Transfer(*aShape, STEPControl_AsIs);
            QString aTmpStepName = QString("tmp_") + QString(aPart->getName()) + QString(".stp");
            aWriter.Write(aTmpStepName.toLatin1());
            return false;
        }

        //then copy the connectivities and insert cells
        aMesh->allocateCells(aTetgenOut.numberoftetrahedra);
        int atmpArray [10];
        if (aTetgenOut.numberofcorners == 4) // if first-order tetrahedron
        {
            for (int j=0; j<aTetgenOut.numberoftetrahedra; j++) {
                //because in connectivities, Tetgen start from 1, MED start from 0
                std::transform(aTetgenOut.tetrahedronlist + j*4, aTetgenOut.tetrahedronlist + j*4 + 4, atmpArray, op_decrease);
                aMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4, 4,atmpArray );
            }
            aMesh->finishInsertingCells();
        }
        else if (aTetgenOut.numberofcorners == 10) // if second-order tetrahedron
        {
            for (int j=0; j<aTetgenOut.numberoftetrahedra; j++) {
                //because in connectivities, Tetgen start from 1, MED start from 0
                std::transform(aTetgenOut.tetrahedronlist + j*10, aTetgenOut.tetrahedronlist + j*10 + 10, atmpArray, op_decrease);
                aMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA10, 10,atmpArray );
            }
            aMesh->finishInsertingCells();
        }

        //try to filter those cells with small volume
        ParaMEDMEM::MEDCouplingUMesh *bMesh=ParaMEDMEM::MEDCouplingUMesh::New(aPart->getName(),3);
        bMesh->setCoords(aCoords);//set the point list
        bMesh->allocateCells(aTetgenOut.numberoftetrahedra); //can be more than the actually cells
        //get the volume field of the aMesh
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::DataArrayDouble> aArray =
                aMesh->getMeasureField(true)->getArray();
        vector <double > aTupe;
        aTupe.resize(1);
        const double VolThreshold = aVolThreshold; //volume less than 1 mm3 is remove. unit mm3 !!! only for experimental
        if (aTetgenOut.numberofcorners == 4) // if first-order tetrahedron
        {
            for (int j=0; j<aTetgenOut.numberoftetrahedra; j++) {
                //get the volume of the current cell
                aArray->getTuple(j, aTupe.data());
                if (aTupe[0] < VolThreshold) {
                    cout <<"Found cell\t"<<j<<" has tiny volume\t"<<aTupe[0]<<", try to remove it!"<<endl;
                    continue;
                }
                //because in connectivities, Tetgen start from 1, MED start from 0
                std::transform(aTetgenOut.tetrahedronlist + j*4, aTetgenOut.tetrahedronlist + j*4 + 4, atmpArray, op_decrease);
                bMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4, 4,atmpArray );
            }
            bMesh->finishInsertingCells();
        }
        else if (aTetgenOut.numberofcorners == 10) // if second-order tetrahedron
        {
            for (int j=0; j<aTetgenOut.numberoftetrahedra; j++) {
                //get the volume of the current cell
                aArray->getTuple(j, aTupe.data());
                if (aTupe[0] < VolThreshold) {
                    cout <<"Found cell\t"<<j<<" has tiny volume\t"<<aTupe[0]<<", try to remove it!"<<endl;
                    continue;
                }
                //because in connectivities, Tetgen start from 1, MED start from 0
                std::transform(aTetgenOut.tetrahedronlist + j*10, aTetgenOut.tetrahedronlist + j*10 + 10, atmpArray, op_decrease);
                bMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA10, 10,atmpArray );
            }
            bMesh->finishInsertingCells();
        }



//        //reverse the nodal connectivities because MED mesh has inverted connectivities compared with Tetgen mesh!
//        ParaMEDMEM::DataArrayInt *revNodal=DataArrayInt::New();
//        ParaMEDMEM::DataArrayInt *revNodalIndx=DataArrayInt::New();
//        aMesh->getReverseNodalConnectivity(revNodal,revNodalIndx);
//        aMesh->setConnectivity(revNodal,revNodalIndx );
//        revNodal->decrRef();
//        revNodalIndx->decrRef();
//        aMesh->rearrange2ConsecutiveCellTypes();
//        vector <int> aRevCellId;
//        for (int i=aTetgenOut.numberoftetrahedra-1; i>=0; i--)
//            aRevCellId.push_back(i);
//        aMesh->renumberCells(aRevCellId.data());

        //assign this mesh to the part
        ParaMEDMEM::MEDCouplingUMeshServant *aMeshServant=new ParaMEDMEM::MEDCouplingUMeshServant(bMesh);
        SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr aMeshPtr = aMeshServant->_this();
        aCoords->decrRef();
        aMesh->decrRef();
        bMesh->decrRef();
        aPart->setMesh(aMeshPtr);
//        QFile::remove(aTmpName);


//        //for testing
//        aTetgenOut.save_elements("/mnt/shared/Tessellated2Tetrahedral/Alite80/Problem_mesh");
//        aTetgenOut.save_nodes("/mnt/shared/Tessellated2Tetrahedral/Alite80/Problem_mesh");
//        aTetgenOut.save_poly("/mnt/shared/Tessellated2Tetrahedral/Alite80/Problem_mesh");


//    }
#endif
    return true;
}

//CORBA::Boolean   MCCAD:: generateNetgenMesh(MCCAD_ORB::Part_ptr aPart, CORBA::Double aDeflection,
//                                         CORBA::Double aCoefficient)  throw (SALOME::SALOME_Exception)
//{
//    if (aPart->_is_nil()) {
//        MESSAGE("A nil part, we skip it!");
//        return false;
//    }
//    TopoDS_Shape * aShape = (TopoDS_Shape *)aPart->getShapePointer(); //we use this method in the same host
//    if (aShape->IsNull()) {
//        MESSAGE("This part contains no shape, we skip it!");
//        return false;
//    }
//    //write the shape into stl
//    QString aTmpName = QString("tmp_") + QString(aPart->getName()) + QString(".stl");
//    StlAPI_Writer aSTLWriter ;
//    aSTLWriter.ASCIIMode() = Standard_True; // if ASCII output
//    //        aSTLWriter.RelativeMode() = Standard_True; //force to use user parameter
//    aSTLWriter.SetDeflection((Standard_Real)aDeflection);  //set parameters
//    aSTLWriter.SetCoefficient((Standard_Real)aCoefficient);//set parameters
//    aSTLWriter.Write(*aShape, aTmpName.toStdString().c_str());

//    //load the file with Netgen
//    //http://netgen.sourcearchive.com/documentation/4.4-9/ng__vol_8cpp-source.html
//    //http://netgen.sourcearchive.com/documentation/4.4-9/ng__stl_8cpp-source.html
//    nglib::Ng_Mesh * aNetgenMesh;
//    nglib::Ng_Init();
//    //create mesh
//    aNetgenMesh = nglib::Ng_NewMesh();
//    nglib::Ng_STL_Geometry * aNetgenGeom;
//    nglib::Ng_Meshing_Parameters mp;
//    mp.maxh=100000;
//    mp.fineness = 0.5;
//    mp.secondorder = 0;

//    aNetgenGeom = nglib::Ng_STL_LoadGeometry (aTmpName.toStdString().c_str());
////   aNetgenGeom = nglib::Ng_STL_LoadGeometry ("/home/qiu/Desktop/beryllium_1.stl");
//   if (!aNetgenGeom) {
//        MESSAGE("Cannot read the STL geometry!");
//        nglib::Ng_DeleteMesh(aNetgenMesh);
//        nglib::Ng_Exit();
//        return false;
//    }

//    nglib::Ng_Result aRsltCode;
//    aRsltCode = nglib::Ng_STL_InitSTLGeometry(aNetgenGeom);
//    if ( aRsltCode != nglib::NG_OK) {
//        nglib::Ng_DeleteMesh(aNetgenMesh);
//        nglib::Ng_Exit();
//        MESSAGE("Initiating STL geometry failed!  Ng_result code: " << aRsltCode );
//        return false;
//    }
//    aRsltCode = nglib::Ng_STL_MakeEdges (aNetgenGeom, aNetgenMesh, &mp);
//    if ( aRsltCode != nglib::NG_OK) {
//        nglib::Ng_DeleteMesh(aNetgenMesh);
//        nglib::Ng_Exit();
//        MESSAGE("Error making edge from STL! Ng_result code: " << aRsltCode);
//        return false;
//    }
//    aRsltCode = nglib::Ng_STL_GenerateSurfaceMesh (aNetgenGeom, aNetgenMesh, &mp);
//    if ( aRsltCode != nglib::NG_OK) {
//        nglib::Ng_DeleteMesh(aNetgenMesh);
//        nglib::Ng_Exit();
//        MESSAGE("Error Generating surface mesh from STL! Ng_result code: " << aRsltCode);
//        return false;
//    }
//    aRsltCode = nglib::Ng_GenerateVolumeMesh(aNetgenMesh,&mp);
//    if ( aRsltCode != nglib::NG_OK) {
//        nglib::Ng_DeleteMesh(aNetgenMesh);
//        nglib::Ng_Exit();
//        MESSAGE("Error Generating volume mesh! Ng_result code: " << aRsltCode);
//        return false;
//    }

//    //get the mesh from netgen mesh, and make a MED mesh
//    int NbPoints = nglib::Ng_GetNP(aNetgenMesh);
//    int NbElements = nglib::Ng_GetNE(aNetgenMesh);
//    vector <double>  aCoordsVec ;
//    double  aPoint[3];
//    int     aTetElm[4];
//    for (int i=1; i <= NbPoints; i++) { //should start from 1
//        nglib::Ng_GetPoint(aNetgenMesh, i, aPoint);
//        aCoordsVec.push_back(aPoint[0]);
//        aCoordsVec.push_back(aPoint[1]);
//        aCoordsVec.push_back(aPoint[2]);
//    }
//    ParaMEDMEM::DataArrayDouble *aCoords=ParaMEDMEM::DataArrayDouble::New();
//    aCoords->alloc(NbPoints,3);
//    std::copy(aCoordsVec.begin() , aCoordsVec.end() , aCoords->getPointer());

//    ParaMEDMEM::MEDCouplingUMesh *aMesh=ParaMEDMEM::MEDCouplingUMesh::New(aPart->getName(),3);
//    aMesh->setCoords(aCoords);//set the point list
//    aCoords->decrRef();
//    aCoordsVec.clear();

//    if (NbElements == 0){
//        MESSAGE("The mesh has no cell!");
//        aMesh->decrRef();
//        STEPControl_Writer aWriter  ;
//        aWriter.Transfer(*aShape, STEPControl_AsIs);
//        QString aTmpStepName = QString("tmp_") + QString(aPart->getName()) + QString(".stp");
//        aWriter.Write(aTmpStepName.toLatin1());
//        nglib::Ng_DeleteMesh(aNetgenMesh);
//        nglib::Ng_Exit();
//        return false;
//    }

//    //then copy the connectivities and insert cells
//    aMesh->allocateCells(NbElements);

//    for (int i=1; i< NbElements; i++) {
//        nglib::Ng_GetVolumeElement(aNetgenMesh, i, aTetElm);

//        //because in connectivities, Tetgen start from 1, MED start from 0
////        std::transform(aTetgenOut.tetrahedronlist + j*4, aTetgenOut.tetrahedronlist + j*4 + 4, atmpArray, op_decrease);
//        aMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4, 4,aTetElm );
//    }
//    aMesh->finishInsertingCells();

//    //assign this mesh to the parat
//    ParaMEDMEM::MEDCouplingUMeshServant *aMeshServant=new ParaMEDMEM::MEDCouplingUMeshServant(aMesh);
//    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr aMeshPtr = aMeshServant->_this();
//    aMesh->decrRef();
//    aPart->setMesh(aMeshPtr);
//    QFile::remove(aTmpName);
//    nglib::Ng_DeleteMesh(aNetgenMesh);
//    nglib::Ng_Exit();
//    return true;
//}


//this version try to using trianglization function of OpenCASCADE
// The problem is that some edge is repeated so the meshing are not success

void MCCAD:: MeshShape(const TopoDS_Shape theShape, double & theDeflection)
{

  Standard_Real aDeflection = theDeflection <= 0 ? 0.001 : theDeflection;

  //If deflection <= 0, than return default deflection
  if(theDeflection <= 0)
    theDeflection = aDeflection;

  // Is shape triangulated?
//  Standard_Boolean alreadymeshed = Standard_True;
//  TopExp_Explorer ex;
//  TopLoc_Location aLoc;
//  for (ex.Init(theShape, TopAbs_FACE); ex.More(); ex.Next()) {
//    const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
//    Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
//    if(aPoly.IsNull()) {
//  alreadymeshed = Standard_False;
//  break;
//    }
//  }

  //the judgement of triangulation is comment out because we would like to mesh it
  //everytime with different deflection.
    Standard_Boolean alreadymeshed = Standard_False;  // force to mesh again

  if(!alreadymeshed) {
    Bnd_Box B;
    BRepBndLib::Add(theShape, B);
    if ( B.IsVoid() )
  return; // NPAL15983 (Bug when displaying empty groups)
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    // This magic line comes from Prs3d_ShadedShape.gxx in OCCT
    aDeflection = max(aXmax-aXmin, max( aYmax-aYmin, aZmax-aZmin)) * aDeflection * 4;

    //Clean triangulation before compute incremental mesh
    BRepTools::Clean(theShape);

    //Compute triangulation
    BRepMesh_IncrementalMesh MESH(theShape,aDeflection);
  }
}

/*!
 * \brief TessellateShape
 *  Tessellated the solid into many facets for meshing
 *  this function is used for replacing the STL file, since the
 * STL file have only float (32bit) precision.
 * \param theShape a opencascade CAD solid
 * \param PointList  a list of points (x1,y1,z1,x2,y2,z2,...) for returning
 * \param FacetList  a list of facets (va1,vb1,vc1, va2,vb2,vc2,... ) for returning
 */
void  MCCAD::TessellateShape(const TopoDS_Shape aShape, vector <double> &PointList, vector <int> &FacetList
                             , double & theDeflection)
{
    //clear the lists
    PointList.clear();
    FacetList.clear();

    //mesh the solid
    int NodeTotal=0;  //each time a face is mesh, the total node will be updated

    TopoDS_Shape theShape = aShape;
    MeshShape(theShape, theDeflection);
    TopExp_Explorer ex;
    TopLoc_Location aLoc;
    for (ex.Init(theShape, TopAbs_FACE); ex.More(); ex.Next())
    {
        const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
        Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
        if(aPoly.IsNull()) {
            cout<<"MCCAD::TessellateShape: error when tessellating the solid!!";
            return;
        }
        //get the transformation
        gp_Trsf myTransf;
        Standard_Boolean identity = true;
        if(!aLoc.IsIdentity()){
          identity = false;
          myTransf = aLoc.Transformation();
        }

        //get the nodes
        const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
        Standard_Integer nbNodesInFace = aPoly->NbNodes();
        for( int i = 1; i <= nbNodesInFace; i++) { // i start with 1 ??
          gp_Pnt P = Nodes(i);
          if(!identity)
            P.Transform(myTransf);
          //append a node
          PointList.push_back(P.X());
          PointList.push_back(P.Y());
          PointList.push_back(P.Z());
        }

        //get the facets
        const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
        Standard_Integer nbTriInFace = aPoly->NbTriangles();
        for( int i = 1; i <= nbTriInFace; i++)  // i start with 1 ??
        {
            // Get the triangle
            Standard_Integer N1,N2,N3;
            Triangles(i).Get(N1,N2,N3); //node id here start with 1
            N1 += NodeTotal ; //node id should start from 1 for Netgen!
            N2 += NodeTotal ;
            N3 += NodeTotal ;

            //add a facet
            //IMPORTANT!! this must do otherwise the visualization will be wrong!
            if (aFace.Orientation() == TopAbs_REVERSED) {

                FacetList.push_back(N3);//reverse node order
                FacetList.push_back(N2);
                FacetList.push_back(N1);
            }
            else {
                FacetList.push_back(N1);
                FacetList.push_back(N2);
                FacetList.push_back(N3);
            }
        }
        NodeTotal += nbNodesInFace;  //MUST DO!!

    }

}



CORBA::Boolean   MCCAD:: generateNetgenMesh(MCCAD_ORB::Part_ptr aPart, CORBA::Double aDeflection,
                                         CORBA::Double /*aCoefficient*/aVolThreshold)  throw (SALOME::SALOME_Exception)
{
    if (aPart->_is_nil()) {
        MESSAGE("A nil part, we skip it!");
        return false;
    }
    TopoDS_Shape * aShape = (TopoDS_Shape *)aPart->getShapePointer(); //we use this method in the same host
    if (aShape->IsNull()) {
        MESSAGE("This part contains no shape, we skip it!");
        return false;
    }
    //mesh the solid
    TopoDS_Shape theShape = *aShape;
    MeshShape(theShape, aDeflection);

    //load the file with Netgen
    //http://netgen.sourcearchive.com/documentation/4.4-9/ng__vol_8cpp-source.html
    //http://netgen.sourcearchive.com/documentation/4.4-9/ng__stl_8cpp-source.html
    nglib::Ng_Mesh * aNetgenMesh;
    nglib::Ng_Init();
    //create mesh
    aNetgenMesh = nglib::Ng_NewMesh();

    //parameters ---- to be optimized
    nglib::Ng_Meshing_Parameters mp;
    mp.uselocalh= 0; //1;
    mp.maxh= 1000.0;
    mp.fineness= 0.5;//0.5;
    mp.grading= 0.3;//0.3;
    mp.elementsperedge= 2.0;//2.0;
    mp.elementspercurve= 2.0;//2.0;
    mp.closeedgeenable= 0;
    mp.closeedgefact= 1.0;//2.0;
    mp.secondorder= 0.0;
//    mp.meshsize_filename= null;
    mp.quad_dominated= 0;
    mp.optsurfmeshenable= 1;//1;
    mp.optvolmeshenable= 1;//1;
    mp.optsteps_2d= 3;//3;
    mp.optsteps_3d= 3;//3;

    //create STL geometry
    nglib::Ng_STL_Geometry * aNetgenGeom;
    aNetgenGeom = nglib::Ng_STL_NewGeometry();

//    int NodeTotal=0;  //each time a face is mesh, the total node will be updated
    TopExp_Explorer ex;
    TopLoc_Location aLoc;
    //the thing comment out is old way, which is not working
    /*
    for (ex.Init(theShape, TopAbs_FACE); ex.More(); ex.Next())
    {
        const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
        Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
        if(aPoly.IsNull()) {
            return false;
        }
        //get the transformation
        gp_Trsf myTransf;
        Standard_Boolean identity = true;
        if(!aLoc.IsIdentity()){
          identity = false;
          myTransf = aLoc.Transformation();
        }
        //get the nodes and facets
        const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
        Standard_Integer nbNodesInFace = aPoly->NbNodes();
        for( int i = 1; i <= nbNodesInFace; i++) { // i start with 1 ??
          gp_Pnt P = Nodes(i);
          if(!identity)
            P.Transform(myTransf);
          //add a node
          double aPoint [3];
          aPoint [0] = P.X();
          aPoint [1] = P.Y();
          aPoint [2] = P.Z();
          nglib::Ng_AddPoint (aNetgenMesh, aPoint);
        }

        const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
        Standard_Integer nbTriInFace = aPoly->NbTriangles();
        for( int i = 1; i <= nbTriInFace; i++){  // i start with 1 ??
          // Get the triangle
          Standard_Integer N1,N2,N3;
          Triangles(i).Get(N1,N2,N3); //node id here start with 1
          N1 += NodeTotal ; //node id should start from 1 for Netgen!
          N2 += NodeTotal ;
          N3 += NodeTotal ;

          //add a facet
          int aFacet [3];
          //IMPORTANT!! this must do otherwise the visualization will be wrong!
          if (aFace.Orientation() == TopAbs_REVERSED) {
              aFacet[0] = N3; //reverse node order
              aFacet[1] = N2;
              aFacet[2] = N1;
              nglib::Ng_AddSurfaceElement(aNetgenMesh, nglib::NG_TRIG, aFacet);
          }
          else {
              aFacet[0] = N1;
              aFacet[1] = N2;
              aFacet[2] = N3;
              nglib::Ng_AddSurfaceElement(aNetgenMesh, nglib::NG_TRIG, aFacet);
          }

        }
        NodeTotal += nbNodesInFace;  //MUST DO!!
    }
    */

    //faceting all surfaces
    for (ex.Init(theShape, TopAbs_FACE); ex.More(); ex.Next())
    {
        const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
        Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
        if(aPoly.IsNull()) {
            return false;
        }
        //get the transformation
        gp_Trsf myTransf;
        Standard_Boolean identity = true;
        if(!aLoc.IsIdentity()){
          identity = false;
          myTransf = aLoc.Transformation();
        }

        //get the nodes
        const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
        Standard_Integer nbNodesInFace = aPoly->NbNodes();

        //get the facets
        const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
        Standard_Integer nbTriInFace = aPoly->NbTriangles();
        for( int i = 1; i <= nbTriInFace; i++)  // i start with 1 ??
        {
            // Get the triangle
            Standard_Integer N1,N2,N3;
            Triangles(i).Get(N1,N2,N3); //node id here start with 1
            //no need          N1 += NodeTotal ; //node id should start from 1 for Netgen!
            //no need          N2 += NodeTotal ;
            //no need          N3 += NodeTotal ;

            //add a facet
            int aFacet [3];
            //IMPORTANT!! this must do otherwise the visualization will be wrong!
            if (aFace.Orientation() == TopAbs_REVERSED) {
                aFacet[0] = N3; //reverse node order
                aFacet[1] = N2;
                aFacet[2] = N1;
                //no need               nglib::Ng_AddSurfaceElement(aNetgenMesh, nglib::NG_TRIG, aFacet);
            }
            else {
                aFacet[0] = N1;
                aFacet[1] = N2;
                aFacet[2] = N3;
                //no need               nglib::Ng_AddSurfaceElement(aNetgenMesh, nglib::NG_TRIG, aFacet);
            }

            //get the points of a triangle according to the node index
            double aTriangle[3][3];
            vector <gp_Pnt> aPointList;
            for (int i=0; i<3; i++) {
                if (aFacet[i] > nbNodesInFace) {
                    MESSAGE("Node index exceed number of nodes in this face!");
                    return false;
                }
                gp_Pnt P = Nodes(aFacet[i]);
                if(!identity)
                    P.Transform(myTransf);
                aTriangle [i][0] =P.X();
                aTriangle [i][1] =P.Y();
                aTriangle [i][2] =P.Z();
                aPointList.push_back(P);
            }

            //calculate the normal
            //from http://www.opencascade.org/org/forum/thread_25750/?forum=3
            //V1: P1->P2 V2: P2->P3
            gp_Vec aVec = gp_Vec(aPointList.at(1).XYZ()) - gp_Vec(aPointList.at(0).XYZ());
            gp_Vec bVec = gp_Vec(aPointList.at(2).XYZ()) - gp_Vec(aPointList.at(1).XYZ());
            gp_Vec aNormal;
            aNormal.SetX(aVec.Y()*bVec.Z() - aVec.Z()*bVec.Y());
            aNormal.SetY(aVec.Z()*bVec.X() - aVec.X()*bVec.Z());
            aNormal.SetZ(aVec.X()*bVec.Y() - aVec.Y()*bVec.X());
            aNormal.Normalize();
            double aNorm [3];
            aNorm[0] = aNormal.X();
            aNorm[1] = aNormal.Y();
            aNorm[2] = aNormal.Z();

            //add a facet into the netgen
            nglib::Ng_STL_AddTriangle(aNetgenGeom, aTriangle[0], aTriangle[1], aTriangle[2],aNorm);
        }
    }


//process the STL geometry and generated the mesh
    nglib::Ng_Result aRsltCode;
    aRsltCode = nglib::Ng_STL_InitSTLGeometry(aNetgenGeom);
    if ( aRsltCode != nglib::NG_OK) {
        nglib::Ng_DeleteMesh(aNetgenMesh);
        nglib::Ng_Exit();
        MESSAGE("Initiating STL geometry failed!  Ng_result code: " << aRsltCode );
        return false;
    }
    aRsltCode = nglib::Ng_STL_MakeEdges (aNetgenGeom, aNetgenMesh, &mp);
    if ( aRsltCode != nglib::NG_OK) {
        nglib::Ng_DeleteMesh(aNetgenMesh);
        nglib::Ng_Exit();
        MESSAGE("Error making edge from STL! Ng_result code: " << aRsltCode);
        return false;
    }
    aRsltCode = nglib::Ng_STL_GenerateSurfaceMesh (aNetgenGeom, aNetgenMesh, &mp);
    if ( aRsltCode != nglib::NG_OK) {
        nglib::Ng_DeleteMesh(aNetgenMesh);
        nglib::Ng_Exit();
        MESSAGE("Error Generating surface mesh from STL! Ng_result code: " << aRsltCode);
        return false;
    }
    aRsltCode = nglib::Ng_GenerateVolumeMesh(aNetgenMesh,&mp);
    if ( aRsltCode != nglib::NG_OK) {
        nglib::Ng_DeleteMesh(aNetgenMesh);
        nglib::Ng_Exit();
        MESSAGE("Error Generating volume mesh! Ng_result code: " << aRsltCode);
        return false;
    }

    //get the mesh from netgen mesh, and make a MED mesh
    int NbPoints = nglib::Ng_GetNP(aNetgenMesh);
    int NbElements = nglib::Ng_GetNE(aNetgenMesh);
    vector <double>  aCoordsVec ;
    double  aPoint[3];
    for (int i=1; i <= NbPoints; i++) { //should start from 1
        nglib::Ng_GetPoint(aNetgenMesh, i, aPoint);
        aCoordsVec.push_back(aPoint[0]);
        aCoordsVec.push_back(aPoint[1]);
        aCoordsVec.push_back(aPoint[2]);
    }
    ParaMEDMEM::DataArrayDouble *aCoords=ParaMEDMEM::DataArrayDouble::New();
    aCoords->alloc(NbPoints,3);
    std::copy(aCoordsVec.begin() , aCoordsVec.end() , aCoords->getPointer());

    ParaMEDMEM::MEDCouplingUMesh *aMesh=ParaMEDMEM::MEDCouplingUMesh::New(aPart->getName(),3);
    aMesh->setCoords(aCoords);//set the point list
    aCoords->decrRef();
    aCoordsVec.clear();

    if (NbElements == 0){
        MESSAGE("The mesh "<<aPart->getName()<<"  has no cell!");
        aMesh->decrRef();
        STEPControl_Writer aWriter  ;
        aWriter.Transfer(*aShape, STEPControl_AsIs);
        QString aTmpStepName = QString("tmp_") + QString(aPart->getName()) + QString(".stp");
        aWriter.Write(aTmpStepName.toLatin1());
        nglib::Ng_DeleteMesh(aNetgenMesh);
        nglib::Ng_Exit();
        return false;
    }

    //then copy the connectivities and insert cells
    aMesh->allocateCells(NbElements);

    int     aTetElm[4];
    int atmpArray [4];
    for (int i=1; i< NbElements; i++) {
        nglib::Ng_GetVolumeElement(aNetgenMesh, i, aTetElm);

        //because in connectivities, Netgen start from 1, MED start from 0
        std::transform(aTetElm, aTetElm + 4, atmpArray, op_decrease);
        aMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4, 4,atmpArray );
    }
    aMesh->finishInsertingCells();

    //assign this mesh to the parat
    ParaMEDMEM::MEDCouplingUMeshServant *aMeshServant=new ParaMEDMEM::MEDCouplingUMeshServant(aMesh);
    SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr aMeshPtr = aMeshServant->_this();
    aMesh->decrRef();
    aPart->setMesh(aMeshPtr);
    nglib::Ng_DeleteMesh(aNetgenMesh);
    nglib::Ng_Exit();
    return true;
}


void MCCAD::exportGeom(const MCCAD_ORB::BaseObjList & aBaseObjList, const char* FileName)
{
    if (*FileName == 0) return ;
    if (aBaseObjList.length() == 0) return;

    //obtain the geom and make a compound
    TopoDS_Compound aCompound;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound (aCompound);
    for (int i = 0; i < aBaseObjList.length(); i++)
    {
        if (!CORBA::is_nil(aBaseObjList[i]))
        {
            TopoDS_Shape* aShape = (TopoDS_Shape*)( aBaseObjList[i]->getShapePointer());
            if (aShape != NULL)
                aBuilder.Add (aCompound, *aShape);
        }
    }

    //output the shape into the file
    QString aFileName = QString(FileName);

    QFile aGeomFile (FileName);

    QString aTmpName = aGeomFile.fileName();
    QString aTmpExt =  aTmpName.split(".").last();

//    QStringList aStringList = aFileName.split("/", QString::SkipEmptyParts);
//    QString aTmpName = aStringList[aStringList.size() -1].split(".").at(0); //get the file name without path, only works in Linux
//    QString aTmpExt =  aStringList[aStringList.size() -1].split(".").last();

    if (aTmpExt.isEmpty()) { //if no extention, add one
        aFileName += ".stp";
        aTmpExt = "stp";
    }

    if(aTmpExt == "brep" ){

        BRepTools::Write(aCompound, aFileName.toLatin1());
    }
    else if( aTmpExt == "iges" || aTmpExt == "igs"){
//        new IGESControl_Controller().init(); //??
        IGESControl_Writer aWriter ("MM", 0); //unit : mm
        aWriter.AddShape(aCompound);
        aWriter.ComputeModel();
        aWriter.Write(aFileName.toLatin1());
    }
    else if( aTmpExt == "step" || aTmpExt == "stp"){
        STEPControl_Writer aWriter  ;
        aWriter.Transfer(aCompound, STEPControl_AsIs);
        aWriter.Write(aFileName.toLatin1());
    }
    else {  //other case
        aFileName += ".stp";
        STEPControl_Writer aWriter  ;
        aWriter.Transfer(aCompound, STEPControl_AsIs);
        aWriter.Write(aFileName.toLatin1());
    }
}



//void MCCAD::unittest()
//{
//    if (test())
//        cout <<"Unit Test Failed!!"<<endl;
//}

//############### Component ####################


Component::Component(const char *Name,  CORBA::Long id)
{

    myName = CORBA::string_dup(Name);
    myID = id;
    myList = new MCCAD_ORB::GroupList ();
    myShape = TopoDS_Shape();
    myColor[0] = 255;
    myColor[1] = 255;
    myColor[2] = 0;
    myRemark = CORBA::string_dup("");


}

Component::~Component()
{
    CORBA::string_free(myName);
    CORBA::string_free(myRemark);

    // no need to delete??
            delete myList;
}


///*!
// * \brief get Name
// * \return the name of the Component
// */
//char* Component::getName()
//{
//    //duplicate in case destroy outside
//    return CORBA::string_dup(myName);
//}
///*!
// * \brief set the Component Name
// * \param Name
// */
//void Component::setName(const char *Name)
//{
//    CORBA::string_free(myName);
//    myName = CORBA::string_dup(Name);
//}


/*!
 * \brief set a list of Group into this Component
 * \param Groups a list of group
 * \return \c true if success
 */
CORBA::Boolean Component::setGroupList(const MCCAD_ORB::GroupList & aGroupList)
{
    const int n = aGroupList.length();
    if (n == 0) return false;
    myList->length(0);
    myList->length(n);

    for (int i = 0; i < n; i++ )
        (*myList)[i] = aGroupList[i];

    return true;
}

//MCCAD_ORB::Group_ptr Component::generateGroup(const char* Name)
//{
//    if (Name[0] == 0)
//        THROW_SALOME_CORBA_EXCEPTION("Invalid Group name", SALOME::BAD_PARAM);
//    MESSAGE("Generate Group: " <<Name);
//    Group * aGroup = new Group(Name, genId());
//    return aGroup->_this();
//}

/*!
 * \brief get the group according to the \a id
 * \param id group id,
 * \return Group pointer, a duplicate pointer, should release outside
 */
MCCAD_ORB::Group_ptr Component::getGroup(CORBA::Long GroupID)
{
    const int n = myList->length();

    for (int i = 0; i < n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            MCCAD_ORB::Group_var  aGroup = (*myList)[i];
            if (GroupID == aGroup->getID())
                return aGroup._retn(); //if find the Group, return the pointer
        }
    }
    return MCCAD_ORB::Group::_nil();  //else return NULL
}

/*!
 * \brief Component::getNumParts
 * calculate how many parts
 * \return
 */
CORBA::Long  Component::getNumParts()
{
    const int n = myList->length();
    int count = 0;

    for (int i = 0; i < n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            MCCAD_ORB::Group_var  aGroup = (*myList)[i];
            count += aGroup->getNumParts();
        }
    }
    return count;
}


/*!
 * \brief append a new group into the end of the Group Component
 * \param aGroup group to be append, should take care of release outside
 * \return \c true if is success
 */
CORBA::Boolean Component::appendGroup(MCCAD_ORB::Group_ptr aGroup)
{
    if(aGroup->_is_nil()) return false;
    MESSAGE("Append a group : " << aGroup->getName());
    int n = myList->length() + 1;
    myList->length(n);
    //should use duplicate, other wise  segment fault when aGroup is delete outside
    (*myList)[n-1] = MCCAD_ORB::Group::_duplicate(aGroup);
    return true;
}

/*!
 * \brief delete one group from the list
 *  see the detail design documents for details
 * \param GroupID the group with \a GroupID to be delete
 * \return \c true is succeed
 */
CORBA::Boolean  Component::deleteGroup(CORBA::Long GroupID)
{
    const int n = myList->length();
    int i;
    for (i=0; i<n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            MCCAD_ORB::Group_var  aGroup = (*myList)[i];
            if (GroupID == aGroup->getID())
            {
                for (int j=i; j<n-1; j++)
                {
                    (*myList)[j] = (*myList)[j+1];
                }
                break;//important
            }
        }
    }
    if (i == n)// cannot find the group
        return false;

    myList->length(n-1);  //reduce the lenght therefore delete on item
    return true;
}

/*!
 * \brief insert a Group into the list before the group with \a GroupID
 * \param aGroup the group to be inserted, duplicated, should take care outside for release
 * \param GroupID put the \a aGroup before this group
 * \return \c true if succeed
 */
CORBA::Boolean  Component::insertGroup(MCCAD_ORB::Group_ptr aGroup, CORBA::Long GroupID)
{
    if (aGroup->_is_nil()) return false;
    MESSAGE("Insert a group : " << aGroup->getName());
    const int n = myList->length();
    //if no group in the Component , just append it
    if (n ==0 )
    {
        appendGroup(aGroup);
        return true; //just append
    }
    //if the group with GroupID could not be found, append it
    if (getGroup(GroupID)->_is_nil())
    {
        appendGroup(aGroup);
        return true; //just append
    }
    //normal situation
    myList->length(n+1) ;  //increase the length with one elemetn
    int i;
    for (i=0; i<n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            MCCAD_ORB::Group_var  aGroup = (*myList)[i];
            if (GroupID == aGroup->getID())
            {
                for (int j=n; j>i; j--)
                {
                    (*myList)[j] = (*myList)[j-1]; //see the detail design documents
                }
                break; //important
            }
        }
    }
    if (i == n)// cannot find the group
    {
        myList->length(n);//recover the length if not insert
        return false;
    }

    //2013-03-22 should use duplicate, other wise  segment fault when aGroup is delete outside
    (*myList)[i] = MCCAD_ORB::Group::_duplicate(aGroup);  //copy the group to be insert into the list
    return true;

}
/*!
 * \brief clear all Groups
 * \return
 */
CORBA::Boolean  Component::clearGroups()
{
    myList->length(0);
    return true;
}

/*!
 * \brief append all Groups In another Component
 * \param aComponent
 */
void Component::appendGroupInComponent(MCCAD_ORB::Component_ptr aComponent)
{
    if (aComponent->_is_nil() || aComponent->getGroupList()->length() == 0)
        return;
    const int n = aComponent->getGroupList()->length();
    for (int i=0; i<n; i++)
        this->appendGroup((*aComponent->getGroupList())[i]);
}




/*!
 * \brief get all the shape in this component
 * \return TopoDS_shape, as compound
 */
TopoDS_Shape Component::getShape()
{
    TopoDS_Compound aCompound;
    BRep_Builder aBuilder;
    //make a compound and return
    aBuilder.MakeCompound (aCompound);

    const int n = myList->length();
    for (int i = 0; i < n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            TopoDS_Shape* aShape = (TopoDS_Shape*)( (*myList)[i]->getShapePointer());
            if (aShape != NULL)
                aBuilder.Add (aCompound, *aShape);
        }
    }
    return aCompound; //return compound as a shape
}

void  Component::setColor(const MCCAD_ORB::MColor aColor)
{
    //first copy the color for myself
    myColor[0] = aColor[0];myColor[1] = aColor[1];myColor[2] = aColor[2];

    //set the color for all child
    const int n = myList->length();
    for (int i = 0; i < n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            (*myList)[i]->setColor(aColor);
        }
    }
}

/*!
 * \brief set the shape in the stream file as envelop for the part
 * \param aShape the stream file which include OCC shape
 */
void  Component::setEnvelopStream(const SALOMEDS::TMPFile& aShapeStream)
{
    if(aShapeStream.length() == 0) return; //if emtpy, return;
    myEnvelop = Stream2Shape(aShapeStream);
}

/*!
 * \brief wrap the envelop of the part into a stream
 * \return the shape stream
 */
SALOMEDS::TMPFile*  Component::getEnvelopStream()
{
    if(myEnvelop.IsNull()) return NULL;
    return  Shape2Stream(myEnvelop);
}

/*!
 * \brief return the pointer of the envelop shape
 * \return
 */
CORBA::LongLong Component::getEnvelopPointer()
{
    if (myEnvelop.IsNull()) return NULL;
    return (CORBA::LongLong)(&myEnvelop) ;
}


//void  Component::getColor(MCCAD_ORB::MColor aColor)
//{
//    aColor[0] = myColor[0]; aColor[1] = myColor[1]; aColor[2] = myColor[2];
//}


///*!
// * \brief return the shape as a memory pointer
// * \return
// */
//CORBA::LongLong Component::getShapePointer()
//{
//    myShape = getShape();
//    if (myShape.IsNull()) return NULL;
//    return (CORBA::LongLong)(&myShape) ;
//}

///*!
// * \brief wrap the shape of the part into a stream
// * \return the shape stream
// */
//SALOMEDS::TMPFile*  Component::getShapeStream()
//{
//    TopoDS_Shape aShape = getShape();
//    if (aShape.IsNull()) return NULL;//ok to return NULL?
//    return  Shape2Stream(aShape);
//}

//############### Group ####################


Group::Group(const char *Name,  CORBA::Long id)
{

    myName = CORBA::string_dup(Name);
    myID = id;
    myList = new MCCAD_ORB::PartList ();
    myShape = TopoDS_Shape();
    myColor[0] = 255;
    myColor[1] = 255;
    myColor[2] = 0;
    myMaterial = MCCAD_ORB::Material::_nil();
    myRemark = CORBA::string_dup("");


}

Group::~Group()
{
    CORBA::string_free(myName);
    CORBA::string_free(myRemark);

    // no need to delete?????
            delete myList;
    if (!myMaterial->_is_nil())
        CORBA::release(myMaterial);
}


///*!
// * \brief get Name
// * \return the name of the Group
// */
//char* Group::getName()
//{
//    //duplicate in case destroy outside
//    return CORBA::string_dup(myName);
//}
///*!
// * \brief set the Group Name
// * \param Name
// */
//void Group::setName(const char *Name)
//{
//    CORBA::string_free(myName);
//    myName = CORBA::string_dup(Name);
//}


/*!
 * \brief set a list of Part into this group
 * \param Parts a list of part, this aprt list is just make a copy of reference!
 * \return \c true if success
 */
CORBA::Boolean Group::setPartList(const MCCAD_ORB::PartList & aPartList)
{
    const int n = aPartList.length();
    if (n == 0) return false;
    myList->length(0);
    myList->length(n);

    for (int i = 0; i < n; i++ )
        (*myList)[i] = aPartList[i];

    return true;
}

/*!
 * \brief get Material info
 * \return a duplicate of reference, should be released outside!
 */
MCCAD_ORB::Material_ptr Group::getMaterial()
{
    if (!myMaterial->_is_nil())
        return MCCAD_ORB::Material::_duplicate(myMaterial);
    else return MCCAD_ORB::Material::_nil();
}

CORBA::Long Group::getMaterialID()
{
    if (!myMaterial->_is_nil())
       return myMaterial->getID();
    else return -1;
}

/*!
 * \brief set Material info
 *  the material will be duplicated!
 * \return
 */
void Group::setMaterial(MCCAD_ORB::Material_ptr aMaterial)
{


    if(aMaterial->_is_nil()) return;
    if (!myMaterial->_is_nil())
        CORBA::release(myMaterial); //???
    myMaterial = MCCAD_ORB::Material::_duplicate(aMaterial);  // aMaterial should not be remove reference outside
}


/*!
 * \brief get the part according to the \a id
 * \param id part id
 * \return Part pointer, should be release outside!!
 */
MCCAD_ORB::Part_ptr Group::getPart(CORBA::Long PartID)
{
    const int n = myList->length();

    for (int i = 0; i < n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            MCCAD_ORB::Part_var  aPart = (*myList)[i];
            if (PartID == aPart->getID())
                return aPart._retn(); //if find the Part, return the pointer
        }
    }
    return MCCAD_ORB::Part::_nil();  //else return NULL
}

/*!
 * \brief append a new part into the end of the Part group
 * \param aPart part to be append, the aPart is duplicate, should be release outside!
 * \return \c true if is success
 */
CORBA::Boolean Group::appendPart(MCCAD_ORB::Part_ptr aPart)
{
    if(aPart->_is_nil()) return false;
    MESSAGE("Append a part : " << aPart->getName());
    int n = myList->length() + 1;
    myList->length(n);
    //should use duplicate, other wise  segment fault when aPart is delete outside
    (*myList)[n-1] = MCCAD_ORB::Part::_duplicate(aPart);
    return true;
}

/*!
 * \brief delete one part from the list
 *  see the detail design documents for details
 * \param PartID the part with \a PartID to be delete
 * \return \c true is succeed
 */
CORBA::Boolean  Group::deletePart(CORBA::Long PartID)
{
    const int n = myList->length();
    int i;
    for (i=0; i<n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            MCCAD_ORB::Part_var  aPart = (*myList)[i];
            if (PartID == aPart->getID())
            {
//                CORBA::release((*myList)[i]);//for testing
                for (int j=i; j<n-1; j++)
                {
                    (*myList)[j] = (*myList)[j+1];
                }
                break;//important
            }
        }
    }
    if (i == n)// cannot find the part
        return false;

    myList->length(n-1);  //reduce the lenght therefore delete on item
    return true;
}

/*!
 * \brief insert a Part into the list before the part with \a PartID
 * \param aPart the part to be inserted, it is dupicate, should be released outside
 * \param PartID put the \a aPart before this part
 * \return \c true if succeed
 */
CORBA::Boolean  Group::insertPart(MCCAD_ORB::Part_ptr aPart, CORBA::Long PartID)
{
    if (aPart->_is_nil()) return false;
    MESSAGE("Insert a part : " << aPart->getName());
    const int n = myList->length();
    //if no part in the group , just append it
    if (n ==0 )
    {
        appendPart(aPart);
        return true; //just append
    }
    //if the part with PartID could not be found, append it
    if (getPart(PartID)->_is_nil())
    {
        appendPart(aPart);
        return true; //just append
    }
    //normal situation
    myList->length(n+1) ;  //increase the length with one elemetn
    int i;
    for (i=0; i<n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            MCCAD_ORB::Part_var  aPart = (*myList)[i];
            if (PartID == aPart->getID())
            {
                for (int j=n; j>i; j--)
                {
                    (*myList)[j] = (*myList)[j-1]; //see the detail design documents
                }
                break; //important
            }
        }
    }
    if (i == n)// cannot find the part
    {
        myList->length(n);//recover the length if not insert
        return false;
    }

    //2013-03-22 should use duplicate, other wise  segment fault when aPart is delete outside
    (*myList)[i] = MCCAD_ORB::Part::_duplicate(aPart);  //copy the part to be insert into the list
    return true;

}
/*!
 * \brief clear all Parts
 * \return
 */
CORBA::Boolean  Group::clearParts()
{
    myList->length(0);
    return true;
}

/*!
 * \brief append all Parts In another Group
 * \param aGroup, only the parts are duplicate reference, the group is not take cared
 */
void Group::appendPartInGroup(MCCAD_ORB::Group_ptr aGroup)
{
    if (aGroup->_is_nil() || aGroup->getPartList()->length() == 0)
        return;
    const int n = aGroup->getPartList()->length();
    for (int i=0; i<n; i++)
        this->appendPart((*aGroup->getPartList())[i]);
}

/*!
 * \brief get all the shape in this group
 * \return TopoDS_shape, as compound
 */
TopoDS_Shape Group::getShape()
{
    TopoDS_Compound aCompound;
    BRep_Builder aBuilder;
    //make a compound and return
    aBuilder.MakeCompound (aCompound);

    const int n = myList->length();
    for (int i = 0; i < n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            TopoDS_Shape* aShape = (TopoDS_Shape*)( (*myList)[i]->getShapePointer());
            if (aShape != NULL)
                aBuilder.Add (aCompound, *aShape);
        }
    }
    return aCompound; //return compound as a shape
}

void  Group::setColor(const MCCAD_ORB::MColor aColor)
{
    //first copy the color for myself
    myColor[0] = aColor[0];myColor[1] = aColor[1];myColor[2] = aColor[2];

    //set the color for all child
    const int n = myList->length();
    for (int i = 0; i < n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            (*myList)[i]->setColor(aColor);
        }
    }
}

//void  Group::getColor(MCCAD_ORB::MColor aColor)
//{
//    aColor[0] = myColor[0]; aColor[1] = myColor[1]; aColor[2] = myColor[2];
//}

///*!
// * \brief return the shape as a memory pointer
// * \return
// */
//CORBA::LongLong Group::getShapePointer()
//{
//    myShape = getShape();
//    if (myShape.IsNull()) return NULL;
//    return (CORBA::LongLong)(&myShape) ;
//}

///*!
// * \brief wrap the shape of the part into a stream
// * \return the shape stream
// */
//SALOMEDS::TMPFile*  Group::getShapeStream()
//{
//    TopoDS_Shape aShape = getShape();
//    if (aShape.IsNull()) return NULL;//ok to return NULL?
//    return  Shape2Stream(aShape);
//}


//############### Material ####################
Material::Material(const char * Name, const CORBA::Long id)
{
    myName = CORBA::string_dup(Name);
    myID = id;
    myDensity = 0;
    myMatCard=0;
    myColor[0] = 255;
    myColor[1] = 170;
    myColor[2] = 0;

}

Material::~Material()
{
    CORBA::string_free(myName) ;
    CORBA::string_free(myMatCard) ;
}

void Material::setName(const char *Name)
{

    CORBA::string_free(myName);
    myName = CORBA::string_dup(Name);
}

void Material::setID(CORBA::Long id)
{
    myID = id;
}

void Material::setDensity(CORBA::Double aDensity)
{
    myDensity = aDensity;
}

void Material::setMatCard(const char * MatCard)
{

    CORBA::string_free(myMatCard);
    myMatCard = CORBA::string_dup(MatCard);
}

void  Material:: getColor(MCCAD_ORB::MColor aColor)
{
    aColor[0] = myColor[0];
    aColor[1] = myColor[1];
    aColor[2] = myColor[2];
}

void  Material::setColor(const MCCAD_ORB::MColor aColor)
{
    myColor[0] = aColor[0];
    myColor[1] = aColor[1];
    myColor[2] = aColor[2];
}


//############### Part ####################


Part::Part(const char * Name, const CORBA::Long id)
{
    myName = CORBA::string_dup(Name);
    myRemark = CORBA::string_dup("");
    myShape = TopoDS_Shape();
    myShapeBackup = TopoDS_Shape();
    myIsDecomposed = false; //default no decomposed
    myID = id;
    myMesh = SALOME_MED::MEDCouplingUMeshCorbaInterface::_nil();//initialize with nill mesh
    myColor[0] = 255;
    myColor[1] = 255;
    myColor[2] = 0;
    myImportance[0] = -1;  //-1 using default value
    myImportance[1] = -1;
    myImportance[2] = -1;
    myAdditive = CORBA::string_dup("");

}

Part::~Part()
{
    CORBA::string_free(myName) ;
    CORBA::string_free(myRemark) ;
    CORBA::string_free(myAdditive) ;


    if (!myMesh->_is_nil())
    {
        myMesh->UnRegister();
        CORBA::release(myMesh);
    }
}

/*!
 * \brief Part::recoverShape
 *  recover the myShape to myShapeBackup,
 *  in case of after decomposed, people want to undo the operation
 */
void   Part::recoverShape()
{
    if (!myShapeBackup.IsNull()) {
        myShape = myShapeBackup;
        myShapeBackup = TopoDS_Shape(); //assign a nill shape
        myIsDecomposed = false; //consider the original shape not being decomposed
    }
}

void   Part::setIsDecomposed(CORBA::Boolean  isDecomposed)
{
    myIsDecomposed = isDecomposed;
}

CORBA::Boolean  Part::getIsDecomposed()
{
    return myIsDecomposed ;
}

///*!
// * \brief set the part Name
// * \param Name
// */
//void Part::setName(const char *Name)
//{

//    CORBA::string_free(myName);
//    myName = CORBA::string_dup(Name);
//}

///*!
// * \brief set the description of the part
// * \param aDescription
// */
//void Part::setDescription(const char *aDescription)
//{

//    CORBA::string_free(myDescription);
//    myDescription = CORBA::string_dup(aDescription);
//}


void  Part::setShape(TopoDS_Shape & aShape)
{
    myShapeBackup = myShape;  //backup
    myShape = aShape;
    myIsDecomposed = false;  //if set new shape, consider as not decompsed

}


/*!
 * \brief set the shape in the stream file into the part
 * \param aShape the stream file which include OCC shape
 */
void  Part::setShapeStream(const SALOMEDS::TMPFile& aShapeStream)
{
    if(aShapeStream.length() == 0) return; //if emtpy, return;
    //backup the shape //2013-12-1
    myShapeBackup = myShape; //do this even myShape is nill
    myShape = Stream2Shape(aShapeStream);
    myIsDecomposed = false;  //if set new shape, consider as not decompsed
}

///*!
// * \brief wrap the shape of the part into a stream
// * \return the shape stream
// */
//SALOMEDS::TMPFile*  Part::getShapeStream()
//{
//    if(myShape.IsNull()) return NULL;  //ok to return NULL?
//    return  Shape2Stream(myShape);
//}


/*!
 * \brief set a unstrcutred mesh for the part
 * \param aMesh a MED mesh to be set
 */
void Part::setMesh(SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr aMesh)
{
    if (!myMesh->_is_nil()) {
        myMesh->UnRegister();
        CORBA::release(myMesh);  //2013-11-29
    }

    if(!aMesh->_is_nil())  //if not nil
        myMesh = aMesh;  //directly assign, not copy, thus aMesh should not be destroy outside
//    else  //assign a nil Mesh
//        myMesh = SALOME_MED::MEDCouplingUMeshCorbaInterface::_nil();
}
/*!
 * \brief get the unstructured mesh of the
 *
 * \param aMesh a MED mesh to be set
 */
SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr Part::getMesh()
{
    if (!myMesh->_is_nil())  //if not nil
        return SALOME_MED::MEDCouplingUMeshCorbaInterface::_duplicate(myMesh);
    return SALOME_MED::MEDCouplingUMeshCorbaInterface::_nil();
}

/*!
 * \brief Part::clearMesh clear mesh data
 */
void  Part::clearMesh()
{
    if (!myMesh->_is_nil()) {
        myMesh->UnRegister();
        CORBA::release(myMesh);  //2013-11-29
    }
    myMesh = SALOME_MED::MEDCouplingUMeshCorbaInterface::_nil();
}

/*!
 * \brief Part::setImportance set importance
 * \param aIMP
 */
void            Part::setImportance(const MCCAD_ORB::FixArray aIMP)
{
    myImportance[0] = aIMP[0];
    myImportance[1] = aIMP[1];
    myImportance[2] = aIMP[2];
}

/*!
 * \brief Part::getImportance
 *  return a pointer to a array which include importances
 * \return FixArray_slice, should use FixArray_var to get this return!
 */
MCCAD_ORB::FixArray_slice* Part::getImportance()
{
    MCCAD_ORB::FixArray_slice* aIMP =  MCCAD_ORB::FixArray_alloc();//allocate the array
    aIMP[0] = myImportance[0] ;
    aIMP[1] = myImportance[1] ;
    aIMP[2] = myImportance[2] ;
    return aIMP;
}

char* Part::getAdditive()
{
    return CORBA::string_dup(myAdditive);
}

void  Part::setAdditive(const char* aAdditive)
{
    CORBA::string_free(myAdditive);
    myAdditive = CORBA::string_dup(aAdditive);
}


/*!
 * \brief Part::getVolume
 *  return the volume size of the CAD model
 * \return
 */

CORBA::Double   Part::getVolume()
{
  GProp_GProps aShapeProp;
  BRepGProp::VolumeProperties(myShape, aShapeProp);
  return aShapeProp.Mass();
}

/*!
 * \brief Part::getVolume
 *  return the volume size of the mesh
 * \return
 */

CORBA::Double   Part::getMeshVolume()
{
    double Volume = 0.;

    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDCouplingUMesh> aUMesh =
            ParaMEDMEM::MEDCouplingUMeshClient::New(myMesh);
//    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <MEDCouplingFieldDouble > aVolumeField =
//            aUMesh->getMeasureField(true);
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::DataArrayDouble> aArray =
            aUMesh->getMeasureField(true)->getArray();
    vector <double > aTupe;
    aTupe.resize(1);

    for (int i=0; i<aArray->getNumberOfTuples(); i++)
    {
        aArray->getTuple(i, aTupe.data());
        Volume += aTupe[0];
    }
    return Volume;
}

void  Part::setColor(const MCCAD_ORB::MColor aColor)
{
    myColor[0] = aColor[0];myColor[1] = aColor[1];myColor[2] = aColor[2];
}

//void  Part::getColor(MCCAD_ORB::MColor aColor)
//{
//    aColor[0] = myColor[0];    aColor[1] = myColor[1];    aColor[2] = myColor[2];
//}

CORBA::Boolean  Part::isExplodible()
{
    if( getShape().ShapeType() == TopAbs_COMPSOLID || getShape().ShapeType() == TopAbs_COMPOUND )
        return true;
    else
        return false;
}


/*!
 * \brief set the BaseObj Name
 * \param Name
 */
void BaseObj::setName(const char *Name)
{

    CORBA::string_free(myName);
    myName = CORBA::string_dup(Name);
}

/*!
 * \brief set the Remark of the BaseObj
 * \param aRemark
 */
void BaseObj::setRemark(const char *aRemark)
{

    CORBA::string_free(myRemark);
    myRemark = CORBA::string_dup(aRemark);
}

/*!
 * \brief BaseObj::getShape virtual method to avoid salome error
 * \return
 */
TopoDS_Shape BaseObj::getShape()
{
    return TopoDS_Shape(); //return a nil shape
}

/*!
 * \brief return the shape as a memory pointer
 * \return
 */
CORBA::LongLong BaseObj::getShapePointer()
{
    myShape = getShape();
    if (myShape.IsNull()) return NULL;
    return (CORBA::LongLong)(&myShape) ;
}

/*!
 * \brief wrap the shape of the part into a stream
 * \return the shape stream
 */
SALOMEDS::TMPFile*  BaseObj::getShapeStream()
{
    TopoDS_Shape aShape = getShape();
    if (aShape.IsNull()) return NULL;//ok to return NULL?
    return  Shape2Stream(aShape);
}

void  BaseObj::getColor(MCCAD_ORB::MColor aColor)
{
    aColor[0] = myColor[0];    aColor[1] = myColor[1];    aColor[2] = myColor[2];
}


extern "C"
{
Standard_EXPORT PortableServer::ObjectId * MCCADEngine_factory(
        CORBA::ORB_ptr orb,
        PortableServer::POA_ptr poa,
        PortableServer::ObjectId * contId,
        const char *instanceName,
        const char *interfaceName)
{
    cout << "PortableServer::ObjectId * MCCADEngine_factory()"<<endl;
    MESSAGE("PortableServer::ObjectId * MCCADEngine_factory()");
    SCRUTE(interfaceName);
    MCCAD * myMCCAD
            = new MCCAD(orb, poa, contId, instanceName, interfaceName);
    return myMCCAD->getId() ;
}
}
