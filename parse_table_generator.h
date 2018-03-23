#include <string>
#include <stack>
#include <fstream>
#include <set>

#ifndef PARSE_TABLE_GENERATOR_H_INCLUDED
#define PARSE_TABLE_GENERATOR_H_INCLUDED

using namespace std;

void generate_parse_table(string, char, bool);

void push_to_stack(string *, string *, stack<string> *, bool *);

#endif // PARSE_TABLE_GENERATOR_H_INCLUDED
