#ifndef _ABPARSER_INCLUDE_ABPARSER_H
#define _ABPARSER_INCLUDE_ABPARSER_H
#include "ABParserBase.h"

template<typename T, typename U = char>
class BeforeTokenProcessedArgs {
public:
	ABParserToken<T, U>* PreviousToken;
	uint32_t PreviousTokenStart;

	ABParserToken<T, U>* Token;
	uint32_t TokenStart;

	T* Leading;
	uint32_t LeadingLength;

	BeforeTokenProcessedArgs(
		ABParserToken<T, U>* previousToken, 
		uint32_t  previousTokenStart, 
		ABParserToken<T, U>* token, 
		uint32_t tokenStart, 
		T* leading, 
		uint32_t leadingLength) {

		PreviousToken = previousToken;
		PreviousTokenStart = previousTokenStart;

		Token = token;
		TokenStart = tokenStart;

		Leading = leading;
		LeadingLength = leadingLength;
	}

	std::basic_string<T>* GetLeadingAsString() const { return new std::basic_string<T>(Leading); }
};

template<typename T, typename U = char>
class OnTokenProcessedArgs : public BeforeTokenProcessedArgs<T, U> {
public:
	ABParserToken<T, U>* NextToken;
	uint32_t NextTokenStart;

	T* Trailing;
	uint32_t TrailingLength;

	OnTokenProcessedArgs(
		ABParserToken<T, U>* previousToken,
		uint32_t previousTokenStart,
		ABParserToken<T, U>* token,
		uint32_t tokenStart,
		ABParserToken<T, U>* nextToken,
		uint32_t nextTokenStart,
		T* leading,
		uint32_t leadingLength,
		T* trailing,
		uint32_t trailingLength)
		: BeforeTokenProcessedArgs<T, U>(previousToken, previousTokenStart, token, tokenStart, leading, leadingLength) {

		NextToken = nextToken;
		NextTokenStart = nextTokenStart;

		Trailing = trailing;
		TrailingLength = trailingLength;
	}

	std::basic_string<T>* GetTrailingAsString() const { return new std::basic_string<T>(Trailing); }
};

template<typename T, typename U = char>
class ABParser {
public:
	ABParserBase<T, U> Base;
	ABParserToken<T, U>* Tokens;

	ABParser(ABParserConfiguration<T>* configuration, ABParserToken<T, U>* tokens) {
		Base.InitConfiguration(configuration);
		Tokens = tokens;
	}

	void SetText(T* text, uint32_t textLength) {
		Base.InitString(text, textLength);
	}

	void SetText(const T* text, uint32_t textLength) {
		SetText((T*)text, textLength);
	}

	void SetText(const std::basic_string<T>& text) {
		SetText((T*)text.c_str(), text.size());
	}

	void Execute() {
		ABParserResult result = ABParserResult::None;
		bool firstOTP = true;

		while (result != ABParserResult::StopAndFinalOnTokenProcessed) {
			result = Base.ContinueExecution();

			switch (result) {
			case ABParserResult::BeforeTokenProcessed:

				BeforeTokenProcessed(&BeforeTokenProcessedArgs<T, U>(nullptr, 0, &Tokens[Base.BeforeTokenProcessedToken->MixedIdx], Base.BeforeTokenProcessedTokenStart, Base.OnTokenProcessedTrailing, Base.OnTokenProcessedTrailingLength));

				break;
			case ABParserResult::OnThenBeforeTokenProcessed:
			{
				ABParserToken<T, U>* onTokenProcessedPreviousToken = firstOTP ? nullptr : &Tokens[Base.OnTokenProcessedPreviousToken->MixedIdx];
				ABParserToken<T, U>* onTokenProcessedToken = &Tokens[Base.OnTokenProcessedToken->MixedIdx];
				ABParserToken<T, U>* beforeTokenProcessedToken = &Tokens[Base.BeforeTokenProcessedToken->MixedIdx];

				BeforeTokenProcessedArgs<T, U> btpArgs(onTokenProcessedToken, Base.OnTokenProcessedTokenStart, beforeTokenProcessedToken, Base.BeforeTokenProcessedTokenStart, Base.OnTokenProcessedTrailing, Base.OnTokenProcessedTrailingLength);

				OnTokenProcessed(&OnTokenProcessedArgs<T, U>(onTokenProcessedPreviousToken, Base.OnTokenProcessedPreviousTokenStart, onTokenProcessedToken, Base.OnTokenProcessedTokenStart, beforeTokenProcessedToken, Base.BeforeTokenProcessedTokenStart, Base.OnTokenProcessedLeading, Base.OnTokenProcessedLeadingLength, Base.OnTokenProcessedTrailing, Base.OnTokenProcessedTrailingLength));
				BeforeTokenProcessed(&btpArgs);

				firstOTP = false;

				break;
			}
			case ABParserResult::StopAndFinalOnTokenProcessed:
			{
				if (!Base.OnTokenProcessedToken)
					return;

				ABParserToken<T, U>* onTokenProcessedPreviousToken = firstOTP ? nullptr : &Tokens[Base.OnTokenProcessedPreviousToken->MixedIdx];
				ABParserToken<T, U>* onTokenProcessedToken = &Tokens[Base.OnTokenProcessedToken->MixedIdx];

				OnTokenProcessed(&OnTokenProcessedArgs<T, U>(onTokenProcessedPreviousToken, Base.OnTokenProcessedPreviousTokenStart, onTokenProcessedToken, Base.OnTokenProcessedTokenStart, nullptr, 0, Base.OnTokenProcessedLeading, Base.OnTokenProcessedLeadingLength, Base.OnTokenProcessedTrailing, Base.OnTokenProcessedTrailingLength));
				break;
			}
			}
		}
	}

	void EnterTokenLimit(T* limitName, uint8_t limitNameSize) { Base.EnterTokenLimit(limitName, limitNameSize); }
	void EnterTokenLimit(const T* limitName, uint8_t limitNameSize) { EnterTokenLimit((T*)limitName, limitNameSize); }
	void EnterTokenLimit(std::basic_string<T>& limitName) { EnterTokenLimit((T*)limitName.data(), limitName.size()); }

	void ExitTokenLimit() { Base.ExitTokenLimit(); }

	virtual void OnStart() {}
	virtual void OnEnd(T* leading) {}
	virtual void BeforeTokenProcessed(BeforeTokenProcessedArgs<T, U>* args) {}
	virtual void OnTokenProcessed(OnTokenProcessedArgs<T, U>* args) {}
};

#endif