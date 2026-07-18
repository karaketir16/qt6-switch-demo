set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(UNIX TRUE)
set(NX TRUE)

if(NOT DEFINED ENV{DEVKITPRO})
    message(FATAL_ERROR "DEVKITPRO is not set")
endif()

set(DEVKITPRO "$ENV{DEVKITPRO}")
set(DEVKITA64 "${DEVKITPRO}/devkitA64")
set(LIBNX "${DEVKITPRO}/libnx")

set(CMAKE_C_COMPILER "${DEVKITA64}/bin/aarch64-none-elf-gcc")
set(CMAKE_CXX_COMPILER "${DEVKITA64}/bin/aarch64-none-elf-g++")
set(CMAKE_ASM_COMPILER "${DEVKITA64}/bin/aarch64-none-elf-gcc")
set(CMAKE_AR "${DEVKITA64}/bin/aarch64-none-elf-gcc-ar")
set(CMAKE_RANLIB "${DEVKITA64}/bin/aarch64-none-elf-gcc-ranlib")
set(CMAKE_STRIP "${DEVKITA64}/bin/aarch64-none-elf-strip")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(SWITCH_ARCH_FLAGS "-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE")
set(SWITCH_COMMON_FLAGS "${SWITCH_ARCH_FLAGS} -D__SWITCH__")

set(CMAKE_C_FLAGS_INIT "${SWITCH_COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${SWITCH_COMMON_FLAGS} -fno-exceptions")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-specs=${LIBNX}/switch.specs ${SWITCH_ARCH_FLAGS}")
# libnx's switch.specs supplies the CRT and linker layout, while applications
# and QtTest executables must still link the runtime library itself.
set(CMAKE_C_STANDARD_LIBRARIES_INIT "-lnx")
set(CMAKE_CXX_STANDARD_LIBRARIES_INIT "-lnx")

set(CMAKE_FIND_ROOT_PATH
    "${DEVKITA64}"
    "${LIBNX}"
    "${DEVKITPRO}/portlibs/switch"
)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

include_directories(
    SYSTEM
    "${LIBNX}/include"
    "${DEVKITPRO}/portlibs/switch/include"
)

link_directories(
    "${LIBNX}/lib"
    "${DEVKITPRO}/portlibs/switch/lib"
)
