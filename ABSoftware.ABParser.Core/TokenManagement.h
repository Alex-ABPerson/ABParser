#ifndef _ABPARSER_INCLUDE_TOKEN_MANAGEMENT_H
#define _ABPARSER_INCLUDE_TOKEN_MANAGEMENT_H

#include "ABParserHelpers.h"
#include "Debugging.h"
#include <string>
#include <vector>
#include <wchar.h>
#include <cstring>
#include <memory>

template<typename T>
class UnorganizedTokenLimit {
public:
	// If the exact "Name" memory location gets used in the final "ABParserConfiguration" to represent the limit.
	std::shared_ptr<T[]> Name;
	uint8_t NameSize;

	UnorganizedTokenLimit() {
		Name = nullptr;
		NameSize = 0;
	}

	UnorganizedTokenLimit(T* name, uint8_t nameSize) {
		Init(name, nameSize);
	}

	void Init(T* name, uint8_t nameSize) {
		Name = std::shared_ptr<T[]>(new T[nameSize], std::default_delete<T[]>());
		for (uint8_t i = 0; i < nameSize; i++)
			Name[i] = name[i];

		NameSize = nameSize;
	}
};

template<typename T>
class ABParserToken {
public:

	UnorganizedTokenLimit<T>* Limits;
	uint16_t LimitsSize;

	std::shared_ptr<T[]> Data;
	uint16_t DataSize;

	ABParserToken() {
		Data = nullptr;
		DataSize = 0;
		LimitsSize = 0;
		Limits = nullptr;
	}

	ABParserToken(T* data, uint16_t dataSize) {
		Init(data, dataSize);
	}

	ABParserToken(const T* data, uint16_t dataSize) {
		Init((T*)data, dataSize);
	}

	ABParserToken(std::basic_string<T> str) {
		Init(str.data(), str.size());
	}

	void Init(T* data, uint16_t dataSize) {
		Data = std::shared_ptr<T[]>(new T[dataSize], std::default_delete<T[]>());
		for (size_t i = 0; i < dataSize; i++)
			Data[i] = data[i];

		DataSize = dataSize;
		Limits = nullptr;
		LimitsSize = 0;
	}

	ABParserToken<T>* SetLimits(UnorganizedTokenLimit<T>* limits, uint16_t numberOfLimits) {
		Limits = limits;
		LimitsSize = numberOfLimits;
		return this;
	}

	~ABParserToken() {
		delete[] Limits;
	}
};

template<typename T>
class TokenLimit {
public:
	std::shared_ptr<T[]> LimitName;
	uint8_t LimitNameSize;

	SingleCharToken<T>** SingleCharTokens;
	uint16_t NumberOfSingleCharTokens;
	MultiCharToken<T>** MultiCharTokens;
	uint16_t NumberOfMultiCharTokens;

	TokenLimit(std::shared_ptr<T[]> limitName, uint8_t limitNameSize, uint16_t maximumAmountOfTokens) {
		SingleCharTokens = new SingleCharToken<T>*[maximumAmountOfTokens];
		MultiCharTokens = new MultiCharToken<T>*[maximumAmountOfTokens];
		NumberOfSingleCharTokens = 0;
		NumberOfMultiCharTokens = 0;

		LimitName = limitName;
		LimitNameSize = limitNameSize;
	}

	~TokenLimit() {
		delete[] SingleCharTokens;
		delete[] MultiCharTokens;
	}
};

template<typename T>
class ABParserConfiguration {
public:
	SingleCharToken<T>** SingleCharTokens;
	uint16_t NumberOfSingleCharTokens;
	MultiCharToken<T>** MultiCharTokens;
	uint16_t NumberOfMultiCharTokens;

	TokenLimit<T>** TokenLimits;
	uint16_t NumberOfTokenLimits;

	ABParserConfiguration(SingleCharToken<T>** singleCharTokens, uint16_t numberOfSingleCharTokens, MultiCharToken<T>** multiCharTokens, uint16_t numberOfMultiCharTokens, TokenLimit<T>** tokenLimits, uint16_t numberOfTokenLimits) {
		SingleCharTokens = singleCharTokens;
		NumberOfSingleCharTokens = numberOfSingleCharTokens;
		MultiCharTokens = multiCharTokens;
		NumberOfMultiCharTokens = numberOfMultiCharTokens;

		TokenLimits = tokenLimits;
		NumberOfTokenLimits = numberOfTokenLimits;
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
};

template<typename T>
void ProcessTokenLimits(UnorganizedTokenLimit<T>* unorganizedLimits, uint16_t numberOfUnorganizedLimits, std::vector<TokenLimit<T>*>* organizedLimits, ABParserInternalToken<T>* token, bool isSingleChar, uint16_t maximumAmountOfTokens) {

	uint16_t organizedTokenSize = (uint16_t)organizedLimits->size();

	for (uint16_t i = 0; i < numberOfUnorganizedLimits; i++) {

		int matchedIndex = -1;
		for (uint16_t j = 0; j < organizedTokenSize; j++)
			if (Matches(unorganizedLimits[i].Name.get(), (*organizedLimits)[j]->LimitName.get(), unorganizedLimits[i].NameSize, (*organizedLimits)[j]->LimitNameSize)) {
				matchedIndex = j;
				break;
			}

		// If there isn't already an organized limit for this.
		if (matchedIndex == -1) {
			(*organizedLimits).push_back(new TokenLimit<T>(unorganizedLimits[i].Name, unorganizedLimits[i].NameSize, maximumAmountOfTokens));

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
//ABParserConfiguration<T>* CreateTokensFromRawData(T** tokens, size_t* tokenLengths, int numberOfTokens, T*** tokenLimitNames, int* tokenLimitSizes) {

template<typename T>
ABParserConfiguration<T>* CreateConfiguration(ABParserToken<T>* tokens, uint16_t numberOfTokens) {

	std::vector<TokenLimit<T>*> organizedTokens;

	// Initialize the arrays the results will go into - we try to set them to the maximum potentional size it could be.
	SingleCharToken<T>** singleCharTokens = new SingleCharToken<T>*[numberOfTokens];
	uint16_t singleCharTokensLength = 0;

	MultiCharToken<T>** multiCharTokens = new MultiCharToken<T>*[numberOfTokens];
	uint16_t multiCharTokensLength = 0;

	uint16_t currentTokenLimitPosition = 0;

	// One character big tokens are organized as "singleCharTokens" and multiple character-long tokens are "multiCharTokens".
	for (int i = 0; i < numberOfTokens; i++) {
		_ABP_DEBUG_OUT("Processing token %d", i);

		ABParserToken<T>* currentToken = &(tokens[i]);

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

	TokenLimit<T>** organizedTokensArr = new TokenLimit<T>*[organizedTokensSize];
	TokenLimit<T>** vectorData = organizedTokens.data();

	for (uint16_t i = 0; i < organizedTokensSize; i++)
		organizedTokensArr[i] = vectorData[i];

	return new ABParserConfiguration<T>(singleCharTokens, singleCharTokensLength, multiCharTokens, multiCharTokensLength, organizedTokensArr, organizedTokensSize);
}
#endif