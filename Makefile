# GNG Makefile
# requires Rcpp,R and boost in path

#rcpp include flags (in readme)
CC=g++
CINCLUDE=-I./include -I./src -I/home/staszek/usr/include -I/usr/share/R/include -I/usr/local/lib/R/site-library/Rcpp/include -I/usr/local/lib/R/site-library/RcppArmadillo/include
CLIBS=-lboost_system -lpthread -lrt -lboost_thread
RFLAGS=$(shell Rscript scripts/generateflags.r)
CFLAGS=-fPIC -DDEBUG 


BUILD_DIR=build/performance
SRC_DIR=src
INCLUDE_DIR=include


#SOURCES:=$(shell cat build/files|sed "s/\(.*\)/src\/\1\.cpp/g")
#DEPFILES:=$(shell mcat build/files|sed "s/\(.*\)/build\/performance\/\1\.o/g")
#OBJFILES:=$(shell cat build/files|sed "s/\(.*\)/build\/performance\/\1\.d/g")

SOURCES=$(SRC_DIR)/SHGraphDefs.cpp $(SRC_DIR)/GNGAlgorithm.cpp $(SRC_DIR)/SHMemoryManager.cpp $(SRC_DIR)/Utils.cpp 
DEPFILES=$(BUILD_DIR)/SHGraphDefs.d $(BUILD_DIR)/GNGAlgorithm.d $(BUILD_DIR)/SHMemoryManager.d $(BUILD_DIR)/Utils.d 
OBJFILES=$(BUILD_DIR)/SHGraphDefs.o $(BUILD_DIR)/GNGAlgorithm.o $(BUILD_DIR)/SHMemoryManager.o $(BUILD_DIR)/Utils.o 
 


all: $(OBJFILES) main.cpp



#$(OBJRFILES)

main.cpp:$(OBJFILES)
	$(CC) $(SRC_DIR)/main.cpp $(OBJFILES) -o main  $(CFLAGS) $(CLIBS) $(CINCLUDE) 

#build/performance/%.d:src/%.cpp
#	$(CC) -c $< -o $@ $(CFLAGS) $(CLIBS) $(CINCLUDE) 

 
build/performance/%.o:src/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) -l$(SRC_DIR) $(CLIBS) $(CINCLUDE)  

rcpp: all
	$(CC) $(OBJFILES) -c src/RcppInterface.cpp -o scripts/RcppInterface.o $(RFLAGS) $(CFLAGS)  $(CLIBS) $(CINCLUDE)  
	$(CC) $(OBJFILES) -shared -o scripts/RcppInterface.so scripts/RcppInterface.o $(RFLAGS) $(CFLAGS)  $(CLIBS) $(CINCLUDE)  


clean:
	rm $(BUILD_DIR)/*
	rm scripts/*.o
