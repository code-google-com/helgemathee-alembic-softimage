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

#ifndef _Alembic_AbcCoreAbstract_ScalarPropertyWriter_h_
#define _Alembic_AbcCoreAbstract_ScalarPropertyWriter_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/BasePropertyWriter.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace v1 {

//-*****************************************************************************
//! A Scalar Property is a Rank 0 property which has a single value for each
//! sample. This is distinguished from an Array Property, which has a
//! variable number of elements per sample, and requires more sophisticated
//! resource management.
class ScalarPropertyWriter : public BasePropertyWriter
{
public:
    //! Virtual destructor
    //! ...
    virtual ~ScalarPropertyWriter();

    //-*************************************************************************
    // NEW FUNCTIONS
    //-*************************************************************************

    //! Sets a sample at a given sample index, with
    //! a given time. Depending on the time sampling type,
    //! the sampleTime may be ignored, or it may be checked for consistency
    //! to ensure synchronization.
    //! Samples must always be written starting at index 0, and
    //! moving incrementally forward, writing each subsequent index in order.
    //! An exception will be thrown if the samples are written out of order,
    //! or if the sample times are inconsistent.
    //!
    //! For specifying the sample, this takes a void pointer which
    //! points to the beginning of the memory corresponding to the scalar.
    //!
    //! For String and Wstring, the const void *iSamp is assumed to be
    //! static_castable to const std::string * and const std::wstring *,
    //! respectively.
    //!
    //! The data passed into this function will be used or copied locally
    //! by this function, and need not live (in the calling context)
    //! outside the return scope of this function call.
    virtual void setSample( index_t iSampleIndex,
                            chrono_t iSampleTime,
                            const void *iSamp ) = 0;

    //! Simply copies the previously written sample's value.
    //! This is an important feature.
    virtual void setFromPreviousSample( index_t iSampleIndex,
                                        chrono_t iSampleTime ) = 0;

    //! Return the number of samples that have been written so far.
    //! This changes as samples are written.
    virtual size_t getNumSamples() = 0;
};

} // End namespace v1
} // End namespace AbcCoreAbstract
} // End namespace Alembic

#endif
