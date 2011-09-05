#ifndef _ALEMBIC_CAMERA_H_
#define _ALEMBIC_CAMERA_H_

#include "AlembicObject.h"

class AlembicCamera: public AlembicObject
{
private:
   Alembic::AbcGeom::OCamera mObject;
public:

   AlembicCamera(const XSI::CRef & in_Ref, AlembicWriteJob * in_Job, Alembic::Abc::OObject in_Parent = Alembic::Abc::OObject());

   virtual const Alembic::Abc::OObject & GetObject() { return mObject; }
   virtual XSI::CStatus Save(double time);
};

#endif