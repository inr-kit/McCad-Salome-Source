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

INCLUDE(UseOmniORB)

INCLUDE_DIRECTORIES(
  ${KERNEL_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIR}
  ${MED_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

ADD_DEFINITIONS(  ${OMNIORB_DEFINITIONS}  )
IF(WIN32)
  ADD_DEFINITIONS(-DNOGDI)
ENDIF(WIN32)



 SET(SalomeIDLMCCAD_IDLSOURCES
  MCCAD.idl )

SET(IDL_INCLUDE_DIRS
  ${KERNEL_ROOT_DIR}/idl/salome
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${MED_ROOT_DIR}/idl/salome
)
SET(IDL_LINK_FLAGS
  ${KERNEL_SalomeIDLKernel}
  ${MED_SalomeIDLMED}
)

OMNIORB_ADD_MODULE(SalomeIDLMCCAD "${SalomeIDLMCCAD_IDLSOURCES}" "${IDL_INCLUDE_DIRS}" "${IDL_LINK_FLAGS}")
INSTALL(TARGETS SalomeIDLMCCAD EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})
