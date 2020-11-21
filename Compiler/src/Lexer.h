#ifndef LEXER_H
#define LEXER_H
#include "FileException.h"
#include "Token.h"
#include <algorithm>
#include <array>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
class Lexer
{
public:
	Lexer() = default;
	Lexer(std::string_view path);

	void tokenize();
	std::vector<Token>& getTokens() { return m_tokens; }

	int getSize() { return static_cast<int>(m_tokens.size()); }
	Token& operator[](int index)
	{
		return m_tokens.at(index);
	}
	friend std::ostream& operator<<(std::ostream &out, const Lexer &lexer);
private:
	// final states are specified within the documentation with and underline
	enum class FinalState
	{
		IDENTIFIER = 2,
		INTEGER_LITERAL = 4,
		FLOATING_POINT_LITERAL = 6,
		COMMENT = 8,
		ONE_OPERATOR = 11,
		TWO_OPERATOR = 12,
		SEPARATOR = 14,
		ERROR = 15,

		MAX_ELEMENTS
	};
	enum class ColumnIndex {
		LETTER,
		DIGIT,
		DOLLAR,
		APOSTROPHE,
		LPAREN,
		RPAREN,
		LBRACE,
		RBRACE,
		LBRACKET,
		RBRACKET,
		COMMA,
		PERIOD,
		COLON,
		SEMICOLON,
		SPACE,
		ASTERISK,
		PLUS,
		MINUS,
		EQUAL,
		DIVIDE,
		GREATER_THAN,
		LESS_THAN,
		MODULUS,
		EXCALMATION,

		MAX_ELEMENTS
	};
	std::array<std::array<int, 24>, 16> m_state_table{{
		{2,	 4, 16, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 11, 11, 11, 11, 11, 11, 11, 11,  8},
		{2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3},
		{1,	 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
		{5,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5},
		{1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
		{7,  6,	 7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7},
		{1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
		{8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9},
		{1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
		{12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 11, 11, 11, 11, 11, 11, 11, 11, 12},
		{13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13},
		{1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
		{1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
		{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
		{1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
		{16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16}
	} };

	// this list is used after creating an "identifier" from the state table to check if the "identifier" was a keyword
	std::array<std::string, 20> m_keywords{
		"int", "float", "bool", "true", "false", "if", "else", "then", "endif", "while", "whileend", "do"
		, "doend", "for", "forend", "input", "output", "and", "or", "not"
	};

	std::vector<Token> m_tokens{};
	std::ifstream m_input_file{};
	std::string m_buffer{};

	int m_lexeme_start{};
	int m_pos{};
	int m_current_state{};

	int columnFromChar(char current) const;
	bool isFinalState(int current_state) const;
	Token eatToken(int current_state);
};

Lexer::Lexer(std::string_view path)
	: m_lexeme_start{ 0 }, m_pos{ 0 }, m_current_state{ 0 }
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
	//std::cout << m_buffer;
}


// this function performs the FSM operations
// 1.) read from the text file starting from the first character
// 2.) using the current character calculate next state and set as current
// 3.) if it is a final state then create a token and move the the next character
// 4.) if it is not a final state then move to the next character
void Lexer::tokenize()
{
	// read text until the end of the file
	while (m_pos < m_buffer.length())
	{
		m_current_state = m_state_table[m_current_state][columnFromChar(m_buffer[m_pos])] - 1;
		if (isFinalState(m_current_state))
			m_tokens.push_back(eatToken(m_current_state));
		else
			++m_pos;
	}
	// must set the state table to a final state get the last token (so here I set it to "space")
	m_current_state = m_state_table[m_current_state][static_cast<int>(ColumnIndex::SPACE)] - 1;
	if (isFinalState(m_current_state))
		m_tokens.push_back(eatToken(m_current_state));
}

int Lexer::columnFromChar(char current) const
{
	if (std::isalpha(static_cast<unsigned char>(current)))
		return static_cast<int>(ColumnIndex::LETTER);
	else if (std::isdigit(static_cast<unsigned char>(current)))
		return static_cast<int>(ColumnIndex::DIGIT);
	else if (std::isspace(static_cast<unsigned char>(current)))
		return static_cast<int>(ColumnIndex::SPACE);
	switch (current)
	{
	case '$':
		return static_cast<int>(ColumnIndex::DOLLAR);
	case '\'':
		return static_cast<int>(ColumnIndex::APOSTROPHE);
	case '(':
		return static_cast<int>(ColumnIndex::LPAREN);
	case ')':
		return static_cast<int>(ColumnIndex::RPAREN);
	case '{':
		return static_cast<int>(ColumnIndex::LBRACE);
	case '}':
		return static_cast<int>(ColumnIndex::RBRACE);
	case '[':
		return static_cast<int>(ColumnIndex::LBRACKET);
	case ']':
		return static_cast<int>(ColumnIndex::RBRACKET);
	case ',':
		return static_cast<int>(ColumnIndex::COMMA);
	case '.':
		return static_cast<int>(ColumnIndex::PERIOD);
	case ':':
		return static_cast<int>(ColumnIndex::COLON);
	case ';':
		return static_cast<int>(ColumnIndex::SEMICOLON);
	case '*':
		return static_cast<int>(ColumnIndex::ASTERISK);
	case '+':
		return static_cast<int>(ColumnIndex::PLUS);
	case '-':
		return static_cast<int>(ColumnIndex::MINUS);
	case '=':
		return static_cast<int>(ColumnIndex::EQUAL);
	case '/':
		return static_cast<int>(ColumnIndex::DIVIDE);
	case '>':
		return static_cast<int>(ColumnIndex::GREATER_THAN);
	case '<':
		return static_cast<int>(ColumnIndex::LESS_THAN);
	case '%':
		return static_cast<int>(ColumnIndex::MODULUS);
	case '!':
		return static_cast<int>(ColumnIndex::EXCALMATION);
	}
}

bool Lexer::isFinalState(int current_state) const
{
	// final states are states 3, 5, 7, 9, 12, 13, 15 but we must -1 for a 0 index
	switch (static_cast<FinalState>(current_state))
	{
	case FinalState::IDENTIFIER:
	case FinalState::INTEGER_LITERAL:
	case FinalState::FLOATING_POINT_LITERAL:
	case FinalState::COMMENT:
	case FinalState::ONE_OPERATOR:
	case FinalState::TWO_OPERATOR:
	case FinalState::SEPARATOR:
	case FinalState::ERROR:
		return true;
	}
	return false;
}

Token Lexer::eatToken(int current_state)
{
	int offset = m_lexeme_start;
	int length = m_pos - m_lexeme_start;
	std::string token_literal{ m_buffer.substr(offset, length) };
	m_lexeme_start = m_pos;
	m_current_state = 0;
	if (static_cast<FinalState>(current_state) == FinalState::IDENTIFIER)
	{
		if (std::find(m_keywords.begin(), m_keywords.end(), token_literal) != m_keywords.end()) // identifier is a keyword
			return Token(token_literal, Token::Type::KEYWORD);
		else
			return Token(token_literal, Token::Type::IDENTIFIER);
	}
	switch (static_cast<FinalState>(current_state))
	{
	case FinalState::INTEGER_LITERAL:
		return Token(token_literal, Token::Type::LITERAL);
	case FinalState::FLOATING_POINT_LITERAL:
		return Token(token_literal, Token::Type::LITERAL);
	case FinalState::COMMENT:
		return Token(token_literal, Token::Type::COMMENT);
	case FinalState::ONE_OPERATOR:
		return Token(token_literal, Token::Type::OPERATOR);
	case FinalState::TWO_OPERATOR:
		return Token(token_literal, Token::Type::OPERATOR);
	case FinalState::SEPARATOR:
		return Token(token_literal, Token::Type::SEPARATOR);
	case FinalState::ERROR:
		// TODO: Throw exception
		break;
	}
}

std::ostream& operator<<(std::ostream &out, const Lexer &lexer)
{
	for (const Token &token : lexer.m_tokens)
		out << token << '\n';
	return out;
}
#endif