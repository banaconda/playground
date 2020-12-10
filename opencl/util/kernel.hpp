#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class Kernel
{
  public:
    static char *
    loadKernelCode(std::string filename)
    {
        std::ifstream ifs;
        ifs.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
        if (ifs.is_open() == false) {
            return NULL;
        }
        std::streampos size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        char *source = new char[size];
        ifs.read(source, size);
        ifs.close();

        return source;
    }

    static cl::Program::Sources
    getSource(std::vector<std::string> filenameList)
    {
        cl::Program::Sources sources;
        for (auto filename : filenameList) {
            char * source = loadKernelCode(filename);
            sources.push_back({source, strlen(source)});
        }
        return sources;
    }
};