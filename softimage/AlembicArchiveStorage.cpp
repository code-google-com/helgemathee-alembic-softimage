#include "AlembicArchiveStorage.h"
#include <map>
#include <boost/algorithm/string.hpp>

std::map<std::string,Alembic::Abc::IArchive *> gArchives;

Alembic::Abc::IArchive * getArchiveFromID(XSI::CString path)
{
   std::map<std::string,Alembic::Abc::IArchive *>::iterator it;
   it = gArchives.find(path.GetAsciiString());
   if(it == gArchives.end())
   {
      // check if the file exists
      FILE * file = fopen(path.GetAsciiString(),"rb");
      if(file != NULL)
      {
         fclose(file);
         addArchive(new Alembic::Abc::IArchive( Alembic::AbcCoreHDF5::ReadArchive(), path.GetAsciiString()));
         return getArchiveFromID(path);
      }
      return NULL;
   }

   return it->second;
}

XSI::CString addArchive(Alembic::Abc::IArchive * archive)
{
   gArchives.insert(std::pair<std::string,Alembic::Abc::IArchive *>(archive->getName(),archive));
   return archive->getName().c_str();
}

void deleteArchive(XSI::CString path)
{
  std::map<std::string,Alembic::Abc::IArchive *>::iterator it;
  it = gArchives.find(path.GetAsciiString());
  if(it == gArchives.end())
    return;
  delete(it->second);
  gArchives.erase(it);
}

void deleteAllArchives()
{
  std::map<std::string,Alembic::Abc::IArchive *>::iterator it;
  for(it = gArchives.begin(); it != gArchives.end(); it++)
   delete(it->second);
  gArchives.clear();
}

Alembic::Abc::IObject getObjectFromArchive(XSI::CString path, XSI::CString identifier)
{
  Alembic::Abc::IArchive * archive = getArchiveFromID(path);
  if(archive == NULL)
    return Alembic::Abc::IObject();

  // split the path
  std::string stdIdentifier(identifier.GetAsciiString());
  std::vector<std::string> parts;
  boost::split(parts, stdIdentifier, boost::is_any_of("/"));

  // recurse to find it
  Alembic::Abc::IObject obj = archive->getTop();
  for(size_t i=1;i<parts.size();i++)
  {
    Alembic::Abc::IObject child(obj,parts[i]);
    obj = child;
  }

  return obj;
}
