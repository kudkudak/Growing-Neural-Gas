ifndef GTEST
$(error GTEST is not set, add to your .bashrc GTEST env. variable pointing to gtest directory without slash at the end)
else

## Variables
GMUM_SRC_DIR = ../../src

## Separate include dirs for transparency
GNG_INCLUDE  := -I ../../inst/include/gng -I ../../inst/include/tinythread
ROOT_INCLUDE := -I ../../inst/include

## Compiler flags, load Makevars
include ../../src/Makevars
CXX := $(CXX) -isystem $(GTEST)/include
CXXFLAGS := $(PKG_CXXFLAGS) 
CPPFLAGS := $(PKG_CPPFLAGS) $(GNG_INCLUDE) $(ROOT_INCLUDE)
LDLIBS := $(PKG_LIBS)

## Source dirs
GNG_SOURCES := $(wildcard $(GMUM_SRC_DIR)/gng/*.cpp $(GMUM_SRC_DIR)/tinythread/*.cpp)
OBJECTS := $(patsubst %.cpp, %.o, $(GNG_SOURCES))

## Object
GNG_OBJS := $(wildcard $(GNG_SOURCES)/*.o)

## Tests
TESTS = basic_integration_tests parts_tests
TEST_SOURCES = $(wildcard gng/*.cpp)

## GTEST
GTEST_HEADERS = $(GTEST)/include/gtest/*.h \
				$(GTEST)/include/gtest/internal/*.h
GTEST_SRCS_ = $(wildcard $(GTEST)/src/*.cc $(GTEST)/src/*.h $(GTEST_HEADERS))

## Targets

all : $(TESTS)

gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST) $(CXXFLAGS) -c \
			$(GTEST)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST) $(CXXFLAGS) -c \
			$(GTEST)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

%.o : %.cpp %.h
	$(CXX) $(LDLIBS) $(CPPFLAGS) $(CXXFLAGS) $^ -c

$(TESTS): gtest_main.a 
	$(CXX) gng/$@.cpp $(OBJECTS) $(LDLIBS) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

debug:
	@echo $(CXX)
	@echo $(CXXFLAGS)
	@echo $(CPPFLAGS)
	@echo $(LDLIBS)
	@echo $(ARFLAGS)
	@echo $(AR)

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o



.PHONY : echo clean


endif
