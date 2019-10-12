#pragma once
#include "SingleCharToken.h"
#include "MultiCharToken.h"

class VerifyToken {
public:
	bool IsSingleChar;
	SingleCharToken* SingleChar;
	MultiCharToken* MultiChar;

	VerifyToken(SingleCharToken* singleChar) {
		IsSingleChar = false;
		SingleChar = singleChar;
		MultiChar = 0;
	}

	VerifyToken(MultiCharToken* multiChar) {
		IsSingleChar = true;
		MultiChar = multiChar;
		SingleChar = 0;
	}
};