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

	// When all of the triggers in a verify token gets removed, then we finalize that token! However, sometimes there may be lots of verify tokens that all had the same triggers, so, we'll finalize them all in one go with this!
	bool isFinalizingVerifyTokens;
	ABParserVerifyToken* lastVerifyToken;
	size_t finalizingVerifyTokensCurrentToken;

	vector<ABParserVerifyToken*> verifyTokens;

	// The verify token right at the end of the verifyTokens.
	ABParserVerifyToken* currentVerifyToken;
	
	// As we're preparing a token for verification, we'll use this temporaily.
	vector<ABParserFutureToken*> currentVerifyTriggers; // TODO: WTF is this?!
	vector<int> currentVerifyTriggerStarts;

	bool hasQueuedToken;

	wchar_t* buildUp;
	int buildUpLength;

	// Whether the "CurrentTokens" array actually just points to the "Tokens" array.
	// We can use this to decide whether to "delete" CurrentTokens when configuring/resetting it.
	bool singleCharCurrentTokensIsTokens;
	bool multiCharCurrentTokensIsTokens;

	SingleCharToken** singleCharCurrentTokens;
	int singleCharCurrentTokensLength;

	MultiCharToken** multiCharCurrentTokens;
	int multiCharCurrentTokensLength;
	
	// DISPOSAL
	vector<wchar_t*> buildUpsToDelete;
	vector<ABParserVerifyToken*> verifyTokensToDelete;

	// COLLECT
	int ProcessChar(wchar_t ch);
	void AddCharacterToBuildUp(wchar_t ch);
	int UpdateCurrentFutureTokens(wchar_t ch);
	void AddNewFutureTokens(wchar_t ch);
	int ProcessFinishedTokens(wchar_t ch);

	// VERIFY
	void StartVerify(ABParserVerifyToken* token);
	void StopVerify(int tokenIndex, bool wasFinalized);
	bool PrepareSingleCharForVerification(wchar_t ch, SingleCharToken* token);
	bool PrepareMultiCharForVerification(ABParserFutureToken* token, int index);
	int CheckDisabledFutureToken(ABParserFutureToken* token, int index);
	int CheckFinishedFutureToken(ABParserFutureToken* token, int index);
	int FinalizeNextVerifyToken();
    ABParserVerifyToken* LoadCurrentTriggersInto(ABParserVerifyToken* token);
	wchar_t* GenerateVerifyTrailing();

	// FINALIZE
	int FinalizeToken(ABParserVerifyToken* verifyToken, wchar_t* buildUpToUse, int buildUpToUseLength, bool resetBuildUp);
	int FinalizeToken(SingleCharToken* token, int index, wchar_t* buildUpToUse, int buildUpToUseLength, bool resetBuildUp);
	int FinalizeToken(ABParserFutureToken* token, int index, wchar_t* buildUpToUse, int buildUpToUseLength, bool resetBuildUp);

	void PrepareLeadingAndTrailing(int tokenLength, int tokenStart, wchar_t* buildUpToUse, int buildUpToUseLength, bool resetBuildUp);
	int QueueTokenAndReturnFinalizeResult(ABParserToken* token, int index, bool hasQueuedToken);

	// HELPERS
	void AddFutureToken(MultiCharToken* token);
	void MarkFinishedFutureToken(ABParserFutureToken* futureToken);
	void TrimFutureTokens();
	int DisableFutureToken(ABParserFutureToken* futureToken, int index);
	void ResetCurrentTokens(bool deleteCurrentTokens);
	void ConfigureCurrentTokens(int* validSingleCharTokens, int singleCharLength, int* validMultiCharTokens, int multiCharLength);
	void InitTokens(SingleCharToken* singleCharTokens, int singleCharTokensLength, MultiCharToken* multiCharTokens, int multiCharTokensLength);
	void AddVerifyToken(ABParserVerifyToken* token);
	void RemoveVerifyToken(int index);

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

	// Prepares for the next parse.
	void PrepareForParse(unsigned short* text, int textLength);

	// Disposes data after a parse has been completed.
	void DisposeDataForNextParse();

};