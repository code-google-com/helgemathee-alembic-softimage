#include "AlembicWriteJob.h"
#include "AlembicObject.h"
#include "AlembicCamera.h"

#include <xsi_application.h>
#include <xsi_time.h>
#include <xsi_project.h>
#include <xsi_scene.h>
#include <xsi_property.h>
#include <xsi_parameter.h>

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

   // init archive
   mArchive = CreateArchiveWithInfo(
      Alembic::AbcCoreHDF5::WriteArchive(),
      mFileName.GetAsciiString(),
      "Softimage Alembic Plugin",
      "Fabric Engine User",
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
   double timePerUnit = (mFrames[mFrames.size()-1] - mFrames[0]) + 0.001;

   AbcA::TimeSamplingType samplingType(static_cast<Alembic::Util::uint32_t>(mFrames.size()),timePerUnit);
   AbcA::TimeSampling sampling(samplingType,mFrames);
   
   /*
      AbcA::TimeSamplingType(
         static_cast<Alembic::Util::uint32_t>(mFrames.size()),
         timePerUnit
      ), 
      mFrames
   ));
   */

   mArchive.addTimeSampling(sampling);
   Alembic::Abc::OBox3dProperty box =  Alembic::AbcGeom::CreateOArchiveBounds(mArchive,0);

   // create object for each
   std::vector<AlembicObject*> objects;
   for(LONG i=0;i<mSelection.GetCount();i++)
   {
      SIObject obj(mSelection[i]);
      if(obj.GetType().IsEqualNoCase(L"camera"))
         objects.push_back(new AlembicCamera(obj.GetRef(),this));
   }

   for(unsigned int frame=0; frame < (unsigned int)mFrames.size(); frame++)
   {
      for(size_t i=0;i<objects.size();i++)
      {
         CStatus status = objects[i]->Save(frame);
         if(status != CStatus::OK)
            return status; 
      }
   }

   for(size_t i=0;i<objects.size();i++)
      delete(objects[i]);

   mArchive.reset();

   return CStatus::OK;
}