# hif-core

**hif-core** is the shared C++ AST/IR library for the HIF toolchain: it defines the HDL-Independent Format (HIF) data model that every other repo in the toolchain reads, writes, or transforms. It leverages modern C++17 features and integrates with the [Poco](https://pocoproject.org/) libraries for Foundation, Util, and XML support.

Part of the HIF toolchain for HDL-independent-format compilation:
- **hif-core** (this repo) — shared AST/IR library
- [hif-frontend](https://github.com/hif-project/hif-frontend) — Verilog/VHDL → HIF
- [hif-backend](https://github.com/hif-project/hif-backend) — HIF → Verilog/VHDL(/SystemC)
- [hif-muffin](https://github.com/hif-project/hif-muffin) — RTL fault injection, built on the above

![CI](https://github.com/hif-project/hif-core/actions/workflows/ci.yml/badge.svg?branch=develop)

## Features

- Modular C++17 codebase
- Strict compiler warnings (configurable)
- Optional warnings-as-errors
- Integrated code analysis with clang-tidy
- Doxygen documentation with enhanced styling (Doxygen Awesome CSS)

## Requirements

- Linux (only supported/tested platform)
- CMake ≥ 3.1, a C++17 compiler (GCC or Clang)
- Poco libraries (Foundation, Util, XML)
- (Optional) Doxygen for documentation
- (Optional) clang-tidy for static analysis

## Building

```sh
mkdir build && cd build
cmake ..
make
```

### Options

- `STRICT_WARNINGS` (default: ON): Enable strict compiler warnings.
- `WARNINGS_AS_ERRORS` (default: OFF): Treat all warnings as errors.

```sh
cmake -DSTRICT_WARNINGS=ON -DWARNINGS_AS_ERRORS=ON ..
```

### Installing

```sh
make install
```

(Default install prefix is `/usr/local`. Downstream repos in this toolchain don't require an install — they locate a sibling `hif-core` checkout/build directly, see their own READMEs.)

## Running tests

```sh
ctest --test-dir build --output-on-failure
```

## Code analysis

```sh
make hif_clang_tidy       # report
make hif_clang_tidy_fix   # apply automatic fixes
```

## Documentation

If Doxygen is available:

```sh
make hif_documentation
```

## License

BSD 2-Clause. See [LICENSE.md](LICENSE.md).
