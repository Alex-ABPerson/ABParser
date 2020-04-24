#pragma once
#include "ABParserFutureToken.h"
#include "ABParserToken.h"

class ABParserVerifyToken {
public:
	bool IsSingleChar;
	SingleCharToken* SingleChar;
	ABParserFutureToken* MultiChar;

	ABParserFutureToken** Triggers;
	int TriggersLength;

	wchar_t* TrailingBuildUp;
	int TrailingBuildUpLength;

	int* TriggerStarts;
	int TriggerStartsLength;

	int Start;
	bool HasNoTriggers;

	ABParserVerifyToken(SingleCharToken* singleChar, int start, wchar_t* trailingBuildUp) {
		SingleChar = singleChar;
		IsSingleChar = true;
		MultiChar = nullptr;
		Start = start;
		Triggers = nullptr;
		TriggersLength = 0;
		TriggerStarts = nullptr;
		TriggerStartsLength = 0;
		HasNoTriggers = false;
		TrailingBuildUp = trailingBuildUp;
		TrailingBuildUpLength = 0;
	}

	ABParserVerifyToken(ABParserFutureToken* multiChar, int start, wchar_t* trailingBuildUp) {
		SingleChar = nullptr;
		IsSingleChar = false;
		MultiChar = multiChar;
		Start = start;
		Triggers = nullptr;
		TriggersLength = 0;
		TriggerStarts = nullptr;
		TriggerStartsLength = 0;
		HasNoTriggers = false;
		TrailingBuildUp = trailingBuildUp;
		TrailingBuildUpLength = 0;
	}

	~ABParserVerifyToken() {
		delete[] Triggers;
		delete[] TriggerStarts;
		TrailingBuildUp = nullptr;
		SingleChar = nullptr;
		MultiChar = nullptr;

	}

};