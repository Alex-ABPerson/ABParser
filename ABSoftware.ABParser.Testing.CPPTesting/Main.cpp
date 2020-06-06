// ABSoftware.ABParser.Testing.CPPTesting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ABParserBase.h"
#include "TokenManagement.h"
#include <vector>

int main()
{
	ABParserConfiguration<wchar_t>* config = CreateConfiguration(new ABParserToken<wchar_t>[1] {
		ABParserToken<wchar_t>(L"Token1", 6)
	}, 1);

	ABParserBase<wchar_t> parser(config);
	return 0;
}