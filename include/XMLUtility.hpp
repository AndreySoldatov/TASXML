#pragma once

#include <iostream>

enum class XMLElementType {
    SINGLE_TAG,
    PAIRED_TAG,
    TEXT
};

inline std::ostream &operator<<(std::ostream &os, XMLElementType type) {
    switch (type)
    {
        case XMLElementType::PAIRED_TAG:
            os << "XML Paired Tag";
            break;
        case XMLElementType::SINGLE_TAG:
            os << "XML Single Tag";
            break;
        case XMLElementType::TEXT:
            os << "XML Text";
            break;
        default:
            break;
    }
    return os;
}