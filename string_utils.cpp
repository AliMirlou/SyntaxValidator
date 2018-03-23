#include "string_utils.h"

using namespace std;

// trim from start (in place)
static inline void ltrim(string &s) {
	s.erase(s.begin(), find_if(s.begin(), s.end(), [](int ch) {
		return !isspace(ch);
	}));
}

// trim from end (in place)
static inline void rtrim(string &s) {
	s.erase(find_if(s.rbegin(), s.rend(), [](int ch) {
		return !isspace(ch);
	}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(string &s) {
	ltrim(s);
	rtrim(s);
}

vector<string> split_and_trim(const string &s, char delimiter) {
	vector<string> tokens;
	string token;
	istringstream tokenStream(s);
	while (getline(tokenStream, token, delimiter)) {
		trim(token);
		tokens.push_back(token);
	}
	return tokens;
}
