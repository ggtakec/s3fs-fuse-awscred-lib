# s3fs-fuse-awscred-lib
[![awscredlib CI](https://github.com/ggtakec/s3fs-fuse-awscred-lib/actions/workflows/ci.yml/badge.svg)](https://github.com/ggtakec/s3fs-fuse-awscred-lib/actions)
[![GitHub license](https://img.shields.io/badge/license-Apache2.0-blue.svg)](https://raw.githubusercontent.com/ggtakec/s3fs-fuse-awscred-lib/master/LICENSE.txt)
[![GitHub forks](https://img.shields.io/github/forks/ggtakec/s3fs-fuse-awscred-lib.svg)](https://github.com/ggtakec/s3fs-fuse-awscred-lib/network)
[![GitHub stars](https://img.shields.io/github/stars/ggtakec/s3fs-fuse-awscred-lib.svg)](https://github.com/ggtakec/s3fs-fuse-awscred-lib/stargazers)
[![GitHub issues](https://img.shields.io/github/issues/ggtakec/s3fs-fuse-awscred-lib.svg)](https://github.com/ggtakec/s3fs-fuse-awscred-lib/issues)

Authentication module using AWS SDK CPP for s3fs

## Overview
`s3fs-fuse-awscred-lib` is a shared library that performs credential processing of [s3fs-fuse](https://github.com/s3fs-fuse/s3fs-fuse/).  

This shared library can be specified with the option(`credlib` and `credlib_opts`) of s3fs-fuse and works by replacing the built-in credential processing of s3fs-fuse.  
This shared library makes use of `aws-sdk-cpp` internally and leaves all S3 credential processing to it.  

## Usage
You can easily build and use `s3fs-fuse-awscred-lib` by following the steps below.  
_See the `.github/workflows/ci.yml` file for build details._  

### Build

#### Build and Install AWS-SDK-CPP on Ubuntu20.04
```
$ sudo apt-get install libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev
$ git clone --recurse-submodules https://github.com/aws/aws-sdk-cpp
$ mkdir sdk_build
$ cd sdk_build
$ cmake ../aws-sdk-cpp -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/home/ggtakec/work/aws-sdk-cpp -DBUILD_ONLY="core;identity-management" -DAUTORUN_UNIT_TESTS=OFF 
$ make
$ sudo make install
```

#### Install AWS-SDK-CPP by brew on macOS
```
$ brew install aws-sdk-cpp
```

#### Build s3fs-fuse-awscred-lib
```
$ git clone git@github.com:ggtakec/s3fs-fuse-awscred-lib.git
$ cd s3fs-fuse-awscred-lib
$ cmake -S . -B build
$ cmake --build build
```
After that, you can find `libs3fsawscred.so` in `build` sub directory.  

### Run s3fs
```
$ s3fs <bucket> <mountpoint> <options...> -o credlib=libs3fsawscred.so -o credlib_opts=Off
```

To specify this `s3fs-fuse-awscred-lib` for s3fs, use the following options:  

#### credlib
An option to specify the `s3fs-fuse-awscred-lib` library.  
You can specify only the library name or the path to the library file.  
The s3fs use `dlopen` to search for the specified `s3fs-fuse-awscred-lib` library and load it.  

Example:  
```
-o credlib=libs3fsawscred.so
```

#### credlib_opts
Specifies the options provided by `s3fs-fuse-awscred-lib`.  
If you specify `s3fs-fuse-awscred-lib`, you can specify the output level of the debug message shown below for this option:  
- Off
- Fatal
- Error
- Warn
- Info
- Debug
- Trace

_These options are the same as the log level defined in `aws-sdk-cpp`(Aws::Utils::Logging::LogLevel)._  

Example:  
```
-o credlib_opts=Info
```
