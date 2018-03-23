#include "scanner.h"

#include <algorithm>
#include <fstream>

#define number_of_keywords 8
#define number_of_types 7
#define number_of_special_tokens 7

#define is_potential_keyword_or_identifier_or_type(c) ((c) >= 'a' && (c) <= 'z')
//#define is_potential_identifier_start(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
//#define is_potential_identifier_char(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')

using namespace std;

string keywords[] = {"if", "else", "for", "do", "while", "or", "and", "def"}; // update number_of_keywords if changed
string types[] = {"int", "long", "char", "float", "double", "bool", "void"}; // update number_of_types if changed
char special_tokens[] = {'(', ')', ',', ';', ':', '{', '}'}; // update number_of_special_token if changed

string code, line;
long long unsigned length, index = 0, line_index = 1, index_in_line = 0;

char c;
string token;

static void getch() {
	token += c;
	line += c;
	++index;
	++index_in_line;
	c = code[index];
}

long long unsigned get_line_index() {
	return line_index;
}

long long unsigned get_index_in_line() {
	return index_in_line;
}

string get_line() {
	while (c != '\n') getch();
	string temp = token;
	token = "";
	getch();
	return temp;
}

string get_token(bool *flag) {
	token = "";
	if (index == length) return "$";
	if (find(special_tokens, special_tokens + number_of_special_tokens, c) !=
	    (special_tokens + number_of_special_tokens)) {
		getch();
		return token;
	}
	switch (c) {
		// Whitespace
		case '\n':
			++line_index;
			index_in_line = 0;
		case ' ':
		case '\t':
			token = "";
			getch();
			return get_token(flag);
			// Comment
		case '/':
			getch();
			switch (c) {
				case '/':
					get_line();
					return get_token(flag);
				case '*':
					while (c != '/') {
						if (index == length) {
							*flag = true;
							return token;
						}
						while (c != '*') {
							if (index == length) {
								*flag = true;
								return token;
							}
							getch();
						}
						getch();
					}
					token = "";
					getch();
					return get_token(flag);
			}
			getch();
			return token;
			// Relational Operators
		case '=':
			getch();
			switch (c) {
				case '<':
				case '>':
				case '=':
					getch();
					return token;
			}
			return token;
		case '<':
		case '>':
			getch();
			if (c == '=') getch();
			return token;
		case '!':
			getch();
			if (c == '=') {
				getch();
				return token;
			}
			// Operators
		case '+':
			getch();
			if (c == '+') getch();
			return token;
		case '-':
			getch();
			if (c == '-') getch();
			return token;
		case '*':
		case '^':
		case '%':
		case '\'':
			getch();
			return token;
	}
	if (is_potential_keyword_or_identifier_or_type(c)) {
		while (is_potential_keyword_or_identifier_or_type(c)) getch();
		if ((find(keywords, keywords + number_of_keywords, token) != keywords + number_of_keywords) ||
		    (find(types, types + number_of_types, token) != types + number_of_types))
			return token;
		return "id";
	}
	while (c >= '0' && c <= '9') getch();
	if (c == '.') {
		getch();
		while (c >= '0' && c <= '9') getch();
		return "double_number";
	} else return "int_number";
}

void fill_code(string filename) {
	code = string(istreambuf_iterator<char>(ifstream(filename) >> skipws), istreambuf_iterator<char>());
	length = code.size();
	c = code[0];
}
