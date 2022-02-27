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
download the Ubuntu server image from <https://ubuntu.com/download/server>, using `Option 2: Manual Installation`.

First, create a raw ubuntu virtual hard disk image with a desired size, `20G` is a good default

```shell
qemu-img create -f raw ubuntu.raw 20G
```

Next, create a Ubuntu guest with required configs. This example creates a Ubuntu guest with two `4GiB` vNVDIMMs, `4GiB` of
DDR Memory, 4 vCPUs, a VNC Server on port `0` for console access, and ssh traffic redirected from port `2222` on the host to
port `22` in the guest for direct ssh access from a remote system.

```shell
qemu-system-x86_64 -drive file=ubuntu.raw,format=raw,index=0,media=disk \
  -m 4G,slots=4,maxmem=32G \
  -smp 4 \
  -machine pc,accel=kvm,nvdimm=on \
  -enable-kvm \
  -vnc :0 \
  -net nic \
  -net user,hostfwd=tcp::2222-:22 \
  -object memory-backend-file,id=mem1,share,mem-path=/virtual-machines/qemu/f27nvdimm0,size=4G \
  -device nvdimm,memdev=mem1,id=nv1,label-size=2M \
  -object memory-backend-file,id=mem2,share,mem-path=/virtual-machines/qemu/f27nvdimm1,size=4G \
  -device nvdimm,memdev=mem2,id=nv2,label-size=2M
```

The first time you do an install, you'll need to provide the serve image to do the initial setup.
It can be provided like so:

```shell
--drive media=cdrom,file=ubuntu-20.04.4-live-server-amd64.iso,readonly
```