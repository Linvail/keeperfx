cmake_minimum_required(VERSION 3.20)

# Do not allow building in root
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "Do not build in the root. Create a bin directory and remove ${CMAKE_SOURCE_DIR}/CMakeCache.txt")
endif ()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(keeperfx C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# Get the abbreviated commit Id of the head.
find_package(Git REQUIRED)
execute_process(COMMAND "${GIT_EXECUTABLE}" describe --always OUTPUT_VARIABLE COMMIT_ID OUTPUT_STRIP_TRAILING_WHITESPACE)

set(VER_MAJOR       1)
set(VER_MINOR       1)
set(VER_RELEASE     0)
set(VER_BUILD       0)
set(VER_STRING      "${VER_MAJOR}.${VER_MINOR}.${VER_RELEASE}.${VER_BUILD} ${PACKAGE_SUFFIX}")
set(PACKAGE_SUFFIX  "")
set(GIT_REVISION    "${COMMIT_ID}")

# CMAKE_BINARY_DIR is defined in CMakePresets.json.
set(KEEPERFX_VER_DEFS_H_IN ${CMAKE_SOURCE_DIR}/ver_defs.h.in)
set(KEEPERFX_VER_DEFS_H_OUT ${CMAKE_BINARY_DIR}/ver_defs.h)
configure_file(${KEEPERFX_VER_DEFS_H_IN} ${KEEPERFX_VER_DEFS_H_OUT})

# Add binary dir to the include paths, so ver_defs.h can be found.
include_directories($(${CMAKE_BINARY_DIR}))

find_package(unofficial-enet CONFIG REQUIRED)
find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_image REQUIRED CONFIG)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_net CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(cpuid CONFIG REQUIRED)

if ("${CMAKE_CXX_COMPILER_ID};${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "Clang;MSVC") # clang-cl
    message(STATUS "We are using clang-cl")
    # -Wextra will give us too many warnings.
    add_compile_options(-Wall -Wshadow -Wmicrosoft-enum-value)
    set(CLANG_DISABLED_WARNINGS_LIST
    "-Wmicrosoft-fixed-enum"
    "-Wunused-parameter"
    "-Wunused-but-set-variable"
    "-Wstrict-aliasing"
    "-Wunknown-pragmas"
    "-Wundef"
    "-Wmissing-prototypes"
    "-Wsign-compare"
    "-Wsign-conversion"
    "-Wimplicit-int-conversion"
    "-Wstrict-prototypes"
    "-Wunsafe-buffer-usage"
    "-Wgnu-zero-variadic-macro-arguments"
    "-Wzero-as-null-pointer-constant"
    "-Wswitch-enum"
    "-Wunused-macros"
    "-Wdouble-promotion"
    "-Wcomma"
    "-Wextra-semi"
    "-Wextra-semi-stmt"
    "-Wdeclaration-after-statement"
    "-Wdocumentation"
    "-Wc++98-compat"
    "-Wc++98-compat-pedantic"
    "-Wold-style-cast"
    "-Wmissing-variable-declarations"
    "-Wcast-align"
    "-Wkeyword-macro"
    "-Wnested-anon-types"
    "-Wshorten-64-to-32"
    "-Wimplicit-fallthrough"
    "-Wgnu-anonymous-struct"
    "-Wzero-length-array"
    "-Wnonportable-system-include-path"
    "-Wnewline-eof"
    "-Wdeprecated-declarations"
    "-Wignored-attributes"
    "-Wcast-function-type"
    "-Wcast-function-type-strict"
    "-Wconditional-uninitialized"
    "-Wreserved-identifier"
    "-Wfloat-equal"
    "-Wbad-function-cast"
    "-Wcovered-switch-default"
    "-Wimplicit-int-float-conversion"
    "-Wimplicit-float-conversion"
    "-Wfloat-conversion"
    "-Wunreachable-code-break"
    "-Wlanguage-extension-token"
    "-Wpointer-to-int-cast"
    "-Wglobal-constructors"
    "-Wcast-qual"
    "-Wabsolute-value"
    "-Wunreachable-code-return"
    "-Wtautological-value-range-compare"
    "-Wint-to-pointer-cast"
    "-Wconstant-conversion"
    "-Wformat-nonliteral"
    "-Wincompatible-function-pointer-types-strict"
    "-Wempty-translation-unit"
    )
    string(REPLACE "-W" " -Wno-" CLANG_DISABLED_WARNINGS_STR ${CLANG_DISABLED_WARNINGS_LIST})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CLANG_DISABLED_WARNINGS_STR}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_DISABLED_WARNINGS_STR}")
    message(STATUS "${CMAKE_CXX_FLAGS}")
else("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC") # MSVC
    message(STATUS "We are using MSVC")
    set(MSVC_DISABLED_WARNINGS_LIST
    "C4018" # 'token' : signed/unsigned mismatch. @todo: Remove this.
    "C4100" # unreferenced formal parameter
    "C4127" # conditional expression is constant
    "C4200" # nonstandard extension used : zero-sized array in
            # struct/union.
    "C4201" # nonstandard extension used : nameless struct/union
    "C4242" # 'function' : conversion from 'int' to 'uint8_t',
            # possible loss of data
    "C4244" # 'function' : conversion from 'int' to 'uint8_t',
            # possible loss of data
    "C4245" # 'initializing' : conversion from 'long' to
            # 'unsigned long', signed/unsigned mismatch
    "C4267" # conversion from 'size_t' to 'int', possible loss of data
    "C4388" # signed/unsigned mismatch
    "C4296" # '>=' : expression is always true
    "C4366" # The result of the unary '&' operator may be unaligned
    "C4365" # '=' : conversion from 'size_t' to 'int',
            # signed/unsigned mismatch
    "C4389" # '!=' : signed/unsigned mismatch
    "C4800" # 'int' : forcing value to bool 'true' or 'false'
            # (performance warning)
    "C4820" # 'bytes' bytes padding added after construct 'member_name'
    )
    string(REPLACE "C" " /wd" MSVC_DISABLED_WARNINGS_STR ${MSVC_DISABLED_WARNINGS_LIST})
    set(MSVC_WARNING_FLAGS "/W4 ${MSVC_DISABLED_WARNINGS_STR}")
    set(MSVC_OTHER_FLAGS "/Zc:enumTypes") # This fix only works for C++.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MSVC_OTHER_FLAGS} ${MSVC_WARNING_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_OTHER_FLAGS} ${MSVC_WARNING_FLAGS}")
endif()

add_compile_definitions(_CRT_NONSTDC_NO_WARNINGS _CRT_SECURE_NO_WARNINGS
                        SPNG_STATIC)

file(GLOB_RECURSE KEEPERFX_SOURCES_C "src/*.c")
file(GLOB_RECURSE KEEPERFX_SOURCES_CXX "src/*.cpp")
# todo: We may want to handle sources in deps in deps/CMakeLists.txt.
file(GLOB_RECURSE LIBSPNG_SOURCES "deps/libspng/spng/*.c")
file(GLOB_RECURSE CENTIJSON_SOURCES "deps/centijson/src/*.c")
file(GLOB_RECURSE MINIZIP_SOURCES "deps/zlib/contrib/minizip/ioapi.c"
                                "deps/zlib/contrib/minizip/unzip.c")
file(GLOB_RECURSE ASTRONOMY_SOURCES "deps/astronomy/astronomy.c")
file(GLOB_RECURSE CENTITOML_SOURCES "deps/centitoml/toml_api.c")

# Global definitions for all targets.
add_compile_definitions(SPNG_STATIC=1)
add_compile_definitions("DEBUG=$<IF:$<CONFIG:Debug>,1,0>")

# Add two executable targets: keeperfx and keeperfx_hvlog.
add_executable(keeperfx ${KEEPERFX_SOURCES_C} ${KEEPERFX_SOURCES_CXX}
                        ${LIBSPNG_SOURCES}
                        ${CENTIJSON_SOURCES}
                        ${MINIZIP_SOURCES}
                        ${ASTRONOMY_SOURCES}
                        ${CENTITOML_SOURCES}
                        )
target_compile_definitions(keeperfx PUBLIC BFDEBUG_LEVEL=0)
target_sources(keeperfx PRIVATE "res/keeperfx_stdres.rc")
target_link_options(keeperfx PUBLIC "/MANIFEST:NO")

add_executable(keeperfx_hvlog ${KEEPERFX_SOURCES_C} ${KEEPERFX_SOURCES_CXX}
                        ${LIBSPNG_SOURCES}
                        ${CENTIJSON_SOURCES}
                        ${MINIZIP_SOURCES}
                        ${ASTRONOMY_SOURCES}
                        ${CENTITOML_SOURCES}
                        )
target_compile_definitions(keeperfx_hvlog PUBLIC BFDEBUG_LEVEL=10)
target_sources(keeperfx_hvlog PRIVATE "res/keeperfx_stdres.rc")
target_link_options(keeperfx_hvlog PUBLIC "/MANIFEST:NO")

# Add include paths.
list(APPEND includePath
    "${CMAKE_SOURCE_DIR}/deps/centijson/src"
    "${CMAKE_SOURCE_DIR}/deps/libspng/spng"
    "${CMAKE_SOURCE_DIR}/deps/centitoml"
    "${CMAKE_SOURCE_DIR}/deps/astronomy"
    )

target_include_directories(keeperfx PRIVATE "$<BUILD_INTERFACE:${includePath}>")
target_include_directories(keeperfx_hvlog PRIVATE "$<BUILD_INTERFACE:${includePath}>")

# System libraries.
# It seems that dbghelp and imagehlp cannot co-exist. We choose dbghelp.
target_link_libraries(keeperfx PRIVATE dbghelp)
target_link_libraries(keeperfx_hvlog PRIVATE dbghelp)

# External libraries.
target_link_libraries(keeperfx
    PRIVATE unofficial::enet::enet)
target_link_libraries(keeperfx
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
target_link_libraries(keeperfx
    PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
target_link_libraries(keeperfx
    PRIVATE $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,SDL2_net::SDL2_net,SDL2_net::SDL2_net-static>)
target_link_libraries(keeperfx
    PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
target_link_libraries(keeperfx PRIVATE ZLIB::ZLIB)
target_link_libraries(keeperfx PRIVATE cpuid::cpuid)

target_link_libraries(keeperfx_hvlog
    PRIVATE unofficial::enet::enet)
target_link_libraries(keeperfx_hvlog
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>)
target_link_libraries(keeperfx_hvlog
    PRIVATE $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
target_link_libraries(keeperfx_hvlog
    PRIVATE $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,SDL2_net::SDL2_net,SDL2_net::SDL2_net-static>)
target_link_libraries(keeperfx_hvlog
    PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
target_link_libraries(keeperfx_hvlog PRIVATE ZLIB::ZLIB)
target_link_libraries(keeperfx_hvlog PRIVATE cpuid::cpuid)