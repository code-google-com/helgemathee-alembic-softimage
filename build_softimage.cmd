@echo off
setlocal

if NOT "%ALEMBIC_ROOT%" == "" if NOT "%ALEMBIC_OUT%" == "" goto AlembicDefined
echo ALEMBIC_ROOT and ALEMBIC_OUT must be defined!!!
goto :eof
:AlembicDefined

if /i "%1" == "db:" (
	set DB=_db
	set config=Debug
	set cmake_flags=-D CMAKE_CXX_FLAGS_DEBUG="/D_DEBUG /MTd /Zi  /Ob0 /Od /RTC1 /EHsc" -D BUILD_SHARED_LIBS=ON
	set zlib_suffix=d
	set boost_suffix=sgd
	shift
) ELSE (
	set DB=
	set config=Release
	set cmake_flags=-D CMAKE_CXX_FLAGS_RELEASE="/MT /Zi /O2 /Ob1 /D NDEBUG /EHsc" -D BUILD_SHARED_LIBS=ON
	set zlib_suffix=
	set boost_suffix=s
)

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set SYS=i64
	set arch=x64
	set xsiArch=nt-x86-64
	set Generator="Visual Studio 9 2008 Win64"
) ELSE (
	set SYS=x86
	set arch=Win32
	set xsiArch=nt-x86
	set Generator="Visual Studio 9 2008"
)

set srcRoot=%ALEMBIC_ROOT%\softimage
set outDir=%ALEMBIC_OUT%\%SYS%\softimage\alembic\Application\Plugins
if NOT exist "%outDir%" md %outDir%

@echo on
pushd %outDir%
cmake -G %Generator% %cmake_flags% %srcRoot%

vcbuild /nologo %1 %2 %3 %4 %5 %6 softimage-alembic.sln "%config%|%arch%"

@popd