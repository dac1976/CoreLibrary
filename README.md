# CoreLibrary #
## News ##
2026/04/23 - Now use new ManagedSingleton by default for DebugLog instead of Loki.

2026/04/13 - Bumped version to v2.0.8, since v2.0.5 have reworked the ASIO network class MessageHandler message pool implementation and behaviour. Have also added a new ManagedSingleton class (2 variants - one older C++11 compilers and the other for C++14 or newer compilers). The longer term goal is to remove the all dependencies on Loki's singleton.

2026/03/22 - Added support for MessagePack. The ASIO network classes in CoreLibrary now support this method of (de)serialisation. Version 7.0.0 of MessagePack is included in the CoreLibrary. Bumped the library version to v2.0.5.

2026/03/01 - Added support for Google's flatbuffers. The ASIO network classes in CoreLibrary now support this method of (de)serialisation. Flatbuffer usage is a compile time option for CoreLibrary. This will become release v2.0.2. Also updated doxygen docs to match changes.

2026/02/27 - The big library rework has been useful and, as a result, I've been able to make further improvements to the ASIO classes public interfaces so they now use `std::string_view` instead of `std::string const&` and `std::vector<char> const&` has been replaced by `std::span<const char>`. This allows more flexibility in what a user can pass in for arguments such as addresses and message buffers.

2026/02/25 - Library rework is complete and unit test project and scripts have been updated. All 294 unit test cases are passing. This marks the version 2.0.0 release.

2026/02/22 - A large update to the code is complete. This verison will become 2.0.0 once I've finished getting the unit tests back up and running. CMake support is now included and some example build scripts are provided. Still to come is a CMake based build for the unit tests, currently working on this. Cereal 3.3.1 and Loki 0.1.7 are now included as part of the library, so no external dependencies required for these 2 items. Boost, preferably 1.90 or newer, and optionally, Google Protobuf are external dependencies that the user will have to build themselves.

## Introduction ##
This library (CoreLibrary) is a collection of useful C++1x and C++2x utility classes to assist with multi-threading, logging, networking, custom exceptions, file utilities, INI files, serialization, sorting, string utilities and so on. In other words a general collection of code that is very useful in many different C++ coding projects.

If you find any of this code useful and use it in your software or take parts of it to base your own work on then please give credit and respect the licence.

It is licensed under the terms of LGPL 3.0 and the relevant documentation for this can be found at the top of each source file and in the LICENSE text file.

The code is the work of Duncan Crutchley.

📧 Email me: [email me](mailto:15799155+dac1976@users.noreply.github.com).

Copyright (C) 2014 onwards Duncan Crutchley.

## Requirements ##
This library's code is compatible with modern compilers on Windows and Linux. It has also been used on x86 and ARM architectures. The latest versions of the code requires at least a C++17 compatible compiler but to get the full feature set a C++20 compatible compiler is strongly recommended. On Windows it is tested against MSVC 2022 Build Tools and for Linux it is tested against G++ 11.4 and G++ 14. It has also been tested more recently with Clang 15 and Clang 20, within Embarcadero C++ Builder. The recommended way to build the library is with CMake 3.16 or greater.

Included are the CMakeLists.txt and example build scripts for Linux (build_linux_64bit_release.sh) and Windows (build_msvc2022_64bit_release.bat). You can also manually build th elibrary yourself and use the following CMake options to control the build:

`-DBUILD_SHARED_LIBS=ON` -> (Default)  SHARED (.dll, .so)

`-DBUILD_SHARED_LIBS=OFF` -> STATIC (.lib, .a)

`-DCORELIB_USE_STD_FILESYSTEM=ON` -> (Default) use `std::filesystem`

`-DCORELIB_USE_STD_FILESYSTEM=OFF` -> use `boost::filesystem`

`-DCORELIB_SOCKET_DEBUG=ON` -> Extra debug to log file.

`-DCORELIB_SOCKET_DEBUG=OFF` -> (Default) No extra debug to log.

`-DCORELIB_USE_FLATBUFFERS=ON` -> Enables Google flatbuffer support, requires external dependency.

`-DCORELIB_USE_FLATBUFFERS=OFF` -> (Default) Disables Google flatbuffer support.

`-DCORE_LIB_USE_LOKI=ON` -> Enable Loki for singletons.

`-DCORE_LIB_USE_LOKI=OFF` -> (Default ) Use ManagedSingleton for singletons.

The first time you configure CMake to build this library you must first define the following environment variables:

`CORELIB_BOOST_ROOT` -> Path that is parent to the /boost include directory.

`CORELIB_BOOST_LIB` -> Path to the correct Boost lib folder for your compiler and platform, containing the correct builds of Boost's system, program_options, filesystem and local libraries.

`CORELIB_BOOST_LIB_NAME_STUB` -> On Windows, using MSVC Build Tools, sets the name stub for Boost libraries built in release, e.g. `vc143-mt-x64-1_90`.

`CORELIB_BOOST_LIB_NAME_STUB_D` -> On Windows, using MSVC Build Tools, sets the name stub for Boost libraries built in debug, e.g. `vc143-mt-gd-x64-1_90`.

Note that CMake will cache the environment variables so if you need to change them make sure you clear the CMake cache before rebuilding.

As mentioned earlier, the library requires some third-party open source libraries for it to compile and function. These are as follows:

* Boost (tested with 1.55+ and greater - 1.90 recommended): http://www.boost.org/
* (Included) Cereal (tested with 1.2.1 and greater, 1.3.2 recommended): http://uscilab.github.io/cereal/ - included with CoreLibrary in Include/cereal.
* (Included, Optional) Loki: http://loki-lib.sourceforge.net/ - included with CoreLibrary in Include/loki.
* (Included) MessagePack v7.0.0: https://github.com/msgpack/msgpack-c/tree/cpp_master - included with the CoreLibrary in include/msgpack.
* Google Test (tested with 1.7.0+ or greater): https://github.com/google/googletest
* (Optional) Google Protocol Buffers (tested with 3.7.1 and greater): https://github.com/protocolbuffers/protobuf - only needed if you want to plug-in protocol buffers into the networking classes to serialize messages over-the-wire.
* (Optional) Google flatbuffers Buffers (tested with 25.12.19): https://github.com/google/flatbuffers - only needed if you want to plug-in protocol buffers into the networking classes to serialize messages over-the-wire.

NOTE: The unit test project's example build scripts require vcpkg setup with protobuf and flatbuffers installed using 'vcpkg install protobuf flatbuffers'. The example build scripts also show how you can manually link to specific Boost libraries.

## Notes ##
This is largely a hobby project based on the knowledge I've gained in my many years of experience working in software development and academia since 1999. I started the development of this code in early 2014. This represents a collection of, hopefully, straightforward to use and useful classes for commonly occurring software development needs.

All the code has been unit tested with good coverage and I've used this code in my own projects without issue. I  have used much of this code, or rather versions thereof, in commercial projects in my day job as a Principal Software Engineer, often with (soft) real-time requirements, on Linux, Windows, x86 and ARM. Feel free to use this in commercial projects as long as you adhere to the license.

I'll try to fix bugs and improve the code when necessary but make no guarantees on how often this happens. I'm usually pretty busy with my day job. I provide no warranty or support for any issues that are encountered while using it. Although if you are really stuck email me at the provided address and if I have the time I will try to help or fix the issue if it's within my power to do so

The code is commented using Doxygen style comments so check the documentation in the /docs/html/ folder and open index.html in your browser of choice.

For usage examples I recommend looking at the unit test code in the /UnitTests/GoogleTests folder.

The networking unit tests require network connectivity and expects 2 adapters setup, one on 160.50.0.1/255.255.0.0 and another on 160.51.0.1/255.255.0.0, edit tst_Asiotest.cpp if you want to use different IP addresses. These adapters can be "virtual" loop-back adapters or physical network adapters but to test multicast group and UDP broadcasts the user must configure their firewalls and security settings appropriately.

**See the [wiki](https://github.com/dac1976/CoreLibrary/wiki/Home) for more detailed information and examples...(this is a work in progress)**

Note, the Wiki is not currently up-to-date but the Doxygen genreated doc htmls are.

























