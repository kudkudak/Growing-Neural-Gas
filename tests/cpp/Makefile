ifndef GTEST
$(error GTEST is not set, add to your .bashrc GTEST env. variable pointing to gtest directory without slash at the end)
else

GMUMR_SRC_DIR = ../../src

include ../../src/Makevars

## Separate include dirs for transparency
GNG_INCLUDE  :=     -I ../../inst/include/gng
# Root directory (to allow explicit including f.e `svmlight/svm_learn.h`)
INCLUDE_ROOT :=     -I ../../inst/include
INCLUDE_DIRS :=     $(INCLUDE_ROOT) $(GNG_INCLUDE) -I ../../ -I ..

## Source dirs
GNG_SOURCES :=      $(wildcard $(GMUMR_SRC_DIR)/gng/*.cpp)
SOURCES :=          $(SVM_SOURCES)
OBJECTS :=          $(patsubst %.cpp, %.o, $(SOURCES))

GNG_LIBS :=    $(wildcard $(GNG_SOURCES)/*.o)

CXX := $(CXX) -isystem $(GTEST)/include
CPPFLAGS := $(INCLUDE_DIRS) $(PKG_CPPFLAGS)
CXXFLAGS := $(PKG_CXXFLAGS) -g -O2 -s -pthread
LDLIBS := $(PKG_LIBS) $(LDLIBS) -O1 -larmadillo -O1 -llapack -lblas

TESTS = basic_integration_tests parts_tests

GTEST_HEADERS = $(GTEST)/include/gtest/*.h \
				$(GTEST)/include/gtest/internal/*.h

all : $(TESTS)

debug:
	@echo $(CXX)
	@echo $(CXXFLAGS)
	@echo $(CPPFLAGS)
	@echo $(ARFLAGS)
	@echo $(AR)
echo :
	@echo $(R_HOME)
	@echo $(RCPPFLAGS)
	@echo $(RLDFLAGS)
	@echo $(RCPPINCL)
	@echo $(RCPPLIBS)
	@echo $(RCPPARMAINCL)
	@echo $(CXX)
	@echo $(CPPFLAGS)
	@echo $(CXXFLAGS)
	@echo $(LDLIBS)

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o

.PHONY : echo clean

GTEST_SRCS_ = $(GTEST)/src/*.cc $(GTEST)/src/*.h $(GTEST_HEADERS)

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

#%.o : %.cc
#	$(CXX) $(LDLIBS) $(CPPFLAGS) $(CXXFLAGS) $^ -c

%.o : %.cpp %.h
	$(CXX) $(LDLIBS) $(CPPFLAGS) $(CXXFLAGS) $^ -c

libsvm_test: gtest_main.a 
	$(CXX) svm/libsvm_test.cc $(GMUMR_SRC_DIR)/svm/svm_basic.cpp $(GMUMR_SRC_DIR)/svm/libsvm_runner.cpp $(GMUMR_SRC_DIR)/svm/svm.cpp $(LDLIBS) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

svm_utils_test: gtest_main.a 
	$(CXX) svm/svm_utils_test.cc $(LDLIBS) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

2_e_test: gtest_main.a
	$(CXX) svm/2_e_test.cc $(GMUMR_SRC_DIR)/svm/svm_basic.cpp $(GMUMR_SRC_DIR)/svm/libsvm_runner.cpp $(GMUMR_SRC_DIR)/svm/svm.cpp $(GMUMR_SRC_DIR)/svm/two_e_svm_pre.cpp $(GMUMR_SRC_DIR)/svm/two_e_svm_post.cpp $(LDLIBS) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

svmlight_runner_unittest: gtest_main.a $(OBJECTS) $(SVMLIGHT_DIR)/svm_common.o $(SVMLIGHT_DIR)/svm_learn.o $(SVMLIGHT_DIR)/svm_hideo.o
	$(CXX) $(CPPFLAGS) svm/svmlight_runner_unittest.cc $(LDLIBS) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@


endif
