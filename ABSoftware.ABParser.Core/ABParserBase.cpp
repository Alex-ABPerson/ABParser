#include "PlatformImplementation.h"
#include "ABParserBase.h"
#include <wchar.h>

using namespace std;

void ABParserBase::InitString(wchar_t* text, int textLength) {
	Text = new wchar_t[textLength + 1];
	TextLength = textLength;
	futureTokens = new int*[TextLength];
	Leading = new wchar_t[textLength + 1];
	Trailing = new wchar_t[textLength + 1];
	primaryBuildUp = new wchar_t[TextLength + 1];
	secondaryBuildUp = new wchar_t[TextLength + 1];
	wcsncpy(Text, text, textLength + 1);
}

// RESULTS:
// 0 - None
// 1 - Stop
// 2 - BeforeTokenProcessed
// 3 - OnTokenProcessed
int ABParserBase::ContinueExecution() {

	// Continue going through characters.
	for (; currentPosition < TextLength; currentPosition++) {
		int res = ProcessChar(Text[currentPosition]);

		// If we got a result back from the "ProcessChar", return that, otherwise just keep going.
		if (res != 0)
			return res;
	}

	// If we got here, then we reached the end of the string, so, we'll return a "1" to stop.
	return 1;
}

// ======================
// COLLECT/VERIFY
// ======================

int ABParserBase::ProcessChar(wchar_t ch) {

	// First, we'll update our current futureTokens.
	UpdateCurrentFutureTokens(ch);

}

void ABParserBase::UpdateCurrentFutureTokens(wchar_t ch) {

	// Go through all of the future tokens we have so far.
	for (int i = futureTokensHead; i < futureTokensTail; i++) {

		// Compare this futureToken to our current tokens, if it doesn't match any of them.

	}


}

void ABParserBase::ResetCurrentTokens() {
	// If the "CurrentTokens" variable is already pointing to "Tokens", then we won't need to bother setting it again.
	if (CurrentTokensIsTokens)
		return;

	delete CurrentTokens;
	CurrentTokensIsTokens = true;
	CurrentTokens = Tokens;
	CurrentTokensLength = TokensLength;
}

void ABParserBase::ConfigureCurrentTokens(int* validTokens, int validTokensLength) {

	// If the "CurrentTokens" variable isn't pointing to "Tokens", then we can safely delete it, otherwise we'll leave it since we don't want to delete "Tokens".
	if (!CurrentTokensIsTokens)
		delete CurrentTokens;

	CurrentTokensIsTokens = false;
	CurrentTokens = new wchar_t*[validTokensLength];
	CurrentTokensLength = validTokensLength;

	// Go through all of the tokens, and if they aren't in the "validTokens" array, then don't bother.
	for (int i = 0; i < TokensLength; i++)
	{
		for (int j = 0; j < validTokensLength; j++)
			if (i == validTokens[j]) {
				CurrentTokens[i] = Tokens[validTokens[j]];
				break;
			}
	}

};

ABParserBase::ABParserBase(wchar_t** tokens, int tokensLength)
{
	// Add the tokens.
	Tokens = new wchar_t*[tokensLength];
	TokensLength = tokensLength;
	for (int i = 0; i < tokensLength; i++)
		wcsncpy(tokens[i], Tokens[i], tokensLength + 1);

	// Configure the current tokens.
	ResetCurrentTokens();

	// Default values:
	Text = NULL;
	TextLength = 0;
	Leading = NULL;
	LeadingLength = 0;
	Trailing = NULL;
	TrailingLength = 0;
	currentPosition = 0;
	PublicPosition = 0;
	isVerifying = false;
	futureTokens = NULL;
	futureTokensHead = 0;
	futureTokensTail = 0;
	primaryBuildUp = NULL;
	secondaryBuildUp = NULL;
	verifyTokens.reserve(4);
	//verifyTokens = new vector<int>();
}

ABParserBase::~ABParserBase() {
	delete Text;
	delete Leading;
	delete Trailing;
	delete futureTokens;
	delete primaryBuildUp;
	delete secondaryBuildUp;
}