#pragma once

#include <exception>
#include <string>

#include <XMLUtility.hpp>

class ParseError : public std::exception {
    std::string m_msg;
    std::string m_errorSnippet;

public:
    ParseError(
        std::string const &errorLine = "No error code snippet were provided",
        std::string const &msg = "No error message were provided"
    ) : m_msg(msg), m_errorSnippet(errorLine) {}

    virtual const char *what() const noexcept {
        return (std::string("\nParsing exception occured at:\n\t\"") + m_errorSnippet + "\"\n With the message:\n\t\"" + m_msg + "\"").c_str();
    }
};

class TypeError : public std::exception {
    std::string m_msg;
    XMLElementType m_wasType;

public:
    TypeError() = delete;

    TypeError(
        XMLElementType wasType, 
        std::string const &msg = "No error message were provided"
    ) : m_msg(msg), m_wasType(wasType) {}

    virtual const char *what() const noexcept {
        std::string res = "Type exception occured\nThe resulting type was:\n\t\"";
        res += toString(m_wasType);
        res += "\"\nWith the message:\n\t\"" + m_msg + "\"\n";
        return res.c_str();
    }
};