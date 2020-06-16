#include "ABParserBase.h"

using namespace abparser;

#define COMPILE_DLL

#ifdef COMPILE_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

void Convert32BitTo16Bit(uint32_t bit32, uint16_t* out, uint32_t index) {
	out[index] = bit32 << 16;
	out[index + 1] = bit32 & 0xffff;
}

uint32_t MoveStringToArray(uint16_t* str, uint32_t strLen, uint16_t* data, uint32_t index) {

	// Write out the length.
	Convert32BitTo16Bit(strLen, data, index);
	index += 2;

	for (uint32_t i = 0; i < strLen; i++)
		data[index++] = str[i];

	// Finally, return what index we ended at.
	return index;
}

class ConfigAndTokens {
public:
	ABParserConfiguration<uint16_t, uint16_t> Config;
	ABParserToken<uint16_t, uint16_t>* Tokens;

	ConfigAndTokens(ABParserToken<uint16_t, uint16_t>* tokens, uint16_t numberOfTokens) {
		Tokens = tokens;
		Config.Init(tokens, numberOfTokens);
	}
	
	~ConfigAndTokens() {
		delete[] Tokens;
	}
};

extern "C" {
	// Because we can't marshall three pointers for the "tokenLimitNames" (array of an array of limits) in, we need to push token limit names down into an array of strings.
	// Then, we have "numberOfTokenLimitsForToken", which represents how many limit names each token has. So, we can then convert that to "ABParserToken"s.
	EXPORT ConfigAndTokens* InitializeConfiguration(uint16_t** tokens, uint16_t* tokenLengths, uint16_t numberOfTokens, uint16_t** tokenLimitNames, uint8_t* tokenLimitNameSizes, uint16_t* numberOfTokenLimitsForToken, uint16_t** tokenDetectionLimits, uint16_t* tokenDetectionLimitSizes) {

		ABParserToken<uint16_t, uint16_t>* newTokens = new ABParserToken<uint16_t, uint16_t>[numberOfTokens];

		uint16_t currentLimitNamesPos = 0;
		for (uint16_t i = 0; i < numberOfTokens; i++) {
			newTokens[i].SetData(tokens[i], tokenLengths[i]);
			newTokens[i].DirectSetDetectionLimit(tokenDetectionLimits[i], tokenDetectionLimitSizes[i]);
			
			uint16_t numberOfLimits = numberOfTokenLimitsForToken[i];
			if (numberOfLimits) {
				UnorganizedTokenLimit<uint16_t>* newLimits = new UnorganizedTokenLimit<uint16_t>[numberOfLimits];

				for (uint16_t j = 0; j < numberOfLimits; j++) {
					newLimits[j].Init(tokenLimitNames[currentLimitNamesPos], tokenLimitNameSizes[currentLimitNamesPos]);
					currentLimitNamesPos++;
				}

				newTokens[i].DirectSetTokenLimits(newLimits, numberOfLimits);
			}
		}

		ConfigAndTokens* result = new ConfigAndTokens(newTokens, numberOfTokens);
		return result;
	}

	EXPORT void ConfigSetTriviaLimits(ConfigAndTokens* information, uint16_t** limitNames, uint8_t* limitNameLengths, uint16_t** limitContents, uint16_t* limitContentLengths, uint16_t numberOfLimits) {
		TriviaLimit<uint16_t, uint16_t>* limits = new TriviaLimit<uint16_t, uint16_t>[numberOfLimits];

		for (uint16_t i = 0; i < numberOfLimits; i++) {
			limits[i].SetName(limitNames[i]);
			limits[i].DirectSetIgnoreCharacters(limitContents[i], limitContentLengths[i]);
		}
			
		information->Config.SetTriviaLimits(numberOfLimits, limits);
	}

	EXPORT ABParserBase<uint16_t, uint16_t>* CreateBaseParser(ConfigAndTokens* information) {
		return new ABParserBase<uint16_t, uint16_t>(&information->Config);
	}

	EXPORT void DeleteBaseParser(ABParserBase<uint16_t, uint16_t>* parser) {
		delete parser;
	}

	EXPORT void DeleteConfiguration(ConfigAndTokens* parser) {
		delete parser;
	}

	EXPORT void EnterTokenLimit(ABParserBase<uint16_t, uint16_t>* parser, uint16_t* limitName, uint8_t limitNameLength) {
		parser->EnterTokenLimit(limitName, limitNameLength);
	}

	EXPORT void ExitTokenLimit(ABParserBase<uint16_t, uint16_t>* parser, int levels) {
		for (int i = 0; i < levels; i++)
			parser->ExitTokenLimit();
	}

	EXPORT void EnterTriviaLimit(ABParserBase<uint16_t, uint16_t>* parser, uint16_t* limitName, uint8_t limitNameLength) {
		parser->EnterTriviaLimit(limitName, limitNameLength);
	}

	EXPORT void ExitTriviaLimit(ABParserBase<uint16_t, uint16_t>* parser, int levels) {
		for (int i = 0; i < levels; i++)
			parser->ExitTriviaLimit();
	}

	EXPORT ABParserResult ContinueExecution(ABParserBase<uint16_t, uint16_t>* parser, uint16_t* outData) {
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
					Convert32BitTo16Bit(parser->BeforeTokenProcessedTokenStart, outData, 1);
				}

				if (parser->OnTokenProcessedToken != nullptr) {
					outData[3] = parser->OnTokenProcessedToken->MixedIdx;
					Convert32BitTo16Bit(parser->OnTokenProcessedTokenStart, outData, 4);
				}

				uint32_t onTokenProcessedLeadingEnd = MoveStringToArray(result == ABParserResult::StopAndFinalOnTokenProcessed ? parser->OnTokenProcessedLeading : parser->OnTokenProcessedTrailing,
					result == ABParserResult::StopAndFinalOnTokenProcessed ? parser->OnTokenProcessedLeadingLength : parser->OnTokenProcessedTrailingLength, outData, 6);

				// If this was a "Stop" result, then this is the last OnTokenProcessed, so we need to include the trailing as well.
				if (result == ABParserResult::StopAndFinalOnTokenProcessed)
					MoveStringToArray(parser->OnTokenProcessedTrailing, parser->OnTokenProcessedTrailingLength, outData, onTokenProcessedLeadingEnd);

			}
			break;

		case ABParserResult::OnThenBeforeTokenProcessed:
			{
				outData[0] = parser->OnTokenProcessedToken->MixedIdx;
				Convert32BitTo16Bit(parser->OnTokenProcessedTokenStart, outData, 1);
				
				if (parser->OnTokenProcessedPreviousToken != nullptr) {
					outData[3] = parser->OnTokenProcessedPreviousToken->MixedIdx;
					Convert32BitTo16Bit(parser->OnTokenProcessedPreviousTokenStart, outData, 4);
				}
				outData[6] = parser->BeforeTokenProcessedToken->MixedIdx;
				Convert32BitTo16Bit(parser->BeforeTokenProcessedTokenStart, outData, 7);
				uint32_t leadingEnd = MoveStringToArray(parser->OnTokenProcessedLeading, parser->OnTokenProcessedLeadingLength, outData, 9);
				MoveStringToArray(parser->OnTokenProcessedTrailing, parser->OnTokenProcessedTrailingLength, outData, leadingEnd);
			}
			break;

		}

		return result;
	}

	EXPORT void InitString(ABParserBase<uint16_t, uint16_t>* parser, uint16_t* text, int textLength) {
		parser->InitString(text, textLength);
	}

	EXPORT void DisposeDataForNextParse(ABParserBase<uint16_t>* parser) {
		parser->DisposeDataForNextParse();
	}
}