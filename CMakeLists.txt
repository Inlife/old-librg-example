###
# CMake entry point
###
cmake_minimum_required (VERSION 2.8)
project (server)


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


###
# Custom building flags
###
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

###
# Compile vendor dependencies
###
add_subdirectory(vendor/reguider)

## TODO: fix later for beautiful c++17 code
# if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
# else()
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
# endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

###
# Hacks
# On Visual 2005 and above, this module can set the debug working directory
###
# cmake_policy(SET CMP0026 OLD)
# list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/vendor/rpavlik-cmake-modules-fe2273")
# include(CreateLaunchers)
# include(MSVCMultipleProcessCompile) # /MP
set(CMAKE_MACOSX_RPATH 1)

###
# Includes for libs and remote sources
###
include_directories(
    vendor/reguider/
    server/
)

file(GLOB_RECURSE SERVER_FILES
  "${PROJECT_SOURCE_DIR}/server/*.cpp"
)

###
# Includes for current source executable
###
add_executable(server
    ${SERVER_FILES}
)

###
# Linking libraries
###
target_link_libraries(server
	librg
)

target_include_directories(librg PUBLIC
    vendor/reguider/librg
)

# Xcode and Visual working directories
# set_target_properties(server PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/librg/")
# create_target_launcher(server WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/librg/")


# SOURCE_GROUP(shared REGULAR_EXPRESSION ".*/shared/.*" )

# if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
# add_custom_command(
#    TARGET server POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/server${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/server/"
# )

# elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

# endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
