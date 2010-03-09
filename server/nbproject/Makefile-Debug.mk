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
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=

# Macros
PLATFORM=GNU-Linux-x86

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/Debug/${PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/lua/ldo.o \
	${OBJECTDIR}/MyleftServer.o \
	${OBJECTDIR}/lua/ldebug.o \
	${OBJECTDIR}/db_mysql.o \
	${OBJECTDIR}/lua/lundump.o \
	${OBJECTDIR}/lua/lauxlib.o \
	${OBJECTDIR}/luafunction.o \
	${OBJECTDIR}/lua/lvm.o \
	${OBJECTDIR}/lua/lgc.o \
	${OBJECTDIR}/lua/ltablib.o \
	${OBJECTDIR}/log.o \
	${OBJECTDIR}/hash.o \
	${OBJECTDIR}/lua/lmathlib.o \
	${OBJECTDIR}/lua/print.o \
	${OBJECTDIR}/ezxml/ezxml.o \
	${OBJECTDIR}/lua/lopcodes.o \
	${OBJECTDIR}/message.o \
	${OBJECTDIR}/lua/lobject.o \
	${OBJECTDIR}/lua/lstring.o \
	${OBJECTDIR}/function.o \
	${OBJECTDIR}/lua/lparser.o \
	${OBJECTDIR}/lua/linit.o \
	${OBJECTDIR}/lua/lstrlib.o \
	${OBJECTDIR}/lua/lmem.o \
	${OBJECTDIR}/lua/ltable.o \
	${OBJECTDIR}/lua/ltm.o \
	${OBJECTDIR}/lua/ldump.o \
	${OBJECTDIR}/lua/loslib.o \
	${OBJECTDIR}/lua/ldblib.o \
	${OBJECTDIR}/lua/liolib.o \
	${OBJECTDIR}/util/md5.o \
	${OBJECTDIR}/lua/llex.o \
	${OBJECTDIR}/lua/lfunc.o \
	${OBJECTDIR}/lua/lstate.o \
	${OBJECTDIR}/lua/lbaselib.o \
	${OBJECTDIR}/lua/lzio.o \
	${OBJECTDIR}/lua/lcode.o \
	${OBJECTDIR}/lua/loadlib.o \
	${OBJECTDIR}/lua/lapi.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lmysqlclient -lssl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk dist/Debug/${PLATFORM}/myleftchat3

dist/Debug/${PLATFORM}/myleftchat3: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/${PLATFORM}
	${LINK.c} -o dist/Debug/${PLATFORM}/myleftchat3 ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/lua/ldo.o: lua/ldo.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/ldo.o lua/ldo.c

${OBJECTDIR}/MyleftServer.o: MyleftServer.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/MyleftServer.o MyleftServer.c

${OBJECTDIR}/lua/ldebug.o: lua/ldebug.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/ldebug.o lua/ldebug.c

${OBJECTDIR}/db_mysql.o: db_mysql.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/db_mysql.o db_mysql.c

${OBJECTDIR}/lua/lundump.o: lua/lundump.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lundump.o lua/lundump.c

${OBJECTDIR}/lua/lauxlib.o: lua/lauxlib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lauxlib.o lua/lauxlib.c

${OBJECTDIR}/luafunction.o: luafunction.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/luafunction.o luafunction.c

${OBJECTDIR}/lua/lvm.o: lua/lvm.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lvm.o lua/lvm.c

${OBJECTDIR}/lua/lgc.o: lua/lgc.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lgc.o lua/lgc.c

${OBJECTDIR}/lua/ltablib.o: lua/ltablib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/ltablib.o lua/ltablib.c

${OBJECTDIR}/log.o: log.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/log.o log.c

${OBJECTDIR}/hash.o: hash.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/hash.o hash.c

${OBJECTDIR}/lua/lmathlib.o: lua/lmathlib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lmathlib.o lua/lmathlib.c

${OBJECTDIR}/lua/print.o: lua/print.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/print.o lua/print.c

${OBJECTDIR}/ezxml/ezxml.o: ezxml/ezxml.c 
	${MKDIR} -p ${OBJECTDIR}/ezxml
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/ezxml/ezxml.o ezxml/ezxml.c

${OBJECTDIR}/lua/lopcodes.o: lua/lopcodes.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lopcodes.o lua/lopcodes.c

${OBJECTDIR}/message.o: message.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/message.o message.c

${OBJECTDIR}/lua/lobject.o: lua/lobject.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lobject.o lua/lobject.c

${OBJECTDIR}/lua/lstring.o: lua/lstring.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lstring.o lua/lstring.c

${OBJECTDIR}/function.o: function.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/function.o function.c

${OBJECTDIR}/lua/lparser.o: lua/lparser.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lparser.o lua/lparser.c

${OBJECTDIR}/lua/linit.o: lua/linit.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/linit.o lua/linit.c

${OBJECTDIR}/lua/lstrlib.o: lua/lstrlib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lstrlib.o lua/lstrlib.c

${OBJECTDIR}/lua/lmem.o: lua/lmem.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lmem.o lua/lmem.c

${OBJECTDIR}/lua/ltable.o: lua/ltable.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/ltable.o lua/ltable.c

${OBJECTDIR}/lua/ltm.o: lua/ltm.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/ltm.o lua/ltm.c

${OBJECTDIR}/lua/ldump.o: lua/ldump.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/ldump.o lua/ldump.c

${OBJECTDIR}/lua/loslib.o: lua/loslib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/loslib.o lua/loslib.c

${OBJECTDIR}/lua/ldblib.o: lua/ldblib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/ldblib.o lua/ldblib.c

${OBJECTDIR}/lua/liolib.o: lua/liolib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/liolib.o lua/liolib.c

${OBJECTDIR}/util/md5.o: util/md5.c 
	${MKDIR} -p ${OBJECTDIR}/util
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/util/md5.o util/md5.c

${OBJECTDIR}/lua/llex.o: lua/llex.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/llex.o lua/llex.c

${OBJECTDIR}/lua/lfunc.o: lua/lfunc.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lfunc.o lua/lfunc.c

${OBJECTDIR}/lua/lstate.o: lua/lstate.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lstate.o lua/lstate.c

${OBJECTDIR}/lua/lbaselib.o: lua/lbaselib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lbaselib.o lua/lbaselib.c

${OBJECTDIR}/lua/lzio.o: lua/lzio.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lzio.o lua/lzio.c

${OBJECTDIR}/lua/lcode.o: lua/lcode.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lcode.o lua/lcode.c

${OBJECTDIR}/lua/loadlib.o: lua/loadlib.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/loadlib.o lua/loadlib.c

${OBJECTDIR}/lua/lapi.o: lua/lapi.c 
	${MKDIR} -p ${OBJECTDIR}/lua
	${RM} $@.d
	$(COMPILE.c) -g -Wall -MMD -MP -MF $@.d -o ${OBJECTDIR}/lua/lapi.o lua/lapi.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Debug
	${RM} dist/Debug/${PLATFORM}/myleftchat3

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
