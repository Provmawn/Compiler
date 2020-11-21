#ifndef PARSER_H
#define PARSER_H
#include "Lexer.h"
#include <iostream>
#include <string_view>
#include <stack>

class Parser
{
public:
	enum class Symbol
	{
		DOLLAR,
		START,
		EXPRESSION,
		EXPRESSION_P,
		TERM,
		TERM_P,
		FACTOR,
		FACTOR_P,
		PLUS,
		MINUS,
		TIMES,
		DIVIDE,
		EPSILON,
		NUM,
		ID,
		STATEMENT,
		DECLARATION,

		MAX_ELEMENT
	};
	Parser() = default;
	Parser(std::string_view path);
	void parse();
private:
	std::stack<Symbol> m_LLparser{};
	Lexer m_lexer{};
	int m_pos{};

	void E()
	{
		if (m_lexer[m_pos].getType() == Token::Type::IDENTIFIER
			|| m_lexer[m_pos].getType() == Token::Type::LITERAL
			|| m_lexer[m_pos].getValue() == "(")
		{
			std::cout << "E -> TE'\n";
			m_LLparser.pop(); // remove E
			m_LLparser.push(Symbol::EXPRESSION_P); // push E'
			m_LLparser.push(Symbol::TERM); // push T
		}
	}

	void E_P()
	{
		if (m_lexer[m_pos].getValue() == ")" || m_lexer[m_pos].getValue() == "$")
		{
			std::cout << "E' -> epsilon\n";
			m_LLparser.pop();
		}
		else if (m_lexer[m_pos].getValue() == "+")
		{
			std::cout << "E' -> +TE'\n";
			m_LLparser.pop(); // remove E'
			m_LLparser.push(Symbol::PLUS);
			m_LLparser.push(Symbol::TERM);
			m_LLparser.push(Symbol::EXPRESSION_P);
		}
		else if (m_lexer[m_pos].getValue() == "+")
		{
			std::cout << "E' -> -TE'\n";
			m_LLparser.pop(); // remove E'
			m_LLparser.push(Symbol::MINUS);
			m_LLparser.push(Symbol::TERM);
			m_LLparser.push(Symbol::EXPRESSION_P);
		}
	}

	void T()
	{
		if (m_lexer[m_pos].getType() == Token::Type::IDENTIFIER
			|| m_lexer[m_pos].getType() == Token::Type::LITERAL
			|| m_lexer[m_pos].getValue() == "(")
		{
			std::cout << "T -> F\n";
			m_LLparser.pop(); // remove T
			m_LLparser.push(Symbol::FACTOR); // push F
		}
	}

	void T_P()
	{
		if (m_lexer[m_pos].getValue() == "-"
			|| m_lexer[m_pos].getValue() == "+")
		{
			std::cout << "T' -> epsilon\n";
			m_LLparser.pop();
		}
		else if (m_lexer[m_pos].getValue() == "*")
		{
			std::cout << "T' -> *FT'";
			m_LLparser.pop();
			m_LLparser.push(Symbol::TERM_P);
			m_LLparser.push(Symbol::FACTOR);
			m_LLparser.push(Symbol::TIMES);
		}
		else if (m_lexer[m_pos].getValue() == "/")
		{
			std::cout << "T' -> /FT'";
			m_LLparser.pop();
			m_LLparser.push(Symbol::TERM_P);
			m_LLparser.push(Symbol::FACTOR);
			m_LLparser.push(Symbol::DIVIDE);
		}
	}

	void F()
	{
		if (m_lexer[m_pos].getType() == Token::Type::IDENTIFIER)
		{
			std::cout << "F -> ID\n";
			m_LLparser.pop();
			m_LLparser.push(Symbol::ID);
		}
		else if (m_lexer[m_pos].getType() == Token::Type::LITERAL)
		{
			std::cout << "F -> num\n";
			m_LLparser.pop();
		}
		else if (m_lexer[m_pos].getType() == Token::Type::LITERAL)
		{
			std::cout << "F -> (E)\n";
			m_LLparser.pop();
			m_LLparser.push(Symbol::EXPRESSION);
		}
	}

	void ID()
	{
		if (m_lexer[m_pos].getType() == Token::Type::IDENTIFIER)
		{
			std::cout << "ID -> id\n";
			m_LLparser.pop();
		}
	}

	void STATEMENT()
	{
		if (m_lexer[m_pos].getValue() == "int"
			|| m_lexer[m_pos].getValue() == "float"
			|| m_lexer[m_pos].getValue() == "bool")
		{
			std::cout << "STATEMENT -> DECLARATION\n";
			m_LLparser.pop(); // remove STATEMENT
			m_LLparser.push(Symbol::DECLARATION);

		}
	}

	void DECLARATION()
	{
		if (m_lexer[m_pos].getValue() == "int"
			|| m_lexer[m_pos].getValue() == "float"
			|| m_lexer[m_pos].getValue() == "bool")
		{
			m_LLparser.pop();
			// int float or bool
		}

	}
};

Parser::Parser(std::string_view path)
	: m_lexer{ path }
{
	m_lexer.tokenize();
	m_LLparser.push(Symbol::DOLLAR);
	m_LLparser.push(Symbol::EXPRESSION);
	//std::cout << m_lexer << '\n';
}

void Parser::parse()
{
	// peek ahead 1 to see what rule applies
	while (m_pos < m_lexer.getSize())
	{
		if (m_lexer[m_pos].getType() == Token::Type::IDENTIFIER)
		{
			if (m_LLparser.top() == Symbol::EXPRESSION)
			{
				E(); // TE'
			}
			else if (m_LLparser.top() == Symbol::TERM)
			{
				T();
			}
			else if (m_LLparser.top() == Symbol::FACTOR)
			{
				F();
			}
			else if (m_LLparser.top() == Symbol::ID)
			{
				ID();
			}
		}
		else if (m_lexer[m_pos].getType() == Token::Type::LITERAL)
		{
			if (m_LLparser.top() == Symbol::EXPRESSION)
			{
				E(); // TE'
			}
			else if (m_LLparser.top() == Symbol::TERM)
			{
				T();
			}
			else if (m_LLparser.top() == Symbol::FACTOR)
			{
				F();
			}
		}
		else if (m_lexer[m_pos].getType() == Token::Type::SEPARATOR)
		{
			if (m_lexer[m_pos].getValue() == "(")
			{
				if (m_LLparser.top() == Symbol::EXPRESSION)
				{
					E(); // TE'
				}
				else if (m_LLparser.top() == Symbol::TERM)
				{
					T();
				}
				else if (m_LLparser.top() == Symbol::FACTOR)
				{
					F();
				}
			}
			else if (m_lexer[m_pos].getValue() == ")")
			{
				if (m_LLparser.top() == Symbol::EXPRESSION_P)
				{
					E_P();
				}
			}
			else if (m_lexer[m_pos].getValue() == ";")
			{
				if (m_LLparser.top() == Symbol::EXPRESSION_P)
				{
					E_P();
				}
			}
		}
		else if (m_lexer[m_pos].getType() == Token::Type::OPERATOR)
		{
			if (m_lexer[m_pos].getValue() == "+")
			{
				if (m_LLparser.top() == Symbol::EXPRESSION_P)
				{
					E_P();
				}
				else if (m_LLparser.top() == Symbol::TERM_P)
				{
					T_P();
				}
			}
			else if (m_lexer[m_pos].getValue() == "-")
			{
				if (m_LLparser.top() == Symbol::EXPRESSION_P)
				{
					E_P();
				}
				else if (m_LLparser.top() == Symbol::TERM_P)
				{
					T_P();
				}
			}
			else if (m_lexer[m_pos].getValue() == "*")
			{
				if (m_LLparser.top() == Symbol::TERM_P) {
					T_P();
				}
			}
			else if (m_lexer[m_pos].getValue() == "/")
			{
				if (m_LLparser.top() == Symbol::TERM_P) {
					T_P();
				}
			}
		}
		else if (m_lexer[m_pos].getType() == Token::Type::KEYWORD)
		{
			if (m_lexer[m_pos].getValue() == "int")
			{ 
				if (m_LLparser.top() == Symbol::DECLARATION)
				{
					DECLARATION();
				}
				else if (m_LLparser.top() == Symbol::STATEMENT)
				{
					STATEMENT();
				}
			}
			else if (m_lexer[m_pos].getValue() == "float")
			{
				if (m_LLparser.top() == Symbol::DECLARATION)
				{
					DECLARATION();
				}
				else if (m_LLparser.top() == Symbol::STATEMENT)
				{
					STATEMENT();
				}
			}
			else if (m_lexer[m_pos].getValue() == "bool")
			{
				if (m_LLparser.top() == Symbol::DECLARATION)
				{
					DECLARATION();
				}
				else if (m_LLparser.top() == Symbol::STATEMENT)
				{
					STATEMENT();
				}
			}
		}
		++m_pos;
	}
}

#endif