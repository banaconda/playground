#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class Kernel
{
    static char *
    loadKernelCode(string filename)
    {
        ifstream ifs;
        ifs.open(filename, ios::in | ios::binary | ios::ate);
        if (ifs.is_open() == false) {
            return NULL;
        }
        streampos size = ifs.tellg();
        ifs.seekg(0, ios::beg);
        char *source = new char[size];
        ifs.read(source, size);
        ifs.close();

        return source;
    }

    static cl::Program::Sources
    getSource(vector<string> filenameList)
    {
        cl::Program::Sources sources;
        for (auto filename : filenameList) {
            char *source = loadKernelCode(filename);
            sources.push_back({source, strlen(source)});
        }
        return sources;
    }

  public:
    static cl::Program
    buildProgram(cl::Context &context, vector<cl::Device> allDevices, vector<string> filenameList)
    {
        cl::Program::Sources programSources = Kernel::getSource(filenameList);

        cl::Program program(context, programSources);
        try {
            program.build(allDevices);
        } catch (...) {
            for (auto &device : allDevices) {
                cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << endl;
            }
        }

        return program;
    }
};