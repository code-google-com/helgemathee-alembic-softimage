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
public:
   AlembicWriteJob(
      const XSI::CString & in_FileName,
      const XSI::CRefArray & in_Selection,
      const XSI::CDoubleArray & in_Frames);
   ~AlembicWriteJob();

   Alembic::Abc::OArchive * GetArchive() { return &mArchive; }
   const std::vector<double> & GetFrames() { return mFrames; }
   const XSI::CString & GetFileName() { return mFileName; }
 
   XSI::CStatus Process();
};

#endif