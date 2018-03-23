#include <string>

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED

using namespace std;

string get_token(bool *);

void fill_code(string);

long long unsigned get_line_index();

long long unsigned get_index_in_line();

string get_line();

#endif // SCANNER_H_INCLUDED
