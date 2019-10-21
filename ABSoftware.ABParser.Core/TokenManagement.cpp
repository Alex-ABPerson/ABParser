#include "TokenManagement.h"
#include "Debugging.h"
#include <cstring>

void SortTokens(unsigned short** tokens, int* tokenLengths, int numberOfTokens, SingleCharToken** outSingleCharTokens, MultiCharToken** outMultiCharTokens, int* numberOfSingleCharTokens, int* numberOfMultiCharTokens) {

	// We write to a temporary array first, because we don't know how many single character and multi character tokens there will be.
	// So, we will write to a temporary array that is the maximum number it could be, then copy that to a correctly-sized array.

	// Initialize the arrays the results will go into.
	SingleCharToken* singleCharTokens = new SingleCharToken[numberOfTokens];
	int singleCharTokensLength = 0;

	MultiCharToken* multiCharTokens = new MultiCharToken[numberOfTokens];
	int multiCharTokensLength = 0;

	// Go through each token, if it is only one character long, then put into the "singleCharTokens".
	for (int i = 0; i < numberOfTokens; i++) {
		debugLog("Processing token %d", i);
		if (tokenLengths[i] == 1) {
			singleCharTokens[singleCharTokensLength].MixedIdx = i;
			singleCharTokens[singleCharTokensLength++].TokenChar = *tokens[i];
		}
		else {
			multiCharTokens[multiCharTokensLength].MixedIdx = i;

			// Copy across the characters and the length.
			int length = tokenLengths[i];
			wchar_t** tokenContents = &multiCharTokens[multiCharTokensLength].TokenContents;
			*tokenContents = new wchar_t[length];
			for (int j = 0; j < length; j++)
				memcpy(&(*tokenContents)[j], &tokens[i][j], sizeof(unsigned short));
			multiCharTokens[multiCharTokensLength++].TokenLength = length;

		}
	}

	// Output those.
	*outSingleCharTokens = singleCharTokens;
	*outMultiCharTokens = multiCharTokens;
	*numberOfSingleCharTokens = singleCharTokensLength;
	*numberOfMultiCharTokens = multiCharTokensLength;
}