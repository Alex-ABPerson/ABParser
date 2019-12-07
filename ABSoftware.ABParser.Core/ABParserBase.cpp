#include "PlatformImplementation.h"
#include "ABParserBase.h"
#include "Debugging.h"
#include "SingleCharToken.h"
#include "MultiCharToken.h"
#include "ABParserVerifyToken.h"
#include <cstring>
#include <wchar.h>

using namespace std;

// RESULTS:
// 0 - None
// 1 - Do Nothing
// 2 - Stop
// 3 - BeforeTokenProcessed
// 4 - BeforeTokenProcessed+OnTokenProcessed
int ABParserBase::ContinueExecution() {

	// If we're finalizing all of the "verifyTokens", finalize the next one.
	if (isFinalizingVerifyToken)
		if (int result = FinalizeNextVerifyToken())
			return result;

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

	// If there's a token left, we'll prepare the leading and trailing for it so that when we trigger the "stop" result, it can be the final OnTokenProcessed.
	if (BeforeTokenProcessedToken != NULL)
		PrepareLeadingAndTrailing(BeforeTokenProcessedToken->GetLength(), false, true);

	// If we got here, then we reached the end of the string, so, we'll return a "1" to stop.
	return 1;
}

int ABParserBase::ProcessChar(wchar_t ch) {

	debugLog("Processing character: %c", ch);

	// First, we'll update our current futureTokens with this character.
	UpdateCurrentFutureTokens(ch);

	// Next, we'll add any new futureTokens for this character.
	AddNewFutureTokens(ch);

	// Then, process any finished futureTokens, and, if we need to return from that, do that.
	if (int result = ProcessFinishedTokens(ch))
		return result;

	// Now, if we just started finalizing all of the "verifyTokens", then we'll handle the first one, but the handler is at the top of "ContinueExecution", so jump up there!
	// But, make sure to run "AddCharacterToBuildUp", because once we have finalized them all, we'll be returning right here, and this character would never get added to the buildUp, which is an issue.
	if (isFinalizingVerifyToken) {
		int result = ContinueExecution();
		buildUp[buildUpLength++] = ch;
		return result;
	}

	// Finally, if we haven't finalized any tokens, just return.
	AddCharacterToBuildUp(ch);

	return 0;

}

// ======================
// COLLECT
// ======================

void ABParserBase::AddCharacterToBuildUp(wchar_t ch) {

	// If we're in "verify" mode, then we'll add to the "verifyBuildUp".
	if (isVerifying)
		verifyBuildUp.back()[verifyBuildUpLengths.back()++] = ch;
	else
		buildUp[buildUpLength++] = ch;
}

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
					MarkFinishedFutureToken(futureTokens[i][j]);
			} else DisableFutureToken(futureTokens[i][j], i);

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

				// If we are currently verifying, then we need to do some extra checks on it.
				if (isVerifying)
					if (int result = CheckFinishedFutureToken(futureTokens[i][j], i))
						return result;

				// If we need to verify it, do that, otherwise, finalize it.
				if (PrepareMultiCharForVerification(futureTokens[i][j], i))
					StartVerify(new ABParserVerifyToken(futureTokens[i][j], i));
				else
					return FinalizeToken(futureTokens[i][j], i, false);
			}


		};


	}

	// Then, we'll deal with the single character tokens.
	for (int i = 0; i < singleCharCurrentTokensLength; i++) {
		debugLog("Testing single-char: %c vs %c", singleCharCurrentTokens[i].TokenChar, ch);
		if ((unsigned short)singleCharCurrentTokens[i].TokenChar == (unsigned short)ch) {

			debugLog("Finished single-char token!");

			// If we need to verify it, do that, otherwise, finalize it.
			if (PrepareSingleCharForVerification(ch, &(singleCharCurrentTokens[i])))
				StartVerify(new ABParserVerifyToken(&singleCharCurrentTokens[i], currentPosition));
			else
				return FinalizeToken(&singleCharCurrentTokens[i], currentPosition);
		}
	}

	return 0;
}

// ======================
// VERIFY
// ======================

bool ABParserBase::PrepareSingleCharForVerification(wchar_t ch, SingleCharToken* token) {

	debugLog("Checking if single-char token requires verification.");

	bool needsToBeVerified = false;

	// Check to see if any futureTokens contain this character.
	for (int i = futureTokensHead; i < futureTokensTail; i++)
		for (int j = 0; j < NumberOfMultiCharTokens; j++) {

			// If this futureToken is NULL, then that's the end of the array, there's nothing more.
			if (futureTokens[i][j] == NULL)
				break;

			// If this futureToken has already been finalized, or has been disabled, then don't check it.
			if (futureTokens[i][j]->Finished || futureTokens[i][j]->Disabled)
				continue;

			ABParserFutureToken* multiCharToken = futureTokens[i][j];
			if (multiCharToken->Token->TokenContents[currentPosition - i] == ch) {

				needsToBeVerified = true;

				// Log all of the multiCharTokens that could be an issue for us.
				verifyTriggers.push_back(multiCharToken);
			}

		}

	// If this token needs to be verified, then we'll add it to the verifyTokens.
	if (needsToBeVerified) {
		debugLog("Single-char token does require verification.");
		isVerifying = true;
	}

	return needsToBeVerified;
}

bool ABParserBase::PrepareMultiCharForVerification(ABParserFutureToken* token, int index) {

	debugLog("Checking if multi-char token requires verification.");
	bool needsToBeVerified = false;

	// We'll check to see if there are any more futureTokens.
	for (int i = futureTokensHead; i < futureTokensTail; i++) {

		// Calculate the current position we'll be in for these futureTokens.
		int currentPos = currentPosition - i;

		for (int j = 0; j < NumberOfMultiCharTokens; j++) {

			// If this futureToken is NULL, then that's the end of the array, there's nothing more.
			if (futureTokens[i][j] == NULL)
				break;

			// If this futureToken has already been finalized, or has been disabled, then don't check it.
			if (futureTokens[i][j]->Finished || futureTokens[i][j]->Disabled)
				continue;

			ABParserFutureToken* futureToken = futureTokens[i][j];
			MultiCharToken* multiCharToken = futureToken->Token;
			int length = multiCharToken->TokenLength;

			// If the token isn't even long enough to contain our token, then we can ignore it.
			if (token->Token->TokenLength > length)
				continue;

			// If it is, however, then we'll need to check to see whether it actually does contain this token.
			bool contains = true;
			for (int k = 0; k < token->Token->TokenLength; k++) {

				if (token->Token->TokenContents[k] != multiCharToken->TokenContents[k])
					contains = false;
			}

			// If this token does contain the token we're going to verify, then we'll add it as a trigger, and set "needsToBeVerified" to true.
			if (contains) {
				verifyTriggers.push_back(futureToken);
				needsToBeVerified = true;
			}

		}
	}

	return needsToBeVerified;
}

void ABParserBase::StartVerify(ABParserVerifyToken* token) {
	debugLog("Starting verify.");

	// Mark us as verifying.
	isVerifying = true;

	// Add this token to the verify tokens.
	verifyTokens.push_back(token);

	// Add a new item to the buildsUps and tokens.
	verifyBuildUp.push_back(new wchar_t[TextLength - currentPosition]);
	verifyBuildUpLengths.push_back(0);

	// Also, set the start position of this.
	verifyStartPosition = currentPosition;
}

void ABParserBase::StopVerify() {
	isVerifying = false;
	
	// TODO: DISPOSAL
	verifyTokens.clear();
	verifyTriggers.clear();
}

int ABParserBase::CheckFinishedFutureToken(ABParserFutureToken* token, int index) {

	debugLog("Checking finished future token...");

	// Check if this token was one of the tokens that triggered verify.
	for (size_t i = 0; i < verifyTriggers.size(); i++)
		if (verifyTriggers.at(i) == token) {

			// Since this trigger was finished, it must have been this trigger all along, so stop verifying and finalize this trigger!
			int result = FinalizeToken(verifyTriggers.at(i), index, true);
			StopVerify();
			return result;

		}

	return 0;
}

void ABParserBase::CheckDisabledFutureToken(ABParserFutureToken* token, int index) {

	debugLog("Checking disabled future token...");

	// Check if this token was one of the tokens that triggered verify.
	for (size_t i = 0; i < verifyTriggers.size(); i++)
		if (verifyTriggers.at(i) == token) {

			// Since this trigger has just been disabled, we can now remove it.
			verifyTriggers.erase(verifyTriggers.begin() + i);

			// If there aren't any more triggers left - then the original futureTokens win! And we will finalize all of them.
			if (verifyTriggers.size() == 0)
				isFinalizingVerifyToken = true;

		}

}

int ABParserBase::FinalizeNextVerifyToken() {

	debugLog("Finalizing verify original token...");

	// Firstly, if we've hit the end, then we'll stop here.
	if (verifyTokens.size() == 0) {
		StopVerify();
		isFinalizingVerifyToken = false;
		return 0;
	}

	// Then, finalize the next token, and remove it.
	int result = FinalizeToken(verifyTokens.front());
	lastTokenWasOriginalVerified = true;

	// Now, we now need to move the next item in the "verifyBuildUp" into the main buildUp - because, now we know that the data in there is actually valuable.
	buildUp = verifyBuildUp.at(0);
	buildUpLength = verifyBuildUpLengths.at(0);

	// And, finally, remove this item.
	verifyTokens.erase(verifyTokens.begin());
	verifyBuildUp.erase(verifyBuildUp.begin());
	verifyBuildUpLengths.erase(verifyBuildUpLengths.begin());

	return result;

}

// ======================
// FINALIZE
// ======================

int ABParserBase::FinalizeToken(ABParserVerifyToken* verifyToken) {
	if (verifyToken->IsSingleChar)
		return FinalizeToken(verifyToken->SingleChar, verifyToken->Start);
	else
		return FinalizeToken(verifyToken->MultiChar, verifyToken->Start, false);
}

int ABParserBase::FinalizeToken(SingleCharToken* token, int index) {

	debugLog("Finalizing single-char token");

	// First, we need to sort out the leading and trailing.
	PrepareLeadingAndTrailing(1, false, false);

	// And, finally, we'll queue up the token and return the correct result.
	return QueueTokenAndReturnFinalizeResult(token, index, hasQueuedToken);
}

int ABParserBase::FinalizeToken(ABParserFutureToken* token, int index, bool isVerifyTriggerToken) {

	debugLog("Finalizing multi-char token");

	// First, we need to sort out the leading and trailing.
	PrepareLeadingAndTrailing(token->Token->TokenLength, isVerifyTriggerToken, false);

	// Then, we'll mark this token as having been finalized.
	token->Disabled = true;

	// Finally, we'll queue up the token and return the correct result.
	return QueueTokenAndReturnFinalizeResult(token->Token, index, hasQueuedToken);
}

int ABParserBase::QueueTokenAndReturnFinalizeResult(ABParserToken* token, int index, bool hadQueuedToken) {

	// We'll handle the tokens.
	OnTokenProcessedPreviousToken = OnTokenProcessedToken;
	OnTokenProcessedPreviousTokenStart = OnTokenProcessedTokenStart;

	OnTokenProcessedTokenStart = BeforeTokenProcessedTokenStart;
	OnTokenProcessedToken = BeforeTokenProcessedToken;

	BeforeTokenProcessedTokenStart = index;
	BeforeTokenProcessedToken = token;
	hasQueuedToken = true;

	// Based on whether there was a queued-up token before, we'll either trigger "BeforeTokenProcessed" or both that and "OnTokenProcessed".
	if (hadQueuedToken)
		return 3;
	else
		return 2;
}

void ABParserBase::PrepareLeadingAndTrailing(int tokenLength, bool isVerifyTriggerToken, bool endOfText) {

	debugLog("Preparing leading and trailing for token.");
	
	// First, we need to move the trailing to the leading.
	// NOTE: We delete data at the end because CPU usage is more important than just a tiny bit of RAM usage, so we just store the leading on our array of things buildUps to delete.
	//delete[] OnTokenProcessedLeading;
	// TODO: IMPLEMENT PROPER DISPOSALE SYSTEM.
	//if (OnTokenProcessedLeading != NULL)
	//	thingsToDelete[buildUpsToDeleteLength++] = OnTokenProcessedLeading;
	OnTokenProcessedLeading = OnTokenProcessedTrailing;
	OnTokenProcessedLeadingLength = OnTokenProcessedTrailingLength;

	int trailingLength = 0;

	// We're substracting the tokenLength from the total length, because the buildUp will contain this token (excluding the last character) and we want to trim that off.
	// Now, if this is a trigger token that we're finalizing, then it would have gone into the "verifyBuildUp".
	if (isVerifyTriggerToken)
		trailingLength = buildUpLength - (verifyStartPosition - verifyTokens.front()->Start);
	else if (endOfText)
		trailingLength = buildUpLength;
	else
		trailingLength = (buildUpLength - tokenLength) + 1; // + 1 because the last character of the token didn't go into the buildUp - since that's the character we're on now!

	// Next, we need to create the new trailing, so, we'll take the buildUp, but shorten it just to the part that matters.
	// You'll notice it says "lastTokenWasOriginalVerified" multiple times, because for a finalized verifed original token, the last character of it does show up in the main buildUp, so we will skip the last character, simply by pushing the offset back one and ending 1 earlier.
	wchar_t* trailingBuildUp = buildUp;
	OnTokenProcessedTrailing = new wchar_t[trailingLength + 1];
	OnTokenProcessedTrailingLength = 0;

	for (int i = 0; i < trailingLength; i++)
		OnTokenProcessedTrailing[i] = trailingBuildUp[i];
	OnTokenProcessedTrailingLength = trailingLength;
	OnTokenProcessedTrailing[OnTokenProcessedTrailingLength] = 0;

	if (lastTokenWasOriginalVerified)
		lastTokenWasOriginalVerified = false;

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

void ABParserBase::DisableFutureToken(ABParserFutureToken* futureToken, int index) {

	futureToken->Disabled = true;

	// If we're verifying, then make sure to check this disabled futureToken.
	if (isVerifying)
		CheckDisabledFutureToken(futureToken, index);
}

void ABParserBase::AddFutureToken(MultiCharToken* token) {
	futureTokens[currentPosition][currentPositionFutureTokensLength++] = new ABParserFutureToken(token);

	// Make sure that the next one is set to NULL, just in case this is going to be the last item in the array.
	futureTokens[currentPosition][currentPositionFutureTokensLength] = NULL;
}

void ABParserBase::MarkFinishedFutureToken(ABParserFutureToken* futureToken) {
	futureToken->Finished = true;
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

void ABParserBase::InitTokens(SingleCharToken* singleCharTokens, int singleCharTokensLength, MultiCharToken* multiCharTokens, int multiCharTokensLength) {

	debugLog("Initializing Tokens");

	// Copy the single character tokens.
	debugLog("Copying single character tokens...");
	SingleCharTokens = singleCharTokens;
	NumberOfSingleCharTokens = singleCharTokensLength;

	debugLog("Copying multi-character tokens...");
	MultiCharTokens = multiCharTokens;
	NumberOfMultiCharTokens = multiCharTokensLength;
}

ABParserBase::ABParserBase(SingleCharToken* singleCharTokens, int singleCharTokensLength, MultiCharToken* multiCharTokens, int multiCharTokensLength)
{
	// Default values:
	Text = NULL;
	TextLength = 0;
	OnTokenProcessedLeading = NULL;
	OnTokenProcessedLeadingLength = 0;
	OnTokenProcessedTrailing = NULL;
	OnTokenProcessedTrailingLength = 0;
	currentPosition = 0;
	singleCharCurrentTokensIsTokens = false;
	multiCharCurrentTokensIsTokens = false;
	isVerifying = false;
	lastTokenWasOriginalVerified = false;
	hasQueuedToken = false;
	futureTokensHead = 0;
	futureTokensTail = 0;
	buildUp = NULL;
	buildUpLength = 0;
	thingsToDelete = NULL;
	buildUpsToDeleteLength = 0;
	verifyTokens.reserve(20);
	verifyTriggers.reserve(20);
	verifyBuildUp.reserve(20);
	verifyBuildUpLengths.reserve(20);

	// Process the tokens into "singleChar" and "multiChar".
	InitTokens(singleCharTokens, singleCharTokensLength, multiCharTokens, multiCharTokensLength);

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