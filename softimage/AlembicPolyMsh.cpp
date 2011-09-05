#include "AlembicPolyMsh.h"
#include <xsi_application.h>
#include <xsi_x3dobject.h>
#include <xsi_primitive.h>
#include <xsi_geometry.h>
#include <xsi_polygonmesh.h>
#include <xsi_vertex.h>
#include <xsi_polygonface.h>
#include <xsi_sample.h>
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

AlembicPolyMesh::AlembicPolyMesh(const XSI::CRef & in_Ref, AlembicWriteJob * in_Job, Alembic::Abc::OObject in_Parent)
: AlembicObject(in_Ref, in_Job, in_Parent)
{
   X3DObject object(Primitive(GetRef()).GetParent3DObject());
   CString name(object.GetName()+L"Shape");
   mObject = Alembic::AbcGeom::OPolyMesh(GetParent(),name.GetAsciiString(),GetJob()->GetAnimatedTs());
}

XSI::CStatus AlembicPolyMesh::Save(double time)
{
   // access the transform
   PolygonMesh mesh = Primitive(GetRef()).GetGeometry(time);
   CVector3Array pos = mesh.GetVertices().GetPositionArray();
   CPolygonFaceRefArray faces = mesh.GetPolygons();
   LONG vertCount = pos.GetCount();
   LONG faceCount = faces.GetCount();
   LONG sampleCount = mesh.GetSamples().GetCount();

   // allocate the points and normals
   std::vector<Alembic::Abc::V3f> posVec(vertCount);
   for(LONG i=0;i<vertCount;i++)
   {
      posVec[i].x = (float)pos[i].GetX();
      posVec[i].y = (float)pos[i].GetY();
      posVec[i].z = (float)pos[i].GetZ();
   }

   LONG offset = 0;
   /*
   std::vector<Alembic::Abc::V3f> normalVec(sampleCount);
   Alembic::Abc::V3f * normalAlloc = (Alembic::Abc::V3f *)malloc(sizeof(Alembic::Abc::V3f) * sampleCount);
   for(LONG i=0;i<faceCount;i++)
   {
      PolygonFace face(faces[i]);
      CVector3Array normals = face.GetVertices().GetNormalArray();
      for(LONG j=0;j<normals.GetCount();j++)
      {
         normalAlloc[offset].x = (float)normals[j].GetX();
         normalAlloc[offset].y = (float)normals[j].GetY();
         normalAlloc[offset++].z = (float)normals[j].GetZ();
      }
   }
   */

   // allocate for the points and normals
   Alembic::Abc::P3fArraySample posSample(&posVec.front(),posVec.size());
   //Alembic::AbcGeom::ON3fGeomParam::Sample normalSample(Alembic::Abc::N3fArraySample (normalAlloc,vertCount),Alembic::AbcGeom::kFacevaryingScope);

   // if we are the first frame!
   if(mObject.getSchema().getNumSamples() == 0)
   {
      // we also need to store the face counts as well as face indices
      std::vector<int32_t> faceCountVec(faceCount);
      std::vector<int32_t> faceIndicesVec(sampleCount);
      offset = 0;
      for(LONG i=0;i<faceCount;i++)
      {
         PolygonFace face(faces[i]);
         CLongArray indices = face.GetVertices().GetIndexArray();
         faceCountVec[i] = indices.GetCount();
         for(LONG j=indices.GetCount()-1;j>=0;j--)
            faceIndicesVec[offset++] = indices[j];
      }
      Alembic::Abc::Int32ArraySample faceCountSample(&faceCountVec.front(),faceCountVec.size());
      Alembic::Abc::Int32ArraySample faceIndicesSample(&faceIndicesVec.front(),faceIndicesVec.size());

      // also check if we need to store UV
      Alembic::AbcGeom::OPolyMeshSchema::Sample sample(posSample,faceCountSample,faceIndicesSample);
      mObject.getSchema().set(sample);
   }
   else
   {
      Alembic::AbcGeom::OPolyMeshSchema::Sample sample(posSample);
      mObject.getSchema().set(sample);
   }
   //mSample.setNormals(normalSample);

   // save the sample

   return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus alembic_polymesh_Define( CRef& in_ctxt )
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

XSIPLUGINCALLBACK CStatus alembic_polymesh_DefineLayout( CRef& in_ctxt )
{
   Context ctxt( in_ctxt );
   PPGLayout oLayout;
   PPGItem oItem;
   oLayout = ctxt.GetSource();
   oLayout.Clear();
   return CStatus::OK;
}


XSIPLUGINCALLBACK CStatus alembic_polymesh_Update( CRef& in_ctxt )
{
   OperatorContext ctxt( in_ctxt );

   CString path = ctxt.GetParameterValue(L"path");
   CString identifier = ctxt.GetParameterValue(L"identifier");
   Alembic::AbcCoreAbstract::index_t sampleIndex = (Alembic::AbcCoreAbstract::index_t)int(ctxt.GetParameterValue(L"frame"))-1;

   Alembic::AbcGeom::IPolyMesh obj(getObjectFromArchive(path,identifier),Alembic::Abc::kWrapExisting);
   if(!obj.valid())
      return CStatus::OK;

   // clamp the sample
   if(sampleIndex < 0)
      sampleIndex = 0;
   else if(sampleIndex >= (Alembic::AbcCoreAbstract::index_t)obj.getSchema().getNumSamples())
      sampleIndex = int(obj.getSchema().getNumSamples()) - 1;

   Alembic::AbcGeom::IPolyMeshSchema::Sample sample;
   obj.getSchema().get(sample,sampleIndex);

   PolygonMesh inMesh = Primitive((CRef)ctxt.GetInputValue(0)).GetGeometry();
   CVector3Array pos = inMesh.GetPoints().GetPositionArray();

   Alembic::Abc::P3fArraySamplePtr meshPos = sample.getPositions();

   if(pos.GetCount() != meshPos->size())
      return CStatus::OK;

   for(size_t i=0;i<meshPos->size();i++)
      pos[(LONG)i].Set((*meshPos)[i].x,(*meshPos)[i].y,(*meshPos)[i].z);

   Primitive(ctxt.GetOutputTarget()).GetGeometry().GetPoints().PutPositionArray(pos);

   return CStatus::OK;
}
