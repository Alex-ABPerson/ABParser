#ifndef _ABPARSER_INCLUDE_TOKEN_MANAGEMENT_H
#define _ABPARSER_INCLUDE_TOKEN_MANAGEMENT_H

#include "ABParserHelpers.h"
#include "ABParserDebugging.h"
#include <string>
#include <wchar.h>
#include <vector>
#include <cstdarg>
#include <unordered_map>

namespace abparser {
	template<typename T, typename U = char>
	class ABParserToken {
	public:

		T* DetectionLimit;
		uint16_t DetectionLimitSize;

		const std::basic_string<U>* Name;

		const std::basic_string<U>** Limits;
		uint16_t LimitsLength;

		T* Data;
		uint16_t DataLength;

		ABParserToken() {
			Name = nullptr;
			Data = nullptr;
			DataLength = 0;
			LimitsLength = 0;
			Limits = nullptr;

			DetectionLimit = nullptr;
			DetectionLimitSize = 0;
		}

		ABParserToken<T, U>* SetName(const std::basic_string<U>& name) {
			Name = new const std::basic_string<U>(name);

			if (Name->size() > 255)
				throw "Tokens names cannot be longer than 255 characters.";

			return this;
		}

		ABParserToken<T, U>* SetData(T* data, uint16_t dataLength) {
			Data = new T[dataLength];
			for (uint16_t i = 0; i < dataLength; i++)
				Data[i] = data[i];

			DataLength = dataLength;
			return this;
		}

		ABParserToken<T, U>* SetData(const T* data, uint16_t dataLength) {
			return SetData((T*)data, dataLength);
		}

		ABParserToken<T, U>* SetTokenLimits(uint16_t numberOfLimits, ...) {
			LimitsLength = numberOfLimits;
			Limits = new const std::basic_string<U>*[numberOfLimits];

			va_list args;
			va_start(args, numberOfLimits);

			for (uint16_t i = 0; i < numberOfLimits; i++) {
				const std::basic_string<U>& item = va_arg(args, const std::basic_string<U>&);
				Limits[i] = new const std::basic_string<U>(item);

				if (Limits[i].size() > 255)
					throw "Limit Names can only be 255 characters long at a maximum.";
			}

			return this;
		}

		ABParserToken<T, U>* SetTokenLimits(const std::basic_string<U>** limits, uint16_t numberOfLimits, ...) {
			LimitsLength = numberOfLimits;
			Limits = new const std::basic_string<U>*[numberOfLimits];

			for (uint16_t i = 0; i < numberOfLimits; i++) {
				if (Limits[i].size() > 255)
					throw "Limit Names can only be 255 characters long at a maximum.";

				Limits[i] = new const std::basic_string<U>(limits[i]);
			}

			return this;
		}

		ABParserToken<T, U>* SetDetectionLimit(uint16_t numberOfLimitChars, ...) {
			DetectionLimit = new T[numberOfLimitChars];
			
			va_list args;
			va_start(args, numberOfLimitChars);

			for (uint32_t i = 0; i < numberOfLimitChars; i++)
				DetectionLimit[i] = va_arg(args, T);

			va_end(args);
			return this;
		}

		ABParserToken<T, U>* DirectSetDetectionLimit(T* chars, uint16_t numberOfLimitChars) {
			DetectionLimit = new T[numberOfLimitChars];

			for (uint16_t i = 0; i < numberOfLimitChars; i++)
				DetectionLimit[i] = chars[i];

			DetectionLimitSize = numberOfLimitChars;
			return this;
		}

		~ABParserToken() {
			delete Name;
			delete[] Data;

			if (Limits != nullptr) {
				for (uint32_t i = 0; i < LimitsLength; i++)
					delete[] Limits[i];
				delete[] Limits;
			}

			if (DetectionLimit != nullptr)
				delete[] DetectionLimit;
		}
	};

	template<typename T>
	class TokenLimit {
	public:
		SingleCharToken<T>** SingleCharTokens;
		uint16_t NumberOfSingleCharTokens;
		MultiCharToken<T>** MultiCharTokens;
		uint16_t NumberOfMultiCharTokens;

		TokenLimit(uint16_t maximumAmountOfTokens) {
			SingleCharTokens = new SingleCharToken<T>*[maximumAmountOfTokens];
			MultiCharTokens = new MultiCharToken<T>*[maximumAmountOfTokens];
			NumberOfSingleCharTokens = 0;
			NumberOfMultiCharTokens = 0;
		}

		~TokenLimit() {
			delete[] SingleCharTokens;
			delete[] MultiCharTokens;
		}
	};

	template<typename T>
	class TriviaLimit {
	public:
		T* Data;
		uint16_t DataLength;
		bool IsWhitelist;

		TriviaLimit() {
			Data = nullptr;
			DataLength = 0;
			IsWhitelist = false;
		}

		void SetIsWhitelist(bool bl) {
			IsWhitelist = bl;
		}

		void SetData(uint16_t ignoreLength, ...) {
			Data = new T[ignoreLength];
			DataLength = ignoreLength;

			va_list args;
			va_start(args, ignoreLength);

			for (uint16_t i = 0; i < ignoreLength; i++)
				Data[i] = va_arg(args, T);

			va_end(args);
		}

		void DirectSetData(T* chars, uint16_t charsLength) {
			Data = new T[charsLength];
			DataLength = charsLength;

			for (uint16_t i = 0; i < charsLength; i++)
				Data[i] = chars[i];
		}

		~TriviaLimit() {
			delete[] Data;
		}
	};

	template<typename T, typename U = char>
	class ABParserConfiguration {
	public:
		SingleCharToken<T>** SingleCharTokens;
		uint16_t NumberOfSingleCharTokens;
		MultiCharToken<T>** MultiCharTokens;
		uint16_t NumberOfMultiCharTokens;

		std::unordered_map<std::basic_string<U>, TokenLimit<T>*> TokenLimits;
		std::unordered_map<std::basic_string<U>, TriviaLimit<T>*> TriviaLimits;

		ABParserConfiguration() {
			SingleCharTokens = nullptr;
			NumberOfSingleCharTokens = 0;

			MultiCharTokens = nullptr;
			NumberOfMultiCharTokens = 0;
		}

		ABParserConfiguration(ABParserToken<T, U>* tokens, uint16_t numberOfTokens) {
			Init(tokens, numberOfTokens);
		}
		
		void Init(ABParserToken<T, U>* tokens, uint16_t numberOfTokens) {
			// Initialize the arrays the results will go into - we try to set them to the maximum potentional size it could be.
			SingleCharTokens = new SingleCharToken<T>*[numberOfTokens];
			NumberOfSingleCharTokens = 0;

			MultiCharTokens = new MultiCharToken<T>*[numberOfTokens];
			NumberOfMultiCharTokens = 0;

			TokenLimits.reserve(numberOfTokens);

			// One character big tokens are organized as "singleCharTokens" and multiple character-long tokens are "multiCharTokens".
			for (int i = 0; i < numberOfTokens; i++) {
				_ABP_DEBUG_OUT("Processing token %d", i);

				ABParserToken<T, U>* CurrentEventToken = &(tokens[i]);

				if (CurrentEventToken->DataLength == 1) {
					SingleCharTokens[NumberOfSingleCharTokens] = new SingleCharToken<T>();
					if (CurrentEventToken->Limits != nullptr)
						ProcessTokenLimits(CurrentEventToken->Limits, CurrentEventToken->LimitsLength, SingleCharTokens[NumberOfSingleCharTokens], true, numberOfTokens);
					SingleCharTokens[NumberOfSingleCharTokens]->MixedIdx = i;
					SingleCharTokens[NumberOfSingleCharTokens++]->TokenChar = CurrentEventToken->Data[0];
				}
				else {
					MultiCharTokens[NumberOfMultiCharTokens] = new MultiCharToken<T>();
					if (CurrentEventToken->Limits != nullptr)
						ProcessTokenLimits(CurrentEventToken->Limits, CurrentEventToken->LimitsLength, MultiCharTokens[NumberOfMultiCharTokens], false, numberOfTokens);
					MultiCharTokens[NumberOfMultiCharTokens]->MixedIdx = i;
					MultiCharTokens[NumberOfMultiCharTokens]->TokenContents = CurrentEventToken->Data;
					MultiCharTokens[NumberOfMultiCharTokens]->DetectionLimit = CurrentEventToken->DetectionLimit;
					MultiCharTokens[NumberOfMultiCharTokens]->DetectionLimitSize = CurrentEventToken->DetectionLimitSize;
					MultiCharTokens[NumberOfMultiCharTokens++]->TokenLength = CurrentEventToken->DataLength;
				}
			}
		}

		~ABParserConfiguration() {
			if (SingleCharTokens != nullptr) {
				for (int i = 0; i < NumberOfSingleCharTokens; i++)
					delete SingleCharTokens[i];

				delete[] SingleCharTokens;
			}

			if (MultiCharTokens != nullptr) {
				for (int i = 0; i < NumberOfMultiCharTokens; i++)
					delete MultiCharTokens[i];

				delete[] MultiCharTokens;
			}
		}
	private:
		void ProcessTokenLimits(const std::basic_string<U>** unorganizedLimits, uint16_t numberOfUnorganizedLimits, ABParserInternalToken<T>* token, bool isSingleChar, uint16_t maximumAmountOfTokens) {

			for (uint16_t i = 0; i < numberOfUnorganizedLimits; i++) {

				auto item = TokenLimits.find(*(unorganizedLimits[i]));

				// If there isn't already an organized limit for this, add one.
				if (item == TokenLimits.end()) {
					TokenLimit<T>* limit = new TokenLimit<T>(maximumAmountOfTokens);

					if (isSingleChar)
						limit->SingleCharTokens[limit->NumberOfSingleCharTokens++] = (SingleCharToken<T>*)token;
					else
						limit->MultiCharTokens[limit->NumberOfMultiCharTokens++] = (MultiCharToken<T>*)token;

					TokenLimits.emplace(std::move(*(unorganizedLimits[i])), limit);
				}

				else if (isSingleChar)
					item->second->SingleCharTokens[item->second->NumberOfSingleCharTokens++] = (SingleCharToken<T>*)token;
				else
					item->second->MultiCharTokens[item->second->NumberOfMultiCharTokens++] = (MultiCharToken<T>*)token;
			}
		}
	};
}
#endif