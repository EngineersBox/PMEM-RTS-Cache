# PMEM RTS Cache
A configurable persistent memory Cache designed for real-time search engines

## Building

This project is designed to be used with `pkg-config` to find the relevant pmdk installation. Make sure you have
that installed and configured to find pmdk before building this project.

Once you have pmdk and `pkg-config` installed and configured run the following:

```shell
cmake .
make
./rts_cache <args>
```

## Docker Compose

A Dockerfile and compose configuration have been provided as a linux dev env to run the project.
You'll need to build it with `docker compose up -d --build` and then attach to the container.

Once in the container, navigate to `/pmem_rts_cache` where you can run `cmake .` to initialise the project
and `make` to build. Executing the program can be done with `./rts_cache`

## QEMU Dev Env

You'll need to get and install QEMU from <https://www.qemu.org/download/>. Once you have it installed, you'll need to also
download the Ubuntu server image from <https://ubuntu.com/download/server>.

First, create a ubuntu virtual hard disk image with a desired size, `20G` is a good default

```shell
qemu-img create ubuntu 20G
```

Next, boot the virtual machine with the server image and complete the Ubuntu server installation.
Here `2048MB` of memory is supplied, feel free to change that but generally use a value at or above `640MB`.
Also, note that `nvdimm=on` enables us to interface with PMEM
```shell
qemu-system-x86_64 -hda ubuntu.img -boot d -cdrom ubuntu-20.04.4-desktop-amd64.iso -m 2048M  -machine nvdimm=on
```

Once you are finished with the server setup, go ahead and boot the guest system.

```shell
qemu-system-x86_64 -hda ubuntu.img -m 2048M -machine nvdimm=on
```