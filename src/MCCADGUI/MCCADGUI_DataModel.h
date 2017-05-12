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


#if !defined(MCCADGUI_DATAMODEL_H)
#define MCCADGUI_DATAMODEL_H
#include "MCCADGUI.h"
#include "MCCADGUI_DataObject.h"
#include <LightApp_DataModel.h>
#include <SUITApp_Application.h>
#include <SUIT_Desktop.h>
#include <CAM_Module.h>
#include <CAM_Application.h>
#include <vector>

#include <QColor>

#include <SALOME_InteractiveObject.hxx>


//OCC include files
#include <TopoDS_Shape.hxx>


using namespace std;

class CAM_Module;


class MCCADGUI_DataModel : public LightApp_DataModel
{
public:
  MCCADGUI_DataModel ( CAM_Module* );
  virtual ~MCCADGUI_DataModel();

  virtual bool          open(const QString&url, CAM_Study*study, QStringList listOfFiles);
  virtual bool          save(QStringList& theListOfFiles);
  virtual bool          saveAs(const QString&url, CAM_Study*study, QStringList& theListOfFiles);
  virtual bool          close();
  virtual bool          create(CAM_Study* study);
  virtual bool          isModified() const;
  virtual bool          isSaved() const;
  virtual void          update( LightApp_DataObject* = 0, LightApp_Study* = 0 );

  int                   getStudyID();
  static int            getComponentID(const QString& aEntry );
  static int            getGroupID(const QString& aEntry );
  static int            getPartID (const QString& aEntry );
  static TopoDS_Shape   Stream2Shape(const SALOMEDS::TMPFile& aShapeStream);
  static SALOMEDS::TMPFile* Shape2Stream(const TopoDS_Shape& aShape);
  MCCADGUI_DataObject * lastComponent ();
  MCCADGUI_DataObject * lastGroup();



  MCCADGUI_DataObject * findObject(const QString& aEntry);
  Handle(SALOME_InteractiveObject)  findObjectIO(const QString& aEntry);

  SUIT_Desktop*         desktop() { return module()->application()->desktop(); }
  MCCADGUI *            getMCCADGUImodule() { return dynamic_cast< MCCADGUI*> ( module() );}
  int                   getPastePartsSize() { return PasteParts->length() ;}
  int                   getPasteGroupsSize() { return PasteGroups->length() ;}

  bool                  generateComponent(const QString& aComponentName);
  bool                  generateGroup(const QString& aGroupName, const int &ComponentID);
  void                  deleteComponent(const QString& aEntry);
  void                  deleteComponents(const QStringList& aList);
  void                  renameComponent(const QString& aEntry, const QString& newName);
  void                  deleteGroup(const QString& aEntry);
  void                  deleteGroups(const QStringList& aList);
  void                  renameGroup(const QString& aEntry, const QString& newName);
  void                  deletePart (const QString& aEntry);
  void                  deleteParts (const QStringList& aList);
  void                  renamePart(const QString& aEntry, const QString& newName);
  void                  copyPart(const QString& aEntry);
  void                  copyParts(const QStringList& aList, const bool &emptyList);
  void                  cutPart(const QString& aEntry);
  void                  cutParts(const QStringList& aList, const bool &emptyList);
  void                  pasteParts(const QString& aEntry);
  void                  copyGroup(const QString& aEntry);
  void                  copyGroups(const QStringList& aList, const bool &emptyList);
  void                  cutGroup(const QString& aEntry);
  void                  cutGroups(const QStringList& aList, const bool &emptyList);
  void                  pasteGroups(const QString& aEntry);
  bool                  generateMaterial(const int & MaterialID, const QString & Name, const double & Density,
                                         const QString & MaterialCard, const QColor & aColor= QColor() );
  bool                  updateMaterial(const int & MaterialID, const int & newMaterialID,const QString & Name,
                                       const double & Density, const QString & MaterialCard, const QColor & aColor= QColor());
  bool                  deleteMaterial(const int & MaterialID);
  bool                  assignMaterial (const QStringList& aList, const int & MaterialID, const bool & isReColor);
  bool                  saveMaterial2Xml(const QString &FileName);
  bool                  loadMaterialXml(const QString &FileName);
  bool                  formNewGroup(const QStringList& aList,  const QString &aName);
  bool                  formNewComponent(const QStringList& aList,  const QString &aName);
  bool                  explodePart(const QString& aEntry);
  bool                  fuseParts  (const QStringList& aList,  const QString &aName );

  bool                  recoverShapeInParts (const QStringList& aList);
  bool                  decomposeShapes(const QStringList& aList);
  bool                  decomposeShapes(const DataObjectList & aObjList);
  bool                  send2GEOM(const QStringList& aList);
  bool                  sendShape2GEOM(const SALOMEDS::TMPFile& aShapeStream, const QString &aName);
  bool                  assignShape(const QString& aEntry, const TopoDS_Shape& aShape, const QString &aName = QString());
  bool                  sendMesh2SMESH(const QStringList& aList);
  bool                  assignMesh(const QStringList& aEntryList, const QStringList & aSMESHEntryList  , const QStringList &aNameList);
  bool                  clearMesh (const QStringList& aList);
  bool                  clearEnvelop (const QStringList& aList);
  bool                  assignImportance(const QStringList& aList, MCCAD_ORB::FixArray  aIMP);
  bool                  assignImportance(const DataObjectList & aObjList, MCCAD_ORB::FixArray  aIMP);
  bool                  assignRemark(const QStringList& aList, const QString & aRemark);
  bool                  assignAdditive(const QStringList& aList, const QString & aAdditive);
  bool                  assignAdditive(const DataObjectList & aObjList, const QString & aAdditive);
  bool                  exportAllMesh2Abaqus(const QString & FileName);
  bool                  importGeom(const QString & FileName, bool & isExplode,const int & ComponentID, const int & GroupID);
  bool                  checkBeforeConvert();
  bool                  checkSurface(const TopoDS_Shape & aShape, const QString &aName);
  bool                  checkSolid(const TopoDS_Shape & aShape);
  bool                  convert2MC(const QString &FileName);
  void                  setConvertPara();

  bool                  translateShapes(const QStringList& aList,MCCAD_ORB::FixArray aVector );
  bool                  rotateShapes(const QStringList& aList,MCCAD_ORB::FixArray aOrigin,  MCCAD_ORB::FixArray aDir, const double & aAngle);
  bool                  scaleShapes(const QStringList& aList,const double & aFactor );


  bool                  generateTetMesh(const DataObjectList & aObjList);
  bool                  generateTetMesh(const QStringList& aList);
  bool                  generateMesh();
  bool                  exportGeom(const QStringList& aList, const QString &FileName);

  bool                  markAllIfDecomposed(const bool & isDecomposed);
  bool                  markIfDecomposed(const QStringList& aList, const bool & isDecomposed);
  bool                  markIfDecomposed(const DataObjectList & aObjList, const bool & isDecomposed);


  bool                  importAbaqus(const QString &AbaqusFileName) ;




public:
  virtual void          build();
  void          buildTree();

private:
   MCCAD_ORB::PartList_var PasteParts ;
   MCCAD_ORB::GroupList_var PasteGroups;

   QString myStudyURL;  //store the path of the study



};

#endif // MCCADGUI_DATAMODEL_H
