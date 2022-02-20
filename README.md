# PMEM RTS Cache
A configurable persistent memory cache designed for real-time search engines

## CMake Configuration

In order to build, this project depends on PMDK. You'll need to provide the PMDK lib base location
so that CMake can link the lib properly. The reason for this is that PMDK doesn't have CMake support
out of the box, so it has to be manually linked. Provide the following for CMake:

```shell
-DPMDK_LOCATION="/path/to/lib"
```

## Issues

There is an issue with PMDK with gcc, building produces the following error:

```shell
In file included from /path/to/pmdk/include/libpmem.h:20,
                 from ./src/pmem.c:5:
/path/to/pmdk/include/pmemcompat.h:25:13: error: conflicting types for 'mode_t'; have 'int'
```

At the moment, there is no work around for gcc, so you'll need to use mvsc or clang