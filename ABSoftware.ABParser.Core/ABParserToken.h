#pragma once

class ABParserToken {
public:
	unsigned short MixedIdx = 0;
	virtual int GetLength() {
		return 0;
	}
};