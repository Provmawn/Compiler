#include "Lexer.h"
#include <iostream>

int main()
{
	Lexer lexer{ "src/file.txt" };
	lexer.tokenize();
	std::cout << lexer << '\n';
	return 0;
}