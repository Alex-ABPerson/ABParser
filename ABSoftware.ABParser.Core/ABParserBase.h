#pragma once
#include <vector>
#include "ABParserFutureToken.h"
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
	vector<int> verifyTokens;
	vector<wchar_t*> verifyBuildUp;

public:
	int PublicPosition;

	wchar_t* SingleCharTokens;
	int NumberOfSingleCharTokens;

	wchar_t** MultiCharTokens;
	int* MultiCharTokenLengths;
	int NumberOfMultiCharTokens;

	wchar_t* PrimaryBuildUp;
	int PrimaryBuildUpLength;
	wchar_t* SecondaryBuildUp;
	int SecondaryBuildUpLength;

	int QueuedItem;

	// Whether the "CurrentTokens" array actually just points to the "Tokens" array.
	// We can use this to decide whether to "delete" CurrentTokens when configuring/resetting it.
	bool SingleCharCurrentTokensIsTokens;
	bool MultiCharCurrentTokensIsTokens;

	wchar_t* SingleCharCurrentTokens;
	int SingleCharCurrentTokensLength;

	wchar_t** MultiCharCurrentTokens;
	int MultiCharCurrentTokensLength;

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
	void StartVerify(ABParserFutureToken* token, int index);
	bool CheckIfTokenNeedsVerification(ABParserFutureToken* token, int index);
	
	// FINALIZE
	int StartFinalize(ABParserFutureToken* token, int index);
	void FinalizeSingleCharToken(int tokenIdx);

	// Helpers
	void AddFutureToken(int token);
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