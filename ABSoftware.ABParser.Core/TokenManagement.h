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
	uint8_t NameSize;

	UnorganizedTokenLimit() {
		Name = nullptr;
		NameSize = 0;
	}

	~UnorganizedTokenLimit() {
		delete[] Name;
	}

	UnorganizedTokenLimit(T* name, uint8_t nameSize) {
		Init(name, nameSize);
	}

	void Init(T* name, uint8_t nameSize) {
		Name = new T[nameSize];
		for (uint8_t i = 0; i < nameSize; i++)
			Name[i] = name[i];

		NameSize = nameSize;
	}
};

template<typename T, typename U = char>
class ABParserToken {
public:

	UnorganizedTokenLimit<U>* Limits;
	uint16_t LimitsSize;

	const std::basic_string<U>* TokenName;

	std::shared_ptr<T[]> Data;
	uint16_t DataSize;

	ABParserToken() {
		TokenName = nullptr;
		Data = nullptr;
		DataSize = 0;
		LimitsSize = 0;
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

	ABParserToken<T, U>* SetData(T* data, uint16_t dataSize) {
		Data = std::shared_ptr<T[]>(new T[dataSize], std::default_delete<T[]>());
		for (size_t i = 0; i < dataSize; i++)
			Data[i] = data[i];

		DataSize = dataSize;
		Limits = nullptr;
		LimitsSize = 0;

		return this;
	}

	ABParserToken<T, U>* SetData(const T* data, uint16_t dataSize) {
		SetData((T*)data, dataSize);
		return this;
	}

	ABParserToken<T, U>* SetLimits(uint16_t numberOfLimits, ...) {
		LimitsSize = numberOfLimits;
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

	ABParserToken<T, U>* SetLimits(U** limits, uint8_t* limitSizes, uint16_t numberOfLimits) {
		LimitsSize = numberOfLimits;
		Limits = new UnorganizedTokenLimit<U>[numberOfLimits];

		for (uint16_t i = 0; i < numberOfLimits; i++)
			Limits[i].Init(limits[i], limitSizes[i]);

		return this;
	}

	ABParserToken<T, U>* DirectSetLimits(UnorganizedTokenLimit<U>* limits, uint16_t numberOfLimits) {
		Limits = limits;
		LimitsSize = numberOfLimits;
		return this;
	}

	~ABParserToken() {
		delete[] Limits;
	}
};

template<typename T, typename U = char>
class TokenLimit {
public:
	const std::basic_string<U>* LimitName;
	uint8_t LimitNameSize;

	SingleCharToken<T>** SingleCharTokens;
	uint16_t NumberOfSingleCharTokens;
	MultiCharToken<T>** MultiCharTokens;
	uint16_t NumberOfMultiCharTokens;

	TokenLimit(const U* limitName, uint8_t limitNameSize, uint16_t maximumAmountOfTokens) {
		SingleCharTokens = new SingleCharToken<T>*[maximumAmountOfTokens];
		MultiCharTokens = new MultiCharToken<T>*[maximumAmountOfTokens];
		NumberOfSingleCharTokens = 0;
		NumberOfMultiCharTokens = 0;

		LimitName = new const std::basic_string<U>(limitName);
		LimitNameSize = limitNameSize;
	}

	~TokenLimit() {
		delete[] SingleCharTokens;
		delete[] MultiCharTokens;
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

	ABParserConfiguration(ABParserToken<T, U>* tokens, uint16_t numberOfTokens) {
		std::vector<TokenLimit<T, U>*> organizedTokens;

		// Initialize the arrays the results will go into - we try to set them to the maximum potentional size it could be.
		SingleCharToken<T>** singleCharTokens = new SingleCharToken<T> * [numberOfTokens];
		uint16_t singleCharTokensLength = 0;

		MultiCharToken<T>** multiCharTokens = new MultiCharToken<T> * [numberOfTokens];
		uint16_t multiCharTokensLength = 0;

		uint16_t currentTokenLimitPosition = 0;

		// One character big tokens are organized as "singleCharTokens" and multiple character-long tokens are "multiCharTokens".
		for (int i = 0; i < numberOfTokens; i++) {
			_ABP_DEBUG_OUT("Processing token %d", i);

			ABParserToken<T, U>* currentToken = &(tokens[i]);

			if (currentToken->DataSize == 1) {
				singleCharTokens[singleCharTokensLength] = new SingleCharToken<T>();
				if (currentToken->Limits != nullptr)
					ProcessTokenLimits(currentToken->Limits, currentToken->LimitsSize, &organizedTokens, singleCharTokens[singleCharTokensLength], true, numberOfTokens);
				singleCharTokens[singleCharTokensLength]->MixedIdx = i;
				singleCharTokens[singleCharTokensLength++]->TokenChar = currentToken->Data[0];
			}
			else {
				multiCharTokens[multiCharTokensLength] = new MultiCharToken<T>();
				if (currentToken->Limits != nullptr)
					ProcessTokenLimits(currentToken->Limits, currentToken->LimitsSize, &organizedTokens, multiCharTokens[multiCharTokensLength], false, numberOfTokens);
				multiCharTokens[multiCharTokensLength]->MixedIdx = i;
				multiCharTokens[multiCharTokensLength]->TokenContents = currentToken->Data;
				multiCharTokens[multiCharTokensLength++]->TokenLength = currentToken->DataSize;
			}
		}

		uint16_t organizedTokensSize = (uint16_t)organizedTokens.size();

		TokenLimit<T, U>** organizedTokensArr = new TokenLimit<T, U>*[organizedTokensSize];
		TokenLimit<T, U>** vectorData = organizedTokens.data();

		for (uint16_t i = 0; i < organizedTokensSize; i++)
			organizedTokensArr[i] = vectorData[i];

		SetTokensAndLimits(singleCharTokens, singleCharTokensLength, multiCharTokens, multiCharTokensLength, organizedTokensArr, organizedTokensSize);
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
private:
	template<typename T, typename U = char>
	void ProcessTokenLimits(UnorganizedTokenLimit<U>* unorganizedLimits, uint16_t numberOfUnorganizedLimits, std::vector<TokenLimit<T, U>*>* organizedLimits, ABParserInternalToken<T>* token, bool isSingleChar, uint16_t maximumAmountOfTokens) {

		uint16_t organizedTokenSize = (uint16_t)organizedLimits->size();

		for (uint16_t i = 0; i < numberOfUnorganizedLimits; i++) {

			int matchedIndex = -1;
			for (uint16_t j = 0; j < organizedTokenSize; j++)
				if (Matches(unorganizedLimits[i].Name, (U*)(*organizedLimits)[j]->LimitName->data(), unorganizedLimits[i].NameSize, (*organizedLimits)[j]->LimitNameSize)) {
					matchedIndex = j;
					break;
				}

			// If there isn't already an organized limit for this.
			if (matchedIndex == -1) {
				(*organizedLimits).push_back(new TokenLimit<T, U>(unorganizedLimits[i].Name, unorganizedLimits[i].NameSize, maximumAmountOfTokens));

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

	void SetTokensAndLimits(SingleCharToken<T>** singleCharTokens, uint16_t numberOfSingleCharTokens, MultiCharToken<T>** multiCharTokens, uint16_t numberOfMultiCharTokens, TokenLimit<T, U>** tokenLimits, uint16_t numberOfTokenLimits) {
		SingleCharTokens = singleCharTokens;
		NumberOfSingleCharTokens = numberOfSingleCharTokens;
		MultiCharTokens = multiCharTokens;
		NumberOfMultiCharTokens = numberOfMultiCharTokens;

		TokenLimits = tokenLimits;
		NumberOfTokenLimits = numberOfTokenLimits;
	}
};
//ABParserConfiguration<T>* CreateTokensFromRawData(T** tokens, size_t* tokenLengths, int numberOfTokens, T*** tokenLimitNames, int* tokenLimitSizes) {
#endif