//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/Abc/All.h>
#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>

namespace Abc = Alembic::Abc;
using namespace Abc;

using Alembic::AbcCoreAbstract::chrono_t;
using Alembic::AbcCoreAbstract::index_t;
using Alembic::Util::uint32_t;

//
// The tests in this file are intended to exercize the Abc API;
//  specifically writing and reading of uniformly samples properties
//

unsigned int g_primes[5] = {2, 3, 5, 7, 11};

Imath::V3f g_vectors[5] = { Imath::V3f(1,1,1),
                            Imath::V3f(2,2,2),
                            Imath::V3f(3,3,3),
                            Imath::V3f(4,4,4),
                            Imath::V3f(5,5,5) };

void writeUInt32ArrayProperty(const std::string &archiveName)
{
    const chrono_t dt = 1.0 / 24.0;
    const chrono_t startTime = 123.0;

    // Create an archive for writing. Indicate that we want Alembic to
    //   throw exceptions on errors.
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                      archiveName, ErrorHandler::kThrowPolicy );
    OObject archiveTop = archive.getTop();

    // Create a child, parented under the archive
    std::string name = "child";
    OObject child( archiveTop, name );
    OCompoundProperty childProps = child.getProperties();

    // Create a scalar property on this child object named 'primes'
    OUInt32ArrayProperty primes( childProps,  // owner
                                 "primes"); // uniform with cycle=dt

    std::vector<uint32_t> vals;
    for (int ss=0; ss<5; ss++)
    {
        // This vector increases in length as we go, increasing the
        //  extent of each successive sample
        vals.push_back( g_primes[ss] );
        primes.set( vals );
    }

    std::vector < chrono_t > timeSamps(1, startTime);
    TimeSamplingPtr ts( new TimeSampling(TimeSamplingType( dt ), timeSamps) );
    primes.setTimeSampling( ts );

    std::cout << archiveName << " was successfully written" << std::endl;
    // Done - the archive closes itself
}


void readUInt32ArrayProperty(const std::string &archiveName)
{
    // Open an existing archive for reading. Indicate that we want
    //   Alembic to throw exceptions on errors.
    std::cout  << "Reading " << archiveName << std::endl;
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                      archiveName, ErrorHandler::kThrowPolicy );
    IObject archiveTop = archive.getTop();

    // Determine the number of (top level) children the archive has
    const unsigned int numChildren =  archiveTop.getNumChildren();
    ABCA_ASSERT( numChildren == 1, "Wrong number of children (expected 1)");
    std::cout << "The archive has " << numChildren << " children:"
              << std::endl;

    // Iterate through them, print out their names
    IObject child( archiveTop, archiveTop.getChildHeader(0).getName() );
    std::cout << "  named '" << child.getName() << "'";

    // Properties
    ICompoundProperty props = child.getProperties();
    size_t numProperties = props.getNumProperties();  // only top-level props
    ABCA_ASSERT( numProperties == 1,
                 "Expected 1 property, found " << numProperties);
    std::cout << " with one property";

    std::vector<std::string> propNames(1);
    propNames[0] = props.getPropertyHeader(0).getName();
    std::cout << " named '" << propNames[0] << "'" << std::endl;


    PropertyType pType = props.getPropertyHeader(0).getPropertyType();
    ABCA_ASSERT( pType == kArrayProperty,
                 "Expected an array property, but didn't find one" );

    std::cout << " which is an array property";

    DataType dType = props.getPropertyHeader(0).getDataType();
    ABCA_ASSERT( dType.getPod() == kUint32POD,
                 "Expected an unsigned int (kUint32POD) property, but didn't"
                 " find one" );

    // We know this is an array property (I'm eliding the if/else
    //  statements required to recognize and handle this properly)
    IUInt32ArrayProperty primes( props, propNames[0] );
    size_t numSamples = primes.getNumSamples();
    std::cout << ".. it has " << numSamples << " samples" << std::endl;
    ABCA_ASSERT( numSamples == 5, "Expected 5 samples, found " << numSamples );

    const TimeSamplingPtr ts = primes.getTimeSampling();
    std::cout << "..with time/value pairs: " << std::endl;;
    for (unsigned int ss=0; ss<numSamples; ss++)
    {
        std::cout << "   ";
        ISampleSelector iss( (index_t) ss);
        std::cout << ts->getSampleTime( (index_t) ss ) << " / ";

        UInt32ArraySamplePtr samplePtr;
        primes.get( samplePtr, iss );
        std::cout << "[ ";
        size_t numPoints = samplePtr->size();
        for ( size_t jj=0 ; jj<numPoints ; jj++ )
            std::cout << (*samplePtr)[jj] << " ";
        std::cout << "]" << std::endl;

        // ASSERT that we are reading the correct values
        for ( size_t jj=0 ; jj<numPoints ; jj++ )
            ABCA_ASSERT( (*samplePtr)[jj] == g_primes[jj],
                         "Incorrect value read from archive." );
    }
    std::cout << std::endl;
    // Done - the archive closes itself
}


///////////////////////////////////////////////////////////////////////////////


void writeV3fArrayProperty(const std::string &archiveName)
{
    const chrono_t dt = 1.0 / 24.0;
    const chrono_t startTime = 123.0;

    // Create an archive for writing. Indicate that we want Alembic to
    //   throw exceptions on errors.
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                      archiveName, ErrorHandler::kThrowPolicy );
    OObject archiveTop = archive.getTop();

    // Create a child, parented under the archive
    std::string name = "particles";
    OObject child( archiveTop, name );
    OCompoundProperty childProps = child.getProperties();

    // add the time sampling the other way
    std::vector < chrono_t > timeSamps(1, startTime);
    TimeSampling ts(TimeSamplingType( dt ), timeSamps);
    uint32_t tsid = archive.addTimeSampling(ts);

    // Create a scalar property on this child object named 'positions'
    OV3fArrayProperty positions( childProps,      // owner
                                 "positions");

    std::vector<V3f> vals;
    vals.push_back( g_vectors[0] );
    positions.set( vals );

    vals.push_back( g_vectors[1] );
    positions.set( vals );


    // The next statement will kill this test with the following error:
    //  terminate called after throwing an instance of 'Alembic::Util::Exception'
    //  what():  OArrayProperty::set()
    //  ERROR: EXCEPTION:
    //  Degenerate array sample in WriteArray
    //  Abort
    vals.clear();
    positions.set( vals );

    vals.push_back( g_vectors[0] );
    vals.push_back( g_vectors[1] );
    vals.push_back( g_vectors[2] );
    vals.push_back( g_vectors[3] );
    positions.set( vals );

    vals.push_back( g_vectors[4] );
    positions.set( vals );

    std::cout << archiveName << " was successfully written" << std::endl;
    positions.setTimeSampling(tsid);

    // Done - the archive closes itself
}


void readV3fArrayProperty(const std::string &archiveName)
{
    // Open an existing archive for reading. Indicate that we want
    //   Alembic to throw exceptions on errors.
    std::cout  << "Reading " << archiveName << std::endl;
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                      archiveName, ErrorHandler::kThrowPolicy );
    IObject archiveTop = archive.getTop();

    // Determine the number of (top level) children the archive has
    const unsigned int numChildren =  archiveTop.getNumChildren();
    ABCA_ASSERT( numChildren == 1, "Wrong number of children (expected 1)");
    std::cout << "The archive has " << numChildren << " children:"
              << std::endl;

    // Iterate through them, print out their names
    IObject child( archiveTop, archiveTop.getChildHeader(0).getName() );
    std::cout << "  named '" << child.getName() << "'";

    // Properties
    ICompoundProperty props = child.getProperties();
    size_t numProperties = props.getNumProperties();  // only top-level props
    ABCA_ASSERT( numProperties == 1,
                 "Expected 1 property, found " << numProperties);
    std::cout << " with one property";

    std::vector<std::string> propNames(1);
    propNames[0] = props.getPropertyHeader(0).getName();
    std::cout << " named '" << propNames[0] << "'" << std::endl;


    PropertyType pType = props.getPropertyHeader(0).getPropertyType();
    ABCA_ASSERT( pType == kArrayProperty,
                 "Expected an array property, but didn't find one" );

    std::cout << " which is an array property";

    DataType dType = props.getPropertyHeader(0).getDataType();
    ABCA_ASSERT( dType.getPod() == kFloat32POD,
                 "Expected an v3f property, but didn't find one" );


    // We know this is an array property (I'm eliding the if/else
    //  statements required to recognize and handle this properly)
    IV3fArrayProperty positions( props, propNames[0] );
    size_t numSamples = positions.getNumSamples();
    std::cout << ".. it has " << numSamples << " samples" << std::endl;
    ABCA_ASSERT( numSamples == 5, "Expected 5 samples, found " << numSamples );

    TimeSamplingPtr ts = positions.getTimeSampling();
    std::cout << "..with time/value pairs: " << std::endl;;
    for (unsigned int ss=0; ss<numSamples; ss++)
    {
        std::cout << "   ";
        ISampleSelector iss( (index_t) ss);
        std::cout << ts->getSampleTime( (index_t) ss ) << " / ";

        V3fArraySamplePtr samplePtr;
        positions.get( samplePtr, iss );
        std::cout << "[ ";
        size_t numPoints = samplePtr->size();
        for ( size_t jj=0 ; jj<numPoints ; jj++ )
            std::cout << (*samplePtr)[jj] << " ";
        std::cout << "]" << std::endl;

        if (ss == 2) // no entries in sample #2
        {
            ABCA_ASSERT( numPoints == 0,
                         "Expected an empty sample, but found " << numPoints
                         << " entries." );
        }
        else
        {
            for ( size_t jj=0 ; jj<numPoints ; jj++ )
                ABCA_ASSERT( (*samplePtr)[jj] == g_vectors[jj],
                             "Incorrect value read from archive." );
        }

    }
    std::cout << std::endl;
    // Done - the archive closes itself
}

////////////////////////////////////////////////////////////////////////////////

void readWriteColorArrayProperty(const std::string &archiveName)
{

    {

        OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(), archiveName,
                          ErrorHandler::kThrowPolicy );
        OObject archiveTop = archive.getTop();

        OObject child( archiveTop, "test" );
        OCompoundProperty childProps = child.getProperties();

        OC3fArrayProperty shades( childProps, "shades", 0 );

        std::vector < C3f > grays(8);
        grays[0].x = 0.0; grays[0].y = 0.0; grays[0].z = 0.0;
        grays[1].x = 0.125; grays[1].y = 0.125; grays[1].z = 0.125;

        grays[2].x = 0.25; grays[2].y = 0.25; grays[2].z = 0.25;
        grays[3].x = 0.375; grays[3].y = 0.375; grays[3].z = 0.375;

        grays[4].x = 0.5; grays[4].y = 0.5; grays[4].z = 0.5;
        grays[5].x = 0.625; grays[5].y = 0.625; grays[5].z = 0.625;

        grays[6].x = 0.75; grays[6].y = 0.75; grays[6].z = 0.75;
        grays[7].x = 0.875; grays[7].y = 0.875; grays[7].z = 0.875;

        // let's write 4 different color3f[2]
        Dimensions d;
        d.setRank(2);
        d[0] = 2;
        d[1] = 4;

        C3fArraySample cas(&(grays.front()), d);
        shades.set(cas);
    }
    {
        // now read it
        IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                          archiveName, ErrorHandler::kThrowPolicy );
        IObject archiveTop = archive.getTop();

        IObject child( archiveTop, archiveTop.getChildHeader(0).getName() );
        ICompoundProperty props = child.getProperties();
        IC3fArrayProperty shades( props, "shades" );

        C3fArraySamplePtr samplePtr;
        shades.get( samplePtr );

        ABCA_ASSERT( samplePtr->getDimensions().rank() == 2,
                     "Incorrect rank on the sample." );

        ABCA_ASSERT( samplePtr->getDimensions().numPoints() == 8,
                     "Incorrect number of total points." );

        ABCA_ASSERT( samplePtr->getDimensions()[0] == 2,
                     "Incorrect size on dimension 0." );

        ABCA_ASSERT( samplePtr->getDimensions()[1] == 4,
                     "Incorrect size on dimension 1." );

        Alembic::Util::Dimensions dims;
        shades.getDimensions( dims );

        ABCA_ASSERT( dims.rank() == 2,
                     "Incorrect rank on the sample." );

        ABCA_ASSERT( dims.numPoints() == 8,
                     "Incorrect number of total points." );

        ABCA_ASSERT( dims[0] == 2,
                     "Incorrect size on dimension 0." );

        ABCA_ASSERT( dims[1] == 4,
                     "Incorrect size on dimension 1." );

        for (size_t i = 0; i < 8; ++i)
        {
            ABCA_ASSERT( (*samplePtr)[i].x == i/8.0 &&
                         (*samplePtr)[i].x == (*samplePtr)[i].y &&
                         (*samplePtr)[i].x == (*samplePtr)[i].z,
                         "Color [" << i << "] is incorrect.");
        }

    }
}

////////////////////////////////////////////////////////////////////////////////


int main( int argc, char *argv[] )
{
    // Write and read a simple archive: one child, with one array
    //  property
    try
    {
        // An array of unsigned ints, written with uniform sampling
        std::string archiveName("uint_array_test.abc");
        writeUInt32ArrayProperty ( archiveName );
        readUInt32ArrayProperty  ( archiveName );
    }
    catch (char * str )
    {
        std::cout << "Exception raised: " << str;
        std::cout << " during UInt array, uniform sampling test" << std::endl;
        return 1;
    }


    try
    {
        // An array of v3fs, written with uniform sampling, some of which
        //  are empty
        std::string archiveName("v3f_array_test.abc");
        writeV3fArrayProperty ( archiveName );
        readV3fArrayProperty  ( archiveName );
    }
    catch (char * str )
    {
        std::cout << "Exception raised: " << str;
        std::cout << " during UInt array, uniform sampling test" << std::endl;
        return 1;
    }

    readWriteColorArrayProperty("c3_2_array_test.abc");
    return 0;
}

