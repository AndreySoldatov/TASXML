#include <iomanip>
#include <XMLNode.hpp>

int main() {
    std::string rawXML(R"(
    <html lol="fdfdd" kek="wabawaba">
        <body>
            <img>  Hello World <lol> This is test sentense sdsd </lol>  dfdf  </img>
        </body>
        <img lol="kek"/>
    </html>
    )");

    XMLNode node(rawXML);

    std::cout << node[1]["lol"] << "\n"; // kek

    std::cout << node[0][0][0].dump(0, true) << "\n"; // Hello World

    std::cout << std::setw(4) << node << "\n"; // All document with 4 spaces indent
}