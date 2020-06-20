#ifndef _ABPARSER_INCLUDE_ABPARSER_H
#define _ABPARSER_INCLUDE_ABPARSER_H
#include "ABParserBase.h"

namespace abparser {
	template<typename T, typename U = char>
	class TokenInformation {
	public:
		uint32_t Start;
		uint32_t Length;
		ABParserToken<T, U>* Token;
	};

	template<typename T, typename U = char>
	class BeforeTokenProcessedArgs {
	public:
		const TokenInformation<T, U>* PreviousToken;
		const TokenInformation<T, U>* Token;

		T* Leading;
		uint32_t LeadingLength;

		BeforeTokenProcessedArgs(const TokenInformation<T, U>* previousToken, const TokenInformation<T, U>* token, T* leading, uint32_t leadingLength) {

			PreviousToken = previousToken;
			Token = token;

			Leading = leading;
			LeadingLength = leadingLength;
		}

		const std::basic_string<T>* GetLeadingAsString() const { return new const std::basic_string<T>(Leading, (size_t)LeadingLength); }
	};

	template<typename T, typename U = char>
	class OnTokenProcessedArgs : public BeforeTokenProcessedArgs<T, U> {
	public:
		const TokenInformation<T, U>* NextToken;

		T* Trailing;
		uint32_t TrailingLength;

		OnTokenProcessedArgs(const TokenInformation<T, U>* previousToken, const TokenInformation<T, U>* token, const TokenInformation<T, U>* nextToken, T* leading, uint32_t leadingLength, T* trailing, uint32_t trailingLength)
			: BeforeTokenProcessedArgs<T, U>(previousToken, token, leading, leadingLength) {

			NextToken = nextToken;

			Trailing = trailing;
			TrailingLength = trailingLength;
		}

		const std::basic_string<T>* GetTrailingAsString() const { return new const std::basic_string<T>(Trailing, TrailingLength); }
	};

	template<typename T, typename U = char>
	class ABParser {
	public:
		ABParserBase<T, U> Base;
		ABParserToken<T, U>* Tokens;
		T* Leading;
		uint32_t LeadingLength;

		ABParser(ABParserConfiguration<T, U>* configuration, ABParserToken<T, U>* tokens) {
			Base.InitConfiguration(configuration);
			Tokens = tokens;

			Leading = nullptr;
			LeadingLength = 0;
		}

		void SetText(T* text, uint32_t textLength) {

			// Reallocate the "Leading", if it isn't big enough to contain our new text.
			if (Base.TextLength < textLength) {
				if (Leading != nullptr)
					delete[] Leading;

				Leading = new T[(size_t)textLength + 1];
			}

			Base.InitString(text, textLength);
		}

		void SetText(const T* text, uint32_t textLength) {
			SetText((T*)text, textLength);
		}

		void SetText(const std::basic_string<T>& text) {
			SetText((T*)text.c_str(), (uint32_t)text.size());
		}

		void Start() {

			OnStart();

			TokenInformation<T, U>* swap;

			TokenInformation<T, U> infoStorage[3];
			TokenInformation<T, U>* otpPreviousToken = &infoStorage[0];
			TokenInformation<T, U>* otpToken = &infoStorage[1];
			TokenInformation<T, U>* otpNextToken = &infoStorage[2];

			ABParserResult result = ABParserResult::None;
			bool firstOTP = true;

			while (result != ABParserResult::StopAndFinalOnTokenProcessed) {

				// Swap the Leading with the CurrentTrivia.
				T* triviaSwap = Leading;
				Leading = Base.CurrentTrivia;
				Base.CurrentTrivia = triviaSwap;

				result = Base.ContinueExecution();

				swap = otpPreviousToken;
				otpPreviousToken = otpToken;
				otpToken = otpNextToken;
				otpNextToken = swap;

				otpNextToken->Token = &Tokens[Base.CurrentEventToken->MixedIdx];
				otpNextToken->Start = Base.CurrentEventTokenStart;
				otpNextToken->Length = Base.CurrentEventTokenLengthInText;

				switch (result) {
				case ABParserResult::FirstBeforeTokenProcessed:

					BeforeTokenProcessed(BeforeTokenProcessedArgs<T, U>(nullptr, otpNextToken, Base.CurrentTrivia, Base.CurrentTriviaLength));

					break;
				case ABParserResult::OnThenBeforeTokenProcessed:
				{

					OnTokenProcessed(OnTokenProcessedArgs<T, U>(firstOTP ? nullptr : otpPreviousToken, otpToken, otpNextToken, Leading, LeadingLength, Base.CurrentTrivia, Base.CurrentTriviaLength));
					BeforeTokenProcessed(BeforeTokenProcessedArgs<T, U>(otpToken, otpNextToken, Base.CurrentTrivia, Base.CurrentTriviaLength));

					firstOTP = false;

					break;
				}
				case ABParserResult::StopAndFinalOnTokenProcessed:
				{
					if (!Base.CurrentEventToken)
						return;

					OnTokenProcessed(OnTokenProcessedArgs<T, U>(otpPreviousToken, otpToken, nullptr, Leading, LeadingLength, Base.CurrentTrivia, Base.CurrentTriviaLength));
					break;
				}
				}
			}

			OnEnd(Base.CurrentEventToken ? Base.CurrentTrivia : Base.Text, Base.CurrentEventToken ? Base.CurrentTriviaLength : Base.TextLength);
		}

		void EnterTokenLimit(const T* limitName, uint8_t limitNameSize) { Base.EnterTokenLimit(limitName, limitNameSize); }
		void EnterTokenLimit(std::basic_string<T>& limitName) { EnterTokenLimit(limitName.data(), limitName.size()); }

		void ExitTokenLimit() { Base.ExitTokenLimit(); }

		void EnterTriviaLimit(const T* limitName, uint8_t limitNameSize) { Base.EnterTriviaLimit(limitName, limitNameSize); }
		void EnterTriviaLimit(std::basic_string<T>& limitName) { EnterTriviaLimit(limitName.data(), limitName.size()); }

		void ExitTriviaLimit() { Base.ExitTriviaLimit(); }

		virtual void OnStart() {}
		virtual void OnEnd(T* leading, uint32_t leadingLength) {}
		virtual void BeforeTokenProcessed(const BeforeTokenProcessedArgs<T, U>& args) {}
		virtual void OnTokenProcessed(const OnTokenProcessedArgs<T, U>& args) {}
	};
}
#endif