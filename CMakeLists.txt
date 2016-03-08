cmake_minimum_required(VERSION 3.3)
project(CliqueClac)

add_definitions(-DRETURN_CLIQUES_ONE_BY_ONE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra -pedantic")

include_directories(fm-dm/include)
find_package(Boost 1.60.0 REQUIRED system)

include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE_FILES src/main.cpp)

add_library(enumerate STATIC src/enumeratecliques.cpp src/enumeratecliques.hpp)
add_library(fmdm STATIC src/fmdm/dm.c src/fmdm/dm_english.h src/fmdm/dm_types.h)
add_library(parsegml STATIC src/parsegml.cpp src/parsegml.hpp)
add_library(writegraph STATIC src/writegraph.cpp src/writegraph.hpp)
add_library(degeneracy STATIC
		src/quick-cliques/degeneracy_algorithm.c    src/quick-cliques/degeneracy_algorithm.h
		src/quick-cliques/degeneracy_helper.c       src/quick-cliques/degeneracy_helper.h)
add_library(misc STATIC
		src/quick-cliques/misc.c                    src/quick-cliques/misc.h)
add_library(linkedlist STATIC
		src/quick-cliques/LinkedList.c src/quick-cliques/LinkedList.h)
add_library(memorymanager STATIC
		src/quick-cliques/MemoryManager.c           src/quick-cliques/MemoryManager.h)
add_library(moddecomp src/moddecomp.cpp src/moddecomp.hpp)

add_executable(fmdm_random src/fmdm/random.c)
add_executable(fmdm_fromgml src/fromgml.cpp)
add_executable(CliqueClac ${SOURCE_FILES})

target_link_libraries(misc linkedlist)
target_link_libraries(degeneracy memorymanager linkedlist misc)
target_link_libraries(writegraph degeneracy ${Boost_LIBRARIES})
target_link_libraries(parsegml fmdm expat)
target_link_libraries(enumerate degeneracy writegraph ${Boost_LIBRARIES})
#target_link_libraries(moddecomp )

target_link_libraries(fmdm_random fmdm)
target_link_libraries(fmdm_fromgml parsegml writegraph)
target_link_libraries(CliqueClac fmdm enumerate parsegml moddecomp)