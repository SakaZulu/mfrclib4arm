cmake_minimum_required (VERSION 2.6)
# Project Name
set (PROJECT_NAME Mfrc522)
# The version number.
set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)

set (COMP_PREFIX $ENV{COMPILER_PREFIXX})


# Check  COMPILER_PREFIXX Check
if(DEFINED COMP_PREFIX)
	message(STATUS "*** COMPILER_PREFIXX=${COMP_PREFIX}")
else()
	message(SEND_ERROR "*** COMPILER_PREFIXX variable NOT SET ! !  ")
endif()

# Set Project Name
project (${PROJECT_NAME})
# Set  Library Name
set (LIBRARY_NAME ${PROJECT_NAME})
# Set AuxBinary Name
set (BIN_NAME Mfrc_app)

# Project Sources
set (
	SOURCES_LIB_MFRC 
	gpio.c  spi.c Driver/MF522.c  ## ADD NEW Sources
	)



# Project Inlcudes
set (INCLUDES_LIB Driver/)
set (COMP_PATH /home/eric/x-tools/arm-unknown-linux-gnueabi/bin/ ) 

# Specify the cross compiler
#set ( CMAKE_C_COMPILER   ${COMP_PATH}/arm-unknown-linux-gnueabi-gcc  )
#set ( CMAKE_CXX_COMPILER ${COMP_PATH}/arm-unknown-linux-gnueabi-g++  )
set ( CMAKE_C_COMPILER   ${COMP_PREFIX}gcc  )
set ( CMAKE_CXX_COMPILER ${COMP_PREFIX}g++  )

message(STATUS 	"			>> >> >> Custom Configurations for Raspberry Pi Arm Compiler << << <<")
message(STATUS 	"*** SET > C   COMPILER=${CMAKE_C_COMPILER}")
message(STATUS 	"*** SET > CXX COMPILER=${CMAKE_CXX_COMPILER}")

# **************************** COMPILER FLAGS *****************************************
set(CMAKE_C_FLAGS "-Wall  -fmessage-length=0")
#set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS}")
#set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS}")


# configure a header file to pass some of the CMake settings to the source code
#configure_file (
#  "${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
#  "${PROJECT_BINARY_DIR}/TutorialConfig.h"
#  )
# 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

add_library(
	${LIBRARY_NAME}
	SHARED
	${SOURCES_LIB_MFRC}
	${INCLUDES_LIB}
	)
add_executable(
	${BIN_NAME} 
	${SOURCES_LIB_MFRC} main.c
	)
	