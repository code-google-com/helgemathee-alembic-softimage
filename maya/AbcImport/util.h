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

#ifndef ABCIMPORT_UTIL_H_
#define ABCIMPORT_UTIL_H_

#include <cmath>
#include <vector>
#include <set>
#include <string>

#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MString.h>

#include <Alembic/AbcCoreAbstract/TimeSampling.h>
#include <Alembic/Abc/IArchive.h>

// replace one MObject with another, while keeping all the old hierarchy intact
// The objects have to be a Dag object
MStatus replaceDagObject(MObject & oldObject, MObject & newObject,
    const MString & name);

// disconnect all the plugs that are connected to this plug
MStatus disconnectAllPlugsTo(MPlug & dstPlug);

// given a string of names separated by space, this function returns
// a list of Dagpath for the nodes found and prints errors for those not found
MStatus getDagPathListByName(const MString & objectNames,
    std::vector<MDagPath> & dagPathList);

MStatus getObjectByName(const MString & name, MObject & object);

MStatus getDagPathByName(const MString & name, MDagPath & dagPath);

MStatus getPlugByName(const MString & objName,
    const MString & attrName, MPlug & plug);

// set playback range for the current Maya session
MStatus setPlayback(double min, double max);

// grab the set "initialShadingGroup" and add the the dag path to it
MStatus setInitialShadingGroup(const MString & dagNodeName);

MStatus deleteDagNode(MDagPath & dagPath);

MStatus deleteCurrentSelection();

// assuming this is a valid input of /xxx/xxx/xxx/xxx.abc
bool stripFileName(const MString & filePath, MString & fileName);

// remove AlembicNodes that have no outgoing connections
bool removeDangleAlembicNodes();

double getWeightAndIndex(double iFrame,
    Alembic::AbcCoreAbstract::v1::TimeSamplingPtr iTime, size_t numSamps,
    int64_t & oIndex, int64_t & oCeilIndex);
    
template<typename T>
void clamp(T & min, T & max, T & cur)
{
    if (min > max)
        std::swap(min, max);

    cur = std::max(cur, min);
    cur = std::min(cur, max);
}

template<typename T>
T simpleLerp(double alpha, T val1, T val2)
{
    double ret = ( ( 1.0 - alpha ) * static_cast<double>( val1 ) ) +
        ( alpha * static_cast<double>( val2 ) );
    return static_cast<T>( ret );
}

template<typename T>
void vectorLerp(double alpha, std::vector<T> & vec,
    std::vector<T> & vec1, std::vector<T> & vec2)
{
    vec.clear();
    if (vec1.size() != vec2.size())
        return;
    unsigned int size = vec1.size();
    vec.reserve(size);
    for (unsigned int i = 0; i < size; i++)
    {
        T val = (1 - alpha) * vec1[i] + alpha * vec2[i];
        vec.push_back(val);
    }
}

// convert the status to an MString
inline MString getMStatus(MStatus status)
{
    MString theError("Return status: ");
    theError += status.errorString();
    theError += MString("\n");
    return theError;
}

inline void printError(const MString & theError)
{
    MGlobal::displayError(theError);
}

inline void printWarning(const MString & theWarning)
{
    MGlobal::displayWarning(theWarning);
}

#endif  // ABCIMPORT_UTIL_H_
