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

SALOME_CONFIGURE_FILE(VERSION.in VERSION INSTALL ${SALOME_INSTALL_BINS})

# ===============================================================
# Files to be installed
# ===============================================================

SET(_bin_scripts
    myrunSalome.py
)
SALOME_INSTALL_SCRIPTS("${_bin_scripts}" ${SALOME_INSTALL_SCRIPT_SCRIPTS})
