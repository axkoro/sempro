# About This Project

TODO

# Getting Started

## Building

To build the project, use CMake:

```sh
mkdir build
cd build
cmake ..
make
```
On Windows using mingw64: 

```sh
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make.exe
```

When making a branch: 
```
git submodule update --init
```

## Testing

To run unit tests, execute the following  after building:

```sh
./build/main_testing
```