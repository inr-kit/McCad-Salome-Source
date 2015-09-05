# - Find NETGEN
# Sets the following variables:
#   NETGEN_INCLUDE_DIRS - path to the NETGEN include directories
#   NETGEN_LIBRARIES    - path to the NETGEN libraries to be linked against
#

#########################################################################
# Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

# ------

IF(NOT Netgen_FIND_QUIETLY)
  MESSAGE(STATUS "Check for Netgen ...")
ENDIF()

# ------

SET(NETGEN_ROOT_DIR $ENV{NETGEN_ROOT_DIR})

IF(NETGEN_ROOT_DIR)
 LIST(APPEND CMAKE_PREFIX_PATH "${NETGEN_ROOT_DIR}")
ENDIF(NETGEN_ROOT_DIR)

FIND_PATH(_netgen_base_inc_dir nglib.h)
SET(NETGEN_INCLUDE_DIRS ${_netgen_base_inc_dir})
FIND_PATH(_netgen_add_inc_dir occgeom.hpp HINTS ${_netgen_base_inc_dir} PATH_SUFFIXES share/netgen/include)
LIST(APPEND NETGEN_INCLUDE_DIRS ${_netgen_add_inc_dir})
LIST(REMOVE_DUPLICATES NETGEN_INCLUDE_DIRS)

FIND_LIBRARY(NETGEN_nglib NAMES nglib)
IF(WIN32)
  FIND_LIBRARY(NETGEN_csg NAMES csg)
  FIND_LIBRARY(NETGEN_gen NAMES gen)
  FIND_LIBRARY(NETGEN_geom2d NAMES geom2d)
  FIND_LIBRARY(NETGEN_gprim NAMES gprim)
  FIND_LIBRARY(NETGEN_interface NAMES interface)
  FIND_LIBRARY(NETGEN_la NAMES la)
  FIND_LIBRARY(NETGEN_mesh NAMES mesh)
  FIND_LIBRARY(NETGEN_occ NAMES occ)
  FIND_LIBRARY(NETGEN_stl NAMES stl)
ENDIF(WIN32)

SET(NETGEN_LIBRARIES ${NETGEN_nglib})

IF(WIN32)
  SET(NETGEN_LIBRARIES ${NETGEN_LIBRARIES}
      ${NETGEN_csg}
      ${NETGEN_gen}
      ${NETGEN_geom2d}
      ${NETGEN_gprim}
      ${NETGEN_interface}
      ${NETGEN_la}
      ${NETGEN_mesh}
      ${NETGEN_occ}
      ${NETGEN_stl}
   )
ENDIF(WIN32)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NETGEN REQUIRED_VARS NETGEN_INCLUDE_DIRS NETGEN_LIBRARIES)

IF(NETGEN_FOUND)
  IF(NOT Netgen_FIND_QUIETLY)
    MESSAGE(STATUS "Netgen library: ${NETGEN_LIBRARIES}")
  ENDIF()
  SET(NETGEN_DEFINITIONS "-DOCCGEOMETRY")

  #RNV:  currently on windows use netgen without thread support.
  #TODO: check support of the multithreading on windows
  IF(WIN32)
   SET(NETGEN_DEFINITIONS "${NETGEN_DEFINITIONS} -DNO_PARALLEL_THREADS")
  ENDIF(WIN32)
ENDIF()
