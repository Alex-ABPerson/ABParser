#pragma once
#include "ABParserToken.h"

class MultiCharToken : public ABParserToken {
public:
	wchar_t* TokenContents = 0;
	int TokenLength = 0;

	virtual int GetLength() {
		return TokenLength;
	}

	//MultiCharToken(int mixedIdx, wchar_t* tokenContents, int tokenLength) {
	//	MixedIdx = mixedIdx;
	//	TokenContents = tokenContents;
	//	TokenLength = tokenLength;
	//}
};