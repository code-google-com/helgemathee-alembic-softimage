#ifndef _ALEMBIC_CAMERA_H_
#define _ALEMBIC_CAMERA_H_

#include "AlembicObject.h"

class AlembicCamera : public AlembicObject
{
public:

   AlembicCamera(const XSI::CRef & in_Ref, AlembicWriteJob * in_Job) :
      AlembicObject(in_Ref, in_Job) {};

   virtual XSI::CStatus Save(unsigned int frame);
};

#endif