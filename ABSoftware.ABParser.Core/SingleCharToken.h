#pragma once

class SingleCharToken {
public:
	// When we created an instance of ABParser, the single-char tokens and multi-char tokens were mixed together, this is at what index this token would've been mixed in.
	int MixedIdx;
	wchar_t TokenChar;

	//SingleCharToken(int mixedIdx, wchar_t tokenChar) {
	//	MixedIdx = mixedIdx;
	//	TokenChar = tokenChar;
	//}
};