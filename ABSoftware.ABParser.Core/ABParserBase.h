#pragma once
#include <vector>
using namespace std;

class ABParserBase {
private:

	int** futureTokens;
	int futureTokensHead;
	int futureTokensTail;
	int currentPosition;

	wchar_t* primaryBuildUp;
	wchar_t* secondaryBuildUp;

	bool isVerifying;
	vector<int> verifyTokens;
	vector<wchar_t*> verifyBuildUp;

public:
	int PublicPosition;

	wchar_t** Tokens;
	int TokensLength;

	// Whether the "CurrentTokens" array actually just points to the "Tokens" array.
	// We can use this to decide whether to "delete" CurrentTokens when configuring/resetting it.
	bool CurrentTokensIsTokens;
	wchar_t** CurrentTokens;
	int CurrentTokensLength;

	wchar_t* Text;
	int TextLength;

	wchar_t* Leading;
	int LeadingLength;

	wchar_t* Trailing;
	int TrailingLength;

	int ProcessChar(wchar_t ch);
	void UpdateCurrentFutureTokens(wchar_t ch);

	int ContinueExecution();

	void FinalizeToken(int tokenId);
	
	void InitString(wchar_t* text, int textLength);
	void ResetCurrentTokens();
	void ConfigureCurrentTokens(int* validTokens, int validTokensLength);
	ABParserBase(wchar_t** tokens, int tokensLength);
	~ABParserBase();

};