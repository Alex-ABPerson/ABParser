#include "PlatformImplementation.h"
#include "ABParserBase.h"
#include "Debugging.h"
#include <wchar.h>

using namespace std;

// RESULTS:
// 0 - None
// 1 - Stop
// 2 - BeforeTokenProcessed
// 3 - BeforeTokenProcessed/OnTokenProcessed
int ABParserBase::ContinueExecution() {

	debugLog("Continuing execution... Finished: %c ", (currentPosition < TextLength) ? 'F' : 'T');
	debugLog("Text Length: %d", TextLength);
	debugLog("Current Position: %d", currentPosition);

	// Continue going through characters.
	for (; currentPosition < TextLength; currentPosition++) {
		debugLog("Processing char %d.", currentPosition);
		int res = ProcessChar(Text[currentPosition]);

		// If we got a result back from the "ProcessChar", return that, otherwise just keep going.
		if (res != 0)
			return res;
	}

	// If we got here, then we reached the end of the string, so, we'll return a "1" to stop.
	return 1;
}

int ABParserBase::ProcessChar(wchar_t ch) {

	debugLog("Processing character: %c", ch);

	// First, we'll update our current futureTokens with this character.
	UpdateCurrentFutureTokens(ch);

	// Then, we'll add any new futureTokens for this character.
	AddNewFutureTokens(ch);

	// Finally, process any finished futureTokens.
	ProcessFinishedTokens(ch);

	return 0;

}

// ======================
// COLLECT
// ======================

void ABParserBase::UpdateCurrentFutureTokens(wchar_t ch) {

	debugLog("Updating future tokens.");
	// NOTE: You can only have futureTokens on multi-character tokens.

	// Go through the first dimension of the futureTokens.
	for (int i = futureTokensHead; i < futureTokensTail; i++)
	{
		int itemLength = futureTokenLengths[i];

		if (i == futureTokensHead && itemLength == 0)
			TrimFutureTokens();

		// Then go through each of the futureTokens within that, once we reach a "-1", that means this is the end of it.
		for (int j = 0; j < itemLength; j++)
		{
			// If it's a "-1", then that means that this futureToken has been either been killed or was finished - so we'll ignore it.
			if (futureTokens[i][j] == NULL)
				continue;

			// This complex if statement checks to see that the next character in the futureToken does actually match this character.
			// If it does, we're going to check if this futureToken was completed, and, if so, go ahead and mark it as completed.
			// If it doesn't, we're going to kill this futureToken.
			int nextTokenCharacterPosition = (i - currentPosition) * -1;
			if (MultiCharTokens[futureTokens[i][j]->Token][nextTokenCharacterPosition] == ch) {
				if (MultiCharTokenLengths[futureTokens[i][j]->Token] == nextTokenCharacterPosition + 1)
					MarkFinishedFutureToken(i, j);
			} else DisableFutureToken(i, j);

		}
	}


}

void ABParserBase::AddNewFutureTokens(wchar_t ch) {

	debugLog("Adding future tokens. FirstMultiChar:");

	// Expand the futureTokens by 1.
	futureTokensTail++;

	// Add all of the futureTokens - remember, the futureTokens can only be multiple characters long.
	for (int i = 0; i < MultiCharCurrentTokensLength; i++)
		if (*MultiCharCurrentTokens[i] == ch)
			AddFutureToken(i);
}

int ABParserBase::ProcessFinishedTokens(wchar_t ch) {

	debugLog("Processing finished future tokens.");

	//int inishedTokenFirstIndex;

	// First, we'll deal with the single character tokens, then we'll deal with the multiple character tokens (which are futureTokens).
	for (int i = 0; i < SingleCharCurrentTokensLength; i++)
		if (SingleCharCurrentTokens[i] == ch)
			FinalizeSingleCharToken(i);

	// We'll start from the head, since longer futureTokens are more important than shorter ones.
	for (int i = futureTokensHead; i < futureTokensTail; i++) {

		// Look to see if there are any finished futureTokens. 
		// We will only look at the first finished futureToken because literally the only way that two futureTokens could have been completed on the same character is if they were identical, meaning we'll just ignore the second one (because you shouldn't have duplicate tokens).
		for (int j = 0; j < futureTokenLengths[i]; j++) {

			// Ignore any "null" futureTokens.
			if (futureTokens[i][j] == NULL)
				continue;

			if (futureTokens[i][j]->Finished) {

				// Here what's going to happen:
				// If this token was somewhere after the "head", we MAY need to "verify" it.
				// If it DOES need verification, then we will start the verification process, instead of finalizing it straight away.
				if (CheckIfTokenNeedsVerification(futureTokens[i][j], i))
					StartVerify(futureTokens[i][j], i);
				else
					return StartFinalize(futureTokens[i][j], i);
			}


		};


	}
}

// ======================
// VERIFY
// ======================

bool ABParserBase::CheckIfTokenNeedsVerification(ABParserFutureToken* token, int index) {
	return false;
}

void ABParserBase::StartVerify(ABParserFutureToken* token, int index) {

}

// ======================
// FINALIZE
// ======================

int ABParserBase::StartFinalize(ABParserFutureToken* token, int index) {
	return 3;
}

void ABParserBase::FinalizeSingleCharToken(int tokenIdx) {

}

// ============
// HELPERS
// ============

void ABParserBase::TrimFutureTokens() {
	// Experimental: We now clean up all items at the end, in order to save performance as it runs.
	// CPU usage is more important than RAM usage since this is only a string parser, which won't use up too much RAM.
	//delete[] futureTokens[futureTokensHead];
	futureTokensHead++;
}

void ABParserBase::DisableFutureToken(int firstDimension, int secondDimension) {
	futureTokens[firstDimension][secondDimension] = NULL;

	// That's one less item in there.
	futureTokenLengths[firstDimension]--;
}

void ABParserBase::AddFutureToken(int token) {
	debugLog("Adding futureToken: %d", token);
	futureTokens[currentPosition][futureTokenLengths[currentPosition]] = new ABParserFutureToken(token);
}

void ABParserBase::MarkFinishedFutureToken(int firstDimension, int secondDimension) {
	futureTokens[firstDimension][secondDimension]->Finished = true;
}

void ABParserBase::ConfigureCurrentTokens(int* validSingleCharTokens, int singleCharLength, int* validMultiCharTokens, int multiCharLength) {

	// If the "CurrentTokens" variable isn't pointing to "Tokens", then we can safely delete it, otherwise we'll leave it since we don't want to delete "Tokens".
	if (!SingleCharCurrentTokensIsTokens)
		delete[] SingleCharCurrentTokens;
	if (!MultiCharCurrentTokensIsTokens)
		delete[] MultiCharCurrentTokens;

	SingleCharCurrentTokensIsTokens = false;
	MultiCharCurrentTokensIsTokens = false;

	SingleCharCurrentTokens = new wchar_t[singleCharLength];
	SingleCharCurrentTokensLength = 0;

	MultiCharCurrentTokens = new wchar_t*[multiCharLength];
	MultiCharCurrentTokensLength = 0;

	// Copy all of the single character tokens over, only including the ones that are in the "validSingleCharTokens" array.
	for (int i = 0; i < NumberOfSingleCharTokens; i++)
		for (int j = 0; j < singleCharLength; j++)
			if (i == validSingleCharTokens[j]) {
				SingleCharCurrentTokens[SingleCharCurrentTokensLength] = SingleCharTokens[i];
				SingleCharCurrentTokens++;
				break;
			}

	// Do the same for multiCharTokens.
	for (int i = 0; i < NumberOfMultiCharTokens; i++)
		for (int j = 0; j < multiCharLength; j++)
			if (i == validMultiCharTokens[j]) {
				MultiCharCurrentTokens[MultiCharCurrentTokensLength] = MultiCharTokens[i];
				MultiCharCurrentTokens++;
				break;
			}

};


void ABParserBase::ResetCurrentTokens() {


	// If the "SingleCharCurrentTokens" variable is already pointing to "SingleCharTokens", then we won't need to bother resetting it again.
	if (!SingleCharCurrentTokensIsTokens) {

		delete[] SingleCharCurrentTokens;
		SingleCharCurrentTokensIsTokens = true;
		SingleCharCurrentTokens = SingleCharTokens;
		SingleCharCurrentTokensLength = NumberOfSingleCharTokens;

	}

	if (!MultiCharCurrentTokensIsTokens) {

		delete[] MultiCharCurrentTokens;
		MultiCharCurrentTokensIsTokens = true;
		MultiCharCurrentTokens = MultiCharTokens;
		MultiCharCurrentTokensLength = NumberOfMultiCharTokens;

	}
}

// ============
// INITIALIZATION/DISPOSE
// ============

void ABParserBase::InitString(wchar_t* text, int textLength) {
	debugLog("Initializing String. Text Length: %d ", textLength);
	Text = new wchar_t[textLength + 1];
	TextLength = textLength;
	Leading = new wchar_t[textLength + 1];
	Trailing = new wchar_t[textLength + 1];
	PrimaryBuildUp = new wchar_t[TextLength + 1];
	SecondaryBuildUp = new wchar_t[TextLength + 1];
	wcsncpy(Text, text, textLength);

	// Now that we know the text size, we can initialize the futureTokens.
	futureTokens = new ABParserFutureToken**[TextLength];
	futureTokenLengths = new int[TextLength];
	for (int i = 0; i < TextLength; i++)
		futureTokens[i] = new ABParserFutureToken*[NumberOfMultiCharTokens];
}

void ABParserBase::InitTokens(wchar_t** tokens, int* tokenLengths, int numberOfTokens) {
	
	// We write to a temporary array first, because we don't know how many single character and multi character tokens there will be.
	// So, we will write to a temporary array that is the maximum number it could be, then copy that to a correctly-sized array.

	// Initialize the arrays the results will go into.
	wchar_t* singleCharTokens = new wchar_t[numberOfTokens];
	int singleCharTokensLength = 0;

	wchar_t** multiCharTokens = new wchar_t*[numberOfTokens];
	int* multiCharTokenLengths = new int[numberOfTokens];
	int multiCharTokensLength = 0;

	debugLog("Initializing Tokens");

	// Go through each token, if it is only one character long, then put into the "singleCharTokens".
	for (int i = 0; i < numberOfTokens; i++) {
		debugLog("Processing token %d", i);
		if (tokenLengths[i] == 1)
			singleCharTokens[singleCharTokensLength++] = *tokens[i];
		else {
			// We have a more intense process when copying all of the 
			int length = tokenLengths[i];
			multiCharTokens[multiCharTokensLength] = new wchar_t[length];
			wcsncpy(multiCharTokens[multiCharTokensLength], tokens[multiCharTokensLength], length);
			multiCharTokenLengths[multiCharTokensLength++] = length;

		}
	}

	// Copy the single character tokens.
	debugLog("Copying single character tokens...");
	SingleCharTokens = new wchar_t[singleCharTokensLength];
	NumberOfSingleCharTokens = singleCharTokensLength;
	wcsncpy(SingleCharTokens, singleCharTokens, singleCharTokensLength);

	debugLog("Copying multi-character tokens...");
	NumberOfMultiCharTokens = multiCharTokensLength;

	// Copy across the lengths.
	MultiCharTokenLengths = new int[NumberOfMultiCharTokens];
	memcpy(MultiCharTokenLengths, multiCharTokenLengths, NumberOfMultiCharTokens * sizeof(wchar_t));

	// Then, copy the actual multiChar tokens.
	MultiCharTokens = new wchar_t*[NumberOfMultiCharTokens];
	for (int i = 0; i < NumberOfMultiCharTokens; i++) {
		MultiCharTokens[i] = new wchar_t[multiCharTokenLengths[i]];
		// wcsncpy gives warnings about non-null terminated strings.
		memcpy(MultiCharTokens[i], multiCharTokens[i], multiCharTokenLengths[i] * sizeof(wchar_t));
	}

	//// Print them out.
	//debugLog("SingleCharTokens: %ws", SingleCharTokens);

	// Finally, delete our temporary arrays.
	delete[] singleCharTokens;
	delete[] multiCharTokens;
	delete[] multiCharTokenLengths;

}

ABParserBase::ABParserBase(wchar_t** tokens, int* tokenLengths, int numberOfTokens)
{
	// Process the tokens into "singleChar" and "multiChar".
	InitTokens(tokens, tokenLengths, numberOfTokens);

	// Configure the current tokens.
	ResetCurrentTokens();

	// Default values:
	Text = NULL;
	TextLength = 0;
	Leading = NULL;
	LeadingLength = 0;
	Trailing = NULL;
	TrailingLength = 0;
	currentPosition = 0;
	PublicPosition = 0;
	isVerifying = false;
	futureTokensHead = 0;
	futureTokensTail = 0;
	PrimaryBuildUp = NULL;
	SecondaryBuildUp = NULL;
	verifyTokens.reserve(4);
	//verifyTokens = new vector<int>();
}

ABParserBase::~ABParserBase() {
	delete[] Text;
	delete[] Leading;
	delete[] Trailing;
	delete[] PrimaryBuildUp;
	delete[] SecondaryBuildUp;
	delete[] SingleCharTokens;
	delete[] MultiCharTokens;
	delete[] SingleCharCurrentTokens;
	delete[] MultiCharCurrentTokens;

	// Clear up all of the futureTokens we left lying around.
	for (int i = 0; i < futureTokensTail; i++)
		delete[] futureTokens[futureTokensHead];

	delete[] futureTokens;
	delete[] futureTokenLengths;
}