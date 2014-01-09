#### Growing-Neural-Gas Makefile used for development and by travis #####


## Flags 
CC=g++
CINCLUDE=-I./include -I./src -I/usr/share/R/include -I/usr/local/lib/R/site-library/Rcpp/include -I/usr/local/lib/R/site-library/RcppArmadillo/include -L/usr/local/lib
CLIBS= -lboost_system -lpthread -lrt -lboost_thread
RFLAGS=$(shell Rscript scripts/generateflags.r)
CFLAGS=-fPIC -DDEBUG -MD -MP


#
GTESTFLAGS=-lgtest


## Build directories
BUILD_DIR := build/performance
SRC_DIR := src
INCLUDE_DIR := include
CPP_TEST_DIR := tests/cpp

## Files
CPPFILES := $(filter-out main.cpp, $(foreach dir, $(SRC_DIR)/, $(notdir $(wildcard $(SRC_DIR)/*.cpp))))
OBJFILES := $(addprefix $(BUILD_DIR)/, $(CPPFILES:.cpp=.o))

TESTCPPFILES := $(foreach dir, $(CPP_TEST_DIR)/, $(notdir $(wildcard $(CPP_TEST_DIR)/*.cpp)))
TESTOBJFILES := $(addprefix $(CPP_TEST_DIR)/, $(TESTCPPFILES:.cpp=.o))
TESTSOURCES := $(addprefix $(CPP_TEST_DIR)/, $(TESTCPPFILES))

DEPFILES := $(addprefix $(BUILD_DIR)/, $(CPPFILES:.cpp=.d))



all: $(OBJFILES) main

main: $(OBJFILES)
	$(CC) $(OBJFILES) src/main.cpp -o main  $(CFLAGS) $(CLIBS) $(CINCLUDE) 

$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(CLIBS) $(CINCLUDE)


# Automatic dependency injection
-include $(DEPFILES)


#test: $(TESTOBJFILES) all

#tests/cpp/%.o: tests/cpp/%.cpp
#	$(CC) $(OBJFILES) -c $< -o $@ $(CFLAGS) -l$(SRC_DIR) $(CLIBS) $(CINCLUDE) $(GTESTFLAGS)

test: all
	$(CC) $(OBJFILES) tests/cpp/run_tests.cpp -o tests/cpp/run_tests $(CFLAGS) $(CLIBS) $(CINCLUDE) $(GTESTFLAGS) 


#$(OBJRFILES)

 #rcpp: all
#	$(CC) $(OBJFILES) -c src/RcppInterface.cpp -o scripts/RcppInterface.o $(RFLAGS) $(CFLAGS) $(CLIBS) $(CINCLUDE)
#	$(CC) $(OBJFILES) -shared -o scripts/RcppInterface.so scripts/RcppInterface.o $(RFLAGS) $(CFLAGS) $(CLIBS) $(CINCLUDE)


clean:
	rm $(OBJFILES) $(DEPFILES) -f

# DO NOT DELETE

src/main.o: src/informal_tests/test_database.cpp -
