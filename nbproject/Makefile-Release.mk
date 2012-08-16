#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main_2.o \
	${OBJECTDIR}/GNGAlgorithm.o \
	${OBJECTDIR}/GNGGraph.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/server_client_example/server.o \
	${OBJECTDIR}/SHGraphDefs.o \
	${OBJECTDIR}/Utils.o \
	${OBJECTDIR}/DebugCollector.o \
	${OBJECTDIR}/server_client_example/client.o \
	${OBJECTDIR}/ExtGraphNodeManager.o \
	${OBJECTDIR}/SHMemoryManager.o \
	${OBJECTDIR}/GNGDatabase.o \
	${OBJECTDIR}/RcppInterface.o \
	${OBJECTDIR}/main_1.o \
	${OBJECTDIR}/GNGGraphInfo.o \
	${OBJECTDIR}/test_client.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f2

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${TESTDIR}/TestFiles/f1

${TESTDIR}/TestFiles/f1: ${OBJECTFILES}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc} -o ${TESTDIR}/TestFiles/f1 ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/main_2.o: main_2.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/main_2.o main_2.cpp

${OBJECTDIR}/GNGAlgorithm.o: GNGAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/GNGAlgorithm.o GNGAlgorithm.cpp

${OBJECTDIR}/GNGGraph.o: GNGGraph.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/GNGGraph.o GNGGraph.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/server_client_example/server.o: server_client_example/server.cpp 
	${MKDIR} -p ${OBJECTDIR}/server_client_example
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/server_client_example/server.o server_client_example/server.cpp

${OBJECTDIR}/SHGraphDefs.o: SHGraphDefs.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/SHGraphDefs.o SHGraphDefs.cpp

${OBJECTDIR}/Utils.o: Utils.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Utils.o Utils.cpp

${OBJECTDIR}/DebugCollector.o: DebugCollector.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/DebugCollector.o DebugCollector.cpp

${OBJECTDIR}/server_client_example/client.o: server_client_example/client.cpp 
	${MKDIR} -p ${OBJECTDIR}/server_client_example
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/server_client_example/client.o server_client_example/client.cpp

${OBJECTDIR}/ExtGraphNodeManager.o: ExtGraphNodeManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/ExtGraphNodeManager.o ExtGraphNodeManager.cpp

${OBJECTDIR}/SHMemoryManager.o: SHMemoryManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/SHMemoryManager.o SHMemoryManager.cpp

${OBJECTDIR}/GNGDatabase.o: GNGDatabase.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/GNGDatabase.o GNGDatabase.cpp

${OBJECTDIR}/RcppInterface.o: RcppInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/RcppInterface.o RcppInterface.cpp

${OBJECTDIR}/main_1.o: main_1.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/main_1.o main_1.cpp

${OBJECTDIR}/GNGGraphInfo.o: GNGGraphInfo.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/GNGGraphInfo.o GNGGraphInfo.cpp

${OBJECTDIR}/test_client.o: test_client.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/test_client.o test_client.cpp

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-conf ${TESTFILES}
${TESTDIR}/TestFiles/f2: ${TESTDIR}/server_client_example/tests/newtestclass.o ${TESTDIR}/server_client_example/tests/newtestrunner.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc}   -o ${TESTDIR}/TestFiles/f2 $^ ${LDLIBSOPTIONS} 


${TESTDIR}/server_client_example/tests/newtestclass.o: server_client_example/tests/newtestclass.cpp 
	${MKDIR} -p ${TESTDIR}/server_client_example/tests
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -I. -I. -I. -MMD -MP -MF $@.d -o ${TESTDIR}/server_client_example/tests/newtestclass.o server_client_example/tests/newtestclass.cpp


${TESTDIR}/server_client_example/tests/newtestrunner.o: server_client_example/tests/newtestrunner.cpp 
	${MKDIR} -p ${TESTDIR}/server_client_example/tests
	${RM} $@.d
	$(COMPILE.cc) -O2 -I. -I. -I. -I. -MMD -MP -MF $@.d -o ${TESTDIR}/server_client_example/tests/newtestrunner.o server_client_example/tests/newtestrunner.cpp


${OBJECTDIR}/main_2_nomain.o: ${OBJECTDIR}/main_2.o main_2.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/main_2.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/main_2_nomain.o main_2.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/main_2.o ${OBJECTDIR}/main_2_nomain.o;\
	fi

${OBJECTDIR}/GNGAlgorithm_nomain.o: ${OBJECTDIR}/GNGAlgorithm.o GNGAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/GNGAlgorithm.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/GNGAlgorithm_nomain.o GNGAlgorithm.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/GNGAlgorithm.o ${OBJECTDIR}/GNGAlgorithm_nomain.o;\
	fi

${OBJECTDIR}/GNGGraph_nomain.o: ${OBJECTDIR}/GNGGraph.o GNGGraph.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/GNGGraph.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/GNGGraph_nomain.o GNGGraph.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/GNGGraph.o ${OBJECTDIR}/GNGGraph_nomain.o;\
	fi

${OBJECTDIR}/main_nomain.o: ${OBJECTDIR}/main.o main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/main.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/main_nomain.o main.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/main.o ${OBJECTDIR}/main_nomain.o;\
	fi

${OBJECTDIR}/server_client_example/server_nomain.o: ${OBJECTDIR}/server_client_example/server.o server_client_example/server.cpp 
	${MKDIR} -p ${OBJECTDIR}/server_client_example
	@NMOUTPUT=`${NM} ${OBJECTDIR}/server_client_example/server.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/server_client_example/server_nomain.o server_client_example/server.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/server_client_example/server.o ${OBJECTDIR}/server_client_example/server_nomain.o;\
	fi

${OBJECTDIR}/SHGraphDefs_nomain.o: ${OBJECTDIR}/SHGraphDefs.o SHGraphDefs.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/SHGraphDefs.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/SHGraphDefs_nomain.o SHGraphDefs.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/SHGraphDefs.o ${OBJECTDIR}/SHGraphDefs_nomain.o;\
	fi

${OBJECTDIR}/Utils_nomain.o: ${OBJECTDIR}/Utils.o Utils.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/Utils.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/Utils_nomain.o Utils.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/Utils.o ${OBJECTDIR}/Utils_nomain.o;\
	fi

${OBJECTDIR}/DebugCollector_nomain.o: ${OBJECTDIR}/DebugCollector.o DebugCollector.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/DebugCollector.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/DebugCollector_nomain.o DebugCollector.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/DebugCollector.o ${OBJECTDIR}/DebugCollector_nomain.o;\
	fi

${OBJECTDIR}/server_client_example/client_nomain.o: ${OBJECTDIR}/server_client_example/client.o server_client_example/client.cpp 
	${MKDIR} -p ${OBJECTDIR}/server_client_example
	@NMOUTPUT=`${NM} ${OBJECTDIR}/server_client_example/client.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/server_client_example/client_nomain.o server_client_example/client.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/server_client_example/client.o ${OBJECTDIR}/server_client_example/client_nomain.o;\
	fi

${OBJECTDIR}/ExtGraphNodeManager_nomain.o: ${OBJECTDIR}/ExtGraphNodeManager.o ExtGraphNodeManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/ExtGraphNodeManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/ExtGraphNodeManager_nomain.o ExtGraphNodeManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/ExtGraphNodeManager.o ${OBJECTDIR}/ExtGraphNodeManager_nomain.o;\
	fi

${OBJECTDIR}/SHMemoryManager_nomain.o: ${OBJECTDIR}/SHMemoryManager.o SHMemoryManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/SHMemoryManager.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/SHMemoryManager_nomain.o SHMemoryManager.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/SHMemoryManager.o ${OBJECTDIR}/SHMemoryManager_nomain.o;\
	fi

${OBJECTDIR}/GNGDatabase_nomain.o: ${OBJECTDIR}/GNGDatabase.o GNGDatabase.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/GNGDatabase.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/GNGDatabase_nomain.o GNGDatabase.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/GNGDatabase.o ${OBJECTDIR}/GNGDatabase_nomain.o;\
	fi

${OBJECTDIR}/RcppInterface_nomain.o: ${OBJECTDIR}/RcppInterface.o RcppInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/RcppInterface.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/RcppInterface_nomain.o RcppInterface.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/RcppInterface.o ${OBJECTDIR}/RcppInterface_nomain.o;\
	fi

${OBJECTDIR}/main_1_nomain.o: ${OBJECTDIR}/main_1.o main_1.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/main_1.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/main_1_nomain.o main_1.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/main_1.o ${OBJECTDIR}/main_1_nomain.o;\
	fi

${OBJECTDIR}/GNGGraphInfo_nomain.o: ${OBJECTDIR}/GNGGraphInfo.o GNGGraphInfo.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/GNGGraphInfo.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/GNGGraphInfo_nomain.o GNGGraphInfo.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/GNGGraphInfo.o ${OBJECTDIR}/GNGGraphInfo_nomain.o;\
	fi

${OBJECTDIR}/test_client_nomain.o: ${OBJECTDIR}/test_client.o test_client.cpp 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/test_client.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.cc) -O2 -I. -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/test_client_nomain.o test_client.cpp;\
	else  \
	    ${CP} ${OBJECTDIR}/test_client.o ${OBJECTDIR}/test_client_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f2 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${TESTDIR}/TestFiles/f1

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
