#include "AlembicObject.h"
#include <xsi_ref.h>

namespace AbcA = ::Alembic::AbcCoreAbstract::ALEMBIC_VERSION_NS;
using namespace AbcA;

AlembicObject::AlembicObject
(
   const XSI::CRef & in_Ref,
   AlembicWriteJob * in_Job
)
{
   mRef = in_Ref;
   mJob = in_Job;
}

AlembicObject::~AlembicObject()
{
}
