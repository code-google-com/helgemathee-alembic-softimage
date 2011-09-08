#include "AlembicCamera.h"
#include "AlembicXform.h"

#include <xsi_application.h>
#include <xsi_x3dobject.h>
#include <xsi_primitive.h>
#include <xsi_context.h>
#include <xsi_operatorcontext.h>
#include <xsi_customoperator.h>
#include <xsi_factory.h>
#include <xsi_parameter.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgitem.h>
#include <xsi_math.h>
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h>

using namespace XSI;
using namespace MATH;

namespace AbcA = ::Alembic::AbcCoreAbstract::ALEMBIC_VERSION_NS;
using namespace AbcA;

AlembicCamera::AlembicCamera(const XSI::CRef & in_Ref, AlembicWriteJob * in_Job)
: AlembicObject(in_Ref, in_Job)
{
   Primitive prim(GetRef());
   CString xformName(prim.GetParent3DObject().GetName());
   CString cameraName(xformName+L"Shape");
   Alembic::AbcGeom::OXform xform(in_Job->GetArchive().getTop(),xformName.GetAsciiString(),GetJob()->GetAnimatedTs());
   Alembic::AbcGeom::OCamera camera(xform,cameraName.GetAsciiString(),GetJob()->GetAnimatedTs());

   mXformSchema = xform.getSchema();
   mCameraSchema = camera.getSchema();
}

XSI::CStatus AlembicCamera::Save(double time)
{
   // access the camera
   Primitive prim(GetRef());

   // store the transform
   SaveXformSample(prim.GetParent3DObject().GetKinematics().GetGlobal().GetRef(),mXformSchema,mXformSample,time);

   // store the camera data
   mCameraSample.setFocalLength(prim.GetParameterValue(L"projplanedist",time));
   mCameraSample.setVerticalAperture(prim.GetParameterValue(L"projplaneheight",time));
   mCameraSample.setNearClippingPlane(prim.GetParameterValue(L"near",time));
   mCameraSample.setFarClippingPlane(prim.GetParameterValue(L"far",time));
   mCameraSample.setLensSqueezeRatio(prim.GetParameterValue(L"aspect",time));

   // save the samples
   mCameraSchema.set(mCameraSample);

   return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus alembic_camera_Define( CRef& in_ctxt )
{
   Context ctxt( in_ctxt );
   CustomOperator oCustomOperator;

   Parameter oParam;
   CRef oPDef;

   Factory oFactory = Application().GetFactory();
   oCustomOperator = ctxt.GetSource();

   oPDef = oFactory.CreateParamDef(L"frame",CValue::siInt4,siAnimatable | siPersistable,L"frame",L"frame",1,-100000,100000,0,1);
   oCustomOperator.AddParameter(oPDef,oParam);
   oPDef = oFactory.CreateParamDef(L"path",CValue::siString,siReadOnly | siPersistable,L"path",L"path",L"",L"",L"",L"",L"");
   oCustomOperator.AddParameter(oPDef,oParam);
   oPDef = oFactory.CreateParamDef(L"identifier",CValue::siString,siReadOnly | siPersistable,L"identifier",L"identifier",L"",L"",L"",L"",L"");
   oCustomOperator.AddParameter(oPDef,oParam);

   oCustomOperator.PutAlwaysEvaluate(false);
   oCustomOperator.PutDebug(0);

   return CStatus::OK;

}

XSIPLUGINCALLBACK CStatus alembic_camera_DefineLayout( CRef& in_ctxt )
{
   Context ctxt( in_ctxt );
   PPGLayout oLayout;
   PPGItem oItem;
   oLayout = ctxt.GetSource();
   oLayout.Clear();
   return CStatus::OK;
}


XSIPLUGINCALLBACK CStatus alembic_camera_Update( CRef& in_ctxt )
{
   OperatorContext ctxt( in_ctxt );

   CString path = ctxt.GetParameterValue(L"path");
   CString identifier = ctxt.GetParameterValue(L"identifier");
   Alembic::AbcCoreAbstract::index_t sampleIndex = (Alembic::AbcCoreAbstract::index_t)int(ctxt.GetParameterValue(L"frame"))-1;

   Alembic::AbcGeom::ICamera obj(getObjectFromArchive(path,identifier),Alembic::Abc::kWrapExisting);
   if(!obj.valid())
      return CStatus::OK;

   // clamp the sample
   if(sampleIndex < 0)
      sampleIndex = 0;
   else if(sampleIndex >= (Alembic::AbcCoreAbstract::index_t)obj.getSchema().getNumSamples())
      sampleIndex = int(obj.getSchema().getNumSamples()) - 1;

   Alembic::AbcGeom::CameraSample sample;
   obj.getSchema().get(sample,sampleIndex);

   Primitive prim(ctxt.GetOutputTarget());
   prim.PutParameterValue(L"std",0l);
   prim.PutParameterValue(L"aspect",sample.getLensSqueezeRatio());
   prim.PutParameterValue(L"projplane",true);
   prim.PutParameterValue(L"projplanedist",sample.getFocalLength());
   prim.PutParameterValue(L"projplaneheight",sample.getVerticalAperture());
   prim.PutParameterValue(L"near",sample.getNearClippingPlane());
   prim.PutParameterValue(L"far",sample.getFarClippingPlane());
   prim.PutParameterValue(L"far",sample.getFarClippingPlane());

   return CStatus::OK;
}
