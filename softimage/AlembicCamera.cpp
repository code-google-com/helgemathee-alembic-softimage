#include "AlembicCamera.h"
#include <xsi_application.h>
#include <xsi_camera.h>
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h>
#include <xsi_math.h>

using namespace XSI;
using namespace MATH;

namespace AbcA = ::Alembic::AbcCoreAbstract::ALEMBIC_VERSION_NS;
using namespace AbcA;

XSI::CStatus AlembicCamera::Save(unsigned int frame)
{
   double time = GetJob()->GetFrames().at(frame);

   // access the camera
   Camera xsiCam(GetRef());
   CTransformation global = xsiCam.GetKinematics().GetGlobal().GetTransform(time);

   Alembic::AbcGeom::OCamera camera(GetJob()->GetArchive()->getTop(),xsiCam.GetFullName().GetAsciiString(),frame);
   Alembic::AbcGeom::OCameraSchema camSchema = camera.getSchema();
   Alembic::AbcGeom::CameraSample camSample;

   CString name(xsiCam.GetFullName()+L".kinematics.global");
   Alembic::AbcGeom::OXform xfo(GetJob()->GetArchive()->getTop(),name.GetAsciiString(),frame);
   Alembic::AbcGeom::OXformSchema xfoSchema = xfo.getSchema();
   Alembic::AbcGeom::XformSample xfoSample;

   // store all of the data into the sample
   //sample.set
   CVector3 trans = global.GetTranslation();
   CVector3 axis;
   double angle = global.GetRotationAxisAngle(axis);
   CVector3 scale = global.GetScaling();
   xfoSample.setTranslation(Imath::V3d(trans.GetX(),trans.GetY(),trans.GetZ()));
   xfoSample.setRotation(Imath::V3d(axis.GetX(),axis.GetY(),axis.GetZ()),angle);
   xfoSample.setScale(Imath::V3d(scale.GetX(),scale.GetY(),scale.GetZ()));

   // save the samples
   camSchema.set(camSample);
   xfoSchema.set(xfoSample);

   return CStatus::OK;
}
