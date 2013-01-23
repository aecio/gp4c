project(GPCrawl)
cmake_minimum_required(VERSION 2.8)

include_directories(${PROJECT_SOURCE_DIR}/gpcpp/include)
include_directories(${PROJECT_SOURCE_DIR}/gpcpp/threadpool-0_2_5-src/threadpool)
include_directories(${PROJECT_SOURCE_DIR}/../libs-ext/include)

link_directories(${PROJECT_SOURCE_DIR}/../libs-ext/lib)

###########################################
##       DIRETIVAS DE COMPILACAO         ##
###########################################

# Ex: cmake .. -DCMAKE_BUILD_TYPE=Release

# Define o modo de compilacao
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE DEBUG)
endif(NOT CMAKE_BUILD_TYPE)

# Define o tipo de library: static ou shared
if( NOT DEFINED BUILD_SHARED_LIBS )
    set(BUILD_SHARED_LIBS OFF)
endif( NOT DEFINED BUILD_SHARED_LIBS )

#INCLUDE(CMakeForceCompiler)
#CMAKE_FORCE_C_COMPILER(gcc-4.4 GNU)
#CMAKE_FORCE_CXX_COMPILER(g++-4.4 GNU)

# Flags de instalacao
#set(CMAKE_C_FLAGS "-Wall -Werror")
#set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_C_FLAGS "-Wreturn-type -Wuninitialized")
set(CMAKE_CXX_FLAGS "-Wreturn-type -Wuninitialized")
set(CMAKE_C_FLAGS_DEBUG "-g -DGPINTERNALCHECK=1")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DGPINTERNALCHECK=1")
set(CMAKE_C_FLAGS_RELEASE "-g -O3 -DNDEBUG -DGPINTERNALCHECK=0")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 -DNDEBUG -DGPINTERNALCHECK=0")
set(CMAKE_C_FLAGS_RELEASETUNE "-g -O3 -DNDEBUG -march=native -mtune=native -DGPINTERNALCHECK=0")
set(CMAKE_CXX_FLAGS_RELEASETUNE "-g -O3 -DNDEBUG -march=native -mtune=native -DGPINTERNALCHECK=0")

###########################################
##             SUBDIRETORIOS             ##
###########################################

add_subdirectory(src)
add_subdirectory(gpcpp)

