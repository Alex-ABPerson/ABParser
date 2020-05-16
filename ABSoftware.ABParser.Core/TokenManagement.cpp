#include "TokenManagement.h"
#include "Debugging.h"
#include <cstring>

void SortTokens(unsigned short** tokens, int* tokenLengths, int numberOfTokens, SingleCharToken** outSingleCharTokens, MultiCharToken** outMultiCharTokens, int* numberOfSingleCharTokens, int* numberOfMultiCharTokens) {

	// Initialize the arrays the results will go into - we try to set them to the maximum potentional size it could be.
	SingleCharToken* singleCharTokens = new SingleCharToken[numberOfTokens];
	int singleCharTokensLength = 0;

	MultiCharToken* multiCharTokens = new MultiCharToken[numberOfTokens];
	int multiCharTokensLength = 0;

	// One character big tokens are organized as "singleCharTokens" and multiple character-long tokens are "multiCharTokens".
	for (int i = 0; i < numberOfTokens; i++) {
		debugLog("Processing token %d", i);
		if (tokenLengths[i] == 1) {
			singleCharTokens[singleCharTokensLength].MixedIdx = i;
			singleCharTokens[singleCharTokensLength++].TokenChar = *tokens[i];
		}
		else {
			multiCharTokens[multiCharTokensLength].MixedIdx = i;

			int length = tokenLengths[i];
			wchar_t** tokenContents = &multiCharTokens[multiCharTokensLength].TokenContents;
			*tokenContents = new wchar_t[length];

			for (int j = 0; j < length; j++) {
				(*tokenContents)[j] = 0;
				memcpy(&(*tokenContents)[j], &tokens[i][j], sizeof(unsigned short));
			}
			multiCharTokens[multiCharTokensLength++].TokenLength = length;
		}
	}

	// Output those.
	*outSingleCharTokens = singleCharTokens;
	*outMultiCharTokens = multiCharTokens;
	*numberOfSingleCharTokens = singleCharTokensLength;
	*numberOfMultiCharTokens = multiCharTokensLength;
}