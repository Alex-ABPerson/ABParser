#include "PlatformImplementation.h"
#include "ABParserBase.h"
#include "Debugging.h"
#include "SingleCharToken.h"
#include "MultiCharToken.h"
#include <cstring>
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

	// Continue going through characters.
	for (; currentPosition < TextLength; currentPosition++) {
		debugLog("Current Position: %d", currentPosition);
		int res = ProcessChar(Text[currentPosition]);

		// If we got a result back from the "ProcessChar", return that, otherwise just keep going - make sure to increment the currentPosition, however, so that way when we get back, it will be incremented by one.
		if (res != 0) {
			currentPosition++;
			return res;
		}
			
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
	return ProcessFinishedTokens(ch);

}

// ======================
// COLLECT
// ======================

void ABParserBase::UpdateCurrentFutureTokens(wchar_t ch) {

	debugLog("Updating future tokens.");
	bool hasUnfinalizedFutureToken = false;

	// NOTE: You can only have futureTokens on multi-character tokens.

	// Go through the first dimension of the futureTokens.
	for (int i = futureTokensHead; i < futureTokensTail; i++)
	{
		// Then go through each of the futureTokens within that.
		for (int j = 0; j < NumberOfMultiCharTokens; j++)
		{
			// If this futureToken is NULL, then that's the end of the array, there's nothing more.
			if (futureTokens[i][j] == NULL)
				break;

			// If this token was finalized, then we'll ignore it.
			if (futureTokens[i][j]->Disabled)
				continue;

			hasUnfinalizedFutureToken = true;

			// This complex if statement checks to see that the next character in the futureToken does actually match this character.
			// If it does, we're going to check if this futureToken was completed, and, if so, go ahead and mark it as completed.
			// If it doesn't, we're going to kill this futureToken.
			int nextTokenCharacterPosition = (i - currentPosition) * -1;
			if ((unsigned short)futureTokens[i][j]->Token->TokenContents[nextTokenCharacterPosition] == (unsigned short)ch) {
				if (futureTokens[i][j]->Token->TokenLength == nextTokenCharacterPosition + 1)
					MarkFinishedFutureToken(i, j);
			} else DisableFutureToken(i, j);

		}

		// If there aren't any futureTokens left for this character, then we'll finally remove it from our range (if it is at our head, otherwise we'll do it when it is).
		if (i == futureTokensHead && !hasUnfinalizedFutureToken)
			TrimFutureTokens();
	}


}

void ABParserBase::AddNewFutureTokens(wchar_t ch) {

	debugLog("Adding future tokens.");

	// Expand the futureTokens by 1, and reset the "currentPositionFutureTokensLength", since this is a new character.
	futureTokensTail++;
	currentPositionFutureTokensLength = 0;

	// Make sure the very first item is set to NULL, so that way if we don't add any items, anything read the futureTokens will see the NULL and stop there.
	futureTokens[currentPosition][0] = NULL;

	// Add all of the futureTokens - remember, the futureTokens can only be multiple characters long.
	for (int i = 0; i < multiCharCurrentTokensLength; i++)
		if ((unsigned short)multiCharCurrentTokens[i].TokenContents[0] == (unsigned short)ch)
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
		for (int j = 0; j < NumberOfMultiCharTokens; j++) {

			// If this futureToken is NULL, then that's the end of the array, there's nothing more.
			if (futureTokens[i][j] == NULL)
				break;

			// Ignore any finalized futureTokens.
			if (futureTokens[i][j]->Disabled)
				continue;

			// If it is finished and hasn't been finalized, do that now.
			if (futureTokens[i][j]->Finished) {

				debugLog("Finished multi-char token!");

				// If we need to verify it, do that, otherwise, finalize it.
				if (MultiCharNeedsVerification(futureTokens[i][j], i))
					StartVerify(futureTokens[i][j], i);
				else
					return FinalizeToken(futureTokens[i][j], i);
			}


		};


	}

	// Then, we'll deal with the single character tokens.
	for (int i = 0; i < singleCharCurrentTokensLength; i++) {
		debugLog("Testing single-char: %c vs %c", singleCharCurrentTokens[i].TokenChar, ch);
		if ((unsigned short)singleCharCurrentTokens[i].TokenChar == (unsigned short)ch) {

			debugLog("Finished single-char token!");

			// If we need to verify it, do that, otherwise, finalize it.
			if (SingleCharNeedsVerification(ch, &(singleCharCurrentTokens[i])))
				StartVerify(&singleCharCurrentTokens[i]);
			else
				return FinalizeToken(&singleCharCurrentTokens[i]);
		}
	}

	return 0;
}

// ======================
// VERIFY
// ======================

bool ABParserBase::SingleCharNeedsVerification(wchar_t ch, SingleCharToken* token) {

	debugLog("Checking if single-char token requires verification.");

	bool needsToBeVerified = false;

	// Check to see if any futureTokens contain this character.
	for (int i = futureTokensHead; i < futureTokensTail; i++)
		for (int j = 0; j < NumberOfMultiCharTokens; j++) {

			// If this futureToken is NULL, then that's the end of the array, there's nothing more.
			if (futureTokens[i][j] == NULL)
				break;

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
		debugLog("Single-char token does require verification.");
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

int ABParserBase::FinalizeToken(SingleCharToken* token) {

	debugLog("Finalizing single-char token");

	// First, we need to sort out the leading and trailing.
	PrepareLeadingAndTrailing();

	// And, finally, we'll queue up the token and return the correct result.
	return QueueTokenAndReturnFinalizeResult(token, hasQueuedToken);
}

int ABParserBase::FinalizeToken(ABParserFutureToken* token, int index) {

	debugLog("Finalizing multi-char token");

	// First, we need to sort out the leading and trailing.
	PrepareLeadingAndTrailing();

	// Then, we'll null mark this token as having been finalized.
	token->Disabled = true;

	// Finally, we'll queue up the token and return the correct result.
	return QueueTokenAndReturnFinalizeResult(token->Token, hasQueuedToken);
}

int ABParserBase::QueueTokenAndReturnFinalizeResult(ABParserToken* token, bool hadQueuedToken) {

	// We'll queue the token.
	queuedToken = token;
	hasQueuedToken = true;

	// Based on whether there was a queued-up token before, we'll either trigger "BeforeTokenProcessed" or both that and "OnTokenProcessed".
	if (hadQueuedToken)
		return 3;
	else
		return 2;
}

void ABParserBase::PrepareLeadingAndTrailing() {

	debugLog("Preparing leading and trailing for token.");
	
	// First, we need to move the trailing to the leading.
	// NOTE: We delete data at the end because CPU usage is more important than just a tiny bit of RAM usage, so we just store the leading on our array of things buildUps to delete.
	//delete[] OnTokenProcessedLeading;
	thingsToDelete[buildUpsToDeleteLength++] = OnTokenProcessedLeading;
	OnTokenProcessedLeading = OnTokenProcessedTrailing;
	OnTokenProcessedLeadingLength = OnTokenProcessedTrailingLength;
	
	// Next, we need to create the new trailing, so, we'll take the buildUp, but shorten it just to the part that matters.
	OnTokenProcessedTrailing = new wchar_t[buildUpLength + 1];
	wcsncpy(OnTokenProcessedTrailing, buildUp, buildUpLength + 1);

	// Finally, reset the buildUp.
	buildUpLength = 0;

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
	futureTokens[firstDimension][secondDimension]->Disabled = true;
}

void ABParserBase::AddFutureToken(MultiCharToken* token) {
	futureTokens[currentPosition][currentPositionFutureTokensLength++] = new ABParserFutureToken(token);

	// Make sure that the next one is set to NULL, just in case this is going to be the last item in the array.
	futureTokens[currentPosition][currentPositionFutureTokensLength] = NULL;
}

void ABParserBase::MarkFinishedFutureToken(int firstDimension, int secondDimension) {
	futureTokens[firstDimension][secondDimension]->Finished = true;
}

void ABParserBase::ConfigureCurrentTokens(int* validSingleCharTokens, int singleCharLength, int* validMultiCharTokens, int multiCharLength) {

	debugLog("Configuring CurrentTokens");

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


void ABParserBase::ResetCurrentTokens(bool deleteCurrentTokens) {


	// If the "SingleCharCurrentTokens" variable is already pointing to "SingleCharTokens", then we won't need to bother resetting it again.
	if (!singleCharCurrentTokensIsTokens) {

		debugLog("Resetting single character current tokens... NumberOfSingleCharTokens: %d", NumberOfSingleCharTokens);
		if (deleteCurrentTokens)
			delete[] singleCharCurrentTokens;
		singleCharCurrentTokensIsTokens = true;
		singleCharCurrentTokens = SingleCharTokens;
		singleCharCurrentTokensLength = NumberOfSingleCharTokens;

	}

	if (!multiCharCurrentTokensIsTokens) {

		debugLog("Resetting multi-character current tokens... NumberOfMultiCharTokens: %d", NumberOfMultiCharTokens);
		if (deleteCurrentTokens)
			delete[] multiCharCurrentTokens;
		multiCharCurrentTokensIsTokens = true;
		multiCharCurrentTokens = MultiCharTokens;
		multiCharCurrentTokensLength = NumberOfMultiCharTokens;

	}
}

// ============
// INITIALIZATION/DISPOSE
// ============

// (We're using an "unsigned short*" because Linux and Windows' wchar_t size may differ.)
void ABParserBase::InitString(unsigned short* text, int textLength) {
	debugLog("Initializing String. Text Length: %d", textLength);
	Text = new wchar_t[textLength + 1];
	TextLength = textLength;
	OnTokenProcessedLeading = new wchar_t[textLength + 1];
	OnTokenProcessedTrailing = new wchar_t[textLength + 1];
	buildUp = new wchar_t[textLength + 1];
	thingsToDelete = new wchar_t*[textLength * 2];
	for (int i = 0; i < textLength; i++)
		memcpy(&Text[i], &text[i], sizeof(unsigned short));

	// Now that we know the text size, we can initialize the futureTokens.
	futureTokens = new ABParserFutureToken**[TextLength + 1];
	for (int i = 0; i < TextLength; i++)
		futureTokens[i] = new ABParserFutureToken*[NumberOfMultiCharTokens];
}

void ABParserBase::InitTokens(unsigned short** tokens, int* tokenLengths, int numberOfTokens) {

	debugLog("Initializing Tokens");

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
		} else {
			multiCharTokens[i].MixedIdx = i;

			// Copy across the characters and the length.
			int length = tokenLengths[i];
			wchar_t** tokenContents = &multiCharTokens[multiCharTokensLength].TokenContents;
			*tokenContents = new wchar_t[length];
			for (int j = 0; j < length; j++)
				memcpy(&(*tokenContents)[j], &tokens[i][j], sizeof(unsigned short));
			multiCharTokens[multiCharTokensLength++].TokenLength = length;

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

ABParserBase::ABParserBase(unsigned short** tokens, int* tokenLengths, int numberOfTokens)
{

	// Default values:
	Text = NULL;
	TextLength = 0;
	OnTokenProcessedLeading = NULL;
	OnTokenProcessedLeadingLength = 0;
	OnTokenProcessedTrailing = NULL;
	OnTokenProcessedTrailingLength = 0;
	currentPosition = 0;
	PublicPosition = 0;
	singleCharCurrentTokensIsTokens = false;
	multiCharCurrentTokensIsTokens = false;
	isVerifying = false;
	futureTokensHead = 0;
	futureTokensTail = 0;
	buildUp = NULL;
	thingsToDelete = NULL;
	buildUpsToDeleteLength = 0;
	verifyTokens.reserve(4);

	// Process the tokens into "singleChar" and "multiChar".
	InitTokens(tokens, tokenLengths, numberOfTokens);

	// Configure the current tokens.
	ResetCurrentTokens(false);
	//verifyTokens = new vector<int>();
}

ABParserBase::~ABParserBase() {
	debugLog("Disposing data.");

	delete[] Text;
	delete[] OnTokenProcessedLeading;
	delete[] OnTokenProcessedTrailing;
	delete[] buildUp;
	delete[] SingleCharTokens;
	delete[] MultiCharTokens;

	if (!singleCharCurrentTokensIsTokens)
		delete[] singleCharCurrentTokens;
	if (!multiCharCurrentTokensIsTokens)
		delete[] multiCharCurrentTokens;

	debugLog("-");

	// Clear up any futureTokens that are still left in the array.
	for (int i = 0; i < futureTokensTail; i++) {
		int j = 0;
		while (futureTokens[i][j] != NULL)
			delete futureTokens[i][j++];
	}

	// Clear up anything else we left lying around that we need to delete.
	for (int i = 0; i < buildUpsToDeleteLength; i++)
		delete[] thingsToDelete[i];

	delete[] futureTokens;
	delete[] thingsToDelete;
}