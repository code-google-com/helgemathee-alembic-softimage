##-*****************************************************************************
##
## Copyright (c) 2009-2010, Industrial Light & Magic,
##   a division of Lucasfilm Entertainment Company Ltd.
##
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are
## met:
## *       Redistributions of source code must retain the above copyright
## notice, this list of conditions and the following disclaimer.
## *       Redistributions in binary form must reproduce the above
## copyright notice, this list of conditions and the following disclaimer
## in the documentation and/or other materials provided with the
## distribution.
## *       Neither the name of Industrial Light & Magic nor the names of
## its contributors may be used to endorse or promote products derived
## from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
## OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##-*****************************************************************************


# We shall worry about windowsification later.

#-******************************************************************************
#-******************************************************************************
# FIRST, ILMBASE STUFF
#-******************************************************************************
#-******************************************************************************

IF(NOT DEFINED ILMBASE_ROOT)
    IF ( ${CMAKE_HOST_UNIX} )
        IF( ${DARWIN} )
          # TODO: set to default install path when shipping out
          SET( ALEMBIC_ILMBASE_ROOT NOTFOUND )
        ELSE()
          # TODO: set to default install path when shipping out
          SET( ALEMBIC_ILMBASE_ROOT "/usr/local/ilmbase-1.0.1/" )
        ENDIF()
    ELSE()
        IF ( ${WINDOWS} )
          # TODO: set to 32-bit or 64-bit path
          SET( ALEMBIC_ILMBASE_ROOT "C:/Program Files (x86)/ilmbase-1.0.1/" )
        ELSE()
          SET( ALEMBIC_ILMBASE_ROOT NOTFOUND )
        ENDIF()
    ENDIF()
ELSE()
  SET( ALEMBIC_ILMBASE_ROOT ${ILMBASE_ROOT} )
ENDIF()

IF(NOT DEFINED ILMBASE_ROOT_LIB)
	SET( ILMBASE_ROOT_LIB "${ILMBASE_ROOT}/lib" )
ENDIF()

SET(LIBRARY_PATHS ${ILMBASE_ROOT_LIB})

IF( DEFINED ILMBASE_LIBRARY_DIR )
  SET( LIBRARY_PATHS ${ILMBASE_LIBRARY_DIR} ${LIBRARY_PATHS} )
ENDIF()

SET(INCLUDE_PATHS
    ${ALEMBIC_ILMBASE_ROOT}/include/OpenEXR/
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include/OpenEXR/
    /usr/local/include
    /usr/include
    /usr/include/OpenEXR
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    /usr/freeware/include
)

FIND_PATH( ALEMBIC_ILMBASE_INCLUDE_DIRECTORY ImathMath.h
           PATHS
           ${INCLUDE_PATHS}
           NO_DEFAULT_PATH
           NO_CMAKE_ENVIRONMENT_PATH
           NO_CMAKE_PATH
           NO_SYSTEM_ENVIRONMENT_PATH
           NO_CMAKE_SYSTEM_PATH
           DOC "The directory where ImathMath.h resides" )

IF( NOT DEFINED ALEMBIC_ILMBASE_JOINED_LIB )
  FIND_LIBRARY( ALEMBIC_ILMBASE_JOINED_LIB joined
                PATHS
                ${LIBRARY_PATHS}
                NO_DEFAULT_PATH
                NO_CMAKE_ENVIRONMENT_PATH
                NO_CMAKE_PATH
                NO_SYSTEM_ENVIRONMENT_PATH
                NO_CMAKE_SYSTEM_PATH
                DOC "The joined ilmbase library" )
ENDIF()

IF ( ${ALEMBIC_ILMBASE_JOINED_LIB} STREQUAL "ALEMBIC_ILMBASE_JOINED_LIB-NOTFOUND" )
  MESSAGE( FATAL_ERROR "ilmbase libraries (joined) not found, required" )
ENDIF()

IF ( ${ALEMBIC_ILMBASE_INCLUDE_DIRECTORY} STREQUAL "ALEMBIC_ILMBASE_INCLUDE_DIRECTORY-NOTFOUND" )
  MESSAGE( FATAL_ERROR "ilmbase header files not found, required: ALEMBIC_ILMBASE_ROOT: ${ALEMBIC_ILMBASE_ROOT}" )
ENDIF()


MESSAGE( STATUS "ILMBASE INCLUDE PATH: ${ALEMBIC_ILMBASE_INCLUDE_DIRECTORY}" )
MESSAGE( STATUS "JOINED LIB: ${ALEMBIC_ILMBASE_JOINED_LIB}" )

SET( ILMBASE_FOUND TRUE )

SET( ALEMBIC_ILMBASE_LIBS ${ALEMBIC_ILMBASE_JOINED_LIB} )


