# Chip-8 Emulator
Written in C++ By Jacob Reed Blomquist

## Compiling

Use CMAKE to compile.
Navigate to the top-level folder and run the following commands.

```shell
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```
Tested with VisualStudio 2019 and Unix Makefiles on Linux

## Usage

A single argument indicating the path to the ROM file. 
You can simply "Drag-and-drop" a ROM file onto the executable.
(Several ROMS are included)

## Controls

The Original Chip8 Used the key layout of:

```
123C
456D
789E
A0BF
```
This emulator maps these to the left-hand side of the US keyboard:

```
1234
QWER
ASDF
ZXCV
```

Press `ESCAPE` at any time to quit.

