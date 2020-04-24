#include "HelperClasses.h"

int ABParserToken::GetLength() {
	return 0;
}

int SingleCharToken::GetLength() {
	return 1;
}

int MultiCharToken::GetLength() {
	return TokenLength;
}

ABParserFutureToken::ABParserFutureToken(MultiCharToken* token) {
	Token = token;
	Finished = false;
	Disabled = false;
}

ABParserVerifyToken::ABParserVerifyToken(SingleCharToken* singleChar, int start, wchar_t* trailingBuildUp) {
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

ABParserVerifyToken::ABParserVerifyToken(ABParserFutureToken* multiChar, int start, wchar_t* trailingBuildUp) {
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

ABParserVerifyToken::~ABParserVerifyToken() {
	delete[] Triggers;
	delete[] TriggerStarts;
	TrailingBuildUp = nullptr;
	SingleChar = nullptr;
	MultiChar = nullptr;
}