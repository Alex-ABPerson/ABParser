#include "PlatformImplementation.h"
#include "ABParserBase.h"
#include "Debugging.h"
#include <wchar.h>

using namespace std;

extern "C" {
	EXPORT void SayHello(wchar_t* out) {
		const wchar_t* str = L"Hello";
		wcscpy(out, str);
	}

	EXPORT int GetWideCharacterSize() {
		return sizeof(wchar_t);
	}

	EXPORT ABParserBase* CreateBaseParser(unsigned short** tokens, int* tokenLengths, int numberOfTokens) {
		return new ABParserBase(tokens, tokenLengths, numberOfTokens);
	}

	EXPORT void SetText(ABParserBase* parser, unsigned short* text, int textLength) {
		parser->InitString(text, textLength);
	}

	EXPORT int ContinueExecution(ABParserBase* parser, unsigned char* outData) {
		/*outData[0] = 215;
		outData[1] = 245;*/
		return parser->ContinueExecution();
	}

	EXPORT void DeleteBaseParser(ABParserBase* baseParser) {
		delete baseParser;
	}
}