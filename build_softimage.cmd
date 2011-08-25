@echo off
setlocal

if NOT "%ALEMBIC_ROOT%" == "" if NOT "%ALEMBIC_OUT%" == "" goto AlembicDefined
echo ALEMBIC_ROOT and ALEMBIC_OUT must be defined!!!
goto :eof
:AlembicDefined

python "%ALEMBIC_ROOT%\prefetch_thirdparty_libs.py"
python "%ALEMBIC_ROOT%\convert_ilmbase_vcprojs.py"

if /i "%1" == "db:" (
	set DB=_db
	set config=Debug
	set cmake_flags=-D CMAKE_CXX_FLAGS_DEBUG="/D_DEBUG /MTd /Zi  /Ob0 /Od /RTC1" -D BUILD_SHARED_LIBS=OFF
	set targetLibDir=%ALEMBIC_ROOT%\lib64\Debug
	shift
) ELSE (
	set DB=
	set config=RelWithDebInfo
	set cmake_flags=-D CMAKE_CXX_FLAGS_RELWITHDEBINFO="/MT /Zi /O2 /Ob1 /D NDEBUG" -D BUILD_SHARED_LIBS=OFF
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

set srcRoot=%ALEMBIC_ROOT%\thirdparty\ilmbase-1.0.2\joined
set outDir=%ALEMBIC_OUT%\%SYS%\ilmbase\lib%DB%
set incDir=%ALEMBIC_OUT%\%SYS%\ilmbase\include
if NOT exist "%outDir%" md %outDir%
if NOT exist "%incDir%" md %incDir%

copy %srcRoot%\*.h %incDir% /y

@echo on
pushd %outDir%
cmake -G %Generator% %cmake_flags% %srcRoot%

vcbuild /nologo %1 %2 %3 %4 %5 %6 toFloat.vcproj "%config%|%arch%"
@%outDir%\%config%\toFloat.exe > %srcRoot%\toFloat.h
@copy %srcRoot%\toFloat.h %incDir% /y
@echo Generated toFloat.h include file.

vcbuild /nologo %1 %2 %3 %4 %5 %6 eLut.vcproj "%config%|%arch%"
@%outDir%\%config%\eLut.exe > %srcRoot%\eLut.h
@copy %srcRoot%\eLut.h %incDir% /y
@echo Generated eLut.h include file.

vcbuild /nologo %1 %2 %3 %4 %5 %6 joined.vcproj "%config%|%arch%"

if not exist %targetLibDir% md %targetLibDir%
copy %outDir%\%config%\*.lib %targetLibDir% /y
copy %outDir%\%config%\*.pdb %targetLibDir% /y
copy %outDir%\%config%\*.dll %targetLibDir% /y

@popd