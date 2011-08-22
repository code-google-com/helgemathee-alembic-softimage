import os

# check environment variables
if not os.environ.has_key("ALEMBIC_ROOT") or not os.environ.has_key("ALEMBIC_OUT"):
	raise(Exception("ALEMBIC_ROOT and ALEMBIC_OUT has to be set!"))
if not os.environ.has_key("outLib"):
	raise(Exception("outLib has to be set!"))

# create the thirdparty folder
thirdPartyFolder = os.path.join(os.environ["ALEMBIC_ROOT"],"thirdparty")

vcprojs = [
	"Half",
	"Iex",
	"IlmThread",
	"Imath"
]

versions = ["vc7", "vc8", "vc9"]
for i in range(0,len(vcprojs)):
	for j in range(0,3):
	
		vcproj = os.path.join(thirdPartyFolder,"ilmbase-1.0.2","vc",versions[j],"IlmBase",vcprojs[i],vcprojs[i]+".vcproj")
		content = open(vcproj).read()
		content = content.replace("OPENEXR;","")
		content = content.replace("OPENEXR","")
		content = content.replace("ConfigurationType=\"2\"","ConfigurationType=\"4\"")
		content = content.replace("RuntimeLibrary=\"2\"","RuntimeLibrary=\"0\"")
		content = content.replace("RuntimeLibrary=\"3\"","RuntimeLibrary=\"1\"")
		content = content.replace("VCPostBuildEventTool","DisabledTool")
		content = content.replace("OutputDirectory=\"..\$(PlatformName)\$(ConfigurationName)\"","OutputDirectory=\""+os.environ["outLib"]+"\"")
	
		open(vcproj,'w').write(content)
		print("-------------> Converted "+vcproj+".")
