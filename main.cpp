#include <iomanip>
#include <XMLNode.hpp>
#include <fstream>

int main() {
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

    node1 << XMLNode{"secondSingleTag", {{"rer", "rere"}}};
    
    std::ofstream ofs("test.html", std::ios::trunc);
    ofs << std::setw(4) << node1;
    ofs.close();

    XMLNode node;

    std::ifstream ifs("test.html");
    ifs >> node;

    std::cout << std::setw(4) << node;
}