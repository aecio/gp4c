project(GPCrawl)
cmake_minimum_required(VERSION 2.8)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/threadpool-0_2_5-src/threadpool)
include_directories(${PROJECT_SOURCE_DIR}/../libs-ext/include)

link_directories(${PROJECT_SOURCE_DIR}/lib
                 ${PROJECT_SOURCE_DIR}/../libs-ext/lib)

# Flags de instalacao
#set(CMAKE_C_FLAGS "-Wall -Werror")
#set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_C_FLAGS "-Wreturn-type -Wuninitialized")
set(CMAKE_CXX_FLAGS "-Wreturn-type -Wuninitialized")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -DDISABLE_LOG_DEBUG -DGPINTERNALCHECK=0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -DDISABLE_LOG_DEBUG -DGPINTERNALCHECK=0")
set(CMAKE_C_FLAGS_RELEASETUNE "-O3 -DNDEBUG -DDISABLE_LOG_DEBUG -march=native -mtune=native -DGPINTERNALCHECK=0")
set(CMAKE_CXX_FLAGS_RELEASETUNE "-O3 -DNDEBUG -DDISABLE_LOG_DEBUG -march=native -mtune=native -DGPINTERNALCHECK=0")

add_subdirectory(crawling)

