#ifndef LEXER_H
#define LEXER_H
#include "FileException.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
class Lexer
{
public:
	Lexer() = default;
	Lexer(std::string_view path);
private:
	std::ifstream m_input_file{};
	std::string m_buffer{};
};

Lexer::Lexer(std::string_view path)
{
	try {
		m_input_file.open(static_cast<std::string>(path));
		if (!m_input_file)
			throw FileException("Unable to open file.", path);

		// copy file into buffer
		std::stringstream middle_buffer{};
		middle_buffer << m_input_file.rdbuf();
		m_input_file.close();
		m_buffer = middle_buffer.str();
	}
	catch (const FileException &file_exception)
	{
		std::cerr << file_exception.what() << '\n';
	}
	std::cout << m_buffer;
}
#endif