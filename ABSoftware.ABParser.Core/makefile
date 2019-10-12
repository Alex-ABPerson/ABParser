OUTDIR := LinuxBuild
CSHARP_BUILD_DIR := ../ABSoftware.ABParser.Testing/bin/Debug
FLAGS := -fPIC -o
OUT_FILES:= ${OUTDIR}/Debugging.o ${OUTDIR}/ABParserBase.o ${OUTDIR}/ABParserBaseToken.o ${OUTDIR}/ExportedMethods.o

compile: ${OUTDIR} ${OUTDIR}/final.so

${OUTDIR}/Debugging.o: Debugging.cpp Debugging.h PlatformImplementation.h
${OUTDIR}/ABParserBase.o: ABParserBase.cpp ABParserBase.h PlatformImplementation.h
${OUTDIR}/ABParserBaseToken.o: ABParserBaseToken.cpp ABParserBaseToken.h PlatformImplementation.h
${OUTDIR}/ExportedMethods.o: ExportedMethods.cpp PlatformImplementation.h

compileAndCopy: compile
	cp ${OUTDIR}/final.so ${CSHARP_BUILD_DIR}/libABParserCore.so

testWithMono: compileAndCopy
	mono ${CSHARP_BUILD_DIR}/ABSoftware.ABParser.Testing.exe

clean: 
	rm -r ${OUTDIR}

${OUTDIR}:
	mkdir -p ${OUTDIR}

${OUT_FILES}:
	g++ -c $< ${FLAGS} $@

${OUTDIR}/final.so: ${OUT_FILES}
	g++ $^ -Wall -shared ${FLAGS} $@
