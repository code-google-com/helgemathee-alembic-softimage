#ifndef _ALEMBIC_POLYMSH_H_
#define _ALEMBIC_POLYMSH_H_

#include "AlembicObject.h"

class AlembicPolyMesh: public AlembicObject
{
private:
   Alembic::AbcGeom::OPolyMesh mObject;
public:

   AlembicPolyMesh(const XSI::CRef & in_Ref, AlembicWriteJob * in_Job, Alembic::Abc::OObject in_Parent = Alembic::Abc::OObject());

   virtual const Alembic::Abc::OObject & GetObject() { return mObject; }
   virtual XSI::CStatus Save(double time);
};

#endif