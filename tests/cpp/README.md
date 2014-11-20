C++ Tests
=========

This directory contains C++ code tests.

## Dependencies

### GoogleTest

In order to run these tests you need to acquire **GoogleTest** sources.

Google Code: [http://code.google.com/p/googletest/downloads/list](http://code.google.com/p/googletest/downloads/list)

After unpacking the sources, write path to its directory (for example in `~/.bashrc`)
under environment variable `GTEST`.

### GNG dependencies

C++ **Armadillo** library and its dependencies ( **Lapack**, **Blas**) need to be installed.

## Build

In order to build tests `cd` into this directory (`tests/cpp`) and type `make`.

## Run tests

All test cases are separate executable files. For example:

