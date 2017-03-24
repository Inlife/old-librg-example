###
# CMake entry point
###
cmake_minimum_required (VERSION 2.8)
project (server-test)


###
# Predefines to block make at the root
###
if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
    message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
    message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

###
# Compile vendor dependencies
###
add_subdirectory(vendor/reguider)
add_subdirectory(vendor/SDL)

###
# Includes for libs and remote sources
###
include_directories(
    vendor/reguider/include
    vendor/SDL/include
    server/
)

###
# Includes for current source executable
###
file(GLOB_RECURSE SERVER_FILES "${PROJECT_SOURCE_DIR}/server/*.cpp")
file(GLOB_RECURSE CLIENT_FILES "${PROJECT_SOURCE_DIR}/client/*.cpp")

###
# Adding executables
###
add_executable(server ${SERVER_FILES})
add_executable(client ${CLIENT_FILES})

###
# Linking libraries
###
target_link_libraries(server librg)
target_link_libraries(client librg SDL2-static)
