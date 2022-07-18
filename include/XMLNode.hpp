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
#include <XMLTagType.hpp>

/**
 * @brief This function splits std::string to substrings with assigned XML types:
 * 
 * "Hello<img/>World" -> {"Hello", TEXT}, {<img/>, SINGLE_TAG}, {World, TEXT}
 * 
 * If the splitting cannot be prformed throws ParseError();
 * 
 * @param str 
 * @return std::vector<std::pair<std::string, XMLElementType>> 
 */
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

                size_t indexOfClose{1};
                size_t currentIndex{defStr.find("<" + tagName) + 1};
                while(indexOfClose > 0) {
                    if(defStr.find("<" + tagName, currentIndex) < defStr.find("</" + tagName, currentIndex)) {
                        currentIndex = defStr.find("<" + tagName, currentIndex) + 1;
                        indexOfClose++;
                    }
                    if(defStr.find("</" + tagName, currentIndex) < defStr.find("<" + tagName, currentIndex)) {
                        currentIndex = defStr.find("</" + tagName, currentIndex) + 1;
                        indexOfClose--;
                    }
                }

                auto closeIndex = currentIndex - 1;
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

/**
 * @brief This class represents XML node. XML node is not an XML document, because it lacks all the necessary meta information
 * and cannot handle tags like <!DOCTYPE>. But other than that you can use this class to represent XML documents
 * 
 */
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

                    size_t indexOfClose{1};
                    size_t currentIndex{str.find("<" + m_element) + 1};
                    while(indexOfClose > 0) {
                        if(str.find("<" + m_element, currentIndex) < str.find("</" + m_element, currentIndex)) {
                            currentIndex = str.find("<" + m_element, currentIndex) + 1;
                            indexOfClose++;
                        }
                        if(str.find("</" + m_element, currentIndex) < str.find("<" + m_element, currentIndex)) {
                            currentIndex = str.find("</" + m_element, currentIndex) + 1;
                            indexOfClose--;
                        }
                    }

                    auto vec = splitByTags(str.substr(str.find('>') + 1, currentIndex - 1 - str.find('>') - 1));
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

    std::string dumpWithRecIndex(size_t del, size_t recIndex = 0) const {
        std::string res;
        switch (m_tagType)
        {
            case XMLElementType::TEXT:
                res = std::string(del * recIndex, ' ') + m_content;
                res += (!del ? "" : "\n");
                break;

            case XMLElementType::SINGLE_TAG:
                res = std::string(del * recIndex, ' ') + "<" + m_element + " ";
                for (auto &[k, v] : m_attributes) {
                    res += k + "=\"" + v + "\" ";
                }
                res.erase(res.end() - 1);
                res += "/>";
                res += (!del ? "" : "\n");
                break;

            case XMLElementType::PAIRED_TAG:
                res = std::string(del * recIndex, ' ') + "<" + m_element + " ";
                for (auto &[k, v] : m_attributes) {
                    res += k + "=\"" + v + "\" ";
                }
                res.erase(res.end() - 1);
                res += ">";
                res += (!del ? "" : "\n");
                for (auto &node : m_childNodes) {
                    res += node.dumpWithRecIndex(del, recIndex + 1);
                }
                res += std::string(del * recIndex, ' ') + "</" + m_element + ">";
                res += (!del ? "" : "\n"); 
                break;

            default:
                break;
        }
        return res;
    }

    void clear() {
        m_attributes = {};
        m_childNodes = {};
        m_content = {};
        m_element = {};
        m_tagType = {};
    }

public:
    /**
     * @brief Construct a new empty XMLNode text object
     * 
     */
    XMLNode() {
        m_tagType = XMLElementType::TEXT;
    };

    /**
     * @brief Parses XML node from string
     * 
     * @param str 
     */
    XMLNode(std::string const &str) {
        assign(str);
    }

    /**
     * @brief Copy constructor
     * 
     * @param other 
     */
    explicit XMLNode(XMLNode const &other) : 
        m_tagType(other.m_tagType),
        m_element(other.m_element),
        m_content(other.m_content),
        m_childNodes(other.m_childNodes),
        m_attributes(other.m_attributes) {}
    
    /**
     * @brief Construct a new XMLNode SINGLE_TAG object from string and attribute map
     * 
     * @param name 
     * @param map 
     */
    XMLNode(std::string const &name, std::unordered_map<std::string, std::string> const &map) {
        assign(name, map);
    }

    /**
     * @brief Construct a new XMLNode PAIRED_TAG object from string, attribute map and std::initializer_list of XMLNodes
     * 
     * @param name 
     * @param map 
     * @param nodes 
     */
    XMLNode(
        std::string const &name, 
        std::unordered_map<std::string, std::string> const &map,
        std::initializer_list<XMLNode> const &nodes) 
    {
        assign(name, map, nodes);
    }

    XMLNode& operator=(std::string const &str) {
        return assign(str);
    }

    XMLNode& operator=(XMLNode const &other) {
        return assign(other);
    }

    XMLNode& assign(std::string const &str) {
        clear();
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

    XMLNode& assign(std::string const &name, std::unordered_map<std::string, std::string> const &map) {
        clear();
        m_tagType = XMLElementType::SINGLE_TAG;
        m_element = name;
        m_attributes = map;
        return *this;
    }

    XMLNode& assign(
        std::string const &name, 
        std::unordered_map<std::string, std::string> const &map,
        std::initializer_list<XMLNode> const &nodes) 
    {
        clear();
        m_tagType = XMLElementType::PAIRED_TAG;
        m_element = name;
        m_attributes = map;
        m_childNodes = nodes;

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

    /**
     * @brief Convert XMLNode to the std::string object
     * 
     * @param del the amount of spaces of indent
     * @return std::string 
     */
    std::string dump(size_t del = 0) const {
        return dumpWithRecIndex(del);
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

    std::vector<XMLNode> &getChildren() {
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
        return m_childNodes.at(index);
    }

    XMLNode &operator[](size_t index) {
        return m_childNodes.at(index);
    }

    XMLNode &setElementName(std::string const &str) {
        if(m_tagType == XMLElementType::PAIRED_TAG || m_tagType == XMLElementType::SINGLE_TAG) {
            m_element = str;
        }
        throw TypeError(m_tagType, "Only paired and single tags have names");
    }

    XMLNode &setContent(std::string const &content) {
        if(m_tagType == XMLElementType::TEXT) {
            m_content = content;
        }
        throw TypeError(m_tagType, "Only text tag can have content field");
    }

    std::string const &getContent() {
        if(m_tagType == XMLElementType::TEXT) {
            return m_content;
        }
        throw TypeError(m_tagType, "Only text tag can have content field");
    }

    bool operator==(XMLNode const &other) const {
        if(m_tagType == other.m_tagType) {
            switch (m_tagType)
            {
            case XMLElementType::TEXT:
                return m_content == other.m_content;

            case XMLElementType::SINGLE_TAG:
                return ((m_element == other.m_element) && (m_attributes == other.m_attributes));

            case XMLElementType::PAIRED_TAG:
                return ((m_element == other.m_element) && (m_attributes == other.m_attributes) && (m_childNodes == other.m_childNodes));

            default:
                return false;
            }
        }
        return false;
    }

    XMLNode &operator<<(XMLNode const &other) {
        if(m_tagType == XMLElementType::PAIRED_TAG) {
            m_childNodes.push_back(other);
            return *this;
        }
        throw TypeError(m_tagType, "Cannot push element to a non paired tag");
    }
};

std::ostream& operator<<(std::ostream &os, XMLNode const &xml) {
    return os << xml.dump(os.width());
}

std::istream& operator>>(std::istream &is, XMLNode &xml) {
    std::stringstream ss;
    ss << is.rdbuf();
    xml.assign(ss.str());
    return is;
}