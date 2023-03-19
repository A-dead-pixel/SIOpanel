cmake_minimum_required(VERSION 3.1)

set(project_name SIOpanel)
set(CMAKE_MODULE_PATH /usr/share/cmake/Modules/)

project(${project_name})

#set(OATPP_LINK_ATOMIC False)

add_executable(${project_name} 
    src/app.cpp
    src/config.cpp
    src/utils.cpp
    src/control_utils.cpp
)
target_include_directories(${project_name} PUBLIC src)
find_package(oatpp 1.3.0 REQUIRED)
find_package(tomlplusplus REQUIRED)

target_link_libraries(${project_name} PUBLIC oatpp)

SET(CMAKE_CXX_FLAGS "-std=c++20 -Wall -lboost_thread -lboost_chrono")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3 -pedantic -Werror -Wextra -Wshadow -Wno-unused-result -Wuninitialized -Wconversion -Wfloat-equal -Wformat=2 -Wlogical-op -Wduplicated-cond -Wcast-qual -Wcast-align -DGLIBCXX_DEBUG -fsanitize=address,undefined -fno-sanitize-recover -fstack-protector")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -static")
