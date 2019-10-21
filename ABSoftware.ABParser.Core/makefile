OUTDIR := MakeBuild
FLAGS := -fPIC -o
LINUX_OUTDIR := ${OUTDIR}/Linux
MACOSX_OUTDIR:= ${OUTDIR}/MacOSX
CSHARP_BUILD_DIR := ../ABSoftware.ABParser.Testing/bin/Debug
LINUX_OUT_FILES := ${LINUX_OUTDIR}/Debugging.o ${LINUX_OUTDIR}/ABParserBase.o ${LINUX_OUTDIR}/ABParserBaseToken.o ${LINUX_OUTDIR}/ExportedMethods.o ${LINUX_OUTDIR}/TokenManagement.o
MACOSX_OUT_FILES := ${MACOSX_OUTDIR}/Debugging.o ${MACOSX_OUTDIR}/ABParserBase.o ${MACOSX_OUTDIR}/ABParserBaseToken.o ${MACOSX_OUTDIR}/ExportedMethods.o ${MACOSX_OUTDIR}/TokenManagement.o

compileLinux: ${LINUX_OUTDIR} ${OUTDIR}/final.so
compileMacOS: ${MACOSX_OUTDIR} ${OUTDIR}/final.dylib

${LINUX_OUTDIR}/Debugging.o ${MACOSX_OUTDIR}/Debugging.o: Debugging.cpp Debugging.h PlatformImplementation.h
${LINUX_OUTDIR}/ABParserBase.o ${MACOSX_OUTDIR}/ABParserBase.o: ABParserBase.cpp ABParserBase.h PlatformImplementation.h
${LINUX_OUTDIR}/ABParserBaseToken.o ${MACOSX_OUTDIR}/ABParserBaseToken.o: ABParserBaseToken.cpp ABParserBaseToken.h PlatformImplementation.h
${LINUX_OUTDIR}/ExportedMethods.o ${MACOSX_OUTDIR}/ExportedMethods.o: ExportedMethods.cpp PlatformImplementation.h
${LINUX_OUTDIR}/TokenManagement.o ${MACOSX_OUTDIR}/TokenManagement.o: TokenManagement.cpp TokenManagement.h PlatformImplementation.h

compileAndCopy: compileLinux
	cp ${OUTDIR}/final.so ${CSHARP_BUILD_DIR}/libABParserCore.so
#	cp ${OUTDIR}/final.dylib ${CSHARP_BUILD_DIR}/libABParserCore.dylib

testWithMono: compileAndCopy
	mono ${CSHARP_BUILD_DIR}/ABSoftware.ABParser.Testing.exe

clean: 
	rm -r ${OUTDIR}

${OUTDIR} ${LINUX_OUTDIR} ${MACOSX_OUTDIR}:
	mkdir -p $@

${LINUX_OUT_FILES}:
	g++ -c $< ${FLAGS} $@

${MACOSX_OUT_FILES}:
	g++ -m64 -c $< ${FLAGS} -o $@

${OUTDIR}/final.so: ${LINUX_OUT_FILES}
	g++ $^ -Wall -shared ${FLAGS} $@

${OUTDIR}/final.dylib: ${MACOSX_OUT_FILES}
	g++ $^ -m64 -shared -o $@
