# GNG Makefile
# requires Rcpp,R and boost in path


#TODO: make it use generic dependencies. Now modyfing .h leads to cleaning whole build, so it is extra lame.
#TODO: make it generic.. FINALLY!. For now, please modify it, and sorry for that
# GNG Makefile
# requires Rcpp,R and boost in path

#rcpp include flags (in readme)
CC=g++
CINCLUDE=-I./include -I./src -I/home/staszek/usr/include -I/usr/share/R/include -I/usr/local/lib/R/site-library/Rcpp/include -I/usr/local/lib/R/site-library/RcppArmadillo/include
CLIBS= -lboost_system -lpthread -lrt -lboost_thread
RFLAGS=$(shell Rscript scripts/generateflags.r)
CFLAGS=-fPIC -DDEBUG


BUILD_DIR=build/performance
SRC_DIR=src
INCLUDE_DIR=include

CPPFILES := $(foreach dir, $(SRC_DIR)/, $(notdir $(wildcard $(SRC_DIR)/*.cpp)))
OBJFILES := $(addprefix $(BUILD_DIR)/, $(CPPFILES:.cpp=.o))


#SOURCES=$(SRC_DIR)/SHGraphDefs.cpp $(SRC_DIR)/GNGAlgorithm.cpp $(SRC_DIR)/SHMemoryManager.cpp $(SRC_DIR)/Utils.cpp $(SRC_DIR)/GNGServer.cpp
#DEPFILES=$(BUILD_DIR)/SHGraphDefs.d $(BUILD_DIR)/GNGAlgorithm.d $(BUILD_DIR)/SHMemoryManager.d $(BUILD_DIR)/Utils.d $(BUILD_DIR)/GNGServer.d
#OBJFILES=$(BUILD_DIR)/SHGraphDefs.o $(BUILD_DIR)/GNGAlgorithm.o $(BUILD_DIR)/SHMemoryManager.o $(BUILD_DIR)/Utils.o $(BUILD_DIR)/GNGServer.o


$(OBJFILES): $(CPPFILES)
	$(CC) -c $< -o $@ $(CFLAGS) -l$(SRC_DIR) $(CLIBS) $(CINCLUDE)



all: $(OBJFILES) main



#$(OBJRFILES)

main: $(OBJFILES) main.cpp
	$(CC) $(SRC_DIR)/main.cpp $(wildcard build/performance/*.o) -o main $(CFLAGS) $(CLIBS) $(CINCLUDE)


 #rcpp: all
#	$(CC) $(OBJFILES) -c src/RcppInterface.cpp -o scripts/RcppInterface.o $(RFLAGS) $(CFLAGS) $(CLIBS) $(CINCLUDE)
#	$(CC) $(OBJFILES) -shared -o scripts/RcppInterface.so scripts/RcppInterface.o $(RFLAGS) $(CFLAGS) $(CLIBS) $(CINCLUDE)


clean:
	rm $(BUILD_DIR)/*  

