#ifndef _ALEMBIC_ARCHIVE_STORAGE_H_
#define _ALEMBIC_ARCHIVE_STORAGE_H_

#include "Foundation.h"

#include "AlembicArchiveStorage.h"
#include <map>
#include <xsi_string.h>

Alembic::Abc::IArchive * getArchiveFromID(XSI::CString path);
XSI::CString addArchive(Alembic::Abc::IArchive * archive);
void deleteArchive(XSI::CString path);
void deleteAllArchives();
Alembic::Abc::IObject getObjectFromArchive(XSI::CString path, XSI::CString identifier);

#endif