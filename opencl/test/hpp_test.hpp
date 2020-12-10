#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <CL/cl2.hpp>
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
        vector<cl::CommandQueue> queues;
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});
        queues.push_back(cl::CommandQueue{context, device});

        // KERNEL
        cl::Program::Sources programSources =
            Kernel::getSource(std::vector<std::string>{"kernel/test.cl", "kernel/test2.cl"});

        cl::Program program(context, programSources);
        if (program.build({device}) != CL_SUCCESS) {
            std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
            exit(1);
        }

        // DATA 처리
        cl_uint h_dst[NUM_WORK_ITEMS];
        cl_uint h_src[NUM_WORK_ITEMS];
        for (int i = 0; i < NUM_WORK_ITEMS; i++) {
            h_src[i] = i;
        }

        cl::Buffer d_dst(context, CL_MEM_WRITE_ONLY, sizeof(cl_uint) * NUM_WORK_ITEMS);
        cl::Buffer d_src(context, CL_MEM_READ_ONLY, sizeof(cl_uint) * NUM_WORK_ITEMS);
        size_t group_count = NUM_WORK_ITEMS / queues.size();
        size_t offset = 0;
        for (auto queue : queues) {
            queue.enqueueWriteBuffer(d_src, CL_TRUE, offset, group_count * sizeof(cl_uint), &h_src[offset]);
            offset += group_count;
        }

        // KERNEL
        // KernelFunctor version
        auto k_memcpy = cl::KernelFunctor<cl::Buffer, cl::Buffer>(program, "memcpy");

        offset = 0;
        for (auto queue : queues) {
            k_memcpy(cl::EnqueueArgs(queue, cl::NDRange(offset), cl::NDRange(group_count), cl::NullRange), d_dst,
                     d_src);
            offset += group_count;
        }

        offset = 0;
        for (auto queue : queues) {
            queue.enqueueReadBuffer(d_dst, CL_TRUE, offset, group_count * sizeof(cl_uint), &h_dst[offset]);
            offset += group_count;
        }

        for (auto i : h_dst) {
            cout << i << endl;
        }
    }
};