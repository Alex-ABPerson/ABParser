#ifndef _ABPARSER_INCLUDE_HELPERS_H
#define _ABPARSER_INCLUDE_HELPERS_H

#include <stdint.h>
#include <memory>
#include <wchar.h>

enum class ABParserResult : int {
	None,
	StopAndFinalOnTokenProcessed,
	BeforeTokenProcessed,
	OnAndBeforeTokenProcessed
};

template<typename T>
class ABParserInternalToken {
public:
	// When we created an instance of ABParser, the single-char tokens and multi-char tokens were mixed together, this is at what index this token would've been mixed in.
	uint16_t MixedIdx = 0;
	virtual size_t GetLength() { return 0; }
	virtual bool IsSingleChar() { return false; }
};

template<typename T>
class SingleCharToken : public ABParserInternalToken<T> {
public:
	T TokenChar = 0;

	size_t GetLength() { return 1; }
	bool IsSingleChar() { return true; }
};

template<typename T>
class MultiCharToken : public ABParserInternalToken<T> {
public:
	std::shared_ptr<T[]> TokenContents = 0;
	size_t TokenLength = 0;

	size_t GetLength() { return TokenLength; }
	bool IsSingleChar() { return false; }
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
	}
};

template<typename T>
bool Matches(T* first, T* second, size_t firstLength, size_t secondLength) {
	if (firstLength != secondLength) return false;

	for (size_t i = 0; i < firstLength; i++)
		if (first[i] != second[i])
			return false;

	return true;
}
#endif