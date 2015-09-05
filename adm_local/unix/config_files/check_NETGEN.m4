dnl Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl

AC_DEFUN([CHECK_NETGEN],[

AC_REQUIRE([AC_PROG_CXX])dnl
AC_REQUIRE([AC_PROG_CXXCPP])dnl
AC_REQUIRE([CHECK_CAS])dnl

AC_CHECKING([for Netgen])

AC_LANG_SAVE
AC_LANG_CPLUSPLUS

AC_ARG_WITH([netgen],
	    [AC_HELP_STRING([--with-netgen=DIR], [root directory path to NETGEN library installation])],
	    [NETGEN_HOME=$withval],[NETGEN_HOME=yes])

NETGEN_INCLUDES=""
NETGEN_LIBS=""
NETGEN_V5=no

AC_SUBST(NETGEN_INCLUDES)
AC_SUBST(NETGEN_LIBS)

Netgen_ok=no

if test "x$NETGEN_HOME" = "x" -o "x$NETGEN_HOME" = "xyes" -o "x$NETGEN_HOME" = "xauto" ; then
  # auto-detection

#qiu change to new style in 7.4.0 salome.sh
#  if test "x$NETGENHOME" != "x" ; then
   if test "x$NETGEN_ROOT_DIR" != "x" ; then
# - by environment variable NETGENHOME
    #NETGEN_HOME=$NETGENHOME
    NETGEN_HOME=$NETGEN_ROOT_DIR
  else
    # - by searching NETGEN executables
    AC_PATH_PROG([TEMP], [ng_stl])
    if test "x$TEMP" != "x" ; then
      NETGEN_BIN_DIR=`dirname $TEMP`
      NETGEN_HOME=`dirname $NETGEN_BIN_DIR`
    else
      NETGEN_HOME=""
    fi
  fi 
fi

echo NETGEN_HOME=$NETGEN_HOME

if test "x$NETGEN_HOME" != "x"; then

  echo ----------------------------------------------------------
  echo You are about to choose to use somehow the
  echo Netgen Library to generate Tetrahedric mesh.
  echo
  echo You are strongly advised to consult the file
  echo NETGENPLUGIN_SRC/src/NETGEN/ReadMeForNgUsers, particularly
  echo about assumptions made on the installation of the Netgen
  echo application and libraries.
  echo ----------------------------------------------------------

  NETGEN_INCLUDES="-I${NETGEN_HOME}/include -I${NETGEN_HOME}/share/netgen/include"
  NETGEN_LIBS_OLD="-L${NETGEN_HOME}/lib -lnglib"
  NETGEN_LIBS_NEW="-L${NETGEN_HOME}/lib -lnglib -linterface -lgeom2d -lcsg -lstl -locc -lmesh"
  
  dnl check Netgen headers availability 

  CPPFLAGS_old="$CPPFLAGS"
  CXXFLAGS_old="$CXXFLAGS"
  CPPFLAGS="$CAS_CPPFLAGS $NETGEN_INCLUDES $CPPFLAGS"
  CXXFLAGS="$CAS_CXXFLAGS $NETGEN_INCLUDES $CXXFLAGS"

  AC_MSG_CHECKING([for Netgen header file])

  AC_CHECK_HEADER([nglib.h], [Netgen_ok=yes], [Netgen_ok=no])

  dnl check Netgen libraries

  NETGEN_LIBS=${NETGEN_LIBS_NEW}

  LIBS_old="$LIBS"
  LIBS="$NETGEN_LIBS $CAS_LDPATH -lTKernel -lTKMath -lTKG3d -lTKBRep -lTKShHealing -lTKSTEP -lTKXSBase -lTKIGES -lTKSTL -lTKTopAlgo -lTKGeomBase $LIBS_old"

  if test "x$Netgen_ok" = "xyes"; then

    AC_MSG_NOTICE([checking for Netgen libraries])

    AC_MSG_CHECKING([libraries ... try new style ])

    AC_TRY_LINK(
      [
        #include <iostream>
        #include <fstream>
        namespace nglib {
        #include "nglib.h"
        }
      ],
      [
        nglib::Ng_Init();
        nglib::Ng_Exit();
      ],
      [Netgen_ok=yes],
      [Netgen_ok=no])

    if test "x$Netgen_ok" = "xno" ; then

        AC_MSG_RESULT([no])

        AC_MSG_CHECKING([libraries ... try old style])

	NETGEN_LIBS=${NETGEN_LIBS_OLD}
        LIBS="$NETGEN_LIBS $CAS_LDPATH -lTKernel -lTKMath -lTKG3d -lTKBRep -lTKShHealing -lTKSTEP -lTKXSBase -lTKIGES -lTKSTL -lTKTopAlgo -lTKGeomBase $LIBS_old"

        AC_TRY_LINK(
          [
            #include <iostream>
            #include <fstream>
            namespace nglib {
            #include "nglib.h"
            }
          ],
          [
            nglib::Ng_Init();
            nglib::Ng_Exit();
          ],
          [Netgen_ok=yes],
          [Netgen_ok=no])

    fi

  fi

  dnl check if Netgen of v5 or v4

  if test "$Netgen_ok" = "yes" ; then

    AC_MSG_RESULT([yes])

    AC_MSG_CHECKING([Netgen version])

    AC_TRY_LINK(
      [
        #include <iostream>
        #include <fstream>
        #define OCCGEOMETRY
        namespace nglib {
        #include "nglib.h"
        }
        #include <occgeom.hpp>
      ],
      [
        nglib::Ng_Init();
        netgen::Mesh* ngMesh;
        ngMesh->CalcLocalH(1.0);
        nglib::Ng_Exit();
      ],
      [NETGEN_V5=yes],
      [NETGEN_V5=no])

  fi

  dnl check OCCT support in Netgen

  if test "$Netgen_ok" = "yes" ; then

    AC_MSG_RESULT([yes])

    AC_MSG_CHECKING([for OCCT support in Netgen library])

    AC_TRY_LINK(
      [
        #include <iostream>
        #include <fstream>
        #define OCCGEOMETRY
        namespace nglib {
        #include "nglib.h"
        }
      ],
      [
        nglib::Ng_Init();
        nglib::Ng_OCC_Geometry* ng_occ_geom = nglib::Ng_OCC_NewGeometry();
        nglib::Ng_Exit();
      ],
      [Netgen_ok=yes],
      [Netgen_ok=no])

  fi

  dnl check SALOME patch in Netgen

  if test "$Netgen_ok" = "yes" ; then

    AC_MSG_RESULT([yes])

    AC_MSG_CHECKING([for SALOME patch in Netgen library])

    AC_TRY_LINK(
      [
        #include <iostream>
        #include <fstream>
        #define OCCGEOMETRY
        namespace nglib {
        #include "nglib.h"
        }
        #include <occgeom.hpp>
      ],
      [
        nglib::Ng_Init();
        netgen::OCCGeometry occgeo;
        nglib::Ng_Exit();
      ],
      [Netgen_ok=yes],
      [Netgen_ok=no])

  fi

  LIBS="$LIBS_old"
  CPPFLAGS="$CPPFLAGS_old"
  CXXFLAGS="$CXXFLAGS_old"

fi

if test "x$Netgen_ok" = xyes ; then

  AC_MSG_RESULT([yes])

  if test "x$NETGEN_V5" = "xyes" ; then
    AC_MSG_RESULT([Netgen is of version 5.0 or newer])
    NETGEN_INCLUDES="$NETGEN_INCLUDES -DNETGEN_V5"
  else
    AC_MSG_RESULT([Netgen is of version 4.9.13 or older])
  fi

else

  AC_MSG_RESULT([no])
  AC_MSG_ERROR([Netgen is not properly installed. Read NETGENPLUGIN_SRC/src/NETGEN/ReadMeForNgUsers for more details.])

fi

AC_LANG_RESTORE

])dnl
