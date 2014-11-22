C++ Tests
=========

This directory contains C++ code tests.

### GoogleTest

In order to run these tests you need to acquire **GoogleTest** sources.

```
wget http://googletest.googlecode.com/files/gtest-1.7.0.zip
uzip gtest-1.7.0.zip
mv gtest-1.7.0 gtest
```

### Dependencies

All dependencies should be resolved after installing package `R CMD INSTALL .` in root folder

## Build

In order to build tests `cd` into this directory (`tests/cpp`) and type `GTEST=gtest make`.

## Run tests

All test cases are separate executable files.

