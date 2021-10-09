cmake_minimum_required(VERSION 3.4...3.18)
project(light_show VERSION 0.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(NDARRAY_FFTW OFF)
set(NDARRAY_PYBIND11 ON)

option(RASPBERRY_PI "Build for Raspberry Pi?" ON)
if(RASPBERRY_PI)
    set(BUILD_SHARED ON CACHE BOOL "enable shared library")
    set(BUILD_TEST OFF CACHE BOOL "disable test")
    add_subdirectory(rpi_ws281x)
endif(RASPBERRY_PI)

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
target_compile_definitions(light_show PRIVATE VERSION_INFO=${VERSION_INFO})

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-config
        LIBRARY DESTINATION lib
        )
install(EXPORT ${PROJECT_NAME}-config
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/${PROJECT_NAME}
        )
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
