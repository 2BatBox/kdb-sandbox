cmake_minimum_required(VERSION 3.10)
project(kdb_sandbox VERSION 0.0.1 DESCRIPTION "KDB sandbox")

set(CMAKE_CXX_STANDARD 17)

############################################################
# KDB
############################################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(KDB REQUIRED)
find_package(Threads REQUIRED)
include_directories(${KDB_INCLUDE_DIR})

############################################################
# Common settings.
############################################################
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fomit-frame-pointer -fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
set(APP_COMMON_LIBS KDB::kdb)

############################################################
# Sandbox application.
############################################################
set(APP_SANDBOX "kdb_sandbox")
add_executable(${APP_SANDBOX} src/main.cpp)
target_link_libraries(${APP_SANDBOX} ${APP_COMMON_LIBS})
