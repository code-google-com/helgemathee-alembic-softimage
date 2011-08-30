cmake_minimum_required (VERSION 2.6) 

project (softimage-alembic) 

function(setup_softimage)
	if( WIN32 )
		if( CMAKE_SIZEOF_VOID_P MATCHES "8" )
			SET( SOFTIMAGE_OUTPUT_POSTFIX "nt-x86-64" )
		endif( CMAKE_SIZEOF_VOID_P MATCHES "8" )

		if( CMAKE_SIZEOF_VOID_P MATCHES "4" )
			SET( SOFTIMAGE_OUTPUT_POSTFIX "nt-x86" )
		endif( CMAKE_SIZEOF_VOID_P MATCHES "4" )
	endif( WIN32 )

	if( UNIX )
		if( CMAKE_SIZEOF_VOID_P MATCHES "8" )
			SET( SOFTIMAGE_OUTPUT_POSTFIX "linux-x86-64" )
		endif( CMAKE_SIZEOF_VOID_P MATCHES "8" )
	endif( UNIX )
	
	
	SET( Softimage_INCLUDE_DIR ${Softimage_BASE_DIR}/include PARENT_SCOPE )
	
	SET( Softimage_LIBRARY_DIR ${Softimage_BASE_DIR}/lib/${SOFTIMAGE_OUTPUT_POSTFIX} )
	SET( Softimage_LIBRARIES "${Softimage_LIBRARY_DIR}/sicoresdk.lib" "${Softimage_LIBRARY_DIR}/sicppsdk.lib" PARENT_SCOPE )		
endfunction(setup_softimage)



file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB Includes ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

SOURCE_GROUP("Source Files" FILES ${Sources})
SOURCE_GROUP("Header Files" FILES ${Includes})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
#set(Boost_DEBUG ON )
#set(Boost_DETAILED_FAILURE_MSG ON )
set(Boost_USE_STATIC_RUNTIME ON)
SET( Boost_NO_SYSTEM_PATHS true )

set( BOOST_INCLUDEDIR "$ENV{ALEMBIC_ROOT}/thirdparty/boost_1_42_0" )
set( BOOST_LIBRARYDIR "$ENV{ALEMBIC_ROOT}/lib64/$ENV{CONFIG}" )

MESSAGE( $ENV{ALEMBIC_ROOT}/lib64/$ENV{CONFIG} )

find_package( Boost 1.42.0 COMPONENTS date_time program_options thread iostreams REQUIRED )
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

SET( Softimage_BASE_DIR $ENV{XSISDK_ROOT} )

setup_softimage()
include_directories( ${Softimage_INCLUDE_DIR} )

 if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG_INIT "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
	set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT     "/MT /O1 /Ob1 /D NDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE_INIT        "/MT /O2 /Ob2 /D NDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "/MT /Zi /O2 /Ob1 /D NDEBUG")
endif()

#include_directories( "$ENV{ALEMBIC_ROOT}/thirdparty/boost_1_42_0" )
include_directories( "$ENV{ALEMBIC_ROOT}/thirdparty/ilmbase-1.0.2/joined" )
include_directories( "$ENV{ALEMBIC_ROOT}/lib" )
include_directories( "$ENV{XSISDK_ROOT}/include" )

link_directories( "$ENV{ALEMBIC_ROOT}/lib64/$ENV{CONFIG}" )
link_directories( "$ENV{XSISDK_ROOT}/lib/$ENV{xsiArch}" )
MESSAGE ( "$ENV{XSISDK_ROOT}/lib/$ENV{xsiArch}" )

add_library( ${PROJECT_NAME} SHARED ${Sources} ${Includes})

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} joined.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} zlib$ENV{zlib_suffix}.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} hdf5$ENV{zlib_suffix}.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} hdf5_hl$ENV{zlib_suffix}.lib )
#TARGET_LINK_LIBRARIES( ${PROJECT_NAME} libboost_program_options-vc90-mt-$ENV{boost_suffix}.lib )
#TARGET_LINK_LIBRARIES( ${PROJECT_NAME} libboost_iostreams-vc90-mt-$ENV{boost_suffix}.lib )
#TARGET_LINK_LIBRARIES( ${PROJECT_NAME} libboost_thread-vc90-mt-$ENV{boost_suffix}.lib )
#TARGET_LINK_LIBRARIES( ${PROJECT_NAME} libboost_date_time-vc90-mt-$ENV{boost_suffix}.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicUtil.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicMD5Hash.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicAbcCoreAbstract.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicAbcCoreHDF5.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicAbc.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicAbcGeom.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${Softimage_LIBRARIES} )