#include "string_utils.h"

#include <set>
#include <windows.h>
#include <vector>
#include <unordered_map>
#include <map>

#ifndef GRAMMAR_UTILS_H_INCLUDED
#define GRAMMAR_UTILS_H_INCLUDED

using namespace std;

extern HANDLE hConsole;

extern unordered_map<string, vector<string>> firsts;
extern unordered_map<string, set<string>> follows;

struct Production {
	unsigned index;
	string left;
	vector<string> right;
	set<string> predicts;

	Production(unsigned i, string l, vector<string> r) : index{i}, left{l}, right{r}, predicts{} {}

	void print(const string &highlight = "") {
		printf("%d. %s ->", index, left.c_str());
		for (const auto &symbol: right) {
			if (symbol == highlight)
				SetConsoleTextAttribute(hConsole, 6);
			printf(" %s", symbol.c_str());
			SetConsoleTextAttribute(hConsole, 7);
		}
		printf("\n");
	}

	int find_symbol(const string &symbol) {
		unsigned i = distance(right.begin(), find(right.begin(), right.end(), symbol));
		if (i < right.size())
			return (int) i;
		return -1;
	}
};

extern map<string, vector<Production *>> NT_to_P;

vector<Production *> find_symbol(string symbol);

#endif // GRAMMAR_UTILS_H_INCLUDED
