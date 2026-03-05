cmake_minimum_required(VERSION 3.20)
project(cpp-collaborate-platform LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow     CONFIG REQUIRED)
find_package(libpqxx  CONFIG REQUIRED)
find_package(Threads  REQUIRED)


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.cxx")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h"   "src/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE Crow::Crow libpqxx::pqxx Threads::Threads)

