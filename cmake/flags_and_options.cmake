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

set(LT_COMPILE_DEFINITIONS
    "LT_HOST=$<IF:$<CONFIG:Release>,$<IF:$<CXX_COMPILER_ID:MSVC>,[[msvc::forceinline]] inline,inline __attribute__((always_inline))>,$<IF:$<CXX_COMPILER_ID:MSVC>,[[msvc::noinline]],__attribute__((noinline))>>"
    "LT_NOINLINE=$<IF:$<CONFIG:Release>,$<IF:$<CXX_COMPILER_ID:MSVC>,[[msvc::noinline]],__attribute__((noinline))>,$<IF:$<CXX_COMPILER_ID:MSVC>,[[msvc::noinline]],__attribute__((noinline))>>"
    LT_ARCH_ARM64=$<IF:$<OR:$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},aarch64>,$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},ARM64>,$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},arm64>>,1,0>
    LT_ARCH_X64=$<IF:$<OR:$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},x86_64>,$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},AMD64>>,1,0>
    LT_COMPILER_CLANG=$<IF:$<CXX_COMPILER_ID:Clang>,1,0>
    LT_COMPILER_MSVC=$<IF:$<CXX_COMPILER_ID:MSVC>,1,0>
    LT_COMPILER_GNU=$<IF:$<CXX_COMPILER_ID:GNU>,1,0>
    LT_PLATFORM_WINDOWS=$<IF:$<PLATFORM_ID:Windows>,1,0>
    $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX;WIN32_LEAN_AND_MEAN>
    LT_PLATFORM_LINUX=$<IF:$<PLATFORM_ID:Linux>,1,0>
    LT_PLATFORM_MAC=$<IF:$<PLATFORM_ID:Darwin>,1,0>
    LT_DEV=$<IF:$<STREQUAL:${LT_DEV},OFF>,0,1>
    ${LT_SIMD_DEFINITIONS}
)

set(LT_COMPILE_OPTIONS   
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Release>:/O2 /Ob2 /GL /arch:AVX2 /fp:fast /GS- /Gy>
        $<$<CONFIG:Debug>:/Od /Zi /RTC1>
        $<$<BOOL:${LT_ASAN}>:/fsanitize=address>
        /arch:AVX2
    >
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
        $<$<AND:$<CONFIG:Release>,$<NOT:$<BOOL:${LT_ASAN}>>>:
            -O3
            -march=native
            -flto
            -finline-functions
            -fomit-frame-pointer
            -funroll-loops
            -fno-rtti
        >
        $<$<AND:$<CONFIG:Debug>,$<BOOL:${LT_ASAN}>>:
            -fsanitize=address
            -fno-omit-frame-pointer
            -fno-optimize-sibling-calls
            -fsanitize-address-use-after-scope
            -U_FORTIFY_SOURCE
            -D_FORTIFY_SOURCE=0
        >
        $<$<BOOL:${LT_UBSAN}>:
            -fsanitize=undefined
            -fno-sanitize-recover=all
        >
        $<$<AND:$<CONFIG:Release>,$<BOOL:${LT_ASAN}>>:
            -O1
            -fno-rtti
        >
        $<$<CONFIG:Debug>:
            -O0
            -g
            -fno-omit-frame-pointer
        >
    >
    ${LT_SIMD_FLAGS}
)

set(LT_LINK_OPTIONS    
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF>
        $<$<BOOL:${LT_ASAN}>:/INFERASANLIBS>
    >
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
        $<$<AND:$<CONFIG:Release>,$<NOT:$<BOOL:${LT_ASAN}>>>:
            -flto
            $<$<PLATFORM_ID:Darwin>:-Wl,-x>
            $<$<NOT:$<PLATFORM_ID:Darwin>>:-s>
        >
        $<$<AND:$<PLATFORM_ID:Darwin>,$<CXX_COMPILER_ID:GNU>>:
            -L/opt/homebrew/lib/gcc/15
            -Wl,-rpath,/opt/homebrew/lib/gcc/15
        >
        $<$<BOOL:${LT_ASAN}>:
            -fsanitize=address
            $<$<AND:$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:GNU>>:-static-libasan>
        >
        $<$<BOOL:${LT_UBSAN}>:
            -fsanitize=undefined
        >
    >
)