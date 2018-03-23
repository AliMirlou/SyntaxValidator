#include "parse_table_generator.h"
#include "scanner.h"

using namespace std;

main(int argc, char *argv[]) {
	string grammar_filename = "grammar.txt", code_filename = "code.txt";
	char delimiter = '>';
	bool flag = false;
	if (argc > 1) code_filename = argv[1];
	generate_parse_table(grammar_filename, delimiter, true);

	printf("--------------------------\tParsing Code\t--------------------------\n");
	fill_code(code_filename);

	string token = get_token(&flag);
	stack<string> st;
	st.push("$");
	st.push("Program");
	do {
		string top = st.top();
		printf("top of stack: %s, token: %s\n", st.top().c_str(), token.c_str());
		if (token == top) {
			printf("token %s matched\n", token.c_str());
			token = get_token(&flag);
			st.pop();
		} else if (!(top[0] >= 'A' && top[0] <= 'Z')) {
			if (top != "lambda") {
				flag = true;
				break;
			}
			st.pop();
		} else push_to_stack(&top, &token, &st, &flag);
	} while (!st.empty() && !flag);
	if (flag) {
		long long unsigned i = get_index_in_line(), k = 0;
		string temp = get_line(), pointer;
		printf("Error: In line %lld, %lld\n", get_line_index(), i);
		printf("%s\n", temp.c_str());
		for (k = 0; k < temp.size(); ++k)
			if (k != i) pointer += '-';
			else pointer += '^';
		printf("%s\n", pointer.c_str());
		printf("Expected %s, got %s\n", st.top().c_str(), token.c_str());
	}
}
