#****************************This is Project Info****************************
PROJECT(softVehicleRecorder)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#****************************This is Compile Flag****************************
 if(DEBUG)        
 message( STATUS "==============================debug==============================" )        
 ADD_DEFINITIONS(-g)
 #ADD_DEFINITIONS(-D AVX_DEBUG_LEVEL=AVX_DEBUG_DEBUG)
else(DEBUG)   
 message( STATUS "==============================release==============================" )         
 ADD_DEFINITIONS(-O2)
 #ADD_DEFINITIONS(-D AVX_DEBUG_LEVEL=AVX_DEBUG_INFO)
 endif()

ADD_DEFINITIONS("-Wall")
ADD_DEFINITIONS("-fPIC")
ADD_DEFINITIONS("-Wl,-Bsymbolic -lm")
#ADD_DEFINITIONS("-std=c++11")
#ADD_DEFINITIONS("-DLIBSERVLET_EXPORTS")
SET(CMAKE_CXX_FLAGS "-Wall -fPIC -s -Wl,-Bsymbolic -lm")

#****************************This is Compiling environment Flag****************************
SET(CMAKE_SYSTEM_NAME linux)
SET(TOOLCHAIN_DIR "/usr")
SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_DIR})
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/g++)

#****************************This is Include File/Directories****************************
#INCLUDE_DIRECTORIES(../../../Library/Include)

#****************************This is Source File/Directories****************************
#AUX_SOURCE_DIRECTORY(../ VEHICLE_SRCS)

#SET(DIR_SRCS ${Servlet_SRCS})

#****************************This is Library Directories****************************
#LINK_DIRECTORIES(./)
#LINK_DIRECTORIES(../../../Bin/HPC03C)
#LINK_DIRECTORIES(../../../Library/ISVP/lib)

#****************************This is 3Party Library****************************
LINK_LIBRARIES(pthread dl m)

#****************************This is Output Directory****************************
SET(CMAKE_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/output)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_OUTPUT_DIR}/bin)
MESSAGE(STATUS "Bin Path: " ${EXECUTABLE_OUTPUT_PATH})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_OUTPUT_DIR}/lib)
MESSAGE(STATUS "Lib Path: " ${LIBRARY_OUTPUT_PATH})

#****************************This is config Directory****************************
OPTION(CONFIG_COMMON_QUEUE "Use queue Module" ON)
OPTION(CONFIG_COMMON_UART "Use Uart Module" ON)
OPTION(CONFIG_COMMON_MXML "Use XML Module" ON)
OPTION(CONFIG_COMMON_JSON "Use Json Modlue" OFF)

SET(CONFIG_FILE_PATH ${PROJECT_SOURCE_DIR}/config)

CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/config/config.h.in"
    "${PROJECT_SOURCE_DIR}/public/config.h"
)

ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(src)

#****************************This is Output Library****************************
#ADD_LIBRARY(${PROJECT_NAME} SHARED ${VEHICLE_SRCS} )
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} pthread dl)

