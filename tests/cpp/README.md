C++ Tests
=========

This directory contains C++ code tests.

## Dependencies

### GoogleTest

To compile these tests you need to install **GoogleTest** >= 1.6.0.

Arch Linux package: `gtest`, Debian/Ubuntu package: `libgtest-dev`

Ensure that GTest library has been installed:

`ldconfig -p | grep gtest`

If there is no output, you've just installed sources. You must compile them
into a library.

Example for Ubuntu:

```
# Install CMake if you do not have one
sudo apt-get install cmake

# Compile GTest
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make

# Copy libgtest.a and libgtest_main.a to your /usr/lib
sudo cp *.a /usr/lib
```

### GMUM.R dependencies

C++ **Armadillo** library and its dependencies ( **Lapack**, **Blas**) need to
be installed.

## Build

To build tests `cd` into this directory (`tests/cpp`) and type `make`.

## Run tests

Run compiled executable binary: `./run_tests`

Big data tests require external input files, run `big_data_downloader.sh`
to download them.

To run tests individually, use helper `./*.sh` scripts or refer to
`./run_tests -h` from GTest.

Helper scripts:

* `run_primary_tests.sh` runs primary coverage tests used in CI
* `run_secondary_tests.sh` runs long-lasting tests for f.e. validation of
results of the computations

## Adding tests

Add new tests as `*.cc` files in subdirectories. You can create a new directory
for your own branch/type of tests. To compile new directory, add its name to
Makefile `TEST_BRANCHES` variable.

## Duration of the tests

Please consider adding your test to **secondary tests** if your test usually
takes more than a minute. You can do that by giving it a name containing
`NumericTest` string or define your own type of test and append its pattern to
`SECONDARY_TESTS_TYPES` variable (list) in Makefile. This will include them to
secondary tests during the compilation.

## Disabling tests

To disable single test case, prefix it with a `DISABLE_` string.

To disable file, rename its extension. For example from `*.cpp` to `*.xcpp`.

We want to disable tests when they are for example no longer compatible with
current architecture or in any other way are blockers for
continous integration. Disabled tests can be fixed later, while continous
integration still goes good.

