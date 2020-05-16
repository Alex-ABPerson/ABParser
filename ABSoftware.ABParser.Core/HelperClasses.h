#ifndef _INCLUDE_HELPER_CLASSES_H
#define _INCLUDE_HELPER_CLASSES_H
class ABParserToken {
public:
	// When we created an instance of ABParser, the single-char tokens and multi-char tokens were mixed together, this is at what index this token would've been mixed in.
	unsigned short MixedIdx = 0;
	virtual int GetLength();
};

class SingleCharToken : public ABParserToken {
public:
	wchar_t TokenChar = 0;

	int GetLength();
};

class MultiCharToken : public ABParserToken {
public:
	wchar_t* TokenContents = 0;
	int TokenLength = 0;

	int GetLength();
};

class ABParserFutureToken {
public:
	MultiCharToken* Token;
	bool Finished;
	bool Disabled;

	ABParserFutureToken(MultiCharToken* token);
};

class ABParserVerifyToken {
public:
	bool IsSingleChar;
	SingleCharToken* SingleChar;
	ABParserFutureToken* MultiChar;

	ABParserFutureToken** Triggers;
	int TriggersLength;

	wchar_t* TrailingBuildUp;
	int TrailingBuildUpLength;

	int* TriggerStarts;
	int TriggerStartsLength;

	int Start;
	bool HasNoTriggers;

	ABParserVerifyToken(SingleCharToken* singleChar, int start, wchar_t* trailingBuildUp);

	ABParserVerifyToken(ABParserFutureToken* multiChar, int start, wchar_t* trailingBuildUp);

	~ABParserVerifyToken();

};
#endif