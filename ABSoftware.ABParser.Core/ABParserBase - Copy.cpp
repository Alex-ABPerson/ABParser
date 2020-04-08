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
	if (isFinalizingVerifyTokens)
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
		PrepareLeadingAndTrailing(BeforeTokenProcessedToken->GetLength(), (currentPosition - 1) - BeforeTokenProcessedToken->GetLength(), buildUp, buildUpLength, false);

	// If we got here, then we reached the end of the string, so, we'll return a "1" to stop.
	return 1;
}

int ABParserBase::ProcessChar(wchar_t ch) {

	debugLog("Processing character: %c", ch);

	// First, we'll update our current futureTokens with this character.
	if (UpdateCurrentFutureTokens(ch) == -1);

	// Next, we'll add any new futureTokens for this character.
	AddNewFutureTokens(ch);

	// Then, process any finished futureTokens, and, if we need to return from that, do that.
	// This will return -1 if we shouldn't write to the buildUp.
	if (int result = ProcessFinishedTokens(ch))
		return result == -1 ? 0 : result;

	// Now, if we've started finalizing all the "verifyTokens", do that. In order to do that, we need to go up to the top of "ContinueExeuction".
	if (isFinalizingVerifyTokens) {
		int result = ContinueExecution();
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

	if (isVerifying)
		currentVerifyToken->TrailingBuildUp[currentVerifyToken->TrailingBuildUpLength++] = ch;
	else
		buildUp[buildUpLength++] = ch;
}

int ABParserBase::UpdateCurrentFutureTokens(wchar_t ch) {

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
			int nextTokenCharacterPosition = (i - currentPosition) * -1;
			if ((unsigned short)futureTokens[i][j]->Token->TokenContents[nextTokenCharacterPosition] == (unsigned short)ch) {
				if (futureTokens[i][j]->Token->TokenLength == nextTokenCharacterPosition + 1)
					MarkFinishedFutureToken(futureTokens[i][j]);
			// If it doesn't, we're going to disable this futureToken.
			} else
				return DisableFutureToken(futureTokens[i][j], i);

		}

		// If there aren't any futureTokens left for this character, then we'll finally remove it from our range (if it is at our head, otherwise we'll do it when it is).
		if (i == futureTokensHead && !hasUnfinalizedFutureToken)
			TrimFutureTokens();
	}

	return 0;
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
		if ((unsigned short)multiCharCurrentTokens[i]->TokenContents[0] == (unsigned short)ch)
			AddFutureToken(multiCharCurrentTokens[i]);
}

int ABParserBase::ProcessFinishedTokens(wchar_t ch) {

	debugLog("Processing finished future tokens.");

	//int inishedTokenFirstIndex;

	// We deal with the multiple character long futureTokens first because they might contain single character tokens, so if we process them first, then the "PrepareSingleCharForVerification" can look at these futureTokens.
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
					StartVerify(LoadCurrentTriggersInto(new ABParserVerifyToken(futureTokens[i][j], i, GenerateVerifyTrailing())));
				else
					return FinalizeToken(futureTokens[i][j], i, buildUp, buildUpLength, true);
			}


		};


	}

	// Then, we'll deal with the single character tokens.
	for (int i = 0; i < singleCharCurrentTokensLength; i++) {
		debugLog("Testing single-char: %c vs %c", singleCharCurrentTokens[i]->TokenChar, ch);
		if ((unsigned short)singleCharCurrentTokens[i]->TokenChar == (unsigned short)ch) {

			debugLog("Finished single-char token!");

			// If we need to verify it, do that, otherwise, finalize it.
			if (PrepareSingleCharForVerification(ch, singleCharCurrentTokens[i]))
				StartVerify(LoadCurrentTriggersInto(new ABParserVerifyToken(singleCharCurrentTokens[i], currentPosition, GenerateVerifyTrailing())));
			else
				return FinalizeToken(singleCharCurrentTokens[i], currentPosition, buildUp, buildUpLength, true);
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
				currentVerifyTriggers.push_back(multiCharToken);
				currentVerifyTriggerStarts.push_back(i);
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

		// Calculate how far away from the current position we are in for these futureTokens.
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
				currentVerifyTriggers.push_back(futureToken);
				currentVerifyTriggerStarts.push_back(i);
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
	AddVerifyToken(token);

	// Also, clear out the "currentVerifyTriggers" so that we can start any other verify lines with no problem.
	currentVerifyTriggers.clear();
	currentVerifyTriggerStarts.clear();
}

void ABParserBase::StopVerify(int tokenIndex, bool wasFinalized) {
	
	// If we stopping verify because we ended up finalizing - then stop all of the other verify lines as well!
	if (wasFinalized) {
		if (verifyTokens.size()) {
			verifyTokensToDelete.insert(verifyTokensToDelete.begin(), verifyTokens.begin(), verifyTokens.end());
			verifyTokens.clear();
		}

		isVerifying = false;
	}

	// Otherwise, just stop this one.
	else RemoveVerifyToken(tokenIndex);
}

// Returns -1 if the caller should go straight onto the next character, 0 if the caller should just continue like normal, and any other value should be returned from "ContinueExecution".
int ABParserBase::CheckFinishedFutureToken(ABParserFutureToken* token, int index) {
	debugLog("Checking finished future token...");

	// Check if this token was one of the tokens that triggered verify.
	for (size_t i = 0; i < verifyTokens.size(); i++)
		for (int j = 0; j < verifyTokens[i]->TriggersLength; j++) {

			ABParserFutureToken* trigger = verifyTokens[i]->Triggers[j];

			if (trigger == token) {

				// Since this trigger was finished, it must have been this trigger all along, so stop verifying and finalize this trigger!
				// However, before we finalize this trigger - we may need to verify it against one of the other triggers!
				if (verifyTokens[i]->TriggersLength > 1) {

					int thisLength = trigger->Token->TokenLength;
					bool areAnyLonger = false;

					for (int k = 0; k < verifyTokens[i]->TriggersLength; k++) {

						// Ignore the trigger we're about to start verifying.
						if (j == k) 
							continue;

						ABParserFutureToken* currentTrigger = verifyTokens[i]->Triggers[k];

						if (currentTrigger->Token->TokenLength > thisLength) {
							currentVerifyTriggers.push_back(currentTrigger);
							currentVerifyTriggerStarts.push_back(verifyTokens[i]->TriggerStarts[k]);
							areAnyLonger = true;
						}
					}

					if (areAnyLonger) {

						// Now, we need to verify THIS trigger, so, to do that we need to stop verifying the existing token, and start verifying this trigger.
						StopVerify(i, false);
						StartVerify(LoadCurrentTriggersInto(new ABParserVerifyToken(trigger, currentVerifyTriggerStarts[i], GenerateVerifyTrailing())));

						return -1;
					}
				}

				StopVerify(i, true);
				return FinalizeToken(trigger, index, buildUp, buildUpLength, true);
			}
		}

	return 0;
}

// Returns - 1 if the caller should go straight onto the next character, and 0 if the caller should just continue like normal.
int ABParserBase::CheckDisabledFutureToken(ABParserFutureToken* token, int index) {

	debugLog("Checking disabled future token...");

	// Check if this token was one of the tokens that triggered verify.
	for (size_t i = 0; i < verifyTokens.size(); i++) {
		bool hasRemainingTriggers = false;

		for (int j = 0; j < verifyTokens[i]->TriggersLength; j++) {

			ABParserFutureToken** trigger = &verifyTokens[i]->Triggers[j];

			// Ignore any null triggers - we need to ignore them because if we don't "hasRemainingTriggers" will be set to true, when actually, this isn't a verify trigger at all!
			if (*trigger == nullptr)
				continue;

			// Since this trigger has just ended, we can now remove it.
			if (*trigger == token) *trigger = nullptr;
			else hasRemainingTriggers = true;
		}

		// If there are no more triggers left in this token, then it's not the triggers. So, we'll go ahead and get ready to start finalizing this token.
		if (!hasRemainingTriggers) {
			verifyTokens[i]->HasNoTriggers = true;
			finalizingVerifyTokensCurrentToken = 0;
			lastVerifyToken = nullptr;
			isFinalizingVerifyTokens = true;
			return -1;
		}
	}

	return 0;

}

int ABParserBase::FinalizeNextVerifyToken() {

	debugLog("Finalizing verify original token...");

	// First, we need to actually determine the next item to finalize.
	ABParserVerifyToken* nextItem = nullptr;
	for (; finalizingVerifyTokensCurrentToken < verifyTokens.size(); finalizingVerifyTokensCurrentToken++)
		if (verifyTokens[finalizingVerifyTokensCurrentToken]->HasNoTriggers) {
			nextItem = verifyTokens[finalizingVerifyTokensCurrentToken];
			break;
		}

	// If we've hit the end, then we'll stop here, and set the buildUp to the trailing of the last token so that it can be used by the next token.
	if (nextItem == nullptr) {

		isFinalizingVerifyTokens = false;
		buildUp = lastVerifyToken->TrailingBuildUp;
		buildUpLength = lastVerifyToken->TrailingBuildUpLength;

		// The character we're currently on never got added to the buildUp, so, we'll add it to the buildUp now.
		StopVerify(0, true);
		AddCharacterToBuildUp(Text[currentPosition - 1]);

		return 0;
	}

	// Then, finalize the next token, and remove it.
	bool isFirst = lastVerifyToken == nullptr;
	int result = FinalizeToken(verifyTokens.front(), isFirst ? buildUp : lastVerifyToken->TrailingBuildUp, isFirst ? buildUpLength : lastVerifyToken->TrailingBuildUpLength, false);
	lastVerifyToken = verifyTokens.front();
	StopVerify(0, true);

	return result;

}

ABParserVerifyToken* ABParserBase::LoadCurrentTriggersInto(ABParserVerifyToken* token) {

	// First, set the lengths.
	token->TriggersLength = currentVerifyTriggers.size();
	token->TriggerStartsLength = currentVerifyTriggerStarts.size();

	// Next, initialize the two.
	ABParserFutureToken** triggers = token->Triggers = new ABParserFutureToken*[token->TriggersLength];
	int* triggerStarts = token->TriggerStarts = new int[token->TriggerStartsLength];

	// Then, copy across the values.
	for (int i = 0; i < token->TriggersLength; i++)
		triggers[i] = currentVerifyTriggers[i];
	for (int i = 0; i < token->TriggerStartsLength; i++)
		triggerStarts[i] = currentVerifyTriggerStarts[i];

	// Finally, return our new modified verify token!
	return token;

}

wchar_t* ABParserBase::GenerateVerifyTrailing() {
	return &buildUp[currentPosition];
}

// ======================
// FINALIZE
// ======================

int ABParserBase::FinalizeToken(ABParserVerifyToken* verifyToken, wchar_t* buildUpToUse, int buildUpToUseLength, bool resetBuildUp) {
	if (verifyToken->IsSingleChar)
		return FinalizeToken(verifyToken->SingleChar, verifyToken->Start, buildUp, buildUpToUseLength, resetBuildUp);
	else
		return FinalizeToken(verifyToken->MultiChar, verifyToken->Start, buildUp, buildUpToUseLength, resetBuildUp);
}

int ABParserBase::FinalizeToken(SingleCharToken* token, int index, wchar_t* buildUpToUse, int buildUpToUseLength, bool resetBuildUp) {

	debugLog("Finalizing single-char token");

	// First, we need to sort out the leading and trailing.
	PrepareLeadingAndTrailing(1, index, buildUpToUse, buildUpToUseLength, resetBuildUp);

	// And, finally, we'll queue up the token and return the correct result.
	return QueueTokenAndReturnFinalizeResult(token, index, hasQueuedToken);
}

int ABParserBase::FinalizeToken(ABParserFutureToken* token, int index, wchar_t* buildUpToUse, int buildUpToUseLength, bool resetBuildUp) {

	debugLog("Finalizing multi-char token");

	// First, we need to sort out the leading and trailing.
	PrepareLeadingAndTrailing(token->Token->TokenLength, index, buildUpToUse, buildUpToUseLength, resetBuildUp);

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

void ABParserBase::PrepareLeadingAndTrailing(int tokenLength, int tokenStart, wchar_t* buildUpToUse, int buildUpToUseLength, bool resetBuildUp) {

	debugLog("Preparing leading and trailing for token.");
	
	// First, we need to move the trailing to the leading.
	// NOTE: We delete data at the end because CPU usage is more important than just a tiny bit of RAM usage, so we just store the leading on our array of buildUps to delete.
	// We are only deleting the leading, and not the buildUp because the buildUp is only ever allocated once, whereas the leading, which is set to the previous trailing, is allocated everytime, as you can see below.
	if (OnTokenProcessedLeading)
		buildUpsToDelete.push_back(OnTokenProcessedLeading);
	OnTokenProcessedLeading = OnTokenProcessedTrailing;
	OnTokenProcessedLeadingLength = OnTokenProcessedTrailingLength;

	// Now, we need to work out how much of the buildUp is what we really want, because the buildUp will contain this token or part of it at the end, and need to trim that off.
	int trailingLength = tokenStart - (BeforeTokenProcessedToken == nullptr ? 0 : BeforeTokenProcessedTokenStart + BeforeTokenProcessedToken->GetLength());

	// Next, we need to create the new trailing, so, we'll take the buildUp, but shorten it just to the part that matters.
	OnTokenProcessedTrailing = new wchar_t[trailingLength + 1];
	OnTokenProcessedTrailingLength = 0;

	for (int i = 0; i < trailingLength; i++)
		OnTokenProcessedTrailing[OnTokenProcessedTrailingLength++] = buildUpToUse[i];
	OnTokenProcessedTrailing[trailingLength] = 0;

	buildUpsToDelete.push_back(buildUp);

	// Finally, reset the buildUp.
	if (resetBuildUp)
		buildUpLength = 0;

}

// ============
// HELPERS
// ============

void ABParserBase::TrimFutureTokens() {
	// We now clean up all items at the end, in order to save performance as it runs.
	// CPU usage is more important than RAM usage since this is only a string parser, which won't use up too much RAM.
	//delete[] futureTokens[futureTokensHead];
	futureTokensHead++;
}

int ABParserBase::DisableFutureToken(ABParserFutureToken* futureToken, int index) {

	futureToken->Disabled = true;

	// If we're verifying, then make sure to check this disabled futureToken.
	if (isVerifying)
		return CheckDisabledFutureToken(futureToken, index);
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
	singleCharCurrentTokensIsTokens = false;
	multiCharCurrentTokensIsTokens = false;

	// Copy all of the single character tokens over, only including the ones that are in the "validSingleCharTokens" array.
	for (int i = 0; i < NumberOfSingleCharTokens; i++)
		for (int j = 0; j < singleCharLength; j++)
			if (i == validSingleCharTokens[j]) {
				singleCharCurrentTokens[singleCharCurrentTokensLength++] = &SingleCharTokens[i];
				break;
			}

	// Do the same for multiCharTokens.
	for (int i = 0; i < NumberOfMultiCharTokens; i++)
		for (int j = 0; j < multiCharLength; j++)
			if (i == validMultiCharTokens[j]) {
				multiCharCurrentTokens[multiCharCurrentTokensLength++] = &MultiCharTokens[i];
				break;
			}

};


void ABParserBase::ResetCurrentTokens(bool deleteCurrentTokens) {


	// If the "SingleCharCurrentTokens" variable is already pointing to "SingleCharTokens", then we won't need to bother resetting it again.
	if (!singleCharCurrentTokensIsTokens) {

		debugLog("Resetting single character current tokens... NumberOfSingleCharTokens: %d", NumberOfSingleCharTokens);
		singleCharCurrentTokensIsTokens = true;
		for (int i = 0; i < NumberOfSingleCharTokens; i++)
			singleCharCurrentTokens[i] = &SingleCharTokens[i];
		singleCharCurrentTokensLength = NumberOfSingleCharTokens;
	}

	if (!multiCharCurrentTokensIsTokens) {

		debugLog("Resetting multi-character current tokens... NumberOfMultiCharTokens: %d", NumberOfMultiCharTokens);
		multiCharCurrentTokensIsTokens = true;
		for (int i = 0; i < NumberOfMultiCharTokens; i++)
			multiCharCurrentTokens[i] = &MultiCharTokens[i];
		multiCharCurrentTokensLength = NumberOfMultiCharTokens;

	}
}

void ABParserBase::AddVerifyToken(ABParserVerifyToken* token) {
	verifyTokens.push_back(token);
	currentVerifyToken = token;
}

void ABParserBase::RemoveVerifyToken(int index) {
	auto tokenToRemoveIterator = verifyTokens.begin() + index;
	verifyTokensToDelete.insert(verifyTokensToDelete.begin(), tokenToRemoveIterator, tokenToRemoveIterator);
	verifyTokens.erase(tokenToRemoveIterator);

	// If that was the last of them, stop verifying.
	if (!verifyTokens.size())
		isVerifying = false;
}

// ============
// INITIALIZATION/DISPOSE
// ============

// (We're using an "unsigned short*" because different platform's wchar_t size may differ.)
void ABParserBase::InitString(unsigned short* text, int textLength) {
	debugLog("Initializing String. Text Length: %d", textLength);
	Text = new wchar_t[textLength + 1];
	TextLength = textLength;
	OnTokenProcessedLeading = new wchar_t[textLength + 1];
	OnTokenProcessedTrailing = new wchar_t[textLength + 1];
	buildUp = new wchar_t[textLength + 1];
	for (int i = 0; i < textLength; i++)
		memcpy(&Text[i], &text[i], sizeof(unsigned short));

	// Now that we know the text size, we can initialize the futureTokens.
	futureTokens = new ABParserFutureToken**[TextLength];
	for (int i = 0; i < TextLength; i++)
		futureTokens[i] = new ABParserFutureToken*[NumberOfMultiCharTokens + 1];
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

	singleCharCurrentTokens = new SingleCharToken*[singleCharTokensLength];
	singleCharCurrentTokensLength = 0;

	multiCharCurrentTokens = new MultiCharToken*[multiCharTokensLength];
	multiCharCurrentTokensLength = 0;
}

ABParserBase::ABParserBase(SingleCharToken* singleCharTokens, int singleCharTokensLength, MultiCharToken* multiCharTokens, int multiCharTokensLength)
{
	const size_t ESTIMATED_VECTOR_SIZE = 20;
	// Default values:
	Text = NULL;
	TextLength = 0;
	singleCharCurrentTokensIsTokens = false;
	multiCharCurrentTokensIsTokens = false;
	isVerifying = false;
	hasQueuedToken = false;
	buildUp = NULL;

	currentVerifyTriggers.reserve(multiCharTokensLength);
	currentVerifyTriggerStarts.reserve(multiCharTokensLength);

	verifyTokens.reserve(ESTIMATED_VECTOR_SIZE);
	buildUpsToDelete.reserve(ESTIMATED_VECTOR_SIZE);
	verifyTokensToDelete.reserve(ESTIMATED_VECTOR_SIZE);

	// Process the tokens into "singleChar" and "multiChar".
	InitTokens(singleCharTokens, singleCharTokensLength, multiCharTokens, multiCharTokensLength);
	//verifyTokens = new vector<int>();
}

ABParserBase::~ABParserBase() {
	debugLog("Disposing data for complete parser deletion.");

	delete[] buildUp;
	delete[] Text;
	// Don't dispose the Single/MultiCharTokens - that would be disposing the "ABParserTokensArray", which is created once at application start, and can be reused, and so should remain for the lifetime of the application.
	// However, we do dispose the CURRENT Single/MultiCharTokens. But, by using "free", because using "delete" would destruct the pointers in the "ABParserTokensArray", which, as we just said, shouldn't be destructed ever.
	free(singleCharCurrentTokens);
	free(multiCharCurrentTokens);

}

void ABParserBase::PrepareForParse(unsigned short* text, int textLength) {
	debugLog("Preparing for a parse.");

	OnTokenProcessedLeading = NULL;
	OnTokenProcessedLeadingLength = 0;
	OnTokenProcessedTrailing = NULL;
	OnTokenProcessedTrailingLength = 0;
	currentPosition = 0;
	futureTokensHead = 0;
	futureTokensTail = 0;
	buildUpLength = 0;

	// Initialize the string.
	InitString(text, textLength);

	// Configure the current tokens.
	ResetCurrentTokens(false);
}

void ABParserBase::DisposeDataForNextParse() {
	debugLog("Disposing data ready for the next parse.");

	delete[] OnTokenProcessedLeading;
	delete[] OnTokenProcessedTrailing;

	// Clear up all of the futureTokens (they are re-generated everytime we get some new text.)
	for (int i = 0; i < futureTokensTail; i++) {
		// Delete each of the items first.
		int j = 0;
		while (futureTokens[i][j] != NULL)
			delete futureTokens[i][j++];
		delete[] futureTokens[i];
	}
	delete[] futureTokens;

	// Clear up the different things we've marked to delete - so, first we destruct the items themselves.
	for (size_t i = 0; i < buildUpsToDelete.size(); i++)
		delete[] buildUpsToDelete[i];
	for (size_t i = 0; i < verifyTokensToDelete.size(); i++)
		delete verifyTokensToDelete[i];

	// Then we clear out the vectors, which will get rid of all of the pointers.
	buildUpsToDelete.clear();
	verifyTokensToDelete.clear();
}