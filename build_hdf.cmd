@echo off
setlocal

if NOT "%ALEMBIC_ROOT%" == "" if NOT "%ALEMBIC_OUT%" == "" goto AlembicDefined
echo ALEMBIC_ROOT and ALEMBIC_OUT must be defined!!!
goto :eof
:AlembicDefined

python "%ALEMBIC_ROOT%\prefetch_thirdparty_libs.py"

if /i "%1" == "db:" (
	set DB=_db
	set config=Debug
	shift
) ELSE (
	set DB=
	set config=RelWithDebInfo
)

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set SYS=i64
	set arch=x64
	set Generator="Visual Studio 9 2008 Win64"
) ELSE (
	set SYS=x86
	set arch=Win32
	set Generator="Visual Studio 9 2008"
)

set outDir=%ALEMBIC_OUT%\%SYS%\hdf5
if NOT exist "%outDir%" md %outDir%
if NOT exist "%outDir%\include" (
	md %outDir%\include
	xcopy %ALEMBIC_ROOT%\thirdparty\hdf5-1.8.7\c++\src\*.h %outDir%\include
	xcopy %ALEMBIC_ROOT%\thirdparty\hdf5-1.8.7\hl\src\*.h %outDir%\include
	xcopy %ALEMBIC_ROOT%\thirdparty\hdf5-1.8.7\windows\src\*.h %outDir%\include
	xcopy %ALEMBIC_ROOT%\thirdparty\hdf5-1.8.7\src\*.h %outDir%\include
)

set cmake_flags=-D CMAKE_C_FLAGS=" /MT " -D CMAKE_CXX_FLAGS=" /MT "

@echo on
pushd %outDir%
cmake -G %Generator% -D HDF5_BUILD_HL_LIB:BOOL=ON %cmake_flags% %ALEMBIC_ROOT%\thirdparty\hdf5-1.8.7
vcbuild /nologo %1 %2 %3 %4 %5 %6 HDF5.sln "%config%|%arch%"
@popd