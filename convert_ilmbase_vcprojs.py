import os
import glob
import shutil

# check environment variables
if not os.environ.has_key("ALEMBIC_ROOT") or not os.environ.has_key("ALEMBIC_OUT"):
	raise(Exception("ALEMBIC_ROOT and ALEMBIC_OUT has to be set!"))

# create the thirdparty folder
thirdPartyFolder = os.path.join(os.environ["ALEMBIC_ROOT"],"thirdparty")

projects = [
	"Half",
	"Iex",
	"IlmThread",
	"Imath"
]

# construct a new, joined project
projectPath = os.path.join(thirdPartyFolder,'ilmbase-1.0.2','joined')
if not os.path.exists(projectPath):
	os.makedirs(projectPath)

	# has for which project to use
	projectsUsed = {}
	for i in range(0,len(projects)):
		projectsUsed[projects[i]] = 1

	# find all headers files
	folders = [ os.path.join(thirdPartyFolder,'ilmbase-1.0.2') ]
	headers = []
	sources = []
	folderIndex = 0
	while folderIndex < len(folders):
		files = glob.glob(os.path.join(folders[folderIndex],'*'))
		for file in files:
			if(os.path.isdir(file)):
				folders.append(file)
				continue
			suffix = file.rpartition('.')[2].lower()
			if suffix == 'h':
				headers.append(file)
				continue
			if suffix == 'cpp':
				lastFolder = os.path.split(folders[folderIndex])[1]
				if projectsUsed.has_key(lastFolder):
					sources.append(file)
		folderIndex += 1

	# copy all headers
	for i in range(0,len(headers)):
		filename = os.path.split(headers[i])[1]
		shutil.copyfile(headers[i],os.path.join(projectPath,filename))
		print("Copied header "+filename+" ...")
	# copy all sources
	for i in range(0,len(sources)):
		filename = os.path.split(sources[i])[1]
		shutil.copyfile(sources[i],os.path.join(projectPath,filename))
		print("Copied source "+filename+" ...")
	# copy the cmake file
	shutil.copyfile(os.path.join(os.environ["ALEMBIC_ROOT"],"CMakeLists_ilmbase.txt"),os.path.join(projectPath,"CMakeLists.txt"))

