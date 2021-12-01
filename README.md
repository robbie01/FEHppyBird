# Software Design Project ENGR 1281.02H 2021

## Todo
* Create menu
* Create partial website
* Add regenerating pipe
* Add multiple pipes

[Flowchart](https://www.figma.com/file/E5ZmUvbhixMxcTwcUR0In5/SDP-21-Flowchart?node-id=0%3A1)

## Setup
To get libraries submodule:

    git submodule update --recursive --init

Note: this is done automatically by the Makefile

## Prerequisites

### Windows & macOS

Cross your fingers

### Fedora

    # dnf install mesa-libGLU-devel

### Ubuntu

Idk i dont use it

### Void

    # xbps-install glu-devel

## Building

### On Windows

    mingw32-make.exe

### On macOS

It's probably just `make`

### On Linux

    make -f Makefile.linux
