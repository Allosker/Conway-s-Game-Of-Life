cmake_minimum_required(VERSION 3.11)

project(IW LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/src")

file(GLOB_RECURSE CPP_FILES CMAKE_CONFIGURE_DEPENDS
	"${SRC_DIR}/*.cpp"
	"${SRC_DIR}/*.c"  
) 
 
file(GLOB_RECURSE HPP_FILES CMAKE_CONFIGURE_DEPENDS  
	"${INCLUDE_DIR}/*.hpp"
	"${INCLUDE_DIR}/*.h"
)  

# add executables

add_executable( IW
	${CPP_FILES}
)  

target_include_directories(IW PUBLIC ${INCLUDE_DIR})

set(ASSET_DIR "${CMAKE_SOURCE_DIR}/assets/")
target_compile_definitions(IW PRIVATE ASSET_PATH="${ASSET_DIR}")

# libs

include(FetchContent) 

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# compile and link libs

target_compile_features(IW PUBLIC cxx_std_20)

target_link_libraries(IW PUBLIC SFML::Graphics)