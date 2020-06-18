#ifndef _ABPARSER_INCLUDE_ABPARSER_MAIN_H
#define _ABPARSER_INCLUDE_ABPARSER_MAIN_H
#include "ABParserHelpers.h"
#include "ABParserConfig.h"
#include "ABParserDebugging.h"
#include <string>
#include <vector>
#include <stack>
#include <wchar.h>

namespace abparser {
	template<typename T, typename U = char>
	class ABParserBase {
	public:

		ABParserConfiguration<T, U>* Configuration;

		uint32_t BeforeTokenProcessedTokenStart;
		ABParserInternalToken<T>* BeforeTokenProcessedToken;
		uint32_t BeforeTokenProcessedTokenLengthInText;

		uint32_t OnTokenProcessedTokenStart;
		ABParserInternalToken<T>* OnTokenProcessedToken;

		uint32_t OnTokenProcessedPreviousTokenStart;
		ABParserInternalToken<T>* OnTokenProcessedPreviousToken;

		T* Text;
		uint32_t TextLength;

		T* OnTokenProcessedLeading;
		uint32_t OnTokenProcessedLeadingLength;

		T* OnTokenProcessedTrailing;
		uint32_t OnTokenProcessedTrailingLength;

		std::stack<TokenLimit<T, U>*> CurrentTokenLimits;
		std::stack<TriviaLimit<T, U>*> CurrentTriviaLimits;

		ABParserResult ContinueExecution() {

			if (isFinalizingVerifyTokens) {
				ABParserResult result = FinalizeNextVerifyToken();
				if (result != ABParserResult::None)
					return result;
			}

			if (justStarted) PrepareForParse();

			_ABP_DEBUG_OUT("Continuing execution... Finished: %c ", (currentPosition < TextLength) ? 'F' : 'T');

			// The main loop - go through every character.
			for (; currentPosition < TextLength; currentPosition++) {
				_ABP_DEBUG_OUT("Current Position: %d", currentPosition);

				ABParserResult res = ProcessChar();

				// Return any result we got.
				if (res != ABParserResult::None) {
					currentPosition++;
					return res;
				}
			}

			// If there's a token left, we'll prepare the leading and trailing for it so that when we trigger the "stop" result, it can be the final OnTokenProcessed.
			if (BeforeTokenProcessedToken)
				PrepareLeadingAndTrailing((currentPosition - 1) - BeforeTokenProcessedTokenLengthInText, buildUp, buildUpLength, false, true);

			// Reset anything for next time.
			while (!CurrentTokenLimits.empty())
				CurrentTokenLimits.pop();
			while (!CurrentTriviaLimits.empty())
				CurrentTriviaLimits.pop();
			ResetCurrentTokens();

			justStarted = true;
			isFinalizingVerifyTokens = false;
			return ABParserResult::StopAndFinalOnTokenProcessed;
		}

		ABParserBase() {
			InitParser();
		}

		ABParserBase(ABParserConfiguration<T, U>* configuration) {
			InitParser();
			InitConfiguration(configuration);
		}

		void InitParser() {
			Text = nullptr;
			TextLength = 0;
			OnTokenProcessedLeading = nullptr;
			OnTokenProcessedTrailing = nullptr;
			OnTokenProcessedLeadingLength = 0;
			OnTokenProcessedTrailingLength = 0;

			BeforeTokenProcessedToken = nullptr;
			BeforeTokenProcessedTokenStart = 0;
			OnTokenProcessedPreviousToken = nullptr;
			OnTokenProcessedPreviousTokenStart = 0;
			OnTokenProcessedToken = nullptr;
			OnTokenProcessedTokenStart = 0;

			isVerifying = false;
			isFinalizingVerifyTokens = false;
			hasQueuedToken = false;
			buildUpStart = nullptr;
			futureTokens = nullptr;
			justStarted = true;

			// Estimated to have 2 verifyTokens at a given time.
			verifyTokens.reserve(2);
			verifyTokensToDelete.reserve(2);
		}

		void InitConfiguration(ABParserConfiguration<T, U>* configuration) {
			Configuration = configuration;

			currentVerifyTriggers.reserve(Configuration->NumberOfMultiCharTokens);
			currentVerifyTriggerStarts.reserve(Configuration->NumberOfMultiCharTokens);

			ResetCurrentTokens();
		}

		~ABParserBase() {
			_ABP_DEBUG_OUT("Disposing data for complete parser deletion.");
			DisposeForTextChange(true);
		}

		// Prepares for the next parse.
		void PrepareForParse() {
			_ABP_DEBUG_OUT("Preparing for a parse.");

			currentPosition = 0;

			BeforeTokenProcessedToken = nullptr;
			BeforeTokenProcessedTokenStart = 0;
			BeforeTokenProcessedTokenLengthInText = 0;
			OnTokenProcessedToken = nullptr;
			OnTokenProcessedTokenStart = 0;

			OnTokenProcessedLeadingLength = 0;
			OnTokenProcessedTrailingLength = 0;

			futureTokensHead = 0;
			futureTokensTail = 0;
			isVerifying = false;
			hasQueuedToken = false;
			buildUpLength = 0;
			buildUp = buildUpStart;
			justStarted = false;
		}

		void InitString(T* text, uint32_t textLength) {
			_ABP_DEBUG_OUT("Initializing String. Text Length: %d", textLength);

			// Re-allocate anything that wouldn't work on the new text.
			bool recreateTextSpecific = TextLength < textLength;
			DisposeForTextChange(recreateTextSpecific);

			Text = new T[textLength];
			TextLength = textLength;

			for (uint32_t i = 0; i < textLength; i++)
				Text[i] = text[i];

			if (recreateTextSpecific) {
				buildUpStart = buildUp = new T[textLength];
				OnTokenProcessedLeading = new T[(size_t)textLength + 1];
				OnTokenProcessedTrailing = new T[(size_t)textLength + 1];

				futureTokens = new ABParserFutureToken<T> * [TextLength];
				for (uint32_t i = 0; i < TextLength; i++)
					futureTokens[i] = new ABParserFutureToken<T>[Configuration->NumberOfMultiCharTokens + 1];
			}
		}

		void EnterTokenLimit(U* limitName, uint8_t limitNameSize) {
			for (uint16_t i = 0; i < Configuration->NumberOfTokenLimits; i++) {
				TokenLimit<T, U>* currentLimit = Configuration->TokenLimits[i];

				if (Matches(limitName, (U*)currentLimit->LimitName->data(), limitNameSize, currentLimit->LimitName->length())) {
					_ABP_DEBUG_OUT("Entered into token limit");
					CurrentTokenLimits.push(currentLimit);
					SetCurrentTokens(currentLimit);
					break;
				}
			}
		}

		void ExitTokenLimit() {
			CurrentTokenLimits.pop();

			if (CurrentTokenLimits.empty())
				ResetCurrentTokens();
			else
				SetCurrentTokens(CurrentTokenLimits.top());
		}

		void EnterTriviaLimit(U* limitName, uint8_t limitNameSize) {
			for (uint16_t i = 0; i < Configuration->NumberOfTriviaLimits; i++) {
				TriviaLimit<T, U>* limit = &Configuration->TriviaLimits[i];

				if (Matches(limitName, (U*)limit->LimitName->data(), limitNameSize, limit->LimitName->length())) {
					_ABP_DEBUG_OUT("Entered into trivia limit");
					CurrentTriviaLimits.push(limit);
					break;
				}
			}
		}

		void ExitTriviaLimit() {
			CurrentTriviaLimits.pop();
		}

		// Disposes data after a parse has been completed.
		void DisposeDataForNextParse() {
			_ABP_DEBUG_OUT("Disposing data ready for the next parse.");

			for (size_t i = 0; i < verifyTokensToDelete.size(); i++)
				delete verifyTokensToDelete[i];

			verifyTokensToDelete.clear();
		}

		void DisposeForTextChange(bool disposeBuildUpAndFutureTokens) {
			if (disposeBuildUpAndFutureTokens) {

				if (futureTokens) {
					for (uint32_t i = 0; i < TextLength; i++)
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
		bool justStarted;

		ABParserFutureToken<T>** futureTokens;
		uint32_t futureTokensHead;
		uint32_t futureTokensTail;

		uint32_t currentPosition;

		bool isVerifying;

		// When all of the triggers in a verify token gets removed, then we finalize that token! However, sometimes there may be lots of verify tokens that all had the same triggers, so, we'll finalize them all in one go with this!
		bool isFinalizingVerifyTokens;
		ABParserVerifyToken<T>* lastVerifyToken;
		uint32_t finalizingVerifyTokensCurrentToken;

		std::vector<ABParserVerifyToken<T>*> verifyTokens;

		// The verify token right at the end of the verifyTokens.
		ABParserVerifyToken<T>* currentVerifyToken;

		// As we're preparing a token for verification, we'll use this temporaily.
		std::vector<ABParserFutureToken<T>*> currentVerifyTriggers;
		std::vector<uint32_t> currentVerifyTriggerStarts;

		bool hasQueuedToken;

		// When verify tokens are finalized, the buildUp is pushed forwards to make it point towards the last verify tokens' trailing, since that is the correct leading for the next token. This is the buildUp without any of that "movement".
		T* buildUpStart;
		T* buildUp;
		uint32_t buildUpLength;

		uint32_t buildUpOffset;

		SingleCharToken<T>** singleCharCurrentTokens;
		uint16_t singleCharCurrentTokensLength;

		MultiCharToken<T>** multiCharCurrentTokens;
		uint16_t multiCharCurrentTokensLength;

		// DISPOSAL
		std::vector<ABParserVerifyToken<T>*> verifyTokensToDelete;

		// COLLECT
		ABParserResult ProcessChar() {

			_ABP_DEBUG_OUT("Processing character: %c", Text[currentPosition]);

			UpdateCurrentFutureTokens();
			AddNewFutureTokens();
			ABParserResult result = ProcessFinishedTokens();

			if (result != ABParserResult::None) return result;
			if (isFinalizingVerifyTokens) return FinalizeNextVerifyToken();

			AddCharToBuildUp(Text[currentPosition]);
			return ABParserResult::None;
		}

		void AddCharToBuildUp(T ch) {
			if (isVerifying)
				currentVerifyToken->TrailingBuildUp[currentVerifyToken->TrailingBuildUpLength++] = ch;
			else
				buildUp[buildUpLength++] = ch;
		}

		void UpdateCurrentFutureTokens() {

			_ABP_DEBUG_OUT("Updating future tokens.");

			for (uint32_t i = futureTokensHead; i < futureTokensTail; i++)
			{
				bool hasUnfinalizedFutureToken = false;

				for (uint16_t j = 0; j < Configuration->NumberOfMultiCharTokens; j++)
				{
					if (futureTokens[i][j].EndOfArray) break;
					if (futureTokens[i][j].CollectionComplete) continue;

					hasUnfinalizedFutureToken = true;

					futureTokens[i][j].LengthInText++;

					// If this future tokens' detection limit tells us to ignore this character, then do so.
					if (futureTokens[i][j].Token->DetectionLimitSize > 0)
						if (ArrContainsChar(futureTokens[i][j].Token->DetectionLimit, futureTokens[i][j].Token->DetectionLimitSize, Text[currentPosition]))
							continue;

					// Check if this character matches the next character in this token.
					if (futureTokens[i][j].Token->TokenContents[futureTokens[i][j].NoOfCharactersMatched] == Text[currentPosition]) {
						futureTokens[i][j].NoOfCharactersMatched++;

						// If all the characters have matched, then mark this token as complete.
						if (futureTokens[i][j].Token->TokenLength == futureTokens[i][j].NoOfCharactersMatched)
							futureTokens[i][j].Finished = true;
					}
					else
						DisableFutureToken(&futureTokens[i][j]);
				}

				// Trim off any parts of the futureTokens that no longer contain anything.
				if (!hasUnfinalizedFutureToken)
					if (i == futureTokensHead) futureTokensHead++;
					else futureTokens[i][0].EndOfArray = true;
			}
		}

		void AddNewFutureTokens() {

			_ABP_DEBUG_OUT("Adding future tokens.");
			futureTokensTail++;

			uint16_t currentLength = 0;
			for (uint16_t i = 0; i < multiCharCurrentTokensLength; i++)
				if (multiCharCurrentTokens[i]->TokenContents[0] == Text[currentPosition])
					AddFutureToken(multiCharCurrentTokens[i], currentLength++);

			futureTokens[currentPosition][currentLength].EndOfArray = true;
		}

		ABParserResult ProcessFinishedTokens() {
			_ABP_DEBUG_OUT("Processing finished future tokens.");

			// We deal with the multiple character long tokens first because they might contain single character tokens, so, if we process them first,
			// then the "PrepareSingleCharForVerification" can look at these futureTokens. Also, longer futureTokens are more important than shorter ones.
			for (uint32_t i = futureTokensHead; i < futureTokensTail; i++) {

				// We'll ignore if there are two tokens both finished, as the only way that can occur is if two tokens are identical.
				for (uint16_t j = 0; j < Configuration->NumberOfMultiCharTokens; j++) {

					if (futureTokens[i][j].EndOfArray) break;
					if (futureTokens[i][j].CollectionComplete) continue;

					if (futureTokens[i][j].Finished) {

						_ABP_DEBUG_OUT("Finished multi-char token!");

						futureTokens[i][j].CollectionComplete = true;

						// If we are currently verifying, then we need to do some extra checks on it.
						if (isVerifying)
							if (int result = CheckFinishedFutureToken(&futureTokens[i][j], i))
								if (result == -1) return ABParserResult::None;
								else return static_cast<ABParserResult>(result);

						// Finalize it or verify it.
						if (PrepareMultiCharForVerification(&futureTokens[i][j], i))
							StartVerify(LoadCurrentTriggersInto(new ABParserVerifyToken<T>(&futureTokens[i][j], false, i, GenerateVerifyTrailing())));
						else
							return FinalizeToken(&futureTokens[i][j], i, buildUp, buildUpLength, true);
					}
				};
			}

			for (uint16_t i = 0; i < singleCharCurrentTokensLength; i++) {
				if (singleCharCurrentTokens[i]->TokenChar == Text[currentPosition]) {

					_ABP_DEBUG_OUT("Finished single-char token!");

					// Finalize it or verify it.
					if (PrepareSingleCharForVerification(Text[currentPosition], singleCharCurrentTokens[i]))
						StartVerify(LoadCurrentTriggersInto(new ABParserVerifyToken<T>(singleCharCurrentTokens[i], true, currentPosition, GenerateVerifyTrailing())));
					else
						return FinalizeToken(singleCharCurrentTokens[i], currentPosition, buildUp, buildUpLength, true);
				}
			}

			return ABParserResult::None;
		}

		// VERIFY
		void StartVerify(ABParserVerifyToken<T>* token) {
			_ABP_DEBUG_OUT("Starting verify.");

			isVerifying = true;
			AddVerifyToken(token);

			currentVerifyTriggers.clear();
			currentVerifyTriggerStarts.clear();
		}

		void StopAllVerify() {
			if (verifyTokens.size()) {
				for (size_t i = 0; i < verifyTokens.size(); i++)
					verifyTokensToDelete.push_back(verifyTokens[i]);
				verifyTokens.clear();
			}

			isVerifying = false;
		}

		void StopVerify(uint32_t tokenIndex) {
			auto tokenToRemoveIterator = verifyTokens.begin() + tokenIndex;
			verifyTokensToDelete.push_back(verifyTokens[tokenIndex]);
			verifyTokens.erase(tokenToRemoveIterator);

			// If that was the last of them, stop verifying.
			if (!verifyTokens.size())
				isVerifying = false;
		}

		bool PrepareSingleCharForVerification(T ch, SingleCharToken<T>* token) {
			_ABP_DEBUG_OUT("Checking if single-char token requires verification.");

			bool needsToBeVerified = false;

			for (uint32_t i = futureTokensHead; i < futureTokensTail; i++)
				for (uint16_t j = 0; j < Configuration->NumberOfMultiCharTokens; j++) {
					if (futureTokens[i][j].EndOfArray) break;
					if (futureTokens[i][j].CollectionComplete) continue;

					ABParserFutureToken<T>* multiCharToken = &futureTokens[i][j];
					if (multiCharToken->Token->TokenContents[currentPosition - i] == ch) {

						needsToBeVerified = true;

						// Keep track of all of the multiCharTokens that could be an issue for us, as those will become triggers if we really need to verify.
						currentVerifyTriggers.push_back(multiCharToken);
						currentVerifyTriggerStarts.push_back(i);
					}

				}

			if (needsToBeVerified) {
				_ABP_DEBUG_OUT("Single-char token does require verification.");
				isVerifying = true;
			}
			else {
				currentVerifyTriggers.clear();
				currentVerifyTriggerStarts.clear();
			}

			return needsToBeVerified;
		}

		bool PrepareMultiCharForVerification(ABParserFutureToken<T>* token, uint32_t index) {
			_ABP_DEBUG_OUT("Checking if multi-char token requires verification.");
			bool needsToBeVerified = false;

			for (uint32_t i = futureTokensHead; i <= index; i++) {

				for (uint16_t j = 0; j < Configuration->NumberOfMultiCharTokens; j++) {
					if (futureTokens[i][j].EndOfArray) break;
					if (futureTokens[i][j].CollectionComplete) continue;

					ABParserFutureToken<T>* futureToken = &futureTokens[i][j];
					MultiCharToken<T>* multiCharToken = futureToken->Token;

					uint32_t distanceAway = index - i;

					// If the token isn't even long enough to contain our token, then we can ignore it.
					if (token->Token->TokenLength > multiCharToken->TokenLength)
						continue;

					bool contains = true;
					for (uint32_t k = 0; k < token->Token->TokenLength; k++)
						if (token->Token->TokenContents[k] != multiCharToken->TokenContents[k + distanceAway])
							contains = false;

					if (contains) {
						currentVerifyTriggers.push_back(futureToken);
						currentVerifyTriggerStarts.push_back(i);
						needsToBeVerified = true;
					}
				}
			}

			if (needsToBeVerified)
				token->IsBeingVerified = true;
			else {
				currentVerifyTriggers.clear();
				currentVerifyTriggerStarts.clear();
			}

			return needsToBeVerified;
		}

		void CheckDisabledFutureToken(ABParserFutureToken<T>* token) {
			_ABP_DEBUG_OUT("Checking disabled future token...");

			for (uint32_t i = 0; i < verifyTokens.size(); i++) {
				bool hasRemainingTriggers = false;

				for (uint16_t j = 0; j < verifyTokens[i]->TriggersLength; j++) {

					ABParserFutureToken<T>** trigger = &verifyTokens[i]->Triggers[j];

					if (*trigger == nullptr)
						continue;

					// Since this trigger has just ended, we can now remove it.
					if (*trigger == token) *trigger = nullptr;
					else hasRemainingTriggers = true;
				}

				// If there are no more triggers left in this token, then it WAS actually the token in the text - not the triggers! So, we'll go ahead and get ready to start finalizing this token.
				if (!hasRemainingTriggers) {
					// We'll reset the trigger length down to 0, to specifically tell us that this is one of the verify tokens that got completed.
					verifyTokens[i]->TriggersLength = 0;
					isFinalizingVerifyTokens = true;
				}
			}

		}

		int CheckFinishedFutureToken(ABParserFutureToken<T>* token, uint32_t index) {
			_ABP_DEBUG_OUT("Checking finished future token...");

			for (uint32_t i = 0; i < verifyTokens.size(); i++)
				for (uint16_t j = 0; j < verifyTokens[i]->TriggersLength; j++) {

					ABParserVerifyToken<T>* currentVerifyToken = verifyTokens[i];
					ABParserFutureToken<T>* trigger = currentVerifyToken->Triggers[j];

					if (trigger == token) {

						// Since this trigger was finished, it must have been this trigger all along, so stop verifying and finalize this trigger!
						// However, before we finalize this trigger - we need to check if we need verify it against one of the other triggers!
						if (currentVerifyToken->TriggersLength > 1) {

							uint32_t thisLength = trigger->Token->TokenLength;
							bool areAnyLonger = false;

							for (uint16_t k = 0; k < currentVerifyToken->TriggersLength; k++) {

								if (j == k) continue;

								ABParserFutureToken<T>* currentTrigger = currentVerifyToken->Triggers[k];
								if (currentTrigger == nullptr) continue;

								if (currentTrigger->Token->TokenLength > thisLength) {
									currentVerifyTriggers.push_back(currentTrigger);
									currentVerifyTriggerStarts.push_back(currentVerifyToken->TriggerStarts[k]);
									areAnyLonger = true;
								}
							}

							if (areAnyLonger) {

								// Now, we need to verify THIS trigger, so, to do that we need to stop verifying the existing token, and start verifying this trigger.
								StopVerify(i);
								StartVerify(LoadCurrentTriggersInto(new ABParserVerifyToken<T>(trigger, false, currentVerifyToken->TriggerStarts[j], GenerateVerifyTrailing())));

								return -1;
							}
						}

						StopAllVerify();
						return static_cast<int>(FinalizeToken(trigger, index, buildUp, buildUpLength, true));
					}
				}

			return 0;
		}

		ABParserResult FinalizeNextVerifyToken() {

			// We're adding "1" whenever we use the "TrailingBuildUp" because its first character is the last character of the token.
			_ABP_DEBUG_OUT("Finalizing verify original token...");

			// Determine the next item to finalize.
			ABParserVerifyToken<T>* nextItem = nullptr;
			for (; finalizingVerifyTokensCurrentToken < verifyTokens.size(); finalizingVerifyTokensCurrentToken++)
				if (verifyTokens[finalizingVerifyTokensCurrentToken]->TriggersLength == 0) {
					nextItem = verifyTokens[finalizingVerifyTokensCurrentToken];
					break;
				}

			if (!nextItem) {

				isFinalizingVerifyTokens = false;

				// Set the buildUp to the trailing of the last token so that it can be used as the leading of the next token.
				buildUp = lastVerifyToken->TrailingBuildUp + 1;
				buildUpLength = lastVerifyToken->TrailingBuildUpLength - 1;
				finalizingVerifyTokensCurrentToken = 0;
				lastVerifyToken = nullptr;

				// The character we were on when we started never got added to the buildUp, so, we'll add it to the buildUp now.
				StopAllVerify();
				AddCharToBuildUp(Text[currentPosition - 1]);

				return ABParserResult::None;
			}

			// Finalize the next token, and remove it.
			bool isFirst = lastVerifyToken == nullptr;
			ABParserResult result = FinalizeToken(verifyTokens.front(), isFirst ? buildUp : lastVerifyToken->TrailingBuildUp + 1, isFirst ? buildUpLength : lastVerifyToken->TrailingBuildUpLength - 1, false);
			lastVerifyToken = nextItem;
			StopVerify(finalizingVerifyTokensCurrentToken);

			return result;
		}

		ABParserVerifyToken<T>* LoadCurrentTriggersInto(ABParserVerifyToken<T>* token) {
			token->TriggersLength = (uint16_t)currentVerifyTriggers.size();

			ABParserFutureToken<T>** triggers = token->Triggers = new ABParserFutureToken<T> * [token->TriggersLength];
			uint32_t* triggerStarts = token->TriggerStarts = new uint32_t[token->TriggersLength];

			// Copy across the values.
			for (uint16_t i = 0; i < token->TriggersLength; i++)
				triggers[i] = currentVerifyTriggers[i];
			for (uint16_t i = 0; i < token->TriggersLength; i++)
				triggerStarts[i] = currentVerifyTriggerStarts[i];

			// Finally, return our new modified verify token!
			return token;

		}

		T* GenerateVerifyTrailing() {
			return &buildUpStart[currentPosition + 1];
		}

		// FINALIZE
		ABParserResult FinalizeToken(ABParserVerifyToken<T>* verifyToken, T* buildUpToUse, uint32_t buildUpToUseLength, bool resetBuildUp) {
			if (verifyToken->IsSingleChar)
				return FinalizeToken((SingleCharToken<T>*)verifyToken->Token, verifyToken->Start, buildUpToUse, buildUpToUseLength, resetBuildUp);
			else
				return FinalizeToken((ABParserFutureToken<T>*)verifyToken->Token, verifyToken->Start, buildUpToUse, buildUpToUseLength, resetBuildUp);
		}

		ABParserResult FinalizeToken(SingleCharToken<T>* token, uint32_t index, T* buildUpToUse, uint32_t buildUpToUseLength, bool resetBuildUp) {

			_ABP_DEBUG_OUT("Finalizing single-char token");

			PrepareLeadingAndTrailing(index, buildUpToUse, buildUpToUseLength, resetBuildUp, false);

			BeforeTokenProcessedTokenLengthInText = 1;

			return QueueTokenAndReturnFinalizeResult((ABParserInternalToken<T>*)token, index, hasQueuedToken);
		}

		ABParserResult FinalizeToken(ABParserFutureToken<T>* token, uint32_t index, T* buildUpToUse, uint32_t buildUpToUseLength, bool resetBuildUp) {

			_ABP_DEBUG_OUT("Finalizing multi-char token");

			PrepareLeadingAndTrailing(index, buildUpToUse, buildUpToUseLength, resetBuildUp, false);

			BeforeTokenProcessedTokenLengthInText = token->LengthInText;

			return QueueTokenAndReturnFinalizeResult((ABParserInternalToken<T>*)token->Token, index, hasQueuedToken);
		}

		ABParserResult QueueTokenAndReturnFinalizeResult(ABParserInternalToken<T>* token, uint32_t index, bool hadQueuedToken) {
			OnTokenProcessedPreviousToken = OnTokenProcessedToken;
			OnTokenProcessedPreviousTokenStart = OnTokenProcessedTokenStart;

			OnTokenProcessedTokenStart = BeforeTokenProcessedTokenStart;
			OnTokenProcessedToken = BeforeTokenProcessedToken;

			BeforeTokenProcessedTokenStart = index;
			BeforeTokenProcessedToken = token;
			hasQueuedToken = true;

			// Based on whether there was a queued-up token before, we'll either trigger "BeforeTokenProcessed" or both that and "OnTokenProcessed".
			if (hadQueuedToken)
				return ABParserResult::OnThenBeforeTokenProcessed;
			else
				return ABParserResult::BeforeTokenProcessed;
		}

		void PrepareLeadingAndTrailing(uint32_t tokenStart, T* buildUpToUse, uint32_t buildUpToUseLength, bool resetBuildUp, bool isEnd) {
			_ABP_DEBUG_OUT("Preparing leading and trailing for token.");

			// Swap the leading/trailing around, as the trailing becomes the leading.
			T* dataSwap = OnTokenProcessedLeading;
			uint32_t dataSwapLength = OnTokenProcessedLeadingLength;

			OnTokenProcessedLeading = OnTokenProcessedTrailing;
			OnTokenProcessedLeadingLength = OnTokenProcessedTrailingLength;

			OnTokenProcessedTrailing = dataSwap;
			OnTokenProcessedTrailingLength = dataSwapLength;

			// We need to work out how much of the buildUp is what we really want, as the buildUp will contain this token or part of it at the end, and we need to trim that off.
			uint32_t trailingLength;
			if (isEnd)
				trailingLength = buildUpToUseLength;
			else if (BeforeTokenProcessedToken)
				trailingLength = tokenStart - (BeforeTokenProcessedTokenStart + BeforeTokenProcessedTokenLengthInText);
			else
				trailingLength = tokenStart;

			OnTokenProcessedTrailingLength = 0;

			// Copy the buildUp into the final trailing, but excluding any of the trivia limit characters.
			if (CurrentTriviaLimits.empty())
				for (uint32_t i = 0; i < trailingLength; i++)
					OnTokenProcessedTrailing[OnTokenProcessedTrailingLength++] = buildUpToUse[i];
			else {

				TriviaLimit<T, U>* limit = CurrentTriviaLimits.top();

				for (uint32_t i = 0; i < trailingLength; i++) {
					if (ArrContainsChar(limit->ToIgnore, limit->ToIgnoreLength, buildUpToUse[i])) {
						_ABP_DEBUG_OUT("Character is in trivia limit - ignored!");
						continue;
					}

					OnTokenProcessedTrailing[OnTokenProcessedTrailingLength++] = buildUpToUse[i];
				}
			}

			OnTokenProcessedTrailing[OnTokenProcessedTrailingLength] = 0;

			if (resetBuildUp)
				buildUpLength = 0;
		}

		// HELPERS
		void AddFutureToken(MultiCharToken<T>* token, uint16_t currentLength) {
			futureTokens[currentPosition][currentLength].Reset(token);
			futureTokens[currentPosition][currentLength].LengthInText++;
			futureTokens[currentPosition][currentLength].NoOfCharactersMatched++;
		}

		void DisableFutureToken(ABParserFutureToken<T>* futureToken) {
			futureToken->CollectionComplete = true;
			if (isVerifying)
				CheckDisabledFutureToken(futureToken);
		}

		void ResetCurrentTokens() {
			singleCharCurrentTokens = Configuration->SingleCharTokens;
			singleCharCurrentTokensLength = Configuration->NumberOfSingleCharTokens;

			multiCharCurrentTokens = Configuration->MultiCharTokens;
			multiCharCurrentTokensLength = Configuration->NumberOfMultiCharTokens;
		}

		void SetCurrentTokens(TokenLimit<T, U>* limit) {
			singleCharCurrentTokens = limit->SingleCharTokens;
			singleCharCurrentTokensLength = limit->NumberOfSingleCharTokens;

			multiCharCurrentTokens = limit->MultiCharTokens;
			multiCharCurrentTokensLength = limit->NumberOfMultiCharTokens;
		}

		void AddVerifyToken(ABParserVerifyToken<T>* token) {
			verifyTokens.push_back(token);
			currentVerifyToken = token;
		}

		// LIMITS:
		bool ArrContainsChar(T* arr, uint16_t arrSize, T ch) {
			for (uint16_t i = 0; i < arrSize; i++)
				if (arr[i] == ch)
					return true;

			return false;
		}
	};
}
#endif