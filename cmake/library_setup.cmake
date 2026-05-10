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

option(LT_ASAN "Enable AddressSanitizer" OFF)
option(LT_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

include(cmake/detection/lt_detect_cpu_properties.cmake)
include(cmake/flags_and_options.cmake)

if(APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(LT_ASAN)
        message(WARNING "LT_ASAN is not supported with GCC on macOS -- disabling.")
        set(LT_ASAN OFF CACHE BOOL "" FORCE)
    endif()
    if(LT_UBSAN)
        message(WARNING "LT_UBSAN is not supported with GCC on macOS -- disabling.")
        set(LT_UBSAN OFF CACHE BOOL "" FORCE)
    endif()
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_options(${PROJECT_NAME}
    INTERFACE ${LT_COMPILE_OPTIONS}
)

target_link_options(${PROJECT_NAME}
    INTERFACE ${LT_LINK_OPTIONS}
)

target_compile_definitions(${PROJECT_NAME}
    INTERFACE ${LT_COMPILE_DEFINITIONS}
)