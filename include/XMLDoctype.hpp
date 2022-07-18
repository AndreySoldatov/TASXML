#pragma once

#include <unordered_map>

#include <XMLUtility.hpp>

class XMLDoctype {
    std::string m_root;
    bool m_hasDoctype{};

public:

    XMLDoctype(std::string const &str) {
        assign(str);
    }

    XMLDoctype &operator=(std::string const &str) {
        return assign(str);
    }

    XMLDoctype &assign(std::string const &str) {
        std::string defStr(deflate(str));
        
        size_t doctypeIndex = defStr.find("<!");
        if(toLowerCase(defStr.substr(doctypeIndex + 2, 7)) == "doctype") {
            m_hasDoctype = true;
            size_t indexOfName = std::min(defStr.find(">"), defStr.find(" ", doctypeIndex + 10));
            m_root = defStr.substr(doctypeIndex + 10, indexOfName - doctypeIndex - 10);
            if(m_root.empty()) {
                throw ParseError(defStr, "Doctype has no root");
            }
        }
        
        return *this;
    }
};