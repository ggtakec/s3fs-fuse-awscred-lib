# Helpers for building s3fs-fuse-awscred-lib

The tools in this directory can be used to build the library locally, so it can either
be installed manually or just used from the current directory.

## Requirements:

 - Linux
 - Docker, Podman or an alternative container runtime
 - GNU Make (optional, read `Makefile` for alternative instructions)

## Usage

The provided `Makefile` currently supports building for the currently supported
Ubuntu LTS versions.

To build all the required binaries, including the test tool, and copy them into the
current directory, run:

`make -f contrib/Makefile [UBUNTU_VERSION]`

where `UBUNTU_VERSION` is optional and can be an LTS version number such as `20.04`.
If not specified, `22.04` is used (which is the current latest LTS release).

After the build is complete, the resulting `.so` files can be manually copied to
someplace in the dynamic linker path (for example `/usr/local/lib`) or can be
loaded directly by setting the environment variable `LD_LIBRARY_PATH` for the
`s3fs` command, e.g. `LD_LIBRARY_PATH=. s3fs …`
