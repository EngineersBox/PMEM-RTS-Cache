# PMEM RTS Cache
A configurable persistent memory cache designed for real-time search engines

## Building

This project is designed to be used with `pkg-config` to find the relevant pmdk installation. Make sure you have
that installed and configured to find pmdk before building this project.

Once you have pmdk and `pkg-config` installed and configured run the following:

```shell
cmake .
make
./PMEM_mmap_IO <args>
```