cmake_minimum_required(VERSION 3.3)

set(TARGET_NAME st_sid_player.elf)

configure_file(system/STM32F746NGHx_FLASH.ld linker.ld)
 
set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_ASM_COMPILER arm-none-eabi-as)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(SIZE arm-none-eabi-size)

SET(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)
SET(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 1)

set(COMMON_FLAGS "-mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -ggdb -O0 -Wall -Werror -fmessage-length=0 -ffunction-sections -specs=nosys.specs -specs=nano.specs" CACHE STRING "C compiler flags")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}" CACHE STRING "assembler flags")

set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> ${CMAKE_ASM_FLAGS} <SOURCE> -o <OBJECT>")

project(st_sid_player C ASM)

include(gui/icons/icons.cmake)

add_definitions(-DSTM32F746xx)
add_definitions(-DUSE_HAL_DRIVER)
add_definitions(-DUSE_STM32746G_DISCO)
add_definitions(-D__FPU_PRESENT=1)

include_directories(.)
include_directories(config)
include_directories(system)
include_directories(drivers/CMSIS/Include)
include_directories(drivers/CMSIS/Device/ST/STM32F7xx/Include)
include_directories(drivers/HAL/Inc)
include_directories(drivers/BSP/STM32746G-Discovery)
include_directories(drivers/FatFs/src)
include_directories(utils)
include_directories(gui)
include_directories(gui/STemWin/inc)
include_directories(gui/STemWin/OS)
include_directories(${CMAKE_BINARY_DIR})

aux_source_directory(. SRC)
aux_source_directory(config SRC)
aux_source_directory(app SRC)
aux_source_directory(system SRC)
aux_source_directory(drivers/BSP/STM32746G-Discovery SRC)
aux_source_directory(drivers/BSP/Components/wm8994 SRC)
aux_source_directory(drivers/BSP/Components/ft5336 SRC)
aux_source_directory(drivers/HAL/Src SRC)
aux_source_directory(drivers/FatFs/src SRC)
aux_source_directory(drivers/FatFs/src/drivers SRC)
aux_source_directory(drivers/FatFs/src/option SRC)
aux_source_directory(sid SRC)
aux_source_directory(utils SRC)
aux_source_directory(utils/RTT SRC)
aux_source_directory(gui SRC)
aux_source_directory(gui/screens SRC)
aux_source_directory(gui/widgets SRC)
aux_source_directory(gui/icons SRC)
aux_source_directory(gui/fonts SRC)
aux_source_directory(gui/STemWin/OS SRC)

set(SRC ${SRC} system/startup_stm32f746xx.s)
set_source_files_properties(system/startup_stm32f746xx.s PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp")

link_directories(gui/STemWin/Lib)

add_executable(${TARGET_NAME} ${SRC})

target_link_libraries(${TARGET_NAME} STemWin528_CM7_GCC)
target_link_libraries(${TARGET_NAME} m c)

set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-T linker.ld -Wl,-Map=${TARGET_NAME}.map -Wl,--gc-sections")

add_custom_target(size ALL DEPENDS ${TARGET_NAME} COMMAND ${SIZE} -A ${TARGET_NAME})
