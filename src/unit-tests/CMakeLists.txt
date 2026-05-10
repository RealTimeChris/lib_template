# MIT License
# 
# Copyright (c) 2026 RealTimeChris
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
include(FetchContent)

option(LT_ASAN "Enable AddressSanitizer" OFF)
option(LT_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

set(LT_CLANG_COMPILE_OPTIONS
    $<$<BOOL:${LT_ASAN}>:
        -fsanitize=address
        -fsanitize-address-use-after-scope
        -U_FORTIFY_SOURCE
        -D_FORTIFY_SOURCE=0
    >
    $<$<BOOL:${LT_UBSAN}>:
        -fsanitize=undefined
        -fno-sanitize-recover=all
    >
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g>
    -Weverything
    -Werror
    -Wno-undef
    -Wno-unsafe-buffer-usage
    -Wno-poison-system-directories
    -Wno-c++20-compat
    -Wno-padded
    -Wno-zero-as-null-pointer-constant
    -Wno-c++98-compat-pedantic
    -Wno-c++98-compat
    -Wno-float-equal
    -Wno-class-varargs
)

set(LT_GNU_COMPILE_OPTIONS
    $<$<BOOL:${LT_ASAN}>:
        -fsanitize=address
        -fsanitize-address-use-after-scope
        -U_FORTIFY_SOURCE
        -D_FORTIFY_SOURCE=0
    >
    $<$<BOOL:${LT_UBSAN}>:
        -fsanitize=undefined
        -fno-sanitize-recover=all
    >
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g>
    -Wpedantic
    -Wextra
    -Werror
    -Wall
    $<$<NOT:$<CONFIG:Debug>>:-fwhole-program>
)

set(LT_MSVC_COMPILE_OPTIONS
    $<$<BOOL:${LT_ASAN}>:/fsanitize=address>
    /W4
    /WX
)

set(LT_COMPILE_OPTIONS
    $<$<CXX_COMPILER_ID:AppleClang>:${LT_CLANG_COMPILE_OPTIONS}>
    $<$<CXX_COMPILER_ID:Clang>:${LT_CLANG_COMPILE_OPTIONS}>
    $<$<CXX_COMPILER_ID:MSVC>:${LT_MSVC_COMPILE_OPTIONS}>
    $<$<CXX_COMPILER_ID:GNU>:${LT_GNU_COMPILE_OPTIONS}>
)

set(LT_CLANG_LINK_OPTIONS
    $<$<BOOL:${LT_ASAN}>:-fsanitize=address>
    $<$<BOOL:${LT_UBSAN}>:-fsanitize=undefined>
)

set(LT_GNU_LINK_OPTIONS
   $<$<BOOL:${LT_ASAN}>:
        -fsanitize=address
        -static-libasan
    >
    $<$<BOOL:${LT_UBSAN}>:-fsanitize=undefined>
)

set(LT_MSVC_LINK_OPTIONS
    $<$<BOOL:${LT_ASAN}>:/INFERASANLIBS>
)

set(LT_LINK_OPTIONS
    $<$<CXX_COMPILER_ID:AppleClang>:${LT_CLANG_LINK_OPTIONS}>
    $<$<CXX_COMPILER_ID:Clang>:${LT_CLANG_LINK_OPTIONS}>
    $<$<CXX_COMPILER_ID:MSVC>:${LT_MSVC_LINK_OPTIONS}>
    $<$<CXX_COMPILER_ID:GNU>:${LT_GNU_LINK_OPTIONS}>
)

FetchContent_Declare(
  rt-ut
  GIT_REPOSITORY https://github.com/realtimechris/rt-ut.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(rt-ut)

add_executable(lib_template_unit_tests "lib_template.cpp")

target_link_libraries(lib_template_unit_tests PUBLIC lib_template::lib_template rt-ut::rt-ut)

set(LT_COMPILE_DEFINITIONS
    $<$<BOOL:${LT_ASAN}>:
        LT_ASAN_ENABLED
    >
    $<$<BOOL:${LT_UBSAN}>:LT_UBSAN_ENABLED>
)

target_compile_definitions(lib_template_unit_tests PUBLIC ${LT_COMPILE_DEFINITIONS})

target_compile_options(lib_template_unit_tests PUBLIC ${LT_COMPILE_OPTIONS})

target_link_options(lib_template_unit_tests PUBLIC ${LT_LINK_OPTIONS})

install(
    FILES
        $<TARGET_FILE:lib_template_unit_tests>
    DESTINATION bin
    OPTIONAL
)