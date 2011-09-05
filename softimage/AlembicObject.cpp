#include "AlembicObject.h"
#include <xsi_ref.h>

namespace AbcA = ::Alembic::AbcCoreAbstract::ALEMBIC_VERSION_NS;
using namespace AbcA;

AlembicObject::AlembicObject
(
   const XSI::CRef & in_Ref,
   AlembicWriteJob * in_Job,
   Alembic::Abc::OObject in_Parent
)
{
   mRef = in_Ref;
   mJob = in_Job;
   if(in_Parent.valid())
      mParent = in_Parent;
   else
      mParent = mJob->GetArchive()->getTop();
}

AlembicObject::~AlembicObject()
{
}
