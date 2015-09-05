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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Library target
lib_LTLIBRARIES = libMCCADEngine.la

# Library sources
dist_libMCCADEngine_la_SOURCES = \
        MCCAD.hxx \
        MCCAD.cxx \
        MCMESHTRAN_AbaqusMeshWriter.cxx \
        MCMESHTRAN_AbaqusMeshWriter.hxx

libMCCADEngine_la_CPPFLAGS =  \
        $(CORBA_CXXFLAGS) \
        $(CORBA_INCLUDES) \
        -I$(top_builddir)/idl \
        $(QT_INCLUDES) \
        $(KERNEL_CXXFLAGS)  \
        $(MED_CXXFLAGS) \
        $(MED3_INCLUDES) \
        @CPPUNIT_INCLUDES@ \
        $(CAS_CPPFLAGS) \
        $(MCCAD_INCLUDES) \
        $(TETGEN_INCLUDES) \
        $(NETGEN_INCLUDES)


libMCCADEngine_la_LDFLAGS = \
        ../../idl/libMCCADIDL.la \
        $(KERNEL_LDFLAGS) \
        -lSalomeContainer \
        -lOpUtil \
        $(MED_LDFLAGS) \
        -lmedcouplingclient \
        -lmedcouplingcorba \
        -lmedloader \
        -lSalomeIDLMED \
        -lmedcouplingremapper \
        $(HDF5_LIBS) \
        @CPPUNIT_LIBS@ \
        $(MED3_LIBS_C_ONLY) \
        $(CAS_LDFLAGS) \
        $(MCCAD_LIBS) -lTKMcCadMcVoid -lTKMcCadConvertTools \
        $(CAS_LDPATH) -lTKSTL \
        $(TETGEN_LIBS) \
        $(NETGEN_LIBS)
#        $(CAS_DATAEXCHANGE)


