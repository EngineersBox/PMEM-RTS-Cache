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

## Docker Compose

A Dockerfile and compose configuration have been provided as a linux dev env to run the project.
You'll need to build it with `docker compose up -d --build` and then attach to the container.

Once in the container, navigate to `/pmem_rts_cache` where you can run `cmake .` to initialise the project
and `make` to build. Executing the program can be done with `./rts_cache`