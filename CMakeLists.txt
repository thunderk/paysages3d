cmake_minimum_required(VERSION 2.8)
project(paysages)

include(FindPkgConfig)

find_package(DevIL)

pkg_check_modules(GTK3 gtk+-3.0)

include_directories(${IL_INCLUDE_DIR})
include_directories(${GTK3_INCLUDE_DIRS})

aux_source_directory(src SOURCE_FILES)
aux_source_directory(src/gui SOURCE_FILES)

#set(CMAKE_C_FLAGS "${GTK3_CFLAGS_OTHER}")
set(CMAKE_C_FLAGS_PROFILE "-pg -O0")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE debug)
endif(NOT CMAKE_BUILD_TYPE)

add_executable(paysages ${SOURCE_FILES})
target_link_libraries(paysages ${IL_LIBRARIES} ${ILU_LIBRARIES} ${GTK3_LIBRARIES})

