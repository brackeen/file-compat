cmake_minimum_required(VERSION 3.22.1) # Version supported on Ubuntu 22

project(file_compat_test)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11) # Android needs C++11 for __VA_ARGS__

if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(file_compat_flags_c "-Weverything -Wwrite-strings -Wno-used-but-marked-unused -Wno-declaration-after-statement -Wno-unknown-warning-option -Wno-unsafe-buffer-usage -Wno-c++98-compat -Wno-c++98-compat-pedantic -Werror")
    set(file_compat_flags_cxx "-fno-rtti -fno-exceptions ${file_compat_flags_c}")
    set(file_compat_flags_objc "-x objective-c -fno-objc-arc ${file_compat_flags_c}")
    set(file_compat_flags_objc_arc "-x objective-c -fobjc-arc ${file_compat_flags_c}")
    set(file_compat_flags_objcxx "-x objective-c++ -fno-objc-arc ${file_compat_flags_cxx}")
    set(file_compat_flags_objcxx_arc "-x objective-c++ -fobjc-arc ${file_compat_flags_cxx}")
elseif (CMAKE_C_COMPILER_ID MATCHES "GNU")
    set(file_compat_flags_c "-Wall -Wextra -Wwrite-strings -Werror")
    set(file_compat_flags_cxx "-fno-rtti -fno-exceptions ${file_compat_flags_c}")
elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
    string(REGEX REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(file_compat_flags_c "/Wall /WX")
    set(file_compat_flags_cxx "/Zc:__cplusplus ${file_compat_flags_c}")
else()
    set(file_compat_flags_c)
    set(file_compat_flags_cxx)
endif()

if (CMAKE_SYSTEM_NAME MATCHES "Emscripten")
    set(file_compat_libraries "-lidbfs.js")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(file_compat_libraries "-framework Foundation")
elseif (CMAKE_SYSTEM_NAME MATCHES "Android")
    find_library(log_lib log)
    find_library(android_lib android)
    set(file_compat_libraries ${log_lib} ${android_lib})
else()
    set(file_compat_libraries)
endif()

include_directories(..)
enable_testing()

macro(file_compat_add_test target files flags)
    if (CMAKE_SYSTEM_NAME MATCHES "Android")
        add_library(${target} SHARED ${files})
    else()
        add_executable(${target} ${files})
    endif()
    set_target_properties(${target} PROPERTIES COMPILE_FLAGS ${flags})
    target_link_libraries(${target} ${file_compat_libraries})
    add_test(NAME ${target} COMMAND ${target})
endmacro()

file_compat_add_test(file_compat_test_c "test.c" ${file_compat_flags_c})
file_compat_add_test(file_compat_test_cxx "test.cxx" ${file_compat_flags_cxx})

if ((CMAKE_SYSTEM_NAME MATCHES "Darwin") AND (CMAKE_C_COMPILER_ID MATCHES "Clang"))
    file_compat_add_test(file_compat_test_objc "test.c" ${file_compat_flags_objc})
    file_compat_add_test(file_compat_test_objc_arc "test.c" ${file_compat_flags_objc_arc})
    file_compat_add_test(file_compat_test_objcxx "test.cxx" ${file_compat_flags_objcxx})
    file_compat_add_test(file_compat_test_objcxx_arc "test.cxx" ${file_compat_flags_objcxx_arc})
endif()
