#pragma once
#include "HelperClasses.h"

// Sorts tokens into their "SingleCharTokens" and "MultiCharTokens" give back a pointer to them, as well as the total number of each.
void SortTokens(unsigned short** tokens, int* tokenLengths, int numberOfTokens, SingleCharToken** outSingleCharTokens, MultiCharToken** outMultiCharTokens, int* numberOfSingleCharTokens, int* numberOfMultiCharTokens);