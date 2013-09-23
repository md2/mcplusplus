#include <string.h>
#include "common.h"

#define INVALID_LENGTH  0x7f

#define CHARS_LOWER "abcdefghijklmnopqrstuvwxyz"
#define CHARS_UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHARS_DIGITS "0123456789"

#define RULE \
	(*rule++)

#define LAST \
	(*(rule-1))

#define NEXT \
	(*rule)

#define POSITION \
	{ \
		unsigned char c = RULE; \
		if (c == '-') { \
			c = RULE; \
			if ((pos = map_LENGTH[c]) == INVALID_LENGTH) { \
				if (LAST) \
					errno = RA_ERROR_POSITION; \
				else \
					errno = RA_ERROR_END; \
				return NULL; \
			} \
			pos = len - pos; \
		} else { \
			if ((pos = map_LENGTH[c]) == INVALID_LENGTH) { \
				if (LAST) \
					errno = RA_ERROR_POSITION; \
				else \
					errno = RA_ERROR_END; \
				return NULL; \
			} \
		} \
	}

#define VALUE \
	if (!(value = RULE)) { \
		errno = RA_ERROR_END; \
		return NULL; \
	}

#define MAP0(map, pos) \
	out[pos] = (map)[(unsigned char)in[pos]];

#define MAP(map) { \
		for (pos=0; (out[pos]=(map)[(unsigned char)in[pos]]); pos++); \
	}

#define CLASS(start, true, false) { \
		if ((value = RULE) == '?') { \
			if (!(_class = map_CLASS[(unsigned char)RULE])) { \
				if (LAST) \
					errno = RA_ERROR_CLASS; \
				else \
					errno = RA_ERROR_END; \
				return NULL; \
			} \
			for (pos=(start); pos<len; pos++) \
				if (_class[(unsigned char)in[pos]]) { \
					true; \
				} \
				else { \
					false; \
				} \
		} \
		else { \
			if (!value) { \
				errno = RA_ERROR_END; \
				return NULL; \
			} \
			for (pos=(start); pos<len; pos++) \
				if (in[pos] == value) { \
					true; \
				} \
				else { \
					false; \
				} \
		} \
	}

#define SKIP_CLASS { \
		VALUE \
		if (value == '?') \
			VALUE \
		}


static char map_TOLOWER[256], map_TOUPPER[256], map_INVERT[256], map_CLASS[256][256];
static char map_LENGTH[256];

void InitMap(const char *src, const char *dst, char *map) {
	for (int pos = 0; pos < 256; pos++)
		map[pos] = pos;
	while (*src)
		map[*src++] = *dst++;
}

void InitClassMap(int name, const char *valid) {
	const char *pos;
	memset(map_CLASS[name], 0, 0x100);
	for (pos = valid; *pos; pos++)
		map_CLASS[name][*pos] = 1;
	if (islower(name)) {
		int inv = toupper(name);
		memset(map_CLASS[inv], 1, 0x100);
		for (pos = valid; *pos; pos++)
			map_CLASS[inv][*pos] = '\0';
	}
}

void InitLengthMap() {
	int c;
	for (c = '0'; c <= '9'; c++)
		map_LENGTH[c] = c - '0';
	for (c = 'a'; c <= 'z'; c++)
		map_LENGTH[c] = c - 'a' + 10;
	for (c = 'A'; c <= 'Z'; c++)
		map_LENGTH[c] = c - 'A' + 10;
}

RuleApplicator::RuleApplicator() {
	errno = RA_NO_ERROR;
	InitMap(CHARS_UPPER, CHARS_LOWER, map_TOLOWER);
	InitMap(CHARS_LOWER, CHARS_UPPER, map_TOUPPER);
	InitMap(CHARS_LOWER CHARS_UPPER, CHARS_UPPER CHARS_LOWER, map_INVERT);
	InitClassMap('?', "?");
	InitClassMap('v', "aeiouAEIOU");
	InitClassMap('c', "bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ");
	InitClassMap('w', " \t");
	InitClassMap('p', ".,:;'\"?!`");
	InitClassMap('s', "$%^&*()-_+=|\\<>[]{}#@/~");
	InitClassMap('l', CHARS_LOWER);
	InitClassMap('u', CHARS_UPPER);
	InitClassMap('d', CHARS_DIGITS);
	InitClassMap('a', CHARS_LOWER CHARS_UPPER);
	InitClassMap('x', CHARS_LOWER CHARS_UPPER CHARS_DIGITS);
	memset(map_LENGTH, INVALID_LENGTH, 0x100);
	InitLengthMap();
}

const char *RuleApplicator::Apply(const char *rule, const char *word, size_t &len) {
	static char buffer[2][3*MAX_WORD_LENGTH+1];
	char *in, *out;
	char value;
	char *_class;
	char memo[MAX_WORD_LENGTH+1];
	int memory;
	int pos, pos0;
	int count, required;
	in = buffer[0];
	out = buffer[1];
	memory = 0;
	memcpy(in, word, len);
	in[len] = '\0';
	errno = RA_NO_ERROR;
	while (NEXT) {
		switch (RULE) {
			case ':':
			case ' ':
			case '\t':
				out = in;
				break;
			case '^':
				VALUE;
				out[0] = value;
				strcpy(&out[1], in);
				len++;
				break;
			case '$':
				VALUE;
				out = in;
				out[len++] = value;
				out[len] = '\0';
				break;
			case '[':
				if (len) {
					strcpy(out, &in[1]);
					len--;
				} else {
					out[0] = '\0';
				}
				break;
			case ']':
				out = in;
				if (len)
					out[--len] = '\0';
				break;
			case 'l':
				MAP(map_TOLOWER);
				break;
			case 'u':
				MAP(map_TOUPPER);
				break;
			case 'c':
				pos = 0;
				MAP0(map_TOUPPER, 0);
				while (++pos <= len)
					MAP0(map_TOLOWER, pos);
				break;
			case 'C':
				pos = 0;
				MAP0(map_TOLOWER, 0);
				while (++pos <= len)
					MAP0(map_TOUPPER, pos);
				break;
			case 'T':
				POSITION;
				out = in;
				if (pos < len)
					MAP0(map_INVERT, pos);
				break;
			case 't':
				MAP(map_INVERT);
				break;
			case 'r':
				*(out += len) = '\0';
				while (*in)
					*--out = *in++;
				break;
			case 'd':
				if (len > MAX_WORD_LENGTH)
					return NULL;
				strcpy(out, in);
				strcat(out, in);
				len *= 2;
				break;
			case 'f':
				if (len > MAX_WORD_LENGTH)
					return NULL;
				(out = in)[pos = len *= 2] = '\0';
				while (*in)
					out[--pos] = *in++;
				break;
			case '>':
				POSITION;
				if (len > pos)
					out = in;
				else
					return NULL;
				break;
			case '<':
				POSITION;
				if (len < pos)
					out = in;
				else
					return NULL;
				break;
			case '\'':
				POSITION;
				out = in;
				if (pos < len) {
					out[pos] = '\0';
					len = pos;
				}
				break;
			case '{':
				if (len) {
					strcpy(out, &in[1]);
					in[1] = '\0';
					strcat(out, in);
				} else {
					out[0] = '\0';
				}
				break;
			case '}':
				if (len) {
					out[0] = in[len-1];
					in[len-1] = '\0';
					strcpy(&out[1], in);
				} else {
					out[0] = '\0';
				}
				break;
			case 'D':
				POSITION;
				if (pos < len) {
					memcpy(out, in, pos);
					strcpy(&out[pos], &in[pos+1]);
					len--;
				} else {
					out = in;
				}
				break;
			case 'x':
				POSITION;
				if (pos < len) {
					len = len - pos;
					in += pos;
					POSITION;
					if (pos < len)
						len = pos;
					in[len] = '\0';
					strcpy(out, in);
				} else {
					POSITION;
					out[0] = '\0';
					len = 0;
				}
				break;
			case 'o':
				POSITION;
				VALUE;
				out = in;
				if (pos < len)
					out[pos] = value;
				break;
			case 'i':
				POSITION;
				VALUE;
				if (pos < len) {
					memcpy(out, in, pos);
					out[pos] = value;
					strcpy(&out[pos+1], &in[pos]);
					len++;
				} else {
					out = in;
					out[len++] = value;
					out[len] = '\0';
				}
				break;
			case 'M':
				strncpy(memo, (out = in), MAX_WORD_LENGTH);
				memo[MAX_WORD_LENGTH] = '\0';
				memory = 1;
				break;
			case 'Q':
				if (memory) {
					if (!strncmp(in, memo, MAX_WORD_LENGTH))
						return NULL;
				} else {
					if (!strncmp(in, word, MAX_WORD_LENGTH))
						return NULL;
				}
				out = in;
				break;
			case 's':
				out = in;
				CLASS(0, out[pos] = NEXT, {});
				VALUE;
				break;
			case '@':
				pos0 = 0;
				CLASS(0, {}, out[pos0++] = in[pos]);
				out[len=pos0] = '\0';
				break;
			case '!':
				CLASS(0, return NULL, {});
				out = in;
				break;
			case '/':
				CLASS(0, break, {});
				if (pos >= len)
					return NULL;
				out = in;
				break;
			case '=':
				POSITION;
				if (pos < len) {
					CLASS(pos, break, return NULL);
				} else {
					SKIP_CLASS;
					return NULL;
				}
				out = in;
				break;
			case '(':
				CLASS(0, break, return NULL);
				out = in;
				break;
			case ')':
				if (len) {
					CLASS(len - 1, break, return NULL);
				} else {
					SKIP_CLASS;
					return NULL;
				}
				out = in;
				break;
			case '%':
				POSITION;
				count = 0;
				required = pos;
				CLASS(0, if (++count >= required) break, {});
				if (count < required) return NULL;
				out = in;
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				count = 0;
				required = LAST - '0';
				CLASS(0, if (++count > required) break, {});
				if (count != required) return NULL;
				out = in;
				break;
			case 'w':
				POSITION;
				pos0 = pos;
				POSITION;
				if (pos < len && pos0 < len) {
					out = in;
					if (pos != pos0)
						out[pos] ^= out[pos0] ^= out[pos] ^= out[pos0];
				} else {
					return NULL;
				}
				break;
			case 'm':
				if (len > MAX_WORD_LENGTH)
					return NULL;
				(out = in)[pos=len=2*len-1] = '\0';
				while (*in)
					out[--pos] = *in++;
				break;
			case 'I':
				if (len) {
					POSITION;
					if (pos >= len)
						pos = len - 1;
					value = in[pos];
					POSITION;
					if (pos < len) {
						memcpy(out, in, pos);
						out[pos] = value;
						strcpy(&out[pos+1], &in[pos]);
						len++;
					} else {
						out = in;
						out[len++] = value;
						out[len] = '\0';
					}
				} else {
					return NULL;
				}
				break;
			case 'O':
				if (len) {
					POSITION;
					if (pos >= len)
						pos = len - 1;
					value = in[pos];
					POSITION;
					out = in;
					if (pos < len)
						out[pos] = value;
				} else {
					return NULL;
				}
				break;
			case '+':
				out = in;
				POSITION;
				if (pos < len)
					out[pos]++;
				break;
			case '-':
				out = in;
				POSITION;
				if (pos < len)
					out[pos]--;
				break;
			default:
				errno = RA_ERROR_COMMAND;
				return NULL;
				break;
		}
		if (len == 0)
			return NULL;
		if (len > MAX_WORD_LENGTH) {
			len = MAX_WORD_LENGTH;
			out[len] = '\0';
		}
		if ((in = out) == buffer[1])
			out = buffer[0];
		else
			out = buffer[1];
	}
	return in;
}

