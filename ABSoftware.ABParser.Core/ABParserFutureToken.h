#pragma once
#include "MultiCharToken.h"

class ABParserFutureToken {
public:
	MultiCharToken* Token;
	bool Finished;

	ABParserFutureToken(MultiCharToken* token) {
		Token = token;
		Finished = false;
	}
};