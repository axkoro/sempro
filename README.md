test merge

# About This Project

TODO

# Getting Started

Install the package:

```sh
pip install .
```


# Contributing

When making a branch: 
```
git submodule update --init
```

## Testing

First build the project with CMake:

```sh
mkdir build
cd build
cmake -DBUILDTESTS=ON .. 
make
```
On Windows using mingw64: 

```sh
mkdir build
cd build
cmake -G -DBUILDTESTS=ON "MinGW Makefiles" ..
mingw32-make.exe
```

Then run the test you want:
```sh
TODO
```