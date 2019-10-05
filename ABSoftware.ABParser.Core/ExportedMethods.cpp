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

	EXPORT ABParserBase* CreateBaseParser(wchar_t** tokens, int* tokenLengths, int numberOfTokens) {
		return new ABParserBase(tokens, tokenLengths, numberOfTokens);
	}

	EXPORT void SetText(ABParserBase* parser, wchar_t* text, int textLength) {
		parser->InitString(text, textLength);
	}

	EXPORT int ContinueExecution(ABParserBase* parser, int* outData) {
		return parser->ContinueExecution();
	}

	EXPORT void DeleteBaseParser(ABParserBase* baseParser) {
		delete baseParser;
	}
}