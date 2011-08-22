import os
import urllib
import zipfile
import tarfile

# set a good umask
os.umask(0002)

# check environment variables
if not os.environ.has_key("ALEMBIC_ROOT") or not os.environ.has_key("ALEMBIC_OUT"):
	raise(Exception("ALEMBIC_ROOT and ALEMBIC_OUT has to be set!"))

# create the thirdparty folder
thirdPartyFolder = os.path.join(os.environ["ALEMBIC_ROOT"],"thirdparty")
if not os.path.exists(thirdPartyFolder):
	os.makedirs(thirdPartyFolder)

# check if we need to download boost
if not os.path.exists(os.path.join(thirdPartyFolder,"boost_1_42_0.tar.gz")):
	print("Downloading boost 1.42.0, this can take a couple of minutes...")
	urllib.urlretrieve("http://downloads.sourceforge.net/project/boost/boost/1.42.0/boost_1_42_0.tar.gz", os.path.join(thirdPartyFolder,"boost_1_42_0.tar.gz"))

# check if we need to download HDF5
if not os.path.exists(os.path.join(thirdPartyFolder,"hdf5-1.8.7.tar")):
	print("Downloading hdf5 1.8.7, this can take a couple of minutes...")
	urllib.urlretrieve("http://www.hdfgroup.org/ftp/HDF5/prev-releases/hdf5-1.8.7/src/hdf5-1.8.7.tar", os.path.join(thirdPartyFolder,"hdf5-1.8.7.tar"))

# check if we need to download ilmbase
if not os.path.exists(os.path.join(thirdPartyFolder,"ilmbase-1.0.2.tar.gz")):
	print("Downloading ilmbase 1.0.2, this can take a couple of minutes...")
	urllib.urlretrieve("http://download.savannah.nongnu.org/releases/openexr/ilmbase-1.0.2.tar.gz", os.path.join(thirdPartyFolder,"ilmbase-1.0.2.tar.gz"))

# check if we need to download openexr
if not os.path.exists(os.path.join(thirdPartyFolder,"openexr-1.6.1.tar.gz")):
	print("Downloading openexr 1.6.1, this can take a couple of minutes...")
	urllib.urlretrieve("http://download.savannah.nongnu.org/releases/openexr/openexr-1.6.1.tar.gz", os.path.join(thirdPartyFolder,"openexr-1.6.1.tar.gz"))

# check if we need to download zlib
if not os.path.exists(os.path.join(thirdPartyFolder,"zlib-1.2.5.tar.gz")):
	print("Downloading zlib 1.2.5, this can take a couple of minutes...")
	urllib.urlretrieve("http://zlib.net/zlib-1.2.5.tar.gz", os.path.join(thirdPartyFolder,"zlib-1.2.5.tar.gz"))

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

# check if we need to extract ilmbase
if not os.path.exists(os.path.join(thirdPartyFolder,"ilmbase-1.0.2")):
	tar = tarfile.open(os.path.join(thirdPartyFolder,"ilmbase-1.0.2.tar.gz"),'r:gz')
	print("Extracting ilmbase, this may take a couple of minutes...")
	tar.extractall(thirdPartyFolder)

# check if we need to extract openexr
if not os.path.exists(os.path.join(thirdPartyFolder,"openexr-1.6.1")):
	tar = tarfile.open(os.path.join(thirdPartyFolder,"openexr-1.6.1.tar.gz"),'r:gz')
	print("Extracting openexr, this may take a couple of minutes...")
	tar.extractall(thirdPartyFolder)

# check if we need to extract zlib
if not os.path.exists(os.path.join(thirdPartyFolder,"zlib-1.2.5")):
	tar = tarfile.open(os.path.join(thirdPartyFolder,"zlib-1.2.5.tar.gz"),'r:gz')
	print("Extracting zlib, this may take a couple of minutes...")
	tar.extractall(thirdPartyFolder)


