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


#include "MCCADGUI_DataModel.h"
//#include "MCCADGUI_DataObject.h"
//#include "MCCADGUI.h"
//SALOME includes
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SALOME_LifeCycleCORBA.hxx>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_DataObjectIterator.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Tools.h>
#include <LightApp_Study.h>
#include <LightApp_DataOwner.h>
#include <CAM_Module.h>
#include <CAM_Application.h>
#include "utilities.h"
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(MEDCouplingCorbaServant)
#include CORBA_CLIENT_HEADER(GEOM_Gen)
#include CORBA_CLIENT_HEADER(SMESH_Gen)
#include CORBA_CLIENT_HEADER(MCCAD)
#include <SMESH_Client.hxx>
#include <SMESHGUI.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

//QT includes
#include <QFile>
#include <string>
#include <QString>
#include <QDir>
#include <QtXml/QDomComment>
#include <QTextStream>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

//MEDCoupling
#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingMeshServant.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingUMeshServant.hxx"
#include "MEDCouplingUMeshClient.hxx"
#include "MEDCouplingAutoRefCountObjectPtr.hxx"

//MCCAD and OCC
#include "../MCCAD/McCadTool/McCadCSGTool_SurfaceChecker.hxx"
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <STEPControl_Reader.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include "../MCCAD/McCadTool/McCadConvertConfig.hxx"
#include "S_McCadVoidCellManager.hxx"
#include "S_McCadMcnpWriter.hxx"
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax1.hxx>
#include <gp_Trsf.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include "MCMESHTRAN_AbaqusMeshReader.hxx"
#include <time.h>

/*! Constructor */
MCCADGUI_DataModel::MCCADGUI_DataModel( CAM_Module* module )
  : LightApp_DataModel( module ),
    myStudyURL("")
{
    PasteParts  = new MCCAD_ORB::PartList;
    PasteGroups  = new MCCAD_ORB::GroupList;
}

/*! Destructor */
MCCADGUI_DataModel::~MCCADGUI_DataModel()
{
    PasteParts->length(0);
    PasteGroups->length(0);
}

/*!
  \brief Open data model (read data from the files). This method is load
  when a saved study is loaded
  \param url study file path
  \param study study pointer
  \param listOfFiles list of the (temporary) files with data
  \return operation status (\c true on success and \c false on error)
*/
bool MCCADGUI_DataModel::open( const QString& url, CAM_Study* study, QStringList listOfFiles )
{
    LightApp_Study* aDoc = dynamic_cast<LightApp_Study*>( study );
    if ( !aDoc )
        return false;

    LightApp_DataModel::open( url, aDoc, listOfFiles );

    return true;
}

/*!
  \brief Save data model (write data to the files).
  \param listOfFiles returning list of the (temporary) files with saved data
  \return operation status (\c true on success and \c false on error)
*/
bool MCCADGUI_DataModel::save( QStringList& theListOfFiles )
{
//    bool isMultiFile = true;

    LightApp_DataModel::save( theListOfFiles );
    return true;
}

/*!
  \brief Save data model (write data to the files).
  \param url study file path
  \param study study pointer
  \param listOfFiles returning list of the (temporary) files with saved data
  \return operation status (\c true on success and \c false on error)
*/
bool MCCADGUI_DataModel::saveAs( const QString& url, CAM_Study* study, QStringList& theListOfFiles )
{
    myStudyURL = url;
    return save( theListOfFiles );
}

/*!
  \brief Close data model (remove all relevant data).
  \return operation status (\c true on success and \c false on error)
*/
bool MCCADGUI_DataModel::close()
{
    return LightApp_DataModel::close();
}

/*!
  \brief Create empty data model.
  \param study study pointer
  \return operation status (\c true on success and \c false on error)
*/
bool MCCADGUI_DataModel::create( CAM_Study* study )
{
    return true;
}

/*!
  \brief Get 'modified' flag.
  \return \c true if data is changed from the last saving
*/
bool MCCADGUI_DataModel::isModified() const
{
    // return false to avoid masking study's isModified()
    return false;
}

/*!
  \brief Get 'saved' flag.
  \return \c true if data has been saved
*/
bool MCCADGUI_DataModel::isSaved() const
{
    // return true to avoid masking study's isSaved()
    return true;
}

/*!
  \brief Update data model.
  \param obj data object (starting for the update)
  \param study study pointer
*/
void MCCADGUI_DataModel::update( LightApp_DataObject* /*obj*/, LightApp_Study* /*study*/ )
{
    // Nothing to do here: we always keep the data tree in the up-to-date state
    // The only goal of this method is to hide default behavior from LightApp_DataModel
    return;
}

bool MCCADGUI_DataModel::generateComponent(const QString&  aComponentName)
{
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        //generate a component inside the engine
        MCCAD_ORB::ComponentList * aComponentList = engine->getData(studyID);
        if (!aComponentList)
        {
            MESSAGE("Component list is emtpy");
            return false;
        }
        aComponentList->length(aComponentList->length()+1);// increase a space for new component
        MCCAD_ORB::Component_var aComponent = engine->generateComponent(aComponentName.toLatin1());
        (*aComponentList)[aComponentList->length()-1] = aComponent;
        return true;
     }
    return false;
}

bool MCCADGUI_DataModel::generateGroup(const QString&  aGroupName, const int & ComponentID)
{
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        //generate a group
        MCCAD_ORB::Group_var aGroup = engine->generateGroup(aGroupName.toLatin1());
        MCCAD_ORB::Component_var aComponent = engine->getComponent(studyID, ComponentID);  //2013-11-29, take care of the release
        if (!aComponent->_is_nil())
        {
            aComponent->appendGroup(aGroup._retn()) ;
            return true;
        }
        else {
            MESSAGE("Cannot find the Component to append the group");
            return false;
        }
     }
    return false;
}

int  MCCADGUI_DataModel::getStudyID()
{
//    LightApp_Study* study = getStudy();
    LightApp_Study* study = dynamic_cast<LightApp_Study*>(
                module()->application()->activeStudy() ); //obtain the current study
    if (study)
        return study->id();
    else
        return 0;
}
/*!
 * \brief get the component id from the entry
 * \param aEntry
 * \return component id
 */
int  MCCADGUI_DataModel::getComponentID(const QString&  aEntry )
{
    QString aIdStr = aEntry.split("_").at(1); //
    if (aIdStr == "root")
        return -1;   //not a group id

    bool  isOK;
    int aId = aIdStr.toInt(&isOK);
    if (!isOK)
        return -1;
    return aId;
}

/*!
 * \brief get the group id from the entry
 * \param aEntry
 * \return group id
 */
int  MCCADGUI_DataModel::getGroupID(const QString&  aEntry )
{
    if (aEntry.split("_").size() < 3)
        return -1;

    bool  isOK;
    int aId =  aEntry.split("_").at(2).toInt(&isOK);
    if (!isOK)
        return -1;
    return aId;
}
/*!
 * \brief get the part id from the entry
 * \param aEntry
 * \return part id
 */
int  MCCADGUI_DataModel::getPartID (const QString&  aEntry )
{

    QStringList aStrList = aEntry.split("_");
    if (aStrList.size() < 4)  //has no part id
        return -1;  //indicate a group or the root

    bool  isOK;
    int aId = aStrList.at(3).toInt(& isOK);
    if (!isOK)
        return -1;
    return aId;
}

/*!
 * \brief convert the Shape stream to shape
 * \param aShapeStream  the octet stream with OCC shape
 * \param theShape OCC shape
 */
TopoDS_Shape MCCADGUI_DataModel::Stream2Shape(const SALOMEDS::TMPFile& aShapeStream)
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
SALOMEDS::TMPFile* MCCADGUI_DataModel::Shape2Stream(const TopoDS_Shape& aShape)
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

/*!
 * \brief return the last component for appending group
 * \return  the last Component
 */
MCCADGUI_DataObject * MCCADGUI_DataModel::lastComponent ()
{
//    for ( SUIT_DataObjectIterator it( root(), SUIT_DataObjectIterator::DepthRight ); it.current(); it-- )
//    {
//        MCCADGUI_DataObject* obj = dynamic_cast<MCCADGUI_DataObject*>( it.current() );
//        if (obj->isComponent()) return obj;
//    }
//    return 0;
    MCCADGUI_ModuleObject* modelRoot = dynamic_cast<MCCADGUI_ModuleObject*>( root() );
    if( !modelRoot )  {  // root is not set yet
        modelRoot = new MCCADGUI_ModuleObject( this, 0 );
        setRoot( modelRoot );
    }

    if (!modelRoot->lastChild()) return 0;
    MCCADGUI_DataObject * aObj = dynamic_cast <MCCADGUI_DataObject *> (modelRoot->lastChild());
    if (aObj->isComponent() ) {
        return aObj;
    }
    else if (aObj->isGroup()) {
        return dynamic_cast <MCCADGUI_DataObject *> (aObj->parent());
    }
    else if (aObj->isPart()) {
        return dynamic_cast <MCCADGUI_DataObject *> (aObj->parent()->parent());
    }
    else return 0;

}

/*!
 * \brief return the last group for appending parts
 * \return the last group
 */
MCCADGUI_DataObject * MCCADGUI_DataModel::lastGroup()
{
//    for ( SUIT_DataObjectIterator it( root(), SUIT_DataObjectIterator::DepthRight ); it.current(); it-- )
//    {
//        MCCADGUI_DataObject* obj = dynamic_cast<MCCADGUI_DataObject*>( it.current() );
//        if (obj->isGroup()) return obj;
//    }
    return 0;

    /*
    MCCADGUI_DataObject * aObj = dynamic_cast <MCCADGUI_DataObject *> (root()->lastChild());
    if (aObj->isGroup()) {
        return dynamic_cast <MCCADGUI_DataObject *> (aObj->parent());
    }
    else if (aObj->isPart()) {
        return dynamic_cast <MCCADGUI_DataObject *> (aObj->parent()->parent());
    }
    else return 0;
    */
}


/*!
 * \brief find the data object according to the entry
 * \param aEntry
 * \return data object
 */
MCCADGUI_DataObject * MCCADGUI_DataModel::findObject(const QString&  aEntry)
{
    for ( SUIT_DataObjectIterator it( root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
    {
        MCCADGUI_DataObject* obj = dynamic_cast<MCCADGUI_DataObject*>( it.current() );
        if (obj && obj->entry() == aEntry)
            return obj;
    }
    return 0;
}

/*!
 * \brief get the SALOME_InteractiveOject of this Object
 * \param aEntry
 * \return the SALOME_InteractiveOject representing this object
 */
Handle(SALOME_InteractiveObject) MCCADGUI_DataModel::findObjectIO(const QString&  aEntry)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    Handle( SALOME_InteractiveObject ) anIO;
    anIO = new SALOME_InteractiveObject( aObj->entry().toLatin1(), "MCCAD", aObj->name().toLatin1() );
    return anIO;
}


void  MCCADGUI_DataModel::deleteComponent(const QString& aEntry)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() || !aObj || !aObj->isComponent())
    {
        MESSAGE("Error in Entry");
        return;
    }
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        if (!engine->deleteComponent(studyID, getComponentID(aEntry)))
        {
            MESSAGE("Delete group failed!");
        }
        else        //remove the object
            aObj->parent()->removeChild(aObj);
        return;
    }
    MESSAGE("Error in study or engine!");
}

void  MCCADGUI_DataModel::deleteComponents(const QStringList& aList)
{
    if (aList.size() == 0)
    {
        MESSAGE("No group to be deleted");
        return;
    }
    for (int i=0; i<aList.size(); i++)
        deleteComponent(aList.at(i));
}

void MCCADGUI_DataModel::renameComponent(const QString& aEntry, const QString& newName)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() ||  newName.trimmed().isEmpty() ||
            !aObj || !aObj->isComponent())
    {
        MESSAGE("Error in Entry or name!");
        return;
    }
    MCCAD_ORB::Component_var aComponent = aObj->getComponent();//2013-11-29, take care of the release
    aComponent->setName(newName.toLatin1());
}

void  MCCADGUI_DataModel::deleteGroup(const QString& aEntry)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() || !aObj || !aObj->isGroup())
    {
        MESSAGE("Error in Entry");
        return;
    }
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        MCCAD_ORB::Component_var aComponent = aObj->getComponent();//2013-11-29, take care of the release
        if (!aComponent->deleteGroup(getGroupID(aEntry))) {
            MESSAGE("Delete group failed!");
        }
        //remove the object
        else
            aObj->parent()->removeChild(aObj);
        return;
    }
    MESSAGE("Error in study or engine!");
}

void  MCCADGUI_DataModel::deleteGroups(const QStringList& aList)
{
    if (aList.size() == 0)
    {
        MESSAGE("No group to be deleted");
        return;
    }
    for (int i=0; i<aList.size(); i++)
        deleteGroup(aList.at(i));
}

void MCCADGUI_DataModel::renameGroup(const QString& aEntry, const QString& newName)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() ||  newName.trimmed().isEmpty() ||
            !aObj || !aObj->isGroup())
    {
        MESSAGE("Error in Entry or name!");
        return;
    }
    MCCAD_ORB::Group_var aGroup = aObj->getGroup();//2013-11-29, take care of the release
    aGroup->setName(newName.toLatin1());
}

void  MCCADGUI_DataModel::deletePart (const QString& aEntry)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() || !aObj || !aObj->isPart())
    {
        MESSAGE("Error in Entry");
        return;
    }
    MCCAD_ORB::Group_var aGroup = aObj->getGroup();//2013-11-29, take care of the release
    if(!aGroup->deletePart( getPartID(aEntry )))
        MESSAGE("Delete part failed!");
    //remove the object from the tree
    aObj->parent()->removeChild(aObj);

}

void  MCCADGUI_DataModel::deleteParts (const QStringList& aList)
{
    if (aList.size() == 0)
    {
        MESSAGE("No part to be deleted");
        return;
    }
    for (int i=0; i<aList.size(); i++)
        deletePart(aList.at(i));
}

void MCCADGUI_DataModel::renamePart(const QString& aEntry, const QString& newName)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() ||  newName.trimmed().isEmpty() ||
            !aObj || !aObj->isPart())
    {
        MESSAGE("Error in Entry or Name!");
        return;
    }
    MCCAD_ORB::Part_var aPart = aObj->getPart();
    aPart->setName(newName.toLatin1());
}

void  MCCADGUI_DataModel::copyPart(const QString& aEntry)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() ||  !aObj || !aObj->isPart())
    {
        MESSAGE("Error in Entry !");
        return;
    }
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if (!CORBA::is_nil(engine))
    {
        try
        {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            PasteParts->length(PasteParts->length()+1); //increase a space
            PasteParts[PasteParts->length()-1]  =
                    engine->deepCopyPart((aPart));
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Copy failed: " << ex.details.text)  ;
        }
        return;
    }
    MESSAGE("Error in engine !");
}

void  MCCADGUI_DataModel::copyParts (const QStringList& aList, const bool & emptyList)
{
    if (emptyList)
        PasteParts->length(0);  //clear
    if (aList.size() == 0)
    {
        MESSAGE("No part to be copy");
        return;
    }
    for (int i=0; i<aList.size(); i++)
        copyPart(aList.at(i));
}

void   MCCADGUI_DataModel::cutPart(const QString& aEntry)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() ||  !aObj || !aObj->isPart())
    {
        MESSAGE("Error in Entry !");
        return;
    }
    PasteParts->length(PasteParts->length()+1); //increase a space
    PasteParts[PasteParts->length()-1]  = aObj->getPart();

    deletePart(aEntry);  //delete after cut
}

void   MCCADGUI_DataModel::cutParts(const QStringList& aList, const bool &emptyList)
{
    if (emptyList)
        PasteParts->length(0);  //clear
    if (aList.size() == 0)
    {
        MESSAGE("No part to be cut");
        return;
    }
    for (int i=0; i<aList.size(); i++)
        cutPart(aList.at(i));
}

//paste part before aEntry
void  MCCADGUI_DataModel::pasteParts(const QString& aEntry)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() ||  !aObj )
    {
        MESSAGE("Error in Entry !");
        return;
    }
    if (aObj ->isGroup())
    {
        for (int i=0; i<PasteParts->length(); i++) {
            MCCAD_ORB::Group_var aGroup = aObj->getGroup();
            aGroup->appendPart(PasteParts[i]);
        }
        PasteParts->length(0);
    }
    else if(aObj->isPart())
    {
        for (int i=0; i<PasteParts->length(); i++) {
            MCCAD_ORB::Group_var aGroup = aObj->getGroup();
            aGroup->insertPart(PasteParts[i], aObj->getPartID());
        }
        PasteParts->length(0);
    }
    else
        MESSAGE("Unable to paste here!");
}

void  MCCADGUI_DataModel::copyGroup(const QString& aEntry)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() ||  !aObj || !aObj->isGroup())
    {
        MESSAGE("Error in Entry !");
        return;
    }
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    if (!CORBA::is_nil(engine))
    {
        try
        {
            MCCAD_ORB::Group_var aGroup = aObj->getGroup();
            PasteGroups->length(PasteGroups->length()+1); //increase a space
            PasteGroups[PasteGroups->length()-1]  =
                    engine->deepCopyGroup(aGroup);  //try to copy the reference
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Copy failed: " << ex.details.text)  ;
        }
        return;
    }
    MESSAGE("Error in engine !");
}

void  MCCADGUI_DataModel::copyGroups (const QStringList& aList, const bool & emptyList)
{
    if (emptyList)
        PasteGroups->length(0);  //clear
    if (aList.size() == 0)
    {
        MESSAGE("No Group to be copy");
        return;
    }
    for (int i=0; i<aList.size(); i++)
        copyGroup(aList.at(i));
}

void   MCCADGUI_DataModel::cutGroup(const QString& aEntry)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() ||  !aObj || !aObj->isGroup())
    {
        MESSAGE("Error in Entry !");
        return;
    }
    PasteGroups->length(PasteGroups->length()+1); //increase a space
    PasteGroups[PasteGroups->length()-1]  = aObj->getGroup();

    deleteGroup(aEntry);  //delete after cut
}

void   MCCADGUI_DataModel::cutGroups(const QStringList& aList, const bool &emptyList)
{
    if (emptyList)
        PasteGroups->length(0);  //clear
    if (aList.size() == 0)
    {
        MESSAGE("No Group to be cut");
        return;
    }
    for (int i=0; i<aList.size(); i++)
        cutGroup(aList.at(i));
}

//paste Group before aEntry
void  MCCADGUI_DataModel::pasteGroups(const QString& aEntry)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() ||  !aObj )
    {
        MESSAGE("Error in Entry !");
        return;
    }
    if (aObj ->isComponent())
    {
        for (int i=0; i<PasteGroups->length(); i++) {
            MCCAD_ORB::Component_var aComponent = aObj->getComponent();
            aComponent->appendGroup(PasteGroups[i]);
        }
        PasteGroups->length(0);
    }
    else if(aObj->isGroup())
    {
        for (int i=0; i<PasteGroups->length(); i++) {
            MCCAD_ORB::Component_var aComponent = aObj->getComponent();
            aComponent->insertGroup(PasteGroups[i], aObj->getGroupID());
        }
        PasteGroups->length(0);
    }
    else
        MESSAGE("Unable to paste here!");
}


/*!
 * \brief MCCADGUI_DataModel::generateMaterial
 *  generate new material and append to the study material list
 * \param MaterialID unique id for this material
 * \param Name material name
 * \param Density  atom density (positive double) or mass density (nagative double)
 * \param MaterialCard material  description in MCNP or TRIPOLI format
 * \return false if error
 */
bool  MCCADGUI_DataModel::generateMaterial(const int & MaterialID, const QString & Name, const double & Density, const QString & MaterialCard, const QColor & aColor)
{
    //check the input
    if (MaterialID < 1) return false;
    if (Name.trimmed().isEmpty()) return false;

    //get the engine and study
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }
    MCCAD_ORB::Material_var aMaterial;
    try {
        aMaterial = engine->generateMaterial(Name.toLatin1(), MaterialID, studyID);
    }
    catch (SALOME::SALOME_Exception &ex)    {
        MESSAGE("generate failed: " << ex.details.text)  ;
        return false;
    }

    if (aMaterial->_is_nil() ) return false;
    aMaterial->setDensity(Density);
    aMaterial->setMatCard(MaterialCard.toLatin1());
    if (aColor.isValid()) {
        MCCAD_ORB::MColor aMColor;
        aMColor[0] = aColor.red();
        aMColor[1] = aColor.green();
        aMColor[2] = aColor.blue();
        aMaterial->setColor(aMColor);
    }
//    aMaterial->setColor(); //to be implement
    //append the new material
    engine->appendMaterial(studyID, aMaterial._retn());
    return true;
}


/*!
 * \brief MCCADGUI_DataModel::updateMaterial
 *  update the material with new parameters
 * \param MaterialID the material to be modify
 * \param newMaterialID new id for this material
 * \param Name material name
 * \param Density  atom density (positive double) or mass density (nagative double)
 * \param MaterialCard MaterialCard material  description in MCNP or TRIPOLI format
 * \return false if error
 */
bool    MCCADGUI_DataModel::updateMaterial(const int & MaterialID, const int & newMaterialID,const QString & Name,
                                           const double & Density, const QString & MaterialCard, const QColor & aColor)
{
    //check the input
    if (MaterialID < 1) return false;
    if (Name.trimmed().isEmpty()) return false;

    //get the engine and study
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }

    //get the original material, and check the newID if ok
    MCCAD_ORB::Material_var aMaterial = engine->getMaterial(studyID, MaterialID);
    if (aMaterial->_is_nil()) {
        MESSAGE("Could not find the material in the list!");
        return false;
    }
    MCCAD_ORB::Material_var bMaterial = engine->getMaterial(studyID, newMaterialID);
    if (MaterialID != newMaterialID && !bMaterial->_is_nil()) {
        MESSAGE("Material with this ID is already exist!");
        return false;
    }

    //assign new parameters
    aMaterial->setID(newMaterialID);
    aMaterial->setName(Name.toLatin1());
    aMaterial->setDensity(Density);
    aMaterial->setMatCard(MaterialCard.toLatin1());
    if (aColor.isValid()) { //because the default color of this function is QColor(), which is not invalid
        MCCAD_ORB::MColor aMColor;
        aMColor[0] = aColor.red();
        aMColor[1] = aColor.green();
        aMColor[2] = aColor.blue();
        aMaterial->setColor(aMColor);
    }
    return true;
}

bool  MCCADGUI_DataModel::deleteMaterial(const int & MaterialID)
{
    //get the engine and study
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }
    return engine->deleteMaterial(studyID, MaterialID);
}


/*!
 * \brief MCCADGUI_DataModel::assignMaterial
 * assign the material property for groups
 * \param aList a list of entry link to the groups
 * \param MaterialID the material to be assign
 * \param isReColor if true, the group and parts belonging to it will be recolor
 * \return false if error
 */
bool MCCADGUI_DataModel::assignMaterial (const QStringList& aList, const int & MaterialID, const bool & isReColor)
{
    if (aList.size() == 0) {
        MESSAGE("No group in this list");
        return false;
    }

    //get the engine and study
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }
    //get the  material, and check the newID if ok
    MCCAD_ORB::Material_var aMaterial = engine->getMaterial(studyID, MaterialID);
    if (aMaterial->_is_nil()) {
        MESSAGE("Could not find the material in the list!");
        return false;
    }

    for (int  i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (aObj->isGroup()) {
            MCCAD_ORB::Group_var aGroup = aObj->getGroup();
            if (isReColor) {  //do this because assign material, because aMaterial will be release later
                MCCAD_ORB::MColor aColor ;
                aMaterial->getColor(aColor);
                aGroup->setColor(aColor);
            }
            aGroup->setMaterial(aMaterial); // do not use the _retn() method because the aMaterial is duplicated inside
        }
    }
    return true;
}

bool  MCCADGUI_DataModel::saveMaterial2Xml(const QString &FileName)
{
    if (FileName.isEmpty() ) {
        MESSAGE ("Filename is Empty");
        return false;
    }

    QFile *file = new QFile(FileName);
    if(!file->open(QFile::WriteOnly|QFile::Truncate))
    {
        return false;
    }
    //obtain engine and study id
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) ) {
        MESSAGE("Errors in study or in engine!");
        return false;
    }
    MCCAD_ORB::MaterialList * aMaterialList = engine->getMaterialList(studyID); //don't delete!
    int MatCnt = aMaterialList->length();
    if (!MatCnt){
        MESSAGE("No material data!");
        return false;
    }

    QDomDocument domDoc;
    QDomProcessingInstruction instruction;
    instruction = domDoc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    domDoc.appendChild(instruction);

    QDomElement prntItem=domDoc.createElement(("Material"));
    domDoc.appendChild(prntItem);

    //Output the Material Info
    QDomElement materialItem = domDoc.createElement(("MaterialMap"));


    for(int i = 0; i < MatCnt; i++)    {

        MCCAD_ORB::Material_var aMaterial = (*aMaterialList)[i];

        QDomElement materialIDNode = domDoc.createElement("MaterialID");
        materialIDNode.setAttribute("number",QString::number(i+1)); //here MaterialID is a sequential integer, not aMaterial->getID()

        QDomElement idNode =  domDoc.createElement("id");
        QDomElement nameNode =  domDoc.createElement("name");
        QDomElement densityNode =  domDoc.createElement("density");
        QDomElement mcardNode =  domDoc.createElement("card");

        QDomText txtID = domDoc.createTextNode(QString::number(aMaterial->getID()));
        QDomText txtName = domDoc.createTextNode(aMaterial->getName());
        QDomText txtDensity = domDoc.createTextNode(QString::number(aMaterial->getDensity()));
        QDomText txtMatcard = domDoc.createTextNode(aMaterial->getMatCard());

        idNode.appendChild(txtID);
        nameNode.appendChild(txtName);
        densityNode.appendChild(txtDensity);
        mcardNode.appendChild(txtMatcard);

        materialIDNode.appendChild(idNode);
        materialIDNode.appendChild(nameNode);
        materialIDNode.appendChild(densityNode);
        materialIDNode.appendChild(mcardNode);

        materialItem.appendChild(materialIDNode);
    }

    prntItem.appendChild(materialItem);


    QTextStream out(file);
    domDoc.save(out,4);
    file->close();
    return true;
}

bool  MCCADGUI_DataModel::loadMaterialXml(const QString &FileName)
{
    //obtain engine and study id
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) ) {
        MESSAGE("Errors in study or in engine!");
        return false;
    }

    QFile file(FileName);
    if(!file.open(QFile::ReadOnly))
    {
        return false;
    }

    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        file.close();
        return false;
    }

    // Store the mateirals map
    QDomNodeList theList = doc.elementsByTagName("MaterialMap");
    bool isOK = false;
    if (theList.size() != 0)
    {

        QDomNodeList materialList = theList.at(0).childNodes();

        bool yestoall = false; // for interactive
        bool notoall = false;// for interactive
        QMessageBox::StandardButton aBtn ;
        isOK = true; // default think all is ok
        for(int i = 0; i < materialList.size(); i++)
        {
            QDomNode materialNode = materialList.at(i).firstChild();

            int iID = 0;                // Serienumer of aterial
            double fDensity = 0;        // Density of material
            QString strName = "";       // Material name
            QString strMatCard = "";    // Material card includes the composition

            while(!materialNode.isNull())
            {
                QDomElement materialElem = materialNode.toElement(); // convert the node to an element.
                if(!materialElem.isNull())
                {
                    if (materialElem.tagName() == "id")
                    {
                        iID = materialElem.text().toInt();
                    }
                    else if(materialElem.tagName() == "name")
                    {
                        strName = materialElem.text();
                    }
                    else if(materialElem.tagName() == "density")
                    {
                        fDensity = materialElem.text().toDouble();
                    }
                    else if(materialElem.tagName() == "card")
                    {
                        strMatCard = materialElem.text();
                    }
                }
                materialNode = materialNode.nextSibling();
            }

            //check if material ID repeat
            MCCAD_ORB::Material_var aMaterial = engine->getMaterial(studyID, iID);
            bool YesOrNo = false; //default not override
            if (!aMaterial->_is_nil())
            {
                if (!yestoall && !notoall){
                    aBtn  = SUIT_MessageBox::question(getMCCADGUImodule()->application()->desktop(),
                                                      QString("Question"),
                                                      QString( "Material %1 is already exist, Override it?").arg(iID),
                                                      QMessageBox::Yes| QMessageBox::YesToAll| QMessageBox::No| QMessageBox::NoToAll);
                    if (aBtn == QMessageBox::Yes)
                        YesOrNo = true;
                    else if (aBtn == QMessageBox::YesToAll)
                        yestoall = true;
                    else if (aBtn == QMessageBox::NoToAll)
                        notoall = true;
                }
                if (YesOrNo || yestoall) {
                    updateMaterial(iID, iID, strName, fDensity,strMatCard); //override
                }
                else
                    continue;  //not override
            }
            else { //create new material
                if (!generateMaterial(iID, strName, fDensity,strMatCard))
                    isOK = false;
            }
        }
    }
    return isOK;
}


/*!
 * \brief MCCADGUI_DataModel::formNewGroup
 *  form selected part as a new group
 * \param aList a list of entries of parts
 * \param aName name for the new group
 * \return false if error
 */
bool  MCCADGUI_DataModel::formNewGroup(const QStringList& aList,  const QString &aName)
{
    if (aList.isEmpty()) return false;
    if (aName.trimmed().isEmpty()) {
        MESSAGE("Name should not be empty!");
        return false;
    }
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }
    MCCAD_ORB::Group_var aNewGroup = engine->generateGroup(aName.trimmed().toLatin1());
    // append to a component
    MCCAD_ORB::Component_var aComponent = findObject(aList[0])->getComponent();
    if (aComponent->_is_nil()) {
        MESSAGE("Cannot append the new group to the component!");
        return false;
    }

    //loop for all objects
    for (int i=0; i<aList.size() ; i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj || !aObj->isPart()) continue;
        MCCAD_ORB::Part_var aPart = aObj->getPart();
        if (!aPart->_is_nil() ) {
            aNewGroup->appendPart(aPart); //the part reference is duplicate inside
            deletePart(aList[i]); //delete the part reference,
        }
    }
    //check the new group
    if (aNewGroup->getPartList()->length() == 0) {
        MESSAGE("No part in the new group!");
        return false;
    }

    aComponent->appendGroup(aNewGroup); //duplicated
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::formNewComponent
 *  form selected group as a new component
 * \param aList a list of entries of groups
 * \param aName name for the new component
 * \return false if error
 */
bool  MCCADGUI_DataModel::formNewComponent(const QStringList& aList,  const QString &aName)
{
    if (aList.isEmpty()) return false;
    if (aName.trimmed().isEmpty()) {
        MESSAGE("Name should not be empty!");
        return false;
    }
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }
    MCCAD_ORB::Component_var aNewComponent = engine->generateComponent(aName.trimmed().toLatin1());
    //loop for all objects
    for (int i=0; i<aList.size() ; i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj || !aObj->isGroup()) continue;
        MCCAD_ORB::Group_var aGroup = aObj->getGroup();
        if (!aGroup->_is_nil() ) {
            aNewComponent->appendGroup(aGroup); //the Group reference is duplicate inside
            deleteGroup(aList[i]); //delete the Group reference,
        }
    }
    //check the new group
    if (aNewComponent->getGroupList()->length() == 0) {
        MESSAGE("No part in the new group!");
        return false;
    }
    engine->appendComponent(studyID, aNewComponent); //duplicated
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::explodePart
 *  explode a part with compound to parts with a single solid;
 *  them will be insert to the group which the part belongs to
 * \param aEntry a part to be explode
 * \return false if error
 */
bool MCCADGUI_DataModel::explodePart(const QString& aEntry)
{
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if (!aObj || !aObj->isPart()) {
        MESSAGE("Cannot find the object, or it is not a Part!");
        return false;
    }
    MCCAD_ORB::Part_var aPart = aObj->getPart();
    if (aPart->_is_nil()) {
        MESSAGE("The part is nil!");
        return false;
    }
    if (!aPart->isExplodible()) {
        MESSAGE("The part is not explodible!");
        return false;
    }
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    MCCAD_ORB::Group_var aGroup = aObj->getGroup();
    MCCAD_ORB::PartList_var aPartList;
    try {
        aPartList= engine->explodePart(aPart);
    }
    catch (SALOME::SALOME_Exception &ex)
    {
        MESSAGE("explode failed:" << ex.details.text);
        return false;
    }
    if (aPartList->length() == 0) {
        MESSAGE("Nothing return after explode!");
        return false;
    }
    for (int i=0; i<aPartList->length(); i++)
        aGroup->insertPart(aPartList[i], aPart->getID());
    aGroup->deletePart(aPart->getID()); //after explode, delete the original one
    aObj->parent()->removeChild(aObj);
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::fuseParts
 *  fuse several parts into one part,
 *  append to the first group which part is selected
 * \param aList a list of enties of parts
 * \param aName a name for the new part
 * \return false if error
 */
bool MCCADGUI_DataModel::fuseParts  (const QStringList& aList,  const QString &aName )
{
    if (aList.isEmpty()) return false;
    if (aName.trimmed().isEmpty()) {
        MESSAGE("Name should not be empty!");
        return false;
    }
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    MCCAD_ORB::PartList_var aPartList = new MCCAD_ORB::PartList;
    MCCAD_ORB::Group_var aGroup ; //!!here is not very regious, because it consider all the selected objects are parts
    //for test
    if (aGroup->_is_nil())
        cout <<"agroup is nil!" <<endl;
    //get all the selected parts
    for (int i=0; i<aList.size(); i++) {
        //check if is part
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj || !aObj->isPart()) continue;
        else  {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            MCCAD_ORB::Group_var bGroup = aObj->getGroup();
            if (aGroup->_is_nil()) aGroup = bGroup;  //obtain the first group
            if (!aPart->_is_nil()) {
                aPartList->length(aPartList->length() + 1); //increase a space
                aPartList[aPartList->length()-1] = MCCAD_ORB::Part::_duplicate( aPart);
                if (!bGroup->_is_nil()) { //delete the part
                    bGroup->deletePart(aPart->getID());
                    aObj->parent()->removeChild(aObj);
                }
            }
        }
    }

    //call the engine to fuse
    MCCAD_ORB::Part_var aNewPart ;
    try {
        aNewPart = engine->fuseParts(aPartList, aName.toLatin1());
    }
    catch (SALOME::SALOME_Exception &ex)    {
        MESSAGE("Fuse failed:" << ex.details.text);
        return false;
    }
    aGroup->appendPart(aNewPart);
    return true;
}


/*!
 * \brief MCCADGUI_DataModel::recoverShapeInPart
 *  recover the shape of the part to backup shape
 *  the part has no backup will be skipped, but not report error
 * \param aList a entry list of parts
 * \return false if error
 */
bool   MCCADGUI_DataModel::recoverShapeInParts (const QStringList& aList)
{
    if (aList.size() == 0) {
        MESSAGE("No part in this list");
        return false;
    }
    //get the engine and study
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }
    for (int  i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (aPart->hasBackupShape())
                aPart->recoverShape();
        }
    }
    return true;
}

/*!
 * \brief decomposeShapes
 *  decomposed the shape of the parts,
 *  if group or component, decomposed all parts included
 *  parts already been decomposed will be skip without error
 * \param aList a list of entry for object to be decomposed
 * \return false if error
 */
bool MCCADGUI_DataModel::decomposeShapes(const QStringList& aList)
{
    //check
    if (aList.size() == 0) return false;
    //append all the object into the list
    DataObjectList aObjList ;
    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (aObj)
            aObjList.append(aObj);
    }
    if (aObjList.isEmpty()) return false;

    //call the overloaded method to finish this work
    if (!decomposeShapes(aObjList)) return false;
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::decomposeShapes
 *  recursively find the parts and decompose them,
 * \param aObjList an object list, object can be groups and components as well as parts
 * \return false if error
 */
bool MCCADGUI_DataModel::decomposeShapes(const DataObjectList & aObjList)
{
    //get the engine and study
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }

    for (int i=0; i<aObjList.size(); i++) {
        MCCADGUI_DataObject * aObj = dynamic_cast <MCCADGUI_DataObject * > (aObjList[i]);
        //decompose all parts
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (!aPart->_is_nil()) {
                if (engine->decomposePart(aPart))  {//not using _retn() method because apart is not release inside. do not care about failed or not
                    MESSAGE("Part: "<< aPart->getName() <<" is decomposed.");
                }
                //testing
                else {
                    cout<<"Decomposition failed, sorry..."<<endl;
                }
            }
        }
        //recursively decompose the parts in
        else if (aObj->isGroup() ) {
            DataObjectList aNewObjList = aObj->children();
            if (!aNewObjList.isEmpty())
                decomposeShapes(aNewObjList);
        }
        else if (aObj->isComponent()) {
            MCCAD_ORB::Component_var aComponent = aObj->getComponent();
            if (aComponent->_is_nil()) continue;
            if (aComponent->hasEnvelop())  { //add step to decompose envelop
                if (!engine->decomposeEnvelop(aComponent)) return false;
            }
            else {
                DataObjectList aNewObjList = aObj->children();
                if (!aNewObjList.isEmpty())
                    decomposeShapes(aNewObjList);
            }
        }
        else {
            MESSAGE("Unable to decompose this type of object!");
            return false;
        }
    }
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::send2GEOM
 *  send Shapes in parts to GEOM module, for modifying geometry
 *  only working for parts
 * \param aList a list of entries for parts
 * \return false if error
 */
bool MCCADGUI_DataModel::send2GEOM(const QStringList& aList)
{
    //checking
    if (aList.isEmpty()) return false;

    //get the engine and study
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }

        //###loop all parts and get shape and make new GEOM object###
        for (int i=0; i<aList.size(); i++) {
            MCCADGUI_DataObject * aObj = findObject(aList[i]);
            if (aObj && aObj->isPart()) {
                MCCAD_ORB::Part_var aPart = aObj->getPart();
                if (!aPart->_is_nil()) {
                    sendShape2GEOM(*aPart->getShapeStream(), aPart->getName());
                }
                else MESSAGE("the Part in the "<<i+1<<"th selection is nil!" );
            }
            else if (aObj && aObj->isGroup()) {
                MCCAD_ORB::Group_var aGroup = aObj->getGroup();
                if (!aGroup->_is_nil()) {
                    sendShape2GEOM(*aGroup->getShapeStream(), aGroup->getName());
                }
                else MESSAGE("the Group in the "<<i+1<<"th selection is nil!" );
            }
            else if (aObj && aObj->isComponent()) {
                MCCAD_ORB::Component_var aComponent = aObj->getComponent();
                if (!aComponent->_is_nil()) {
                    QMessageBox::StandardButton aBtn = QMessageBox::No;
                    if (aComponent->hasEnvelop())
                       aBtn = SUIT_MessageBox::information
                                (getMCCADGUImodule()->application()->desktop(),QString("Message"),
                                 QString( "Send the Envelop?"),QMessageBox::Yes, QMessageBox::No);
                    if (aBtn == QMessageBox::Yes)
                        sendShape2GEOM(*aComponent->getEnvelopStream(), aComponent->getName());
                    else
                        sendShape2GEOM(*aComponent->getShapeStream(), aComponent->getName());
                }
                else MESSAGE("the Component in the "<<i+1<<"th selection is nil!" );
            }
        }
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::sendShape2GEOM
 *  send a shape stream to GEOM
 * \param aShapeStream
 * \return false if error
 */
bool  MCCADGUI_DataModel::sendShape2GEOM(const SALOMEDS::TMPFile& aShapeStream, const QString & aName)
{
    //###get GEOM module
    const int studyID = getStudyID();
    if ( (CORBA::is_nil( GeometryGUI::GetGeomGen() ) && !GeometryGUI::InitGeomGen()) || !studyID  )   {
        MESSAGE("Cannot get GEOM module, Open GEOM before this operation, or cannot get study id!")
        return false;
    }
    GEOM::GEOM_Gen_var aGEOMGen = GeometryGUI::GetGeomGen();
    GEOM::GEOM_IInsertOperations_var aGEOMOp = aGEOMGen->GetIInsertOperations(studyID);
    SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(  SUIT_Session::session()->activeApplication()->activeStudy());
    _PTR(Study) studyDS;
    if ( study ) {
        studyDS = study->studyDS();
        GEOM::GEOM_Object_var aGEOMObj = aGEOMOp->RestoreShape(aShapeStream);
        if(!aGEOMObj->_is_nil()) {
            aGEOMObj->SetName(aName.toLatin1());
            aGEOMGen->PublishInStudy(GeometryGUI::ClientStudyToStudy( studyDS ), SALOMEDS::SObject::_nil(), aGEOMObj, aName.toLatin1());
            return true;
        }
        else {
            MESSAGE("the GEOM object is nil!");
            return false;
        }
    }
    return false;
}



/*!
 * \brief MCCADGUI_DataModel::assignShape
 *  assign the new shape to a part, or make a new part for a group
 *  the shape is possibly come from GEOM module
 * \param aEntry , entry of the part or group,
 * \param aShape ashape to be assign
 * \return false if error
 */
bool  MCCADGUI_DataModel::assignShape(const QString& aEntry, const TopoDS_Shape& aShape, const QString & aName)
{
    bool isNew = false;
    MCCADGUI_DataObject * aObj = findObject(aEntry);
    if (!aObj ) {
//        MESSAGE("object is empty!");
//        return false;
        isNew = true;
    }
    if (aShape.IsNull() || (aShape.ShapeType() != TopAbs_COMPSOLID &&
         aShape.ShapeType() != TopAbs_COMPOUND && aShape.ShapeType() != TopAbs_SOLID )    ) {
        MESSAGE("the shape is invalid!");
        return false;
    }

    //create a new component and group for this object
    if (isNew) {
        MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
        const int studyID = getStudyID();
        if ( !studyID || CORBA::is_nil( engine ) ) return false;
        //new component
        MCCAD_ORB::Component_var aComponent = engine->generateComponent(aName.toLatin1());
        MCCAD_ORB::ComponentList * aComponentList = engine->getData(studyID);
        aComponentList->length(aComponentList->length() + 1); //increase one space
        (*aComponentList)[aComponentList->length() -1] = MCCAD_ORB::Component::_duplicate(aComponent);
        //new group
        MCCAD_ORB::Group_var aGroup = engine->generateGroup(aName.toLatin1());
//        MCCAD_ORB::Component_var aComponent = engine->getComponent(studyID, aComponentID);
        aComponent->appendGroup(aGroup);
        MCCAD_ORB::Part_var aPart = engine->generatePart(aName.toLatin1());
        aPart->setShapeStream(*Shape2Stream(aShape));
        MCCAD_ORB::MColor aColor ;  //assignt the same color as group
        aGroup->getColor(aColor);
        aPart->setColor(aColor);
        aGroup->appendPart(aPart) ; //not using _retn(), because the part is copy inside
    }
    //assign the shape to the part
    else if(!isNew && aObj->isPart()) {
        MCCAD_ORB::Part_var aPart = aObj->getPart();
        if (!aPart->_is_nil()) {
            aPart->setShapeStream(*Shape2Stream(aShape));
        }
        else {
            MESSAGE ("the Part is nil!");
            return false;
        }
    }
    else if (!isNew && aObj->isGroup()) {
        MCCAD_ORB::Group_var aGroup = aObj->getGroup();
        if (!aGroup->_is_nil()) {
            MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
            MCCAD_ORB::Part_var aPart = engine->generatePart(aName.toLatin1());
            aPart->setShapeStream(*Shape2Stream(aShape));
            MCCAD_ORB::MColor aColor ;  //assignt the same color as group
            aGroup->getColor(aColor);
            aPart->setColor(aColor);
            aGroup->appendPart(aPart) ; //not using _retn(), because the part is copy inside
        }
        else {
            MESSAGE ("the Part is nil!");
            return false;
        }
    }
    else if (!isNew && aObj->isComponent()) {  //######assigned shape are consider as Mesh Envelop
        //new group
        MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
        MCCAD_ORB::Component_var aComponent = aObj->getComponent();
        if (aComponent->_is_nil()) {
            MESSAGE("The Component is nil")
            return false;
        }
        QMessageBox::StandardButton aBtn = SUIT_MessageBox::information
                (getMCCADGUImodule()->application()->desktop(),QString("Message"),
                 QString( "Set the shape as mesh envelop?"),QMessageBox::Yes, QMessageBox::No);
        if (aBtn == QMessageBox::Yes) {
            aComponent->setEnvelopStream(*Shape2Stream(aShape));
        }
        else {
            MCCAD_ORB::Group_var aGroup = engine->generateGroup(aName.toLatin1());
            aComponent->appendGroup(aGroup);
            MCCAD_ORB::Part_var aPart = engine->generatePart(aName.toLatin1());
            aPart->setShapeStream(*Shape2Stream(aShape));
            MCCAD_ORB::MColor aColor ;  //assignt the same color as group
            aGroup->getColor(aColor);
            aPart->setColor(aColor);
            aGroup->appendPart(aPart) ; //not using _retn(), because the part is copy inside
        }
    }
    else     {
        MESSAGE ("cannot assign shape to this object!");
        return false;
    }
    return true;
}


bool  MCCADGUI_DataModel::sendMesh2SMESH(const QStringList& aList)
{
    //checking
    if (aList.isEmpty()) {
        MESSAGE("the list of part is empty!");
        return false;
    }
    //get the smesh engine
    SMESH::SMESH_Gen_var aSMESHGen = SMESHGUI::GetSMESHGen() ;
    if (aSMESHGen->_is_nil()) {
        MESSAGE("Please open the SMESH module before this operation!");
        return false;
    }
    //get mccad engine
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID || CORBA::is_nil( engine ) ) return false;

    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj) continue;  //if cannot get the object, next
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (aPart->_is_nil()) {
                MESSAGE("This object has a nil part!");
                continue;
            }
            if (!aPart->hasMesh()) {
                MESSAGE("The part" <<aPart->getName() <<" has no mesh!");
                continue;
            }
            //get the mesh
//            ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingUMesh> aUMesh
//                    =ParaMEDMEM::MEDCouplingUMeshClient::New(aUMeshIOR);
            //write the mesh into a temp file
            QString  aTempDir = "tmpfilemccad2smesh.med";
            QFile::remove(aTempDir); //remove the file first if any
            try {
                SALOME_MED::MEDCouplingUMeshCorbaInterface_var aUMeshIOR = aPart->getMesh(); //using _var to release the duplicate mesh
                engine->export2MED(aUMeshIOR, aTempDir.toLatin1());

                SMESH::DriverMED_ReadStatus aStatus;
                aSMESHGen->CreateMeshesFromMED(aTempDir.toLatin1(), aStatus);
                if (aStatus != SMESH::DRS_OK )
                {
                    MESSAGE("problems in reading!" << aStatus);
                    QFile::remove(aTempDir); //remove if any
                    return false;
                }
            }
            catch (SALOME::SALOME_Exception &ex)
            {
                QFile::remove(aTempDir); //remove if any
                MESSAGE("Export failed: " << ex.details.text);
                return false;
            }
            QFile::remove(aTempDir); //remove if any
        }
        else {
            MESSAGE("This object type has no mesh!");
            continue;
        }
    }
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::assignMesh
 *  assign a mesh for a part
 * \param aEntry entry for the part
 * \param aMesh a MEDCouplingUMesh CORBA interface, please give the reference, dont release outside!
 * \return
 */
bool MCCADGUI_DataModel::assignMesh(const QStringList& aEntryList, const QStringList & aSMESHEntryList  , const QStringList &aNameList)
{

    //checking
    if(aEntryList.size() != aSMESHEntryList.size())     {
        MESSAGE("The selected objects are not matching with the number of mesh objects!");
        return false;
    }

    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID || CORBA::is_nil( engine ) ) return false;

    QString aEntry, aSMESHEntry;
    for (int i=0; i<aEntryList.size(); i++)
    {
        aEntry = aEntryList.at(i);
        aSMESHEntry = aSMESHEntryList.at(i);

        MCCADGUI_DataObject * aObj = findObject( aEntry);
        if (!aObj) {
            MESSAGE("Cannot find this object!");
            return false;
        }

        //get the smesh object
        const LightApp_DataOwner* owner = new LightApp_DataOwner (aSMESHEntry);
        // Get geom object from IO
        SMESH::SMESH_Mesh_var aSMESHObj = getMCCADGUImodule()->getSmesh(owner, true);
        if (aSMESHObj->_is_nil())    {
            MESSAGE("the SMESH object is nil!");
            return false;
        }

        //export mesh to a med file
        try {
            QString aTmpDir = aNameList.at(i) + QString(".med") ;
            QFile::remove(aTmpDir);
            aSMESHObj->ExportMED(aTmpDir.toLatin1(), true); //first export to MED mesh file
            //import the file
            SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr aUMesh =
                    engine->importMED(aTmpDir.toLatin1());
            if (aUMesh->_is_nil()) {
                MESSAGE("the read mesh is nil!");
                return false;
            }
            //find the part
            if (aObj->isPart()) {
                MCCAD_ORB::Part_var aPart = aObj->getPart();
                if (aPart->_is_nil()) {
                    MESSAGE("the Part is nil!");
                    QFile::remove(aTmpDir);
                    return false;
                }
                aPart->setMesh(aUMesh);
            }
            else {
                MESSAGE("Cannot assign mesh to this kind of object!");
                QFile::remove(aTmpDir);
                return false;
            }
            QFile::remove(aTmpDir);
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("assign mesh failed: " << ex.details.text);
            return false;
        }
    }
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::clearMesh
 *  clear mesh data on this geometry
 * \param aList a list of entry which are parts
 * \return false if error
 */
bool MCCADGUI_DataModel::clearMesh (const QStringList& aList)
{
    //checking
    if (aList.isEmpty()) return false;
    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj)  continue;
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (aPart->_is_nil()) continue;
            aPart->clearMesh();
        }
        else continue;
    }
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::clearEnvelop
 *  clear envelop in the component
 * \param aList a list of entry for components
 * \return false if error
 */
bool  MCCADGUI_DataModel::clearEnvelop (const QStringList& aList)
{
    if (aList.isEmpty()) return false;
    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj)  continue;
        if (aObj->isComponent()) {
            MCCAD_ORB::Component_var aComponent = aObj->getComponent();
            if (aComponent->_is_nil()) continue;
            aComponent->clearEnvelop();
        }
        else continue;
    }
    return true;
}


/*!
 * \brief MCCADGUI_DataModel::assignImportance
 * assign importance for selected part
 * if group or component is selected, then the part inside it will be changed
 * \param aList a object entry list
 * \param aIMP a importance array : FixArray[3]
 * \return  false if error
 */
bool MCCADGUI_DataModel::assignImportance(const QStringList& aList, MCCAD_ORB::FixArray  aIMP)
{
    //check
    if (aList.size() == 0) return false;
    //append all the object into the list
    DataObjectList aObjList ;
    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (aObj)
            aObjList.append(aObj);
    }
    if (aObjList.isEmpty()) return false;

    //call the overloaded method to finish this work
    if (!assignImportance(aObjList, aIMP)) return false;
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::assignImportance
 * overload method, for assign importance recursively
 * \param aObjList  a dataobject list,
 * \param aIMP a importance array
 * \return false if error
 */
bool MCCADGUI_DataModel::assignImportance(const DataObjectList & aObjList, MCCAD_ORB::FixArray  aIMP)
{
    //get the engine and study
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }

    for (int i=0; i<aObjList.size(); i++) {
        MCCADGUI_DataObject * aObj = dynamic_cast <MCCADGUI_DataObject * > (aObjList[i]);
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (!aPart->_is_nil()) {
                aPart->setImportance(aIMP);
            }
        }
        //recursively assign the parts IMP
        else if (aObj->isGroup() || aObj->isComponent()) {
            DataObjectList aNewObjList = aObj->children();
            if (!aNewObjList.isEmpty())
                assignImportance(aNewObjList, aIMP);
        }
        else {
            MESSAGE("Unable to assign importance this type of object!");
            return false;
        }
    }
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::assignRemark
 * assign remark for parts, groups, or components
 * the remark for part will not override by group or component
 * \param aList a list of object entry
 * \param aRemark a remark
 * \return  false if error
 */
bool  MCCADGUI_DataModel::assignRemark(const QStringList& aList, const QString & aRemark)
{
    //check
    if (aList.size() == 0) return false;
    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj)  continue;
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (aPart->_is_nil()) continue;
            aPart->setRemark(aRemark.toLatin1());
        }
        else if (aObj->isGroup()) {
            MCCAD_ORB::Group_var aGroup = aObj->getGroup();
            if (aGroup->_is_nil()) continue;
            aGroup->setRemark(aRemark.toLatin1());
        }
        else if (aObj->isComponent()) {
            MCCAD_ORB::Component_var aComponent = aObj->getComponent();
            if (aComponent->_is_nil()) continue;
            aComponent->setRemark(aRemark.toLatin1());
        }
    }
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::assignAdditive
 *  assign additive for parts, groups, components
 *  the additive of parts will override by its group
 * \param aList aList a list of object entry
 * \param aAdditive a additive card
 * \return false if error
 */
bool  MCCADGUI_DataModel::assignAdditive(const QStringList& aList, const QString & aAdditive)
{
    //check
    if (aList.size() == 0) return false;
    //append all the object into the list
    DataObjectList aObjList ;
    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (aObj)
            aObjList.append(aObj);
    }
    if (aObjList.isEmpty()) return false;

    //call the overloaded method to finish this work
    if (!assignAdditive(aObjList, aAdditive)) return false;
    return true;
}

bool  MCCADGUI_DataModel::assignAdditive(const DataObjectList & aObjList, const QString & aAdditive)
{
    //get the engine and study
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID && CORBA::is_nil( engine ) )
    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }

    for (int i=0; i<aObjList.size(); i++) {
        MCCADGUI_DataObject * aObj = dynamic_cast <MCCADGUI_DataObject * > (aObjList[i]);
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (!aPart->_is_nil()) {
                aPart->setAdditive(aAdditive.toLatin1());
            }
        }
        //recursively assign the parts IMP
        else if (aObj->isGroup() || aObj->isComponent()) {
            DataObjectList aNewObjList = aObj->children();
            if (!aNewObjList.isEmpty())
                assignAdditive(aNewObjList, aAdditive);
        }
        else {
            MESSAGE("Unable to assign  this type of object!");
            return false;
        }
    }
    return true;
}

bool   MCCADGUI_DataModel::exportAllMesh2Abaqus(const QString & FileName)
{
    if (FileName.trimmed().isEmpty())    {
        MESSAGE("File name is empty!");
        return false;
    }

    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        try {
            engine->exportAllMesh2Abaqus(studyID, FileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("export failed: " << ex.details.text)  ;
            return false;
        }
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}



bool MCCADGUI_DataModel::importGeom(const QString & FileName, bool & isExplode, const int &ComponentID, const int &GroupID)
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    //get engine and studyID
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        MCCAD_ORB::PartList_var aPartList;  //automatic release
        try
        {
             aPartList = engine->ImportGeom(FileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Import failed: " << ex.details.text)  ;
            return false;
        }
        //check if explodible and ask
        bool isExplodible = false;
        for (int i=0; i<aPartList->length(); i++) {
            if (aPartList[i]->isExplodible()) {
                isExplodible = true;
                break;  //if one is explodible, quit
            }
        }
        if (isExplodible && !isExplode) {
            QMessageBox::StandardButton aBtn = SUIT_MessageBox::information(getMCCADGUImodule()->application()->desktop(),QString("Message"),
                                                                            QString( "Model contains compound(s) or comsolid(s), \nexplode them?"),QMessageBox::Yes, QMessageBox::No);

            if (aBtn == QMessageBox::Yes)
                isExplode = true;
            else isExplode = false;
        }
        //if explode, append all the explode part into new group
        MCCAD_ORB::Group_var aGroup = engine->generateGroup(SUIT_Tools::file(FileName,false).toLatin1());
        if (isExplode) {
            try {
                for (int i=0; i<aPartList->length(); i++) {
                    MCCAD_ORB::PartList_var bPartList;  //automatic release
                    bPartList = engine->explodePart(aPartList[i]);
                    for (int j=0; j<bPartList->length(); j++) {
                        aGroup->appendPart(bPartList[j]);
                    }
                }
            }
            catch (SALOME::SALOME_Exception &ex)            {
                MESSAGE("Explode failed: " << ex.details.text)  ;
                return false;
            }
        }
        else {
            aGroup->setPartList(aPartList);  //set the part list to the group
        }
        if (GroupID < 0) //create new group
        {
            MCCAD_ORB::Component_var aComponent = engine->getComponent(studyID, ComponentID);
            aComponent->appendGroup(aGroup._retn());
        }
        else  //append to a exist group
        {
            MCCAD_ORB::Group_var bGroup = engine->getGroup(studyID, GroupID);
            if (bGroup->_is_nil()) {
                MESSAGE("Cannot found the group to append!")
                return false;
            }
            bGroup->appendPartInGroup(aGroup);
        }
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}


/*!
 * \brief MCCADGUI_DataModel::checkBeforeConvert
 *  check if the object in this study meets conversion requriement
 *  surface check, decomposition check, material check, envelop check
 * \return false if check failed
 */
bool   MCCADGUI_DataModel:: checkBeforeConvert()
{
    //get the target MC code
    SUIT_Session* session = SUIT_Session::session();
    SUIT_ResourceMgr* resMgr = session->resourceMgr();
    bool isMCNP6 = false;
    if (resMgr->integerValue("MCCAD", "target_code", 0) == 1) //if MCNP6 is chosen
        isMCNP6 = true;
    bool isGDML = false;
    if (resMgr->integerValue("MCCAD", "target_code", 0) == 3) //if GDML is chosen
        isGDML = true;
    bool isForceDecomp= resMgr->booleanValue("MCCAD", "force_decomp", true);

    double AllowDiff = resMgr->doubleValue("MCCAD", "Mesh_Volume_Diff", 0.01);
    double aTotalVol = 0, bTotalVol = 0; //for calculating the total volume

    //check all the Components groups and parts
    bool isOK = true;

    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID || CORBA::is_nil( engine ) )    {
        MESSAGE("Errors in study or in engine!");
        return false;
    }

    MCCAD_ORB::ComponentList * aComponentList = engine->getData(studyID);
    if (!aComponentList)    {
        MESSAGE("component list is emtpy!")
                return false;
    }
    //*********LOOP for Components*****
    cout <<"O######################O" <<endl;
    cout <<"O####CHECKING START####O" <<endl;
    for (int i=0; i< aComponentList->length(); i++)
    {
        MCCAD_ORB::Component_var aComponent = (*aComponentList)[i];
        if (aComponent->_is_nil()) {
            cout <<"ERROR: The "<<i<<"th Component is nil!" <<endl;
            isOK = false;
            continue;
        }
        cout <<"\tCHECK "<<aComponent->getName() <<": ";
        if (isMCNP6 && aComponent->hasEnvelop()) //if target code is MCNP6, and the component has the envelop
        {
            //check envelop shape surface
            cout << "A meshed component.   ";
          TopoDS_Shape aTmpShape = Stream2Shape(*aComponent->getEnvelopStream());
          if (aTmpShape.IsNull()) {
              cout << "ERROR: Cannot get the envelop!"<<endl;
              isOK = false;
              continue;
          }
          if (!checkSolid(aTmpShape)) {
              cout << "ERROR: Contain no solid!"<<endl;
              isOK = false;
              continue;
          }
          if (!checkSurface(aTmpShape, aComponent->getName())) {
              cout << "ERROR: Contain invalid surfaces!"<<endl;
              isOK = false;
              continue;
          }
        }
        cout <<"OK!" <<endl;
        MCCAD_ORB::GroupList * aGroupList = aComponent->getGroupList();
        //*********LOOP for Groups*****
        for (int j=0; j<aGroupList->length(); j++)
        {

            MCCAD_ORB::Group_var aGroup = (*aGroupList)[j];
            if (aGroup->_is_nil()) {
                cout <<"ERROR: The "<<i<<"th Group in this component is nil!" <<endl;
                isOK = false;
                continue;
            }
            cout <<"\t\tCHECK "<<aGroup->getName() <<": ";
            if (!aGroup->hasMaterial())
                cout << "INFO: This Group has no material, consider as void."<<endl;
            else  cout <<"OK!" <<endl;

            MCCAD_ORB::PartList * aPartList = aGroup->getPartList();
            //*********LOOP for Parts*****
            for (int k=0; k<aPartList->length(); k++)
            {
                MCCAD_ORB::Part_var aPart = (*aPartList)[k];
                if (aPart->_is_nil()) {
                    cout <<"ERROR: The "<<i<<"th Part in this Group is nil!" <<endl;
                    isOK = false;
                    continue;
                }

                cout <<"\t\t\tCHECK "<<aPart->getName() <<": ";
                 double aVol;
                 aVol = aPart->getVolume();
                 cout <<"\t Volume:\t"<<aVol<<"\t\t";
                if (isMCNP6 && aComponent->hasEnvelop()) {  //if convert to MCNP6, check mesh
                    if (!aPart->hasMesh()) {
                        cout << "ERROR: Should contain a mesh!"<<endl;
                        isOK = false;
                        continue;
                    }
                    //Check the volume if match
                    double  bVol;
                    bVol = aPart->getMeshVolume();
                    aTotalVol += aVol;
                    bTotalVol += bVol;
//                    double tmpDouble = fabs(aVol-bVol)/aVol;
                    double tmpDouble = (aVol-bVol)/aVol;
                    cout <<"\t Mesh Volume: \t"<<bVol<<"\t\t Diff.: \t"<<tmpDouble<<"\t\t";
                    if (fabs(aVol-bVol)/aVol > AllowDiff){
                        cout << "ERROR: Mesh volume not much with CAD volume!  \t"<<aVol <<"\t"<<bVol<<"\t Diff: \t"<<fabs(aVol-bVol)/aVol*100 <<"%"<<endl;
                        isOK = false;
                        continue;
                    }
                    else cout <<"OK!" <<endl;
                }
                else { //check surface and decomposition
                    //check envelop shape surface
                  TopoDS_Shape aTmpShape = Stream2Shape(*aPart->getShapeStream());
                  if (aTmpShape.IsNull()) {
                      cout << "ERROR: Cannot get the shape!"<<endl;
                      isOK = false;
                      continue;
                  }
                  if (!checkSolid(aTmpShape)) {
                      cout << "ERROR: Contain no solid!"<<endl;
                      isOK = false;
                      continue;
                  }
                  if (isForceDecomp) {
                      if (!checkSurface(aTmpShape, aPart->getName())) {
                          cout << "ERROR: Contain invalid surfaces!"<<endl;
                          isOK = false;
                          continue;
                      }

                      if (!aPart->getIsDecomposed()) {
                          cout << "INFO: This Part is not decomposed, will be done during conversion."<<endl;
                          continue;
                      }
                      else
                          cout <<"OK!" <<endl;
                  }
                  else cout <<"OK!" <<endl;
                }
            }
        }
    }
    //print the volume comparison if MCNP6
    if(isMCNP6) {
        cout << "Comparison from CAD volume and Mesh volume \t"<<aTotalVol <<"\t"<<bTotalVol<<"\t Diff: \t"<<(aTotalVol-bTotalVol)/aTotalVol*100 <<"%"<<endl;
    }
    cout <<"O####CHECKING ENDED####O" <<endl;
    cout <<"O######################O" <<endl;
    return isOK;
}


/*!
 * \brief MCCADGUI_DataModel::checkSurface
 * check if invalid surface.
 * if get GEOM module, the invalid surface will be loaded into GEOM, else output as files
 * \param aShape a shape to be check
 * \param aName the object name
 * \return false if failed
 */
bool MCCADGUI_DataModel::checkSurface(const TopoDS_Shape & aShape, const QString & aName)
{
    if (aShape.IsNull()) {
        MESSAGE("The shape is nill!");
        return false;
    }

    //start to check
    Handle_TopTools_HSequenceOfShape aShapeList = new TopTools_HSequenceOfShape();
    aShapeList->Append(aShape);
    McCadCSGTool_SurfaceChecker surfCheck;
    TCollection_AsciiString aSolidName = aName.toStdString().c_str();
    if (!surfCheck.printSurfaceInfo(aShapeList,aSolidName)) {
        QString aTmpName = aName + QString("-invalid.stp");
        QFile::rename("invalidSurfaces.stp", aTmpName);
        STEPControl_Reader Reader;
        Standard_Integer status = Reader.ReadFile( aTmpName.toLatin1() );
        if (status != IFSelect_RetDone )  //if faild, return and leave the file
            return false;
        Reader.TransferRoots();
        TopoDS_Shape aTmpShape = Reader.OneShape();
        if (sendShape2GEOM(*Shape2Stream(aTmpShape),aName + QString("-invalid") ))
                QFile::remove(aTmpName); //remove the file after sending this shape
        return false;
    }
    else  return true;
}

/*!
 * \brief MCCADGUI_DataModel::checkSolid
 *  check if the shape has solid
 * \param aShape
 * \return false if has no solid
 */
bool  MCCADGUI_DataModel::checkSolid(const TopoDS_Shape & aShape)
{

    if( aShape.ShapeType() == TopAbs_COMPSOLID || aShape.ShapeType() == TopAbs_COMPOUND )
    {
        int cnt = 0;
        TopExp_Explorer exp;
        for(exp.Init(aShape,TopAbs_SOLID); exp.More(); exp.Next())
            cnt ++;
        if (cnt == 0)
            return false;
        else return true;

    }
    else if( aShape.ShapeType() == TopAbs_SOLID)
        return true;
    else return false;
}


/*!
 * \brief MCCADGUI_DataModel::convert2MC
 *  call McCad to generate MC description
 * \return
 */
bool  MCCADGUI_DataModel::convert2MC(const QString & FileName)
{
    if (FileName.trimmed().isEmpty()) {
        MESSAGE("the file name is empty!");
        return false;
    }
    //#### 1. check the geometry ####
    if (!checkBeforeConvert()) {
        MESSAGE("Check geometry failed! ");
        SUIT_MessageBox::information(getMCCADGUImodule()->application()->desktop(), QString("Convert to MC input"), QString("Check geometry failed! \nPlease check the output info."));
        return false;
    }

    //#### 2. decompose all objects###
    SUIT_Session* session = SUIT_Session::session();
    SUIT_ResourceMgr* resMgr = session->resourceMgr();
    bool isForceDecompose =  resMgr->booleanValue("MCCAD","force_decomp"            , true   );
    if (isForceDecompose)
    {
        MCCADGUI_ModuleObject* modelRoot = dynamic_cast<MCCADGUI_ModuleObject*>( root() );
        if( !modelRoot )  {  // root is not set yet
            modelRoot = new MCCADGUI_ModuleObject( this, 0 );
            setRoot( modelRoot );
        }
        DataObjectList aObjectList = modelRoot->children(false); //get all components
        if (!decomposeShapes(aObjectList)) {
            MESSAGE("Decomposition failed!");
    //        SUIT_MessageBox::information(getMCCADGUImodule()->application()->desktop(), QString("Convert to MC input"), QString("Decomposition failed!"));
            return false;
        }
    }


    //#### 3. set the conversion parameters from preference ###
    setConvertPara();

    //#### 4. if MCNP6, output the mesh ###
//    SUIT_Session* session = SUIT_Session::session();
//    SUIT_ResourceMgr* resMgr = session->resourceMgr();
    int codeOption = resMgr->integerValue("MCCAD", "target_code", 0) ;
    if (codeOption == 1) //if MCNP6 selected
        if (!exportAllMesh2Abaqus(FileName + QString(".inp"))) {
            MESSAGE("export mesh failed!");
            return false;
        }

    //#### 5.perform conversion and write to file
    S_McCadVoidCellManager * aSVoidCellManager = new S_McCadVoidCellManager();
    aSVoidCellManager->S_ReadGeomData(getStudyID());
    if (codeOption == 0 || codeOption == 1) {
        aSVoidCellManager->SetConvetor("MCNP");
    }
    else if (codeOption == 2) {
        aSVoidCellManager->SetConvetor("TRIPOLI");
    }
    else if (codeOption == 3) {
        aSVoidCellManager->SetConvetor("GDML");
    }
    aSVoidCellManager->SetOutFileName(FileName.toStdString().c_str());
    aSVoidCellManager->S_Process();

    delete aSVoidCellManager;
    aSVoidCellManager = NULL;
    return true;

}

/*!
 * \brief MCCADGUI_DataModel::setConvertPara
 * set the conversion parameters into the McCadConvertConfig
 * McCadConvertConfig has static member which can be assess everywhere
 * \return fa
 */
void MCCADGUI_DataModel::setConvertPara()
{
    SUIT_Session* session = SUIT_Session::session();
    SUIT_ResourceMgr* resMgr = session->resourceMgr();

    McCadConvertConfig::SetInitCellNum        (resMgr->integerValue("MCCAD","init_cell_no"        , 1      ));
    McCadConvertConfig::SetInitSurfNum        (resMgr->integerValue("MCCAD","init_surf_no"        , 1      ));
    McCadConvertConfig::SetMinIptSolidVod     (resMgr->doubleValue("MCCAD","min_solid_vol"       , 1.00   ));
    McCadConvertConfig::SetMinVoidVol         (resMgr->doubleValue("MCCAD","min_void_vol"        , 1.25e2 ));
    McCadConvertConfig::SetMinDecomFaceArea   (resMgr->doubleValue("MCCAD","min_dec_face_area"   , 50     ));



    McCadConvertConfig::SetMinSmplPntNum      (resMgr->integerValue("MCCAD","min_sample_points"   , 50     ));
    McCadConvertConfig::SetMaxSmplPntNum      (resMgr->integerValue("MCCAD","max_sample_points"   , 100    ));
    McCadConvertConfig::SetXResolution        (resMgr->doubleValue("MCCAD","x_resl"              , 0.001  ));
    McCadConvertConfig::SetYResolution        (resMgr->doubleValue("MCCAD","y_resl"              , 0.001  ));
    McCadConvertConfig::SetRResolution        (resMgr->doubleValue("MCCAD","r_resl"              , 0.00314));
    McCadConvertConfig::SetVoidDecomposeDepth (resMgr->integerValue("MCCAD","max_dec_depth"       , 15     ));
    McCadConvertConfig::SetMaxDiscLength      (resMgr->integerValue("MCCAD","max_cell_exp_depth"  , 500    ));
    McCadConvertConfig::SetTolerence          (resMgr->doubleValue("MCCAD","Tolerance"           , 1.0e-4 ));
    McCadConvertConfig::SetWriteCollisionFile (resMgr->booleanValue("MCCAD","write_disc_file"     , true   ));

    int codeOption = resMgr->integerValue("MCCAD", "target_code", 0) ;
    if (codeOption == 3) {
        //disable void generation
        McCadConvertConfig::SetGenerateVoid       (false);
    }
    else {
        McCadConvertConfig::SetGenerateVoid       (resMgr->booleanValue("MCCAD","gen_void"            , true   ));
    }
//    McCadConvertConfig::SetInitPseudoCellNum  (resMgr->integerValue("MCCAD","init_pseudo_cell_no" , 90000  ));
//    McCadConvertConfig::SetInitEmbedCardNum   (resMgr->integerValue("MCCAD","embed_card_no"       , 2      ));


}


bool   MCCADGUI_DataModel::generateTetMesh(const QStringList& aList)
{
    if (aList.isEmpty()) {
        MESSAGE("the list of part is empty!");
        return false;
    }

    DataObjectList aObjList ;
    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj) continue;
        aObjList.append(aObj);
    }
    generateTetMesh(aObjList);
    return true;
}

/*!
   * \brief MCCADGUI_DataModel::generateTetMesh
   *  generate Tetrahedron mesh using Tetgen
   *  the parameters is retrived from Preference page
   * \param aList
   * \return
   */
bool   MCCADGUI_DataModel::generateTetMesh(const DataObjectList & aObjList)
{
    //for timing
    const clock_t begin_time = clock();

    //checking
    if (aObjList.isEmpty()) {
        MESSAGE("the list of part is empty!");
        return false;
    }
    //get mccad engine
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID || CORBA::is_nil( engine ) ) return false;

    //get the parameters from Preference page
    SUIT_Session* session = SUIT_Session::session();
    SUIT_ResourceMgr* resMgr = session->resourceMgr();
    int    aMeshEngineOpt  = resMgr->integerValue("MCCAD", "meshing_engine", 0);
    double aDeflection = resMgr->doubleValue("MCCAD","STL_Deflection"              , 0.001  );
//    double aCoefficient = resMgr->doubleValue("MCCAD","STL_Coefficient"             , 0.001  );
    double aVolThreshold = resMgr->doubleValue("MCCAD","Vol_Threshold"             , 0.0  );
    double aMeshQuality = resMgr->doubleValue("MCCAD","Tet_MeshQuality"             , 2.0   );

    bool isOK = true;
    stringstream failedMsg;
//    QDir atmpDir("problem_Geom");
//    if (!atmpDir.exists()) atmpDir.mkdir("problem_Geom");
    for (int i=0; i<aObjList.size(); i++) {
//        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        MCCADGUI_DataObject * aObj = dynamic_cast <MCCADGUI_DataObject *> (aObjList[i]);
        if (!aObj) continue;  //if cannot get the object, next
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (aPart->_is_nil()) {
//                MESSAGE("This object has a nil part!");
                isOK = false;
                failedMsg << aObj->name().toStdString() << " is a nill part!\n";
                continue;
            }
            if (aPart->isExplodible()) { //if compound, it is most possibility to be failed in mesh generation.
//                MESSAGE("The part" <<aPart->getName() <<" contains compound solid, please Explode it before meshing! Skip it.");
                failedMsg << "The part" <<aPart->getName() <<" contains compound solid, please Explode it before meshing! Skip it. \n";
                isOK = false;
                continue;
            }
            if (aPart->hasMesh()) {
//                MESSAGE("The mesh in the part" <<aPart->getName() <<" will be overrided!");
                failedMsg << "The mesh in the part" <<aPart->getName() <<" will be overrided! \n";
            }
            //generatet he mesh
            if (aMeshEngineOpt == 0){
                MESSAGE("Generate mesh for " <<aPart->getName() <<" .");

                if (!engine->generateTetgenMesh(aPart,aDeflection, /*aCoefficient*/aVolThreshold,aMeshQuality )) {
    //                MESSAGE("The  part" <<aPart->getName() <<" is failed in generating tetrahedral mesh!");
                    failedMsg << "The  part" <<aPart->getName() <<" is failed in generating tetrahedral mesh! \n";
                    isOK = false;
                }
            }
            else if (aMeshEngineOpt == 1) {
                MESSAGE("Generate mesh for " <<aPart->getName() <<" .");

                if (!engine->generateNetgenMesh(aPart,aDeflection, /*aCoefficient*/aVolThreshold )) {
    //                MESSAGE("The  part" <<aPart->getName() <<" is failed in generating tetrahedral mesh!");
                    failedMsg << "The  part" <<aPart->getName() <<" is failed in generating tetrahedral mesh! \n";
                    isOK = false;
                }
            }

        }
        else {
            MESSAGE("Skip the object " <<aObj->name().toStdString() <<" because it is not a Part!");
        }
    }
//    MESSAGE("Summary info:\n"<<failedMsg);
    cout << failedMsg.str() <<endl;

    //show time
    cout << "Time used for meshing: "<< float (clock() - begin_time ) /CLOCKS_PER_SEC <<endl;
    return isOK;
}

/*!
 * \brief MCCADGUI_DataModel::generateMesh
 *  generate mesh for all the parts
 *  if some parts has mesh, a message box will ask if override mesh
 * \return false if error
 */
bool MCCADGUI_DataModel::generateMesh()
{
    //check all the objects, and catagolized part into meshed and unmeshed parts
    bool isOK = true;
    stringstream failedMsg;
    DataObjectList unMeshedObjList, MeshedObjList;
    for ( SUIT_DataObjectIterator it( root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
    {
        MCCADGUI_DataObject* aObj = dynamic_cast<MCCADGUI_DataObject*>( it.current() );
        if (!aObj) continue;  //if cannot get the object, next
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (aPart->_is_nil()) {
                isOK = false;
                failedMsg << aObj->name().toStdString() << " is a nill part!\n";
                continue;
            }
            if (aPart->isExplodible()) { //if compound, it is most possibility to be failed in mesh generation.
                failedMsg << "The part" <<aPart->getName() <<" contains compound solid, please Explode it before meshing! Skip it. \n";
                isOK = false;
                continue;
            }
            if (aPart->hasMesh())
                MeshedObjList.append(aObj);
            else
                unMeshedObjList.append(aObj);
        }
    }
    //ask if override meshes
    if (!MeshedObjList.empty())
    {
        bool yestoall = false;
        bool notoall = false;
        QMessageBox::StandardButton aBtn ;
        for (int i=0; i<MeshedObjList.size(); i++) {
            bool YesOrNo = false; //default not override
            MCCADGUI_DataObject* aObj = dynamic_cast<MCCADGUI_DataObject*> (MeshedObjList[i]);
            if (!yestoall) { //only ask if neccessary
                aBtn  = SUIT_MessageBox::question(getMCCADGUImodule()->application()->desktop(),
                                                     QString("Question"),
                                                     QString( "%1 has mesh already, Override the mesh?").arg(aObj->name()),
                                                     QMessageBox::Yes| QMessageBox::YesToAll| QMessageBox::No| QMessageBox::NoToAll);
                if (aBtn == QMessageBox::Yes)
                    YesOrNo = true;
                else if (aBtn == QMessageBox::YesToAll)
                    yestoall = true;
                else if (aBtn == QMessageBox::NoToAll)
                    notoall = true;
            }

            if (YesOrNo || yestoall) {
                unMeshedObjList.append(aObj);
            }
            else if (notoall)
                break;
            else
                continue;
        }
    }
    //generated the mesh
    if (!generateTetMesh(unMeshedObjList))
        isOK = false;
    MESSAGE(failedMsg.str());
    return isOK;
}


bool MCCADGUI_DataModel::exportGeom(const QStringList& aList, const QString &FileName)
{
    //checking
    if (aList.isEmpty()) return false;

    //obtain a list of BaseObj
    MCCAD_ORB::BaseObjList_var aBaseObjList = new MCCAD_ORB::BaseObjList;
    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (aObj) {
            aBaseObjList->length(aBaseObjList->length() + 1); //increase a space
            aBaseObjList[aBaseObjList->length()-1] = aObj->getBaseObj();
        }
    }

    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        try {
            engine->exportGeom(aBaseObjList, FileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("export failed: " << ex.details.text)  ;
            return false;
        }
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}

/*!
 * \brief MCCADGUI_DataModel::translateShapes
 *  translate (move) the shape
 * \param aList a list of part,
 * \param aVector the translation vector
 * \return false if failed
 */
bool  MCCADGUI_DataModel::translateShapes(const QStringList& aList,MCCAD_ORB::FixArray aVector )
{
    //checking
    if (aList.isEmpty()) {
        MESSAGE("the list of part is empty!");
        return false;
    }

    //get mccad engine
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID || CORBA::is_nil( engine ) ) return false;

    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj) continue;  //if cannot get the object, next
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (aPart->_is_nil()) {
                MESSAGE("This object has a nil part!");
                continue;
            }
            //get the shape and translate it
            TopoDS_Shape aTmpShape = Stream2Shape(*aPart->getShapeStream());
            gp_Vec aVec (aVector[0], aVector[1], aVector[2]);
            gp_Trsf aTranslate;
            aTranslate.SetTranslation(aVec);
            BRepBuilderAPI_Transform aBuilder (aTmpShape, aTranslate, true /*make a copy*/);
            aPart->setShapeStream(*Shape2Stream(aBuilder.Shape()));
        }
        else {
            MESSAGE("This object type has no mesh!");
            continue;
        }
    }
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::rotateShapes
 *  Rotate the shapes
 * \param aList a list of parts
 * \param aOrigin the origin of the axis
 * \param aDir vector of the axis
 * \param aAngle rotate angle
 * \return false if failed
 */

bool  MCCADGUI_DataModel::rotateShapes(const QStringList& aList,MCCAD_ORB::FixArray aOrigin,
                                       MCCAD_ORB::FixArray aDir, const double & aAngle)
{
    //checking
    if (aList.isEmpty()) {
        MESSAGE("the list of part is empty!");
        return false;
    }

    //get mccad engine
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID || CORBA::is_nil( engine ) ) return false;

    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj) continue;  //if cannot get the object, next
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (aPart->_is_nil()) {
                MESSAGE("This object has a nil part!");
                continue;
            }
            //get the shape and translate it
            TopoDS_Shape aTmpShape = Stream2Shape(*aPart->getShapeStream());
            gp_Pnt Origin (aOrigin[0], aOrigin[1], aOrigin[2]);
            gp_Dir Dir (aDir[0], aDir[1], aDir[2]);
            gp_Ax1 aAx (Origin,Dir );
            gp_Trsf aRotation;
            aRotation.SetRotation(aAx, aAngle * 3.14159265359 / 180); //convert degree to radian
            BRepBuilderAPI_Transform aBuilder (aTmpShape, aRotation, true /*make a copy*/);
            aPart->setShapeStream(*Shape2Stream(aBuilder.Shape()));
        }
        else {
            MESSAGE("This object type has no mesh!");
            continue;
        }
    }
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::scaleShapes
 *  scale the shapes
 * \param aList a list of parts
 * \param aFactor scaling factor
 * \return false if failed
 */
bool  MCCADGUI_DataModel::scaleShapes(const QStringList& aList,const double & aFactor )
{
    //checking
    if (aList.isEmpty()) {
        MESSAGE("the list of part is empty!");
        return false;
    }

    //get mccad engine
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID || CORBA::is_nil( engine ) ) return false;

    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj) continue;  //if cannot get the object, next
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (aPart->_is_nil()) {
                MESSAGE("This object has a nil part!");
                continue;
            }
            //get the shape and translate it
            TopoDS_Shape aTmpShape = Stream2Shape(*aPart->getShapeStream());
            gp_Trsf aScaling;
            aScaling.SetScaleFactor(aFactor);
            BRepBuilderAPI_Transform aBuilder (aTmpShape, aScaling, true /*make a copy*/);
            aPart->setShapeStream(*Shape2Stream(aBuilder.Shape()));
        }
        else {
            MESSAGE("This object type has no mesh!");
            continue;
        }
    }
    return true;
}
/*!
 * \brief MCCADGUI_DataModel::markIfDecomposed
 * manually mark those part as decomposed or undecomposed part
 * this is useful for import decompsed parts
 * \param aList a entry list
 * \return  false if error
 */

bool MCCADGUI_DataModel::markIfDecomposed(const QStringList& aList, const bool & isDecomposed)
{
    //check
    if (aList.size() == 0) return false;
    //append all the object into the list
    DataObjectList aObjList ;
    for (int i=0; i<aList.size(); i++) {
        MCCADGUI_DataObject * aObj = findObject(aList[i]);
        if (aObj)
            aObjList.append(aObj);
    }
    if (aObjList.isEmpty()) return false;

    //call the overloaded method to finish this work
    if (!markIfDecomposed(aObjList, isDecomposed)) return false;
    return true;
}

/*!
 * \brief MCCADGUI_DataModel::markIfDecomposed
 * manually mark those part as decomposed or undecomposed part
 * this is useful for import decompsed parts
 * \param aList a entry list
 * \return  false if error
 */
bool MCCADGUI_DataModel::markIfDecomposed(const DataObjectList & aObjList, const bool & isDecomposed)
{

    for (int i=0; i<aObjList.size(); i++) {
        MCCADGUI_DataObject * aObj = dynamic_cast <MCCADGUI_DataObject * > (aObjList[i]);
        //decompose all parts
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (!aPart->_is_nil()) {
                aPart->setIsDecomposed(isDecomposed);
            }
        }
        //recursively decompose the parts in
        else if (aObj->isGroup() ) {
            DataObjectList aNewObjList = aObj->children();
            if (!aNewObjList.isEmpty())
                markIfDecomposed(aNewObjList, isDecomposed);
        }
        else if (aObj->isComponent()) {
            MCCAD_ORB::Component_var aComponent = aObj->getComponent();
            if (aComponent->_is_nil()) continue;
            DataObjectList aNewObjList = aObj->children();
            if (!aNewObjList.isEmpty())
                markIfDecomposed(aNewObjList, isDecomposed);
        }
        else {
            MESSAGE("Unable to operate this type of object!");
            return false;
        }
    }
    return true;
}

bool  MCCADGUI_DataModel::markAllIfDecomposed(const bool & isDecomposed)
{
    for ( SUIT_DataObjectIterator it( root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
    {
        MCCADGUI_DataObject* aObj = dynamic_cast<MCCADGUI_DataObject*>( it.current() );
        if (!aObj) continue;  //if cannot get the object, next
        if (aObj->isPart()) {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            if (aPart->_is_nil())
                continue;
            aPart->setIsDecomposed(isDecomposed);
        }
    }
	return true;
}



/*!
 * \brief import a Abaqus mesh file
 *  the abaqus file should contain parts, assembly and instances,
 *  the instances should inherit from parts, independent create of instances is not supported.
 *  part is creat as an independent mesh, the elset and nset is not token care
 * \param AbaqusFileName
 * \return
 */
bool   MCCADGUI_DataModel::importAbaqus(const QString & AbaqusFileName)
{
    if (AbaqusFileName.trimmed().isEmpty()) {
        MESSAGE("Error in filename !");
        return false;
    }
    MESSAGE ("Import Meshes from: " << AbaqusFileName.toStdString());
    //new group for new meshes
    QStringList atmpList = QString(AbaqusFileName).split('/');
    atmpList = atmpList.at(atmpList.size()-1).split('.');
//    MeshGroup * aGroup = new MeshGroup (atmpList.at(0).toLatin1(), genId());

    //get the smesh engine
    SMESH::SMESH_Gen_var aSMESHGen = SMESHGUI::GetSMESHGen() ;
    if (aSMESHGen->_is_nil()) {
        MESSAGE("Please open the SMESH module before this operation!");
        return false;
    }
    //get mccad engine
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( !studyID || CORBA::is_nil( engine ) ) return false;

    //using reader to get the meshes
    MCMESHTRAN_AbaqusMeshReader aAbaqusReader ;
    if (aAbaqusReader.loadFile(AbaqusFileName) == Load_failed) {
        MESSAGE("Read Abaqus file failed !");
        return false;
    }

    vector <const MEDCouplingUMesh*> aTmpList = aAbaqusReader.getMeshList();
    for (int i=0; i< aTmpList.size(); i++ )
    {

        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDCouplingUMesh> aNewUMesh = const_cast<MEDCouplingUMesh *> (aTmpList[i]);
        //create field IOR
        ParaMEDMEM::MEDCouplingUMeshServant *myMeshServant=new ParaMEDMEM::MEDCouplingUMeshServant(aNewUMesh);
        SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr myUMeshIOR = myMeshServant->_this();

        QString  aTempDir = "/tmp/tmpfilemccad2smesh.med";
        QFile::remove(aTempDir); //remove the file first if any

        try {
            engine->export2MED(myUMeshIOR, aTempDir.toLatin1());

            SMESH::DriverMED_ReadStatus aStatus;
            aSMESHGen->CreateMeshesFromMED(aTempDir.toLatin1(), aStatus);
            if (aStatus != SMESH::DRS_OK )
            {
                MESSAGE("problems in reading!" << aStatus);
                QFile::remove(aTempDir); //remove if any
                return false;
            }
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            QFile::remove(aTempDir); //remove if any
            MESSAGE("Export failed: " << ex.details.text);
            return false;
        }
        QFile::remove(aTempDir); //remove if any
    }
    return true;
}



/*! Called on update of the structure of Data Objects */
void MCCADGUI_DataModel::build()
{
    MCCADGUI_ModuleObject* modelRoot = dynamic_cast<MCCADGUI_ModuleObject*>( root() );
    if( !modelRoot )  {  // root is not set yet
        modelRoot = new MCCADGUI_ModuleObject( this, 0 );
        setRoot( modelRoot );
    }

    // create 'molecule' objects under model root object and 'atom' objects under 'molecule'-s
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {

        MCCAD_ORB::ComponentList * aComponentList = engine->getData(studyID);
        if (!aComponentList)
        {
            MESSAGE("component list is emtpy!")
            return;
        }
        //*********LOOP for Components*****
        for (int i=0; i< aComponentList->length(); i++)
        {
            MCCAD_ORB::Component_var aComponent = (*aComponentList)[i];
            MCCADGUI_DataObject * aComponentObject  = new MCCADGUI_DataObject (modelRoot,aComponent->getID());

            MCCAD_ORB::GroupList * aGroupList = aComponent->getGroupList();
            //*********LOOP for Groups*****
            for (int j=0; j<aGroupList->length(); j++)
            {
                MCCAD_ORB::Group_var aGroup = (*aGroupList)[j];
                MCCADGUI_DataObject * aGroupObject =  new MCCADGUI_DataObject(aComponentObject,aComponent->getID(), aGroup->getID() );

                MCCAD_ORB::PartList * aPartList = aGroup->getPartList();
                //*********LOOP for Parts*****
                for (int k=0; k<aPartList->length(); k++)
                {
                    MCCAD_ORB::Part_var aPart = (*aPartList)[k];
                    new MCCADGUI_DataObject(aGroupObject,aComponent->getID(), aGroup->getID(), aPart->getID() );
                }
            }
        }
    }
    modelRoot->setDataModel( this );
    setRoot( modelRoot );
    return;
}


void MCCADGUI_DataModel::buildTree()

{
//    if (count != 5) {
    MCCADGUI_ModuleObject* modelRoot = dynamic_cast<MCCADGUI_ModuleObject*>( root() );
    if( !modelRoot )  {  // root is not set yet
        modelRoot = new MCCADGUI_ModuleObject( this, 0 );
        setRoot( modelRoot );
    }

    // create 'molecule' objects under model root object and 'atom' objects under 'molecule'-s
    MCCAD_ORB::MCCAD_Gen_var engine = MCCADGUI::GetMCCADGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {

        MCCAD_ORB::ComponentList * aComponentList = engine->getData(studyID);
        if (!aComponentList)
        {
            MESSAGE("component list is emtpy!")
            return;
        }
        const int n = aComponentList->length();
        //*********LOOP for Components*****
        for (int i=0; i<n; i++)
        {
            MCCAD_ORB::Component_var aComponent = (*aComponentList)[i];
            MCCADGUI_DataObject * aComponentObject = NULL;
            for ( SUIT_DataObjectIterator it( modelRoot, SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
            {
                MCCADGUI_DataObject* obj = dynamic_cast<MCCADGUI_DataObject*>( it.current() );
                if (obj && obj->isComponent() && obj->getComponentID() == aComponent->getID())
                {
                    aComponentObject = obj;
                    break;
                }
            }
            if (aComponentObject == NULL)
                aComponentObject = new MCCADGUI_DataObject (modelRoot,aComponent->getID());

            MCCAD_ORB::GroupList * aGroupList = aComponent->getGroupList();
            const int m = aGroupList->length();
            //*********LOOP for Groups*****
            for (int j=0; j<m; j++)
            {
                MCCAD_ORB::Group_var aGroup = (*aGroupList)[j];
                MCCADGUI_DataObject * aGroupObject = NULL;
                for ( SUIT_DataObjectIterator it( modelRoot, SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
                {
                    MCCADGUI_DataObject* obj = dynamic_cast<MCCADGUI_DataObject*>( it.current() );
                    if (obj && obj->isGroup() && obj->getGroupID() == aGroup->getID())
                    {
                        aGroupObject = obj;
                        break;
                    }
                }
                if (aGroupObject == NULL)
                    aGroupObject = new MCCADGUI_DataObject(aComponentObject,aComponent->getID(), aGroup->getID() );

                MCCAD_ORB::PartList * aPartList = aGroup->getPartList();
                const int m = aPartList->length();
                //*********LOOP for Parts*****
                for (int k=0; k<m; k++)
                {
                    MCCAD_ORB::Part_var aPart = (*aPartList)[k];
                    MCCADGUI_DataObject * aPartObject = NULL;
                    for ( SUIT_DataObjectIterator it( modelRoot, SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
                    {
                        MCCADGUI_DataObject* obj = dynamic_cast<MCCADGUI_DataObject*>( it.current() );
                        if (obj && obj->isPart() && obj->getPartID() == aPart->getID())
                        {
                            aPartObject = obj;
                            break;
                        }
                    }
                    if (aPartObject == NULL)
                        aPartObject = new MCCADGUI_DataObject(aGroupObject,aComponent->getID(), aGroup->getID(), aPart->getID() );
                }
            }
        }
    }
    modelRoot->setDataModel( this );
    setRoot( modelRoot );
    return;
}





