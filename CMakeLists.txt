cmake_minimum_required(VERSION 3.1)

set(project_name SIOpanel)

project(${project_name})

add_executable(${project_name} src/app.cpp)
target_include_directories(${project_name} PUBLIC src)
find_package(oatpp 1.3.0 REQUIRED)
find_package(tomlplusplus REQUIRED)
target_link_libraries(${project_name} PUBLIC oatpp)

SET(CMAKE_CXX_FLAGS "-std=c++20 -Wall")
IF(LINK STREQUAL "STATIC")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
ENDIF()
SET(CMAKE_CXX_FLAGS_DEBUG "-g -pedantic -Wextra -Wno-unused-result -fsanitize=address,undefined -fno-sanitize-recover -fstack-protector")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -static")
