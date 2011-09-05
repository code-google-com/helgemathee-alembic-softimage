#ifndef _ALEMBIC_XFORM_H_
#define _ALEMBIC_XFORM_H_

#include "AlembicObject.h"

class AlembicXform: public AlembicObject
{
private:
   Alembic::AbcGeom::OXform mObject;
public:

   AlembicXform(const XSI::CRef & in_Ref, AlembicWriteJob * in_Job, Alembic::Abc::OObject in_Parent = Alembic::Abc::OObject());

   virtual const Alembic::Abc::OObject & GetObject() { return mObject; }
   virtual XSI::CStatus Save(double time);
};

#endif