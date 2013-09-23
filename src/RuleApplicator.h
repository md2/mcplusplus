#ifndef RULEAPPLICATOR_H
#define RULEAPPLICATOR_H

class RuleApplicator {
		static const int MAX_WORD_LENGTH = 32;
		int errno;
	public:
		enum {RA_NO_ERROR = 0, RA_ERROR_END, RA_ERROR_COMMAND, RA_ERROR_POSITION, RA_ERROR_CLASS};
		RuleApplicator();
		const char *Apply(const char *, const char *, size_t&);
};

#endif

