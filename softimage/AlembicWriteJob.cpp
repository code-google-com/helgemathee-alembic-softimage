#include "AlembicWriteJob.h"
#include "AlembicObject.h"
#include "AlembicXform.h"
#include "AlembicCamera.h"
#include "AlembicPolyMsh.h"

#include <xsi_application.h>
#include <xsi_time.h>
#include <xsi_project.h>
#include <xsi_scene.h>
#include <xsi_property.h>
#include <xsi_parameter.h>
#include <xsi_x3dobject.h>
#include <xsi_primitive.h>
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h>
#include <xsi_uitoolkit.h>
#include <xsi_progressbar.h>

using namespace XSI;
using namespace MATH;

namespace AbcA = ::Alembic::AbcCoreAbstract::ALEMBIC_VERSION_NS;
using namespace AbcA;

AlembicWriteJob::AlembicWriteJob
(
   const CString & in_FileName,
   const CRefArray & in_Selection,
   const CDoubleArray & in_Frames
)
{
   mFileName = in_FileName;
   mSelection = in_Selection;

   for(LONG i=0;i<in_Frames.GetCount();i++)
      mFrames.push_back(in_Frames[i]);
}

AlembicWriteJob::~AlembicWriteJob()
{
}

void AlembicWriteJob::SetOption(const CString & in_Name, const CValue & in_Value)
{
   std::map<XSI::CString,XSI::CValue>::iterator it = mOptions.find(in_Name);
   if(it == mOptions.end())
      mOptions.insert(std::pair<XSI::CString,XSI::CValue>(in_Name,in_Value));
   else
      it->second = in_Value;
}

bool AlembicWriteJob::HasOption(const CString & in_Name)
{
   std::map<XSI::CString,XSI::CValue>::iterator it = mOptions.find(in_Name);
   return it != mOptions.end();
}

CValue AlembicWriteJob::GetOption(const CString & in_Name)
{
   std::map<XSI::CString,XSI::CValue>::iterator it = mOptions.find(in_Name);
   if(it != mOptions.end())
      return it->second;
   return CValue(false);
}

CStatus AlembicWriteJob::Process()
{
   // check filenames
   if(mFileName.IsEmpty())
   {
      Application().LogMessage(L"[alembic] No filename specified.",siErrorMsg);
      return CStatus::InvalidArgument;
   }

   // check objects
   if(mSelection.GetCount() == 0)
   {
      Application().LogMessage(L"[alembic] No objects specified.",siErrorMsg);
      return CStatus::InvalidArgument;
   }

   // check frames
   if(mFrames.size() == 0)
   {
      Application().LogMessage(L"[alembic] No frames specified.",siErrorMsg);
      return CStatus::InvalidArgument;
   }

   // init archive (use a locally scoped archive)
   CString sceneFileName = L"Exported from: "+Application().GetActiveProject().GetActiveScene().GetParameterValue(L"FileName").GetAsText();
   mArchive = CreateArchiveWithInfo(
         Alembic::AbcCoreHDF5::WriteArchive(),
         mFileName.GetAsciiString(),
         "Softimage Alembic Plugin",
         sceneFileName.GetAsciiString(),
         Alembic::Abc::ErrorHandler::kThrowPolicy);

   // get the frame rate
   double frameRate = 25.0;
   CValue returnVal;
   CValueArray args(1);
   args[0] = L"PlayControl.Rate";
   Application().ExecuteCommand(L"GetValue",args,returnVal);
   frameRate = returnVal;
   if(frameRate == 0.0)
      frameRate = 25.0;
   double timePerSample = 1.0 / frameRate;

   // create the sampling
   AbcA::TimeSampling sampling(timePerSample,0.0);
   mTs = mArchive.addTimeSampling(sampling);

   Alembic::Abc::OBox3dProperty boxProp = Alembic::AbcGeom::CreateOArchiveBounds(mArchive,mTs);

   // create object for each
   std::vector<AlembicObjectPtr> objects;
   for(LONG i=0;i<mSelection.GetCount();i++)
   {
      X3DObject xObj(mSelection[i]);
      if(xObj.GetType().IsEqualNoCase(L"camera"))
      {
         AlembicObjectPtr ptr;
         ptr.reset(new AlembicCamera(xObj.GetActivePrimitive().GetRef(),this));
         objects.push_back(ptr);
      }
      else if(xObj.GetType().IsEqualNoCase(L"polymsh"))
      {
         AlembicObjectPtr ptr;
         ptr.reset(new AlembicPolyMesh(xObj.GetActivePrimitive().GetRef(),this));
         objects.push_back(ptr);
      }
   }

   ProgressBar prog;
   prog = Application().GetUIToolkit().GetProgressBar();
   prog.PutMinimum(0);
   prog.PutMaximum((LONG)(mFrames.size() * objects.size()));
   prog.PutValue(0);
   prog.PutCancelEnabled(true);
   prog.PutVisible(true);

   for(unsigned int frame=0; frame < (unsigned int)mFrames.size(); frame++)
   {
      for(size_t i=0;i<objects.size();i++)
      {
         prog.PutCaption(L"Frame "+CString(mFrames[frame])+L" of "+objects[i]->GetRef().GetAsText());
         CStatus status = objects[i]->Save(mFrames[frame]);
         if(status != CStatus::OK)
            return status;
         if(prog.IsCancelPressed())
            break;
         prog.Increment();
      }
      if(prog.IsCancelPressed())
         break;
   }

   prog.PutVisible(false);

   return CStatus::OK;
}