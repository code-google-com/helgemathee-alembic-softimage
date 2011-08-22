@echo off

if NOT "%ALEMBIC_ROOT%" == "" if NOT "%ALEMBIC_OUT%" == "" goto AlembicDefined
echo ALEMBIC_ROOT and ALEMBIC_OUT must be defined!!!
goto :eof
:AlembicDefined

setlocal
if /i "%1" == "db:" (
REM ******************************************************************************************
REM let's assume we don't need to debug IlmBase, boost, or zlib and just use the non-debug
REM libs even when building debug. This way we don't have to nuke the projects to switch.
REM ******************************************************************************************
:	set DB=_db
	shift
) ELSE (
	set DB=
)

python "%ALEMBIC_ROOT%\convert_ilmbase_vcprojs.py"

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set SYS=i64
	set ARCH=x64
	set Generator="Visual Studio 9 2008 Win64"
) ELSE (
	set SYS=x86
	set ARCH=x86
	set Generator="Visual Studio 9 2008"
)

set rootOut=%ALEMBIC_OUT%
set platOut=%rootOut%\%SYS%
set BOOST_ROOT=%ALEMBIC_ROOT%\thirdparty\boost-1_42_0
set ILMBASE_ROOT=%platOut%\IlmBase
set OPENEXR_ROOT=%ALEMBIC_ROOT%\thirdparty\openexr-1.6.1
set HDF5_ROOT=%ALEMBIC_ROOT%\thirdparty\hdf5-1.8.7
set ZLIB_ROOT=%ALEMBIC_ROOT%\thirdparty\zlib-1.2.5


REM ******************************************************************************************
REM We always use the MT libraries, so comment out the next line if you want MTd libs for debug
REM ******************************************************************************************
set cmake_flags=/MT /U _DEBUG

REM ******************************************************************************************
REM Corrections to vars in bootstrap script
REM ******************************************************************************************
set warnFlags=/wd4267 /wd4800 /wd4018 /wd4244 %cmake_flags%
set ccflags=%warnFlags% /D_WINDOWS /W3 /Zm1000
set cppflags=%ccflags% /EHsc /GR

set BASE_ARGS=--disable-prman --disable-maya --generator=%Generator%
rem set BASE_ARGS=--disable-prman --with-maya=%MAYA_ROOT% --generator=%Generator%
set HDF_ARGS=--hdf5_include_dir=%platOut%\hdf5\include --hdf5_hdf5_library=%platOut%\hdf5\bin\RelWithDebInfo\hdf5_hl%db%.lib
set ILM_ARGS=--ilmbase_include_dir=%ILMBASE_ROOT%\include --ilmbase_imath_library=%ILMBASE_ROOT%\lib%db%\Imath.lib
set BOOST_ARGS=--boost_include_dir=%platOut%\boost\include\boost-1_42 --boost_program_options_library=%platOut%\boost\lib%db%\libboost_program_options-vc90-mt%db%-1_42.lib
set ZLIB_ARGS=--zlib_include_dir=%ZLIB_ROOT% --zlib_library=%platOut%\zlib\RelWithDebInfo\zlib%db%.lib

@echo on
pushd %platOut%
del CMakeCache.txt
python %ALEMBIC_ROOT%\build\bootstrap\alembic_bootstrap.py %BASE_ARGS% %HDF_ARGS% %ILM_ARGS% %BOOST_ARGS% %ZLIB_ARGS% --cflags="%ccflags%" --cxxflags="%cppflags%" %platOut% 
popd