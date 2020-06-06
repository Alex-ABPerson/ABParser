#include "PlatformImplementation.h"
#include "ABParserBase.h"

using namespace std;

void ConvertIntegerToTwoShorts(int integer, uint16_t* data, int index) {
	data[index] = integer << 16;
	data[index + 1] = integer & 0xffff;
}

int MoveStringToArray(uint16_t* str, int strLen, uint16_t* data, int index) {

	// Write out the length.
	ConvertIntegerToTwoShorts(strLen, data, index);
	index += 2;

	for (int i = 0; i < strLen; i++)
		data[index++] = str[i];

	// Finally, return what index we ended at.
	return index;
}

extern "C" {
	// Because we can't marshall three pointers for the "tokenLimitNames" (array of an array of limits) in, we need to push token limit names down into an array of strings.
	// Then, we have "numberOfTokenLimitsForToken", which represents how many limit names each token has. So, we can then convert that to "ABParserToken"s.
	EXPORT ABParserConfiguration<uint16_t>* InitializeTokens(uint16_t** tokens, size_t* tokenLengths, int numberOfTokens, uint16_t** tokenLimitNames, size_t* tokenLimitNameSizes, int* numberOfTokenLimitsForToken) {

		ABParserToken<uint16_t>* newTokens = new ABParserToken<uint16_t>[numberOfTokens];
		size_t currentLimitNamesPos = 0;
		for (int i = 0; i < numberOfTokens; i++) {
			newTokens[i].Init(tokens[i], tokenLengths[i]);
			
			int numberOfLimits = numberOfTokenLimitsForToken[i];
			if (numberOfTokenLimitsForToken[i]) {
				UnorganizedTokenLimit<uint16_t>* newLimits = new UnorganizedTokenLimit<uint16_t>[numberOfLimits];

				for (int j = 0; j < numberOfLimits; j++) {
					newLimits[j].Init(tokenLimitNames[currentLimitNamesPos], tokenLimitNameSizes[currentLimitNamesPos]);
					currentLimitNamesPos++;
				}

				newTokens[i].SetLimits(newLimits, numberOfLimits);
			}
		}

		ABParserConfiguration<uint16_t>* result = CreateTokens(newTokens, numberOfTokens);
		delete[] newTokens;
		return result;
	}

	EXPORT ABParserBase<uint16_t>* CreateBaseParser(ABParserConfiguration<uint16_t>* information) {
		return new ABParserBase<uint16_t>(information);
	}

	EXPORT void DeleteBaseParser(ABParserBase<uint16_t>* baseParser) {
		delete baseParser;
	}

	EXPORT void EnterTokenLimit(ABParserBase<uint16_t>* parser, uint16_t* limitName, size_t limitNameSize) {
		parser->EnterTokenLimit(limitName, limitNameSize);
	}

	EXPORT void ExitTokenLimit(ABParserBase<uint16_t>* parser, int levels) {
		for (int i = 0; i < levels; i++)
			parser->ExitTokenLimit();
	}

	EXPORT int ContinueExecution(ABParserBase<uint16_t>* parser, uint16_t* outData) {
		ABParserResult result = parser->ContinueExecution();

		// SEE ABSOFTWARE DOCS:
		// Send all of the extra data that's associated with this event.
		switch (result) {
		case ABParserResult::StopAndFinalOnTokenProcessed:
		case ABParserResult::BeforeTokenProcessed:
			{
				// The wrapper will know whether these events have been triggered, and will ignore these if so.
				if (parser->BeforeTokenProcessedToken != nullptr) {
					outData[0] = parser->BeforeTokenProcessedToken->MixedIdx;
					ConvertIntegerToTwoShorts(parser->BeforeTokenProcessedTokenStart, outData, 1);
				}

				if (parser->OnTokenProcessedToken != nullptr) {
					outData[3] = parser->OnTokenProcessedToken->MixedIdx;
					ConvertIntegerToTwoShorts(parser->OnTokenProcessedTokenStart, outData, 4);
				}

				int onTokenProcessedLeadingEnd = MoveStringToArray(result == ABParserResult::StopAndFinalOnTokenProcessed ? parser->OnTokenProcessedLeading : parser->OnTokenProcessedTrailing, 
					result == ABParserResult::StopAndFinalOnTokenProcessed ? parser->OnTokenProcessedLeadingLength : parser->OnTokenProcessedTrailingLength, outData, 6);

				// If this was a "Stop" result, then this is the last OnTokenProcessed, so we need to include the trailing as well.
				if (result == ABParserResult::StopAndFinalOnTokenProcessed)
					MoveStringToArray(parser->OnTokenProcessedTrailing, parser->OnTokenProcessedTrailingLength, outData, onTokenProcessedLeadingEnd);

			}
			break;

		case ABParserResult::OnAndBeforeTokenProcessed:
			{
				outData[0] = parser->OnTokenProcessedToken->MixedIdx;
				ConvertIntegerToTwoShorts(parser->OnTokenProcessedTokenStart, outData, 1);
				
				if (parser->OnTokenProcessedPreviousToken != nullptr) {
					outData[3] = parser->OnTokenProcessedPreviousToken->MixedIdx;
					ConvertIntegerToTwoShorts(parser->OnTokenProcessedPreviousTokenStart, outData, 4);
				}
				outData[6] = parser->BeforeTokenProcessedToken->MixedIdx;
				ConvertIntegerToTwoShorts(parser->BeforeTokenProcessedTokenStart, outData, 7);
				int leadingEnd = MoveStringToArray(parser->OnTokenProcessedLeading, parser->OnTokenProcessedLeadingLength, outData, 9);
				MoveStringToArray(parser->OnTokenProcessedTrailing, parser->OnTokenProcessedTrailingLength, outData, leadingEnd);
			}
			break;

		}

		return static_cast<int>(result);
	}

	EXPORT void InitString(ABParserBase<uint16_t>* parser, uint16_t* text, int textLength) {
		parser->InitString(text, textLength);
	}

	EXPORT void DisposeDataForNextParse(ABParserBase<uint16_t>* parser) {
		parser->DisposeDataForNextParse();
	}
}