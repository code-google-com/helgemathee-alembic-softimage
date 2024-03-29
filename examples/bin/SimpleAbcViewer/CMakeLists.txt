##-*****************************************************************************
##
## Copyright (c) 2009-2010,
##  Sony Pictures Imageworks, Inc. and
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
## Industrial Light & Magic nor the names of their contributors may be used
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

SET( H_FILES
     DrawContext.h
     Drawable.h
     Foundation.h
     GLCamera.h
     IObjectDrw.h
     IPointsDrw.h
     IPolyMeshDrw.h
     ISubDDrw.h
     IXformDrw.h
     MeshDrwHelper.h
     Scene.h
     Transport.h
     Viewer.h )

SET( CXX_FILES
     GLCamera.cpp
     IObjectDrw.cpp
     IPointsDrw.cpp
     IPolyMeshDrw.cpp
     ISubDDrw.cpp
     IXformDrw.cpp
     MeshDrwHelper.cpp
     Scene.cpp
     Transport.cpp
     Viewer.cpp
     main.cpp )

SET( SOURCE_FILES ${CXX_FILES} ${H_FILES} )
SET( THIS_LIBS
     AlembicAbcGeom
     AlembicAbc
     AlembicAbcCoreHDF5
     AlembicAbcCoreAbstract
     AlembicMD5Hash
     AlembicGLUtil
     AlembicUtil
     ${ALEMBIC_HDF5_LIBS}
     ${ALEMBIC_GL_LIBS}
     ${ALEMBIC_ILMBASE_LIBS}
     ${Boost_PROGRAM_OPTIONS_LIBRARY}
     ${CMAKE_THREAD_LIBS_INIT}
     ${ZLIB_LIBRARIES} ${EXTERNAL_MATH_LIBS} )

ADD_EXECUTABLE( SimpleAbcViewer ${SOURCE_FILES} )
TARGET_LINK_LIBRARIES( SimpleAbcViewer ${THIS_LIBS} )

INSTALL( TARGETS SimpleAbcViewer
         DESTINATION bin )

#ADD_PYTHON_SCRIPT_INST( SimpleAbcViewerRenderit )

