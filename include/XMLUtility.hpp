#pragma once

#include <iostream>

#include <XMLErrors.hpp>

/**
 * @brief This function clears XML comments from string: "<!-- This is a comment -->"
 * 
 * If comment is not  closed throws ParseError()
 * 
 * @param str 
 */
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

/**
 * @brief This function clears all of the whitespace characters (" ", "\t", "\n") excluding necessary
 * 
 * @param str 
 * @return std::string 
 */
inline std::string deflate(std::string const &str) {
    // Removing all new line characters
    std::string res(str);
    res.erase(std::remove(res.begin(), res.end(), '\n'), res.end());
    res.erase(std::remove(res.begin(), res.end(), '\t'), res.end());

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

/**
 * @brief This function takes string, and spits it on the std::unordered_map of attributes
 * 
 * if Fails throws ParseError()
 * 
 * @param str 
 * @return std::unordered_map<std::string, std::string> 
 */
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

std::string toLowerCase(std::string const &str) {
    std::string res;
    std::transform(str.begin(), str.end(), std::back_inserter(res), [](char const &c) {
        return std::tolower(c);
    });
    return res;
}