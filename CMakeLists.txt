cmake_minimum_required(VERSION 3.0.0)
project(watch-playlist VERSION 0.1.0)

add_executable(watch-playlist src/main.cpp src/regex.cpp)

target_include_directories(watch-playlist PUBLIC include)

set_target_properties(watch-playlist
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
SET(PCRE2_BUILD_PCRE2GREP OFF CACHE BOOL "Build PCRE2GREP")
SET(PCRE2_BUILD_TESTS OFF CACHE BOOL "Build TESTS")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -O3")

add_subdirectory(vendor/pcre2)
target_include_directories(watch-playlist PUBLIC build/vendor/pcre2)
IF(MSVC)
target_link_directories(${PROJECT_NAME} PUBLIC pcre2/release)
target_link_libraries(watch-playlist pcre2-8-static pcre2-posix-static)
ELSE(UNIX)
target_link_directories(${PROJECT_NAME} PUBLIC pcre2)
target_link_libraries(watch-playlist pcre2-8 pcre2-posix)
ENDIF()