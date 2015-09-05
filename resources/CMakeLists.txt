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

# ===============================================================
# Files to be installed
# ===============================================================

# These files are data, module or lib files
SET( _res_files
  Component.png
  Group.png
  MCCAD.png
  Part.png
  )
INSTALL(FILES ${_res_files} DESTINATION ${SALOME_MCCAD_INSTALL_RES_DATA})

SALOME_CONFIGURE_FILE(MCCADCatalog.xml.in MCCADCatalog.xml INSTALL ${SALOME_MCCAD_INSTALL_RES_DATA})
SALOME_CONFIGURE_FILE(SalomeApp.xml.in SalomeApp.xml INSTALL ${SALOME_MCCAD_INSTALL_RES_DATA})




