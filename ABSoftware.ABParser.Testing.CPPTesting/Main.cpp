// ABSoftware.ABParser.Testing.CPPTesting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ABParserBase.h"

int main()
{
    SingleCharToken* stokens = new SingleCharToken[0];
    MultiCharToken* mtokens = new MultiCharToken[3];

    mtokens[0].TokenContents = (wchar_t*)L"the";
    mtokens[0].TokenLength = 3;
    mtokens[1].TokenContents = (wchar_t*)L"they";
    mtokens[1].TokenLength = 4;
    mtokens[2].TokenContents = (wchar_t*)L"theyare";
    mtokens[2].TokenLength = 7;
    
    ABParserBase* parser = new ABParserBase(stokens, 0, mtokens, 3);
    parser->InitString((unsigned short*)L"AtheBtheyCtheyarDtheyareE", 25);
    int result = 0;
    while (result != 1) {
        result = parser->ContinueExecution();
    }
    parser->DisposeDataForNextParse();
    delete parser;
}