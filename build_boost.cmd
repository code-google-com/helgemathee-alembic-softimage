@echo off
setlocal

if NOT "%ALEMBIC_ROOT%" == "" if NOT "%ALEMBIC_OUT%" == "" goto AlembicDefined
echo ALEMBIC_ROOT and ALEMBIC_OUT must be defined!!!
goto :eof
:AlembicDefined

python "%ALEMBIC_ROOT%\prefetch_thirdparty_libs.py"

if /i "%1" == "db:" (
	set DB=_db
	shift
	set VARIANT=variant=debug
) ELSE (
	set VARIANT=variant=release
	set DB=
)

set BOOST_SRC=%ALEMBIC_ROOT%\thirdparty\boost_1_42_0

pushd %BOOST_SRC%

if "%PROCESSOR_ARCHITECTURE%" == "AMD64" (
	set SYS=i64
	set addrModel=address-model=64
) ELSE (
	set SYS=x86
)

if /i NOT "%1" == "noboot" (
	echo bootstrap
	call bootstrap
) ELSE (
	shift
)

set rootOut=%ALEMBIC_OUT%
set targetOut=%rootOut%\%SYS%\boost

if NOT "%icl%" == "" set altCompiler=toolset=intel

set BOOST_INC=%targetOut%\include
set BOOST_LIB=%targetOut%\lib%db%
set BOOST_BUILD=%targetOut%\build
set BOOST_OUT=--includedir=%BOOST_INC% --libdir=%BOOST_LIB% --build-dir=%BOOST_BUILD%
set BOOST_ARGS=--layout=versioned link=static runtime-link=static threading=multi %addrModel% %altCompiler%
set BUILD_THESE_ONLY= --with-program_options --with-iostreams --with-date_time --with-thread

@echo on
call bjam %1 %2 %3 %4 %5 install %VARIANT% %BOOST_ARGS% %BOOST_OUT% %BUILD_THESE_ONLY%
@popd
