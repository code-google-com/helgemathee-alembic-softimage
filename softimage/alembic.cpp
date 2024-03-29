// alembicPlugin
// Initial code generated by Softimage SDK Wizard
// Executed Fri Aug 19 09:14:49 UTC+0200 2011 by helge
// 
// Tip: You need to compile the generated code before you can load the plug-in.
// After you compile the plug-in, you can load it by clicking Update All in the Plugin Manager.
#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_menu.h>
#include <xsi_uitoolkit.h>
#include <xsi_progressbar.h>
#include <xsi_comapihandler.h>
#include <xsi_project.h>
#include <xsi_selection.h>
#include <xsi_model.h>
#include <xsi_camera.h>
#include <xsi_customoperator.h>
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h>
#include <xsi_factory.h>
#include <xsi_primitive.h>
#include <xsi_math.h>
#include <xsi_cluster.h>
#include <xsi_clusterproperty.h>
#include <xsi_primitive.h>
#include <xsi_geometry.h>
#include <xsi_polygonmesh.h>
#include <xsi_ppglayout.h>
#include <xsi_ppgitem.h>
#include <xsi_ppgeventcontext.h>

using namespace XSI; 
using namespace MATH; 

#include "AlembicWriteJob.h"

SICALLBACK XSILoadPlugin( PluginRegistrar& in_reg )
{
	in_reg.PutAuthor(L"helge");
	in_reg.PutName(L"alembicPlugin");
	in_reg.PutVersion(1,0);
	in_reg.RegisterCommand(L"alembic_export",L"alembic_export");
	in_reg.RegisterCommand(L"alembic_import",L"alembic_import");
	in_reg.RegisterOperator(L"alembic_xform");
	in_reg.RegisterOperator(L"alembic_camera");
	in_reg.RegisterOperator(L"alembic_polymesh");
	in_reg.RegisterOperator(L"alembic_normals");
	in_reg.RegisterOperator(L"alembic_uvs");
	in_reg.RegisterMenu(siMenuMainFileExportID,L"alembic_MenuExport",false,false);
	in_reg.RegisterMenu(siMenuMainFileImportID,L"alembic_MenuImport",false,false);
   in_reg.RegisterProperty(L"alembic_export_settings");
   in_reg.RegisterProperty(L"alembic_import_settings");
	//RegistrationInsertionPoint - do not remove this line

	return CStatus::OK;
}

SICALLBACK XSIUnloadPlugin( const PluginRegistrar& in_reg )
{
   deleteAllArchives();

	CString strPluginName;
	strPluginName = in_reg.GetName();
	Application().LogMessage(strPluginName + L" has been unloaded.",siVerboseMsg);
	return CStatus::OK;
}

SICALLBACK alembic_export_Init( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	Command oCmd;
	oCmd = ctxt.GetSource();
	oCmd.PutDescription(L"");
	oCmd.EnableReturnValue(true);

	ArgumentArray oArgs;
	oArgs = oCmd.GetArguments();
	oArgs.Add(L"filename");
	oArgs.AddWithHandler(L"objects",L"Collection");
	oArgs.Add(L"frameIn");
	oArgs.Add(L"frameOut");
	oArgs.Add(L"frameStep");
	oArgs.Add(L"exportNormals");
	oArgs.Add(L"vertexNormals");
	oArgs.Add(L"exportUVs");
	oArgs.Add(L"exportFaceSets");
	return CStatus::OK;
}

SICALLBACK alembic_export_Execute( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	CValueArray args = ctxt.GetAttribute(L"Arguments");
	
   // take care of the filename
   CString filename = (CString)args[0].GetAsText();
   if(filename.IsEmpty())
   {
      // let's see if we are in interactive mode
      if(Application().IsInteractive())
      {
         CComAPIHandler toolkit;
         toolkit.CreateInstance(L"XSI.UIToolkit");
         CComAPIHandler filebrowser(toolkit.GetProperty(L"FileBrowser"));
         filebrowser.PutProperty(L"InitialDirectory",Application().GetActiveProject().GetPath());
         filebrowser.PutProperty(L"Filter",L"Alembic Files(*.abc)|*.abc||");
         CValue returnVal;
         filebrowser.Call(L"ShowSave",returnVal);
         filename = filebrowser.GetProperty(L"FilePathName").GetAsText();
         if(filename.IsEmpty())
            return CStatus::Abort;
      }
      else
      {
         Application().LogMessage(L"[alembic] No filename specified.",siErrorMsg);
         return CStatus::InvalidArgument;
      }
   }
   Application().LogMessage(L"[alembic] filename used: "+filename);

   // check the objects
	CRefArray objects = (CRefArray)args[1];
   if(objects.GetCount() == 0)
   {
      // use the selection
      objects = Application().GetSelection().GetArray();
      if(objects.GetCount() == 0)
      {
         Application().LogMessage(L"[alembic] No objects specified.",siErrorMsg);
         return CStatus::InvalidArgument;
      }
   }
   Application().LogMessage(L"[alembic] objects used: "+CValue(objects.GetCount()).GetAsText());

   // check if we have arguments
   if(args[2].GetAsText().IsEmpty())
   {
      // let's setup the property
      CustomProperty settings = Application().GetActiveSceneRoot().AddProperty(L"alembic_export_settings");

      // inspect it
      CValueArray inspectArgs(5);
      CValue inspectResult;
      inspectArgs[0] = settings.GetFullName();
      inspectArgs[1] = L"";
      inspectArgs[2] = L"Export Settings";
      inspectArgs[3] = siModal;
      inspectArgs[4] = false;
      Application().ExecuteCommand(L"InspectObj",inspectArgs,inspectResult);
      
      // prepare for deletion
      inspectArgs.Resize(1);
      inspectArgs[0] = settings.GetFullName();
      if((bool)inspectResult)
      {
         Application().ExecuteCommand(L"DeleteObj",inspectArgs,inspectResult);
         return CStatus::Abort;
      }

      // retrieve the options
      args[2] = settings.GetParameterValue(L"frame_in");
      args[3] = settings.GetParameterValue(L"frame_out");
      args[4] = settings.GetParameterValue(L"frame_step");
      LONG normalMode = settings.GetParameterValue(L"normals");
      switch(normalMode)
      {
         case 0:
         {
            args[5] = false;
            args[6] = false;
            args[7] = false;
            break;
         }
         case 1:
         {
            args[5] = true;
            args[6] = true;
            args[7] = false;
            break;
         }
         case 2:
         {
            args[5] = true;
            args[6] = false;
            args[7] = settings.GetParameterValue(L"uvs");
            break;
         }
      }
      args[8] = settings.GetParameterValue(L"facesets");

      Application().ExecuteCommand(L"DeleteObj",inspectArgs,inspectResult);
   }

   // check the frames
   double frameIn = (double)args[2];
   double frameOut = (double)args[3];
   double frameStep = (double)args[4];
   CDoubleArray frames;
   for(double frame=frameIn; frame<=frameOut; frame+=frameStep)
      frames.Add(frame);
   Application().LogMessage(L"[alembic] frames used: "+CValue(frameIn).GetAsText()+L" to "+CValue(frameOut).GetAsText()+L", step "+CValue(frameStep).GetAsText());

   // create the job
   AlembicWriteJob job(filename,objects,frames);

   // check the options 
   job.SetOption(L"exportNormals",(bool)args[5]);
   job.SetOption(L"vertexNormals",(bool)args[6]);
   job.SetOption(L"exportUVs",(bool)args[7]);
   job.SetOption(L"exportFaceSets",(bool)args[8]);

   // run the job
   deleteAllArchives();
	CStatus result = job.Process();
   deleteAllArchives();
   return result;
}

SICALLBACK alembic_import_Init( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	Command oCmd;
	oCmd = ctxt.GetSource();
	oCmd.PutDescription(L"");
	oCmd.EnableReturnValue(true);

	ArgumentArray oArgs;
	oArgs = oCmd.GetArguments();
	oArgs.Add(L"filename");
   oArgs.Add(L"normals");
   oArgs.Add(L"uvs");
   oArgs.Add(L"clusters");
	return CStatus::OK;
}

SICALLBACK alembic_import_Execute( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	CValueArray args = ctxt.GetAttribute(L"Arguments");
	
   // take care of the filename
   CString filename = (CString)args[0].GetAsText();
   if(filename.IsEmpty())
   {
      // let's see if we are in interactive mode
      if(Application().IsInteractive())
      {
         CComAPIHandler toolkit;
         toolkit.CreateInstance(L"XSI.UIToolkit");
         CComAPIHandler filebrowser(toolkit.GetProperty(L"FileBrowser"));
         filebrowser.PutProperty(L"InitialDirectory",Application().GetActiveProject().GetPath());
         filebrowser.PutProperty(L"Filter",L"Alembic Files(*.abc)|*.abc||");
         CValue returnVal;
         filebrowser.Call(L"ShowOpen",returnVal);
         filename = filebrowser.GetProperty(L"FilePathName").GetAsText();
         if(filename.IsEmpty())
            return CStatus::Abort;
      }
      else
      {
         Application().LogMessage(L"[alembic] No filename specified.",siErrorMsg);
         return CStatus::InvalidArgument;
      }
   }
   Application().LogMessage(L"[alembic] filename used: "+filename);

   // check if we have arguments
   if(args[1].GetAsText().IsEmpty())
   {
      // let's setup the property
      CustomProperty settings = Application().GetActiveSceneRoot().AddProperty(L"alembic_import_settings");

      // inspect it
      CValueArray inspectArgs(5);
      CValue inspectResult;
      inspectArgs[0] = settings.GetFullName();
      inspectArgs[1] = L"";
      inspectArgs[2] = L"Import Settings";
      inspectArgs[3] = siModal;
      inspectArgs[4] = false;
      Application().ExecuteCommand(L"InspectObj",inspectArgs,inspectResult);
      
      // prepare for deletion
      inspectArgs.Resize(1);
      inspectArgs[0] = settings.GetFullName();
      if((bool)inspectResult)
      {
         Application().ExecuteCommand(L"DeleteObj",inspectArgs,inspectResult);
         return CStatus::Abort;
      }

      // retrieve the options
      args[1] = settings.GetParameterValue(L"normals");
      args[2] = settings.GetParameterValue(L"uvs");
      args[3] = settings.GetParameterValue(L"facesets");
   }

   // let's try to read this
   Alembic::Abc::IArchive * archive = new Alembic::Abc::IArchive( Alembic::AbcCoreHDF5::ReadArchive(), filename.GetAsciiString() );

   // prepare values for the setexpr command
   CValueArray setExprArgs(2);
   CValue setExprReturn;
   setExprArgs[1] = L"fc";

   bool importNormals = (bool)args[1];
   bool importUVs = (bool)args[2];
   bool importClusters = (bool)args[3];

   // let's figure out which objects we have
   CRefArray ops;
   std::vector<Alembic::Abc::IObject> objects;
   objects.push_back(archive->getTop());
   for(size_t i=0;i<objects.size();i++)
   {
      // first, let's recurse
      for(size_t j=0;j<objects[i].getNumChildren();j++)
         objects.push_back(objects[i].getChild(j));
   }

   ProgressBar prog;
   prog = Application().GetUIToolkit().GetProgressBar();
   prog.PutMinimum(0);
   prog.PutMaximum((LONG)objects.size());
   prog.PutValue(0);
   prog.PutCancelEnabled(true);
   prog.PutVisible(true);

   for(size_t i=0;i<objects.size();i++)
   {
      prog.PutCaption(L"Importing "+CString(objects[i].getFullName().c_str())+L" ...");

      // now let's see what we have here
      if(Alembic::AbcGeom::ICamera::matches(objects[i].getMetaData()))
      {
         Alembic::Abc::IObject parent = objects[i].getParent();
         std::string name = objects[i].getName();
         if(Alembic::AbcGeom::IXform::matches(parent.getMetaData()))
            name = parent.getName();

         // let's create a camera
         Camera camera;
         Application().GetActiveSceneRoot().AddCamera(L"Camera",name.c_str(),camera);

         // delete the interest
         CValueArray deleteArgs(1);
         deleteArgs[0] = camera.GetInterest().GetFullName();
         Application().ExecuteCommand(L"DeleteObj",deleteArgs,setExprReturn);

         // let's setup the xform op
         if(Alembic::AbcGeom::IXform::matches(parent.getMetaData()))
         {
            CustomOperator op = Application().GetFactory().CreateObject(L"alembic_xform");
            op.AddOutputPort(camera.GetKinematics().GetGlobal().GetRef());
            op.AddInputPort(camera.GetKinematics().GetGlobal().GetRef());
            op.Connect();
            ops.Add(op.GetRef());
            op.PutParameterValue(L"path",filename);
            op.PutParameterValue(L"identifier",CString(parent.getFullName().c_str()));
            setExprArgs[0] = op.GetFullName()+L".frame";
            Application().ExecuteCommand(L"SetExpr",setExprArgs,setExprReturn);
         }
         
         // let's setup the camera op
         CustomOperator op = Application().GetFactory().CreateObject(L"alembic_camera");
         op.AddOutputPort(camera.GetActivePrimitive().GetRef());
         op.AddInputPort(camera.GetActivePrimitive().GetRef());
         op.Connect();
         ops.Add(op.GetRef());
         op.PutParameterValue(L"path",filename);
         op.PutParameterValue(L"identifier",CString(objects[i].getFullName().c_str()));
         setExprArgs[0] = op.GetFullName()+L".frame";
         Application().ExecuteCommand(L"SetExpr",setExprArgs,setExprReturn);
      }
      else if(Alembic::AbcGeom::IPolyMesh::matches(objects[i].getMetaData()))
      {
         Alembic::Abc::IObject parent = objects[i].getParent();
         std::string name = objects[i].getName();
         if(Alembic::AbcGeom::IXform::matches(parent.getMetaData()))
            name = parent.getName();

         // let's create a mesh
         Alembic::AbcGeom::IPolyMesh meshIObject(objects[i],Alembic::Abc::kWrapExisting);
         Alembic::AbcGeom::IPolyMeshSchema meshSchema = meshIObject.getSchema();
         Alembic::AbcGeom::IPolyMeshSchema::Sample meshSample = meshSchema.getValue();

         // prepare the mesh
         Alembic::Abc::P3fArraySamplePtr meshPos = meshSample.getPositions();
         Alembic::Abc::Int32ArraySamplePtr meshFaceCount = meshSample.getFaceCounts();
         Alembic::Abc::Int32ArraySamplePtr meshFaceIndices = meshSample.getFaceIndices();

         CVector3Array pos((LONG)meshPos->size());
         CLongArray polies((LONG)(meshFaceCount->size() + meshFaceIndices->size()));
         for(size_t j=0;j<meshPos->size();j++)
         {
            pos[(LONG)j].Set(meshPos->get()[j].x,meshPos->get()[j].y,meshPos->get()[j].z);
#ifdef _DEBUG
            if(j == 0)
               Application().LogMessage(L"first pos: "+CString(pos[(LONG)j].GetX())+L" : "+CString(pos[(LONG)j].GetY())+L" : "+CString(pos[(LONG)j].GetZ()));
#endif
         }
         LONG offset1 = 0;
         Alembic::Abc::int32_t offset2 = 0;
#ifdef _DEBUG
         LONG meshFaceCountLong = (LONG)meshFaceCount->size();
         LONG meshFaceIndicesLong = (LONG)meshFaceIndices->size();
#endif
         for(size_t j=0;j<meshFaceCount->size();j++)
         {
            Alembic::Abc::int32_t singleFaceCount = meshFaceCount->get()[j];
            polies[offset1++] = singleFaceCount;
            offset2 += singleFaceCount;
            for(size_t k=0;k<singleFaceCount;k++)
            {
               polies[offset1++] = meshFaceIndices->get()[(size_t)offset2 - 1 - k];
            }
         }

         X3DObject meshObj;
         Application().GetActiveSceneRoot().AddPolygonMesh(pos,polies,name.c_str(),meshObj);
         PolygonMesh meshGeo = meshObj.GetActivePrimitive().GetGeometry();

         // check the face sets
         if(importClusters)
         {
            std::vector<std::string> faceSetNames;
            meshSchema.getFaceSetNames(faceSetNames);
            for(size_t j=0;j<faceSetNames.size();j++)
            {
               Alembic::AbcGeom::IFaceSetSchema faceSet = meshSchema.getFaceSet(faceSetNames[j]).getSchema();
               Alembic::AbcGeom::IFaceSetSchema::Sample faceSetSample = faceSet.getValue();
               Alembic::Abc::Int32ArraySamplePtr faces = faceSetSample.getFaces();
               CLongArray elements((LONG)faces->size());
               for(size_t k=0;k<faces->size();k++)
                  elements[(LONG)k] = (LONG)faces->get()[k];
               Cluster cluster;
               meshGeo.AddCluster(L"poly",CString(faceSetNames[j].c_str()),elements,cluster);
            }
         }

         // let's setup the xform op
         if(Alembic::AbcGeom::IXform::matches(parent.getMetaData()))
         {
            CustomOperator op = Application().GetFactory().CreateObject(L"alembic_xform");
            op.AddOutputPort(meshObj.GetKinematics().GetGlobal().GetRef());
            op.AddInputPort(meshObj.GetKinematics().GetGlobal().GetRef());
            op.Connect();
            ops.Add(op.GetRef());
            op.PutParameterValue(L"path",filename);
            op.PutParameterValue(L"identifier",CString(parent.getFullName().c_str()));
            setExprArgs[0] = op.GetFullName()+L".frame";
            Application().ExecuteCommand(L"SetExpr",setExprArgs,setExprReturn);
         }
         
         // let's setup the positions op
         CustomOperator op = Application().GetFactory().CreateObject(L"alembic_polymesh");
         op.AddOutputPort(meshObj.GetActivePrimitive().GetRef());
         op.AddInputPort(meshObj.GetActivePrimitive().GetRef());
         op.Connect();
         ops.Add(op.GetRef());
         op.PutParameterValue(L"path",filename);
         op.PutParameterValue(L"identifier",CString(objects[i].getFullName().c_str()));
         setExprArgs[0] = op.GetFullName()+L".frame";
         Application().ExecuteCommand(L"SetExpr",setExprArgs,setExprReturn);

         // let's setup the normals op
         Alembic::AbcGeom::IN3fGeomParam meshNormalsParam = meshSchema.getNormalsParam();
         if(meshNormalsParam.valid() && importNormals)
         {
            Alembic::Abc::N3fArraySamplePtr meshNormals = meshNormalsParam.getExpandedValue(0).getVals();
            if(meshNormals->size() > meshPos->size()) // don't do this for vertex normals
            {
               // create user normals
               CValueArray createUserNormalArgs(1);
               createUserNormalArgs[0] = meshObj.GetFullName();
               Application().ExecuteCommand(L"CreateUserNormals",createUserNormalArgs,setExprReturn);
               ClusterProperty userNormalProp;
               CRefArray clusters = meshObj.GetActivePrimitive().GetGeometry().GetClusters();
               for(LONG j=0;j<clusters.GetCount();j++)
               {
                  Cluster cluster(clusters[j]);
                  if(!cluster.GetType().IsEqualNoCase(L"sample"))
                     continue;
                  CRefArray props(cluster.GetLocalProperties());
                  for(LONG k=0;k<props.GetCount();k++)
                  {
                     ClusterProperty prop(props[k]);
                     if(prop.GetType().IsEqualNoCase(L"normal"))
                     {
                        userNormalProp = props[k];
                        break;
                     }
                  }
                  if(userNormalProp.IsValid())
                     break;
               }
               if(userNormalProp.IsValid())
               {
                  // we found it, and we need to attach the op
                  CustomOperator op = Application().GetFactory().CreateObject(L"alembic_normals");
                  op.AddOutputPort(userNormalProp.GetRef());
                  op.AddInputPort(userNormalProp.GetRef());
                  op.Connect();
                  ops.Add(op.GetRef());
                  op.PutParameterValue(L"path",filename);
                  op.PutParameterValue(L"identifier",CString(objects[i].getFullName().c_str()));
                  setExprArgs[0] = op.GetFullName()+L".frame";
                  Application().ExecuteCommand(L"SetExpr",setExprArgs,setExprReturn);
               }
            }
         }
         // let's setup the normals op
         Alembic::AbcGeom::IV2fGeomParam meshUVsParam = meshSchema.getUVsParam();
         if(meshUVsParam.valid() && importUVs)
         {
            Alembic::Abc::V2fArraySamplePtr meshUVs = meshUVsParam.getExpandedValue(0).getVals();
            if(meshUVs->size() > 0)
            {
               // create user normals
               CValueArray createProjectionArgs(3);
               createProjectionArgs[0] = meshObj.GetFullName();
               createProjectionArgs[1] = siTxtSpatial;
               createProjectionArgs[2] = siTxtDefaultPlanarXY;
               Application().ExecuteCommand(L"CreateProjection",createProjectionArgs,setExprReturn);
               ClusterProperty uvProp;
               CRefArray clusters = meshObj.GetActivePrimitive().GetGeometry().GetClusters();
               for(LONG j=0;j<clusters.GetCount();j++)
               {
                  Cluster cluster(clusters[j]);
                  if(!cluster.GetType().IsEqualNoCase(L"sample"))
                     continue;
                  CRefArray props(cluster.GetLocalProperties());
                  for(LONG k=0;k<props.GetCount();k++)
                  {
                     ClusterProperty prop(props[k]);
                     if(prop.GetType().IsEqualNoCase(L"uvspace"))
                     {
                        uvProp = props[k];
                        break;
                     }
                  }
                  if(uvProp.IsValid())
                     break;
               }
               if(uvProp.IsValid())
               {
                  // we found it, and we need to attach the op
                  CustomOperator op = Application().GetFactory().CreateObject(L"alembic_uvs");
                  op.AddOutputPort(uvProp.GetRef());
                  op.AddInputPort(uvProp.GetRef());
                  op.Connect();
                  ops.Add(op.GetRef());
                  op.PutParameterValue(L"path",filename);
                  op.PutParameterValue(L"identifier",CString(objects[i].getFullName().c_str()));
                  // we don't put an expression, since UVs typically don't animate
               }
            }
         }
      }

      if(prog.IsCancelPressed())
         break;
      prog.Increment();
   }

   prog.PutVisible(false);

   // check if we have any ops, if so let's set them up
   if(ops.GetCount() > 0)
      addArchive(archive);
   else
      delete(archive);

   return CStatus::OK;
}

SICALLBACK alembic_MenuExport_Init( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	Menu oMenu;
	oMenu = ctxt.GetSource();
	MenuItem oNewItem;
	oMenu.AddCommandItem(L"Alembic 1.0",L"alembic_export",oNewItem);
	return CStatus::OK;
}

SICALLBACK alembic_MenuImport_Init( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	Menu oMenu;
	oMenu = ctxt.GetSource();
	MenuItem oNewItem;
	oMenu.AddCommandItem(L"Alembic 1.0",L"alembic_import",oNewItem);
	return CStatus::OK;
}

SICALLBACK alembic_export_settings_Define( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	CustomProperty oCustomProperty;
	Parameter oParam;
	oCustomProperty = ctxt.GetSource();

   // get the current frame in an out
   CValueArray cmdArgs(1);
   CValue cmdReturnVal;
   cmdArgs[0] = L"PlayControl.In";
   Application().ExecuteCommand(L"GetValue",cmdArgs,cmdReturnVal);
   oCustomProperty.AddParameter(L"frame_in",CValue::siInt4,siPersistable,L"",L"",cmdReturnVal,-1000000,1000000,1,100,oParam);
   cmdArgs[0] = L"PlayControl.Out";
   Application().ExecuteCommand(L"GetValue",cmdArgs,cmdReturnVal);
   oCustomProperty.AddParameter(L"frame_out",CValue::siInt4,siPersistable,L"",L"",cmdReturnVal,-1000000,1000000,1,100,oParam);
   oCustomProperty.AddParameter(L"frame_step",CValue::siInt4,siPersistable,L"",L"",1,-1000000,1000000,1,5,oParam);

	oCustomProperty.AddParameter(L"normals",CValue::siInt4,siPersistable,L"",L"",2,0,10,0,10,oParam);
   oCustomProperty.AddParameter(L"uvs",CValue::siBool,siPersistable,L"",L"",1,0,1,0,1,oParam);
   oCustomProperty.AddParameter(L"facesets",CValue::siBool,siPersistable,L"",L"",1,0,1,0,1,oParam);
	return CStatus::OK;
}

SICALLBACK alembic_export_settings_DefineLayout( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	PPGLayout oLayout;
	PPGItem oItem;
	oLayout = ctxt.GetSource();
	oLayout.Clear();

   oLayout.AddGroup(L"Animation");
   oLayout.AddItem(L"frame_in",L"In");
   oLayout.AddItem(L"frame_out",L"Out");
   oLayout.AddItem(L"frame_step",L"Step");
   oLayout.EndGroup();

   CValueArray normalItems(6);
   oLayout.AddGroup(L"Geometry");
   normalItems[0] = L"No Normals";
   normalItems[1] = 0l;
   normalItems[2] = L"Per Vertex Normals";
   normalItems[3] = 1l;
   normalItems[4] = L"Per Face Normals";
   normalItems[5] = 2l;
   oLayout.AddEnumControl(L"normals",normalItems,L"Normals");
   oLayout.AddItem(L"uvs",L"UVs");
   oLayout.AddItem(L"facesets",L"Clusters");
   oLayout.EndGroup();

	return CStatus::OK;
}

XSIPLUGINCALLBACK CStatus alembic_export_settings_PPGEvent( const CRef& in_ctxt )
{
	PPGEventContext ctxt( in_ctxt ) ;
	if ( ctxt.GetEventID() == PPGEventContext::siParameterChange )
	{
		Parameter param = ctxt.GetSource() ;	
		CString paramName = param.GetScriptName() ; 

      // depending on what we changed
      if(paramName.IsEqualNoCase(L"normals"))
      {
         Property(param.GetParent()).GetParameter(L"uvs").PutCapabilityFlag(siReadOnly,(LONG)param.GetValue() != 2l);
      }
	}
   else if (ctxt.GetEventID() == PPGEventContext::siButtonClicked)
   {
      CString buttonName = ctxt.GetAttribute(L"Button");
   }

	return CStatus::OK ;
}

SICALLBACK alembic_import_settings_Define( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	CustomProperty oCustomProperty;
	Parameter oParam;
	oCustomProperty = ctxt.GetSource();

   // get the current frame in an out
	oCustomProperty.AddParameter(L"normals",CValue::siBool,siPersistable,L"",L"",0,0,1,0,1,oParam);
   oCustomProperty.AddParameter(L"uvs",CValue::siBool,siPersistable,L"",L"",1,0,1,0,1,oParam);
   oCustomProperty.AddParameter(L"facesets",CValue::siBool,siPersistable,L"",L"",1,0,1,0,1,oParam);
	return CStatus::OK;
}

SICALLBACK alembic_import_settings_DefineLayout( CRef& in_ctxt )
{
	Context ctxt( in_ctxt );
	PPGLayout oLayout;
	PPGItem oItem;
	oLayout = ctxt.GetSource();
	oLayout.Clear();

   CValueArray normalItems(6);
   oLayout.AddGroup(L"Geometry");
   oLayout.AddItem(L"normals",L"Normals");
   oLayout.AddItem(L"uvs",L"UVs");
   oLayout.AddItem(L"facesets",L"Clusters");
   oLayout.EndGroup();

	return CStatus::OK;
}
