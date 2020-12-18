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
    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;
    map<string, cl::Buffer> bufferMap;

    GPU(cl::Context context, cl::Device &device)
    {
        this->context = context;
        this->device = device;
        this->queue = cl::CommandQueue(context, this->device);
    }

    template <typename... Ts>
    void
    runKernel(cl::KernelFunctor<Ts...> &func, size_t offset, size_t global, size_t local, Ts... ts)
    {
        func(cl::EnqueueArgs(this->queue, cl::NDRange(offset), cl::NDRange(global), cl::NDRange(local)), ts...);
    }

    void
    addBuffer(string name, cl_mem_flags flags, size_t size)
    {
        cl::Buffer buffer(this->context, flags, size);
        bufferMap.insert(make_pair(name, buffer));
    }

    template <typename T>
    void
    addBuffer(string name, cl_mem_flags flags, const vector<T> &vec)
    {
        cl::Buffer buffer(this->context, flags, vec.size() * sizeof(T));
        bufferMap.insert(make_pair(name, buffer));
        this->queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, vec.size() * sizeof(T), vec.data());
    }

    void
    writeBuffer(string name, cl_bool blocking, size_t offset, size_t size, const void *ptr)
    {
        cl::Buffer buffer = bufferMap.find(name)->second;
        this->queue.enqueueWriteBuffer(buffer, blocking, offset, size, ptr);
    }

    void
    readBuffer(string name, cl_bool blocking, size_t offset, size_t size, void *ptr)
    {
        cl::Buffer buffer = bufferMap.find(name)->second;
        this->queue.enqueueReadBuffer(buffer, blocking, offset, size, ptr);
    }

    cl::Buffer
    getBuffer(string name)
    {
        return bufferMap.find(name)->second;
    }

    static vector<cl::Device>
    getAllDevices()
    {
        return GPU::allDevices;
    }

    static void
    initGPUs()
    {
        vector<cl::Platform> allPlatforms;

        cl::Platform::get(&allPlatforms);
        cl::Platform platform;
        for (auto &p : allPlatforms) {
            ::string platver = p.getInfo<CL_PLATFORM_VERSION>();
            printf("%s\n", platver.c_str());
            platform = p;
        }

        platform.getDevices(CL_DEVICE_TYPE_GPU, &allDevices);
        for (auto &d : allDevices) {
            ::string version = d.getInfo<CL_DEVICE_VERSION>();
            cout << version << endl;
        }
    }

    static vector<GPU>
    getGPUs(cl::Context context)
    {
        vector<GPU> gpus;
        for (auto &device : allDevices) {
            gpus.push_back(GPU(context, device));
        }
        return gpus;
    }
};

vector<cl::Device> GPU::allDevices;

} // namespace module