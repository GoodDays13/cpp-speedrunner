# C++ Speedrunner
A personal project of a game where you can change the speed of time to beat levels quicker
## Compile
### Dependencies:
- CMake
- glslc
- xxd
### Process:
Just the typical process for compiling and installing CMake
The default prefix is build/install
```
mkdir build
cd build
cmake ..
cmake --build .
cmake --install .
install/speedrunner
```
A `speedrunner` executable will be in build, but this is an intermediate. The final is in install.
