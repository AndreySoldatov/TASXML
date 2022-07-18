# TASXML 0.2.1

This is a library for working with XML documents in a modern C++ way.

`developer note:
    this lib is pretty slow now. Rough benchmarks say that
    the file with the size of 5.345MB was read in 3.85 seconds in average on my Intel Core i7-4790 3.60GHz with 4 cores and 8 threads`

### Parsing:

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

### Constructing:

```c++
XMLNode node1 = {
    "PairedTag", 
    {{"first", "second"}},
    {
        {"TextTag"},
        {
            "SingleTag",
            {{"first", "second"}}
        }
    }
};
```

### Writing to streams:

```c++
XMLNode node1;

std::ofstream ofs("test.html", std::ios::trunc);
ofs << std::setw(4) << node1;
ofs.close();

XMLNode node;

std::ifstream ifs("test.html");
ifs >> node;

std::cout << std::setw(4) << node;
```