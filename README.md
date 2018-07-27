# CoreLibrary #
## Introduction ##
This library (CoreLibrary) is a collection of useful C++1x utility classes to assist with multi-threading, logging, networking, custom exceptions, file utilities, INI files, serialization, sorting, string utilities and so on. In other words a general collectio nof code that is very useful in many different C++ coding projects.

If you find any of this code useful and use it in your software or take parts of it to base your own work on then please give credit and respect the licence.

It is licensed under the terms of LGPL 3.0 and the relevant documentation for this can be found at the top of each source file and in the LICENSE text file.

The code is the work of me (Duncan Crutchley) (<dac1976github@outlook.com>).

Copyright (C) 2014 onwards Duncan Crutchley.

## Requirements ##
This library has been developed in Qt Creator for Windows and Linux. In Windows it is recommended to use the MSVC2015 or MSVC2017 tool chain from within Qt Creator to build and test the code. However, other compilers have been tested in Windows such as Clang 3.8+ and GCC (MinGW) 4.8.X+. In Linux the code has been tested with GCC 4.8.X+ and Clang 3.8+. I always try to use the newest stable version of any compiler that I build this code with. So as such I won't guarantee I'll maintain support for older compilers as I move my code on with improvements and new features.

On Windows (Win10 64bit) I am currently using MSVC 2017 64bit kit in Qt Creator 4.7.0/ Qt Framework 5.11.11. On Linux (Manjaro) I am currently using GCC 8.1.1 and Clang 6.0.1 kits in Qt Creator 4.7.0 Qt Framework 5.11.1.

The projects supplied for the library and unit tests are for Qt Creator and will work in Linux and Windows. To achieve this the projects rely on the custom mkspecs to be copied from the qt_custom_mkspecs folder into Qt's appropriate mkspec feature folder, e.g. <...>\Qt\5.8\msvc2015_64\mkspecs\features. Example mkspecs are provided for
compatibility with MSVC2015/MSVC2017 32 bit and 64bit and Linux. These should be customized for the user's own system.

You may use this code within other development and build environments but it is up to the user to create the required projects or makefiles for those environments. This should be fairly easy by referring to the Qt project (.pro) file and the previously mentioned mkspec files.

This library requires some third-party open source libraries for it to compile and function. These are as follows:

* Boost (tested with 1.57+ but newest version recommended): http://www.boost.org/
* Cereal (tested with 1.2.1+ but newest version recommended): http://uscilab.github.io/cereal/
* Loki: http://loki-lib.sourceforge.net/
* Google Test (tested with 1.7.0+, but newest recommended): https://github.com/google/googletest/graphs/contributors

As with compilers I always keep my copies of the above thrid-party libraries up-to-date with their latest stable versions so I won't guarantee full support for older versins of these libraries as I continue work on this project.

## Notes ##
This is largely a hobby project based on the knowledge I've gained in my many years of experience working in software development and academia since 1999. I started the development of this code in early 2014. This represents a collection of, hopefully, straightforward to use and useful classes for commonly occurring software development needs.

All the code has been unit tested with good coverage and I've used this code in my own projects without issue. I also have used much of this code and similar code written by me in commercial projects, often with (soft) real-time requirements, and have not had any issues so feel free to use this in commercial projects as long as you adhere to the license.

I fix bugs and improve the code when necessary but make no guarantees on how often this happens. I provide no warranty or support for any issues that are encountered while using it. Although if you are really stuck email me at the provided address and if I have the time I will try to help or fix the issue if it's within my power to do so

The code is commented using Doxygen style comments so check the documentation in the /docs/html/ folder and open index.html in your browser of choice.

For usage examples I recommend looking at the unit test code in the /UnitTests/GoogleTests folder.

The networking unit tests require network connectivity and expects 2 adapters setup, one on 160.50.100.76/255.255.0.0 and another on 160.51.100.76/255.255.0.0. These can be "virtual" loop-back adapters or physical network adapters but to test multicast group and UDP broadcasts the user must configure their firewalls and security settings appropriately.