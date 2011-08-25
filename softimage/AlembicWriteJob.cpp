#include "AlembicWriteJob.h"

#include <xsi_application.h>
#include <xsi_time.h>

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

   CTime localTime;
   mTime.reset(
      new AbcA::TimeSampling(
         AbcA::TimeSamplingType(
            static_cast<Alembic::Util::uint32_t>(mFrames.size()),
            (mFrames.back() - mFrames.front()) / localTime.GetFrameRate()
         ), 
         mFrames
      )
   );

   mArchive.addTimeSampling(*mTime);
   Alembic::Abc::OBox3dProperty box =  Alembic::AbcGeom::CreateOArchiveBounds(mArchive,0);

   // create a test object
    Alembic::AbcGeom::OCamera camera(mArchive.getTop(),"camera",0); // 0 is time

   return CStatus::OK;
}