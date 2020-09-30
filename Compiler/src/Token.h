#ifndef TOKEN_H
#define TOKEN_H
#include <iostream>
#include <string>
class Token 
{
public:
	enum class Type
	{
		KEYWORD,
		IDENTIFIER,
		LITERAL,
		SEPARATOR,
		OPERATOR,
		COMMENT,

		MAX_ELEMENTS
	};

	Token(const std::string &token_literal, Type type);
	const char* getType(Type type) const;
	friend std::ostream& operator<<(std::ostream &out, const Token &token);
private:
	std::string m_token_literal{};
	Type m_type{};
};

Token::Token(const std::string &token_literal, Type type)
	: m_token_literal{ token_literal }, m_type{ type }
{
}

const char* Token::getType(Type type) const
{

	switch (type)
	{
	case Type::KEYWORD:
		return "Keyword";
	case Type::IDENTIFIER:
		return "Identifier";
	case Type::LITERAL:
		return "Literal";
	case Type::SEPARATOR:
		return "Separator";
	case Type::OPERATOR:
		return "Operator";
	case Type::COMMENT:
		return "Comment";
	default:
		return "error";
	}
}

std::ostream& operator<<(std::ostream &out, const Token &token)
{
	out << token.m_token_literal << '\t' << token.getType(token.m_type);
	return out;
}
#endif