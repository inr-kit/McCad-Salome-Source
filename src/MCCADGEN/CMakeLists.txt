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
  ${QT_INCLUDES}

  ${MED_INCLUDE_DIRS}
  ${MEDFILE_INCLUDE_DIRS}
  ${CPPUNIT_INCLUDE_DIRS}
#  ${McCad_INCLUDE_DIRS}
#  ${PROJECT_SOURCE_DIR}/src/MCCAD/McCadHeaders  #sorry, just temporary...
  ${NETGEN_INCLUDE_DIRS}
  ${TETGEN_INCLUDE_DIRS}
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${CAS_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  -DTETLIBRARY
  -DWITH_TETGEN
  )
#to advoid conflict with Windows max() and min()
IF(WIN32)
  ADD_DEFINITIONS(-DNOMINMAX )
ENDIF(WIN32)

# libraries to link to
SET(_link_LIBRARIES
   SalomeIDLMCCAD
  ${KERNEL_OpUtil}
  ${KERNEL_SalomeContainer}
  ${MED_medcouplingclient}
  ${MED_medcouplingcorba}
  ${MED_medloader}
  ${MED_SalomeIDLMED}
#  ${MED_medcouplingremapper}
  ${MEDFILE_C_LIBRARIES}
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
#  ${TETGEN_tet}
  Tetgen
  ${NETGEN_LIBRARIES}
  )

# --- headers ---

SET(MCCADEngine_HEADERS
        MCCAD.hxx
        MCMESHTRAN_AbaqusMeshWriter.hxx
  )

# --- sources ---

SET(MCCADEngine_SOURCES
        MCCAD.cxx
        MCMESHTRAN_AbaqusMeshWriter.cxx
  )

# --- rules ---

ADD_LIBRARY(MCCADEngine ${MCCADEngine_SOURCES})
TARGET_LINK_LIBRARIES(MCCADEngine ${_link_LIBRARIES})
INSTALL(TARGETS MCCADEngine EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${MCCADEngine_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})


