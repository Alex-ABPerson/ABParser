#pragma once
#include "ABParserToken.h"

class SingleCharToken : public ABParserToken {
public:
	// When we created an instance of ABParser, the single-char tokens and multi-char tokens were mixed together, this is at what index this token would've been mixed in.
	wchar_t TokenChar = 0;

	virtual int GetLength() {
		return 1;
	}

	//SingleCharToken(int mixedIdx, wchar_t tokenChar) {
	//	MixedIdx = mixedIdx;
	//	TokenChar = tokenChar;
	//}
};