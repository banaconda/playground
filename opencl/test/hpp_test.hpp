#include <CL/cl.hpp>
#include <cstdio>
#include <iostream>
#include <vector>

#include "kernel.hpp"

using namespace std;

#define NUM_WORK_ITEMS 512
class HppTest
{
  public:
    HppTest()
    {
        std::vector<cl::Platform> allPlatforms;
        std::vector<cl::Device> allDevices;

        cl::Platform::get(&allPlatforms);
        cl::Platform platform;
        for (auto &p : allPlatforms) {
            std::string platver = p.getInfo<CL_PLATFORM_VERSION>();
            printf("%s\n", platver.c_str());
            platform = p;
        }


        platform.getDevices(CL_DEVICE_TYPE_GPU, &allDevices);
        cl::Device device = allDevices[0];
        for (auto &d : allDevices) {
            std::string version = d.getInfo<CL_DEVICE_VERSION>();
            cout << version << endl;
        }

        cl::Context context({device});
        cl::CommandQueue queue(context, device);

        // KERNEL
        cl::Program::Sources programSources =
            Kernel::getSource(std::vector<std::string>{"kernel/test.cl", "kernel/test2.cl"});

        cl::Program program(context, programSources);
        if (program.build({device}) != CL_SUCCESS) {
            std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
            exit(1);
        }


        // DATA 처리
        std::vector<cl_uint> h_dst(NUM_WORK_ITEMS), h_src(NUM_WORK_ITEMS);
        for (int i = 0; i < NUM_WORK_ITEMS; i++) {
            h_src[i] = i;
        }
        cl::Buffer d_dst(context, CL_MEM_READ_WRITE, sizeof(cl_uint) * NUM_WORK_ITEMS);
        cl::Buffer d_src(context, h_src.begin(), h_src.end(), CL_TRUE);

        cl::Kernel k_memcpy(program, "memcpy");
        k_memcpy.setArg(0, d_dst);
        k_memcpy.setArg(1, d_src);
        queue.enqueueNDRangeKernel(k_memcpy, cl::NullRange, cl::NDRange(NUM_WORK_ITEMS), cl::NullRange);
        queue.finish();

        queue.enqueueReadBuffer(d_dst, CL_TRUE, 0, h_dst.size() * sizeof(cl_uint), h_dst.data());

        for (auto i : h_dst) {
            cout << i << endl;
        }
    }
};