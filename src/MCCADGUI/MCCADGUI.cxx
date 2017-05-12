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


//myself
#include "MCCADGUI.h"
#include "MCCADGUI_DataModel.h"
#include "MCCADGUI_Displayer.h"
#include "MCCADGUI_Selection.h"
#include "MCCADGUI_Dialogs.h"
#include "MCCADGUI_OperationImpl.h"

//MEDCoupling
#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingMeshServant.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"
#include "MEDCouplingAutoRefCountObjectPtr.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDLoader.hxx"

//SALOME
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_Tools.h>
#include <SUIT_ViewManager.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>

#include <SOCC_ViewModel.h>
#include <SOCC_ViewWindow.h>
#include <OCCViewer_ViewWindow.h>

//#include <SALOME_Actor.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Tools.h>
#include <LightApp_DataOwner.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_Preferences.h>

#include "SALOME_InteractiveObject.hxx"
#include "SALOMEDSClient_SObject.hxx"
#include "SALOMEDS_SObject.hxx"
#include "SALOME_ListIteratorOfListIO.hxx"


// QT includes
#include <qinputdialog.h>
#include <qaction.h>
#include <qstringlist.h>
#include <QtxPopupMgr.h>
#include <QDockWidget>
#include <QVBoxLayout>

// VTK includes
#include <vtkActorCollection.h>
#include <vtkRenderer.h>

//GEOM
#include <GEOM_Client.hxx>
#include <GeometryGUI.h>

//stardart c++
#include <iostream>
#include <string>


using namespace std;

MCCAD_ORB::MCCAD_Gen_var MCCADGUI::myEngine = MCCAD_ORB::MCCAD_Gen::_nil();

// Constructor
MCCADGUI::MCCADGUI() :
    SalomeApp_Module( "MCCAD" ), // default name
//    LightApp_Module( "MCCAD" ), // default name //2013-05-23 modif to adapt change in SALOME 7.2.0

    LastDir("~/"),
  LastGroupName("newGroup"),
  LastComponentName("newComponent")

{
}



void MCCADGUI::createPreferences()
{
    int tabId = addPreference( "McCad Settings" );

    int genGroup = addPreference( "Graphic", tabId );
  setPreferenceProperty( genGroup, "columns", 2 );

  int dispmode = addPreference( "Default dispaly mode", genGroup,
                                LightApp_Preferences::Selector,
                                "MCCAD", "display_mode" );

  addPreference( "Default dispaly color", genGroup,
                 LightApp_Preferences::Color, "MCCAD", "shading_color" );

  addPreference( "Edge in shading" , genGroup,
                 LightApp_Preferences::Color, "MCCAD", "edges_in_shading_color" );

  addPreference( "Default wireframe color", genGroup,
                 LightApp_Preferences::Color, "MCCAD", "wireframe_color" );

  addPreference( "Color for free boundary" , genGroup,
                 LightApp_Preferences::Color, "MCCAD", "free_bound_color" );

  addPreference( "Color for lines" , genGroup,
                 LightApp_Preferences::Color, "MCCAD", "line_color" );

  addPreference( "Color for points", genGroup,
                 LightApp_Preferences::Color, "MCCAD", "point_color" );

  addPreference( "Color for isolines", genGroup,
                 LightApp_Preferences::Color, "MCCAD", "isos_color" );

  addPreference( "Top level color", genGroup,
                 LightApp_Preferences::Color, "MCCAD", "toplevel_color" );

  int top_lev_dm = addPreference( "Top level display mode", genGroup,
                      LightApp_Preferences::Selector, "MCCAD", "toplevel_dm" );

  const int nb = 2;
  int wd[nb];
  int iter=0;

  wd[iter++] = addPreference("Edge width", genGroup,
                              LightApp_Preferences::IntSpin, "MCCAD", "edge_width" );

  wd[iter++] = addPreference( "Isolines width", genGroup,
                              LightApp_Preferences::IntSpin, "MCCAD", "isolines_width" );


  for (int i = 0; i < nb; i++) {
    setPreferenceProperty( wd[i], "min", 1 );
    setPreferenceProperty( wd[i], "max", 5 );
  }

  int isoU = addPreference( "Isoline long U", genGroup,
                                  LightApp_Preferences::IntSpin, "MCCAD", "iso_number_u" );
  setPreferenceProperty( isoU, "min", 0 );
  setPreferenceProperty( isoU, "max", 100000 );
  int isoV = addPreference("Isoline long V", genGroup,
                                 LightApp_Preferences::IntSpin, "MCCAD", "iso_number_v" );
  setPreferenceProperty( isoV, "min", 0 );
  setPreferenceProperty( isoV, "max", 100000 );

  // Set property for default display mode
  QStringList aModesList;
  aModesList.append( "Wireframe" );
  aModesList.append( "Shading" );
  aModesList.append( "Shading with edges" );

  QList<QVariant> anIndexesList;
  anIndexesList.append(0);
  anIndexesList.append(1);
  anIndexesList.append(2);

  setPreferenceProperty( dispmode, "strings", aModesList );
  setPreferenceProperty( dispmode, "indexes", anIndexesList );


  // Set property for top level display mode
  QStringList aTopModesList;
  aTopModesList.append( "Show additional wireframe" );
  aTopModesList.append( "Keep current mode" );
  aTopModesList.append( "Wireframe");
  aTopModesList.append( "Shading" );
  aTopModesList.append( "Shading with edges" );

  QList<QVariant> aTopIndexesList;
  aTopIndexesList.append(0);
  aTopIndexesList.append(1);
  aTopIndexesList.append(2);
  aTopIndexesList.append(3);
  aTopIndexesList.append(4);
  setPreferenceProperty( top_lev_dm, "strings", aTopModesList );
  setPreferenceProperty( top_lev_dm, "indexes", aTopIndexesList );

  //#######prefrence for MCCAD conversion ####
  int ConvertGroup = addPreference( "General conversion", tabId );
  setPreferenceProperty( ConvertGroup, "columns", 1 );

  int targetCode = addPreference( "Target MC code", ConvertGroup,
                                LightApp_Preferences::Selector,
                                "MCCAD", "target_code" );
  QStringList aCodesList;
  aCodesList.append( "MCNP5" );
  aCodesList.append( "MCNP6" );
  aCodesList.append( "TRIPOLI" );
  aCodesList.append( "GDML" );
  QList<QVariant> aCodeIdxList;
  aCodeIdxList.append(0);
  aCodeIdxList.append(1);
  aCodeIdxList.append(2);
  aCodeIdxList.append(3);
  setPreferenceProperty( targetCode, "strings", aCodesList );
  setPreferenceProperty( targetCode, "indexes", aCodeIdxList );

  //initial cell and surface No.
  int init_cell_no = addPreference("Start number cell with ", ConvertGroup,
                                LightApp_Preferences::IntSpin, "MCCAD", "init_cell_no" );
  int init_surf_no = addPreference("Start number Surface with ", ConvertGroup,
                                LightApp_Preferences::IntSpin, "MCCAD", "init_surf_no" );
  setPreferenceProperty( init_cell_no, "min", 1 );
  setPreferenceProperty( init_cell_no, "max", 999999 );
  setPreferenceProperty( init_surf_no, "min", 1 );
  setPreferenceProperty( init_surf_no, "max", 999999 );


  int min_solid_vol = addPreference( "Minimum solid volume", ConvertGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "min_solid_vol" );
  setPreferenceProperty( min_solid_vol, "min", 0.001 );
  setPreferenceProperty( min_solid_vol, "max", 10 );
  setPreferenceProperty( min_solid_vol, "step", 1.0e-03 );
  setPreferenceProperty( min_solid_vol, "precision", 6 );

  int min_void_vol = addPreference( "Minimum void volume", ConvertGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "min_void_vol" );
  setPreferenceProperty( min_void_vol, "min", 0.1 );
  setPreferenceProperty( min_void_vol, "max", 10000 );
  setPreferenceProperty( min_void_vol, "step", 1.0e-01 );
  setPreferenceProperty( min_void_vol, "precision", 6 );

  int min_dec_face_area = addPreference( "Minimum Size Of Decomposition Face Area", ConvertGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "min_dec_face_area" );
  setPreferenceProperty( min_dec_face_area, "min", 0.1 );
  setPreferenceProperty( min_dec_face_area, "max", 1000 );
  setPreferenceProperty( min_dec_face_area, "step", 1.0e-01 );
  setPreferenceProperty( min_dec_face_area, "precision", 6 );

  int min_compl_cell = addPreference( "Maximum Number Of Complemented Cells ", ConvertGroup,
                            LightApp_Preferences::IntSpin, "MCCAD", "min_compl_cell" );
  setPreferenceProperty( min_compl_cell, "min", 0 );
  setPreferenceProperty( min_compl_cell, "max", 100000 );
  setPreferenceProperty( min_compl_cell, "step", 1 );

  int min_pre_dec_cell = addPreference( "Maximum Number Of Pre-Decomposition Cells", ConvertGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "min_pre_dec_cell" );
  setPreferenceProperty( min_pre_dec_cell, "min", 0 );
  setPreferenceProperty( min_pre_dec_cell, "max", 100000 );
  setPreferenceProperty( min_pre_dec_cell, "step", 1 );

  int min_redec_face_area = addPreference( "Minimum Size Of Redecomposition Face Area", ConvertGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "min_redec_face_area" );
  setPreferenceProperty( min_redec_face_area, "min", 0.1 );
  setPreferenceProperty( min_redec_face_area, "max", 1000 );
  setPreferenceProperty( min_redec_face_area, "step", 1.0e-01 );
  setPreferenceProperty( min_redec_face_area, "precision", 6 );

  int min_sample_points = addPreference( "Minimum Number Of Sample Points", ConvertGroup,
                            LightApp_Preferences::IntSpin, "MCCAD", "min_sample_points" );
  setPreferenceProperty( min_sample_points, "min", 1 );
  setPreferenceProperty( min_sample_points, "max", 10000 );
  setPreferenceProperty( min_sample_points, "step", 1 );

  int max_sample_points = addPreference( "Maximum Number Of Sample Points", ConvertGroup,
                            LightApp_Preferences::IntSpin, "MCCAD", "max_sample_points" );
  setPreferenceProperty( max_sample_points, "max", 1 );
  setPreferenceProperty( max_sample_points, "max", 10000 );
  setPreferenceProperty( max_sample_points, "step", 1 );

  int x_resl = addPreference( "Resolution in X direction", ConvertGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "x_resl" );
  setPreferenceProperty( x_resl, "min", 0.00001 );
  setPreferenceProperty( x_resl, "max", 10 );
  setPreferenceProperty( x_resl, "step", 1.0e-05 );
  setPreferenceProperty( x_resl, "precision", 6 );


  int y_resl = addPreference( "Resolution in Y direction", ConvertGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "y_resl" );
  setPreferenceProperty( y_resl, "min", 0.00001 );
  setPreferenceProperty( y_resl, "max", 10 );
  setPreferenceProperty( y_resl, "step", 1.0e-05 );
  setPreferenceProperty( y_resl, "precision", 6 );

  int r_resl = addPreference( "Resolution in R direction", ConvertGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "r_resl" );
  setPreferenceProperty( r_resl, "min", 0.00001 );
  setPreferenceProperty( r_resl, "max", 10 );
  setPreferenceProperty( r_resl, "step", 1.0e-05 );
  setPreferenceProperty( r_resl, "precision", 6 );

  int max_dec_depth = addPreference( "Max Decompose Depth", ConvertGroup,
                            LightApp_Preferences::IntSpin, "MCCAD", "max_dec_depth" );
  setPreferenceProperty( max_dec_depth, "max", 1 );
  setPreferenceProperty( max_dec_depth, "max", 1000 );
  setPreferenceProperty( max_dec_depth, "step", 1 );

  int max_cell_exp_depth = addPreference( "Max Cell Expression Length", ConvertGroup,
                            LightApp_Preferences::IntSpin, "MCCAD", "max_cell_exp_depth" );
  setPreferenceProperty( max_cell_exp_depth, "max", 1 );
  setPreferenceProperty( max_cell_exp_depth, "max", 1000 );
  setPreferenceProperty( max_cell_exp_depth, "step", 1 );

  int Tolerance = addPreference( "Tolerance", ConvertGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "Tolerance" );
  setPreferenceProperty( Tolerance, "min", 1.0e-7 );
  setPreferenceProperty( Tolerance, "max", 0.1 );
  setPreferenceProperty( Tolerance, "step", 1.0e-07 );
  setPreferenceProperty( Tolerance, "precision", 7 );

  addPreference("Write discrete files ", ConvertGroup,
                                LightApp_Preferences::Bool, "MCCAD", "write_disc_file" );
  addPreference("Write discrete model ", ConvertGroup,
                                LightApp_Preferences::Bool, "MCCAD", "write_disc_model" );
  addPreference("Generate void ", ConvertGroup,
                                LightApp_Preferences::Bool, "MCCAD", "gen_void" );
  addPreference("Force decompose ", ConvertGroup,
                                LightApp_Preferences::Bool, "MCCAD", "force_decomp" );


  //#######prefrence for MCNP6 conversion ####
  int MCNP6Group = addPreference( "MCNP6", tabId );
  setPreferenceProperty( MCNP6Group, "columns", 1 );

  int init_pseudo_cell_no = addPreference("Start pseudo cell number with ", MCNP6Group,
                                LightApp_Preferences::IntSpin, "MCCAD", "init_pseudo_cell_no" );
  int embed_card_no = addPreference("MCNP6 Embed card with number", MCNP6Group,
                                LightApp_Preferences::IntSpin, "MCCAD", "embed_card_no" );
  setPreferenceProperty( init_pseudo_cell_no, "min", 1 );
  setPreferenceProperty( init_pseudo_cell_no, "max", 99999 );
  setPreferenceProperty( embed_card_no, "min", 1 );
  setPreferenceProperty( embed_card_no, "max", 99999 );

  //input mesh type
  int MeshType = addPreference( "Input mesh type", MCNP6Group,
                                LightApp_Preferences::Selector,
                                "MCCAD", "MeshType" );
  QStringList aMeshTypeList;
  aMeshTypeList.append( "abaqus" );
  QList<QVariant> aMeshTypeIdxList;
  aMeshTypeIdxList.append(0);
  setPreferenceProperty( MeshType, "strings", aMeshTypeList );
  setPreferenceProperty( MeshType, "indexes", aMeshTypeIdxList );

  //EEOUT result output type
  int EeoutFileType = addPreference( "EEOUT file output type", MCNP6Group,
                                LightApp_Preferences::Selector,
                                "MCCAD", "EeoutFileType" );
  QStringList aEeoutFileTypeList;
  aEeoutFileTypeList.append( "binary" );
  aEeoutFileTypeList.append( "ascii" );
  QList<QVariant> aEeoutFileTypeIdxList;
  aEeoutFileTypeIdxList.append(0);
  aEeoutFileTypeIdxList.append(1);
  setPreferenceProperty( EeoutFileType, "strings", aEeoutFileTypeList );
  setPreferenceProperty( EeoutFileType, "indexes", aEeoutFileTypeIdxList );

  //overlap treament
  int OverlapTreat = addPreference( "Overlap treament", MCNP6Group,
                                LightApp_Preferences::Selector,
                                "MCCAD", "OverlapTreat" );
  QStringList aOverlapTreatList;
  aOverlapTreatList.append( "EXIT" );
  aOverlapTreatList.append( "ENTRY" );
  aOverlapTreatList.append( "AVERAGE" );
  QList<QVariant> aOverlapTreatIdxList;
  aOverlapTreatIdxList.append(0);
  aOverlapTreatIdxList.append(1);
  aOverlapTreatIdxList.append(2);
  setPreferenceProperty( OverlapTreat, "strings", aOverlapTreatList );
  setPreferenceProperty( OverlapTreat, "indexes", aOverlapTreatIdxList );

  //length conversion factor
  int LengthFactor = addPreference( "Length conversion factor", MCNP6Group,
                            LightApp_Preferences::DblSpin, "MCCAD", "LengthFactor" );
  setPreferenceProperty( LengthFactor, "min", 0 );
  setPreferenceProperty( LengthFactor, "step", 1.0 );
  setPreferenceProperty( LengthFactor, "precision", 6 );

  //#######prefrence for Tetgen mesh generation ####
  int TetMeshGroup = addPreference( "Mesh generation", tabId );
  setPreferenceProperty( TetMeshGroup, "columns", 1 );

  int Mesher = addPreference( "Meshing engine", TetMeshGroup,
                                LightApp_Preferences::Selector,
                                "MCCAD", "meshing_engine" );
  QStringList aMesherList;
  aMesherList.append( "Tetgen" );
  aMesherList.append( "Netgen" );
  QList<QVariant> aMesherIdxList;
  aMesherIdxList.append(0);
  aMesherIdxList.append(1);
  setPreferenceProperty( Mesher, "strings", aMesherList );
  setPreferenceProperty( Mesher, "indexes", aMesherIdxList );


  int STL_Deflection = addPreference( "Surface triangulation deflection", TetMeshGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "STL_Deflection" );
  setPreferenceProperty( STL_Deflection, "min", 0.00001 );
  setPreferenceProperty( STL_Deflection, "max", 0.01 );
  setPreferenceProperty( STL_Deflection, "step", 0.0001 );
  setPreferenceProperty( STL_Deflection, "precision", 6 );

//  int STL_Coefficient = addPreference( "Surface triangulation coefficient", TetMeshGroup,
//                            LightApp_Preferences::DblSpin, "MCCAD", "STL_Coefficient" );
//  setPreferenceProperty( STL_Coefficient, "min", 0.00001 );
//  setPreferenceProperty( STL_Coefficient, "max", 0.001 );
//  setPreferenceProperty( STL_Coefficient, "step", 0.0001 );
//  setPreferenceProperty( STL_Coefficient, "precision", 6 );

  int Vol_Threshold = addPreference( "Allow smallest element volume(mm3)", TetMeshGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "Vol_Threshold" );
  setPreferenceProperty( Vol_Threshold, "min", 0.0 );
  setPreferenceProperty( Vol_Threshold, "max", 1e9 );
  setPreferenceProperty( Vol_Threshold, "step", 1 );
  setPreferenceProperty( Vol_Threshold, "precision", 13 );

  int Tet_MeshQuality = addPreference( "Tetgen mesh quality control", TetMeshGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "Tet_MeshQuality" );
  setPreferenceProperty( Tet_MeshQuality, "min", 1.01 );
  setPreferenceProperty( Tet_MeshQuality, "max", 2.0 );
  setPreferenceProperty( Tet_MeshQuality, "step", 0.05 );
  setPreferenceProperty( Tet_MeshQuality, "precision", 3 );

  int Mesh_Volume_Diff = addPreference( "Allow volume difference from CAD model", TetMeshGroup,
                            LightApp_Preferences::DblSpin, "MCCAD", "Mesh_Volume_Diff" );
  setPreferenceProperty( Mesh_Volume_Diff, "min", 0.00001 );
  setPreferenceProperty( Mesh_Volume_Diff, "max", 1.0 );
  setPreferenceProperty( Mesh_Volume_Diff, "step", 0.005 );
  setPreferenceProperty( Mesh_Volume_Diff, "precision", 5 );
}


void MCCADGUI::preferencesChanged( const QString& section, const QString& param )
{
  if (section == "MCCAD") {
    SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
    if (param == QString("toplevel_color")) {
      QColor c = aResourceMgr->colorValue( "MCCAD", "toplevel_color", QColor( 170, 85, 0 ) );
      MCCAD_AISShape::setTopLevelColor(SalomeApp_Tools::color(c));
    }
    else if (param == QString("toplevel_dm")) {
      MCCAD_AISShape::setTopLevelDisplayMode((MCCAD_AISShape::TopLevelDispMode)aResourceMgr->integerValue("MCCAD", "toplevel_dm", 0));
    }
  }
}



// returns a custom displayer object
LightApp_Displayer* MCCADGUI::displayer()
{
    return new MCCADGUI_Displayer();;
}

// Module's initialization
void MCCADGUI::initialize( CAM_Application* app )
{

    SalomeApp_Module::initialize( app );

    InitMCCADGen( dynamic_cast<SalomeApp_Application*>( app ) );
    if (CORBA::is_nil( myEngine ))
        cout <<"initialize() load engine failed! "<<endl;

    SUIT_Desktop* aParent = app->desktop();
    //  SUIT_ResourceMgr* aResourceMgr = app->resourceMgr();

    //create the dock panel for property dialog
    myInputPanel = new QDockWidget(aParent);
    QLayout* inputLayout = new QVBoxLayout(aParent);
    myInputPanel->setLayout(inputLayout);
    myInputPanel->setVisible(true);
    myInputPanel->setWindowTitle("Input Panel");
    myInputPanel->setMinimumWidth(220); // --- force a minimum until display
    aParent->addDockWidget( Qt::LeftDockWidgetArea, myInputPanel );


    createAction( unittest,"UnitTest", QIcon(), "UnitTest",
                  "UnitTest", 0, aParent, false, this, SLOT( OnUnitTest() ) );

    createAction( lgImportGeom,"Import CAD file", QIcon(), "Import CAD file",
                  "Import CAD file", 0, aParent, false, this, SLOT( onImportExport()) );

    createAction( lgCreateComponent,"Create Component", QIcon(), "Create Component",
                  "Create Component", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgDeleteComponents,"Delete Component(s)", QIcon(), "Delete Component(s)",
                  "Delete Component(s)", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgRenameComponent,"Rename Component", QIcon(), "Rename ",
                  "Rename Component", 0, aParent, false, this, SLOT( onOperation() ) );

    createAction( lgCreateGroup,"Create Group", QIcon(), "Create Group",
                  "Create Group", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgDeleteGroups,"Delete Group(s)", QIcon(), "Delete Group(s)",
                  "Delete Group(s)", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgDeleteParts,"Delete part(s)", QIcon(), "Delete part(s)",
                  "Delete part(s)", 0, aParent, false, this, SLOT( onOperation() ) );

    createAction( lgRenameGroup,"Rename Group", QIcon(), "Rename ",
                  "Rename Group", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgRenamePart,"Rename Part", QIcon(), "Rename ",
                  "Rename Part", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgCopyParts,"Copy part(s)", QIcon(), "Copy parts",
                  "Copy part(s)", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgCutParts,"Cut part(s)", QIcon(), "Cut parts",
                  "Cut part(s)", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgPasteParts,"Paste part(s)", QIcon(), "Paste parts",
                  "Paste part(s)", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgRecoverShape,"Recover Shape", QIcon(), "Recover",
                  "Recover Shape", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgDecompose,"Decompose shape", QIcon(), "Decompose",
                  "Decompose shape", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgSend2GEOM,"Sent to GEOM ", QIcon(), "Sent to GEOM",
                  "Sent to GEOM ", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgImportGEOMObj,"Import GEOM Object", QIcon(), "Import GEOM Object",
                  "Import GEOM Object ", 0, aParent, false, this, SLOT( onImportGEOMObj() ) );
    createAction( lgSendMesh2SMESH,"Send mesh to SMESH ", QIcon(), "Send to SMESH",
                  "Send mesh to SMESH ", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgImportSMESHObj,"Import SMESH Object", QIcon(), "Import SMESH Object",
                  "Import SMESH Object ", 0, aParent, false, this, SLOT( onImportSMESHObj() ) );
    createAction( lgClearMesh,"Clear mesh", QIcon(), "Clear mesh",
                  "Clear mesh ", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgExportMesh2Abaqus,"Export Mesh to Abaqus", QIcon(), "Export Mesh to Abaqus",
                  "Export Mesh to Abaqus", 0, aParent, false, this, SLOT( onImportExport() ) );
    createAction( lgClearEnvelop,"Clear Envelop", QIcon(), "Clear Envelop",
                  "Clear Envelop ", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgCheckBeforeConvert,"Check before convert", QIcon(), "Check before convert",
                  "Check before convert ", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgConvert2MCInput,"Convert to MC Input", QIcon(), "Convert to MC Input",
                  "Convert to MC Input ", 0, aParent, false, this, SLOT( onImportExport() ) );

    createAction( lgFormNewGroup,"Form New Group", QIcon(), "Form new group ",
                  "Form New Group", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgFormNewComponent,"Form New Component", QIcon(), "Form new Component ",
                  "Form New Component", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgCopyGroups,"Copy Group(s)", QIcon(), "Copy groups",
                  "Copy Group(s)", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgCutGroups,"Cut Group(s)", QIcon(), "Cut groups",
                  "Cut Group(s)", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgPasteGroups,"Paste Group(s)", QIcon(), "Paste groups",
                  "Paste Group(s)", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgExplodePart,"Explode Part", QIcon(), "Explode Part",
                  "Explode Part", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgFuseParts,"Fuse Parts", QIcon(), "Fuse Parts",
                  "Fuse Parts", 0, aParent, false, this, SLOT( onOperation() ) );
#ifdef WITH_TETGEN
    createAction( lgGenerateTetMesh,"Generate Tetrahedral mesh ", QIcon(), "Generate Tetrahedral mesh",
                  "Generate Tetrahedral mesh ", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgGenerateAllMesh,"Generate all meshes", QIcon(), "Generate all meshes",
                  "Generate Meshes", 0, aParent, false, this, SLOT( onOperation() ) );
#endif
    createAction( lgExportGeom,"Export geometry", QIcon(), "Export geometry",
                  "Export geometry", 0, aParent, false, this, SLOT( onImportExport() ) );

    createAction( lgImportAbaqus,"Import Abaqus mesh", QIcon(), "Import Abaqus mesh",
                  "Import Abaqus mesh", 0, aParent, false, this, SLOT( onImportExport() ) );

    createAction( lgDisplayGEOMObjInPV,"Display GEOM Object in Paraview", QIcon(), "Display GEOM Object in Paraview",
                  "Display GEOM Object in Paraview", 0, aParent, false, this, SLOT( onDisplayGEOMObjInPVIS() ) );
    createAction( lgDisplay,"Show", QIcon(), "Show",
                  "Show", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgErase,"Hide", QIcon(), "Hide",
                  "Hide", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgDisplayOnly,"Show only", QIcon(), "Show only",
                  "Show only", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgShading,"Shading", QIcon(), "Shading",
                  "Shading", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgWireframe,"Wireframe", QIcon(), "Wireframe",
                  "Wireframe", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgShadingWithWireFrame,"ShadingWithWireFrame", QIcon(), "Shading With WireFrame",
                  "Shading With WireFrame", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgTransparency,"Transparency", QIcon(), "Transparency",
                  "Transparency", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );

    createAction( lgExperiment,"Experiment", QIcon(), "Experiment",
                  "Experiment", 0, aParent, false, this, SLOT( OnExperiment() ) );

    createAction( lgEditMaterial,"Edit Material", QIcon(), "Edit Material",
                  "Edit Material", 0, aParent, false, this, SLOT( onEditMaterial() ) );
    createAction( lgSetProperties,"Set Properties", QIcon(), "Set Properties",
                  "Set Properties", 0, aParent, false, this, SLOT( onSetProproties() ) );
    createAction( lgTransform,"Transform", QIcon(), "Transform",
                  "Transform", 0, aParent, false, this, SLOT( onTransform())  );
    createAction( lgMarkAsDecomposed,"Mark as Decomposed", QIcon(), "Mark as Decomposed",
                  "Mark as Decomposed", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgMarkAsUndecomposed,"Mark as Undecomposed", QIcon(), "Mark as Undecomposed",
                  "Mark as Undecomposed", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgMarkAllAsDecomposed,"Mark all as Decomposed", QIcon(), "Mark all as Decomposed",
                  "Mark all as Decomposed", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgMarkAllAsUndecomposed,"Mark all as Undecomposed", QIcon(), "Mark all as Undecomposed",
                  "Mark all as Undecomposed", 0, aParent, false, this, SLOT( onOperation() ) );


    // create menus
    int aFileMnu = createMenu( tr( "File" ), -1, -1 );
    createMenu( lgImportGeom, aFileMnu, 10 );
    createMenu( lgImportAbaqus, aFileMnu, 10 );
    createMenu( lgExportMesh2Abaqus, aFileMnu, 10 );
    createMenu( lgCheckBeforeConvert, aFileMnu, 10 );
    createMenu( lgConvert2MCInput, aFileMnu, 10 );

//    int aImportMCMnu = createMenu( tr( "Import MC Data" ), aFileMnu, -1, 10 );
//    int aImportExtDataMnu = createMenu( tr( "Import External Data" ), aFileMnu, -1, 10 );
//    int aImportExtPartMnu = createMenu( tr( "Import External Part" ), aFileMnu, -1, 10 );
//    int aExportMnu = createMenu( tr( "Export Data" ), aFileMnu, -1, 10 );
    int aMenuId = createMenu( "McCad", -1, -1, 30 );

//    createMenu( unittest, aMenuId, 10 );

//    createMenu( lgExperiment, aMenuId, 10 );
    createMenu( lgEditMaterial, aMenuId, 10 );
    createMenu( lgSetProperties, aMenuId, 10 );
    createMenu( lgTransform, aMenuId, 10 );
#ifdef WITH_TETGEN
    createMenu( lgGenerateAllMesh, aMenuId, 10 );
#endif
    createMenu( lgMarkAllAsDecomposed, aMenuId, 10 );
    createMenu( lgMarkAllAsUndecomposed, aMenuId, 10 );

    QtxPopupMgr* mgr = popupMgr();

    mgr->insert( action( lgRenameComponent ), -1, 0 );
    mgr->insert( action( lgRenameGroup ), -1, 0 );
    mgr->insert( action( lgRenamePart ), -1, 0 );
    mgr->insert( action( lgCopyParts ), -1, 0 );
    mgr->insert( action( lgCutParts ), -1, 0 );
    mgr->insert( action( lgPasteParts ), -1, 0 );
    mgr->insert( action( lgCopyGroups ), -1, 0 );
    mgr->insert( action( lgCutGroups ), -1, 0 );
    mgr->insert( action( lgPasteGroups ), -1, 0 );
    mgr->insert( action( lgDeleteComponents ), -1, 0 );
    mgr->insert( action( lgDeleteGroups ), -1, 0 );
    mgr->insert( action( lgDeleteParts ), -1, 0 );
    mgr->insert( separator(),            -1, 0  );
    mgr->insert( action( lgFormNewGroup ), -1, 0 );
    mgr->insert( action( lgFormNewComponent), -1, 0 );
    mgr->insert( action( lgExplodePart), -1, 0 );
    mgr->insert( action( lgFuseParts), -1, 0 );
    mgr->insert( action( lgClearMesh ), -1, 0 );
    mgr->insert( action( lgClearEnvelop ), -1, 0 );

    mgr->insert( separator(),            -1, 0  );
    mgr->insert( action( lgCreateComponent ), -1, 0 );
    mgr->insert( action( lgCreateGroup ), -1, 0 );
    mgr->insert( separator(),            -1, 0  );
    mgr->insert( action( lgDisplay ), -1, 0 );
    mgr->insert( action( lgDisplayOnly ), -1, 0 );
    mgr->insert( action( lgErase ), -1, 0 );
    int dispmodeId = mgr->insert(  tr( "Display Mode" ), -1, -1 ); // display mode menu
    mgr->insert( action(  lgShadingWithWireFrame ), dispmodeId, -1 );
    mgr->insert( action(  lgWireframe ), dispmodeId, -1 );
    mgr->insert( action(  lgShading ), dispmodeId, -1 );
    mgr->insert( action(  lgTransparency ), -1, 0 );

//    mgr->insert( action( lgDisplayInPV ), -1, 0 );
//    mgr->insert( action( lgDisplayGEOMObjInPV), -1, 0 );
    mgr->insert( separator(),            -1, 0  );
    mgr->insert( action( lgDecompose ), -1, 0 );
    mgr->insert( action( lgRecoverShape ), -1, 0 );
    mgr->insert( action( lgMarkAsDecomposed ), -1, 0 );
    mgr->insert( action( lgMarkAsUndecomposed ), -1, 0 );

    mgr->insert( separator(),            -1, 0  );

    mgr->insert( action( lgSend2GEOM ), -1, 0 );
    mgr->insert( action( lgSendMesh2SMESH ), -1, 0 );
    mgr->insert( action( lgExportGeom ), -1, 0 );
    mgr->insert( action( lgImportGEOMObj ), -1, 0 );
    mgr->insert( action( lgImportSMESHObj ), -1, 0 );
    mgr->insert( separator(),            -1, 0  );

    mgr->insert( action( lgSetProperties), -1, 0 );
    mgr->insert( action( lgTransform), -1, 0 );
    #ifdef WITH_TETGEN
    mgr->insert( action( lgGenerateTetMesh ), -1, 0 );
    #endif


//    int aOpId = mgr->insert("Operation", -1, -1);

//    int aExpId = mgr->insert("Export", -1, -1);


    QString rule = "(client='ObjectBrowser') and activeModule='MCCAD' ";
    mgr->setRule( action( lgCreateComponent ), rule +
                  " and selcount=1 and isMCCADRootObj");
    mgr->setRule( action( lgCreateGroup ), rule +
                  " and selcount=1 and isComponent");
    mgr->setRule( action( lgDeleteComponents ), rule +
                  " and isComponent and selcount>0");
    mgr->setRule( action( lgDeleteGroups ), rule +
                  " and isGroup and selcount>0");
    mgr->setRule( action( lgRenameComponent ), rule +
                  " and isComponent and selcount=1");
    mgr->setRule( action( lgRenameGroup ), rule +
                  " and isGroup and selcount=1");

    rule = "(client='ObjectBrowser' or client='OCCViewer') and activeModule='MCCAD' ";
//    mgr->setRule( action( lgDisplayInPV ), rule +
//                  " and isPart and selcount=1 and true in $canBeDisplayed");
    mgr->setRule( action( lgRenamePart ), rule +
                  " and isPart and selcount=1");
    mgr->setRule( action( lgCopyParts ), rule +
                  " and isPart and selcount>0");
    mgr->setRule( action( lgCutParts ), rule +
                  " and isPart and selcount>0");
    mgr->setRule( action( lgPasteParts ), rule +
                  " and selcount=1 and ( isPart or isGroup ) and isPartPastable");
    mgr->setRule( action( lgDeleteParts ), rule +
                  " and isPart and selcount>0");
    mgr->setRule( action( lgCopyGroups ), rule +
                  " and isGroup and selcount>0");
    mgr->setRule( action( lgCutGroups ), rule +
                  " and isGroup and selcount>0");
    mgr->setRule( action( lgPasteGroups ), rule +
                  " and selcount=1 and ( isGroup or isComponent ) and isGroupPastable");

    mgr->setRule( action( lgDisplay ), rule +
                  " and selcount>0 and isMCCADObj and true in $canBeDisplayed and !isVisible");
    mgr->setRule( action( lgDisplayOnly ), rule +
                  " and selcount>0 and isMCCADObj and true in $canBeDisplayed ");
    mgr->setRule( action( lgErase ), rule +
                  " and selcount>0 and isMCCADObj and ( isVisible or isGroup or isComponent ) ");
    mgr->setRule( action( lgShading ), rule +
                  " and selcount>0 and true in $canBeDisplayed and (isVisible or isGroup or isComponent)");
    mgr->setRule( action( lgWireframe ), rule +
                  " and selcount>0 and true in $canBeDisplayed and (isVisible or isGroup or isComponent)");
    mgr->setRule( action( lgShadingWithWireFrame ), rule +
                  " and selcount>0 and true in $canBeDisplayed and (isVisible or isGroup or isComponent)");
    mgr->setRule( action( lgTransparency ), rule +
                  " and selcount>0 and true in $canBeDisplayed and (isVisible or isGroup or isComponent)");
//    mgr->setRule( action( lgDisplayGEOMObjInPV ), rule +
//                  " and (!isPart and !isGroup) and selcount=1 ");
    mgr->setRule( action( lgSetProperties ), rule +
                  " and selcount>0  and (isPart or isGroup or isComponent)");
    mgr->setRule( action( lgTransform ), rule +
                  " and selcount>0  and isPart");
    mgr->setRule( action( lgRecoverShape ), rule +
                  " and selcount>0 and isPart and hasBackup");
    mgr->setRule( action( lgDecompose ), rule +
                  " and selcount>0 and (!isDecomposed or isGroup or isComponent)");
    mgr->setRule( action( lgSend2GEOM ), rule +
                  " and selcount>0 and (isPart or isGroup or isComponent)");
    mgr->setRule( action( lgImportGEOMObj ), rule +
                  " and selcount=1 and isGEOMObj");
    mgr->setRule( action( lgSendMesh2SMESH ), rule +
                  " and selcount>0 and isPart and hasMesh");
    mgr->setRule( action( lgImportSMESHObj ), rule +
                  " and selcount>0 and isSMESHObj");
    mgr->setRule( action( lgClearMesh ), rule +
                  " and selcount>0 and isPart and hasMesh");
    mgr->setRule( action( lgClearEnvelop ), rule +
                  " and selcount>0 and isComponent and hasEnvelop");

    mgr->setRule( action( lgFormNewGroup ), rule +
                  " and selcount>0 and isPart");
    mgr->setRule( action( lgFormNewComponent ), rule +
                  " and selcount>0 and isGroup");
    mgr->setRule( action( lgExplodePart ), rule +
                  " and selcount=1 and isPart and isExplodible");
    mgr->setRule( action( lgFuseParts ), rule +
                  " and selcount>1 and isPart");
#ifdef WITH_TETGEN
    mgr->setRule( action( lgGenerateTetMesh ), rule +
                  " and selcount>0 and isPart");
#endif
    mgr->setRule( action( lgExportGeom ), rule +
                  " and selcount>0 and (isPart or isGroup or isComponent)");
    mgr->setRule( action( lgMarkAsDecomposed ), rule +
                  " and selcount>0 and ((isPart and !isDecomposed) or isGroup or isComponent)");
    mgr->setRule( action( lgMarkAsUndecomposed ), rule +
                  " and selcount>0 and ((isPart and isDecomposed) or isGroup or isComponent)");

}

//reimplement to avoid calling dm->build()
void MCCADGUI::updateObjBrowser(bool isUpdate, SUIT_DataObject *)
{
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    if (!dm)
    {
        MESSAGE("get Data Model failed");
        return;
    }
    dm->buildTree();
    SalomeApp_Module::updateObjBrowser(isUpdate);
}

// Default windows
void MCCADGUI::windows( QMap<int, int>& theMap ) const
{
    theMap.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
//    theMap.insert( SalomeApp_Application::WT_PyConsole,     Qt::BottomDockWidgetArea );
}

/*! Returns list of entities of selected objects. */
void MCCADGUI::selected( QStringList& entries, const bool multiple )
{
    LightApp_Application* app = getApp();
    LightApp_SelectionMgr* mgr = app ? app->selectionMgr() : 0;
    if( !mgr )   return;

    SUIT_DataOwnerPtrList anOwnersList;
    mgr->selected( anOwnersList );

    for ( int i = 0; i < anOwnersList.size(); i++ )
    {
        const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( anOwnersList[ i ].get() );
        QStringList es = owner->entry().split( "_" );
        if ( es.count() > 1 && es[ 0 ] == "MCCADGUI" && es[ 1 ] != "root" )
        {
            if ( !entries.contains( owner->entry() ) )
                entries.append( owner->entry() );
            if( !multiple )
                break;
        }
    }
}

// Module's engine IOR
QString MCCADGUI::engineIOR() const
{
    cout <<"****first call of engineIOR()"<<endl;
    CORBA::String_var anIOR = getApp()->orb()->object_to_string( GetMCCADGen() );
    return QString( anIOR.in() );
}

// Initialize a reference to the module's engine
void MCCADGUI::InitMCCADGen( SalomeApp_Application* app )
{
    if ( !app )
    {
        myEngine = MCCAD_ORB::MCCAD_Gen::_nil();
    }
    else
    {
        Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component( "FactoryServer", "MCCAD" );
        if ( CORBA::is_nil(comp  ) )
            cout << "InitMCCADGen(): the Component is nil!"<<endl;
        MCCAD_ORB::MCCAD_Gen_ptr mccad_gen = MCCAD_ORB::MCCAD_Gen::_narrow(comp);
        ASSERT( !CORBA::is_nil( mccad_gen ) );
        myEngine = mccad_gen;
    }
}

// Gets an reference to the module's engine
MCCAD_ORB::MCCAD_Gen_var MCCADGUI::GetMCCADGen()
{
    if ( CORBA::is_nil( myEngine ) ) {
        SUIT_Application* suitApp = SUIT_Session::session()->activeApplication();
        SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( suitApp );
        InitMCCADGen( app );
    }
    return myEngine;
}


// Default view managers
void MCCADGUI::viewManagers( QStringList& theViewMgrs ) const
{
  theViewMgrs.append( OCCViewer_Viewer::Type() );
}

// Create custom data model
CAM_DataModel* MCCADGUI::createDataModel()
{
    return new MCCADGUI_DataModel( this );
}

/*! Instantiation of a custom Operation object - component's action manager. */
LightApp_Operation* MCCADGUI::createOperation( const int id ) const
{
  switch( id )
  {
//  case lgEditMaterial:
//      return new MCCADGUI_EditMaterialOp();
  default:
    return 0;
  }
}

// Create custom selection object
LightApp_Selection* MCCADGUI::createSelection() const
{
  return new MCCADGUI_Selection();
}

// Module's deactivation
bool MCCADGUI::deactivateModule( SUIT_Study* theStudy )
{
    setMenuShown( false );
    setToolShown( false );
    if (myInputPanel) myInputPanel->close();  //close the widget


    return SalomeApp_Module::deactivateModule( theStudy );
}

// Module's activation
bool MCCADGUI::activateModule( SUIT_Study* theStudy )
{
    bool bOk = SalomeApp_Module::activateModule( theStudy );

    setMenuShown( true );
    setToolShown( true );

    return bOk;
}

// Study closed callback
void MCCADGUI::studyClosed( SUIT_Study* theStudy )
{
    SalomeApp_Module::studyClosed( theStudy );
}

//emit selection change signal for use
void    MCCADGUI::selectionChanged()
{
    emit selectChg();
}

void    MCCADGUI::OnUnitTest()
{
    MCCAD_ORB::MCCAD_Gen_var engine = GetMCCADGen();
//    engine->unittest();


}

/*!
 * \brief generate a new part
 */
void MCCADGUI::onProcess()
{
    if( sender() && sender()->inherits( "QAction" ) )
    {
      int id = actionId( ( QAction* )sender() );
      startOperation( id );
    }
}

void    MCCADGUI::onOperation()
{
    if( sender() && sender()->inherits( "QAction" ) )
    {
        int id = actionId( ( QAction* )sender() );
        //get selected objects
        QStringList aList ;
        selected(aList, true);

        MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
        MCCADGUI_Displayer * d = dynamic_cast <MCCADGUI_Displayer * > (displayer());

        if (!dm) {
            MESSAGE("get Data Model failed");
            return;
        }
        switch ( id )
        {
        case lgCreateComponent :
        {
            bool bOk;
            QString aName = QInputDialog::getText( getApp()->desktop(),tr( "Create Component" ),tr( "Component Name"),
                                                   QLineEdit::Normal,LastComponentName, &bOk);
            if (aName.trimmed().isEmpty() || bOk == false) {
                SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                          QString( "Name invalid or empty!"));
                return;
            }
            LastComponentName = aName;
            if (!dm->generateComponent(aName ) ) MESSAGE("Create new component failed!");
        } break;
        case lgDeleteComponents :
        {
            if (aList.size() == 0) return;
            QMessageBox::StandardButton aBtn = SUIT_MessageBox::warning(application()->desktop(),QString("Warning"),QString( "Delete item(s)?"),QMessageBox::Yes, QMessageBox::No);
            if (aBtn == QMessageBox::No) return;

            //first erase then delete
            erase(d);
            dm->deleteComponents(aList);
        } break;
        case lgRenameComponent :
        {
            if (aList.size() == 0) return;

            bool bOk;
            QString aName = QInputDialog::getText( application()->desktop(),tr( "Rename Component" ),tr( "Component Name"),
                                                   QLineEdit::Normal,dm->findObject(aList[0])->name(), &bOk);
            if (aName.trimmed().isEmpty() || bOk == false)
            {
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "Name invalid or empty!"));
                return;
            }
            dm->renameComponent(aList.at(0), aName);

        } break;
        case lgCreateGroup :
        { //see if any thing selected
            if (aList.size() == 0) {
                SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                          QString( "You should select a place to insert a group!"));
                return;
            }
            bool bOk;
            QString aName = QInputDialog::getText( getApp()->desktop(),tr( "Create Group" ),tr( "Group Name"),
                                                   QLineEdit::Normal, LastGroupName, &bOk);
            if (aName.trimmed().isEmpty() || bOk == false)
            {
                SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                          QString( "Name invalid or empty!"));
                return;
            }
            LastGroupName = aName;
            if (dm->generateGroup(aName, dm->getComponentID(aList.at(0)) ))
                MESSAGE("Generate a group failed!");

        }break;
        case lgDeleteGroups :
        {
            if (aList.size() == 0) return;

            QMessageBox::StandardButton aBtn = SUIT_MessageBox::warning(application()->desktop(),QString("Warning"),QString( "Delete item(s)?"),QMessageBox::Yes, QMessageBox::No);
            if (aBtn == QMessageBox::No) return;

            //first erase then delete
            erase(d);
            dm->deleteGroups(aList);
        } break;
        case lgRenameGroup :
        {
            if (aList.size() == 0) return;

            bool bOk;
            QString aName = QInputDialog::getText( application()->desktop(),tr( "Rename Group" ),tr( "Group Name"),
                                                   QLineEdit::Normal,dm->findObject(aList[0])->name(), &bOk);
            if (aName.trimmed().isEmpty() || bOk == false)
            {
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "Name invalid or empty!"));
                return;
            }
            dm->renameGroup(aList.at(0), aName);

        } break;
        case lgDeleteParts :
        {
            if (aList.size() == 0) return;
            //for confirmation
            QMessageBox::StandardButton aBtn = SUIT_MessageBox::warning(application()->desktop(),QString("Warning"),QString( "Delete item(s)?"),QMessageBox::Yes, QMessageBox::No);
            if (aBtn == QMessageBox::No) return;
            //first erase then delete
            erase(d);
            dm->deleteParts(aList);
        } break;
        case lgRenamePart :
        {
            if (aList.size() == 0) return;

            bool bOk;
            QString aName = QInputDialog::getText( application()->desktop(),tr( "Rename Part" ),tr( "Part Name"),
                                                   QLineEdit::Normal,dm->findObject(aList[0])->name(), &bOk);
            if (aName.trimmed().isEmpty() || bOk == false)
            {
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "Name invalid or empty!"));
                return;
            }
            dm->renamePart(aList.at(0), aName);
        } break;
        case lgCopyParts :
        {
            if (aList.size() == 0) return;
            bool emptyList = false;  //if the PasteParts should be empty or not
            if (dm->getPastePartsSize() > 0) { //if has some part in the list
                QMessageBox::StandardButton aBtn = SUIT_MessageBox::warning(application()->desktop(),
                                                 QString("Warning"),QString( "Overide previous copied/cut parts? "),
                                                 QMessageBox::Yes, QMessageBox::No);
                if (aBtn == QMessageBox::Yes) emptyList = true;
            }
            dm->copyParts(aList, emptyList);
        } break;
        case lgCutParts :
        {
            if (aList.size() == 0) return;
            bool emptyList = false;  //if the PasteParts should be empty or not
            if (dm->getPastePartsSize() > 0) { //if has some part in the list
                QMessageBox::StandardButton aBtn = SUIT_MessageBox::warning(application()->desktop(),
                                                 QString("Warning"),QString( "Overide previous copied/cut parts? "),
                                                 QMessageBox::Yes, QMessageBox::No);
                if (aBtn == QMessageBox::Yes) emptyList = true;
            }
            //first erase then cut
            erase(d);
            dm->cutParts(aList,emptyList);
        } break;
        case lgPasteParts :
        {
            if (aList.size() == 0) return;
            dm->pasteParts(aList.at(0));
//            updateObjBrowser(true);
        } break;
        case lgCopyGroups :
        {
            if (aList.size() == 0) return;
            bool emptyList = false;  //if the PasteGroups should be empty or not
            if (dm->getPasteGroupsSize() > 0) { //if has some Group in the list
                QMessageBox::StandardButton aBtn = SUIT_MessageBox::warning(application()->desktop(),
                                                 QString("Warning"),QString( "Overide previous copied/cut Groups? "),
                                                 QMessageBox::Yes, QMessageBox::No);
                if (aBtn == QMessageBox::Yes) emptyList = true;
            }
            dm->copyGroups(aList, emptyList);
        } break;
        case lgCutGroups :
        {
            if (aList.size() == 0) return;
            bool emptyList = false;  //if the PasteGroups should be empty or not
            if (dm->getPasteGroupsSize() > 0) { //if has some Group in the list
                QMessageBox::StandardButton aBtn = SUIT_MessageBox::warning(application()->desktop(),
                                                 QString("Warning"),QString( "Overide previous copied/cut Groups? "),
                                                 QMessageBox::Yes, QMessageBox::No);
                if (aBtn == QMessageBox::Yes) emptyList = true;
            }
            //first erase then cut
            erase(d);
            dm->cutGroups(aList,emptyList);
        } break;
        case lgPasteGroups :
        {
            if (aList.size() == 0) return;
            dm->pasteGroups(aList.at(0));
//            updateObjBrowser(true);
        } break;

        case lgRecoverShape :
        {
            if (aList.size() == 0) return;
            dm->recoverShapeInParts(aList);
//            updateObjBrowser(true);
            redisplay(displayer());
        } break;
        case lgDecompose :
        {
            if (aList.size() == 0) return;
            dm->decomposeShapes(aList);
//            updateObjBrowser(true);
            redisplay(displayer());
        } break;
        case lgSend2GEOM :
        {
            if (aList.size() == 0) return;
            dm->send2GEOM(aList);

        } break;
        case lgSendMesh2SMESH :
        {
            if (aList.size() == 0) return;
            dm->sendMesh2SMESH(aList);

        } break;
        case lgClearMesh :
        {
            if (aList.size() == 0) return;
            dm->clearMesh(aList);

        } break;

        case lgCheckBeforeConvert :
        {
            if (dm->checkBeforeConvert())
                SUIT_MessageBox::information(application()->desktop(), QString("Check Before Convert"), QString("Check OK!"));
            else
                SUIT_MessageBox::information(application()->desktop(), QString("Check Before Convert"), QString("Check failed!\nPlease find the screen message for more information!"));

        } break;

        case lgFormNewGroup :
        {
            if (aList.size() == 0) return;
            bool bOk;
            QString aName = QInputDialog::getText( application()->desktop(),tr( "Form new group" ),tr( "Group name"),
                                                   QLineEdit::Normal,"NewGroup", &bOk);
            if (aName.trimmed().isEmpty() || bOk == false)
            {
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "Name invalid or empty!"));
                return;
            }
            erase(displayer());// erase before form new group
            dm->formNewGroup(aList, aName);

        } break;
        case lgFormNewComponent :
        {
            if (aList.size() == 0) return;
            bool bOk;
            QString aName = QInputDialog::getText( application()->desktop(),tr( "Form new Component" ),tr( "Component name"),
                                                   QLineEdit::Normal,"NewComponent", &bOk);
            if (aName.trimmed().isEmpty() || bOk == false)
            {
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "Name invalid or empty!"));
                return;
            }
            erase(displayer());// erase before form new component
            dm->formNewComponent(aList, aName);

        } break;

        case lgExplodePart :
        {
            if (aList.size() == 0) return;
            erase(displayer()); //erase the selected because it will be deleted
            dm->explodePart(aList[0]);

        } break;
        case lgFuseParts :
        {
            if (aList.size() <= 1) return;
            bool bOk;
            QString aName = QInputDialog::getText( application()->desktop(),tr( "Fuse parts" ),tr( "New part name"),
                                                   QLineEdit::Normal,"FusedPart", &bOk);
            if (aName.trimmed().isEmpty() || bOk == false)
            {
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "Name invalid or empty!"));
                return;
            }
            erase(displayer());// erase before form new component
            dm->fuseParts(aList, aName);
        } break;
#ifdef WITH_TETGEN
        case lgGenerateTetMesh :
        {
            if (aList.size() == 0) return;
            if (!dm->generateTetMesh(aList))
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "Mesh generation for this part (or some of these parts) failed!"));

        } break;
        case lgGenerateAllMesh :
        {
            if (!dm->generateMesh())
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "Mesh generation for might failed for some parts"));

        } break;
#endif
        case lgMarkAsDecomposed :
        {
            if (aList.size() == 0) return;
                QMessageBox::StandardButton aBtn = SUIT_MessageBox::warning(application()->desktop(),
                                                 QString("Warning"),QString( "Mark these parts as decomposed? "),
                                                 QMessageBox::Yes, QMessageBox::No);
                if (aBtn == QMessageBox::Yes)
                    dm->markIfDecomposed(aList, true);
                else return;
        } break;
        case lgMarkAsUndecomposed:
        {
            if (aList.size() == 0) return;
                QMessageBox::StandardButton aBtn = SUIT_MessageBox::warning(application()->desktop(),
                                                 QString("Warning"),QString( "Mark these parts as undecomposed? "),
                                                 QMessageBox::Yes, QMessageBox::No);
                if (aBtn == QMessageBox::Yes)
                    dm->markIfDecomposed(aList, false);
                else return;
        } break;
        case lgMarkAllAsDecomposed:
        {
            if (aList.size() == 0) return;
                QMessageBox::StandardButton aBtn = SUIT_MessageBox::warning(application()->desktop(),
                                                 QString("Warning"),QString( "Mark all parts as decomposed? "),
                                                 QMessageBox::Yes, QMessageBox::No);
                if (aBtn == QMessageBox::Yes)
                    dm->markAllIfDecomposed(true);
                else return;
        } break;
        case lgMarkAllAsUndecomposed:
        {
            if (aList.size() == 0) return;
                QMessageBox::StandardButton aBtn = SUIT_MessageBox::warning(application()->desktop(),
                                                 QString("Warning"),QString( "Mark all parts as undecomposed? "),
                                                 QMessageBox::Yes, QMessageBox::No);
                if (aBtn == QMessageBox::Yes)
                    dm->markAllIfDecomposed(false);
                else return;
        } break;
        default: MESSAGE ("Action not found!"); break;

        }
        updateObjBrowser();
    }
}

void   MCCADGUI:: onImportExport()
{
    if( sender() && sender()->inherits( "QAction" ) )
    {
        MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
        if (!dm)
        {
            MESSAGE("get Data Model failed");
            return;
        }

        //get selected group
        QStringList aList ;
        selected(aList, true);
        int id = actionId( ( QAction* )sender() );

        //switch actions
        switch ( id )
        {
        case lgImportGeom:
        {
            //create component and get group id
            int aComponentID = -1;
            int aGroupID = -1;

            MCCAD_ORB::MCCAD_Gen_var engine = GetMCCADGen();
            const int studyID = application()->activeStudy()->id();
            if ( !studyID || CORBA::is_nil( engine ) ) return;

            if (aList.size() > 0) {
                aComponentID = dm->getComponentID(aList.at(0));
                aGroupID = dm->getGroupID(aList.at(0));
                if (aComponentID == -1)  {
                    //create a new component
                    MCCAD_ORB::Component_var aComponent = engine->generateComponent("NewComponent");
                    aComponentID = aComponent->getID();
                    MCCAD_ORB::ComponentList * aComponentList = engine->getData(studyID);
                    aComponentList->length(aComponentList->length() + 1); //increase one space
                    (*aComponentList)[aComponentList->length() -1] = aComponent._retn();
                }
            }
            else {  // nothing selected, create a new component
                MCCAD_ORB::Component_var aComponent = engine->generateComponent("NewComponent");
                aComponentID = aComponent->getID();
                MCCAD_ORB::ComponentList * aComponentList = engine->getData(studyID);
                aComponentList->length(aComponentList->length() + 1); //increase one space
                (*aComponentList)[aComponentList->length() -1] = aComponent._retn();
            }

            //get all the selected files
//            QString fileName = "/mnt/shared/tmp/CoolingPlate.stp";  //for testing
            QStringList fileNames = QFileDialog::getOpenFileNames(getApp()->desktop(),
                                                            tr("Open OCC File"),
                                                            LastDir,
 //                                                           tr("STEP (*.stp, *.step);;All(*.*)"));
                                                            tr("STEP (*.stp *.step *.STEP);;BREP (*.brep);;IGES (*.iges *.igs);;All(*.*)"));


            if (fileNames.size() == 0) return;
            //ask if explode
//            bool isExplode = false;
//            QMessageBox::StandardButton aBtn = SUIT_MessageBox::information(application()->desktop(),QString("Message"),
//                                                                            QString( "If model contains compound(s), explode them?"),QMessageBox::Yes, QMessageBox::No);

//            if (aBtn == QMessageBox::Yes) isExplode = true;
            if (fileNames.size() > 1  ) {
                QMessageBox::StandardButton aBtn = SUIT_MessageBox::information(application()->desktop(),QString("Message"),
                                                                                QString( "Put these files into one group?"),QMessageBox::Yes, QMessageBox::No);
                if (aBtn == QMessageBox::Yes) {
                    MCCAD_ORB::Group_var aGroup = engine->generateGroup("NewGroup");
                    aGroupID = aGroup->getID();
                    MCCAD_ORB::Component_var aComponent = engine->getComponent(studyID, aComponentID);
                    aComponent->appendGroup(aGroup);
                }

            }
            bool isExplode = false;  //for explode
            for (int i=0; i<fileNames.size(); i++)
            {
                if (fileNames[i].trimmed().isEmpty() || SUIT_Tools::file(fileNames[i]).trimmed().isEmpty())
                {
                    SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                              QString( "File directory invalid or name empty!"));
                    return;
                }

                if (!dm->importGeom(fileNames[i], isExplode, aComponentID, aGroupID)) {
                    MESSAGE("Import failed!");
                    return;
                }
            }
            LastDir = SUIT_Tools::dir(fileNames[0]);  //save the dir


        } break;
        case lgExportMesh2Abaqus:
        {
            QString fileName = QFileDialog::getSaveFileName(application()->desktop(),
                                                            tr("Save to Abaqus inp file"),
                                                            LastDir,
                                                            tr("Abaqus (*.inp);;All(*.*)"));
            if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
                return;

            if (SUIT_Tools::extension(fileName).toLower() != "inp")
            {
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "The extension will be changed to *.inp"));
                fileName = SUIT_Tools::addSlash(SUIT_Tools::dir(fileName)) + SUIT_Tools::file(fileName, false); //get a filename without extension
                fileName += ".inp";
            }
            if (dm->exportAllMesh2Abaqus(fileName))
                SUIT_MessageBox::information(application()->desktop(), QString("Export Abaqus Mesh"), QString("Export done!"));
            else
                SUIT_MessageBox::information(application()->desktop(), QString("Export Abaqus Mesh"), QString("Export failed!"));

        } break;
        case lgConvert2MCInput:
        {
            QString fileName = QFileDialog::getSaveFileName(application()->desktop(),
                                                            tr("Save to file"),
                                                            LastDir,
                                                            tr("All(*.*)"));
            if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
                return;

            if (dm->convert2MC(fileName))
                SUIT_MessageBox::information(application()->desktop(), QString("Convert to MC input"), QString("Conversion done!"));
            else
                SUIT_MessageBox::information(application()->desktop(), QString("Convert to MC input"), QString("Conversion failed!"));

        } break;
        case lgExportGeom:
        {
            QString fileName = QFileDialog::getSaveFileName(application()->desktop(),
                                                            tr("Save geometry"),
                                                            LastDir,
                                                            tr("STEP (*.stp *.step);;BREP (*.brep);;IGES (*.iges *.igs);;All(*.*)"));
            if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
                return;

            if (SUIT_Tools::extension(fileName).toLower() != "stp" &&
                    SUIT_Tools::extension(fileName).toLower() != "step" &&
                    SUIT_Tools::extension(fileName).toLower() != "brep" &&
                    SUIT_Tools::extension(fileName).toLower() != "iges" &&
                    SUIT_Tools::extension(fileName).toLower() != "igs" )
            {
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "The extension will be changed to *.stp"));
                fileName = SUIT_Tools::addSlash(SUIT_Tools::dir(fileName)) + SUIT_Tools::file(fileName, false); //get a filename without extension
                fileName += ".stp";
            }
            if (dm->exportGeom(aList, fileName))
                SUIT_MessageBox::information(application()->desktop(), QString("Export geometry"), QString("Export done!"));
            else
                SUIT_MessageBox::information(application()->desktop(), QString("Export geometry"), QString("Export failed!"));

        } break;

        case lgImportAbaqus:
        {
            QStringList fileNames = QFileDialog::getOpenFileNames(getApp()->desktop(),
                                                            tr("Open Abaqus Files"),
                                                            LastDir,
                                                            tr("Abaqus input (*.inp );;All(*.*)"));


            if (fileNames.size() == 0) return;

            bool isOk = true;
            for (int i=0; i<fileNames.size(); i++)
            {
                if (fileNames[i].trimmed().isEmpty() || SUIT_Tools::file(fileNames[i]).trimmed().isEmpty())
                {
                    SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                              QString( "File directory invalid or name empty!"));
                    return;
                }
                if (!dm->importAbaqus(fileNames[i])) {
                    MESSAGE("Import file failed!: " << fileNames[i].toStdString());
                    isOk = false;
                    return;
                }
            }

            if (isOk)
                SUIT_MessageBox::information(application()->desktop(), QString("Import Abaqus file"), QString("Import done! Find the meshes in SMESH module"));
            else
                SUIT_MessageBox::information(application()->desktop(), QString("Import Abaqus file"), QString("Import failed for some files!"));

        } break;

        default: MESSAGE ("Action not found!"); break;
        }
        updateObjBrowser();
    }

}


//find http://www.salome-platform.org/forum/forum_12/842743364 for detail introduction
//this function is changed because it cause trouble loading ParaVIS libraries preceed ParaVIS loading them by itself.
/*void    MCCADGUI::onDisplayPVIS()
{
    //get A selected objects
    QStringList aList ;
    selected(aList, false);
    if (aList.size() == 0)
    {
        MESSAGE("No object selected");
        return;
    }
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    if (!dm)
    {
        MESSAGE("get Data Model failed");
        return;
    }

    MCCADGUI_DataObject * aObj = dm->findObject(aList.at(0));
    if (aObj && aObj->isPart())
    {
        Engines::EngineComponent_var comp = getApp()->lcc()->FindOrLoad_Component("FactoryServer", "PARAVIS" );
        PARAVIS::PARAVIS_Gen_var  aPVGen = PARAVIS::PARAVIS_Gen::_narrow(comp);
        if (!aPVGen->_is_nil())
        {
            MCCAD_ORB::Part_var aPart = aObj->getPart();
            std::ostringstream aScript;
            char *IOR;
            if (strcmp(aPart->getType(), "EmptyPart") == 0)
            {
                SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_var aFieldTemplate =
                        aPart->getFieldTemplate();
                if (!aFieldTemplate->_is_nil())
                    IOR = getApp()->orb()->object_to_string(aFieldTemplate);
            }
            else
            {
                SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_var aField =
                        aPart->getField();
                if (!aField->_is_nil())
                    IOR =getApp()->orb()->object_to_string(aField);
            }
            if (*IOR == 0)
            {
                MESSAGE("Emtpy IOR!")
                return;
            }
            aScript << "src1 = ParaMEDCorbaPluginSource()\nsrc1.IORCorba = '" << IOR
                    << "'\nasc=GetAnimationScene()\nrw=GetRenderView()\ndr=Show()\ndr.Visibility = 1\nRender()";
            CORBA::string_free(IOR);
            aPVGen->ExecuteScript(aScript.str().c_str());
            return;
        }
        MESSAGE("Please open ParaVIS before this operation!");
        return;
    }
    MESSAGE("Cannot find object, or object is not a part!");
}
*/

//copy from  SMESH_NumberFilter::getGeom
GEOM::GEOM_Object_ptr MCCADGUI::getGeom  (const SUIT_DataOwner* theDataOwner, const bool extractReference )
{
  const LightApp_DataOwner* owner =
    dynamic_cast<const LightApp_DataOwner*>(theDataOwner);
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
    (SUIT_Session::session()->activeApplication()->activeStudy());

  GEOM::GEOM_Object_var anObj;

  if (!owner || !appStudy)
    return GEOM::GEOM_Object::_nil();

  _PTR(Study) study = appStudy->studyDS();
  QString entry = owner->entry();

  _PTR(SObject) aSO( study->FindObjectID( entry.toLatin1().data() ) ), aRefSO;
  if( extractReference && aSO && aSO->ReferencedObject( aRefSO ) )
    aSO = aRefSO;

  if (!aSO)
    return GEOM::GEOM_Object::_nil();

  CORBA::Object_var anObject = _CAST(SObject,aSO)->GetObject();
  anObj = GEOM::GEOM_Object::_narrow(anObject);
  if (!CORBA::is_nil(anObj))
    return anObj._retn();

  return GEOM::GEOM_Object::_nil();
}


//copy from  SMESH_NumberFilter::getGeom
SMESH::SMESH_Mesh_var MCCADGUI::getSmesh  (const SUIT_DataOwner* theDataOwner, const bool extractReference )
{
  const LightApp_DataOwner* owner =
    dynamic_cast<const LightApp_DataOwner*>(theDataOwner);
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
    (SUIT_Session::session()->activeApplication()->activeStudy());

  SMESH::SMESH_Mesh_var anObj;

  if (!owner || !appStudy)
    return SMESH::SMESH_Mesh::_nil();

  _PTR(Study) study = appStudy->studyDS();
  QString entry = owner->entry();

  _PTR(SObject) aSO( study->FindObjectID( entry.toLatin1().data() ) ), aRefSO;
  if( extractReference && aSO && aSO->ReferencedObject( aRefSO ) )
    aSO = aRefSO;

  if (!aSO)
    return SMESH::SMESH_Mesh::_nil();

  CORBA::Object_var anObject = _CAST(SObject,aSO)->GetObject();
  anObj = SMESH::SMESH_Mesh::_narrow(anObject);
  if (!CORBA::is_nil(anObj))
    return anObj._retn();

  return SMESH::SMESH_Mesh::_nil();
}

//display GEOM object in PVIS
//A OCC2VTK Plugin is programed for this application
void    MCCADGUI::onDisplayGEOMObjInPVIS()
{
    //get a selected objects
    LightApp_Application* app = getApp();
    LightApp_SelectionMgr* SltMgr = app ? app->selectionMgr() : 0;
    if( !SltMgr )
        return;
    SUIT_DataOwnerPtrList anOwnersList;
    SltMgr->selected( anOwnersList );
    if (anOwnersList.size() == 0 || anOwnersList.size() > 1)
    {
        MESSAGE("No or more than one object selected!");
        return;
    }

    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( anOwnersList[ 0 ].get() );
    // Get geom object from IO
    GEOM::GEOM_Object_var aGeomObj = getGeom(owner, true);
    if (aGeomObj->_is_nil())
    {
        MESSAGE("the GEOM object is nil!");
        return;
    }

    //Get paraVIS_Gen
    int argc=0;
    CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
    CORBA::Object_var obj=orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa=PortableServer::POA::_narrow(obj);
    PortableServer::POAManager_var mgr=poa->the_POAManager();
    mgr->activate();
    SALOME_NamingService ns(orb);
    ns.Change_Directory("/Containers");
    //search for the ParaVIS Gen
    vector <string> aPathList = ns.list_directory_recurs();
    std::ostringstream path;
    for (int i=0; i<aPathList.size(); i++){
        string atmpStr = aPathList [i];
        if (atmpStr.find("PARAVIS") >= 0 && atmpStr.find("PARAVIS") <atmpStr.size()){
            path << aPathList[i];
            break;
        }
    }
    MESSAGE("path for PARAVIS: "<< path.str().c_str());
    CORBA::Object_var paravis=ns.Resolve(path.str().c_str());
    if (CORBA::is_nil(paravis))        {
        MESSAGE("Please open ParaVIS before this operation!");
        return;
    }
    CORBA::Request_var req=paravis->_request("ExecuteScript");
    CORBA::NVList_ptr args=req->arguments();
    CORBA::Any ob;
    std::ostringstream script;

    char *IOR;
    IOR = orb->object_to_string(aGeomObj);
    if (*IOR == 0){
        MESSAGE("Emtpy IOR!")
        return;
    }
//    cout <<IOR<<endl;
    script << "src1 = OCC2VTKCorbaPluginSource()\nsrc1.IORCorba = '" << IOR << "'\nasc=GetAnimationScene()\nrw=GetRenderView()\ndr=Show()\ndr.Visibility = 1\n";
//    script << "src1 = ParaMEDCorbaPluginSource()\nsrc1.IORCorba = '" << IOR << "'\nasc=GetAnimationScene()\nrw=GetRenderView()\ndr=Show()\ndr.Visibility = 1\n";

    CORBA::string_free(IOR);
    ob <<= script.str().c_str();
    args->add_value("script",ob,CORBA::ARG_IN);
    req->set_return_type(CORBA::_tc_void);
    req->invoke();
    cout <<"Invoked."<<endl;

    return;
}



void MCCADGUI::OnDisplayerCommand()
{
  const QObject* obj = sender();
  if ( obj && obj->inherits( "QAction" ) ) {

      MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
      if (!dm) {
          MESSAGE("Get data model failed!");
          return ;
      }

      const int id = actionId ( (QAction*)obj );
      MCCADGUI_Displayer * d = dynamic_cast <MCCADGUI_Displayer * > (displayer());
      QStringList entries;
      selected ( entries, true );

      switch ( id ) {
      case lgDisplayOnly: {
          d->EraseAll(/*force*/true);
          display(d);
          d->UpdateViewer();
      } break;
      case lgDisplay : {
          display(d);
      } break;
      case lgErase   : {
         erase(d);
      } break;
      case lgShading   : {
          setProperty(entries, MCCAD::DisplayMode, 1);
          d->SetDisplayMode(1 /*shading mode*/);
          redisplay(d);
      } break;
      case lgWireframe   : {
          setProperty(entries, MCCAD::DisplayMode, 0);
          d->SetDisplayMode(0 /*wireframe mode*/);
          redisplay(d);
      } break;
      case lgShadingWithWireFrame   : {
          setProperty(entries, MCCAD::DisplayMode, 2);
          d->SetDisplayMode(2 /*shading with wireframe*/);
          redisplay(d);
      } break;
      case lgTransparency   : {
          MCCADGUI_TransparencyDlg( getApp()->desktop(), entries ).exec();
      } break;
      default: printf( "ERROR: Action with ID = %d was not found in MCCADGUI\n", id ); break;
      }
  }
}

void MCCADGUI::OnExperiment()
{
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    dm->buildTree();
    updateObjBrowser();
}

void MCCADGUI::onEditMaterial()
{
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    if (!dm) return;

    MCCADGUI_MCardDialog aMatDialog (application()->desktop(),dm );
    aMatDialog.Refresh();
    aMatDialog.exec();
}

void    MCCADGUI::onSetProproties()
{
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    if (!dm) return;

    MCCADGUI_SetPropertyDialog * aPropertyDialog = new MCCADGUI_SetPropertyDialog(myInputPanel, dm);
    myInputPanel->setWidget(aPropertyDialog);
    aPropertyDialog->exec();
    delete aPropertyDialog;
}

void    MCCADGUI::onTransform()
{
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    if (!dm) return;

    MCCADGUI_TransformDialog * aPropertyDialog = new MCCADGUI_TransformDialog(application()->desktop(), dm);
    aPropertyDialog->exec();
    delete aPropertyDialog;
}

void    MCCADGUI::onImportGEOMObj()
{
    //get a selected objects
    LightApp_Application* app = getApp();
    LightApp_SelectionMgr* SltMgr = app ? app->selectionMgr() : 0;
    if( !SltMgr )
        return;
    SUIT_DataOwnerPtrList anOwnersList;
    SltMgr->selected( anOwnersList );
    if (anOwnersList.size() == 0 || anOwnersList.size() > 1)    {
        MESSAGE("No or more than one object selected!");
        return;
    }
    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( anOwnersList[ 0 ].get() );
    // Get geom object from IO
    GEOM::GEOM_Object_var aGeomObj = getGeom(owner, true);
    if (aGeomObj->_is_nil())    {
        MESSAGE("the GEOM object is nil!");
        return;
    }

    // Get shape from geom object and verify its parameters
    GEOM_Client aGeomClient;
    if ( CORBA::is_nil( GeometryGUI::GetGeomGen() ) && !GeometryGUI::InitGeomGen() )    {
        MESSAGE("Could not get the GEOM Engine!");
        return ;
    }
    TopoDS_Shape aShape = aGeomClient.GetShape(GeometryGUI::GetGeomGen(), aGeomObj);
    if (aShape.IsNull())    {
        MESSAGE("The Shape is Nil!");
        return ;
    }

    //call the dialog
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    if (!dm) return;
    MCCADGUI_ImportObjDialog aImportDialog (myInputPanel, dm);  //!!! temporary. the parent of this dialog should be application->destop()
    myInputPanel->setWidget(&aImportDialog);
    aImportDialog.SetSourceObjName(QStringList (aGeomObj->GetName()));
    if (aImportDialog.exec() == 1) //if canceled
        return;

    QStringList aList ;
    selected(aList, /*multiple*/false);
    QString aEntry;
    if (aList.isEmpty() )
        aEntry = ""; //push a empty object, to invoke dm to create a new component and group
    else
        aEntry = aList[0];
    if (dm->assignShape(aEntry, aShape, aGeomObj->GetName()))
        SUIT_MessageBox::information(application()->desktop(), QString("Import GEOM Object"), QString("Import done!"));
    else
        SUIT_MessageBox::information(application()->desktop(), QString("Import GEOM Object"), QString("Import failed!"));
    redisplay(displayer());
    updateObjBrowser();
}


/*!
 * \brief MCCADGUI::onImportSMESHObj
 *  import SMESH Object for parts
 */
void    MCCADGUI::onImportSMESHObj()
{
    //get a selected objects
    LightApp_Application* app = getApp();
    LightApp_SelectionMgr* SltMgr = app ? app->selectionMgr() : 0;
    if( !SltMgr )
        return;
    SALOME_ListIO aListIO;
//    SUIT_DataOwnerPtrList anOwnersList;
//    SltMgr->selected( anOwnersList );
    SltMgr->selectedObjects(aListIO);
    if (aListIO.IsEmpty())    {
        MESSAGE("No object selected!");
        return;
    }
//    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( anOwnersList[ 0 ].get() );

//    Handle(SALOME_InteractiveObject) anIO = aListIO.First();

    //call the dialog
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    if (!dm) return;
    MCCADGUI_ImportObjDialog aImportDialog (myInputPanel, dm);  //!!! temporary. the parent of this dialog should be application->destop()
    myInputPanel->setWidget(&aImportDialog);

    QStringList aNameList, aEntryList;
    for (  SALOME_ListIteratorOfListIO it( aListIO ) ; it.More(); it.Next() ) {
      Handle(SALOME_InteractiveObject) io = it.Value();
      aNameList.append(io->getName());
      aEntryList.append(io->getEntry());
    }
//    aImportDialog.SetSourceObjName(anIO->getName());
    aImportDialog.SetSourceObjName(aNameList);

    if (aImportDialog.exec() == 1) //if canceled
        return;

    QStringList aList ;
//   selected(aList, /*multiple*/false);
    selected(aList, /*multiple*/true);

    if (dm->assignMesh(aList/*[0]*/, /*anIO->getEntry()*/ aEntryList, /*anIO->getName()*/ aNameList))
        SUIT_MessageBox::information(application()->desktop(), QString("Import SMESH Object"), QString("Import done!"));
    else
        SUIT_MessageBox::information(application()->desktop(), QString("Import SMESH Object"), QString("Import failed!"));
    updateObjBrowser();
}


/*!
 * \brief display selected items
 */
void    MCCADGUI::display(  LightApp_Displayer* aDisplayer)
{
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    if (!dm) {
        MESSAGE("Get data model failed!");
        return ;
    }
    QStringList entries;
    selected ( entries, true );
    MCCADGUI_Displayer * d = dynamic_cast <MCCADGUI_Displayer * > (aDisplayer);

    for ( QStringList::const_iterator it = entries.begin(), last = entries.end(); it != last; it++ )
    {
        MCCADGUI_DataObject * aObj = dm->findObject(*it);
        if (!aObj)              {
            SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),QString( "Cannot find the object in this study!"));
            return ;
        }
        //to see what it is the object
        if (aObj->isPart())
            d->Display( aObj->entry().toLatin1(), /*updateviewer=*/false, 0 );
        else if (aObj->isGroup() ) {
            MCCADGUI_DataObject * bObj = dynamic_cast <MCCADGUI_DataObject *> (aObj->firstChild());
            for (int i=0; i<aObj->childCount(); i++) {
                if (!d->isVisible(bObj->entry()))
                    d->Display( bObj->entry().toLatin1(), /*updateviewer=*/false, 0 );
                bObj = dynamic_cast <MCCADGUI_DataObject *> (bObj->nextBrother());
            }
        }
        else if (aObj->isComponent()) {
            MCCADGUI_DataObject * bObj = dynamic_cast <MCCADGUI_DataObject *> (aObj->firstChild());
            for (int i=0; i<aObj->childCount(); i++) {
                 MCCADGUI_DataObject * cObj =  dynamic_cast <MCCADGUI_DataObject *> (bObj->firstChild());
                 for (int j=0; j<bObj->childCount();  j++) {
                     if (!d->isVisible(cObj->entry()))
                         d->Display( cObj->entry().toLatin1(), /*updateviewer=*/false, 0 );
                     cObj = dynamic_cast <MCCADGUI_DataObject *> (cObj->nextBrother());
                 }
                bObj = dynamic_cast <MCCADGUI_DataObject *> (bObj->nextBrother());
            }
        }
        else
            MESSAGE("Cannot display this object!");
    }
    d->UpdateViewer();
}

/*!
 * \brief erase selected items
 */
void   MCCADGUI::erase(LightApp_Displayer* aDisplayer)
{
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    if (!dm) {
        MESSAGE("Get data model failed!");
        return ;
    }
    QStringList entries;
    selected ( entries, true );
    MCCADGUI_Displayer * d = dynamic_cast <MCCADGUI_Displayer * > (aDisplayer);
    for ( QStringList::const_iterator it = entries.begin(), last = entries.end(); it != last; it++ ) {
        MCCADGUI_DataObject * aObj = dm->findObject(*it);
        if (!aObj)              {
            SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),QString( "Cannot find the object in this study!"));
            return ;
        }
        //to see what it is the object
        if (aObj->isPart())
            d->Erase( aObj->entry().toLatin1(),/*forced=*/true,  /*updateviewer=*/true, 0 );
        else if (aObj->isGroup() ) {
            MCCADGUI_DataObject * bObj = dynamic_cast <MCCADGUI_DataObject *> (aObj->firstChild());
            for (int i=0; i<aObj->childCount(); i++) {
                if (d->isVisible(bObj->entry()))
                    d->Erase( bObj->entry().toLatin1(),/*forced=*/true, /*updateviewer=*/false, 0 );
                bObj = dynamic_cast <MCCADGUI_DataObject *> (bObj->nextBrother());
            }
        }
        else if (aObj->isComponent()) {
            MCCADGUI_DataObject * bObj = dynamic_cast <MCCADGUI_DataObject *> (aObj->firstChild());
            for (int i=0; i<aObj->childCount(); i++) {
                 MCCADGUI_DataObject * cObj =  dynamic_cast <MCCADGUI_DataObject *> (bObj->firstChild());
                 for (int j=0; j<bObj->childCount();  j++) {
                     if (d->isVisible(cObj->entry()))
                         d->Erase( cObj->entry().toLatin1(),/*forced=*/true, /*updateviewer=*/false, 0 );
                     cObj = dynamic_cast <MCCADGUI_DataObject *> (cObj->nextBrother());
                 }
                bObj = dynamic_cast <MCCADGUI_DataObject *> (bObj->nextBrother());
            }
        }
        else
            MESSAGE("Cannot display this object!");
    }
    d->UpdateViewer();
}

/*!
 * \brief Redisplay selected items
 */
void    MCCADGUI::redisplay(LightApp_Displayer* aDisplayer)
{
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    if (!dm) {
        MESSAGE("Get data model failed!");
        return ;
    }
    QStringList entries;
    selected ( entries, true );
    MCCADGUI_Displayer * d = dynamic_cast <MCCADGUI_Displayer * > (aDisplayer);
    for ( QStringList::const_iterator it = entries.begin(), last = entries.end(); it != last; it++ )
    {
        MCCADGUI_DataObject * aObj = dm->findObject(*it);
        if (!aObj)
        {
            SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),QString( "Cannot find the object in this study!"));
            return ;
        }
        //to see what it is the object
        if (aObj->isPart())  {
            if (d->isVisible(aObj->entry()))
                d->Redisplay( aObj->entry().toLatin1(), /*updateviewer=*/false );
        //2013-12-1 no need to display
//            else
//                d->Display( aObj->entry().toLatin1(), /*updateviewer=*/false, 0 );
        }
        else if (aObj->isGroup() ) {
            MCCADGUI_DataObject * bObj = dynamic_cast <MCCADGUI_DataObject *> (aObj->firstChild());
            for (int i=0; i<aObj->childCount(); i++) {
                if (d->isVisible(bObj->entry()))
                    d->Redisplay( bObj->entry().toLatin1(), /*updateviewer=*/false );
                //2013-12-1 no need to display
//                else
//                    d->Display( bObj->entry().toLatin1(), /*updateviewer=*/false, 0 );
                bObj = dynamic_cast <MCCADGUI_DataObject *> (bObj->nextBrother());
            }
        }
        else if (aObj->isComponent()) {
            MCCADGUI_DataObject * bObj = dynamic_cast <MCCADGUI_DataObject *> (aObj->firstChild());
            for (int i=0; i<aObj->childCount(); i++) {
                 MCCADGUI_DataObject * cObj =  dynamic_cast <MCCADGUI_DataObject *> (bObj->firstChild());
                 for (int j=0; j<bObj->childCount();  j++) {
                     if (d->isVisible(cObj->entry()))
                         d->Redisplay( cObj->entry().toLatin1(), /*updateviewer=*/false );
                     //2013-12-1 no need to display
//                     else
//                         d->Display( cObj->entry().toLatin1(), /*updateviewer=*/false, 0 );

                     cObj = dynamic_cast <MCCADGUI_DataObject *> (cObj->nextBrother());
                 }
                bObj = dynamic_cast <MCCADGUI_DataObject *> (bObj->nextBrother());
            }
        }
        else
            MESSAGE("Cannot Redisplay this object!");
    }
    d->UpdateViewer();
}

void     MCCADGUI::setProperty (const QStringList & entries, MCCAD::Property aProp, QVariant aValue )
{
    MCCADGUI_DataModel * dm = dynamic_cast <MCCADGUI_DataModel *> (dataModel());
    if (!dm) {
        MESSAGE("Get data model failed!");
        return ;
    }

    SALOME_View * aView = MCCADGUI_Displayer::GetActiveView();
    SalomeApp_Study* study = dynamic_cast<SalomeApp_Study * > (application()->activeStudy());
    if ( study && aView) {
        SUIT_ViewModel* viewModel = dynamic_cast<SUIT_ViewModel*>( aView );
        SUIT_ViewManager* viewMgr = ( viewModel != 0 ) ? viewModel->getViewManager() : 0;
        int viewId = ( viewMgr != 0 ) ? viewMgr->getGlobalId() : -1;

        if ( viewModel && viewId != -1 ) {
            // get properties from the study

            for ( QStringList::const_iterator it = entries.begin(), last = entries.end(); it != last; it++ )
            {
                MCCADGUI_DataObject * aObj = dm->findObject(*it);
                if (!aObj)              {
                    SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),QString( "Cannot find the object in this study!"));
                    return ;
                }
                //to see what it is the object
                if (aObj->isPart()) {
                    PropMap aPropMap = study->getObjectPropMap( viewId, aObj->entry() );
//                    if ( aPropMap.contains( MCCAD::propertyName( aProp ) ) )
                        study->setObjectProperty(viewId,aObj->entry() , propertyName( aProp ), aValue);
                }
                else if (aObj->isGroup() ) {
                    MCCADGUI_DataObject * bObj = dynamic_cast <MCCADGUI_DataObject *> (aObj->firstChild());
                    for (int i=0; i<aObj->childCount(); i++) {
                        PropMap aPropMap = study->getObjectPropMap( viewId, bObj->entry() );
//                        if ( aPropMap.contains( MCCAD::propertyName( aProp ) ) )
                            study->setObjectProperty(viewId,bObj->entry() , propertyName( aProp ), aValue);
                        bObj = dynamic_cast <MCCADGUI_DataObject *> (bObj->nextBrother());
                    }
                }
                else if (aObj->isComponent()) {
                    MCCADGUI_DataObject * bObj = dynamic_cast <MCCADGUI_DataObject *> (aObj->firstChild());
                    for (int i=0; i<aObj->childCount(); i++) {
                        MCCADGUI_DataObject * cObj =  dynamic_cast <MCCADGUI_DataObject *> (bObj->firstChild());
                        for (int j=0; j<bObj->childCount();  j++) {

                            PropMap aPropMap = study->getObjectPropMap( viewId, cObj->entry() );
//                            if ( aPropMap.contains( MCCAD::propertyName( aProp ) ) )
                                study->setObjectProperty(viewId,cObj->entry() , propertyName( aProp ), aValue);
                            cObj = dynamic_cast <MCCADGUI_DataObject *> (cObj->nextBrother());
                        }
                        bObj = dynamic_cast <MCCADGUI_DataObject *> (bObj->nextBrother());
                    }
                }
                else
                    MESSAGE("Cannot set this object property!");
            }
        }
    }
}



// Export the module
extern "C" {
// this function should be EXPORT in order to be call in Windows
Standard_EXPORT CAM_Module* createModule()
{
    return new MCCADGUI();
}
}
