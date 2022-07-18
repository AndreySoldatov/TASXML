#pragma once

#include <vector>

#include <XMLNode.hpp>
#include <XMLProlog.hpp>

class XMLDocument {
    XMLProlog m_prolog;
    XMLNode m_nodes;
};