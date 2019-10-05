#pragma once

class ABParserFutureToken {
public:
	int Token;
	bool Finished;

	ABParserFutureToken(int token) {
		Token = token;
		Finished = false;
	}
};