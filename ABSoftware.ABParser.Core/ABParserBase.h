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

	ABParserToken* queuedToken;
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

public:
	int PublicPosition;
	ABParserToken OnTokenProcessed;

	SingleCharToken* SingleCharTokens;
	int NumberOfSingleCharTokens;

	MultiCharToken* MultiCharTokens;
	int NumberOfMultiCharTokens;

	wchar_t* Text;
	int TextLength;

	wchar_t* OnTokenProcessedLeading;
	int OnTokenProcessedLeadingLength;

	wchar_t* OnTokenProcessedTrailing;
	int OnTokenProcessedTrailingLength;

	// GENERAL
	int ContinueExecution();

	// COLLECT
	int ProcessChar(wchar_t ch);
	void UpdateCurrentFutureTokens(wchar_t ch);
	void AddNewFutureTokens(wchar_t ch);
	int ProcessFinishedTokens(wchar_t ch);

	// VERIFY
	void StartVerify(SingleCharToken* token);
	void StartVerify(ABParserFutureToken* token, int index);

	bool SingleCharNeedsVerification(wchar_t ch, SingleCharToken* token);
	bool MultiCharNeedsVerification(ABParserFutureToken* token, int index);
	
	// FINALIZE
	int FinalizeToken(SingleCharToken* token);
	int FinalizeToken(ABParserFutureToken* token, int index);

	void PrepareLeadingAndTrailing();
	int QueueTokenAndReturnFinalizeResult(ABParserToken* token, bool hasQueuedToken);

	// HELPERS
	void AddFutureToken(MultiCharToken* token);
	void MarkFinishedFutureToken(int firstDimension, int secondDimension);
	void TrimFutureTokens();
	void DisableFutureToken(int firstDimension, int secondDimension);
	void ResetCurrentTokens(bool deleteCurrentTokens);
	void ConfigureCurrentTokens(int* validSingleCharTokens, int singleCharLength, int* validMultiCharTokens, int multiCharLength);
	
	void InitString(unsigned short* text, int textLength);
	void InitTokens(unsigned short** tokens, int* tokenLengths, int numberOfTokens);
	ABParserBase(unsigned short** tokens, int* tokenLengths, int numberOfTokens);
	~ABParserBase();

};