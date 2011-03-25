@echo off
setlocal

if NOT "%ALEMBIC_ROOT%" == "" if NOT "%ALEMBIC_OUT%" == "" goto AlembicDefined
echo ALEMBIC_ROOT and ALEMBIC_OUT must be defined!!!
goto :eof
:AlembicDefined

if /i "%1" == "db:" (
	set DB=_db
	shift
) ELSE (
	set VARIANT=variant=release
	set DB=
)

set BOOST_SRC=%ALEMBIC_ROOT%\contrib\boost_1_42_0

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
set targetOut=%rootOut%\%SYS%\boost%db%

set BOOST_INC=%rootOut%\boost
set BOOST_LIB=%targetOut%\lib
set BOOST_BUILD=%targetOut%\build

@echo on
call bjam %1 %2 %3 %4 %5 install %VARIANT% --layout=versioned link=static threading=multi --prefix=%BOOST_INC% --libdir=%BOOST_LIB% --build-dir=%BOOST_BUILD% %addrModel%
@popd
