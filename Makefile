#### Growing-Neural-Gas Makefile used for development and by travis #####


## Flags 
CC=g++
CINCLUDE=-I./include -I./src -I/usr/share/R/include -I/usr/local/lib/R/site-library/Rcpp/include -I/usr/local/lib/R/site-library/RcppArmadillo/include
CLIBS= -lboost_system -lpthread -lrt -lboost_thread
RFLAGS=$(shell Rscript scripts/generateflags.r)
CFLAGS=-fPIC -DDEBUG -MD -MP

## Build directories
BUILD_DIR := build/performance
SRC_DIR := src
INCLUDE_DIR := include

## Files
CPPFILES := $(foreach dir, $(SRC_DIR)/, $(notdir $(wildcard $(SRC_DIR)/*.cpp)))
OBJFILES := $(addprefix $(BUILD_DIR)/, $(CPPFILES:.cpp=.o))
SOURCES := $(addprefix $(SRC_DIR)/, $(CPPFILES:.cpp=.o))
DEPFILES := $(addprefix $(BUILD_DIR)/, $(CPPFILES:.cpp=.d))



all: $(OBJFILES) main

main: $(OBJFILES)
	$(CC) $(OBJFILES) -o main  $(CFLAGS) $(CLIBS) $(CINCLUDE) 

$(BUILD_DIR)/%.o:$(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) -l$(SRC_DIR) $(CLIBS) $(CINCLUDE)


# Automatic dependency injection
-include $(DEPFILES)


test:
	echo $(DEPFILES)
	echo $(CPPFILES)
	echo $(OBJFILES)


#$(OBJRFILES)

 #rcpp: all
#	$(CC) $(OBJFILES) -c src/RcppInterface.cpp -o scripts/RcppInterface.o $(RFLAGS) $(CFLAGS) $(CLIBS) $(CINCLUDE)
#	$(CC) $(OBJFILES) -shared -o scripts/RcppInterface.so scripts/RcppInterface.o $(RFLAGS) $(CFLAGS) $(CLIBS) $(CINCLUDE)


clean:
	rm $(OBJFILES) $(DEPFILES) -f

# DO NOT DELETE

src/main.o: src/informal_tests/test_database.cpp -
