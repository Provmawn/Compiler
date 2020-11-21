#include "Parser.h"
#include <iostream>

int main()
{
	Parser parser{ "src/file.txt" };
	parser.parse();
	return 0;
}