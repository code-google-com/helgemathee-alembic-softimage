//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks, Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#ifndef _FluidSimDemo_Foundation_h_
#define _FluidSimDemo_Foundation_h_

#include <Alembic/AbcGeom/All.h>
#include <Alembic/GLUtil/GLUtil.h>
#include <Alembic/Util/Util.h>

#include <boost/smart_ptr.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/range/iterator_range.hpp>

#include <ImathMatrix.h>
#include <ImathVec.h>
#include <ImathBox.h>
#include <ImathMatrixAlgo.h>
#include <ImathVec.h>
#include <ImathBoxAlgo.h>
#include <ImathQuat.h>

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <exception>
#include <stdexcept>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

namespace FluidSimDemo {

//-*****************************************************************************
namespace Abc = Alembic::AbcGeom;

//-*****************************************************************************
using Abc::V3f;
using Abc::V3d;
typedef Imath::Vec3<Abc::uint32_t> V3i;
using Abc::Box3f;
using Abc::Box3d;
using Abc::M44f;
using Abc::M44d;

#ifdef DEBUG

#define FSD_THROW( TEXT ) abort()

#else

//-*****************************************************************************
#define FSD_THROW( TEXT )                         \
    do                                            \
    {                                             \
        std::stringstream sstr;                   \
        sstr << TEXT ;                            \
        std::runtime_error exc( sstr.str() );     \
        boost::throw_exception( exc );            \
    }                                             \
    while( 0 )

#endif

} // End namespace FluidSimDemo

#endif
