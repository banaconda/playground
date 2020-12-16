#include <CL/cl2.hpp>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

namespace module
{

class GPU
{
    static vector<cl::Device> allDevices;

  public:
    cl::Platform platform;
    cl::Device device;
    cl::CommandQueue queue;

    GPU(cl::Platform &platform, cl::Device &device)
    {
        this->platform = platform;
        this->device = device;

        GPU::allDevices.push_back(device);
    }

    void
    createCommandQueue(cl::Context &context)
    {
        this->queue = cl::CommandQueue(context, this->device);
    }

    template <typename... Ts>
    void
    runKernel(cl::KernelFunctor<Ts...> &func, size_t offset, size_t global, size_t local, Ts... ts)
    {
        func(cl::EnqueueArgs(this->queue, cl::NDRange(offset), cl::NDRange(global), cl::NDRange(local)), ts...);
    }

    static vector<cl::Device>
    getAllDevices()
    {
        return GPU::allDevices;
    }
};

vector<cl::Device> GPU::allDevices;

} // namespace module