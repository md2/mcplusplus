#ifndef RULEPREPROCESSOR_H
#define RULEPREPROCESSOR_H

#include <list>
#include "stringlist.h"

class Range {
	public:
		int idx;
		char * pos;
		std::string sequence;
		Range();
};

class RulePreprocessor {
	public:
		static const int MAX_RULE_LENGTH = 64;
	private:
		std::list<Range> ranges;
		stringlistiterator it;
		char CurrentRule[MAX_RULE_LENGTH+1];
		size_t len;
		int state;
	public:
		RulePreprocessor();
		int Init();
		const char *NextRule();
		size_t getLength() const;
};

#endif

