# Copyright (C) 2014-2015  KIT-INR/NK
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#

#

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${KERNEL_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/idl
#  ${PROJECT_SOURCE_DIR}/src/MCCAD/McCadHeaders  #sorry, just temporary...
  ${PROJECT_BINARY_DIR}/src/MCCADGUI  #sorry, just temporary...
# ${IDL_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${MED_INCLUDE_DIRS}
#  ${CPPUNIT_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS} #
  ${SMESH_INCLUDE_DIRS}
  ${GEOM_INCLUDE_DIRS}
  ${McCad_INCLUDE_DIRS}

  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
    -DWITH_TETGEN
  )
#to advoid conflict with Windows max() and min()
IF(WIN32)
  ADD_DEFINITIONS(-DNOMINMAX -DNOGDI)
ENDIF(WIN32)

MESSAGE ("VTK_LIBS: ${VTK_LIBRARIES}")
# libraries to link to
SET(_link_LIBRARIES
   SalomeIDLMCCAD
  ${KERNEL_OpUtil}
  ${KERNEL_SalomeContainer}
  ${KERNEL_SalomeIDLKernel}
  ${MED_medcouplingclient}
  ${MED_medcouplingcorba}
  ${MED_medloader}
  ${MED_SalomeIDLMED}
  ${HDF5_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  ${CAS_KERNEL}
  ${CAS_MODELER}
  ${CAS_TKV3d}
  ${CAS_TKBool}
  ${CAS_TKBRep}
  ${CAS_TKIGES}
  ${CAS_TKSTEP}
  ${CAS_TKSTL}
#  TKMcCadMcVoid
#  TKMcCadConvertTools
  TKMcCad
  ${TETGEN_tet}
  ${NETGEN_LIBRARIES}
  ${GUI_LightApp}
  ${GUI_SalomeApp}
  ${VTK_LIBRARIES}
  ${SMESH_SMESHimpl}
  ${SMESH_SMESHEngine}
  ${SMESH_StdMeshers}
  ${SMESH_StdMeshersEngine}
  ${SMESH_SMDS}
  ${SMESH_SMESHDS}
  ${SMESH_SMESH}
  ${GEOM_GEOMBase}
  ${GEOM_GEOMFiltersSelection}
  ${GEOM_GEOMObject}
  ${GEOM_GEOMClient}
  ${GEOM_GEOM}
  )


# uic files / to be processed by pyuic
SET(_pyuic_files
        MCardDialog.ui
        SetPropertyDialog.ui
        ImportObjDialog.ui
        TransformDialog.ui
)

# scripts / pyuic wrappings
QT4_WRAP_UI(_uic_HEADERS ${_pyuic_files})

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS
        MCCADGUI.h
        MCCADGUI_Operation.h
        MCCADGUI_OperationImpl.h
        MCCADGUI_Dialogs.h
)
# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})


# --- headers ---

SET(MCCAD_HEADERS
        MCCADGUI.h
        MCCADGUI_DataModel.h
        MCCADGUI_DataObject.h
        MCCADGUI_Operation.h
        MCCADGUI_Selection.h
        MCCADGUI_OperationImpl.h
        MCCADGUI_Dialogs.h
        MCCADGUI_Displayer.h
        S_McCadSolid.hxx
        S_McCadVoidCellManager.hxx
        S_McCadMcnpWriter.hxx
        S_McCadGeomData.hxx
        S_McCadTripoliWriter.hxx
        S_McCadGDMLWriter.hxx
        MCMESHTRAN_AbaqusMeshReader.hxx
  )



# sources / to compile

# --- sources ---

SET(MCCAD_SOURCES
        MCCADGUI.cxx
        MCCADGUI_DataModel.cxx
        MCCADGUI_DataObject.cxx
        MCCADGUI_Operation.cxx
        MCCADGUI_Selection.cxx
        MCCADGUI_OperationImpl.cxx
        MCCADGUI_Dialogs.cxx
        MCCADGUI_Displayer.cxx
        MCCAD_AISShape.cxx
        MCCAD_Constants.cxx
        S_McCadSolid.cxx
        S_McCadVoidCellManager.cxx
        S_McCadMcnpWriter.cxx
        S_McCadGeomData.cxx
        S_McCadTripoliWriter.cxx
        S_McCadGDMLWriter.cxx
        MCMESHTRAN_AbaqusMeshReader.cxx
  ${_uic_HEADERS}
  ${_moc_SOURCES}
  ${_rcc_SOURCES}
  )
# --- rules ---

ADD_LIBRARY(MCCAD ${MCCAD_SOURCES})
TARGET_LINK_LIBRARIES(MCCAD ${_link_LIBRARIES})
INSTALL(TARGETS MCCAD EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${MCCAD_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})


