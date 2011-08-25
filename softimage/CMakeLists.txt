cmake_minimum_required (VERSION 2.6) 

project (softimage-alembic) 

file(GLOB Sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB Includes ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

SOURCE_GROUP("Source Files" FILES ${Sources})
SOURCE_GROUP("Header Files" FILES ${Includes})

add_library( ${PROJECT_NAME} SHARED ${Sources} ${Includes})