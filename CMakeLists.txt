cmake_minimum_required(VERSION 3.5.0)
project(VoronoiEve)

# Include uses based paths, start at src/.
include_directories(src)
include_directories(libs/include)

# Add sources using GLOB pattern.
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Use c++14.
set(CMAKE_CXX_STANDARD 14)

# Command line flags
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

# Build target.
add_executable(VoronoiEve ${SOURCES})

# Library includes
message(${CMAKE_CXX_FLAGS_RELEASE})

# Add all required system libraries.
# set(PROJECT_LINK_LIBS curses)

# Do OS X application stuff.
if (APPLE)
	find_library(CF CoreFoundation)
	find_library(SECURITY Security)
	target_link_libraries(VoronoiEve ${CF})
	target_link_libraries(VoronoiEve ${SECURITY})

	# Linker step.
	target_link_libraries(VoronoiEve "${PROJECT_SOURCE_DIR}/libs/osx/libboost_system-mt.a")
	target_link_libraries(VoronoiEve "${PROJECT_SOURCE_DIR}/libs/osx/libboost_chrono-mt.a")
	target_link_libraries(VoronoiEve "${PROJECT_SOURCE_DIR}/libs/osx/libboost_thread-mt.a")
	target_link_libraries(VoronoiEve "${PROJECT_SOURCE_DIR}/libs/osx/libcrypto.a")
	target_link_libraries(VoronoiEve "${PROJECT_SOURCE_DIR}/libs/osx/libssl.a")
	target_link_libraries(VoronoiEve "${PROJECT_SOURCE_DIR}/libs/osx/libcommon_utilities.a")
	target_link_libraries(VoronoiEve "${PROJECT_SOURCE_DIR}/libs/osx/libcpprest.a")

endif(APPLE)
