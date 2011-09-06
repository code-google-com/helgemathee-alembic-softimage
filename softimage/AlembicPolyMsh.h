#ifndef _ALEMBIC_POLYMSH_H_
#define _ALEMBIC_POLYMSH_H_

#include "AlembicObject.h"

class AlembicPolyMesh: public AlembicObject
{
private:
   Alembic::AbcGeom::OXformSchema mXformSchema;
   Alembic::AbcGeom::OPolyMeshSchema mMeshSchema;
   Alembic::AbcGeom::XformSample mXformSample;
   Alembic::AbcGeom::OPolyMeshSchema::Sample mMeshSample;
   int mNumSamples;
public:

   AlembicPolyMesh(const XSI::CRef & in_Ref, AlembicWriteJob * in_Job);

   virtual XSI::CStatus Save(double time);
};

#endif