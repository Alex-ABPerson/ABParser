#ifndef _ABPARSER_INCLUDE_ABPARSER_MAIN_H
#define _ABPARSER_INCLUDE_ABPARSER_MAIN_H
#include "HelperClasses.h"
#include "TokenManagement.h"
#include "Debugging.h"
#include <vector>

using namespace std;

template<typename T>
class ABParserBase {
public:
	TokensInformation<T>* Tokens;

	int BeforeTokenProcessedTokenStart;
	ABParserToken* BeforeTokenProcessedToken;

	int OnTokenProcessedTokenStart;
	ABParserToken* OnTokenProcessedToken;

	int OnTokenProcessedPreviousTokenStart;
	ABParserToken* OnTokenProcessedPreviousToken;

	T* Text;
	int TextLength;

	T* OnTokenProcessedLeading;
	int OnTokenProcessedLeadingLength;

	T* OnTokenProcessedTrailing;
	int OnTokenProcessedTrailingLength;

	// RESULTS:
	// 0 - None
	// 1 - Stop+Final OnTokenProcessed
	// 2 - BeforeTokenProcessed
	// 3 - BeforeTokenProcessed+OnTokenProcessed
	ABParserResult ContinueExecution() {

		if (currentPosition == 0)
			PrepareForParse();

		if (isFinalizingVerifyTokens)
			if (ABParserResult result = FinalizeNextVerifyToken())
				return result;

		debugLog("Continuing execution... Finished: %c ", (currentPosition < TextLength) ? 'F' : 'T');
		debugLog("Text Length: %d", TextLength);

		// The main loop - go through every character.
		for (; currentPosition < TextLength; currentPosition++) {
			debugLog("Current Position: %d", currentPosition);

			ABParserResult res = ProcessChar(Text[currentPosition]);

			// Return any result we got.
			if (res != ABParserResult::None) {
				currentPosition++;
				return res;
			}
		}

		// If there's a token left, we'll prepare the leading and trailing for it so that when we trigger the "stop" result, it can be the final OnTokenProcessed.
		if (BeforeTokenProcessedToken)
			PrepareLeadingAndTrailing(BeforeTokenProcessedToken->GetLength(), (currentPosition - 1) - BeforeTokenProcessedToken->GetLength(), buildUp, buildUpLength, false, true);

		currentPosition = 0;
		return ABParserResult::StopAndFinalOnTokenProcessed;

	}

	ABParserBase(TokensInformation<T>* tokensInformation) {

		const size_t ESTIMATED_VECTOR_SIZE = 20;

		Text = nullptr;
		TextLength = 0;
		singleCharCurrentTokensIsTokens = false;
		multiCharCurrentTokensIsTokens = false;
		isVerifying = false;
		hasQueuedToken = false;
		buildUpStart = nullptr;
		futureTokens = nullptr;
		OnTokenProcessedLeading = nullptr;
		OnTokenProcessedTrailing = nullptr;
		currentPosition = 0;
		Tokens = tokensInformation;

		currentVerifyTriggers.reserve(Tokens->NumberOfMultiCharTokens);
		currentVerifyTriggerStarts.reserve(Tokens->NumberOfMultiCharTokens);

		verifyTokens.reserve(ESTIMATED_VECTOR_SIZE);
		verifyTokensToDelete.reserve(ESTIMATED_VECTOR_SIZE);

		InitTokens();
	}

	~ABParserBase() {
		debugLog("Disposing data for complete parser deletion.");

		// We never delete the tokens, because those are created once (usually at application start) and kept. However, we don't want the "currentTokens" arrays.
		// So, we'll free them up, NOT delete them because deleting them would delete the tokens too.
		free(singleCharCurrentTokens);
		free(multiCharCurrentTokens);

		DisposeForTextChange(true);
	}

	// Prepares for the next parse.
	void PrepareForParse() {
		debugLog("Preparing for a parse.");

		BeforeTokenProcessedToken = nullptr;
		BeforeTokenProcessedTokenStart = 0;
		OnTokenProcessedToken = nullptr;
		OnTokenProcessedTokenStart = 0;
		OnTokenProcessedLeadingLength = 0;
		OnTokenProcessedTrailingLength = 0;
		currentPosition = 0;
		futureTokensHead = 0;
		futureTokensTail = 0;
		isVerifying = false;
		isFinalizingVerifyTokens = false;
		hasQueuedToken = false;
		buildUpLength = 0;
		buildUp = buildUpStart;

		// Configure the current tokens.
		ResetCurrentTokens();
	}

	void InitString(T* text, int textLength) {
		debugLog("Initializing String. Text Length: %d", textLength);

		// Re-allocate anything that wouldn't work on the new text
		bool recreateTextSpecific = TextLength < textLength;
		DisposeForTextChange(recreateTextSpecific);

		Text = new T[textLength];
		TextLength = textLength;

		for (int i = 0; i < textLength; i++)
			Text[i] = text[i];

		if (recreateTextSpecific) {
			buildUpStart = buildUp = new T[textLength];
			OnTokenProcessedLeading = new T[textLength + 1];
			OnTokenProcessedTrailing = new T[textLength + 1];

			futureTokens = new ABParserFutureToken<T>**[TextLength];
			for (int i = 0; i < TextLength; i++) {
				futureTokens[i] = new ABParserFutureToken<T>*[Tokens->NumberOfMultiCharTokens + 1];
				futureTokens[i][Tokens->NumberOfMultiCharTokens] = nullptr;
			}
		}
	}

	// Disposes data after a parse has been completed.
	void DisposeDataForNextParse() {
		debugLog("Disposing data ready for the next parse.");

		// Erase any futureTokens we created during this parse, but don't erase the futureToken array, that only needs to be replaced when we change the text (affecting the size of the array).
		for (int i = 0; i < TextLength; i++)
			for (int j = 0; j < Tokens->NumberOfMultiCharTokens; j++) {
				if (futureTokens[i][j] == nullptr) break;
				delete futureTokens[i][j];
			}

		for (size_t i = 0; i < verifyTokensToDelete.size(); i++)
			delete verifyTokensToDelete[i];

		verifyTokensToDelete.clear();
	}

	void DisposeForTextChange(bool disposeBuildUpAndFutureTokens) {
		if (disposeBuildUpAndFutureTokens) {

			if (futureTokens) {
				// Don't delete the individual items as those were deleted when we disposed after a parse.
				for (int i = 0; i < TextLength; i++)
					delete[] futureTokens[i];
				delete[] futureTokens;
				futureTokens = nullptr;
			}

			if (buildUpStart) {
				delete[] buildUpStart;
				buildUpStart = nullptr;
			}

			if (OnTokenProcessedLeading) {
				delete[] OnTokenProcessedLeading;
				OnTokenProcessedLeading = nullptr;
			}
			if (OnTokenProcessedTrailing) {
				delete[] OnTokenProcessedTrailing;
				OnTokenProcessedTrailing = nullptr;
			}
		}

		if (Text) {
			delete[] Text;
			Text = nullptr;
		}
	}

private:

	// NOTE: When the parser loads text from a "wchar_t" it uses "codecvt" to convert the wide characters into 16-bit (UTF-16) integers.
	// When this conversion occurs, it places the result into a string, and if that string is deleted, the text goes with it.
	// Therefore, IF (and only if) we copied the text from "codecvt", we need to keep the string alive by holding onto it in the parser.
	// And, if we replace the text, we need to delete "_text" as opposed to the "T" array.
	bool _textFromCodecvt;
	std::string _text;

	// NOTE: futureTokens can only be multiple characters.
	ABParserFutureToken<T>*** futureTokens;

	// When we add "futureTokens" we need to keep track of how many futureTokens we've already added so far in order to know at what second dimension to place the futureToken.
	int currentPositionFutureTokensLength;
	//int* futureTokenLengths;
	int futureTokensHead;
	int futureTokensTail;
	int currentPosition;

	bool isVerifying;

	// When all of the triggers in a verify token gets removed, then we finalize that token! However, sometimes there may be lots of verify tokens that all had the same triggers, so, we'll finalize them all in one go with this!
	bool isFinalizingVerifyTokens;
	ABParserVerifyToken<T>* lastVerifyToken;
	size_t finalizingVerifyTokensCurrentToken;

	vector<ABParserVerifyToken<T>*> verifyTokens;

	// The verify token right at the end of the verifyTokens.
	ABParserVerifyToken<T>* currentVerifyToken;
	
	// As we're preparing a token for verification, we'll use this temporaily.
	vector<ABParserFutureToken<T>*> currentVerifyTriggers;
	vector<int> currentVerifyTriggerStarts;

	bool hasQueuedToken;

	// When verify tokens are finalized, the buildUp is pushed forwards to make it point towards the last verify tokens' trailing, since that is the correct leading for the next token. This is the buildUp without any of that "movement".
	T* buildUpStart;

	T* buildUp;
	int buildUpLength;

	int buildUpOffset;

	// Whether the "CurrentTokens" array actually just points to the "Tokens" array.
	// We can use this to decide whether to "delete" CurrentTokens when configuring/resetting it.
	bool singleCharCurrentTokensIsTokens;
	bool multiCharCurrentTokensIsTokens;

	SingleCharToken<T>** singleCharCurrentTokens;
	int singleCharCurrentTokensLength;

	MultiCharToken<T>** multiCharCurrentTokens;
	int multiCharCurrentTokensLength;
	
	// DISPOSAL
	vector<ABParserVerifyToken<T>*> verifyTokensToDelete;

	// COLLECT
	ABParserResult ProcessChar(T ch) {

		debugLog("Processing character: %c", ch);

		// First, we'll update our current futureTokens with this character.
		UpdateCurrentFutureTokens(ch);

		// Next, we'll add any new futureTokens for this character.
		AddNewFutureTokens(ch);

		// Then, process any finished futureTokens, and, if we need to return a result from that, do that.
		if (ABParserResult result = ProcessFinishedTokens(ch))
			return result;

		// Finalizing all the "verifyTokens" (if necessary) is done from the top of "ContinueExeuction".
		if (isFinalizingVerifyTokens)
			return ContinueExecution();

		AddCharacterToBuildUp(ch);
		return ABParserResult::None;
	}

	void AddCharacterToBuildUp(T ch) {
		if (isVerifying)
			currentVerifyToken->TrailingBuildUp[currentVerifyToken->TrailingBuildUpLength++] = ch;
		else
			buildUp[buildUpLength++] = ch;
	}

	void UpdateCurrentFutureTokens(T ch) {

		debugLog("Updating future tokens.");
		bool hasUnfinalizedFutureToken = false;

		for (int i = futureTokensHead; i < futureTokensTail; i++)
		{
			for (int j = 0; j < Tokens->NumberOfMultiCharTokens; j++)
			{
				if (futureTokens[i][j] == nullptr) break;
				if (futureTokens[i][j]->Disabled) continue;

				hasUnfinalizedFutureToken = true;

				// This if statement checks to see that the next character in the futureToken does actually match this character. If it does, we're going 
				// to check if we've matched the whole futureToken, and if so mark it as complete, otherwise if a character didn't match, we'll disable it.
				int nextTokenCharacterPosition = (i - currentPosition) * -1;

				if (futureTokens[i][j]->Token->TokenContents[nextTokenCharacterPosition] == ch) {
					if (futureTokens[i][j]->Token->TokenLength == nextTokenCharacterPosition + 1)
						MarkFinishedFutureToken(futureTokens[i][j]);
				}
				else
					DisableFutureToken(futureTokens[i][j], i);
			}

			// Trim off any parts of the futureTokens that no longer contain anything.
			if (i == futureTokensHead && !hasUnfinalizedFutureToken)
				TrimFutureTokens();
		}
	}

	void AddNewFutureTokens(T ch) {

		debugLog("Adding future tokens.");

		futureTokensTail++;
		currentPositionFutureTokensLength = 0;

		// Make sure the very first item is set to nullptr, so that way if we don't add any items, anything that reads the futureTokens will see the nullptr and stop there.
		futureTokens[currentPosition][0] = nullptr;

		for (int i = 0; i < multiCharCurrentTokensLength; i++)
			if (multiCharCurrentTokens[i]->TokenContents[0] == ch)
				AddFutureToken(multiCharCurrentTokens[i]);
	}

	ABParserResult ProcessFinishedTokens(T ch) {
		debugLog("Processing finished future tokens.");

		// We deal with the multiple character long tokens first because they might contain single character tokens, so, if we process them first,
		// then the "PrepareSingleCharForVerification" can look at these futureTokens. Also, longer futureTokens are more important than shorter ones.
		for (int i = futureTokensHead; i < futureTokensTail; i++) {

			// We'll ignore if there are two tokens both finished, as the only way that can occur is if two tokens are identical.
			for (int j = 0; j < Tokens->NumberOfMultiCharTokens; j++) {

				if (futureTokens[i][j] == nullptr) break;
				if (futureTokens[i][j]->Disabled) continue;

				if (futureTokens[i][j]->Finished) {

					debugLog("Finished multi-char token!");

					// If we are currently verifying, then we need to do some extra checks on it.
					if (isVerifying)
						if (int result = CheckFinishedFutureToken(futureTokens[i][j], i))
							if (result == -1) return ABParserResult::None;
							else return static_cast<ABParserResult>(result);

					// Finalize it or verify it.
					if (PrepareMultiCharForVerification(futureTokens[i][j], i))
						StartVerify(LoadCurrentTriggersInto(new ABParserVerifyToken<T>(futureTokens[i][j], i, GenerateVerifyTrailing())));
					else
						return FinalizeToken(futureTokens[i][j], i, buildUp, buildUpLength, true);
				}
			};
		}

		for (int i = 0; i < singleCharCurrentTokensLength; i++) {
			debugLog("Testing single-char: %c vs %c", singleCharCurrentTokens[i]->TokenChar, ch);
			if (singleCharCurrentTokens[i]->TokenChar == ch) {

				debugLog("Finished single-char token!");

				// Finalize it or verify it.
				if (PrepareSingleCharForVerification(ch, singleCharCurrentTokens[i]))
					StartVerify(LoadCurrentTriggersInto(new ABParserVerifyToken<T>(singleCharCurrentTokens[i], currentPosition, GenerateVerifyTrailing())));
				else
					return FinalizeToken(singleCharCurrentTokens[i], currentPosition, buildUp, buildUpLength, true);
			}
		}

		return ABParserResult::None;
	}

	// VERIFY
	void StartVerify(ABParserVerifyToken<T>* token) {
		debugLog("Starting verify.");

		isVerifying = true;
		AddVerifyToken(token);

		currentVerifyTriggers.clear();
		currentVerifyTriggerStarts.clear();
	}

	void StopVerify(int tokenIndex, bool wasFinalized) {

		// If this token was finalized, then we'll stop ALL other verifications going on.
		if (wasFinalized) {
			if (verifyTokens.size()) {
				for (size_t i = 0; i < verifyTokens.size(); i++)
					verifyTokensToDelete.push_back(verifyTokens[i]);
				verifyTokens.clear();
			}

			isVerifying = false;
		}

		else RemoveVerifyToken(tokenIndex);
	}

	bool PrepareSingleCharForVerification(T ch, SingleCharToken<T>* token) {
		debugLog("Checking if single-char token requires verification.");

		bool needsToBeVerified = false;

		// Check to see if any futureTokens contain this character.
		for (int i = futureTokensHead; i < futureTokensTail; i++)
			for (int j = 0; j < Tokens->NumberOfMultiCharTokens; j++) {

				// The futureToken array ends on a null pointer.
				if (futureTokens[i][j] == nullptr) break;
				if (futureTokens[i][j]->Finished || futureTokens[i][j]->Disabled) continue;

				ABParserFutureToken<T>* multiCharToken = futureTokens[i][j];
				if (multiCharToken->Token->TokenContents[currentPosition - i] == ch) {

					needsToBeVerified = true;

					// Keep track of all of the multiCharTokens that could be an issue for us, as those will become triggers if we really need to verify.
					currentVerifyTriggers.push_back(multiCharToken);
					currentVerifyTriggerStarts.push_back(i);
				}

			}

		if (needsToBeVerified) {
			debugLog("Single-char token does require verification.");
			isVerifying = true;
		}
		else {
			currentVerifyTriggers.clear();
			currentVerifyTriggerStarts.clear();
		}

		return needsToBeVerified;
	}

	bool PrepareMultiCharForVerification(ABParserFutureToken<T>* token, int index) {
		debugLog("Checking if multi-char token requires verification.");
		bool needsToBeVerified = false;

		// Check to see if any other futureTokens contain this token.
		for (int i = futureTokensHead; i <= index; i++) {

			for (int j = 0; j < Tokens->NumberOfMultiCharTokens; j++) {

				// The futureToken array ends on a null pointer.
				if (futureTokens[i][j] == nullptr) break;
				if (futureTokens[i][j]->Finished || futureTokens[i][j]->Disabled) continue;

				ABParserFutureToken<T>* futureToken = futureTokens[i][j];
				MultiCharToken<T>* multiCharToken = futureToken->Token;

				int distanceAway = index - i;

				// If the token isn't even long enough to contain our token, then we can ignore it.
				if (token->Token->TokenLength > multiCharToken->TokenLength)
					continue;

				bool contains = true;
				for (int k = 0; k < token->Token->TokenLength; k++)
					if (token->Token->TokenContents[k] != multiCharToken->TokenContents[k + distanceAway])
						contains = false;

				if (contains) {
					currentVerifyTriggers.push_back(futureToken);
					currentVerifyTriggerStarts.push_back(i);
					needsToBeVerified = true;
				}

			}
		}

		if (!needsToBeVerified) {
			currentVerifyTriggers.clear();
			currentVerifyTriggerStarts.clear();
		}

		return needsToBeVerified;
	}

	void CheckDisabledFutureToken(ABParserFutureToken<T>* token, int index) {
		debugLog("Checking disabled future token...");

		for (size_t i = 0; i < verifyTokens.size(); i++) {
			bool hasRemainingTriggers = false;

			for (int j = 0; j < verifyTokens[i]->TriggersLength; j++) {

				ABParserFutureToken<T>** trigger = &verifyTokens[i]->Triggers[j];

				if (*trigger == nullptr)
					continue;

				// Since this trigger has just ended, we can now remove it.
				if (*trigger == token) *trigger = nullptr;
				else hasRemainingTriggers = true;
			}

			// If there are no more triggers left in this token, then it WAS actually the token in the text - not the triggers! So, we'll go ahead and get ready to start finalizing this token.
			if (!hasRemainingTriggers) {
				verifyTokens[i]->HasNoTriggers = true;
				finalizingVerifyTokensCurrentToken = 0;
				lastVerifyToken = nullptr;
				isFinalizingVerifyTokens = true;
			}
		}

	}

	int CheckFinishedFutureToken(ABParserFutureToken<T>* token, int index) {
		debugLog("Checking finished future token...");

		for (size_t i = 0; i < verifyTokens.size(); i++)
			for (int j = 0; j < verifyTokens[i]->TriggersLength; j++) {

				ABParserFutureToken<T>* trigger = verifyTokens[i]->Triggers[j];

				if (trigger == token) {

					// Since this trigger was finished, it must have been this trigger all along, so stop verifying and finalize this trigger!
					// However, before we finalize this trigger - we need to check if we need verify it against one of the other triggers!
					if (verifyTokens[i]->TriggersLength > 1) {

						int thisLength = trigger->Token->TokenLength;
						bool areAnyLonger = false;

						for (int k = 0; k < verifyTokens[i]->TriggersLength; k++) {

							if (j == k)
								continue;

							ABParserFutureToken<T>* currentTrigger = verifyTokens[i]->Triggers[k];

							if (currentTrigger->Token->TokenLength > thisLength) {
								currentVerifyTriggers.push_back(currentTrigger);
								currentVerifyTriggerStarts.push_back(verifyTokens[i]->TriggerStarts[k]);
								areAnyLonger = true;
							}
						}

						if (areAnyLonger) {

							// Now, we need to verify THIS trigger, so, to do that we need to stop verifying the existing token, and start verifying this trigger.
							StopVerify(i, false);
							StartVerify(LoadCurrentTriggersInto(new ABParserVerifyToken<T>(trigger, currentVerifyTriggerStarts[i], GenerateVerifyTrailing())));

							return -1;
						}
					}

					StopVerify(i, true);
					return FinalizeToken(trigger, index, buildUp, buildUpLength, true);
				}
			}

		return 0;
	}

	ABParserResult FinalizeNextVerifyToken() {
		debugLog("Finalizing verify original token...");

		// Determine the next item to finalize.
		ABParserVerifyToken<T>* nextItem = nullptr;
		for (; finalizingVerifyTokensCurrentToken < verifyTokens.size(); finalizingVerifyTokensCurrentToken++)
			if (verifyTokens[finalizingVerifyTokensCurrentToken]->HasNoTriggers) {
				nextItem = verifyTokens[finalizingVerifyTokensCurrentToken];
				break;
			}

		// Stop if we reached the end.
		if (nextItem == nullptr) {

			isFinalizingVerifyTokens = false;

			// Set the buildUp to the trailing of the last token so that it can be used as the leading of the next token.
			// We're adding 1 and pushing the length back by 1 because the first character in the "trailingBuildUp" is the last character of the token.
			buildUp = lastVerifyToken->TrailingBuildUp + 1;
			buildUpLength = lastVerifyToken->TrailingBuildUpLength - 1;
			lastVerifyToken = nullptr;

			// The character we're currently on never got added to the buildUp, so, we'll add it to the buildUp now.
			StopVerify(0, true);
			AddCharacterToBuildUp(Text[currentPosition - 1]);

			return ABParserResult::None;
		}

		// Finalize the next token, and remove it.
		bool isFirst = lastVerifyToken == nullptr;
		ABParserResult result = FinalizeToken(verifyTokens.front(), isFirst ? buildUp : lastVerifyToken->TrailingBuildUp, isFirst ? buildUpLength : lastVerifyToken->TrailingBuildUpLength, false);
		lastVerifyToken = verifyTokens.front();
		StopVerify(0, true);

		return result;
	}

	ABParserVerifyToken<T>* LoadCurrentTriggersInto(ABParserVerifyToken<T>* token) {
		token->TriggersLength = currentVerifyTriggers.size();
		token->TriggerStartsLength = currentVerifyTriggerStarts.size();

		ABParserFutureToken<T>** triggers = token->Triggers = new ABParserFutureToken<T>*[token->TriggersLength];
		int* triggerStarts = token->TriggerStarts = new int[token->TriggerStartsLength];

		// Copy across the values.
		for (int i = 0; i < token->TriggersLength; i++)
			triggers[i] = currentVerifyTriggers[i];
		for (int i = 0; i < token->TriggerStartsLength; i++)
			triggerStarts[i] = currentVerifyTriggerStarts[i];

		// Finally, return our new modified verify token!
		return token;

	}

	T* GenerateVerifyTrailing() {
		return &buildUpStart[currentPosition];
	}

	// FINALIZE
	ABParserResult FinalizeToken(ABParserVerifyToken<T>* verifyToken, T* buildUpToUse, int buildUpToUseLength, bool resetBuildUp) {
		if (verifyToken->IsSingleChar)
			return FinalizeToken(verifyToken->SingleChar, verifyToken->Start, buildUp, buildUpToUseLength, resetBuildUp);
		else
			return FinalizeToken(verifyToken->MultiChar, verifyToken->Start, buildUp, buildUpToUseLength, resetBuildUp);
	}

	ABParserResult FinalizeToken(SingleCharToken<T>* token, int index, T* buildUpToUse, int buildUpToUseLength, bool resetBuildUp) {

		debugLog("Finalizing single-char token");

		PrepareLeadingAndTrailing(1, index, buildUpToUse, buildUpToUseLength, resetBuildUp, false);
		return QueueTokenAndReturnFinalizeResult(token, index, hasQueuedToken);
	}

	ABParserResult FinalizeToken(ABParserFutureToken<T>* token, int index, T* buildUpToUse, int buildUpToUseLength, bool resetBuildUp) {

		debugLog("Finalizing multi-char token");

		PrepareLeadingAndTrailing(token->Token->TokenLength, index, buildUpToUse, buildUpToUseLength, resetBuildUp, false);
		token->Disabled = true;
		return QueueTokenAndReturnFinalizeResult(token->Token, index, hasQueuedToken);
	}

	ABParserResult QueueTokenAndReturnFinalizeResult(ABParserToken* token, int index, bool hadQueuedToken) {
		OnTokenProcessedPreviousToken = OnTokenProcessedToken;
		OnTokenProcessedPreviousTokenStart = OnTokenProcessedTokenStart;

		OnTokenProcessedTokenStart = BeforeTokenProcessedTokenStart;
		OnTokenProcessedToken = BeforeTokenProcessedToken;

		BeforeTokenProcessedTokenStart = index;
		BeforeTokenProcessedToken = token;
		hasQueuedToken = true;

		// Based on whether there was a queued-up token before, we'll either trigger "BeforeTokenProcessed" or both that and "OnTokenProcessed".
		if (hadQueuedToken)
			return ABParserResult::OnAndBeforeTokenProcessed;
		else
			return ABParserResult::BeforeTokenProcessed;
	}

	void PrepareLeadingAndTrailing(int tokenLength, int tokenStart, T* buildUpToUse, int buildUpToUseLength, bool resetBuildUp, bool isEnd) {
		debugLog("Preparing leading and trailing for token.");

		// We are only deleting the leading, and not the buildUp because the buildUp is only ever allocated once, whereas the leading, which is set to the previous trailing, is allocated everytime, as you can see below.
		OnTokenProcessedLeadingLength = 0;
		for (int i = 0; i < OnTokenProcessedTrailingLength; i++)
			OnTokenProcessedLeading[OnTokenProcessedLeadingLength++] = OnTokenProcessedTrailing[i];
		OnTokenProcessedLeading[OnTokenProcessedTrailingLength] = 0;

		// Now, we need to work out how much of the buildUp is what we really want, because the buildUp will contain this token or part of it at the end, and we need to trim that off.
		int trailingLength;
		if (isEnd)
			trailingLength = buildUpToUseLength;
		else
			trailingLength = tokenStart - (BeforeTokenProcessedToken == nullptr ? 0 : BeforeTokenProcessedTokenStart + BeforeTokenProcessedToken->GetLength());

		OnTokenProcessedTrailingLength = 0;
		for (int i = 0; i < trailingLength; i++)
			OnTokenProcessedTrailing[OnTokenProcessedTrailingLength++] = buildUpToUse[i];
		OnTokenProcessedTrailing[trailingLength] = 0;

		if (resetBuildUp)
			buildUpLength = 0;
	}

	// HELPERS
	void AddFutureToken(MultiCharToken<T>* token) {
		futureTokens[currentPosition][currentPositionFutureTokensLength++] = new ABParserFutureToken<T>(token);

		// Mark the next as the end of the array, if this is the end.
		futureTokens[currentPosition][currentPositionFutureTokensLength] = nullptr;
	}

	void MarkFinishedFutureToken(ABParserFutureToken<T>* futureToken) {
		futureToken->Finished = true;
	}

	void TrimFutureTokens() {
		futureTokensHead++;
	}

	void DisableFutureToken(ABParserFutureToken<T>* futureToken, int index) {
		futureToken->Disabled = true;

		if (isVerifying)
			CheckDisabledFutureToken(futureToken, index);
	}

	void ResetCurrentTokens() {
		if (!singleCharCurrentTokensIsTokens) {

			debugLog("Resetting single character current tokens... NumberOfSingleCharTokens: %d", Tokens->NumberOfSingleCharTokens);
			singleCharCurrentTokensIsTokens = true;
			for (int i = 0; i < Tokens->NumberOfSingleCharTokens; i++)
				singleCharCurrentTokens[i] = Tokens->SingleCharTokens[i];
			singleCharCurrentTokensLength = Tokens->NumberOfSingleCharTokens;
		}

		if (!multiCharCurrentTokensIsTokens) {

			debugLog("Resetting multi-character current tokens... NumberOfMultiCharTokens: %d", Tokens->NumberOfMultiCharTokens);
			multiCharCurrentTokensIsTokens = true;
			for (int i = 0; i < Tokens->NumberOfMultiCharTokens; i++)
				multiCharCurrentTokens[i] = Tokens->MultiCharTokens[i];
			multiCharCurrentTokensLength = Tokens->NumberOfMultiCharTokens;

		}
	}

	void ConfigureCurrentTokens(int* validSingleCharTokens, int singleCharLength, int* validMultiCharTokens, int multiCharLength) {

		debugLog("Configuring CurrentTokens");

		singleCharCurrentTokensIsTokens = false;
		multiCharCurrentTokensIsTokens = false;

		// Limit all of the tokens down to only the ones that are in the validTokens array.
		for (int i = 0; i < Tokens->NumberOfSingleCharTokens; i++)
			for (int j = 0; j < singleCharLength; j++)
				if (i == validSingleCharTokens[j]) {
					singleCharCurrentTokens[singleCharCurrentTokensLength++] = Tokens->SingleCharTokens[i];
					break;
				}

		for (int i = 0; i < Tokens->NumberOfMultiCharTokens; i++)
			for (int j = 0; j < multiCharLength; j++)
				if (i == validMultiCharTokens[j]) {
					multiCharCurrentTokens[multiCharCurrentTokensLength++] = Tokens->MultiCharTokens[i];
					break;
				}
	}

	void InitTokens() {
		debugLog("Initializing Tokens");

		singleCharCurrentTokens = new SingleCharToken<T>*[Tokens->NumberOfSingleCharTokens];
		singleCharCurrentTokensLength = 0;

		multiCharCurrentTokens = new MultiCharToken<T>*[Tokens->NumberOfMultiCharTokens];
		multiCharCurrentTokensLength = 0;
	}

	void AddVerifyToken(ABParserVerifyToken<T>* token) {
		verifyTokens.push_back(token);
		currentVerifyToken = token;
	}

	void RemoveVerifyToken(int index) {
		auto tokenToRemoveIterator = verifyTokens.begin() + index;
		verifyTokensToDelete.push_back(verifyTokens[index]);
		verifyTokens.erase(tokenToRemoveIterator);

		// If that was the last of them, stop verifying.
		if (!verifyTokens.size())
			isVerifying = false;
	}
};
#endif