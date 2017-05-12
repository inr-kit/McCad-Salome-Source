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

#ifndef _MCCADGUI_H_
#define _MCCADGUI_H_

#include <SalomeApp_Module.h>
#include <SALOMEconfig.h>
#include <SALOME_LifeCycleCORBA.hxx>
#include CORBA_CLIENT_HEADER(MCCAD)
#include CORBA_CLIENT_HEADER(GEOM_Gen)
#include CORBA_CLIENT_HEADER(SMESH_Gen)
#include CORBA_CLIENT_HEADER(MEDCouplingCorbaServant)
#include "MCCAD_Constants.h"
#include <QVariant>
#include <QDockWidget>
#include <SUIT_DataOwner.h>

class SalomeApp_Application;

class MCCADGUI: public SalomeApp_Module
{
  Q_OBJECT

public:
  MCCADGUI();

  void    initialize( CAM_Application* );
  void    windows( QMap<int, int>& ) const;
  void    viewManagers( QStringList& ) const;

  void    selected( QStringList&, const bool multiple);

  virtual QString engineIOR() const;

  static void InitMCCADGen( SalomeApp_Application* );
  static MCCAD_ORB::MCCAD_Gen_var GetMCCADGen();
  void    createPreferences();
  void    preferencesChanged( const QString& section, const QString& param );

  virtual LightApp_Displayer* displayer();
  virtual void updateObjBrowser(bool isUpdate = false, SUIT_DataObject* = 0 );
  GEOM::GEOM_Object_ptr   getGeom(const SUIT_DataOwner* theDataOwner, const bool extractReference );
  SMESH::SMESH_Mesh_var   getSmesh(const SUIT_DataOwner* theDataOwner, const bool extractReference );


protected:
  enum { unittest               = 901,

         lgImportGeom           = 902,
         lgCreateGroup             = 1001,
         lgDeleteGroups            = 1002,
         lgDeleteParts            = 1003,
         lgRenameGroup             = 1004,
         lgRenamePart              = 1005,
         lgCopyParts              = 1006,
         lgCutParts               =1007,
         lgPasteParts             =1008,
         lgCreateComponent       = 1009,
         lgDeleteComponents      = 1010,
         lgRenameComponent       = 1011,
         lgRecoverShape             = 1012,
         lgDecompose                = 1013,
         lgSend2GEOM                = 1014,
         lgImportGEOMObj               = 1015,
         lgSendMesh2SMESH           = 1016,
         lgImportSMESHObj               = 1017,
         lgClearMesh                = 1018,
         lgExportMesh2Abaqus               = 1019,
         lgClearEnvelop             = 1020,
         lgCheckBeforeConvert       = 1021,
         lgConvert2MCInput          = 1022,

         lgFormNewGroup             = 1023,
         lgFormNewComponent         = 1024,
         lgCopyGroups              = 1025,
         lgCutGroups               =1026,
         lgPasteGroups             =1027,
         lgExplodePart             =1028,
         lgFuseParts             =1029,
   #ifdef WITH_TETGEN
         lgGenerateTetMesh             =1030,
         lgGenerateAllMesh             =1031,
   #endif
         lgExportGeom                = 1032,

//         lgDisplayInPV             =1101,
         lgDisplay                 =1102,
         lgErase                   =1103,
         lgShading                 =1104,
         lgWireframe               =1105,
         lgShadingWithWireFrame    =1106,
         lgTransparency            =1107,
         lgDisplayGEOMObjInPV      =1108,
         lgDisplayOnly             =1109,

         lgEditMaterial            =1110,
         lgSetProperties            =1111,
         lgTransform                =1112,
         lgMarkAsDecomposed        =1113,
         lgMarkAsUndecomposed        =1114,
         lgMarkAllAsDecomposed        =1115,
         lgMarkAllAsUndecomposed        =1116,

         lgImportAbaqus        =1117,

         lgExperiment              =1999

       };

  virtual CAM_DataModel*      createDataModel();
  virtual LightApp_Operation* createOperation( const int ) const;
  virtual LightApp_Selection* createSelection() const;

public slots:
  virtual bool    deactivateModule( SUIT_Study* );
  virtual bool    activateModule( SUIT_Study* );
  virtual void    studyClosed( SUIT_Study* );
  virtual void    selectionChanged();

signals:
  void            selectChg();

protected slots:
  void    OnUnitTest();
  void    onProcess();
  void    onOperation();
  void    onImportExport();
  void    onDisplayGEOMObjInPVIS();

  void    OnDisplayerCommand();
  void    OnExperiment();
  void    onEditMaterial();
  void    onSetProproties();
  void    onImportGEOMObj();
  void    onImportSMESHObj();
  void    onTransform();


public:
  void    display( LightApp_Displayer  * aDisplayer);
  void    erase(LightApp_Displayer  * aDisplayer);
  void    redisplay( LightApp_Displayer  * aDisplayer);
  void    setProperty (const QStringList & entries, MCCAD::Property aProp , QVariant aValue);

private:

  static MCCAD_ORB::MCCAD_Gen_var myEngine;
  QString LastDir;
  QString LastGroupName;
  QString LastComponentName;
  QDockWidget *myInputPanel;    // for property dialog

};

#endif
