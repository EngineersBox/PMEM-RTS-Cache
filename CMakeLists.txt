cmake_minimum_required(VERSION 3.21)
project(PMEM_mmap_IO C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_VERBOSE_MAKEFILE ON)

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
add_executable(PMEM_mmap_IO ${sourceFiles})
target_include_directories(PMEM_mmap_IO PRIVATE ${includeDirs})

# ---- LIBS ---- #

# PMDK
target_include_directories(PMEM_mmap_IO PRIVATE "${PMDK_LOCATION}/include")
target_link_libraries(
        PMEM_mmap_IO
        PRIVATE
        "${PMDK_LOCATION}/lib/libpmem.lib"
        "${PMDK_LOCATION}/lib/libpmemblk.lib"
        "${PMDK_LOCATION}/lib/libpmemlog.lib"
        "${PMDK_LOCATION}/lib/libpmemobj.lib"
        "${PMDK_LOCATION}/lib/libpmempool.lib"
)