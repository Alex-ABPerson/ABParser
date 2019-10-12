#pragma once
#include "MultiCharToken.h"

class ABParserFutureToken {
public:
	MultiCharToken* Token;
	bool Finished;
	bool Disabled;

	ABParserFutureToken(MultiCharToken* token) {
		Token = token;
		Finished = false;
		Disabled = false;
	}
};