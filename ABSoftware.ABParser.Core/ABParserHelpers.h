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
	virtual uint32_t GetLength() { return 0; }
	virtual bool IsSingleChar() { return false; }
};

template<typename T>
class SingleCharToken : public ABParserInternalToken<T> {
public:
	T TokenChar = 0;

	uint32_t GetLength() { return 1; }
	bool IsSingleChar() { return true; }
};

template<typename T>
class MultiCharToken : public ABParserInternalToken<T> {
public:
	std::shared_ptr<T[]> TokenContents = 0;
	uint32_t TokenLength = 0;

	uint32_t GetLength() { return TokenLength; }
	bool IsSingleChar() { return false; }
};

template<typename T>
class ABParserFutureToken {
public:
	MultiCharToken<T>* Token;
	bool Finished;
	bool Disabled;
	bool EndOfArray;

	ABParserFutureToken() {
		Reset(nullptr);
	}

	void Reset(MultiCharToken<T>* token) {
		Token = token;
		Finished = false;
		Disabled = false;
		EndOfArray = false;
	}
};

template<typename T>
class ABParserVerifyToken {
public:
	bool IsSingleChar;
	void* Token;

	ABParserFutureToken<T>** Triggers;
	uint16_t TriggersLength;

	uint32_t* TriggerStarts;
	uint16_t TriggerStartsLength;

	T* TrailingBuildUp;
	uint32_t TrailingBuildUpLength;

	uint32_t Start;

	ABParserVerifyToken(void* token, bool isSingleChar, uint32_t start, T* trailingBuildUp) {
		Token = token;
		IsSingleChar = isSingleChar;

		Start = start;

		Triggers = nullptr;
		TriggersLength = 0;

		TriggerStarts = nullptr;
		TriggerStartsLength = 0;

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