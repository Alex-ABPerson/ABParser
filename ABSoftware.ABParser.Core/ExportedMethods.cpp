#include "PlatformImplementation.h"
#include "ABParserBase.h"
#include <wchar.h>

using namespace std;

extern "C" {
	EXPORT void SayHello(wchar_t* out) {
		const wchar_t* str = L"Hello";
		wcscpy(out, str);
	}

	EXPORT ABParserBase* CreateBaseParser(wchar_t** tokens, int textLength) {
		return new ABParserBase(tokens, textLength);
	}

	EXPORT void SetText(ABParserBase* parser, wchar_t* text, int textLength) {
		parser->InitString(text, textLength);
	}

	EXPORT int ContinueExecution(wchar_t* leadingOutput, wchar_t* trailingOutput) {
		return 0;
	}

	EXPORT void DeleteBaseParser(ABParserBase* baseParser) {
		delete baseParser;
	}
}