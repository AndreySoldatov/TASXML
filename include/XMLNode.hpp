#pragma once

#define DEBUG

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#endif

#include <XMLErrors.hpp>
#include <XMLUtility.hpp>

inline void clearComments(std::string &str) {
    while(str.find("<!--") != std::string::npos) {
        auto indexOfCommentStart = str.find("<!--");
        auto indexOfCommentEnd = str.find("-->");
        if(indexOfCommentEnd == std::string::npos) {
            throw ParseError(str, "Comment block has no end");
        }
        indexOfCommentEnd += 3;
        str.erase(str.begin() + indexOfCommentStart, str.begin() + indexOfCommentEnd);
    }
}

inline std::string deflate(std::string const &str) {
    // Removing all new line characters
    std::string res(str);
    res.erase(std::remove(res.begin(), res.end(), '\n'), res.end());

    //Removing comments
    clearComments(res);
    
    // Removing spaces
    int index{};
    while(index < (int)res.length()) {
        if(std::isspace(res[index]) && (index == 0 || res[index - 1] == '>')) {
            int spaceLength{index};
            while(std::isspace(res[spaceLength]) && spaceLength < (int)res.length()) {
                spaceLength++;
            }
            res.erase(res.begin() + index, res.begin() + spaceLength);
        }
        index++;
    }

    index = res.length() - 1;
    while(index >= 0) {
        if(std::isspace(res[index]) && (index == (int)res.length() - 1 || res[index + 1] == '<')) {
            int spaceLength{index};
            while(std::isspace(res[spaceLength]) && spaceLength >= 0) {
                spaceLength--;
            }
            res.erase(res.begin() + spaceLength + 1, res.begin() + index + 1);
            index = spaceLength;
        }
        index--;
    }
    
    return res;
}

inline std::vector<std::pair<std::string, XMLElementType>> splitByTags(std::string const &str) {
    std::vector<std::pair<std::string, XMLElementType>> res;

    auto defStr = deflate(str);

    while(!defStr.empty()) {
        if(defStr[0] == '<') {
            auto endIndex = defStr.find('>', 1);
            if(defStr.find('<', 1) < endIndex || endIndex == std::string::npos) {
                throw ParseError(defStr, "tag is not closed");
            }
            if(defStr[endIndex-1] == '/') {
                res.push_back({defStr.substr(0, endIndex + 1), XMLElementType::SINGLE_TAG});
                defStr.erase(defStr.begin(), defStr.begin() + endIndex + 1);
            } else {
                std::string tagName = defStr.substr(1, std::min(defStr.find(' '), defStr.find('>')) - 1);
                auto closeIndex = defStr.find("</" + tagName + ">");
                if(closeIndex == std::string::npos) {
                    throw ParseError(str, "paired tag does not have a counterpart");
                }
                res.push_back({defStr.substr(0, closeIndex + 3 + tagName.length()), XMLElementType::PAIRED_TAG});
                defStr.erase(defStr.begin(), defStr.begin() + closeIndex + 3 + tagName.length());
            }
        } else {
            auto endIndex = std::min(defStr.find('<'), defStr.length());
            res.push_back({defStr.substr(0, endIndex), XMLElementType::TEXT});
            defStr.erase(defStr.begin(), defStr.begin() + endIndex);
        }
    }

    return res;
}

std::unordered_map<std::string, std::string> splitAttributes(std::string const &str) {
    std::unordered_map<std::string, std::string> res;

    std::string defStr(str);
    defStr.erase(std::remove(defStr.begin(), defStr.end(), ' '), defStr.end());
    
    while(!defStr.empty()) {
        std::string key = defStr.substr(0, defStr.find('='));

        if(key.length() == 0 || key.length() == defStr.length()) {
            throw ParseError(str, "Attribute has no name or has no assign sign");
        }
        size_t firstQuoteIndex = defStr.find('=') + 1;
        size_t secondQuoteIndex{};
        std::string val;
        if(defStr[firstQuoteIndex] == '"' && firstQuoteIndex < defStr.length()) {
            secondQuoteIndex = defStr.find('"', firstQuoteIndex + 1);
            val = defStr.substr(firstQuoteIndex + 1, secondQuoteIndex - firstQuoteIndex - 1);
        } else if(defStr[firstQuoteIndex] == '\'' && firstQuoteIndex < defStr.length()) {
            secondQuoteIndex = defStr.find('\'', firstQuoteIndex + 1);
            val = defStr.substr(firstQuoteIndex + 1, secondQuoteIndex - firstQuoteIndex - 1);
        } else {
            throw ParseError(str, "Attribute does not have quotes");
        }
        
        res[key] = val;

        defStr.erase(defStr.begin(), defStr.begin() + secondQuoteIndex + 1);
    }

    return res;
}

class XMLNode {
    XMLElementType m_tagType;
    std::string m_element;
    std::string m_content;
    std::vector<XMLNode> m_childNodes;
    std::unordered_map<std::string, std::string> m_attributes;

    XMLNode &assign(std::string const &str, XMLElementType const &type) {
        switch (type)
        {
            case XMLElementType::TEXT:
                m_content = str;
                break;
            
            case XMLElementType::SINGLE_TAG:
                m_element = str.substr(1, std::min(str.find(' '), str.find('/')) - 1);
                
                m_attributes = splitAttributes(
                    str.substr(
                        std::min(str.find(' '), str.find('/')), 
                        str.find("/>") - std::min(str.find(' '), str.find('/'))
                    )
                );
                break;
            
            case XMLElementType::PAIRED_TAG:
                {
                    m_element = str.substr(1, std::min(str.find(' '), str.find('>')) - 1); 

                    m_attributes = splitAttributes(
                        str.substr(
                            std::min(str.find(' '), str.find('>')), 
                            str.find(">") - std::min(str.find(' '), str.find('>'))
                        )
                    );  

                    auto vec = splitByTags(str.substr(str.find('>') + 1, str.find("</" + m_element) - str.find('>') - 1));
                    for (auto &pair : vec) {
                        m_childNodes.push_back({});
                        m_childNodes[m_childNodes.size() - 1].assign(pair.first, pair.second);
                    }
                }
                break;
            
            default:
                break;
        }
        m_tagType = type;
        
        return *this;
    }

    std::string dumpWithRecIndex(size_t del, bool oneLine, size_t recIndex = 0) const {
        std::string res;
        switch (m_tagType)
        {
            case XMLElementType::TEXT:
                res = std::string(del * recIndex, ' ') + m_content + (oneLine ? "" : "\n");
                break;

            case XMLElementType::SINGLE_TAG:
                res = std::string(del * recIndex, ' ') + "<" + m_element + " ";
                for (auto &[k, v] : m_attributes) {
                    res += k + "=\"" + v + "\" ";
                }
                res += "/>";
                res += (oneLine ? "" : "\n");
                break;

            case XMLElementType::PAIRED_TAG:
                res = std::string(del * recIndex, ' ') + "<" + m_element + " ";
                for (auto &[k, v] : m_attributes) {
                    res += k + "=\"" + v + "\" ";
                }
                res += ">";
                res += (oneLine ? "" : "\n");
                for (auto &node : m_childNodes) {
                    res += node.dumpWithRecIndex(del, oneLine, recIndex + 1);
                }
                res += std::string(del * recIndex, ' ') + "</" + m_element + ">";
                res += (oneLine ? "" : "\n"); 
                break;

            default:
                break;
        }
        return res;
    }

public:
    XMLNode() = default;

    explicit XMLNode(std::string const &str) {
        assign(str);
    }

    explicit XMLNode(XMLNode const &other) : 
        m_tagType(other.m_tagType),
        m_element(other.m_element),
        m_content(other.m_content),
        m_childNodes(other.m_childNodes),
        m_attributes(other.m_attributes) {}

    XMLNode& operator=(std::string const &str) {
        return assign(str);
    }

    XMLNode& operator=(XMLNode const &other) {
        return assign(other);
    }

    XMLNode& assign(std::string const &str) {
        auto res = splitByTags(str)[0];
        return assign(res.first, res.second);
    }

    XMLNode& assign(XMLNode const &other) {
        m_tagType = other.m_tagType;
        m_element = other.m_element;
        m_content = other.m_content;
        m_childNodes = other.m_childNodes;
        m_attributes = other.m_attributes;

        return *this;
    }

    XMLElementType const &getTagType() {
        return m_tagType;
    }
    
    std::string const &getElementName() const {
        if(m_tagType == XMLElementType::PAIRED_TAG || m_tagType == XMLElementType::SINGLE_TAG) {
            return m_element;
        }
        throw TypeError(m_tagType, "Only paired and single tags have names");
    }

    std::string dump(size_t del, bool oneLine = false) const {
        return dumpWithRecIndex(del, oneLine);
    }

    std::unordered_map<std::string, std::string> const &getAttributes() const {
        if(m_tagType == XMLElementType::PAIRED_TAG || m_tagType == XMLElementType::SINGLE_TAG) {
            return m_attributes;
        }
        throw TypeError(m_tagType, "Only paired and single tags have attributes");
    }

    std::vector<XMLNode> const &getChildren() const {
        if(m_tagType == XMLElementType::PAIRED_TAG) {
            return m_childNodes;
        }
        throw TypeError(m_tagType, "Only paired tags have children nodes");
    }

    std::string const &operator[](std::string const &attribute) const {
        if(m_tagType == XMLElementType::PAIRED_TAG || m_tagType == XMLElementType::SINGLE_TAG) {
            return m_attributes.at(attribute);
        }
        throw TypeError(m_tagType, "Only paired and single tags have attributes");
    }

    std::string &operator[](std::string const &attribute) {
        if(m_tagType == XMLElementType::PAIRED_TAG || m_tagType == XMLElementType::SINGLE_TAG) {
            return m_attributes[attribute];
        }
        throw TypeError(m_tagType, "Only paired and single tags have attributes");
    }
    
    XMLNode const &operator[](size_t index) const {
        return m_childNodes[index];
    }

    XMLNode &operator[](size_t index) {
        return m_childNodes[index];
    }

    XMLNode &setElementName(std::string const &str);
    XMLNode &setContent(std::string const &content);
};

std::ostream& operator<<(std::ostream &os, XMLNode const &xml) {
    return os << xml.dump(os.width());
}

std::istream& operator>>(std::istream &is, XMLNode &xml);