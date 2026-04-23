cmake_minimum_required(VERSION 3.20)
project(cpp-collaborate-platform LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_UNITY_BUILD "Enable CMake unity/jumbo build for faster compilation" ON)
set(UNITY_BUILD_BATCH_SIZE "16" CACHE STRING "Unity build batch size")

find_package(Crow     CONFIG REQUIRED)
find_package(libpqxx  CONFIG REQUIRED)
find_package(jwt-cpp  CONFIG REQUIRED)
find_package(unofficial-argon2 CONFIG REQUIRED)
find_package(spdlog   CONFIG REQUIRED)
find_package(Threads  REQUIRED)

# Dotenv setup
find_path(DOTENV_INCLUDE_DIR "dotenv/dotenv.hpp"
    HINTS "${CMAKE_SOURCE_DIR}/third_party"
)

# ODB 2.5 runtime setup (Docker/Windows)
if(NOT ODB_ROOT)
    if(DEFINED ENV{ODB_ROOT})
        set(ODB_ROOT "$ENV{ODB_ROOT}")
    elseif(WIN32)
        set(ODB_ROOT "${CMAKE_SOURCE_DIR}/.deps/odb-2.5/libodb-2.5.0-x86_64-windows10-msvc17.10")
    else()
        set(ODB_ROOT "/opt/odb-2.5")
    endif()
endif()

if(NOT ODB_PGSQL_ROOT)
    if(DEFINED ENV{ODB_PGSQL_ROOT})
        set(ODB_PGSQL_ROOT "$ENV{ODB_PGSQL_ROOT}")
    elseif(WIN32)
        set(ODB_PGSQL_ROOT "${CMAKE_SOURCE_DIR}/.deps/odb-2.5/libodb-pgsql-2.5.0-x86_64-windows10-msvc17.10")
    else()
        set(ODB_PGSQL_ROOT "/opt/odb-2.5")
    endif()
endif()

if(NOT ODB_BOOST_ROOT)
    if(DEFINED ENV{ODB_BOOST_ROOT})
        set(ODB_BOOST_ROOT "$ENV{ODB_BOOST_ROOT}")
    elseif(WIN32)
        set(ODB_BOOST_ROOT "${CMAKE_SOURCE_DIR}/.deps/odb-2.5/libodb-boost-2.5.0-x86_64-windows10-msvc17.10")
    else()
        set(ODB_BOOST_ROOT "/opt/odb-2.5")
    endif()
endif()

set(ODB_ROOT       "${ODB_ROOT}"       CACHE PATH "ODB 2.5 core runtime root")
set(ODB_PGSQL_ROOT "${ODB_PGSQL_ROOT}" CACHE PATH "ODB 2.5 PostgreSQL runtime root")
set(ODB_BOOST_ROOT "${ODB_BOOST_ROOT}" CACHE PATH "ODB 2.5 Boost runtime root")

set(ODB_CORE_TARGET odb::core)
set(ODB_PGSQL_TARGET odb::pgsql)
set(ODB_BOOST_TARGET)
set(ODB_LOCAL_INCLUDE_DIRS)

if(EXISTS "${ODB_ROOT}/include/odb/core.hxx" AND EXISTS "${ODB_PGSQL_ROOT}/include/odb/pgsql/database.hxx")
    find_path(ODB_INCLUDE_DIR       "odb/core.hxx"            HINTS "${ODB_ROOT}/include"       NO_DEFAULT_PATH REQUIRED)
    find_path(ODB_PGSQL_INCLUDE_DIR "odb/pgsql/database.hxx"  HINTS "${ODB_PGSQL_ROOT}/include" NO_DEFAULT_PATH REQUIRED)
    if(EXISTS "${ODB_BOOST_ROOT}/include/odb/boost/version.hxx")
        find_path(ODB_BOOST_INCLUDE_DIR "odb/boost/version.hxx" HINTS "${ODB_BOOST_ROOT}/include" NO_DEFAULT_PATH REQUIRED)
    endif()

    add_library(odb::core  UNKNOWN IMPORTED)
    add_library(odb::pgsql UNKNOWN IMPORTED)
    if(ODB_BOOST_INCLUDE_DIR)
        add_library(odb::boost UNKNOWN IMPORTED)
        set(ODB_BOOST_TARGET odb::boost)
    endif()

    if(WIN32)
        set_target_properties(odb::core  PROPERTIES IMPORTED_LOCATION "${ODB_ROOT}/lib/odb.lib")
        set_target_properties(odb::pgsql PROPERTIES IMPORTED_LOCATION "${ODB_PGSQL_ROOT}/lib/odb-pgsql.lib")
        if(ODB_BOOST_TARGET)
            set_target_properties(odb::boost PROPERTIES IMPORTED_LOCATION "${ODB_BOOST_ROOT}/lib/odb-boost.lib")
        endif()

        set(_odb_pgsql_extra_libs
            "${ODB_PGSQL_ROOT}/lib/pq.lib"
            "${ODB_PGSQL_ROOT}/lib/ssl.lib"
            "${ODB_PGSQL_ROOT}/lib/crypto.lib"
            "${ODB_PGSQL_ROOT}/lib/z.lib"
            "${ODB_PGSQL_ROOT}/lib/zstd.lib"
            secur32 crypt32 ws2_32
        )
        set_target_properties(odb::pgsql PROPERTIES
            INTERFACE_LINK_LIBRARIES "${_odb_pgsql_extra_libs}"
        )
    else()
        find_library(_odb_lib       NAMES odb       HINTS "${ODB_ROOT}/lib"       NO_DEFAULT_PATH REQUIRED)
        find_library(_odb_pgsql_lib NAMES odb-pgsql HINTS "${ODB_PGSQL_ROOT}/lib" NO_DEFAULT_PATH REQUIRED)
        if(ODB_BOOST_TARGET)
            find_library(_odb_boost_lib NAMES odb-boost HINTS "${ODB_BOOST_ROOT}/lib" NO_DEFAULT_PATH REQUIRED)
        endif()
        set_target_properties(odb::core  PROPERTIES IMPORTED_LOCATION "${_odb_lib}")
        set_target_properties(odb::pgsql PROPERTIES IMPORTED_LOCATION "${_odb_pgsql_lib}")
        if(ODB_BOOST_TARGET)
            set_target_properties(odb::boost PROPERTIES IMPORTED_LOCATION "${_odb_boost_lib}")
        endif()
    endif()

    set_target_properties(odb::core PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ODB_INCLUDE_DIR}"
    )
    set_target_properties(odb::pgsql PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ODB_PGSQL_INCLUDE_DIR}"
    )
    if(ODB_BOOST_TARGET)
        set_target_properties(odb::boost PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${ODB_BOOST_INCLUDE_DIR}"
        )
    endif()

    set(ODB_LOCAL_INCLUDE_DIRS
        "${ODB_INCLUDE_DIR}"
        "${ODB_PGSQL_INCLUDE_DIR}"
    )
    if(ODB_BOOST_INCLUDE_DIR)
        list(APPEND ODB_LOCAL_INCLUDE_DIRS "${ODB_BOOST_INCLUDE_DIR}")
    endif()
else()
    message(STATUS "ODB 2.5 runtime roots not found under .deps; falling back to vcpkg ODB packages.")
    find_package(odb CONFIG REQUIRED COMPONENTS libodb pgsql)
    set(ODB_CORE_TARGET odb::libodb)
    set(ODB_PGSQL_TARGET odb::libodb-pgsql)
    if(TARGET odb::libodb-boost)
        set(ODB_BOOST_TARGET odb::libodb-boost)
    endif()
endif()

# Source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.cxx")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h"   "src/*.hpp")

# Keep Unity build enabled globally but exclude files with duplicate local helpers.
set_source_files_properties(
    "${CMAKE_SOURCE_DIR}/src/modules/tasks/adapters/TaskRouteHandler.cpp"
    "${CMAKE_SOURCE_DIR}/src/modules/users/adapters/UserRouteHandler.cpp"
    PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${ODB_LOCAL_INCLUDE_DIRS}
    src
    "${DOTENV_INCLUDE_DIR}"
)

# Definition setup for ODB and project source directory
target_compile_definitions(${PROJECT_NAME} PRIVATE
    _HAS_AUTO_PTR=1
    _HAS_AUTO_PTR_ETC=1
    _HAS_DEPRECATED_FEATURES=1
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0A00)
endif()

if(MSVC)
    # /MP speeds up compilation by compiling multiple sources per cl.exe invocation.
    target_compile_options(${PROJECT_NAME} PRIVATE /MP)
    # ODB headers intentionally use #pragma db; silence MSVC unknown pragma warning.
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4068)
endif()

if(ENABLE_UNITY_BUILD)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        UNITY_BUILD ON
        UNITY_BUILD_BATCH_SIZE ${UNITY_BUILD_BATCH_SIZE}
    )
endif()

# Env file copy to output build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/.env
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/.env
)

# ODB runtime DLL copy for Windows (if using .deps runtimes)
if(WIN32 AND EXISTS "${ODB_ROOT}/bin/odb-2.5.dll" AND EXISTS "${ODB_PGSQL_ROOT}/bin/odb-pgsql-2.5.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ODB_ROOT}/bin/odb-2.5.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ODB_PGSQL_ROOT}/bin/odb-pgsql-2.5.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif()

# Libraries linking
target_link_libraries(${PROJECT_NAME} PRIVATE
    Crow::Crow
    libpqxx::pqxx
    Threads::Threads
    ${ODB_CORE_TARGET}
    ${ODB_PGSQL_TARGET}
    ${ODB_BOOST_TARGET}
    jwt-cpp::jwt-cpp
    unofficial::argon2::libargon2
    spdlog::spdlog
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /bigobj)
endif()

