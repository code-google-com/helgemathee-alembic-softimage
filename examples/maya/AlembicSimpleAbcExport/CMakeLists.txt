##-*****************************************************************************
##
## Copyright (c) 2009-2010,
##  Sony Pictures Imageworks Inc. and
##  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
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
## *       Neither the name of Sony Pictures Imageworks, nor
## Industrial Light & Magic, nor the names of their contributors may be used
## to endorse or promote products derived from this software without specific
## prior written permission.
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

PROJECT( ALEMBIC_SIMPLE_ABC_EXPORT_MAYA_MODULE )

SET( H_FILES 
  Attribute.h
  Exportable.h
  ExportCmd.h
  Factory.h
  Foundation.h
  GenericNode.h
  MayaUtil.h
  PolyMesh.h
  Top.h
  Transform.h
)

SET( CXX_FILES 
  Attribute.cpp
  Exportable.cpp
  ExportCmd.cpp
  Factory.cpp
  MayaUtil.cpp
  PolyMesh.cpp
  Top.cpp
  Transform.cpp )
SET( SOURCE_FILES ${CXX_FILES} ${H_FILES} )

SET( CORE_LIBS 
  AlembicAbcGeom
  AlembicAbc
  AlembicAbcCoreHDF5
  AlembicAbcCoreAbstract
  AlembicMD5Hash
  AlembicUtil )

INCLUDE_DIRECTORIES( ${ALEMBIC_HDF5_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ".." )

ADD_MAYA_CXX_PLUGIN( AlembicSimpleAbcExport ${SOURCE_FILES} )
TARGET_LINK_LIBRARIES( AlembicSimpleAbcExport
		         ${MAYA_LIBRARIES}
                         ${CORE_LIBS}
                         ${ALEMBIC_HDF5_LIBS} 
			 ${ALEMBIC_ILMBASE_LIBS}
			 ${CMAKE_THREAD_LIBS_INIT}
			 ${ZLIB_LIBRARIES} ${EXTERNAL_MATH_LIBS} )
  
INSTALL( TARGETS AlembicSimpleAbcExport
         DESTINATION maya/plug-ins )
