cmake_minimum_required(VERSION 3.21)
project(rts_cache C)

set(CC gcc-9.3)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_VERBOSE_MAKEFILE ON)

# ---- OS DETECTION ---- #

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# ---- PKG CONFIG ---- #

if(LINUX)

    find_package(PkgConfig)

    # Create var aliases for libs
    pkg_check_modules(libpmem REQUIRED IMPORTED_TARGET libpmem)
    pkg_check_modules(libpmem2 REQUIRED IMPORTED_TARGET libpmem2)
    pkg_check_modules(libpmemobj REQUIRED IMPORTED_TARGET libpmemobj)

endif()

# ---- SOURCES ---- #

# Include source content
file(GLOB_RECURSE sourceFiles CONFIGURE_DEPENDS "src/*.c")
file(GLOB_RECURSE headerFiles CONFIGURE_DEPENDS "src/*.h")

set(includeDirs "")
foreach(_headerFile ${headerFiles})
    get_filename_component(_dir ${_headerFile} PATH)
    list(APPEND includeDirs ${_dir})
endforeach()
list(REMOVE_DUPLICATES includeDirs)

# Mark executable
add_executable(rts_cache ${sourceFiles})
target_include_directories(rts_cache PRIVATE ${includeDirs})

# ---- LIBS ---- #

if(LINUX)

    # Link libs via aliases
    target_link_libraries(rts_cache PUBLIC PkgConfig::libpmem)
    target_link_libraries(rts_cache PUBLIC PkgConfig::libpmemobj)

else()

    # PMDK headers (for OSX dev, this is compiled on linux)
    target_include_directories(rts_cache PRIVATE "./include")

endif()

# OpenCL
find_package(OpenCL REQUIRED)
target_link_libraries(rts_cache PRIVATE OpenCL::OpenCL)