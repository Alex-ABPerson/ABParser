#include "PlatformImplementation.h"
#include "ABParserBase.h"
#include "TokenManagement.h"

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
	EXPORT TokensInformation<uint16_t>* InitializeTokens(uint16_t** tokens, int* tokenLengths, int numberOfTokens) {
		return CreateTokens(tokens, tokenLengths, numberOfTokens);
	}

	EXPORT ABParserBase<uint16_t>* CreateBaseParser(TokensInformation<uint16_t>* information) {
		return new ABParserBase<uint16_t>(information);
	}

	EXPORT void DeleteBaseParser(ABParserBase<uint16_t>* baseParser) {
		delete baseParser;
	}

	EXPORT int ContinueExecution(ABParserBase<uint16_t>* parser, uint16_t* outData) {
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

				int onTokenProcessedLeadingEnd = MoveStringToArray(result == 1 ? parser->OnTokenProcessedLeading : parser->OnTokenProcessedTrailing, result == 1 ? parser->OnTokenProcessedLeadingLength : parser->OnTokenProcessedTrailingLength, outData, 6);

				// If this was a "Stop" result, then this is the last OnTokenProcessed, so we need to include the trailing as well.
				if (result == 1)
					MoveStringToArray(parser->OnTokenProcessedTrailing, parser->OnTokenProcessedTrailingLength, outData, onTokenProcessedLeadingEnd);

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
				int leadingEnd = MoveStringToArray(parser->OnTokenProcessedLeading, parser->OnTokenProcessedLeadingLength, outData, 9);
				MoveStringToArray(parser->OnTokenProcessedTrailing, parser->OnTokenProcessedTrailingLength, outData, leadingEnd);
			}
			break;

		}

		return result;
	}

	EXPORT void InitString(ABParserBase<uint16_t>* parser, uint16_t* text, int textLength) {
		parser->InitString(text, textLength);
	}

	EXPORT void DisposeDataForNextParse(ABParserBase<uint16_t>* parser) {
		parser->DisposeDataForNextParse();
	}
}