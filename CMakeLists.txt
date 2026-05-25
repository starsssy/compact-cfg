cmake_minimum_required(VERSION "3.19.2")

set(CMAKE_CXX_STANDARD 20)

project(compact-cfg)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

set(PROJECT_SOURCES
	${SOURCES}
	${HEADERS}
)

add_executable(basic-example ${PROJECT_SOURCES} "examples/basic_usage.cpp")
add_executable(full-example ${PROJECT_SOURCES} "examples/example_system.cpp")
