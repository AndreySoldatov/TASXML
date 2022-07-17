# TASXML 0.1.0

This is a library for working with XML documents in a modern C++ way.

### Example:

```c++
#include <iomanip>
#include <XMLNode.hpp>

int main() {
    XMLNode node(R"(
    <html lol="fdfdd" kek="wabawaba">
        <body>
            <img>  Hello World <lol> This is test sentense sdsd </lol>  dfdf  </img>
        </body>
        <img lol="kek"/>
    </html>
    )");

    std::cout << node[1]["lol"] << "\n"; // kek

    std::cout << node[0][0][0] << "\n"; // Hello World

    std::cout << std::setw(4) << node << "\n"; // All document with 4 spaces indent
}
```