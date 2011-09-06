#include "AlembicXform.h"
#include <xsi_application.h>
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h>
#include <xsi_x3dobject.h>
#include <xsi_math.h>
#include <xsi_context.h>
#include <xsi_operatorcontext.h>
#include <xsi_customoperator.h>
#include <xsi_factory.h>
#include <xsi_parameter.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgitem.h>

using namespace XSI;
using namespace MATH;

namespace AbcA = ::Alembic::AbcCoreAbstract::ALEMBIC_VERSION_NS;
using namespace AbcA;

void SaveXformSample(XSI::CRef kinestateRef, Alembic::AbcGeom::OXformSchema & schema, Alembic::AbcGeom::XformSample & sample, double time)
{
   CTransformation global = KinematicState(kinestateRef).GetTransform(time);

   // store the transform
   CVector3 trans = global.GetTranslation();
   CVector3 axis;
   double angle = global.GetRotationAxisAngle(axis);
   CVector3 scale = global.GetScaling();
   sample.setTranslation(Imath::V3d(trans.GetX(),trans.GetY(),trans.GetZ()));
   sample.setRotation(Imath::V3d(axis.GetX(),axis.GetY(),axis.GetZ()),RadiansToDegrees(angle));
   sample.setScale(Imath::V3d(scale.GetX(),scale.GetY(),scale.GetZ()));

   // save the sample
   schema.set(sample);
}

XSIPLUGINCALLBACK CStatus alembic_xform_Define( CRef& in_ctxt )
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

XSIPLUGINCALLBACK CStatus alembic_xform_DefineLayout( CRef& in_ctxt )
{
   Context ctxt( in_ctxt );
   PPGLayout oLayout;
   PPGItem oItem;
   oLayout = ctxt.GetSource();
   oLayout.Clear();
   return CStatus::OK;
}


XSIPLUGINCALLBACK CStatus alembic_xform_Update( CRef& in_ctxt )
{
   OperatorContext ctxt( in_ctxt );

   CString path = ctxt.GetParameterValue(L"path");
   CString identifier = ctxt.GetParameterValue(L"identifier");
   Alembic::AbcCoreAbstract::index_t sampleIndex = (Alembic::AbcCoreAbstract::index_t)int(ctxt.GetParameterValue(L"frame"))-1;

   Alembic::AbcGeom::IXform obj(getObjectFromArchive(path,identifier),Alembic::Abc::kWrapExisting);
   if(!obj.valid())
      return CStatus::OK;

   // clamp the sample
   if(sampleIndex < 0)
      sampleIndex = 0;
   else if(sampleIndex >= (Alembic::AbcCoreAbstract::index_t)obj.getSchema().getNumSamples())
      sampleIndex = int(obj.getSchema().getNumSamples()) - 1;

   Alembic::AbcGeom::XformSample sample;
   obj.getSchema().get(sample,sampleIndex);

   CTransformation transform;
   transform.SetTranslationFromValues(sample.getTranslation().x,sample.getTranslation().y,sample.getTranslation().z);
   transform.SetRotationFromAxisAngle(
      CVector3(sample.getAxis().x,sample.getAxis().y,sample.getAxis().z),DegreesToRadians(sample.getAngle()));
   transform.SetScalingFromValues(sample.getScale().x,sample.getScale().y,sample.getScale().z);

   KinematicState state(ctxt.GetOutputTarget());
   state.PutTransform(transform);

   return CStatus::OK;
}

