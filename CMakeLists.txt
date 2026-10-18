cmake_minimum_required(VERSION 2.7)
PROJECT(indi_gpio_drv8833_focus C CXX)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/")
#LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../indi-3rdparty/cmake_modules/")
include(GNUInstallDirs)

set (GPIO_DRV8833_VERSION_MAJOR 0)
set (GPIO_DRV8833_VERSION_MINOR 0)

find_package(INDI REQUIRED)
find_package(Threads REQUIRED)

find_library(pigpiod_if2 REQUIRED)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/indi_gpio_drv8833_focus.xml.cmake ${CMAKE_CURRENT_BINARY_DIR}/indi_gpio_drv8833_focus.xml)

include_directories( ${CMAKE_CURRENT_BINARY_DIR})
include_directories( ${CMAKE_CURRENT_SOURCE_DIR})
include_directories( ${INDI_INCLUDE_DIR})

include(CMakeCommon)

########### GPIO DRV8833 Focuser ###########
set(indigpio_drv8833_SRCS
   ${CMAKE_CURRENT_SOURCE_DIR}/GPIO_DRV8833_Focuser.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/Stepper.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/bcm2835.c
   )

IF (UNITY_BUILD)
    ENABLE_UNITY_BUILD(indigpio_drv8833_focus indigpio_drv8833_SRCS 10 cpp)
ENDIF ()

add_executable(indi_gpio_drv8833_focus ${indigpio_drv8833_SRCS})

target_link_libraries(indi_gpio_drv8833_focus ${INDI_DRIVER_LIBRARIES} pigpiod_if2 pthread)

install(TARGETS indi_gpio_drv8833_focus RUNTIME DESTINATION bin )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/indi_gpio_drv8833_focus.xml DESTINATION ${INDI_DATA_DIR})
