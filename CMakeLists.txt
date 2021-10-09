cmake_minimum_required(VERSION 3.4...3.18)
project(light_show VERSION 0.0.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O2")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

include(CheckIncludeFile)
CHECK_INCLUDE_FILE(/opt/vc/include/bcm_host.h RASPBERRY_PI)
if(RASPBERRY_PI)
    set(BUILD_SHARED ON CACHE BOOL "enable shared library")
    set(BUILD_TEST OFF CACHE BOOL "disable test")
    add_subdirectory(rpi_ws281x)
endif(RASPBERRY_PI)

find_package(pybind11 CONFIG REQUIRED)
find_package(Boost)
find_package(eigen3)

include_directories(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/ndarray/include
                    ${Boost_INCLUDE_DIR}
                    ${EIGEN3_INCLUDE_DIRS}
                    )

option(NDARRAY_FFTW "Enable FFTW tests in ndarray?" OFF)
option(NDARRAY_PYBIND11 "Enable Pybind11 tests in ndarray?" ON)
add_subdirectory(ndarray)

foreach(PYMOD LedController colors LedStrip)
    pybind11_add_module(${PYMOD} SHARED python/light_show/${PYMOD}.cc)
    target_link_libraries(${PYMOD} PUBLIC pybind11::module)
    set_target_properties(${PYMOD} PROPERTIES PREFIX "" SUFFIX ".so")
    install(TARGETS ${PYMOD} COMPONENT PYTHON LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/python/light_show)
endforeach()

include(GNUInstallDirs)
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY python DESTINATION ${CMAKE_INSTALL_PREFIX})
