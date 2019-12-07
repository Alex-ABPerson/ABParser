#pragma once
#include "ABParserFutureToken.h"
#include "ABParserToken.h"

class ABParserVerifyToken {
public:
	SingleCharToken* SingleChar;
	ABParserFutureToken* MultiChar;
	bool IsSingleChar;
	int Start;

	ABParserVerifyToken(SingleCharToken* singleChar, int start) {
		SingleChar = singleChar;
		IsSingleChar = true;
		MultiChar = nullptr;
		Start = start;
	}

	ABParserVerifyToken(ABParserFutureToken* multiChar, int start) {
		SingleChar = nullptr;
		IsSingleChar = false;
		MultiChar = multiChar;
		Start = start;
	}

};