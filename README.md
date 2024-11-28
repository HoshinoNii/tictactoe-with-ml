# tictactoe-with-ml

# Compilation Guide

This compilation guide is to assist anyone who wishes to compile the code and run it on their own.

## Prerequisites

To ensure a smooth compilation of the code, do download the required prerequisites as stated below:
1) Raylib
2) GCC
3) CMake
4) Make

## Windows (x64) – Raylib Installer Method

1) Open “C:\raylib\w64devkit\w64devkit.exe"
2) Using the terminal, navigate to the project folder (.\tictactoe\..)
3) Run the following command: “gcc -o main main.c -lraylib -lgdi32 -lwinmm”
4) Congratulations, you are ready to go!

## Windows (x64) – Manual Build Method

1) Important Clone the Raylib library outside csc1104-3-assignment, in this format
   1.1 ./raylib
   1.2 ./tictactoe-with-ml
   git clone https://github.com/raysan5/raylib.git

2) Install make
   winget install make

3) Navigate to ./raylib/src
   cd raylib/src

4) Run make in ./raylib/src
   make

5) Once completed, navigate to csc1104-3-assignment and run make
   make

6) Thereafter, it will compile the game and put it into the /build/ folder

## Raspberry PI 4 (ARM64)

1) Install the following packages beforehand (you can just run the commands in shell)
   sudo apt-get install libraspberrypi-dev raspberrypi-kernel-headers
   sudo apt-get install libegl1-mesa libegl1-mesa-dev mesa-common-dev libgles2-mesa libgles2-mesa-dev

2) Clone the Raylib library into ~/Projects/ folder
   git clone https://github.com/raysan5/raylib.git

3) Navigate to ./raylib/src in the terminal
   cd /raylib/src

4) Compile Raylib by running the following command into the terminal
   make PLATFORM=PLATFORM_DESKTOP  GRAPHICS=GRAPHICS_API_OPENGL_21

5) Once Raylib has been compiled, structure your project and app folder this way with the following commands
   ./raylib/src
   ./csc1104-3-assignment

6) Run the following commands to compile the game in RP4
   make

7) Once compilation is completed, the game is ready to run. Have fun!
