# hif-core

## Overview

**hif-core** is a C++ library designed for hardware interface modeling and related utilities. It leverages modern C++17 features and integrates with the [Poco](https://pocoproject.org/) libraries for Foundation, Util, and XML support.

## Features

- Modular C++17 codebase
- Strict compiler warnings (configurable)
- Optional warnings-as-errors
- Integrated code analysis with clang-tidy
- Doxygen documentation with enhanced styling (Doxygen Awesome CSS)
- Easy installation and integration

## Build Instructions

### Prerequisites

- CMake ≥ 3.1
- C++17 compatible compiler (GCC, Clang, or MSVC)
- Poco libraries (Foundation, Util, XML)
- (Optional) Doxygen for documentation
- (Optional) clang-tidy for static analysis

### Building the Library

```sh
mkdir build
cd build
cmake ..
make
```

#### Options

- `STRICT_WARNINGS` (default: ON): Enable strict compiler warnings.
- `WARNINGS_AS_ERRORS` (default: OFF): Treat all warnings as errors.

You can set these options via CMake:

```sh
cmake -DSTRICT_WARNINGS=ON -DWARNINGS_AS_ERRORS=ON ..
```

### Installing

```sh
make install
```

(Default install prefix is `/usr/local`.)

## Code Analysis

To run clang-tidy:

```sh
make hif_clang_tidy
```

To apply automatic fixes:

```sh
make hif_clang_tidy_fix
```

## Documentation

If Doxygen is available, generate documentation with:

```sh
make hif_documentation
```

## License

See [LICENSE.md](LICENSE.md).
