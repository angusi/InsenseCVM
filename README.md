An Insense Virtual Machine in C
===============================

TL;DR
--------
* *Insense* is a programming language for developing Wireless Sensor Network applications. For more information, see [insense.cs.st-andrews.ac.uk](http://insense.cs.st-andrews.ac.uk).
* The *Insense Virtual Machine in C* is a virtual machine for running Insense Programs compiled with the *Insense Bytecode Compiler*, available separately. Currently, only Linux systems are supported as VM hosts.

Introduction
----------------
Wireless Sensor Networks, or WSNs, are composed of a number of independent sensor nodes, also known as "motes", designed to sense changes in environmental conditions, such as air and water temperature or pressure, or acceleration in a given direction through an accelerometer. These motes are commonly small, "embedded" computer systems, each of which are typically highly resource constrained in terms of memory, processing capacity and power available for consumption.

The users of Wireless Sensor Networks and their constituent devices often have a primary interest and expertise in a field other than computer programming, and thus may experience difficulty in programming these devices correctly and with maximum efficiency. The Insense language was developed to facilitate application development for wireless sensor networks. It abstracts many of the more difficult aspects of programming for constrained wireless sensor systems whilst still providing a powerful computation platform for highly specialised interests.

This project sought to implement a virtual machine for running Insense programs. This virtual machine would run Insense programs compiled to an intermediary bytecode format by a separate, pre-existing compiler. Using
a bytecode format provides a platform-independent method of storing the program code, with lower storage requirements as compared to Insense source code. The bytecode files are also smaller than fully compiled Insense programs after they are linked with the Insense Runtime Library, making them more efficient to transfer between motes over radio links. It is also easier for a irtual machine to switch to executing new byte code than it is to launch a new ELF binary received.

An existing bytecode compiler already exists, along with a virtual machine for running the bytecode produced by this compiler. However, this virtual machine is written in the Java programming language. The Java
Runtime Environment requires a far greater quantity of storage space and memory than is available from a typical sensor mote, such as the *TMote Sky*, a device with just 48kB of storage space and 10kB of RAM. For this reason, the virtual machine was to be written in the relatively lower level C programming language, providing fast and efficient computation on the target platform.

Building and Running the VM
---------------------------------------

The virtual machine is built with CMake.
To build on *nix, create the ./bin directory, cd into it and run:

    $ cmake ../
    $ make


A number of options can be specified on the command line to change build options:

    -DDEBUGGINGENABLED:BOOL=[TRUE|FALSE]  Enable debug output (default: FALSE)
    -DTARGET:STRING=Linux                 Compile for Linux (default: Linux)

Alternatively, to set options interactively, run

    $ ccmake ../
    $ make

To run the virtual machine:

    $ ./CVM /path/to/bytecode/directory

The log level can optionally be set:

    -l [DEBUG|INFO|WARNING|ERROR|FATAL]  (default: INFO)

A number of precompiled programs are provided in the ./InsensePrograms directory.

Further Reading & Resources
---------------------------------------
* Insense's home page @ [insense.cs.st-andrews.ac.uk](http://insense.cs.st-andrews.ac.uk)

