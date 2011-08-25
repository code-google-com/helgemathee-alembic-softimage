#ifndef _ALEMBIC_WRITE_JOB_H_
#define _ALEMBIC_WRITE_JOB_H_

#include "Foundation.h"
#include <xsi_ref.h>
#include <xsi_doublearray.h>

class AlembicWriteJob
{
private:
    XSI::CString mFileName;
    XSI::CRefArray mSelection;
    std::vector<double> mFrames;
    Alembic::Abc::OArchive mArchive;
    Alembic::AbcCoreAbstract::TimeSamplingPtr mTime;
public:
   AlembicWriteJob(
      const XSI::CString & in_FileName,
      const XSI::CRefArray & in_Selection,
      const XSI::CDoubleArray & in_Frames);
   ~AlembicWriteJob();

   XSI::CStatus Process();
};

#endif