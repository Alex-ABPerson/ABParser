#pragma once

class ABParserToken {
public:
	// When we created an instance of ABParser, the single-char tokens and multi-char tokens were mixed together, this is at what index this token would've been mixed in.
	unsigned short MixedIdx = 0;
	virtual int GetLength() {
		return 0;
	}
};