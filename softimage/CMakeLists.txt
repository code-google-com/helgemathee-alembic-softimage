cmake_minimum_required (VERSION 2.6) 

project (softimage-alembic) 

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB Includes ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

SOURCE_GROUP("Source Files" FILES ${Sources})
SOURCE_GROUP("Header Files" FILES ${Includes})

include_directories( "$ENV{ALEMBIC_ROOT}/thirdparty/boost_1_42_0" )
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
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} libboost_program_options-vc90-mt-$ENV{boost_suffix}.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} libboost_iostreams-vc90-mt-$ENV{boost_suffix}.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} libboost_thread-vc90-mt-$ENV{boost_suffix}.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} libboost_date_time-vc90-mt-$ENV{boost_suffix}.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicUtil.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicMD5Hash.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicAbcCoreAbstract.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicAbcCoreHDF5.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicAbc.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} AlembicAbcGeom.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} sicoresdk.lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} sicppsdk.lib )