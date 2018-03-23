#include "parse_table_generator.h"
#include "string_utils.h"
#include "grammar_utils.h"

using namespace std;

vector<Production *> all_productions;
set<string> non_terminals;
set<string> terminals;

map<string, map<string, int>> parse_table;

unsigned number_of_non_terminals = 0;
bool print_flag;

static void print_NT_to_P() {
	for (auto NT: NT_to_P)
		for (auto production: NT.second)
			production->print();
}

static void scan(string *filename, char delimiter) {
	string s(istreambuf_iterator<char>(ifstream(*filename) >> skipws), istreambuf_iterator<char>());
	vector<string> production_lines = split_and_trim(s, '\n');
	for (const auto &production: production_lines) {
		if (production.empty()) continue;
		vector<string> splitted_production = split_and_trim(production, delimiter);
		string left_side = splitted_production[0];
		vector<string> right_side_list = split_and_trim(splitted_production[1], '|');
		for (const auto &right_side: right_side_list) {
			vector<string> symbols = split_and_trim(right_side, ' ');
			for (auto symbol: symbols) {
				if (symbol[0] >= 'A' && symbol[0] <= 'Z') non_terminals.insert(symbol);
				else terminals.insert(symbol);
			}
			Production *p = new Production(number_of_non_terminals, left_side, symbols);
			all_productions.push_back(p);
			NT_to_P[left_side].push_back(p);
			++number_of_non_terminals;
		}
	}
}

static vector<string> first(string symbol) {
	if (!(symbol[0] >= 'A' && symbol[0] <= 'Z')) return vector<string>(1, symbol);
	else if (!firsts[symbol].empty()) return firsts[symbol];
	unsigned i;
	bool add_lambda = false;
	vector<string> f;
	for (auto production: NT_to_P[symbol]) {
		i = 0;
		for (const auto &s: production->right) {
			vector<string> ftemp = first(s);
			f.insert(f.end(), ftemp.begin(), remove(ftemp.begin(), ftemp.end(), "lambda"));
			if (find(ftemp.begin(), ftemp.end(), "lambda") == ftemp.end()) break;
			++i;
		}
		if (i == production->right.size()) add_lambda = true;
	}
	if (add_lambda) f.emplace_back("lambda");
	firsts[symbol] = f;
	return f;
}

static void fill_firsts() {
	for (auto NT: NT_to_P) {
		vector<string> temp = first(NT.first);
		printf("First(%s) = {", NT.first.c_str());
		for (const auto &t: temp)
			printf("%s, ", t.c_str());
		printf("}\n");
	}
}

static set<string> follow(const string &non_terminal, const string &target) {
	if (!follows[non_terminal].empty()) return follows[non_terminal];
	set<string> f;
	vector<Production *> productions = find_symbol(non_terminal);
	bool add_follow;
	for (auto a: productions) {
		add_follow = true;
		int i = a->find_symbol(non_terminal) + 1;
		for (int j = i; add_follow && j < a->right.size(); ++j) {
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
		for (const auto &t: temp)
			printf("%s, ", t.c_str());
		printf("}\n");
	}
}

static set<string> predict(Production *production) {
	if (!production->predicts.empty()) return production->predicts;
	set<string> p;
	unsigned c = 0;
	for (const auto &symbol: production->right) {
		vector<string> ftemp = first(symbol);
		p.insert(ftemp.begin(), remove(ftemp.begin(), ftemp.end(), "lambda"));
		if (find(ftemp.begin(), ftemp.end(), "lambda") == ftemp.end()) break;
		++c;
	}
	if (c == production->right.size()) {
		set<string> fotemp = follow(production->left, production->left);
		p.insert(fotemp.begin(), fotemp.end());
	}
	production->predicts = p;
	return p;
}

static void fill_parse_table() {
	for (auto NT: NT_to_P) {
		for (auto production: NT.second) {
			production->print();
			printf("Predict = {");
			for (const auto &symbol: predict(production)) {
				printf("%s, ", symbol.c_str());
				if (parse_table[production->left][symbol] == 0)
					parse_table[production->left][symbol] = production->index;
				else printf("Predict error in production number %d\n", production->index);
			}
			printf("}\n");
		}
	}
}

void generate_parse_table(string filename = "Grammar.txt", char delimiter = '>', bool flag = true) {
	print_flag = flag;

	printf("\n--------------------------\tProductions\t--------------------------\n\n");
	scan(&filename, delimiter);
	print_NT_to_P();

	printf("\n--------------------------\tHighlights\t--------------------------\n\n");
	for (auto NT: NT_to_P) {
		for (auto p: find_symbol(NT.first))
			p->print(NT.first);
		printf("\n");
	}

	printf("\n--------------------------\tFirsts\t--------------------------\n\n");
	fill_firsts();

	printf("\n--------------------------\tFollows\t--------------------------\n\n");
	fill_follows();

	printf("\n--------------------------\tPredicts\t--------------------------\n\n");
	fill_parse_table();

	printf("\n--------------------------\tParse Table\t--------------------------\n\n");
	for (auto n: parse_table) {
		printf("%s: ", n.first.c_str());
		for (auto t: n.second) {
			printf("{%s: %d}, ", t.first.c_str(), t.second);
		}
		printf("\n");
	}
}

void push_to_stack(std::string *non_terminal, std::string *terminal, std::stack<string> *st, bool *error_flag) {
	try {
		Production *p = all_productions.at(parse_table.at(*non_terminal).at(*terminal));
		(*st).pop();
		vector<string> right_hand_side = p->right;
		reverse(right_hand_side.begin(), right_hand_side.end());
		printf("add: ");
		p->print();
		for (const auto &symbol: right_hand_side)
			st->push(symbol);
	} catch (const out_of_range &oor) {
		*error_flag = true;
	}
}
