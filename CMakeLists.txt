cmake_minimum_required(VERSION 3.10)

project(Speedrunner CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SDL3_STATIC "Static link SDL3" OFF)

add_executable(speedrunner
	src/main.cpp
	src/game.cpp
	src/video.cpp
)

target_include_directories(speedrunner PUBLIC "${PROJECT_SOURCE_DIR}/include")

find_package(SDL3 REQUIRED)

if(SDL3_STATIC)
	target_link_libraries(speedrunner ${SDL3_STATIC_LIBRARIES})
else()
	target_link_libraries(speedrunner ${SDL3_LIBRARIES})
endif()
