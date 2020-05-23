#ifndef _ABPARSER_INCLUDE_TOKEN_MANAGED_H
#define _ABPARSER_INCLUDE_TOKEN_MANAGED_H

#include "HelperClasses.h"
#include "Debugging.h"

template<typename T>
class TokensInformation {
public:
	SingleCharToken<T>** SingleCharTokens;
	int NumberOfSingleCharTokens;
	MultiCharToken<T>** MultiCharTokens;
	int NumberOfMultiCharTokens;

	TokensInformation(SingleCharToken<T>** singleCharTokens, int numberOfSingleCharTokens, MultiCharToken<T>** multiCharTokens, int numberOfMultiCharTokens) {
		SingleCharTokens = singleCharTokens;
		NumberOfSingleCharTokens = numberOfSingleCharTokens;
		MultiCharTokens = multiCharTokens;
		NumberOfMultiCharTokens = numberOfMultiCharTokens;
	}
};

template<typename T>
TokensInformation<T>* CreateTokens(T** tokens, int* tokenLengths, int numberOfTokens) {

	// Initialize the arrays the results will go into - we try to set them to the maximum potentional size it could be.
	SingleCharToken<T>** singleCharTokens = new SingleCharToken<T> *[numberOfTokens];
	int singleCharTokensLength = 0;

	MultiCharToken<T>** multiCharTokens = new MultiCharToken<T>*[numberOfTokens];
	int multiCharTokensLength = 0;

	// One character big tokens are organized as "singleCharTokens" and multiple character-long tokens are "multiCharTokens".
	for (int i = 0; i < numberOfTokens; i++) {
		debugLog("Processing token %d", i);

		if (tokenLengths[i] == 1) {
			singleCharTokens[singleCharTokensLength] = new SingleCharToken<T>();
			singleCharTokens[singleCharTokensLength]->MixedIdx = i;
			singleCharTokens[singleCharTokensLength]->TokenChar = 0;
			singleCharTokens[singleCharTokensLength++]->TokenChar = *tokens[i];
		}
		else {
			multiCharTokens[multiCharTokensLength] = new MultiCharToken<T>();
			multiCharTokens[multiCharTokensLength]->MixedIdx = i;

			int length = tokenLengths[i];

			T** tokenContents = &multiCharTokens[multiCharTokensLength]->TokenContents;
			*tokenContents = new T[length];

			for (int j = 0; j < length; j++)
				(*tokenContents)[j] = tokens[i][j];

			multiCharTokens[multiCharTokensLength++]->TokenLength = length;
		}
	}

	// Output those.
	return new TokensInformation<T>(singleCharTokens, singleCharTokensLength, multiCharTokens, multiCharTokensLength);
}
#endif