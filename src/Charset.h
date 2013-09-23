#ifndef CHARSET_H
#define CHARSET_H

#include <string>
#include <set>

class Charset {
		typedef std::set<char> charset_t;
		std::string sequence;
		charset_t charset;
	public:
		Charset();
		Charset(const char *);
		void Add(char);
		std::string getSequence() const;
		void Clear();
};

#endif

