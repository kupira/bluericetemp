cmake_minimum_required(VERSION 3.16)
project(Bluerice LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-3.0 REQUIRED)
pkg_check_modules(GIOMM giomm-2.68 REQUIRED)

include_directories(${GTKMM_INCLUDE_DIRS} ${GIOMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS} ${GIOMM_LIBRARY_DIRS})
add_definitions(${GTKMM_CFLAGS_OTHER} ${GIOMM_CFLAGS_OTHER})

file(GLOB SRC_FILES src/*.cpp)

add_executable(bluerice ${SRC_FILES})

target_link_libraries(bluerice ${GTKMM_LIBRARIES} ${GIOMM_LIBRARIES})

install(TARGETS bluerice RUNTIME DESTINATION bin)

