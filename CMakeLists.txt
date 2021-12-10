cmake_minimum_required(VERSION 3.14)
project(light_show VERSION 0.0.0 LANGUAGES C CXX)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g")

include(CheckIncludeFile)
CHECK_INCLUDE_FILE(bcm_host.h RASPBERRY_PI "-I/opt/vc/include/")
if(RASPBERRY_PI)
    set(BUILD_SHARED OFF CACHE BOOL "enable shared library")
    set(BUILD_TEST OFF CACHE BOOL "disable test")
    add_subdirectory(rpi_ws281x)
    add_definitions(-DRASPBERRY_PI)
endif(RASPBERRY_PI)

find_package(Boost)
find_package(Eigen3)

add_subdirectory(pybind11)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(SYSTEM
                    ${PROJECT_SOURCE_DIR}/ndarray/include
                    ${Boost_INCLUDE_DIR}
                    ${EIGEN3_INCLUDE_DIRS}
                    )

set(LIB "light_show")
file(GLOB LIB_SRC src/*.cc)
add_library(${LIB} ${LIB_SRC})
if(RASPBERRY_PI)
    target_link_libraries(${LIB} PRIVATE ws2811)
endif(RASPBERRY_PI)

file(GLOB PYMOD_SRC python/light_show/*.cc)
foreach(PYMOD_FILE ${PYMOD_SRC})
    get_filename_component(PYMOD ${PYMOD_FILE} NAME_WLE)
    pybind11_add_module(${PYMOD} SHARED ${PYMOD_FILE})
    set_target_properties(${PYMOD} PROPERTIES PREFIX "" SUFFIX ".so")
    set_target_properties(${PYMOD} PROPERTIES CXX_VISIBILITY_PRESET "default")
    target_link_libraries(${PYMOD} PRIVATE ${LIB})
    install(TARGETS ${PYMOD} COMPONENT PYTHON LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/python/light_show)
endforeach()

include(GNUInstallDirs)
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY python DESTINATION ${CMAKE_INSTALL_PREFIX})
