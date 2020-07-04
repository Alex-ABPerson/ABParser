#ifndef _ABPARSER_INCLUDE_HELPERS_H
#define _ABPARSER_INCLUDE_HELPERS_H

#include <stdint.h>
#include <memory>
#include <wchar.h>

namespace abparser {
	enum class ABParserResult : int {
		None,
		StopAndFinalOnTokenProcessed,
		FirstBeforeTokenProcessed,
		OnThenBeforeTokenProcessed,
		OnFirstUnlimitedCharacterProcessed
	};

	template<typename T>
	class ABParserInternalToken {
	public:

		// When we created an instance of ABParser, the single-char tokens and multi-char tokens were mixed together, this is at what index this token would've been mixed in.
		uint16_t MixedIdx = 0;
		virtual uint16_t GetLength() { return 0; }
		virtual bool IsSingleChar() { return false; }
	};

	template<typename T>
	class SingleCharToken : public ABParserInternalToken<T> {
	public:
		T TokenChar = 0;

		uint16_t GetLength() { return 1; }
		bool IsSingleChar() { return true; }
	};

	template<typename T>
	class MultiCharToken : public ABParserInternalToken<T> {
	public:
		T* DetectionLimit = nullptr;
		uint16_t DetectionLimitSize = 0;

		T* TokenContents = nullptr;
		uint32_t TokenLength = 0;

		uint16_t GetLength() { return TokenLength; }
		bool IsSingleChar() { return false; }
	};

	template<typename T>
	class ABParserFutureToken {
	public:
		// Due to detection limits, simply looking at the token's data isn't enough to determine how long the token is in the text (which needs to be done in places), so we add it up into this.
		uint32_t LengthInText;
		uint32_t NoOfCharactersMatched;

		MultiCharToken<T>* Token;
		bool CollectionComplete;
		bool Finished;
		bool IsBeingVerified;
		bool EndOfArray;

		ABParserFutureToken() {
			Reset(nullptr);
		}

		void Reset(MultiCharToken<T>* token) {
			Token = token;
			CollectionComplete = false;
			Finished = false;
			EndOfArray = false;
			IsBeingVerified = false;
			LengthInText = 0;
			NoOfCharactersMatched = 0;
		}
	};

	template<typename T>
	class ABParserVerifyToken {
	public:
		bool IsSingleChar;
		void* Token;

		ABParserFutureToken<T>** Triggers;
		uint32_t* TriggerStarts;

		// 0 if this token has been confirmed.
		uint16_t TriggersLength;

		T* TrailingBuildUp;
		uint32_t TrailingBuildUpLength;

		uint32_t Start;

		ABParserVerifyToken(void* token, bool isSingleChar, uint32_t start, T* trailingBuildUp) {
			Token = token;
			IsSingleChar = isSingleChar;

			Start = start;

			Triggers = nullptr;
			TriggerStarts = nullptr;
			TriggersLength = 0;

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
}
#endif