# Chip-8 Emulator
Written in C++ By Jacob Reed Blomquist

## Compiling

Use CMAKE to compile.

`mkdir build`
`cd build`
`cmake ..`
`cmake --build .`

## Usage

A single argument indicating the path to the ROM file. 
(Several ROMS are included)

## Controls

The Original Chip8 Used the a layout of:

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

