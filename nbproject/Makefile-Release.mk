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
CND_PLATFORM=GNU-Linux
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
	${OBJECTDIR}/src/AvlTree.o \
	${OBJECTDIR}/test/AvlTreeTest.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1

# Test Object Files
TESTOBJECTFILES= \
	${TESTDIR}/test/AvlTreeUnitTest.o \
	${TESTDIR}/test/test.o

# C Compiler Flags
CFLAGS=-Wall -std=gnu99 -m32

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lm

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cutil

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cutil: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cutil ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/AvlTree.o: src/AvlTree.c
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O3 -DNDEBUG -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AvlTree.o src/AvlTree.c

${OBJECTDIR}/test/AvlTreeTest.o: test/AvlTreeTest.c
	${MKDIR} -p ${OBJECTDIR}/test
	${RM} "$@.d"
	$(COMPILE.c) -O3 -DNDEBUG -Iinclude -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/test/AvlTreeTest.o test/AvlTreeTest.c

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-tests-subprojects .build-conf ${TESTFILES}
.build-tests-subprojects:

${TESTDIR}/TestFiles/f1: ${TESTDIR}/test/AvlTreeUnitTest.o ${TESTDIR}/test/test.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.c} -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS}   


${TESTDIR}/test/AvlTreeUnitTest.o: test/AvlTreeUnitTest.c 
	${MKDIR} -p ${TESTDIR}/test
	${RM} "$@.d"
	$(COMPILE.c) -O3 -DNDEBUG -Iinclude -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/test/AvlTreeUnitTest.o test/AvlTreeUnitTest.c


${TESTDIR}/test/test.o: test/test.c 
	${MKDIR} -p ${TESTDIR}/test
	${RM} "$@.d"
	$(COMPILE.c) -O3 -DNDEBUG -Iinclude -I. -MMD -MP -MF "$@.d" -o ${TESTDIR}/test/test.o test/test.c


${OBJECTDIR}/src/AvlTree_nomain.o: ${OBJECTDIR}/src/AvlTree.o src/AvlTree.c 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/AvlTree.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O3 -DNDEBUG -Iinclude -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AvlTree_nomain.o src/AvlTree.c;\
	else  \
	    ${CP} ${OBJECTDIR}/src/AvlTree.o ${OBJECTDIR}/src/AvlTree_nomain.o;\
	fi

${OBJECTDIR}/test/AvlTreeTest_nomain.o: ${OBJECTDIR}/test/AvlTreeTest.o test/AvlTreeTest.c 
	${MKDIR} -p ${OBJECTDIR}/test
	@NMOUTPUT=`${NM} ${OBJECTDIR}/test/AvlTreeTest.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.c) -O3 -DNDEBUG -Iinclude -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/test/AvlTreeTest_nomain.o test/AvlTreeTest.c;\
	else  \
	    ${CP} ${OBJECTDIR}/test/AvlTreeTest.o ${OBJECTDIR}/test/AvlTreeTest_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
