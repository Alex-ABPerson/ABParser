// ABSoftware.ABParser.Testing.CPPTesting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ABParser.h"
#include "ABParserConfig.h"
#include <string.h>
#include <vector>

class TestParser : public abparser::ABParser<wchar_t> {
public:
	TestParser(abparser::ABParserConfiguration<wchar_t>* config, abparser::ABParserToken<wchar_t>* tokens) : ABParser(config, tokens) {}

	void WriteTokenInformation(const abparser::TokenInformation<wchar_t>* info) {

		if (info == nullptr)
			std::wcout << "(NULL)";
		else
			std::wcout << "(Token: " << info->Token->Name->c_str() << ", Start: " << info->Start << ", Length: " << info->Length << ") ";
	}

	void BeforeTokenProcessed(const abparser::BeforeTokenProcessedArgs<wchar_t>& args) {
		std::wcout << "BeforeTokenProcessed PreviousToken: ";
		WriteTokenInformation(args.PreviousToken);

		std::wcout << "Token: ";
		WriteTokenInformation(args.Token);

		std::wcout << ", Leading: " << args.Leading << std::endl;
	}

	void OnTokenProcessed(const abparser::OnTokenProcessedArgs<wchar_t>& args) {
		std::wcout << "OnTokenProcessed PreviousToken: ";
		WriteTokenInformation(args.PreviousToken);

		std::wcout << "Token: ";
		WriteTokenInformation(args.Token);

		std::wcout << "NextToken: ";
		WriteTokenInformation(args.NextToken);

		std::wcout << "Leading: " << args.Leading << " Trailing: " << args.Trailing << std::endl;
	}
};

int main()
{
	std::cout << "Hello world!";

	abparser::ABParserToken<wchar_t> tokens[3];
	tokens[0].SetName("the")->SetData(L"the", 3);
	tokens[1].SetName("they")->SetData(L"they", 4);
	tokens[2].SetName("theyare")->SetData(L"theyare", 7);

	

	abparser::ABParserConfiguration<wchar_t> config(tokens, 3);

	TestParser parser(&config, tokens);
	parser.SetText(L"AtheBtheyCtheyarDtheyareE");
	parser.Start();

	return 0;
}