#pragma once

#include <unordered_map>

#include <XMLUtility.hpp>
#include <XMLErrors.hpp>

class XMLProlog {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_prolog;

public:

    XMLProlog(std::string const &str) {
        assign(str);
    }

    XMLProlog &operator=(std::string const &str) {
        return assign(str);
    }

    XMLProlog &assign(std::string const &str) {
        std::string defStr(deflate(str));

        while(defStr.find("<?") != std::string::npos) {
            size_t indexOfName = std::min(defStr.find("?>"), defStr.find(" "));
            if(indexOfName == defStr.find("<?") + 2 || indexOfName == std::string::npos) {
                throw ParseError(defStr, "prolog has no id");
            }
            std::string tagName = defStr.substr(
                defStr.find("<?") + 2,
                indexOfName - defStr.find("<?") - 2
            );
            std::cout << tagName << "\n";

            m_prolog[tagName] = splitAttributes(defStr.substr(indexOfName, defStr.find("?>") - indexOfName));
            
            defStr.erase(defStr.begin(), defStr.begin() + defStr.find("?>") + 2);
        }

        return *this;
    }

    std::unordered_map<std::string, std::string> const &operator[](std::string const &str) const {
        return m_prolog.at(str);
    }

    std::unordered_map<std::string, std::string> &operator[](std::string const &str) {
        return m_prolog.at(str);
    }
};