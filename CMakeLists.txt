cmake_minimum_required(VERSION 3.12)
project(LASUScreeningServer LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Add executable
add_executable(lasu_screening_server screen.cpp)

# Set C++ properties
set_property(TARGET lasu_screening_server PROPERTY CXX_STANDARD 17)
set_property(TARGET lasu_screening_server PROPERTY CXX_STANDARD_REQUIRED ON)

# Link libraries
target_link_libraries(lasu_screening_server 
    Threads::Threads
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(lasu_screening_server ws2_32)
    target_compile_definitions(lasu_screening_server PRIVATE _WIN32_WINNT=0x0601)
elseif(UNIX)
    # Linux/Unix specific settings if needed
    target_compile_options(lasu_screening_server PRIVATE -Wall -Wextra)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(lasu_screening_server PRIVATE
        -O3
        -Wall
        -Wextra
        -Wpedantic
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(lasu_screening_server PRIVATE
        /O2
        /W4
    )
endif()

# Set output directory
set_target_properties(lasu_screening_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Create output directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Optional: Add install target
install(TARGETS lasu_screening_server
    RUNTIME DESTINATION bin
    COMPONENT Runtime
)

# Optional: Set debug/release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(lasu_screening_server PRIVATE DEBUG=1)
    if(NOT WIN32)
        target_compile_options(lasu_screening_server PRIVATE -g)
    endif()
else()
    target_compile_definitions(lasu_screening_server PRIVATE NDEBUG=1)
endif()

# Print configuration info
message(STATUS "Building LASU Screening HTTP Server")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Output Directory: ${CMAKE_BINARY_DIR}/bin")
