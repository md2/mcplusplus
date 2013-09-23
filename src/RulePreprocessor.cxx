#include "common.h"

using namespace std;

Range::Range() {
	idx = 0;
	pos = NULL;
}

RulePreprocessor::RulePreprocessor() {
	it = rules.begin();
	state = 0;
	Init();
}

int RulePreprocessor::Init() {
	if (it == rules.end())
		return -1;
	const char *p = it->c_str();
	char *pp = CurrentRule;
	int c1, c2;
	int i;
	Charset cs;
	Range rng;
	int error = 0;
	while (*p != '\0' && !error) {
		switch (*p) {
			case '\\':
				if (*++p != '\0')
					*pp++ = *p++;
				else
					error = 1;
				break;
			case '[':
				c1 = c2 = '\0';
				p++;
				rng.pos = pp;
				while (*p != '\0' && *p != ']') {
					switch (*p) {
						case '\\':
							if ((c1 = (unsigned char)*++p) != '\0')
								cs.Add((char)c1);
							else
								error = 1;
							break;
						case '-':
							if (c1 == '\0')
								cs.Add(*p++);
							else {
								if ((c2 = (unsigned char)*++p) == '\\')
									c2 = (unsigned char)*++p;
								if (c2 == '\0') {
									error = 1;
									break;
								}
								if (c1 < c2) {
									for (i = c1; i <= c2; i++)
										cs.Add((char)i);
								} else {
									for (i = c1; i >= c2; i--)
										cs.Add((char)i);
								}
							}
							break;
						default:
							c1 = (unsigned char) *p++;
							cs.Add((char)c1);
							break;
					}
				}
				if (*p == '\0') {
					error = 1;
					break;
				}
				p++;
				rng.sequence = cs.getSequence();
				ranges.push_front(rng);
				cs.Clear();
				*pp++ = rng.sequence[0];
				break;
			default:
				*pp++ = *p++;
				break;
		}
	}
	*pp++ = '\0';
	len = pp - CurrentRule - 1;
	if (error)
		fprintf(stderr, _("Error parsing rule: %s\n"), it->c_str());
	return 0;
}

const char * RulePreprocessor::NextRule() {
	while (state != 0) {
		list<Range>::iterator i;
		list<Range>::iterator const end = ranges.end();
		for (i = ranges.begin(); i != end; i++) {
			++i->idx;
			if (i->idx < i->sequence.length()) {
				*i->pos = i->sequence[i->idx];
				break;
			}
			*i->pos = i->sequence[0];
			i->idx = 0;
		}
		if (i == end) {
			ranges.clear();
			it++;
			if (Init() == -1)
				return NULL;
		}
		return CurrentRule;
	}
	state = 1;
	return CurrentRule;
}

size_t RulePreprocessor::getLength() const {
	return len;
}

