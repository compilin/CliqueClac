cmake_minimum_required(VERSION 3.3)
project(CliqueClac)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra -pedantic")

include_directories(fm-dm/include)


set(SOURCE_FILES src/main.cpp)

add_library(enumerate src/bronkerbosch.cpp src/bronkerbosch.h src/enumeratecliques.cpp src/enumeratecliques.h)
add_library(fmdm src/fmdm/dm.c src/fmdm/dm_english.h src/fmdm/dm.hpp)
add_library(parsegml src/fmdm/parsegml.cpp src/fmdm/parsegml.h)


add_executable(fmdm_random src/fmdm/random.c)
add_executable(fmdm_fromgml src/fmdm/fromgml.cpp)
add_executable(CliqueClac ${SOURCE_FILES})

target_link_libraries(parsegml fmdm expat)

target_link_libraries(fmdm_random fmdm)
target_link_libraries(fmdm_fromgml parsegml)
target_link_libraries(CliqueClac fmdm enumerate parsegml)