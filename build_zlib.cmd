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
	set cmake_flags=-D CMAKE_C_FLAGS_DEBUG="/D_DEBUG /MDd /Zi  /Ob0 /Od /RTC1" -D BUILD_SHARED_LIBS=OFF
	set targetLibDir=%ALEMBIC_ROOT%\lib64\Debug
	shift
) ELSE (
	set DB=
	set config=RelWithDebInfo
	set cmake_flags=-D CMAKE_C_FLAGS_RELWITHDEBINFO="/MD /Zi /O2 /Ob1 /D NDEBUG" -D BUILD_SHARED_LIBS=OFF
	set targetLibDir=%ALEMBIC_ROOT%\lib64\Release
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

set outDir=%ALEMBIC_OUT%\%SYS%\zlib
if NOT exist "%outDir%" md %outDir%
if exist "%ALEMBIC_ROOT%\thirdparty\zlib-1.2.5\zconf.h" del "%ALEMBIC_ROOT%\thirdparty\zlib-1.2.5\zconf.h"

@echo on
pushd %outDir%
cmake -G %Generator% %cmake_flags% %ALEMBIC_ROOT%\thirdparty\zlib-1.2.5
copy %ALEMBIC_OUT%\%SYS%\zlib\zconf.h %ALEMBIC_ROOT%\thirdparty\zlib-1.2.5
vcbuild /nologo %1 %2 %3 %4 %5 %6 %ALEMBIC_OUT%\%SYS%\zlib\zlib.vcproj "%config%|%arch%"

if not exist %targetLibDir% md %targetLibDir%
copy %outDir%\%config%\*.lib %targetLibDir% /y
copy %outDir%\%config%\*.pdb %targetLibDir% /y
copy %outDir%\%config%\*.dll %targetLibDir% /y

@popd