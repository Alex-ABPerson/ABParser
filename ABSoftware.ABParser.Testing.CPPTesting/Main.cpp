// ABSoftware.ABParser.Testing.CPPTesting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ABParserBase.h"
#include "TokenManagement.h"

int main()
{
    TokensInformation<wchar_t>* tokensInformation = CreateTokens<wchar_t>(
        new wchar_t*[3] {
            (wchar_t*)L"the",
            (wchar_t*)L"they",
            (wchar_t*)L"theyare"
        }, 
        new int[3]{ 3, 3, 4 }, 3);

    ABParserBase<wchar_t>* parser = new ABParserBase<wchar_t>(tokensInformation);
    parser->InitString((wchar_t*)L"Test", 4);
   
}