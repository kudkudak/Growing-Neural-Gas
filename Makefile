#### Growing-Neural-Gas Makefile used for development and by travis #####


## Flags 
CC=g++
CINCLUDE=-O3 -I./inst/include -I./src -I/usr/share/R/include -I/usr/local/lib/R/site-library/Rcpp/include -I/usr/local/lib/R/site-library/RcppArmadillo/include -L/usr/local/lib
CLIBS= -lboost_system -lpthread -lrt -lboost_thread
RFLAGS=$(shell Rscript scripts/generateflags.r)
CFLAGS=-fPIC


#
GTESTFLAGS=-lgtest


## Build directories
BUILD_PERF_DIR := build/performance
BUILD_DEBUG_DIR := build/debug

SRC_DIR := src
INCLUDE_DIR := inst/include
CPP_TEST_DIR := tests/cpp

## Files
CPPFILES := $(filter-out main.cpp, $(foreach dir, $(SRC_DIR)/, $(notdir $(wildcard $(SRC_DIR)/*.cpp))))
OBJFILES := $(addprefix $(BUILD_PERF_DIR)/, $(CPPFILES:.cpp=.o))
DEBUGOBJFILES := $(addprefix $(BUILD_DEBUG_DIR)/, $(CPPFILES:.cpp=.o))

TESTCPPFILES := $(foreach dir, $(CPP_TEST_DIR)/, $(notdir $(wildcard $(CPP_TEST_DIR)/*.cpp)))
TESTOBJFILES := $(addprefix $(CPP_TEST_DIR)/, $(TESTCPPFILES:.cpp=.o))
TESTSOURCES := $(addprefix $(CPP_TEST_DIR)/, $(TESTCPPFILES))

## Automatic dependency injection (from previous build)
DEPFILES := $(addprefix $(BUILD_DEBUG_DIR)/, $(CPPFILES:.cpp=.d))
-include $(DEPFILES)

## Main targets
all: $(OBJFILES)
	$(CC) $(OBJFILES) src/main.cpp -o main  $(CFLAGS) $(CLIBS) $(CINCLUDE) -O3

debug: $(DEBUGOBJFILES)
	$(CC) $(DEBUGOBJFILES) src/main.cpp -o main  $(CFLAGS) $(CLIBS) $(CINCLUDE) -DDEBUG -ggdb -O0


## Helping targets
$(BUILD_PERF_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(CLIBS) $(CINCLUDE) -O3 -MD -MP

$(BUILD_DEBUG_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(CLIBS) $(CINCLUDE) -DDEBUG -ggdb -O0 -MD -MP



## Tests
test: debug
	$(CC) $(DEBUGOBJFILES) tests/cpp/run_tests.cpp -o tests/cpp/run_tests $(CFLAGS) $(CLIBS) $(CINCLUDE) $(GTESTFLAGS) -DDEBUG -ggdb -O0

test_performance: all
	$(CC) $(OBJFILES) tests/cpp/run_tests.cpp -o tests/cpp/run_tests $(CFLAGS) $(CLIBS) $(CINCLUDE) $(GTESTFLAGS) -O3



#$(OBJRFILES)

 #rcpp: all
#	$(CC) $(OBJFILES) -c src/RcppInterface.cpp -o scripts/RcppInterface.o $(RFLAGS) $(CFLAGS) $(CLIBS) $(CINCLUDE)
#	$(CC) $(OBJFILES) -shared -o scripts/RcppInterface.so scripts/RcppInterface.o $(RFLAGS) $(CFLAGS) $(CLIBS) $(CINCLUDE)


## Cleaning
clean:
	rm $(OBJFILES) $(DEPFILES) $(DEBUGOBJFILES) -f

# DO NOT DELETE

src/main.o: src/informal_tests/test_database.cpp -
