#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org

AC_DEFUN([CHECK_MCCAD],[

AC_CHECKING(for MCCAD)

MCCAD_ok=no

AC_ARG_WITH(atomic,
	    [  --with-mccad=DIR root directory path of MCCAD module installation ],
	    MCCAD_DIR="$withval",MCCAD_DIR="")

if test "x$MCCAD_DIR" == "x" ; then
   # no --with-atomic-dir option used
   if test "x$MCCAD_ROOT_DIR" != "x" ; then
      # MCCAD_ROOT_DIR environment variable defined
      MCCAD_DIR=$MCCAD_ROOT_DIR
   fi
fi

if test -f ${MCCAD_DIR}/lib/salome/libMCCAD.so ; then
   MCCAD_ok=yes
   AC_MSG_RESULT(Using MCCAD module distribution in ${MCCAD_DIR})

   if test "x$MCCAD_ROOT_DIR" == "x" ; then
      MCCAD_ROOT_DIR=${MCCAD_DIR}
   fi
   AC_SUBST(MCCAD_ROOT_DIR)
else
   AC_MSG_WARN("Cannot find MCCAD module sources")
fi
  
AC_MSG_RESULT(for MCCAD: $MCCAD_ok)
 
])dnl
 
