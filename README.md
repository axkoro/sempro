# About This Project

TODO

# Getting Started

Install the package (by being located in the repo folder):

```sh
pip install .
```

You can now use the modules (_graph_, _strats_) directly or run the evaluation or benchmark scripts.
To run those scripts you need to be inside the root folder of the repo. Then you can run either `evaluate` or `benchmark` from the terminal.


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
cmake -G  "MinGW Makefiles" -DBUILDTESTS=ON ..
mingw32-make.exe
```

Then run the test you want, such as `./KNNImputerTest`.