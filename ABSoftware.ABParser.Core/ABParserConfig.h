#ifndef _ABPARSER_INCLUDE_TOKEN_MANAGEMENT_H
#define _ABPARSER_INCLUDE_TOKEN_MANAGEMENT_H

#include "ABParserHelpers.h"
#include "Debugging.h"
#include <string>
#include <vector>
#include <wchar.h>
#include <cstring>
#include <memory>
#include <cstdarg>

template<typename T>
class UnorganizedTokenLimit {
public:
	T* Name;
	uint8_t NameLength;

	UnorganizedTokenLimit() {
		Name = nullptr;
		NameLength = 0;
	}

	~UnorganizedTokenLimit() {
		delete[] Name;
	}

	UnorganizedTokenLimit(T* name, uint8_t nameLength) {
		Init(name, nameLength);
	}

	void Init(T* name, uint8_t nameLength) {
		Name = new T[nameLength];
		for (uint8_t i = 0; i < nameLength; i++)
			Name[i] = name[i];

		NameLength = nameLength;
	}
};

template<typename T, typename U = char>
class ABParserToken {
public:

	const std::basic_string<U>* TokenName;

	UnorganizedTokenLimit<U>* Limits;
	uint16_t LimitsLength;

	std::shared_ptr<T[]> Data;
	uint16_t DataLength;

	ABParserToken() {
		TokenName = nullptr;
		Data = nullptr;
		DataLength = 0;
		LimitsLength = 0;
		Limits = nullptr;
	}

	ABParserToken<T, U>* SetName(const U* name) {
		TokenName = new const std::basic_string<U>(name);

		if (TokenName->size() > 255)
			throw "Tokens names cannot be longer than 255 characters.";

		return this;
	}

	ABParserToken<T, U>* SetName(U* name) {
		return SetName((const U*)name);
	}

	ABParserToken<T, U>* SetData(T* data, uint16_t dataLength) {
		Data = std::shared_ptr<T[]>(new T[dataLength], std::default_delete<T[]>());
		for (uint16_t i = 0; i < dataLength; i++)
			Data[i] = data[i];

		DataLength = dataLength;
		Limits = nullptr;
		LimitsLength = 0;

		return this;
	}

	ABParserToken<T, U>* SetData(const T* data, uint16_t dataLength) {
		SetData((T*)data, dataLength);
		return this;
	}

	ABParserToken<T, U>* SetLimits(uint16_t numberOfLimits, ...) {
		LimitsLength = numberOfLimits;
		Limits = new UnorganizedTokenLimit<U>[numberOfLimits];

		va_list args;
		va_start(args, numberOfLimits);

		for (uint16_t i = 0; i < numberOfLimits; i++) {
			U* item = va_arg(args, U*);

			size_t length = strlen(item);
			if (length > 255)
				throw "Limit Names can only be 255 characters long at a maximum";

			Limits[i].Init(item, (uint8_t)length);
		}

		return this;
	}

	ABParserToken<T, U>* SetLimits(U** limits, uint8_t* limitLengths, uint16_t numberOfLimits) {
		LimitsLength = numberOfLimits;
		Limits = new UnorganizedTokenLimit<U>[numberOfLimits];

		for (uint16_t i = 0; i < numberOfLimits; i++)
			Limits[i].Init(limits[i], limitLengths[i]);

		return this;
	}

	ABParserToken<T, U>* DirectSetLimits(UnorganizedTokenLimit<U>* limits, uint16_t numberOfLimits) {
		Limits = limits;
		LimitsLength = numberOfLimits;
		return this;
	}

	~ABParserToken() {
		delete[] Limits;
	}
};

template<typename T, typename U = char>
class TokenLimit {
public:
	std::basic_string<U>* LimitName;

	SingleCharToken<T>** SingleCharTokens;
	uint16_t NumberOfSingleCharTokens;
	MultiCharToken<T>** MultiCharTokens;
	uint16_t NumberOfMultiCharTokens;

	TokenLimit(U* limitName, uint8_t limitNameLength, uint16_t maximumAmountOfTokens) {
		SingleCharTokens = new SingleCharToken<T>*[maximumAmountOfTokens];
		MultiCharTokens = new MultiCharToken<T>*[maximumAmountOfTokens];
		NumberOfSingleCharTokens = 0;
		NumberOfMultiCharTokens = 0;

		LimitName = new std::basic_string<U>(limitName, limitNameLength);
	}

	~TokenLimit() {
		delete[] SingleCharTokens;
		delete[] MultiCharTokens;
	}
};

template<typename T, typename U = char>
class TriviaLimit {
public:
	const std::basic_string<U>* LimitName;

	T* ToIgnore;
	uint16_t ToIgnoreLength;

	void SetName(U* name) {
		LimitName = new const std::basic_string<U>(name);
	}

	void SetName(U* name, uint8_t length) {
		LimitName = new const std::basic_string<U>(name, length);
	}

	void SetName(const U* name) {
		SetName((U*)name);
	}

	void SetName(const std::basic_string<U>& name) {
		LimitName = new const std::basic_string<U>(name);
	}

	void SetIgnoreCharacters(uint16_t ignoreLength, ...) {
		ToIgnore = new T[ignoreLength];
		ToIgnoreLength = ignoreLength;

		va_list args;
		va_start(args, ignoreLength);

		for (uint16_t i = 0; i < ignoreLength; i++)
			ToIgnore[i] = va_arg(args, T);

		va_end(args);
	}

	void DirectSetIgnoreCharacters(T* chars, uint16_t charsLength) {
		ToIgnore = new T[charsLength];
		ToIgnoreLength = charsLength;

		for (uint16_t i = 0; i < charsLength; i++)
			ToIgnore[i] = chars[i];
	}
};

template<typename T, typename U = char>
class ABParserConfiguration {
public:
	SingleCharToken<T>** SingleCharTokens;
	uint16_t NumberOfSingleCharTokens;
	MultiCharToken<T>** MultiCharTokens;
	uint16_t NumberOfMultiCharTokens;

	TokenLimit<T, U>** TokenLimits;
	uint16_t NumberOfTokenLimits;

	TriviaLimit<T, U>* TriviaLimits;
	uint16_t NumberOfTriviaLimits;

	ABParserConfiguration(ABParserToken<T, U>* tokens, uint16_t numberOfTokens) {
		std::vector<TokenLimit<T, U>*> organizedTokens;

		// Initialize the arrays the results will go into - we try to set them to the maximum potentional size it could be.
		SingleCharTokens = new SingleCharToken<T>*[numberOfTokens];
		NumberOfSingleCharTokens = 0;

		MultiCharTokens = new MultiCharToken<T>*[numberOfTokens];
		NumberOfMultiCharTokens = 0;

		// One character big tokens are organized as "singleCharTokens" and multiple character-long tokens are "multiCharTokens".
		for (int i = 0; i < numberOfTokens; i++) {
			_ABP_DEBUG_OUT("Processing token %d", i);

			ABParserToken<T, U>* currentToken = &(tokens[i]);

			if (currentToken->DataLength == 1) {
				SingleCharTokens[NumberOfSingleCharTokens] = new SingleCharToken<T>();
				if (currentToken->Limits != nullptr)
					ProcessTokenLimits(currentToken->Limits, currentToken->LimitsLength, &organizedTokens, SingleCharTokens[NumberOfSingleCharTokens], true, numberOfTokens);
				SingleCharTokens[NumberOfSingleCharTokens]->MixedIdx = i;
				SingleCharTokens[NumberOfSingleCharTokens++]->TokenChar = currentToken->Data[0];
			}
			else {
				MultiCharTokens[NumberOfMultiCharTokens] = new MultiCharToken<T>();
				if (currentToken->Limits != nullptr)
					ProcessTokenLimits(currentToken->Limits, currentToken->LimitsLength, &organizedTokens, MultiCharTokens[NumberOfMultiCharTokens], false, numberOfTokens);
				MultiCharTokens[NumberOfMultiCharTokens]->MixedIdx = i;
				MultiCharTokens[NumberOfMultiCharTokens]->TokenContents = currentToken->Data;
				MultiCharTokens[NumberOfMultiCharTokens++]->TokenLength = currentToken->DataLength;
			}
		}

		uint16_t organizedTokensSize = (uint16_t)organizedTokens.size();

		NumberOfTokenLimits = organizedTokensSize;
		TokenLimits = new TokenLimit<T, U>*[organizedTokensSize];

		TokenLimit<T, U>** vectorData = organizedTokens.data();

		for (uint16_t i = 0; i < organizedTokensSize; i++)
			TokenLimits[i] = vectorData[i];

		TriviaLimits = nullptr;
		NumberOfTriviaLimits = 0;
	}

	~ABParserConfiguration() {
		for (int i = 0; i < NumberOfSingleCharTokens; i++)
			delete SingleCharTokens[i];
		for (int i = 0; i < NumberOfMultiCharTokens; i++)
			delete MultiCharTokens[i];

		delete[] SingleCharTokens;
		delete[] MultiCharTokens;
		delete[] TokenLimits;
	}

	ABParserConfiguration<T, U>* SetTriviaLimits(uint16_t numberOfTriviaLimits, TriviaLimit<T, U>* limits) {
		TriviaLimits = limits;
		NumberOfTriviaLimits = numberOfTriviaLimits;
		return this;
	}
private:
	void ProcessTokenLimits(UnorganizedTokenLimit<U>* unorganizedLimits, uint16_t numberOfUnorganizedLimits, std::vector<TokenLimit<T, U>*>* organizedLimits, ABParserInternalToken<T>* token, bool isSingleChar, uint16_t maximumAmountOfTokens) {

		uint16_t organizedTokenSize = (uint16_t)organizedLimits->size();

		for (uint16_t i = 0; i < numberOfUnorganizedLimits; i++) {

			int matchedIndex = -1;
			for (uint16_t j = 0; j < organizedTokenSize; j++)
				if (Matches(unorganizedLimits[i].Name, (U*)(*organizedLimits)[j]->LimitName->data(), unorganizedLimits[i].NameLength, (*organizedLimits)[j]->LimitName->length())) {
					matchedIndex = j;
					break;
				}

			// If there isn't already an organized limit for this.
			if (matchedIndex == -1) {
				(*organizedLimits).push_back(new TokenLimit<T, U>(unorganizedLimits[i].Name, unorganizedLimits[i].NameLength, maximumAmountOfTokens));

				if (isSingleChar)
					(*organizedLimits)[organizedTokenSize]->SingleCharTokens[(*organizedLimits)[organizedTokenSize]->NumberOfSingleCharTokens++] = (SingleCharToken<T>*)token;
				else
					(*organizedLimits)[organizedTokenSize]->MultiCharTokens[(*organizedLimits)[organizedTokenSize]->NumberOfMultiCharTokens++] = (MultiCharToken<T>*)token;

				organizedTokenSize++;
			}

			else if (isSingleChar)
				(*organizedLimits)[matchedIndex]->SingleCharTokens[(*organizedLimits)[matchedIndex]->NumberOfSingleCharTokens++] = (SingleCharToken<T>*)token;
			else
				(*organizedLimits)[matchedIndex]->MultiCharTokens[(*organizedLimits)[matchedIndex]->NumberOfMultiCharTokens++] = (MultiCharToken<T>*)token;
		}
	}
};
#endif