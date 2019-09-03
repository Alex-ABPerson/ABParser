#pragma once

class ABParserBaseToken {
public:
	wchar_t* Name;
	wchar_t* TokenCharacters;

	ABParserBaseToken(wchar_t* name, wchar_t* tokenCharacters);
};