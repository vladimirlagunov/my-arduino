cmake_minimum_required(VERSION 3.5)
project(foobar)

if(NOT DEFINED SDK_PATH)
  set(SDK_PATH "/home/lagunov/src/opensource/esp-open-sdk/sdk")
endif()

include_directories(
  $<TARGET_FILE_DIR:target_elf>/include
  src
  SYSTEM ${SDK_PATH}/include
  SYSTEM ${SDK_PATH}/driver_lib/include
)

set(_options_file "options.cmake")
include(${_options_file} OPTIONAL)
foreach(_varname SSID SSID_PASSWORD HOSTNAME)
  if(NOT DEFINED ${_varname})
    message(FATAL_ERROR "Specify variable ${_varname} directly or create `${_options_file}`")
  endif()
endforeach()

link_directories(${SDK_PATH}/lib)
set(CMAKE_C_COMPILER xtensa-lx106-elf-gcc)
set(CMAKE_CXX_COMPILER xtensa-lx106-elf-g++)
set(CMAKE_CXX_FLAGS -std=gnu++17)
set(
  common_compile_options

  -DICACHE_FLASH
  -D__ets__
  -O2
  -Os
  # little-endian
  -Wl,-EL

  # Warn about anything that depends on the "size of" a function type
  # or of "void".
  -Wpointer-arith

  # Warn whenever an identifier which is not a macro is encountered in
  # an #if directive, outside of defined.
  -Wundef

  # Do not expand any functions inline apart from those marked with
  # the "always_inline" attribute.
  -fno-inline-functions

  -g

  # GCC instructs the assembler to translate direct calls to indirect
  # calls unless it can determine that the target of a direct call is
  # in the range allowed by the call instruction.
  -mlongcalls

  # The literals are interspersed in the text section in order to keep
  # them as close as possible to their references. This may be
  # necessary for large assembly files.
  -mtext-section-literals

  -nostdlib
)
set(
  common_link_libraries

  -Wl,-static
  -nostdlib
)

file(GLOB esp_nonos_sdk_src ${SDK_PATH}/driver_lib/driver/*.c)
add_library(esp_nonos_sdk ${esp_nonos_sdk_src})
target_compile_options(
  esp_nonos_sdk PRIVATE

  ${common_compile_options}
  -Wno-implicit-function-declaration
)
target_link_libraries(
  esp_nonos_sdk

  ${common_link_libraries}

  -L${SDK_PATH}/lib
  -T${SDK_PATH}/ld/eagle.app.v6.ld

  # Asks the linker _not_ to check section addresses after they have
  # been assigned to see if there are any overlaps.
  -Wl,--no-check-sections
  -Wl,--start-group c gcc hal pp phy net80211 lwip wpa main -Wl,--end-group
)

configure_file(
  "src/user_config.h.in"
  "include/user_config.h"  # into build directory
)
file(
  GLOB target_elf_src
  src/*.c
  src/*.cpp
)
add_executable(target_elf ${target_elf_src})
add_dependencies(target_elf esp_nonos_sdk)

target_compile_options(target_elf PRIVATE ${common_compile_options})
target_link_libraries(target_elf "-u call_user_start" esp_nonos_sdk)

##############################
# Uploading firmware
# Usage: PORT=/dev/ttyUSB123 make upload
##############################

set(
  firmware_offsets
  0x00000
  0x10000
)

list(LENGTH firmware_offsets _firmware_offset_count)
MATH(EXPR _firmware_offset_count "${_firmware_offset_count} - 1")
set(_firmware_files)
set(_write_flash_args)
foreach(_idx RANGE ${_firmware_offset_count})
  list(GET firmware_offsets ${_idx} _firmware_offset)
  list(APPEND _firmware_files ${_firmware_offset}.bin)
  list(APPEND _write_flash_args ${_firmware_offset})
  list(APPEND _write_flash_args ${_firmware_offset}.bin)
endforeach()
add_custom_command(
  OUTPUT ${_firmware_files}
  COMMAND esptool.py elf2image -o ./ target_elf
  DEPENDS target_elf
)
add_custom_target(firmware ALL DEPENDS ${_firmware_files})
set(ADDITIONAL_MAKE_CLEAN_FILES ${_firmware_files})
add_custom_target(
  upload
  # TODO: allow specify port
  COMMAND esptool.py write_flash ${_write_flash_args}
  DEPENDS ${_firmware_files}
)
