#include "PlatformImplementation.h"
#include "ABParserBase.h"
#include "Debugging.h"
#include "SingleCharToken.h"
#include "MultiCharToken.h"
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
			if (futureTokens[i][j]->Token->TokenContents[nextTokenCharacterPosition] == ch) {
				if (futureTokens[i][j]->Token->TokenLength == nextTokenCharacterPosition + 1)
					MarkFinishedFutureToken(i, j);
			} else DisableFutureToken(i, j);

		}
	}


}

void ABParserBase::AddNewFutureTokens(wchar_t ch) {

	debugLog("Adding future tokens.");

	// Expand the futureTokens by 1.
	futureTokensTail++;

	// Add all of the futureTokens - remember, the futureTokens can only be multiple characters long.
	for (int i = 0; i < multiCharCurrentTokensLength; i++)
		if (multiCharCurrentTokens[i].TokenContents[0] == ch)
			AddFutureToken(&(multiCharCurrentTokens[i]));
}

int ABParserBase::ProcessFinishedTokens(wchar_t ch) {

	debugLog("Processing finished future tokens.");

	//int inishedTokenFirstIndex;

	// We deal with the multiple character long futureTokens because they might contain single character tokens, which wouldn't be any good for us.
	// We'll start from the head, since longer futureTokens are more important than shorter ones.
	for (int i = futureTokensHead; i < futureTokensTail; i++) {

		// Look to see if there are any finished futureTokens. 
		// We will only look at the first finished futureToken because literally the only way that two futureTokens could have been completed on the same character is if they were identical, meaning we'll just ignore the second one (because you shouldn't have duplicate tokens).
		for (int j = 0; j < futureTokenLengths[i]; j++) {

			// Ignore any "null" futureTokens.
			if (futureTokens[i][j] == NULL)
				continue;

			if (futureTokens[i][j]->Finished) {

				// If we need to verify it, do that, otherwise, finalize it.
				if (MultiCharNeedsVerification(futureTokens[i][j], i))
					StartVerify(futureTokens[i][j], i);
				else
					return FinalizeToken(futureTokens[i][j], i);
			}


		};


	}

	// Then, we'll deal with the single character tokens.
	for (int i = 0; i < singleCharCurrentTokensLength; i++)
		if (singleCharCurrentTokens[i].TokenChar == ch)

			// If we need to verify it, do that, otherwise, finalize it.
			if (SingleCharNeedsVerification(ch, &(singleCharCurrentTokens[i])))
				StartVerify(&singleCharCurrentTokens[i]);
			else
				return FinalizeToken(i);

	return 0;
}

// ======================
// VERIFY
// ======================

bool ABParserBase::SingleCharNeedsVerification(wchar_t ch, SingleCharToken* token) {
	bool needsToBeVerified = false;

	// Check to see if any futureTokens contain this character.
	for (int i = futureTokensHead; i < futureTokensTail; i++)
		for (int j = 0; j < futureTokenLengths[i]; j++) {

			MultiCharToken* multiCharToken = futureTokens[i][j]->Token;
			int length = multiCharToken->TokenLength;

			for (int k = 0; k < length; k++)
				if (multiCharToken->TokenContents[k] == ch) {

					needsToBeVerified = true;

					// Log all of the multiCharTokens that could be an issue for us.
					verifyTriggers.push_back(VerifyToken(multiCharToken));
				}

		}

	// If this token needs to be verified, then we'll add it to the verifyTokens.
	if (needsToBeVerified) {
		isVerifying = true;
		verifyTokens.push_back(VerifyToken(token));
	}

	return needsToBeVerified;
}

bool ABParserBase::MultiCharNeedsVerification(ABParserFutureToken* token, int index) {
	return false;
}

void ABParserBase::StartVerify(SingleCharToken* token) {
	return;
}

void ABParserBase::StartVerify(ABParserFutureToken* token, int index) {
	return;
}

// ======================
// FINALIZE
// ======================

int ABParserBase::FinalizeToken(int tokenIdx) {
	return 0;
}

int ABParserBase::FinalizeToken(ABParserFutureToken* token, int index) {

	return 3;
}

void ABParserBase::PrepareLeading() {
	
	// This will trim the leading down to what it is.

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

void ABParserBase::AddFutureToken(MultiCharToken* token) {
	debugLog("Adding futureToken: %d", token);
	futureTokens[currentPosition][futureTokenLengths[currentPosition]] = new ABParserFutureToken(token);
}

void ABParserBase::MarkFinishedFutureToken(int firstDimension, int secondDimension) {
	futureTokens[firstDimension][secondDimension]->Finished = true;
}

void ABParserBase::ConfigureCurrentTokens(int* validSingleCharTokens, int singleCharLength, int* validMultiCharTokens, int multiCharLength) {

	// If the "CurrentTokens" variable isn't pointing to "Tokens", then we can safely delete it, otherwise we'll leave it since we don't want to delete "Tokens".
	if (!singleCharCurrentTokensIsTokens)
		delete[] singleCharCurrentTokens;
	if (!multiCharCurrentTokensIsTokens)
		delete[] multiCharCurrentTokens;

	singleCharCurrentTokensIsTokens = false;
	multiCharCurrentTokensIsTokens = false;

	singleCharCurrentTokens = new SingleCharToken[singleCharLength];
	singleCharCurrentTokensLength = 0;

	multiCharCurrentTokens = new MultiCharToken[multiCharLength];
	multiCharCurrentTokensLength = 0;

	// Copy all of the single character tokens over, only including the ones that are in the "validSingleCharTokens" array.
	for (int i = 0; i < NumberOfSingleCharTokens; i++)
		for (int j = 0; j < singleCharLength; j++)
			if (i == validSingleCharTokens[j]) {
				singleCharCurrentTokens[singleCharCurrentTokensLength] = SingleCharTokens[i];
				singleCharCurrentTokens++;
				break;
			}

	// Do the same for multiCharTokens.
	for (int i = 0; i < NumberOfMultiCharTokens; i++)
		for (int j = 0; j < multiCharLength; j++)
			if (i == validMultiCharTokens[j]) {
				multiCharCurrentTokens[multiCharCurrentTokensLength] = MultiCharTokens[i];
				multiCharCurrentTokens++;
				break;
			}

};


void ABParserBase::ResetCurrentTokens() {


	// If the "SingleCharCurrentTokens" variable is already pointing to "SingleCharTokens", then we won't need to bother resetting it again.
	if (!singleCharCurrentTokensIsTokens) {

		delete[] singleCharCurrentTokens;
		singleCharCurrentTokensIsTokens = true;
		singleCharCurrentTokens = SingleCharTokens;
		singleCharCurrentTokensLength = NumberOfSingleCharTokens;

	}

	if (!multiCharCurrentTokensIsTokens) {

		delete[] multiCharCurrentTokens;
		multiCharCurrentTokensIsTokens = true;
		multiCharCurrentTokens = MultiCharTokens;
		multiCharCurrentTokensLength = NumberOfMultiCharTokens;

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
	primaryBuildUp = new wchar_t[TextLength + 1];
	secondaryBuildUp = new wchar_t[TextLength + 1];
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
	SingleCharToken* singleCharTokens = new SingleCharToken[numberOfTokens];
	int singleCharTokensLength = 0;

	MultiCharToken* multiCharTokens = new MultiCharToken[numberOfTokens];
	int multiCharTokensLength = 0;

	debugLog("Initializing Tokens");

	// Go through each token, if it is only one character long, then put into the "singleCharTokens".
	for (int i = 0; i < numberOfTokens; i++) {
		debugLog("Processing token %d", i);
		if (tokenLengths[i] == 1) {
			singleCharTokens[singleCharTokensLength].MixedIdx = i;
			singleCharTokens[singleCharTokensLength].TokenChar = *tokens[i];
		} else {
			multiCharTokens[i].MixedIdx = i;

			// Copy across the characters and the length.
			int length = tokenLengths[i];
			wchar_t** tokenContents = &multiCharTokens[multiCharTokensLength].TokenContents;
			*tokenContents = new wchar_t[length];
			wcsncpy(*tokenContents, tokens[i], length);
			multiCharTokens[multiCharTokensLength++].TokenLength = length;

			//debugLog("1: ", multiCharTokens[i].TokenContents);
		}
	}

	// Copy the single character tokens.
	debugLog("Copying single character tokens...");
	SingleCharTokens = singleCharTokens;
	NumberOfSingleCharTokens = singleCharTokensLength;

	debugLog("Copying multi-character tokens...");
	MultiCharTokens = multiCharTokens;
	NumberOfMultiCharTokens = multiCharTokensLength;
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
	primaryBuildUp = NULL;
	secondaryBuildUp = NULL;
	verifyTokens.reserve(4);
	//verifyTokens = new vector<int>();
}

ABParserBase::~ABParserBase() {
	delete[] Text;
	delete[] Leading;
	delete[] Trailing;
	delete[] primaryBuildUp;
	delete[] secondaryBuildUp;
	delete[] SingleCharTokens;
	delete[] MultiCharTokens;
	delete[] singleCharCurrentTokens;
	delete[] multiCharCurrentTokens;

	// Clear up all of the futureTokens we left lying around.
	for (int i = 0; i < futureTokensTail; i++)
		delete[] futureTokens[futureTokensHead];

	delete[] futureTokens;
	delete[] futureTokenLengths;
}