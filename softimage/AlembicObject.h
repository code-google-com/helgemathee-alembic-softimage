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

public:
   AlembicObject(const XSI::CRef & in_Ref, AlembicWriteJob * in_Job);
   ~AlembicObject();

   AlembicWriteJob * GetJob() { return mJob; }
   const XSI::CRef & GetRef() { return mRef; }

   virtual XSI::CStatus Save(double time) = 0;
};

typedef boost::shared_ptr < AlembicObject > AlembicObjectPtr;

#include "AlembicWriteJob.h"

#endif