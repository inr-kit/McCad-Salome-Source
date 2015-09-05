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
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)
INCLUDE(CMakeDependentOption)

#qiu
PROJECT(SalomeMCCAD C CXX) #qiu

SET(CMAKE_BUILD_TYPE Release)
# Ensure a proper linker behavior:
CMAKE_POLICY(SET CMP0003 NEW)

# Versioning
# ===========
# Project name, upper case
STRING(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC)

SET(${PROJECT_NAME_UC}_MAJOR_VERSION 0)#qiu
SET(${PROJECT_NAME_UC}_MINOR_VERSION 5)#qiu
SET(${PROJECT_NAME_UC}_PATCH_VERSION 0)
SET(${PROJECT_NAME_UC}_VERSION
  ${${PROJECT_NAME_UC}_MAJOR_VERSION}.${${PROJECT_NAME_UC}_MINOR_VERSION}.${${PROJECT_NAME_UC}_PATCH_VERSION})
SET(${PROJECT_NAME_UC}_VERSION_DEV 0)

# Find KERNEL
# ===========
SET(KERNEL_ROOT_DIR $ENV{KERNEL_ROOT_DIR} CACHE PATH "Path to the Salome KERNEL")
IF(EXISTS ${KERNEL_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${KERNEL_ROOT_DIR}/salome_adm/cmake_files")
  INCLUDE(SalomeMacros)
  FIND_PACKAGE(SalomeKERNEL REQUIRED)
  KERNEL_WITH_CORBA() #check whether KERNEL builded with CORBA
  ADD_DEFINITIONS(${KERNEL_DEFINITIONS})
  INCLUDE_DIRECTORIES(${KERNEL_INCLUDE_DIRS})
ELSE(EXISTS ${KERNEL_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a Salome KERNEL, please define KERNEL_ROOT_DIR")
ENDIF(EXISTS ${KERNEL_ROOT_DIR})

# Platform setup
# ==============
INCLUDE(SalomeSetupPlatform)   # From KERNEL
# Always build libraries as shared objects:
SET(BUILD_SHARED_LIBS TRUE)
# Local macros:
LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/adm_local/cmake_files")

# User options 
# ============
#OPTION(SALOME_BUILD_TESTS "Build SALOME tests" ON)


# Prerequisites
# =============
# Find "big" prerequisites first - they reference themselves many others
#   -> this can help finding the smaller prerequisites and detect conficts.
# In our case KERNEL has already loaded many prereq:

##
## From KERNEL:
##

# Various
FIND_PACKAGE(SalomePython REQUIRED)
FIND_PACKAGE(SalomePThread REQUIRED)
FIND_PACKAGE(SalomeSWIG REQUIRED)
FIND_PACKAGE(SalomeBoost REQUIRED)
FIND_PACKAGE(SalomeOmniORB REQUIRED)
FIND_PACKAGE(SalomeOmniORBPy REQUIRED)
FIND_PACKAGE(SalomeLibXml2 REQUIRED)
FIND_PACKAGE(SalomeHDF5 REQUIRED COMPONENTS C)


# Find GUI (optional)
#qiu IF(SALOME_BUILD_GUI)
  SET(GUI_ROOT_DIR $ENV{GUI_ROOT_DIR} CACHE PATH "Path to the Salome GUI")
  IF(EXISTS ${GUI_ROOT_DIR})
    LIST(APPEND CMAKE_MODULE_PATH "${GUI_ROOT_DIR}/adm_local/cmake_files")
    FIND_PACKAGE(SalomeGUI)
    FULL_GUI(TRUE) #check whether GUI builded in full mode and with CORBA
    ADD_DEFINITIONS(${GUI_DEFINITIONS})
    INCLUDE_DIRECTORIES(${GUI_INCLUDE_DIRS})
  ELSE(EXISTS ${GUI_ROOT_DIR})
    MESSAGE(FATAL_ERROR "We absolutely need a Salome GUI, please define GUI_ROOT_DIR")
  ENDIF(EXISTS ${GUI_ROOT_DIR})
  SALOME_LOG_OPTIONAL_PACKAGE(SalomeGUI SALOME_BUILD_GUI)
#qiu ENDIF(SALOME_BUILD_GUI)

##
## Prerequisites From GUI:
##

#qiu IF(SALOME_BUILD_GUI)

  # Qt4
  FIND_PACKAGE(SalomeQt4 REQUIRED COMPONENTS QtCore QtGui QtXml)
  INCLUDE(${QT_USE_FILE})
  
  MESSAGE ("QTLibsFind: ${QT_LIBRARIES}")

  # Optional prerequisites for GUI
  IF(SALOME_USE_GLVIEWER)
    FIND_PACKAGE(SalomeOpenGL)
    SALOME_LOG_OPTIONAL_PACKAGE(OpenGL SALOME_USE_GLVIEWER)
  ENDIF()
#qiu ENDIF(SALOME_BUILD_GUI)

##
## MCCAD specifics
##

FIND_PACKAGE(SalomeCAS REQUIRED)
FIND_PACKAGE(SalomeVTK  REQUIRED)#qiu
INCLUDE(${VTK_USE_FILE})

# Find MED
# ===========
SET(MED_ROOT_DIR $ENV{MED_ROOT_DIR} CACHE PATH "Path to the Salome MED")
IF(EXISTS ${MED_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${MED_ROOT_DIR}/adm_local/cmake_files")
  FIND_PACKAGE(SalomeMED REQUIRED)
  FIND_PACKAGE(SalomeMEDFile REQUIRED)
  ADD_DEFINITIONS(${MED_DEFINITIONS})
  INCLUDE_DIRECTORIES(${MED_INCLUDE_DIRS})
ELSE(EXISTS ${MED_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a Salome MED, please define MED_ROOT_DIR")
ENDIF(EXISTS ${MED_ROOT_DIR})
# Find GEOM
# ===========
SET(GEOM_ROOT_DIR $ENV{GEOM_ROOT_DIR} CACHE PATH "Path to the Salome GEOM")
IF(EXISTS ${GEOM_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${GEOM_ROOT_DIR}/adm_local/cmake_files")
  FIND_PACKAGE(SalomeGEOM REQUIRED)
  ADD_DEFINITIONS(${GEOM_DEFINITIONS})
  INCLUDE_DIRECTORIES(${GEOM_INCLUDE_DIRS})
ELSE(EXISTS ${GEOM_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a Salome GEOM, please define GEOM_ROOT_DIR")
ENDIF(EXISTS ${GEOM_ROOT_DIR})
# Find SMESH
# ===========
SET(SMESH_ROOT_DIR $ENV{SMESH_ROOT_DIR} CACHE PATH "Path to the Salome SMESH")
IF(EXISTS ${SMESH_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${SMESH_ROOT_DIR}/adm_local/cmake_files")
  FIND_PACKAGE(SalomeSMESH REQUIRED)
  ADD_DEFINITIONS(${SMESH_DEFINITIONS})
  INCLUDE_DIRECTORIES(${SMESH_INCLUDE_DIRS})
ELSE(EXISTS ${SMESH_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a Salome SMESH, please define SMESH_ROOT_DIR")
ENDIF(EXISTS ${SMESH_ROOT_DIR})

## Find TETGEN
## ===========
#SET(TETGEN_ROOT_DIR $ENV{TETGEN_ROOT_DIR} CACHE PATH "Path to the Salome TETGEN")
#IF(EXISTS ${TETGEN_ROOT_DIR})
#  LIST(APPEND CMAKE_MODULE_PATH "${TETGEN_ROOT_DIR}/adm_local/cmake_files")
#  FIND_PACKAGE(TETGEN REQUIRED)
#  ADD_DEFINITIONS(${TETGEN_DEFINITIONS})
#  INCLUDE_DIRECTORIES(${TETGEN_INCLUDE_DIRS})
#ELSE(EXISTS ${TETGEN_ROOT_DIR})
#  MESSAGE(WARNING "We absolutely need a  TETGEN, please define TETGEN_ROOT_DIR")
#ENDIF(EXISTS ${TETGEN_ROOT_DIR})


# Find NETGEN
# ===========
SET(NETGEN_ROOT_DIR $ENV{NETGEN_ROOT_DIR} CACHE PATH "Path to the Salome NETGEN")
IF(EXISTS ${NETGEN_ROOT_DIR})
  LIST(APPEND CMAKE_MODULE_PATH "${NETGEN_ROOT_DIR}/adm_local/cmake_files")
  FIND_PACKAGE(Netgen REQUIRED)
  ADD_DEFINITIONS(${NETGEN_DEFINITIONS})
  INCLUDE_DIRECTORIES(${NETGEN_INCLUDE_DIRS})
ELSE(EXISTS ${NETGEN_ROOT_DIR})
  MESSAGE(FATAL_ERROR "We absolutely need a  NETGEN, please define NETGEN_ROOT_DIR")
ENDIF(EXISTS ${NETGEN_ROOT_DIR})

#qiu  FIND_PACKAGE(McCad REQUIRED)



# Detection summary:
SALOME_PACKAGE_REPORT_AND_CHECK()

# Directories
# (default values taken from KERNEL)
# ===========
SET(SALOME_INSTALL_BINS "${SALOME_INSTALL_BINS}" CACHE PATH "Install path: SALOME binaries")
SET(SALOME_INSTALL_LIBS "${SALOME_INSTALL_LIBS}" CACHE PATH "Install path: SALOME libs")
SET(SALOME_INSTALL_IDLS "${SALOME_INSTALL_IDLS}" CACHE PATH "Install path: SALOME IDL files")
SET(SALOME_INSTALL_HEADERS "${SALOME_INSTALL_HEADERS}" CACHE PATH "Install path: SALOME headers")
SET(SALOME_INSTALL_SCRIPT_SCRIPTS "${SALOME_INSTALL_SCRIPT_SCRIPTS}" CACHE PATH 
   "Install path: SALOME scripts")
SET(SALOME_INSTALL_SCRIPT_DATA "${SALOME_INSTALL_SCRIPT_DATA}" CACHE PATH 
   "Install path: SALOME script data")
SET(SALOME_INSTALL_SCRIPT_PYTHON "${SALOME_INSTALL_SCRIPT_PYTHON}" CACHE PATH 
   "Install path: SALOME Python scripts")
SET(SALOME_INSTALL_PYTHON "${SALOME_INSTALL_PYTHON}" CACHE PATH "Install path: SALOME Python stuff")
SET(SALOME_INSTALL_PYTHON_SHARED "${SALOME_INSTALL_PYTHON_SHARED}" CACHE PATH 
   "Install path: SALOME Python shared modules")
SET(SALOME_INSTALL_CMAKE "${SALOME_INSTALL_CMAKE}" CACHE PATH "Install path: SALOME CMake files")
SET(SALOME_INSTALL_CMAKE_LOCAL "${SALOME_INSTALL_CMAKE_LOCAL}" CACHE PATH 
    "Install path: local SALOME CMake files") 
SET(SALOME_INSTALL_AMCONFIG_LOCAL "${SALOME_INSTALL_AMCONFIG_LOCAL}" CACHE PATH
  "Install path: local SALOME config files (obsolete, to be removed)")

SET(SALOME_INSTALL_RES "${SALOME_INSTALL_RES}" CACHE PATH "Install path: SALOME resources")
SET(SALOME_INSTALL_DOC "${SALOME_INSTALL_DOC}" CACHE PATH "Install path: SALOME documentation")



SET(SALOME_MCCAD_INSTALL_RES_DATA "${SALOME_INSTALL_RES}/mccad" CACHE PATH
    "Install path: SALOME MCCAD specific data")
SET(SALOME_MCCAD_INSTALL_RES_SCRIPTS "${SALOME_INSTALL_RES}/mccad" CACHE PATH
    "Install path: SALOME MCCAD scripts")


MARK_AS_ADVANCED(SALOME_INSTALL_BINS SALOME_INSTALL_LIBS SALOME_INSTALL_IDLS SALOME_INSTALL_HEADERS)
MARK_AS_ADVANCED(SALOME_INSTALL_SCRIPT_SCRIPTS SALOME_INSTALL_SCRIPT_DATA SALOME_INSTALL_SCRIPT_PYTHON)
MARK_AS_ADVANCED(SALOME_INSTALL_APPLISKEL_SCRIPTS  SALOME_INSTALL_APPLISKEL_PYTHON SALOME_INSTALL_CMAKE SALOME_INSTALL_CMAKE_LOCAL SALOME_INSTALL_RES)
MARK_AS_ADVANCED(SALOME_INSTALL_PYTHON SALOME_INSTALL_PYTHON_SHARED)
#qiu MARK_AS_ADVANCED(SALOME_GEOM_INSTALL_RES_DATA SALOME_GEOM_INSTALL_RES_SCRIPTS SALOME_GEOM_INSTALL_PLUGINS)
MARK_AS_ADVANCED(SALOME_MCCAD_INSTALL_RES_DATA SALOME_MCCAD_INSTALL_RES_SCRIPTS)
MARK_AS_ADVANCED(SALOME_INSTALL_AMCONFIG_LOCAL SALOME_INSTALL_DOC)

# Accumulate environment variables for MCCAD module
SALOME_ACCUMULATE_ENVIRONMENT(PYTHONPATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_BINS}
                                                 ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_PYTHON}
                                                 ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_PYTHON_SHARED})
SALOME_ACCUMULATE_ENVIRONMENT(LD_LIBRARY_PATH NOCHECK ${CMAKE_INSTALL_PREFIX}/${SALOME_INSTALL_LIBS}) 

# Sources 
# ========
ADD_SUBDIRECTORY(idl)

ADD_SUBDIRECTORY(adm_local)
ADD_SUBDIRECTORY(resources)
ADD_SUBDIRECTORY(bin)
ADD_SUBDIRECTORY(src)


# Header configuration
# ====================
SALOME_XVERSION(${PROJECT_NAME})
SALOME_CONFIGURE_FILE(MCCAD_version.h.in MCCAD_version.h INSTALL ${SALOME_INSTALL_HEADERS})

# Configuration export
# (here only the level 1 prerequisites are exposed)
# ====================
INCLUDE(CMakePackageConfigHelpers)

# List of targets in this project we want to make visible to the rest of the world.
# They all have to be INSTALL'd with the option "EXPORT ${PROJECT_NAME}TargetGroup"

SET(_${PROJECT_NAME}_exposed_targets
                  MCCADEngine
                  SalomeIDLMCCAD
                  MCCAD
#qiu compile as one library to avoid million of linking issue.
				  TKMcCad
				  Tetgen
#                  TKMcCadAEV
#                  TKMcCadBoolExp
#                  TKMcCadCom
#                  TKMcCadConvertTools
#                  TKMcCadCSG
#                  TKMcCadCSGBuild
#                  TKMcCadCSGGeom
#                  TKMcCadCSGUtil
#                  TKMcCadDiscDs
#                  TKMcCadDiscretization
#                  TKMcCadEXPlug
#                  TKMcCadGTOOL
#                  TKMcCadGUI
#                  TKMcCadIOHelper
#                  TKMcCadIOTools
#                  TKMcCadMcRead
#                  TKMcCadMcVoid
#                  TKMcCadMcWrite
#                  TKMcCadMDReader
#                  TKMcCadMessenger
#                  TKMcCadTDS
##qiu move this library to McCadCom
#                  TKMcCadTool
#                  TKMcCadXCAF
##qiu comment-out this library because McCad GUI is not used anymore
##                  TKMcCadViewTool
#                  TKQMcCadMessenger
##qiu comment-out this library because McCad GUI is not used anymore
##                  TKQMcCadTools
#                  TKMcCadExBRep
#                  TKMcCadExIges
#                  TKMcCadExStep
#                  TKMcCadExMcnp
#                  TKMcCadExTripoli
     )



# Add all targets to the build-tree export set
EXPORT(TARGETS ${_${PROJECT_NAME}_exposed_targets}
  FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)

# Create the configuration files:
#   - in the build tree:

#      Ensure the variables are always defined for the configure:
SET(KERNEL_ROOT_DIR "${KERNEL_ROOT_DIR}")
SET(GUI_ROOT_DIR "${GUI_ROOT_DIR}")
SET(CAS_ROOT_DIR "${CAS_ROOT_DIR}")
SET(VTK_ROOT_DIR "${VTK_ROOT_DIR}")

SET(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include")

# Build variables that will be expanded when configuring Salome<MODULE>Config.cmake:
SALOME_CONFIGURE_PREPARE(CAS VTK)

                         
CONFIGURE_PACKAGE_CONFIG_FILE(${PROJECT_NAME}Config.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}"
    PATH_VARS CONF_INCLUDE_DIRS SALOME_INSTALL_CMAKE_LOCAL CMAKE_INSTALL_PREFIX
    KERNEL_ROOT_DIR GUI_ROOT_DIR CAS_ROOT_DIR VTK_ROOT_DIR )#qiu OPENCV_ROOT_DIR

#   - in the install tree (VSR 16/08/2013: TEMPORARILY COMMENT THIS - TO REMOVE?):
#       Get the relative path of the include directory so 
#       we can register it in the generated configuration files:
#
WRITE_BASIC_PACKAGE_VERSION_FILE(${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${${PROJECT_NAME_UC}_VERSION}
    COMPATIBILITY AnyNewerVersion)
  
# Install the CMake configuration files:
INSTALL(FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}")

# Install the export set for use with the install-tree
INSTALL(EXPORT ${PROJECT_NAME}TargetGroup DESTINATION "${SALOME_INSTALL_CMAKE_LOCAL}" 
        FILE ${PROJECT_NAME}Targets.cmake)
