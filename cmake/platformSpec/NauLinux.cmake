# Setup for GCC/Clang compiler for Linux
# Compiler_GCC:   gcc
# Compiler_Clang: clang

set(Platform_Linux ON)

if(${Host_Arch} STREQUAL x64)
    set(Platform_Linux64 ON)
elseif(${Host_Arch} STREQUAL x86)
    set(Platform_Linux32 ON)
else()
    message(FATAL_ERROR "Unknown linux target platform:(${Platform})")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(Compiler_GCC ON)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(Compiler_Clang ON)
else()
    message(FATAL_ERROR "Unsupported compiler:(${CMAKE_CXX_COMPILER_ID}) on (${CMAKE_SYSTEM_NAME})")
endif()

set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
include(InstallRequiredSystemLibraries)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(_DEF_C_CPP_DEFINITIONS_DEBUG)
set(_CONFIG_OPTIONS_RELWITHDEBINFO
    NDEBUG=1
)
set(_DEF_C_CPP_DEFINITIONS_RELEASE
    NDEBUG=1
)

set(_DEF_C_CPP_DEFINITIONS
    $<$<CONFIG:Debug>:${_DEF_C_CPP_DEFINITIONS_DEBUG}>
    $<$<CONFIG:Release>:${_DEF_C_CPP_DEFINITIONS_RELEASE}>
    $<$<CONFIG:RelWithDebInfo>:${_CONFIG_OPTIONS_RELWITHDEBINFO}>
)

set(_DEF_C_CPP_OPTIONS
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    $<$<CONFIG:Debug>:-O0 -g>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g -DNDEBUG>
)

if(Compiler_GCC)
    list(APPEND _DEF_C_CPP_OPTIONS
        -fstack-protector-strong
        -fPIC
    )
endif()

if(Compiler_Clang)
    list(APPEND _DEF_C_CPP_OPTIONS
        -fstack-protector-strong
        -fPIC
    )
endif()

set(_STRICT_DIAGNOSTIC_OPTIONS -Wall -Wextra -Wpedantic -Werror)

set(_CONFIG_OPTIONS_DEBUG -O0 -g)
set(_CONFIG_OPTIONS_RELEASE -O3 -DNDEBUG)
set(_CONFIG_OPTIONS_RELWITHDEBINFO -O2 -g -DNDEBUG)

set(_CONFIG_OPTIONS
    $<$<CONFIG:Debug>:${_CONFIG_OPTIONS_DEBUG}>
    $<$<CONFIG:Release>:${_CONFIG_OPTIONS_RELEASE}>
    $<$<CONFIG:RelWithDebInfo>:${_CONFIG_OPTIONS_RELWITHDEBINFO}>
)

set(_CPP_OPTIONS
    ${_DEF_C_CPP_OPTIONS}
    ${_DIAGNOSTIC_OPTIONS}
    ${_CONFIG_OPTIONS}
)

set(_CPP_STRICT_OPTIONS
    ${_DEF_C_CPP_OPTIONS}
    ${_STRICT_DIAGNOSTIC_OPTIONS}
    ${_CONFIG_OPTIONS}
)

set(_C_OPTIONS ${_DEF_C_CPP_OPTIONS} ${_DIAGNOSTIC_OPTIONS} ${_CONFIG_OPTIONS})
set(_C_STRICT_OPTIONS ${_DEF_C_CPP_OPTIONS} ${_STRICT_DIAGNOSTIC_OPTIONS} ${_CONFIG_OPTIONS})

if(Compiler_GCC)
    list(APPEND _C_OPTIONS -std=c11)
    list(APPEND _C_STRICT_OPTIONS -std=c11)
endif()

if(Compiler_Clang)
    list(APPEND _C_OPTIONS -std=c11)
    list(APPEND _C_STRICT_OPTIONS -std=c11)
endif()

# Enable language for ASM if needed
# enable_language(ASM_NASM)
# enable_language(ASM_MASM)

# Set compile options for ASM if needed
# set(CMAKE_ASM_NASM_COMPILE_OBJECT "<CMAKE_ASM_NASM_COMPILER> <INCLUDES> <FLAGS> -o <OBJECT> <SOURCE>")
# set(CMAKE_ASM_MASM_CREATE_STATIC_LIBRARY "<CMAKE_AR> /OUT:<TARGET> <LINK_FLAGS> <OBJECTS>")

# Add compile options for ASM if needed
# add_compile_options(
#     "$<$<COMPILE_LANGUAGE:ASM_NASM>:-f $<IF:$<BOOL:$<TARGET_PROPERTY:NASM_OBJ_FORMAT>>, \
#     $<TARGET_PROPERTY:NASM_OBJ_FORMAT>, ${CMAKE_ASM_NASM_OBJECT_FORMAT}>>"
# )
