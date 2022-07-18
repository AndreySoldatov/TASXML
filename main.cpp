#include <iomanip>
#include <XMLNode.hpp>
#include <XMLProlog.hpp>
#include <XMLDoctype.hpp>
#include <fstream>

#include <json.hpp>

#include <chrono>

namespace ch = std::chrono;

int main() {
    std::cout << "start: \n";
    auto startTime = ch::system_clock::now();

    XMLNode node;

    std::ifstream ifs("test.xml");

    ifs >> node;

    std::cout << "end: " << (ch::duration_cast<ch::milliseconds>(ch::system_clock::now() - startTime)).count() << "\n";

    ifs.close();

    std::cout << node[3]["content-type"] << "\n";

    std::ofstream ofs("pretty.xml", std::ios::trunc);

    ofs << std::setw(4) << node;
}