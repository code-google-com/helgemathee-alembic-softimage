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

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

// We include some global mesh data to test with from an external source
// to keep this example code clean.
#include <Alembic/AbcGeom/Tests/CurvesData.h>

#include "Assert.h"

using namespace std;
using namespace Alembic::AbcGeom; // Contains Abc, AbcCoreAbstract

//-*****************************************************************************
//-*****************************************************************************
// WRITING OUT AN ANIMATED MESH
//
// Here we'll create an "Archive", which is Alembic's term for the actual
// file on disk containing all of the scene geometry. The Archive will contain
// a single animated Transform with a single static PolyMesh as its child.
//-*****************************************************************************
//-*****************************************************************************

void doSample( OCurves &iCurves )
{

    OCurvesSchema &curves = iCurves.getSchema();

    OFloatGeomParam::Sample widthSample(
        FloatArraySample( (const float *)g_widths, g_numWidths ),
        kVertexScope );

    OV2fGeomParam::Sample uvSample(
        V2fArraySample( (const V2f *)g_uvs, g_totalVerts ),
        kVertexScope );


    std::cout << "creating sample " << curves.getNumSamples() << std::endl;
    OCurvesSchema::Sample curves_sample(
        V3fArraySample( ( const V3f * ) g_verts, g_totalVerts ),
        Int32ArraySample( g_numVerts, g_numCurves),
        kCubic,
        kNonPeriodic,
        widthSample,
        uvSample
                                       );

    std::cout << "setting sample" << std::endl;

    // Set the sample.
    curves.set( curves_sample );

    std::cout << "curves now have " << curves.getNumSamples() << " samples"
              << std::endl;

}

void Example1_CurvesOut()
{
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                      "curves1.abc" );

    OCurves myCurves( OObject( archive, kTop ),
                      "reallly_long_curves_name");

    for ( size_t i = 0 ; i < 5 ; ++i )
    {
        doSample( myCurves );
    }

    // Alembic objects close themselves automatically when they go out
    // of scope. So - we don't have to do anything to finish
    // them off!
    std::cout << "Writing: " << archive.getName() << std::endl;
}


void Example1_CurvesIn()
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), "curves1.abc" );
    std::cout << "Reading: " << archive.getName() << std::endl;

    std::cout <<"constructing curves" << std::endl;
    ICurves myCurves( IObject( archive, kTop) , "reallly_long_curves_name");
    std::cout << "getting schema" << std::endl;
    ICurvesSchema &curves = myCurves.getSchema();
    std::cout << "done getting schema" << std::endl;

    // get the samples from the curves
    ICurvesSchema::Sample curvesSample;
    curves.get( curvesSample );

    // test the bounding box
    TESTING_ASSERT( curvesSample.getSelfBounds().min == V3d( -1.0, -1.0, -1.0 ) );
    std::cout << "bounds max " << curvesSample.getSelfBounds().max << std::endl;
    TESTING_ASSERT( curvesSample.getSelfBounds().max == V3d( 1.0, 1.0, 1.0 ) );

    // test other attributes
    TESTING_ASSERT( curvesSample.getPositions() -> size() == 12);

    IFloatGeomParam::Sample widthSample;
    curves.getWidthsParam().getExpanded( widthSample );
    TESTING_ASSERT( widthSample.getVals()->size() == 12);
    TESTING_ASSERT( widthSample.getScope() == kVertexScope );


}



//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MAIN FUNCTION!
// I'm not going to bother with exceptions, since I have no actions I
// could do to deal with them. If something goes wrong, it will cheerfully
// crash and print the exception information.
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
int main( int argc, char *argv[] )
{

    std::cout << "writing curves" << std::endl;

    // Curves Out
    Example1_CurvesOut();

    std::cout << "wrote curves" << std::endl;

    Example1_CurvesIn();

    return 0;
}
