#pragma once

#include <exception>
#include <string>

#include <XMLTagType.hpp>

class ParseError : public std::exception {
    std::string m_msg;
    std::string m_errorSnippet;

    std::string m_what;

public:
    ParseError(
        std::string const &errorLine = "No error code snippet were provided",
        std::string const &msg = "No error message were provided"
    ) : m_msg(msg), m_errorSnippet(errorLine) {
        m_what = (std::string("\nParsing exception occured at:\n\t\"") + m_errorSnippet + "\"\n With the message:\n\t\"" + m_msg + "\"");
    }

    virtual const char *what() const noexcept {
        return m_what.c_str();
    }
};

class TypeError : public std::exception {
    std::string m_msg;
    XMLElementType m_wasType;

    std::string m_what;

public:
    TypeError() = delete;

    TypeError(
        XMLElementType wasType, 
        std::string const &msg = "No error message were provided"
    ) : m_msg(msg), m_wasType(wasType) {
        m_what = "Type exception occured\nThe resulting type was:\n\t\"";
        m_what += toString(m_wasType);
        m_what += "\"\nWith the message:\n\t\"" + m_msg + "\"\n";
    }

    virtual const char *what() const noexcept {
        return m_what.c_str();
    }
};