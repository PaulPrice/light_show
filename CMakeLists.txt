cmake_minimum_required(VERSION 3.4...3.18)
project(light_show VERSION 0.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(NDARRAY_FFTW OFF)
set(NDARRAY_PYBIND11 ON)

#set(BUILD_SHARED ON CACHE BOOL "enable shared library")
#set(BUILD_TEST OFF CACHE BOOL "disable test")
#add_subdirectory(rpi_ws281x)

find_package(pybind11)
find_package(Boost)
find_package(eigen3)

include_directories(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/ndarray/include
                    ${Boost_INCLUDE_DIR}
                    ${EIGEN3_INCLUDE_DIRS}
                    )

add_subdirectory(ndarray)

pybind11_add_module(light_show python/light_show/colors.cc
                               python/light_show/LedController.cc
                               python/light_show/LedStrip.cc
                    )
