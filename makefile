IS_64 = true
ifdef IS_64
FLAGS := -m64 -O2 -g -fPIC -o
PLATFORM_DIR = x64
else
FLAGS := -m32 -O2 -g -fPIC -o
PLATFORM_DIR = x86
endif

compileAll: compileMILinux compileCPPT

GENERAL_OUTDIR := MakeBuild
GENERAL_LINUX_OUTDIR := Linux
GENERAL_MACOSX_OUTDIR:= MacOSX

# ABSOFTWARE.ABPARSER.CORE
CORE_DIR := ABSoftware.ABParser.Core

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

# ABSOFTWARE.ABPARSER.CORE.MANAGEDINTEROP:
MI_LINUX_OUT_FILES := ${MI_LINUX_OUTDIR}/ExportedMethods.o
MI_MACOSX_OUT_FILES := ${MI_MACOSX_OUTDIR}/ExportedMethods.o

# ABSOFTWARE.ABPARSER.TESTING.CPPTESTING:
CPPT_LINUX_OUT_FILES := ${CPPT_LINUX_OUTDIR}/Main.o
CPPT_MACOSX_OUT_FILES := ${CPPT_MACOSX_OUTDIR}/Main.o

# ====================================
# INDIVIDUAL FILES DEPENDENCIES:
# ====================================
# CPP File always comes first on compileable files!

${CORE_DIR}/ABParser.h: ${CORE_DIR}/ABParserBase.h
${CORE_DIR}/ABParserBase.h: ${CORE_DIR}/ABParserHelpers.h ${CORE_DIR}/ABParserConfig.h ${CORE_DIR}/ABParserDebugging.h

# ABSOFTWARE.ABPARSER.CORE.MANAGEDINTEROP:
# ExportedMethods.o
${MI_LINUX_OUTDIR}/ExportedMethods.o ${MI_MACOSX_OUTDIR}/ExportedMethods.o: \
	${MI_DIR}/ExportedMethods.cpp \
	${CORE_DIR}/ABParserBase.h

# ABSOFTWARE.ABPARSER.TESTING.CPPTESTING:
${CPPT_LINUX_OUTDIR}/Main.o ${CPPT_MACOSX_OUTDIR}/Main.o: \
	${CPPT_DIR}/Main.cpp \
	${CORE_DIR}/ABParser.h

# ====================================
# MODES:
# ====================================
compileMILinux: ${MI_LINUX_OUTDIR} ${MI_LINUX_FINAL} copyMILinux
compileCPPT: ${CPPT_LINUX_OUTDIR} ${CPPT_LINUX_FINAL}

runConsoleApp: compileAll
	dotnet run --project ABSoftware.ABParser.Testing.ConsoleApp
runMemPerf: compileAll
	dotnet run --project ABSoftware.ABParser.Testing.MemPerfTests
runUnitTests: compileAll
	dotnet vstest ABSoftware.ABParser.Testing.UnitTests/bin/${PLATFORM_DIR}/Debug/netcoreapp3.1/ABSoftware.ABParser.Testing.UnitTests.dll

clean: 
	rm -r ${MI_OUTDIR} 
	rm -r ${CPPT_LINUX}

# ====================================
# BASE COMMANDS:
# ====================================

# FOLDER CREATION:
${MI_LINUX_OUTDIR} ${MI_MACOSX_OUTDIR}: ${MI_OUTDIR}
${CPPT_LINUX_OUTDIR} ${CPPT_MACOSX_OUTDIR}: ${CPPT_OUTDIR}

${MI_OUTDIR} ${CPPT_OUTDIR} ${MI_LINUX_OUTDIR} ${MI_MACOSX_OUTDIR} ${CPPT_LINUX_OUTDIR} ${CPPT_MACOSX_OUTDIR}:
	mkdir -p $@

# COMPILATION:
${MI_LINUX_FINAL}: ${MI_LINUX_OUT_FILES}
${CPPT_LINUX_FINAL}: ${CPPT_LINUX_OUT_FILES}

# Output Files:
${MI_LINUX_OUT_FILES}:
	g++ -I${CORE_DIR} -c $< ${FLAGS} $@

${CPPT_LINUX_OUT_FILES}:
	g++ -I${CORE_DIR} -c $< -o $@

# Dynamic Libraries:
${MI_LINUX_FINAL}:
	g++  $^ -I${CORE_DIR} -Wall -shared ${FLAGS} $@

# Applications:
${CPPT_LINUX_FINAL}:
	g++ -m64 $^ -o $@

copyMILinux: 
	cp ${MI_LINUX_OUTDIR}/final.so ABSoftware.ABParser.Testing.ConsoleApp/bin/${PLATFORM_DIR}/Debug/netcoreapp3.1/libABParserCore.so
	cp ${MI_LINUX_OUTDIR}/final.so ABSoftware.ABParser.Testing.MemPerfTests/bin/${PLATFORM_DIR}/Debug/netcoreapp3.1/libABParserCore.so
	cp ${MI_LINUX_OUTDIR}/final.so ABSoftware.ABParser.Testing.UnitTests/bin/${PLATFORM_DIR}/Debug/netcoreapp3.1/libABParserCore.so
