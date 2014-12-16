include ../../src/Makevars

# Explicit Makevars inheritances adn overrides
CXX := $(CXX)
CPPFLAGS := $(PKG_CPPFLAGS)
CXXFLAGS := $(PKG_CXXFLAGS) -O2 -g -s
LDLIBS := $(PKG_LIBS) -lpthread -lgtest -lgtest_main

###
# Compilation parameters
###

# Output executable
TEST_MAIN_BIN := run_tests

# Root paths
ROOT_INCLUDE_PATH := ../../inst/include
ROOT_SRC_PATH := ../../src
ROOT_TEST_PATH := .

# Branches that will be compiled
GMUMR_BRANCHES := gng utils
TEST_BRANCHES := gng

# Additional cleaning paths
ADDITIONAL_CLEANING := *.graphml *.bin

# Generate helper scripts for extra tests (run_TESTTYPE_tests.sh)
HELPER_SCRIPT_PATTERN_1 := run_
HELPER_SCRIPT_PATTERN_2 := _tests.sh
PRIMARY_TESTS_NAME := primary
SECONDARY_TESTS_NAME := secondary

# GTest names patterns that should be separated from primary tests
# See: `./run_tests -h | grep -- --gtest_filter`
SECONDARY_TESTS_TYPES := *NumericTest*

###
# Generic variables
###

# Compilation
SOURCES_WILDCARD := $(patsubst %, $(ROOT_SRC_PATH)/%/*.cpp, $(GMUMR_BRANCHES))
TEST_SOURCES_WILDCARD := $(patsubst %, $(ROOT_TEST_PATH)/%/*.cpp, $(TEST_BRANCHES))

SOURCES := $(wildcard $(SOURCES_WILDCARD))
TEST_SOURCES := $(wildcard $(TEST_SOURCES_WILDCARD))

OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))
TEST_OBJECTS := $(patsubst %.cpp, %.o, $(TEST_SOURCES))

# Helper scripts
PRIMARY_TESTS_SCRIPT := $(patsubst %, $(HELPER_SCRIPT_PATTERN_1)%$(HELPER_SCRIPT_PATTERN_2), $(PRIMARY_TESTS_NAME))
SECONDARY_TESTS_SCRIPT := $(patsubst %, $(HELPER_SCRIPT_PATTERN_1)%$(HELPER_SCRIPT_PATTERN_2), $(SECONDARY_TESTS_NAME))

###
# Includes
###

# Root directory (to allow explicit including f.e `#include "svm/svm_basic.h"`)
ROOT_INCLUDE := -I $(ROOT_INCLUDE_PATH)

# Generic branch includes
BRANCH_INCLUDES := $(patsubst %, -I $(ROOT_INCLUDE_PATH)/%, $(GMUMR_BRANCHES))

# Sum up
INCLUDES := $(ROOT_INCLUDE) $(BRANCH_INCLUDES)

###
# Internally included libs
###
SVMLIGHT_INCLUDE := $(ROOT_INCLUDE_PATH)/svmlight
SVMLIGHT_LIBS := $(SVMLIGHT_INCLUDE)/svm_learn.o $(SVMLIGHT_INCLUDE)/svm_common.o $(SVMLIGHT_INCLUDE)/svm_hideo.o

CPPFLAGS := $(CPPFLAGS) $(INCLUDES)

###
# Compilation rules
###

all: $(TEST_MAIN_BIN) helper_scripts
	@echo ... Done!
	@echo See running options with ./run_tests -h
	@echo or use helper scripts: $(PRIMARY_TESTS_SCRIPT) $(SECONDARY_TESTS_SCRIPT)

clean:
	rm -f $(TEST_MAIN_BIN) $(TEST_OBJECTS) $(PRIMARY_TESTS_SCRIPT) \
	$(SECONDARY_TESTS_SCRIPT) $(ADDITIONAL_CLEANING)

$(TEST_MAIN_BIN): $(TEST_OBJECTS) $(OBJECTS)
	@echo Linking all tests...
	$(CXX) $^ -lgtest_main -o $@ $(LDLIBS)

%.o: %.cpp %.hpp
	@echo Compiling $@ ...
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

helper_scripts:
	@echo Making helper scripts...
	make helper_script filename="$(PRIMARY_TESTS_SCRIPT)" pattern=-$(SECONDARY_TESTS_TYPES)
	make helper_script filename="$(SECONDARY_TESTS_SCRIPT)" pattern=$(SECONDARY_TESTS_TYPES)

helper_script:
	$(shell printf "#!/usr/bin/env bash\n./run_tests --gtest_filter=$(pattern)\n" > $(filename))
	$(shell chmod +x $(filename))

