#include <ctype.h>
#include <assert.h>
#include "Charset.h"

using namespace std;

Charset::Charset() {
}

Charset::Charset(const char *s) {
	assert(s != NULL);
	int c1 = 0, c2;
	while (*s != '\0') {
		switch(*s) {
			case '-':
				s++;
				if (c1 == 0 || (c2 = *s) == 0)
					Add('-');
				else if (c1 < c2) {
					for (int i = c1; i <= c2; i++)
						Add(i);
					s++;
					c1 = 0;
				} else {
					for (int i = c1; i >= c2; i--)
						Add(i);
					s++;
					c1 = 0;
				}
				break;
			default:
				c1 = *s++;
				Add(c1);
				break;
		}
	}
}

void Charset::Add(char c) {
	if (charset.find(c) == charset.end()) {
		charset.insert(c);
		sequence += c;
	}
}

void Charset::Clear() {
	charset.clear();
	sequence.clear();
}

string Charset::getSequence() const {
	return sequence;
}

