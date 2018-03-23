#include "grammar_utils.h"

using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
unordered_map<string, vector<string>> firsts;
unordered_map<string, set<string>> follows;
map<string, vector<Production *>> NT_to_P;

vector<Production *> find_symbol(string symbol) {
	vector<Production *> result;
	for (auto NT: NT_to_P)
		for (auto production: NT.second)
			if (find(production->right.begin(), production->right.end(), symbol) != production->right.end())
				result.push_back(production);
	return result;
}

static vector<string> first(string symbol) {
	if (!(symbol[0] >= 'A' && symbol[0] <= 'Z')) return vector<string>(1, symbol);
	else if (!firsts[symbol].empty()) return firsts[symbol];
	unsigned i;
	bool add_lambda = false;
	vector<string> f;
	for (auto production: NT_to_P[symbol]) {
		i = 0;
		for (auto s: production->right) {
			vector<string> ftemp = first(s);
			f.insert(f.end(), ftemp.begin(), remove(ftemp.begin(), ftemp.end(), "lambda"));
			if (find(ftemp.begin(), ftemp.end(), "lambda") == ftemp.end()) break;
			++i;
		}
		if (i == production->right.size()) add_lambda = true;
	}
	if (add_lambda) f.push_back("lambda");
	firsts[symbol] = f;
	return f;
}

static void fill_firsts() {
	for (auto NT: NT_to_P) {
		vector<string> temp = first(NT.first);
		printf("First(%s) = {", NT.first.c_str());
		for (auto t: temp)
			printf("%s, ", t.c_str());
		printf("}\n");
	}
}

static set<string> follow(string non_terminal, string target) {
	if (!follows[non_terminal].empty()) return follows[non_terminal];
	set<string> f;
	vector<Production *> productions = find_symbol(non_terminal);
	bool add_follow;
	for (auto a: productions) {
		add_follow = true;
		int i = a->find_symbol(non_terminal) + 1;
		for (unsigned j = i; add_follow && j < a->right.size(); ++j) {
			vector<string> ftemp = first(a->right[j]);
			f.insert(ftemp.begin(), remove(ftemp.begin(), ftemp.end(), "lambda"));
			if (find(ftemp.begin(), ftemp.end(), "lambda") == ftemp.end()) add_follow = false;
		}
		if (add_follow && a->left != target) {
			set<string> fo = follow(a->left, a->left);
			f.insert(fo.begin(), fo.end());
		}
	}
	follows[non_terminal] = f;
	return f;
}

static void fill_follows() {
	follows["Program"].insert("$");
	for (auto NT: NT_to_P) {
		set<string> temp = follow(NT.first, NT.first);
		printf("Follow(%s) = {", NT.first.c_str());
		for (auto t: temp)
			printf("%s, ", t.c_str());
		printf("}\n");
	}
}
