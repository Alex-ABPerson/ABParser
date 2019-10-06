#pragma once
#include <vector>
#include "ABParserFutureToken.h"
#include "VerifyToken.h"
#include "SingleCharToken.h"
#include "MultiCharToken.h"
using namespace std;

class ABParserBase {
private:

	// NOTE: futureTokens can only be multiple characters.
	ABParserFutureToken*** futureTokens;
	int* futureTokenLengths;
	int futureTokensHead;
	int futureTokensTail;
	int currentPosition;

	bool isVerifying;
	vector<VerifyToken> verifyTokens;
	vector<VerifyToken> verifyTriggers;
	vector<wchar_t*> verifyBuildUp;

	ABParserToken queuedToken;

	wchar_t* primaryBuildUp;
	int primaryBuildUpLength;
	wchar_t* secondaryBuildUp;
	int secondaryBuildUpLength;

	bool usingPrimaryBuildUp;

	// Whether the "CurrentTokens" array actually just points to the "Tokens" array.
	// We can use this to decide whether to "delete" CurrentTokens when configuring/resetting it.
	bool singleCharCurrentTokensIsTokens;
	bool multiCharCurrentTokensIsTokens;

	SingleCharToken* singleCharCurrentTokens;
	int singleCharCurrentTokensLength;

	MultiCharToken* multiCharCurrentTokens;
	int multiCharCurrentTokensLength;


public:
	int PublicPosition;
	ABParserToken OnTokenProcessed;

	SingleCharToken* SingleCharTokens;
	int NumberOfSingleCharTokens;

	MultiCharToken* MultiCharTokens;
	int NumberOfMultiCharTokens;

	wchar_t* Text;
	int TextLength;

	wchar_t* Leading;
	int LeadingLength;

	wchar_t* Trailing;
	int TrailingLength;

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
	int FinalizeToken(int tokenIdx);
	int FinalizeToken(ABParserFutureToken* token, int index);
	int HandleQueuedToken();

	void PrepareLeading();

	// HELPERS
	void AddFutureToken(MultiCharToken* token);
	void MarkFinishedFutureToken(int firstDimension, int secondDimension);
	void TrimFutureTokens();
	void DisableFutureToken(int firstDimension, int secondDimension);
	void ResetCurrentTokens();
	void ConfigureCurrentTokens(int* validSingleCharTokens, int singleCharLength, int* validMultiCharTokens, int multiCharLength);
	
	void InitString(wchar_t* text, int textLength);
	void InitTokens(wchar_t** tokens, int* tokenLengths, int numberOfTokens);
	ABParserBase(wchar_t** tokens, int* tokenLengths, int numberOfTokens);
	~ABParserBase();

};