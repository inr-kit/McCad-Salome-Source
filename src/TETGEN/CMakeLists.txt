# Set  the minimum  required version  of cmake  for a  project.
#qiu cmake_minimum_required(VERSION 2.6)
#qiu SET(CMAKE_BUILD_TYPE Release)
# Add an executable to the project using the specified source files.
#qiu add_executable(tetgen tetgen.cxx predicates.cxx)

#Add a library to the project using the specified source files. 
# In Linux/Unix, it will creates the libtet.a

ADD_DEFINITIONS(
  -DTETLIBRARY
  )
IF(WIN32)
##qiu##ATTENTION:: we consider WIN64 by default, if not remove "-D_WIN64" in the following line

  ADD_DEFINITIONS(-D_MSC_VER -D_WIN64)
ENDIF(WIN32)
  
add_library(Tetgen SHARED tetgen.cxx predicates.cxx)

#Set properties on a target. 
#We use this here to set -DTETLIBRARY for when compiling the
#library
#qiu set_target_properties(tet PROPERTIES "COMPILE_DEFINITIONS" TETLIBRARY)

#qiu integrate TETGEN into the compilation of McCad
INSTALL(TARGETS Tetgen EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES tetgen.h DESTINATION ${SALOME_INSTALL_HEADERS})



