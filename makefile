FLAGS := -g -fPIC -o

GENERAL_OUTDIR := MakeBuild
GENERAL_LINUX_OUTDIR := Linux
GENERAL_MACOSX_OUTDIR:= MacOSX

# ABSOFTWARE.ABPARSER.CORE
CORE_DIR := ABSoftware.ABParser.Core
CORE_OUTDIR := ${CORE_DIR}/${GENERAL_OUTDIR}
CORE_LINUX_OUTDIR := ${CORE_OUTDIR}/${GENERAL_LINUX_OUTDIR}
CORE_MACOSX_OUTDIR := ${CORE_OUTDIR}/${GENERAL_MACOSX_OUTDIR}
CORE_LINUX_FINAL := ${CORE_LINUX_OUTDIR}/libfinal.a
CORE_MACOSX_FINAL := ${CORE_MACOSX_OUTDIR}/final.a

# ABSOFTWARE.ABPARSER.CORE.MANAGEDINTEROP
MI_DIR := ABSoftware.ABParser.Core.ManagedInterop
MI_OUTDIR := ${MI_DIR}/${GENERAL_OUTDIR}
MI_LINUX_OUTDIR := ${MI_OUTDIR}/${GENERAL_LINUX_OUTDIR}
MI_MACOSX_OUTDIR := ${MI_OUTDIR}/${GENERAL_MACOSX_OUTDIR}
MI_LINUX_FINAL := ${MI_LINUX_OUTDIR}/final.so
MI_MACOSX_FINAL := ${MI_MACOSX_OUTDIR}/final.dylib

# ABSOFTWARE.ABPARSER.TESTING.CPPTESTING
CPPT_DIR := ABSoftware.ABParser.Testing.CPPTesting
CPPT_OUTDIR := ${CPPT_DIR}/${GENERAL_OUTDIR}
CPPT_LINUX_OUTDIR := ${CPPT_OUTDIR}/${GENERAL_LINUX_OUTDIR}
CPPT_MACOSX_OUTDIR := ${CPPT_OUTDIR}/${GENERAL_MACOSX_OUTDIR}
CPPT_LINUX_FINAL := ${CPPT_LINUX_OUTDIR}/final.out
CPPT_MACOSX_FINAL := ${CPPT_MACOSX_OUTDIR}/final.out

# ====================================
# LIST OF REQUIRED FILES:
# ====================================

# ABSOFTWARE.ABPARSER.CORE:
CORE_LINUX_OUT_FILES := \
	${CORE_LINUX_OUTDIR}/ABParserBase.o \
	${CORE_LINUX_OUTDIR}/TokenManagement.o \
	${CORE_LINUX_OUTDIR}/HelperClasses.o 

CORE_MACOSX_OUT_FILES := \
	${CORE_MACOSX_OUTDIR}/ABParserBase.o \
	${CORE_MACOSX_OUTDIR}/TokenManagement.o \
	${CORE_MACOSX_OUTDIR}/HelperClasses.o

# ABSOFTWARE.ABPARSER.CORE.MANAGEDINTEROP:
MI_LINUX_OUT_FILES := \
	${MI_LINUX_OUTDIR}/ExportedMethods.o

MI_MACOSX_OUT_FILES := \
	${MI_MACOSX_OUTDIR}/ExportedMethods.o

# ABSOFTWARE.ABPARSER.TESTING.CPPTESTING:
CPPT_LINUX_OUT_FILES := ${CPPT_LINUX_OUTDIR}/Main.o
CPPT_MACOSX_OUT_FILES := ${CPPT_MACOSX_OUTDIR}/Main.o

# ====================================
# INDIVIDUAL FILES DEPENDENCIES:
# ====================================
# CPP File always comes first!

# ABSOFTWARE.ABPARSER.CORE:

# ABParserBase.o
${CORE_LINUX_OUTDIR}/ABParserBase.o ${CORE_MACOSX_OUTDIR}/ABParserBase.o: \
	${CORE_DIR}/ABParserBase.cpp \
	${CORE_DIR}/ABParserBase.h \
	${CORE_DIR}/PlatformImplementation.h \
	${CORE_DIR}/Debugging.h

# TokenManagement.o
${CORE_LINUX_OUTDIR}/TokenManagement.o ${CORE_MACOSX_OUTDIR}/TokenManagement.o: \
	${CORE_DIR}/TokenManagement.cpp \
	${CORE_DIR}/TokenManagement.h \
	${CORE_DIR}/PlatformImplementation.h

${CORE_LINUX_OUTDIR}/HelperClasses.o ${CORE_MACOSX_OUTDIR}/HelperClasses.o ${CORE_DIR}/ABParserBase.h: \
	${CORE_DIR}/HelperClasses.cpp \
	${CORE_DIR}/HelperClasses.h

# ABSOFTWARE.ABPARSER.CORE.MANAGEDINTEROP:
# ExportedMethods.o
${MI_LINUX_OUTDIR}/ExportedMethods.o ${MI_MACOSX_OUTDIR}/ExportedMethods.o: \
	${MI_DIR}/ExportedMethods.cpp \
	${CORE_DIR}/PlatformImplementation.h \
	${CORE_DIR}/ABParserBase.h \
	${CORE_DIR}/Debugging.h \
	${CORE_DIR}/TokenManagement.h

# ABSOFTWARE.ABPARSER.TESTING.CPPTESTING:
${CPPT_LINUX_OUTDIR}/Main.o ${CPPT_MACOSX_OUTDIR}/Main.o: \
	${CPPT_DIR}/Main.cpp \
	${CORE_DIR}/ABParserBase.h

# ====================================
# MODES:
# ====================================
compileAll: compileCoreLinux compileMILinux compileCPPT
compileCoreLinux: ${CORE_LINUX_OUTDIR} ${CORE_LINUX_FINAL}
compileMILinux: compileCoreLinux ${MI_LINUX_OUTDIR} ${MI_LINUX_FINAL} copyMILinux
compileCPPT: compileCoreLinux compileMILinux ${CPPT_LINUX_OUTDIR} ${CPPT_LINUX_FINAL}

testWithMono: compileAll
	mono ABSoftware.ABParser.Testing/bin/Debug/ABSoftware.ABParser.Testing.exe

clean: 
	rm -r ${CORE_OUTDIR} ${MI_OUTDIR}

# ====================================
# BASE COMMANDS:
# ====================================

# FOLDER CREATION:
${CORE_LINUX_OUTDIR} ${CORE_MACOSX_OUTDIR}: ${CORE_OUTDIR}
${MI_LINUX_OUTDIR} ${MI_MACOSX_OUTDIR}: ${MI_OUTDIR}
${CPPT_LINUX_OUTDIR} ${CPPT_MACOSX_OUTDIR}: ${CPPT_OUTDIR}

${CORE_OUTDIR} ${MI_OUTDIR} ${CPPT_OUTDIR} ${CORE_LINUX_OUTDIR} ${CORE_MACOSX_OUTDIR} ${MI_LINUX_OUTDIR} ${MI_MACOSX_OUTDIR} ${CPPT_LINUX_OUTDIR} ${CPPT_MACOSX_OUTDIR}:
	mkdir -p $@

# COMPILATION:

${CORE_LINUX_FINAL}: ${CORE_LINUX_OUT_FILES}
${MI_LINUX_FINAL}: ${MI_LINUX_OUT_FILES}
${CPPT_LINUX_FINAL}: ${CPPT_LINUX_OUT_FILES}

# Output Files:
${CORE_LINUX_OUT_FILES}:
	g++ -c $< ${FLAGS} $@

${MI_LINUX_OUT_FILES}:
	g++ -I${CORE_DIR} -c $< ${FLAGS} $@

${CPPT_LINUX_OUT_FILES}:
	g++ -I${CORE_DIR} -c $< ${FLAGS} $@

# Static Libraries:
${CORE_LINUX_FINAL} ${CORE_MACOSX_FINAL}:
	ar rcs $@ $^

# Dynamic Libraries:
${MI_LINUX_FINAL}:
	g++ $^ -I${CORE_DIR} -L${CORE_LINUX_OUTDIR} -lfinal -Wall -shared ${FLAGS} $@

# Applications:
${CPPT_LINUX_FINAL}:
	g++ $^ -I${CORE_DIR} -L${CORE_LINUX_OUTDIR} -lfinal ${FLAGS} $@

copyMILinux: 
	cp ${MI_LINUX_OUTDIR}/final.so ABSoftware.ABParser.Testing/bin/Debug/libABParserCore.so
	cp ${MI_LINUX_OUTDIR}/final.so ABSoftware.ABParser.Testing.MemoryTests/bin/Debug/libABParserCore.so
	cp ${MI_LINUX_OUTDIR}/final.so ABSoftware.ABParser.Testing.UnitTests/bin/Debug/libABParserCore.so
