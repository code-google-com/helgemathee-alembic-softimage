#ifndef _ALEMBIC_OBJECT_H_
#define _ALEMBIC_OBJECT_H_

#include "Foundation.h"
#include <xsi_ref.h>
#include <xsi_doublearray.h>

class AlembicWriteJob;

class AlembicObject
{
private:
   XSI::CRef mRef;
   AlembicWriteJob * mJob;
   Alembic::Abc::OObject mParent;

public:
   AlembicObject(const XSI::CRef & in_Ref, AlembicWriteJob * in_Job, Alembic::Abc::OObject in_Parent = Alembic::Abc::OObject());
   ~AlembicObject();

   AlembicWriteJob * GetJob() { return mJob; }
   const XSI::CRef & GetRef() { return mRef; }
   Alembic::Abc::OObject GetParent() { return mParent; }

   virtual const Alembic::Abc::OObject & GetObject() = 0;
   virtual XSI::CStatus Save(double time) = 0;
};

#include "AlembicWriteJob.h"

#endif