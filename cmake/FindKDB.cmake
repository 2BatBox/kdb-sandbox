if(KDB_FOUND)
    return()
endif()

if(DEFINED ENV{QHOME})
    set(QHOME $ENV{QHOME})
else()
    set(QHOME "$ENV{HOME}/kdb")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
        set(KDB_ARCH "l64arm")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(KDB_ARCH "l64")
    else()
        set(KDB_ARCH "l32")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(KDB_ARCH "w64")
    else()
        set(KDB_ARCH "w32")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(KDB_ARCH "m64")
    else()
        set(KDB_ARCH "m32")
    endif()
endif()

find_path(KDB_INCLUDE_DIR
        NAMES k.h
        PATHS ${QHOME}/c/c
        NO_DEFAULT_PATH
        DOC "KDB+ include directory"
)

find_file(KDB_LIBRARY
        NAMES c.o
        PATHS ${QHOME}/${KDB_ARCH}
        NO_DEFAULT_PATH
        DOC "KDB+ library object file"
)

message(STATUS "Detected architecture: ${KDB_ARCH}")
message(STATUS "Looking for k.h in: ${QHOME}/c/c")
message(STATUS "Looking for c.o in: ${QHOME}/${KDB_ARCH}")

if(KDB_INCLUDE_DIR)
    message(STATUS "Found k.h: ${KDB_INCLUDE_DIR}/k.h")
else()
    message(STATUS "k.h not found")
endif()

if(KDB_LIBRARY)
    message(STATUS "Found c.o: ${KDB_LIBRARY}")
else()
    message(STATUS "c.o not found")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KDB
        REQUIRED_VARS KDB_LIBRARY KDB_INCLUDE_DIR
        FAIL_MESSAGE "KDB+ not found. Architecture: ${KDB_ARCH}"
)

if(KDB_FOUND)
    if(NOT TARGET KDB::kdb)
        add_library(KDB::kdb STATIC IMPORTED)
        set_target_properties(KDB::kdb PROPERTIES
                IMPORTED_LOCATION ${KDB_LIBRARY}
                INTERFACE_INCLUDE_DIRECTORIES ${KDB_INCLUDE_DIR}
        )
    endif()

    set(KDB_LIBRARIES ${KDB_LIBRARY})
    set(KDB_INCLUDE_DIRS ${KDB_INCLUDE_DIR})
endif()

mark_as_advanced(KDB_INCLUDE_DIR KDB_LIBRARY)
