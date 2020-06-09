// ABSoftware.ABParser.Testing.CPPTesting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ABParser.h"
#include "TokenManagement.h"
#include <vector>

class TestParser : public ABParser<wchar_t> {
public:
	TestParser(ABParserConfiguration<wchar_t>* config, ABParserToken<wchar_t>* tokens) : ABParser(config, tokens) {}

	void BeforeTokenProcessed(const BeforeTokenProcessedArgs<wchar_t>& args) {

	}

	void OnTokenProcessed(const OnTokenProcessedArgs<wchar_t>& args) {

	}
};

int main()
{
	ABParserToken<wchar_t> tokens[3];
	tokens[0].SetName("the")->SetData(L"the", 3);
	tokens[1].SetName("they")->SetData(L"they", 4);
	tokens[2].SetName("theyare")->SetData(L"theyare", 7);

	ABParserConfiguration<wchar_t> config(tokens, 3);

	TestParser parser(&config, tokens);
	parser.SetText(L"AtheBtheyCtheyarDtheyareE");
	parser.Execute();

	return 0;
}