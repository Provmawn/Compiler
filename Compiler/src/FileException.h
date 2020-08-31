#ifndef FILEEXCEPTION_H
#define FILEEXCEPTION_H
#include <exception>
#include <iostream>
#include <string>
#include <string_view>
class FileException : public std::exception
{
public:
	FileException(std::string_view error, std::string_view path);
	const char* what() const override;
private:
	std::string m_error{};
	std::string m_path{};
	std::string m_message{};
};

FileException::FileException(std::string_view error, std::string_view path)
	: m_error{ static_cast<std::string>(error) }
	, m_path{ static_cast<std::string>(path) }
{
	m_message = m_error + " \"" + m_path + '\"';
}

const char* FileException::what() const
{
	return m_message.c_str();
}
#endif