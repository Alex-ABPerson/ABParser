#ifndef _ABPARSER_INCLUDE_HELPER_CLASSES_H
#define _ABPARSER_INCLUDE_HELPER_CLASSES_H

#include <stdint.h>

enum ABParserResult {
	None,
	StopAndFinalOnTokenProcessed,
	BeforeTokenProcessed,
	OnAndBeforeTokenProcessed
};

class ABParserToken {
public:
	// When we created an instance of ABParser, the single-char tokens and multi-char tokens were mixed together, this is at what index this token would've been mixed in.
	uint16_t MixedIdx = 0;
	virtual int GetLength() { return 0; }
};

template<typename T>
class SingleCharToken : public ABParserToken {
public:
	T TokenChar = 0;

	int GetLength() { return 1; }
};

template<typename T>
class MultiCharToken : public ABParserToken {
public:
	T* TokenContents = 0;
	int TokenLength = 0;

	int GetLength() { return TokenLength; }
};

template<typename T>
class ABParserFutureToken {
public:
	MultiCharToken<T>* Token;
	bool Finished;
	bool Disabled;

	ABParserFutureToken(MultiCharToken<T>* token) {
		Token = token;
		Finished = false;
		Disabled = false;
	}
};

template<typename T>
class ABParserVerifyToken {
public:
	bool IsSingleChar;
	SingleCharToken<T>* SingleChar;
	ABParserFutureToken<T>* MultiChar;

	ABParserFutureToken<T>** Triggers;
	int TriggersLength;

	T* TrailingBuildUp;
	int TrailingBuildUpLength;

	int* TriggerStarts;
	int TriggerStartsLength;

	int Start;
	bool HasNoTriggers;

	ABParserVerifyToken(SingleCharToken<T>* singleChar, int start, T* trailingBuildUp) {
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

	ABParserVerifyToken(ABParserFutureToken<T>* multiChar, int start, T* trailingBuildUp) {
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
#endif