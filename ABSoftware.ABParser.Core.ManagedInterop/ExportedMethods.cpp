#include "PlatformImplementation.h"
#include "ABParserBase.h"
#include "Debugging.h"
#include "TokenManagement.h"
#include <wchar.h>

using namespace std;

void ConvertIntegerToTwoShorts(int integer, unsigned short* data, int index) {
	data[index] = integer << 16;
	data[index + 1] = integer & 0xffff;
}

int ConvertStringToShorts(wchar_t* str, int strLen, unsigned short* data, int index) {

	// Write out the length.
	ConvertIntegerToTwoShorts(strLen, data, index);
	index += 2;

	// Then, write out the text, converting each "wchar_t" to an unsigned short.
	for (int i = 0; i < strLen; i++)
		data[index++] = str[i];

	// Finally, return what index we ended at.
	return index;
}

extern "C" {
	EXPORT void InitializeTokens(unsigned short** tokens, int* tokenLengths, int numberOfTokens, SingleCharToken** singleCharTokens, MultiCharToken** multiCharTokens, int* numberOfSingleCharTokens, int* numberOfMultiCharTokens) {
		SortTokens(tokens, tokenLengths, numberOfTokens, singleCharTokens, multiCharTokens, numberOfSingleCharTokens, numberOfMultiCharTokens);
	}

	EXPORT ABParserBase* CreateBaseParser(SingleCharToken* singleCharTokens, MultiCharToken* multiCharTokens, int singleCharTokensLength, int multiCharTokensLength) {
		return new ABParserBase(singleCharTokens, singleCharTokensLength, multiCharTokens, multiCharTokensLength);
	}

	EXPORT void DeleteBaseParser(ABParserBase* baseParser) {
		delete baseParser;
	}

	EXPORT int ContinueExecution(ABParserBase* parser, unsigned short* outData) {
		/*outData[0] = 215;
		outData[1] = 245;*/
		int result = parser->ContinueExecution();

		// SEE ABSOFTWARE DOCS:
		// Send all of the extra data that's associated with this event.
		switch (result) {
		case 1: // Stop
		case 2: // BeforeTokenProcessed
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

				int onTokenProcessedLeadingEnd = ConvertStringToShorts(result == 1 ? parser->OnTokenProcessedLeading : parser->OnTokenProcessedTrailing, result == 1 ? parser->OnTokenProcessedLeadingLength : parser->OnTokenProcessedTrailingLength, outData, 6);

				// If this was a "Stop" result, then this is the last OnTokenProcessed, so we need to include the trailing as well.
				if (result == 1)
					ConvertStringToShorts(parser->OnTokenProcessedTrailing, parser->OnTokenProcessedTrailingLength, outData, onTokenProcessedLeadingEnd);

			}
			break;

		case 3: // Before + OnTokenProcessed
			{
				outData[0] = parser->OnTokenProcessedToken->MixedIdx;
				ConvertIntegerToTwoShorts(parser->OnTokenProcessedTokenStart, outData, 1);
				
				if (parser->OnTokenProcessedPreviousToken != nullptr) {
					outData[3] = parser->OnTokenProcessedPreviousToken->MixedIdx;
					ConvertIntegerToTwoShorts(parser->OnTokenProcessedPreviousTokenStart, outData, 4);
				}
				outData[6] = parser->BeforeTokenProcessedToken->MixedIdx;
				ConvertIntegerToTwoShorts(parser->BeforeTokenProcessedTokenStart, outData, 7);
				int leadingEnd = ConvertStringToShorts(parser->OnTokenProcessedLeading, parser->OnTokenProcessedLeadingLength, outData, 9);
				ConvertStringToShorts(parser->OnTokenProcessedTrailing, parser->OnTokenProcessedTrailingLength, outData, leadingEnd);
			}
			break;

		}

		return result;
	}

	EXPORT void InitString(ABParserBase* parser, unsigned short* text, int textLength) {
		parser->InitString(text, textLength);
	}

	EXPORT void DisposeDataForNextParse(ABParserBase* parser) {
		parser->DisposeDataForNextParse();
	}
}