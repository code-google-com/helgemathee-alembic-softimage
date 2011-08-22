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
	set configFolder=Debug
	shift
) ELSE (
	set DB=
	set config=Release
	set configFolder=RelWithDebInfo
)

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set SYS=i64
	set arch=x64
) ELSE (
	set SYS=x86
	set arch=Win32
)

set srcRoot=%ALEMBIC_ROOT%\thirdparty\ilmbase-1.0.2
set srcDir=%srcRoot%\vc\vc9\IlmBase
set outDir=%ALEMBIC_OUT%\%SYS%\ilmbase
set outLib=%outDir%\lib%DB%
set IncDir=%outDir%\include

if not EXIST "%ALEMBIC_ROOT%\thirdparty\ilmbase-1.0.2\vc\vc9" (
	xcopy %ALEMBIC_ROOT%\thirdparty\ilmbase-1.0.2\vc\vc8 %ALEMBIC_ROOT%\thirdparty\ilmbase-1.0.2\vc\vc9 /S /I
	vcbuild /nologo %ALEMBIC_ROOT%\thirdparty\ilmbase-1.0.2\vc\vc9\IlmBase\Half\Half.vcproj /upgrade
	vcbuild /nologo %ALEMBIC_ROOT%\thirdparty\ilmbase-1.0.2\vc\vc9\IlmBase\Iex\Iex.vcproj /upgrade
	vcbuild /nologo %ALEMBIC_ROOT%\thirdparty\ilmbase-1.0.2\vc\vc9\IlmBase\IlmThread\IlmThread.vcproj /upgrade
	vcbuild /nologo %ALEMBIC_ROOT%\thirdparty\ilmbase-1.0.2\vc\vc9\IlmBase\Imath\Imath.vcproj /upgrade
)

python "%ALEMBIC_ROOT%\convert_ilmbase_vcprojs.py"

if NOT exist "%outLib%"		md %outLib%
if NOT exist "%IncDir%" md %IncDir%

@echo on
vcbuild /nologo %1 %2 %3 %4 %5 %6 %srcDir%\Half\Half.vcproj "%config%|%arch%"
vcbuild /nologo %1 %2 %3 %4 %5 %6 %srcDir%\Iex\Iex.vcproj "%config%|%arch%"
vcbuild /nologo %1 %2 %3 %4 %5 %6 %srcDir%\IlmThread\IlmThread.vcproj "%config%|%arch%"
vcbuild /nologo %1 %2 %3 %4 %5 %6 %srcDir%\Imath\Imath.vcproj "%config%|%arch%"

@echo off
copy %srcDir%\%arch%\%config%\*.lib %outLib%
copy %srcRoot%\Half\half*.h 	%IncDir%
copy %srcRoot%\Iex\*.h 		%IncDir%
copy %srcRoot%\IMath\*.h 	%IncDir%
copy %srcRoot%\IlmThread\*.h 	%IncDir%
