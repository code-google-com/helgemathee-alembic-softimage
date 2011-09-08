#include "AlembicPolyMsh.h"
#include "AlembicXform.h"

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
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h>
#include <xsi_clusterproperty.h>
#include <xsi_cluster.h>
#include <xsi_geometryaccessor.h>

using namespace XSI;
using namespace MATH;

namespace AbcA = ::Alembic::AbcCoreAbstract::ALEMBIC_VERSION_NS;
using namespace AbcA;

AlembicPolyMesh::AlembicPolyMesh(const XSI::CRef & in_Ref, AlembicWriteJob * in_Job)
: AlembicObject(in_Ref, in_Job)
{
   Primitive prim(GetRef());
   CString xformName(prim.GetParent3DObject().GetName());
   CString meshName(xformName+L"Shape");
   Alembic::AbcGeom::OXform xform(in_Job->GetArchive().getTop(),xformName.GetAsciiString(),GetJob()->GetAnimatedTs());
   Alembic::AbcGeom::OPolyMesh mesh(xform,meshName.GetAsciiString(),GetJob()->GetAnimatedTs());

   mXformSchema = xform.getSchema();
   mMeshSchema = mesh.getSchema();
   mNumSamples = 0;
}

XSI::CStatus AlembicPolyMesh::Save(double time)
{
   // store the transform
   Primitive prim(GetRef());
   SaveXformSample(prim.GetParent3DObject().GetKinematics().GetGlobal().GetRef(),mXformSchema,mXformSample,time);

   // access the mesh
   PolygonMesh mesh = prim.GetGeometry(time);
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
   std::vector<Alembic::Abc::N3f> normalVec;
   std::vector<Alembic::Abc::V2f> uvVec;
   if((bool)GetJob()->GetOption(L"exportNormals"))
   {
      CVector3Array normals = mesh.GetVertices().GetNormalArray();
      if((bool)GetJob()->GetOption(L"vertexNormals"))
      {
         normalVec.resize(vertCount);
         for(LONG i=0;i<vertCount;i++)
         {
            normalVec[i].x = (float)normals[i].GetX();
            normalVec[i].y = (float)normals[i].GetY();
            normalVec[i].z = (float)normals[i].GetZ();
         }
      }
      else
      {
         CGeometryAccessor accessor = mesh.GetGeometryAccessor(siConstructionModeSecondaryShape);
         CFloatArray shadingNormals;
         accessor.GetNodeNormals(shadingNormals);
         normalVec.resize(shadingNormals.GetCount() / 3);

         /*
         // we need to loop the faces, there is nothing we can do
         LONG offset = 0;
         for(LONG i=0;i<faces.GetCount();i++)
         {
            PolygonFace face(faces[i]);
            CLongArray samples = face.GetSamples().GetIndexArray();
            for(LONG j=samples.GetCount()-1;j>=0;j--)
            {
               LONG sampleIndex = samples[j];
               normalVec[sampleIndex].x = (float)shadingNormals[offset++];
               normalVec[sampleIndex].y = (float)shadingNormals[offset++];
               normalVec[sampleIndex].z = (float)shadingNormals[offset++];
               //normalVec[offset].x = shadingNormals[sampleIndex * 3];
               //normalVec[offset].y = shadingNormals[sampleIndex * 3 + 1];
               //normalVec[offset++].z = shadingNormals[sampleIndex * 3 + 2];
            }
         }
         */
         memcpy(&normalVec.front(),shadingNormals.GetArray(),sizeof(float) * shadingNormals.GetCount());
      }
   }

   // allocate for the points and normals
   Alembic::Abc::P3fArraySample posSample(&posVec.front(),posVec.size());

   // if we are the first frame!
   if(mNumSamples == 0)
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

      mMeshSample.setPositions(posSample);
      if(normalVec.size() > 0)
      {
         Alembic::AbcGeom::ON3fGeomParam::Sample normalSample(Alembic::Abc::N3fArraySample(&normalVec.front(),normalVec.size()),Alembic::AbcGeom::kFacevaryingScope);
         mMeshSample.setNormals(normalSample);
      }
      mMeshSample.setFaceCounts(faceCountSample);
      mMeshSample.setFaceIndices(faceIndicesSample);

      // also check if we need to store UV
      CRefArray clusters = mesh.GetClusters();
      if((bool)GetJob()->GetOption(L"exportUVs"))
      {
         CRef uvPropRef;
         for(LONG i=0;i<clusters.GetCount();i++)
         {
            Cluster cluster(clusters[i]);
            if(!cluster.GetType().IsEqualNoCase(L"sample"))
               continue;
            CRefArray props(cluster.GetLocalProperties());
            for(LONG k=0;k<props.GetCount();k++)
            {
               ClusterProperty prop(props[k]);
               if(prop.GetType().IsEqualNoCase(L"uvspace"))
               {
                  uvPropRef = props[k];
                  break;
               }
            }
            if(uvPropRef.IsValid())
               break;
         }
         if(uvPropRef.IsValid())
         {
            // ok, great, we found UVs, let's set them up
            uvVec.resize(sampleCount);
            CDoubleArray uvValues = ClusterProperty(uvPropRef).GetElements().GetArray();

            /*
            LONG offset = 0;
            for(LONG i=0;i<faces.GetCount();i++)
            {
               PolygonFace face(faces[i]);
               CLongArray samples = face.GetSamples().GetIndexArray();
               for(LONG j=samples.GetCount()-1;j>=0;j--)
               {
                  LONG sampleIndex = samples[j];
                  uvVec[sampleIndex].x = (float)uvValues[offset++];
                  uvVec[sampleIndex].y = (float)uvValues[offset++];
                  offset++;
                  //uvVec[offset].x = (float)uvValues[sampleIndex * 3];
                  //uvVec[offset++].y = (float)uvValues[sampleIndex * 3 + 1];
               }
            }
            */

            LONG offset = 0;
            for(LONG i=0;i<sampleCount;i++)
            {
               uvVec[i].x = (float)uvValues[offset++];
               uvVec[i].y = (float)uvValues[offset++];
               offset++;
            }

            Alembic::AbcGeom::OV2fGeomParam::Sample uvSample(Alembic::Abc::V2fArraySample(&uvVec.front(),uvVec.size()),Alembic::AbcGeom::kFacevaryingScope);
            mMeshSample.setUVs(uvSample);
         }
      }

      // sweet, now let's have a look at face sets
      std::vector<std::vector<int32_t>> faceSetVecs;
      if(GetJob()->GetOption(L"exportFaceSets"))
      {
         for(LONG i=0;i<clusters.GetCount();i++)
         {
            Cluster cluster(clusters[i]);
            if(!cluster.GetType().IsEqualNoCase(L"poly"))
               continue;

            CLongArray elements = cluster.GetElements().GetArray();
            if(elements.GetCount() == 0)
               continue;

            std::string name(cluster.GetName().GetAsciiString());

            faceSetVecs.push_back(std::vector<int32_t>());
            std::vector<int32_t> & faceSetVec = faceSetVecs.back();
            for(LONG j=0;j<elements.GetCount();j++)
               faceSetVec.push_back(elements[j]);

            Alembic::AbcGeom::OFaceSet faceSet = mMeshSchema.createFaceSet(name);
            Alembic::AbcGeom::OFaceSetSchema::Sample faceSetSample(Alembic::Abc::Int32ArraySample(&faceSetVec.front(),faceSetVec.size()));
            faceSet.getSchema().set(faceSetSample);
         }
      }

      mMeshSchema.set(mMeshSample);
   }
   else
   {
      mMeshSample.setPositions(posSample);
      if(normalVec.size() > 0)
      {
         Alembic::AbcGeom::ON3fGeomParam::Sample normalSample(Alembic::Abc::N3fArraySample(&normalVec.front(),normalVec.size()),Alembic::AbcGeom::kFacevaryingScope);
         mMeshSample.setNormals(normalSample);
      }
      mMeshSchema.set(mMeshSample);
   }
   mNumSamples++;

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
      pos[(LONG)i].Set(meshPos->get()[i].x,meshPos->get()[i].y,meshPos->get()[i].z);

   Primitive(ctxt.GetOutputTarget()).GetGeometry().GetPoints().PutPositionArray(pos);

   return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus alembic_normals_Define( CRef& in_ctxt )
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

XSIPLUGINCALLBACK CStatus alembic_normals_DefineLayout( CRef& in_ctxt )
{
   Context ctxt( in_ctxt );
   PPGLayout oLayout;
   PPGItem oItem;
   oLayout = ctxt.GetSource();
   oLayout.Clear();
   return CStatus::OK;
}


XSIPLUGINCALLBACK CStatus alembic_normals_Update( CRef& in_ctxt )
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

   CDoubleArray normalValues = ClusterProperty((CRef)ctxt.GetInputValue(0)).GetElements().GetArray();

   Alembic::AbcGeom::IN3fGeomParam meshNormalsParam = obj.getSchema().getNormalsParam();
   if(meshNormalsParam.valid())
   {
      Alembic::Abc::N3fArraySamplePtr meshNormals = meshNormalsParam.getExpandedValue(sampleIndex).getVals();
      if(meshNormals->size() * 3 == normalValues.GetCount())
      {
         // let's apply it!
         LONG offset = 0;
         for(size_t i=0;i<meshNormals->size();i++)
         {
            normalValues[offset++] = meshNormals->get()[i].x;
            normalValues[offset++] = meshNormals->get()[i].y;
            normalValues[offset++] = meshNormals->get()[i].z;
         }
      }
   }

   ClusterProperty(ctxt.GetOutputTarget()).GetElements().PutArray(normalValues);

   return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus alembic_uvs_Define( CRef& in_ctxt )
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

XSIPLUGINCALLBACK CStatus alembic_uvs_DefineLayout( CRef& in_ctxt )
{
   Context ctxt( in_ctxt );
   PPGLayout oLayout;
   PPGItem oItem;
   oLayout = ctxt.GetSource();
   oLayout.Clear();
   return CStatus::OK;
}


XSIPLUGINCALLBACK CStatus alembic_uvs_Update( CRef& in_ctxt )
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

   CDoubleArray uvValues = ClusterProperty((CRef)ctxt.GetInputValue(0)).GetElements().GetArray();

   Alembic::AbcGeom::IV2fGeomParam meshUvParam = obj.getSchema().getUVsParam();
   if(meshUvParam.valid())
   {
      Alembic::Abc::V2fArraySamplePtr meshUVs = meshUvParam.getExpandedValue(sampleIndex).getVals();
      if(meshUVs->size() * 3 == uvValues.GetCount())
      {
         // let's apply it!
         LONG offset = 0;
         for(size_t i=0;i<meshUVs->size();i++)
         {
            uvValues[offset++] = meshUVs->get()[i].x;
            uvValues[offset++] = meshUVs->get()[i].y;
            uvValues[offset++] = 0.0;
         }
      }
   }

   ClusterProperty(ctxt.GetOutputTarget()).GetElements().PutArray(uvValues);

   return CStatus::OK;
}
