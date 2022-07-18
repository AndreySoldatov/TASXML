#pragma once

#include <iostream>

/**
 * @brief This enum represents three possible XML element states:
 * SINGLE_TAG: "<single_tag/>" 
 * PAIRED_TAG: "<paired_tag></paired_tag>"
 * TEXT: "Text"
 * 
 */
enum class XMLElementType {
    SINGLE_TAG,
    PAIRED_TAG,
    TEXT
};

std::string toString(XMLElementType type) {
    switch (type)
    {
        case XMLElementType::PAIRED_TAG:
            return "XML Paired Tag";
        case XMLElementType::SINGLE_TAG:
            return "XML Single Tag";
        case XMLElementType::TEXT:
            return "XML Text";
        default:
            return {};
    }
}

inline std::ostream &operator<<(std::ostream &os, XMLElementType type) {
    return os << toString(type);
}