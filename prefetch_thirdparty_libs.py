import os
import urllib2
import zipfile
import tarfile

# set a good umask
os.umask(0002)

def download(url,file):
	request = urllib2.urlopen(url)
	meta = request.info()
	file_size = int(meta.getheaders("Content-Length")[0])
	file_name = file.rpartition("\\")[2]
	print "Downloading: %s Bytes: %s" % (file_name, file_size)
	output = open(file,'wb')

	file_size_dl = 0
	block_sz = 8192
	while True:
		buffer = request.read(block_sz)
		if not buffer:
			break

		file_size_dl += len(buffer)
		output.write(buffer)
		status = r"%10d  [%3.2f%%]" % (file_size_dl, file_size_dl * 100. / file_size)
		status = status + chr(8)*(len(status)+1)
		print status,	
	output.close()
	print('done.                                      ')

# check environment variables
if not os.environ.has_key("ALEMBIC_ROOT") or not os.environ.has_key("ALEMBIC_OUT"):
	raise(Exception("ALEMBIC_ROOT and ALEMBIC_OUT has to be set!"))

# create the thirdparty folder
thirdPartyFolder = os.path.join(os.environ["ALEMBIC_ROOT"],"thirdparty")
if not os.path.exists(thirdPartyFolder):
	os.makedirs(thirdPartyFolder)

# check if we need to download boost
if not os.path.exists(os.path.join(thirdPartyFolder,"boost_1_42_0.tar.gz")):
	download("http://downloads.sourceforge.net/project/boost/boost/1.42.0/boost_1_42_0.tar.gz", os.path.join(thirdPartyFolder,"boost_1_42_0.tar.gz"))

# check if we need to download HDF5
if not os.path.exists(os.path.join(thirdPartyFolder,"hdf5-1.8.7.tar")):
	download("http://www.hdfgroup.org/ftp/HDF5/prev-releases/hdf5-1.8.7/src/hdf5-1.8.7.tar", os.path.join(thirdPartyFolder,"hdf5-1.8.7.tar"))

# check if we need to download Pthreads
if not os.path.exists(os.path.join(thirdPartyFolder,"pthreads-w32-2-8-0-release.exe")):
	download("http://mirrors.kernel.org/sources.redhat.com/pthreads-win32/pthreads-w32-2-8-0-release.exe", os.path.join(thirdPartyFolder,"pthreads-w32-2-8-0-release.exe"))
	download("http://stahlworks.com/dev/unzip.exe", os.path.join(thirdPartyFolder,"unzip.exe"))

# check if we need to download ilmbase
if not os.path.exists(os.path.join(thirdPartyFolder,"ilmbase-1.0.2.tar.gz")):
	download("http://mirrors.zerg.biz/nongnu/openexr/ilmbase-1.0.2.tar.gz", os.path.join(thirdPartyFolder,"ilmbase-1.0.2.tar.gz"))

# check if we need to download zlib
if not os.path.exists(os.path.join(thirdPartyFolder,"zlib-1.2.5.tar.gz")):
	download("http://zlib.net/zlib-1.2.5.tar.gz", os.path.join(thirdPartyFolder,"zlib-1.2.5.tar.gz"))
	
# check if we need to extract boost
if not os.path.exists(os.path.join(thirdPartyFolder,"boost_1_42_0")):
	tar = tarfile.open(os.path.join(thirdPartyFolder,"boost_1_42_0.tar.gz"),'r:gz')
	print("Extracting boost, this may take a couple of minutes...")
	tar.extractall(thirdPartyFolder)

# check if we need to extract hdf5
if not os.path.exists(os.path.join(thirdPartyFolder,"hdf5-1.8.7")):
	tar = tarfile.open(os.path.join(thirdPartyFolder,"hdf5-1.8.7.tar"))
	print("Extracting hdf5, this may take a couple of minutes...")
	tar.extractall(thirdPartyFolder)
	
# also now extract pthread
if not os.path.exists(os.path.join(thirdPartyFolder,"pthreads-w32-2-8-0-release")):
	os.system(os.path.join(thirdPartyFolder,"unzip")+" "+os.path.join(thirdPartyFolder,"pthreads-w32-2-8-0-release.exe")+" -d "+os.path.join(thirdPartyFolder,"pthreads-w32-2-8-0-release"))

# check if we need to extract ilmbase
if not os.path.exists(os.path.join(thirdPartyFolder,"ilmbase-1.0.2")):
	tar = tarfile.open(os.path.join(thirdPartyFolder,"ilmbase-1.0.2.tar.gz"),'r:gz')
	print("Extracting ilmbase, this may take a couple of minutes...")
	tar.extractall(thirdPartyFolder)

# check if we need to extract zlib
if not os.path.exists(os.path.join(thirdPartyFolder,"zlib-1.2.5")):
	tar = tarfile.open(os.path.join(thirdPartyFolder,"zlib-1.2.5.tar.gz"),'r:gz')
	print("Extracting zlib, this may take a couple of minutes...")
	tar.extractall(thirdPartyFolder)


