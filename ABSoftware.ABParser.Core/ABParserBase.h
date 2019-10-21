#pragma once
#include "ABParserFutureToken.h"
#include "VerifyToken.h"
#include "SingleCharToken.h"
#include "MultiCharToken.h"
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
	vector<VerifyToken> verifyTokens;
	vector<VerifyToken> verifyTriggers;
	vector<wchar_t*> verifyBuildUp;

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
	void StartVerify(SingleCharToken* token);
	void StartVerify(ABParserFutureToken* token, int index);

	bool SingleCharNeedsVerification(wchar_t ch, SingleCharToken* token);
	bool MultiCharNeedsVerification(ABParserFutureToken* token, int index);

	// FINALIZE
	int FinalizeToken(SingleCharToken* token, int index);
	int FinalizeToken(ABParserFutureToken* token, int index);

	void PrepareLeadingAndTrailing(int tokenLength);
	int QueueTokenAndReturnFinalizeResult(ABParserToken* token, int index, bool hasQueuedToken);

	// HELPERS
	void AddFutureToken(MultiCharToken* token);
	void MarkFinishedFutureToken(int firstDimension, int secondDimension);
	void TrimFutureTokens();
	void DisableFutureToken(int firstDimension, int secondDimension);
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