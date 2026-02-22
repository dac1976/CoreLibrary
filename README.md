# CoreLibrary #
## News ##
2026/02/22 - Update to the code is complete. CMake support included and some example build scripts. Still to come is a CMake based build for the unit tests.

2026/02/16 - Currently in the middle of doing a refresh to most of the library code. Some things will be broken while I do this work. Some of the highlights are new network/ASIO features. Updated Threading code. Big improvements to the debug logging classes. Support for wider range of Boost versions from about 1.55 up to the lastest (1.90), bug fixes and genreal tweaks. Also including CMake support.

2020/05/04 - A bit of code tidying and refactoring and removal of warnings flagged by certain compilers. Over last few months I've moved to newer Boost due to updates to Boost ASIO code.

2019/08/29 - Added extra string utility functions.

2019/04/11 - Added support for Google Protocol Buffers in serialization utilities. Therefore support automatically achieved in all ASIO classes, such as TCP, UDP and multicast classes.

## Introduction ##
This library (CoreLibrary) is a collection of useful C++1x utility classes to assist with multi-threading, logging, networking, custom exceptions, file utilities, INI files, serialization, sorting, string utilities and so on. In other words a general collection of code that is very useful in many different C++ coding projects.

If you find any of this code useful and use it in your software or take parts of it to base your own work on then please give credit and respect the licence.

It is licensed under the terms of LGPL 3.0 and the relevant documentation for this can be found at the top of each source file and in the LICENSE text file.

The code is the work of me (Duncan Crutchley) (<dac1976github@outlook.com>).

Copyright (C) 2014 onwards Duncan Crutchley.

## Requirements ##
This library's code is compatible with modern compilers on Windows and Linux. It has also been used on x86 and ARM architectures. The latest versions of the code requires at least a C++17 compatible compiler but to get the full feature set a C++20 compatible compiler is strongly recommended. On Windows it is tested against MSVC 2022 Build Tools and for Linux it is tested against G++ 11 and greater. It has also been tested more recently with Clang 15 and Clang 20. The recommended way to build the library is with CMake 3.16 or greater.

Use the following CMake options to control the build:

-DBUILD_SHARED_LIBS=ON -> (Default)  SHARED (.dll, .so)

-DBUILD_SHARED_LIBS=OFF -> STATIC (.lib, .a)

-DCORELIB_USE_STD_FILESYSTEM=ON -> (Default) use std::filesystem

-DCORELIB_USE_STD_FILESYSTEM=OFF -> use boost::filesystem

-DCORELIB_SOCKET_DEBUG=ON -> Extra debug to log file.

-DCORELIB_SOCKET_DEBUG=OFF -> (Default) No extra debug to log.

The first time you configure CMake to build this library you must first define the following environment variables:

CORELIB_BOOST_ROOT -> Path that is parent to the /boost include directory.

CORELIB_BOOST_LIB -> Path to the correct Boost lib folder for your compiler and platform.

CORELIB_CEREAL_ROOT -> Path (inclusive) for the Cereal /include directory.

CORELIB_LOKI_ROOT -> Path (inclusive) for the Loki /include directory.

CORELIB_BOOST_LIB_NAME_STUB -> On Windows using MSVC Build Tools sets the name stub for Boost libraries built in release, e.g. vc143-mt-x64-1_90.

CORELIB_BOOST_LIB_NAME_STUB -> On Windows using MSVC Build Tools sets the name stub for Boost libraries built in debug, e.g. vc143-mt-gd-x64-1_90.

Note that CMake will cache the environment variables so if you need to change them make sure you clear the CMake cache before rebuilding.

This library requires some third-party open source libraries for it to compile and function. These are as follows: 

* Boost (tested with 1.55+ or greater - 1.90 recommended): http://www.boost.org/
* Cereal (tested with 1.2.1+ or greater): http://uscilab.github.io/cereal/
* Loki: http://loki-lib.sourceforge.net/
* Google Test (tested with 1.7.0+ or greater): https://github.com/google/googletest/graphs/contributors
* Google Protocol Buffers (tested with 3.7.1 or greater): https://developers.google.com/protocol-buffers/ 

## Notes ##
This is largely a hobby project based on the knowledge I've gained in my many years of experience working in software development and academia since 1999. I started the development of this code in early 2014. This represents a collection of, hopefully, straightforward to use and useful classes for commonly occurring software development needs.

All the code has been unit tested with good coverage and I've used this code in my own projects without issue. I also have used much of this code and similar code written by me in commercial projects, often with (soft) real-time requirements, and have not had any issues so feel free to use this in commercial projects as long as you adhere to the license.

I fix bugs and improve the code when necessary but make no guarantees on how often this happens. I provide no warranty or support for any issues that are encountered while using it. Although if you are really stuck email me at the provided address and if I have the time I will try to help or fix the issue if it's within my power to do so

The code is commented using Doxygen style comments so check the documentation in the /docs/html/ folder and open index.html in your browser of choice.

For usage examples I recommend looking at the unit test code in the /UnitTests/GoogleTests folder.

The networking unit tests require network connectivity and expects 2 adapters setup, one on 10.34.6.1/255.255.0.0 and another on 10.35.6.1/255.255.0.0. These can be "virtual" loop-back adapters or physical network adapters but to test multicast group and UDP broadcasts the user must configure their firewalls and security settings appropriately.

**See the [wiki](https://github.com/dac1976/CoreLibrary/wiki/Home) for more detailed information and examples.**










