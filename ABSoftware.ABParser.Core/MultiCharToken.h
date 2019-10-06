#pragma once
#include "ABParserToken.h"

class MultiCharToken : public ABParserToken {
public:
	// When we created an instance of ABParser, the single-char tokens and multi-char tokens were mixed together, this is at what index this token would've been mixed in.
	wchar_t* TokenContents;
	int TokenLength;

	//MultiCharToken(int mixedIdx, wchar_t* tokenContents, int tokenLength) {
	//	MixedIdx = mixedIdx;
	//	TokenContents = tokenContents;
	//	TokenLength = tokenLength;
	//}
};