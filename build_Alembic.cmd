@echo off
setlocal

if NOT "%ALEMBIC_ROOT%" == "" if NOT "%ALEMBIC_OUT%" == "" goto AlembicDefined
echo ALEMBIC_ROOT and ALEMBIC_OUT must be defined!!!
goto :eof
:AlembicDefined

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
) ELSE (
	set SYS=x86
	set arch=Win32
)

set buildArg="%config%|%arch%"
set outDir=%ALEMBIC_OUT%\%SYS%

@echo on
vcbuild /nologo %1 %2 %3 %4 %5 %6 %outDir%\lib\Alembic\Util\AlembicUtil.vcproj %buildArg%
vcbuild /nologo %1 %2 %3 %4 %5 %6 %outDir%\lib\Alembic\Abc\AlembicAbc.vcproj %buildArg%
vcbuild /nologo %1 %2 %3 %4 %5 %6 %outDir%\lib\Alembic\AbcCoreAbstract\AlembicAbcCoreAbstract.vcproj %buildArg%
vcbuild /nologo %1 %2 %3 %4 %5 %6 %outDir%\lib\Alembic\AbcCoreHDF5\AlembicAbcCoreHDF5.vcproj %buildArg%
vcbuild /nologo %1 %2 %3 %4 %5 %6 %outDir%\lib\Alembic\MD5Hash\AlembicMD5Hash.vcproj %buildArg%
vcbuild /nologo %1 %2 %3 %4 %5 %6 %outDir%\lib\Alembic\AbcGeom\AlembicAbcGeom.vcproj %buildArg%
rem vcbuild /nologo %1 %2 %3 %4 %5 %6 %outDir%\Alembic.sln %buildArg%

@echo off
if not exist %outDir%\alembic\%config% md %outDir%\alembic\%config%
copy %outDir%\lib\Alembic\Util\%config%\*.lib %outDir%\alembic\%config% /y
copy %outDir%\lib\Alembic\Abc\%config%\*.lib %outDir%\alembic\%config% /y
copy %outDir%\lib\Alembic\AbcCoreAbstract\%config%\*.lib %outDir%\alembic\%config% /y
copy %outDir%\lib\Alembic\AbcCoreHDF5\%config%\*.lib %outDir%\alembic\%config% /y
copy %outDir%\lib\Alembic\MD5Hash\%config%\*.lib %outDir%\alembic\%config% /y
copy %outDir%\lib\Alembic\AbcGeom\%config%\*.lib %outDir%\alembic\%config% /y
copy %outDir%\lib\Alembic\Util\%config%\*.pdb %outDir%\alembic\%config% /y
copy %outDir%\lib\Alembic\Abc\%config%\*.pdb %outDir%\alembic\%config% /y
copy %outDir%\lib\Alembic\AbcCoreAbstract\%config%\*.pdb %outDir%\alembic\%config% /y
copy %outDir%\lib\Alembic\AbcCoreHDF5\%config%\*.pdb %outDir%\alembic\%config% /y
copy %outDir%\lib\Alembic\MD5Hash\%config%\*.pdb %outDir%\alembic\%config% /y
copy %outDir%\lib\Alembic\AbcGeom\%config%\*.pdb %outDir%\alembic\%config% /y

