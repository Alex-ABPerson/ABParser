#pragma once
#include "ABParserFutureToken.h"
#include "SingleCharToken.h"
#include "MultiCharToken.h"
#include "ABParserVerifyToken.h"
#include <vector>
using namespace std;

class ABParserBase {
private:

	// NOTE: futureTokens can only be multiple characters.
	ABParserFutureToken*** futureTokens;

	// When we add "futureTokens" we need to keep track of how many futureTokens we've already added so far in order to know at what second dimension to place the futureToken.
	int currentPositionFutureTokensLength;
	//int* futureTokenLengths;
	int futureTokensHead;
	int futureTokensTail;
	int currentPosition;

	bool isVerifying;
	bool isFinalizingVerifyToken;
	bool lastTokenWasOriginalVerified;

	// This states the position that verify started on.
	int verifyStartPosition;
	vector<ABParserVerifyToken*> verifyTokens;
	vector<ABParserFutureToken*> verifyTriggers;
	vector<wchar_t*> verifyBuildUp;
	vector<int> verifyBuildUpLengths;

	bool hasQueuedToken;

	wchar_t* buildUp;
	int buildUpLength;

	// Whether the "CurrentTokens" array actually just points to the "Tokens" array.
	// We can use this to decide whether to "delete" CurrentTokens when configuring/resetting it.
	bool singleCharCurrentTokensIsTokens;
	bool multiCharCurrentTokensIsTokens;

	SingleCharToken* singleCharCurrentTokens;
	int singleCharCurrentTokensLength;

	MultiCharToken* multiCharCurrentTokens;
	int multiCharCurrentTokensLength;
	
	wchar_t** thingsToDelete;
	int buildUpsToDeleteLength;

	// COLLECT
	int ProcessChar(wchar_t ch);
	void AddCharacterToBuildUp(wchar_t ch);
	void UpdateCurrentFutureTokens(wchar_t ch);
	void AddNewFutureTokens(wchar_t ch);
	int ProcessFinishedTokens(wchar_t ch);

	// VERIFY
	void StartVerify(ABParserVerifyToken* token);
	void StopVerify();
	bool PrepareSingleCharForVerification(wchar_t ch, SingleCharToken* token);
	bool PrepareMultiCharForVerification(ABParserFutureToken* token, int index);
	void CheckDisabledFutureToken(ABParserFutureToken* token, int index);
	int CheckFinishedFutureToken(ABParserFutureToken* token, int index);
	int FinalizeNextVerifyToken();

	// FINALIZE
	int FinalizeToken(ABParserVerifyToken* verifyToken);
	int FinalizeToken(SingleCharToken* token, int index);
	int FinalizeToken(ABParserFutureToken* token, int index, bool isVerifyTriggerToken);

	void PrepareLeadingAndTrailing(int tokenLength, bool isVerifyTriggerToken, bool endOfText);
	int QueueTokenAndReturnFinalizeResult(ABParserToken* token, int index, bool hasQueuedToken);

	// HELPERS
	void AddFutureToken(MultiCharToken* token);
	void MarkFinishedFutureToken(ABParserFutureToken* futureToken);
	void TrimFutureTokens();
	void DisableFutureToken(ABParserFutureToken* futureToken, int index);
	void ResetCurrentTokens(bool deleteCurrentTokens);
	void ConfigureCurrentTokens(int* validSingleCharTokens, int singleCharLength, int* validMultiCharTokens, int multiCharLength);
	void InitTokens(SingleCharToken* singleCharTokens, int singleCharTokensLength, MultiCharToken* multiCharTokens, int multiCharTokensLength);

public:
	SingleCharToken* SingleCharTokens;
	int NumberOfSingleCharTokens;

	MultiCharToken* MultiCharTokens;
	int NumberOfMultiCharTokens;

	int BeforeTokenProcessedTokenStart;
	ABParserToken* BeforeTokenProcessedToken;

	int OnTokenProcessedTokenStart;
	ABParserToken* OnTokenProcessedToken;

	int OnTokenProcessedPreviousTokenStart;
	ABParserToken* OnTokenProcessedPreviousToken;

	wchar_t* Text;
	int TextLength;

	wchar_t* OnTokenProcessedLeading;
	int OnTokenProcessedLeadingLength;

	wchar_t* OnTokenProcessedTrailing;
	int OnTokenProcessedTrailingLength;

	void InitString(unsigned short* text, int textLength);
	int ContinueExecution();
	ABParserBase(SingleCharToken* singleCharTokens, int singleCharTokensLength, MultiCharToken* multiCharTokens, int multiCharTokensLength);
	~ABParserBase();

};