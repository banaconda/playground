#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <CL/cl2.hpp>
#include <cstdio>
#include <iostream>
#include <vector>

#include "kernel.hpp"
#include "../common/Module.hpp"

using namespace std;
using namespace module;

#define NUM_WORK_ITEMS 32
class HppTest
{
  public:
    vector<GPU>
    getGPU()
    {
        vector<cl::Platform> allPlatforms;
        vector<cl::Device> allDevices;

        cl::Platform::get(&allPlatforms);
        cl::Platform platform;
        for (auto &p : allPlatforms) {
            ::string platver = p.getInfo<CL_PLATFORM_VERSION>();
            printf("%s\n", platver.c_str());
            platform = p;
        }

        platform.getDevices(CL_DEVICE_TYPE_GPU, &allDevices);
        allDevices.pop_back();
        for (auto &d : allDevices) {
            ::string version = d.getInfo<CL_DEVICE_VERSION>();
            cout << version << endl;
        }

        vector<GPU> gpus;
        for (auto &device : allDevices) {
            gpus.push_back(GPU(platform, device));
        }
        return gpus;
    }

    HppTest()
    {
        vector<GPU> gpus = getGPU();

        cl::Context context(GPU::getAllDevices());

        for (auto &gpu : gpus) {
            gpu.createCommandQueue(context);
        }

        // KERNEL
        cl::Program program =
            Kernel::buildProgram(context, GPU::getAllDevices(), vector<::string>{"kernel/test.cl", "kernel/test2.cl"});


        auto k_memcpy = cl::KernelFunctor<cl::Buffer, cl::Buffer, int>(program, "memcpy");

        size_t group_size = NUM_WORK_ITEMS / gpus.size();

        cl::Buffer d_dst(context, CL_MEM_WRITE_ONLY, NUM_WORK_ITEMS * sizeof(cl_uint));
        cl::Buffer d_src(context, CL_MEM_READ_ONLY, NUM_WORK_ITEMS * sizeof(cl_uint));

        cout << GPU::getAllDevices().size() << endl;

        cl_uint h_dst[NUM_WORK_ITEMS];
        cl_uint h_src[NUM_WORK_ITEMS];

        try {
            gpus[0].queue.enqueueWriteBuffer(d_dst, CL_TRUE, 0, NUM_WORK_ITEMS * sizeof(cl_uint), &h_dst[0]);
            gpus[0].queue.enqueueWriteBuffer(d_src, CL_TRUE, 0, NUM_WORK_ITEMS * sizeof(cl_uint), &h_src[0]);
        } catch (cl::Error err) {
            cout << err.err() << endl;
        }

        k_memcpy(cl::EnqueueArgs(gpus[0].queue, cl::NDRange(0), cl::NDRange(group_size), cl::NDRange(1)), d_dst, d_src, 0);
        // gpus[0].runKernel<>(k_memcpy, 0, group_size, 1, d_dst, d_src, 0);

        // // for (auto device : gpus) {
        //     k_memcpy(cl::EnqueueArgs(device.queue, cl::NDRange(0), cl::NDRange(group_size), cl::NDRange(1)),
        //              device.getBuffer("dst"), device.getBuffer("src"), 0);
        // }

        // // DATA 처리
        // cl_uint h_dst[NUM_WORK_ITEMS] = {0};
        // cl_uint h_src[NUM_WORK_ITEMS];
        // for (int i = 0; i < NUM_WORK_ITEMS; i++) {
        //     h_src[i] = i;
        // }

        // cl::Buffer d_dst(context, CL_MEM_WRITE_ONLY, NUM_WORK_ITEMS * sizeof(cl_uint));
        // cl::Buffer d_src(context, CL_MEM_READ_ONLY, NUM_WORK_ITEMS * sizeof(cl_uint));

        // size_t offset = 0;
        // for (auto queue : queues) {
        //     queue.enqueueWriteBuffer(d_dst, CL_TRUE, 0, NUM_WORK_ITEMS * sizeof(cl_uint), &h_dst[0]);
        //     queue.enqueueWriteBuffer(d_src, CL_TRUE, 0, NUM_WORK_ITEMS * sizeof(cl_uint), &h_src[0]);
        // }

        // offset = 0;
        // for (auto queue : queues){
        //     k_memcpy(cl::EnqueueArgs(queue, cl::NDRange(offset), cl::NDRange(group_size), cl::NDRange(1)), d_dst,
        //     d_src, 0);
        //     // queue.enqueueReadBuffer(d_dst, CL_TRUE, offset * sizeof(cl_uint), group_size * sizeof(cl_uint),
        //     &h_dst[offset]); offset += group_size;
        // }

        // offset = 0;
        // for (auto queue : queues) {
        //     // queue.enqueueReadBuffer(d_dst, CL_TRUE, offset * sizeof(cl_uint), group_size * sizeof(cl_uint),
        //     &h_dst[offset]); queue.enqueueReadBuffer(d_src, CL_TRUE, offset * sizeof(cl_uint), group_size *
        //     sizeof(cl_uint), &h_src[offset]); offset += group_size;
        // }

        // int count = 0;
        // for (auto i : h_dst) {
        //     if (count++ % group_size == 0) {
        //         cout << " | ";
        //     }
        //     cout << i << " ";
        // }
        // cout << " | " << endl;

        // for (auto i : h_src) {
        //     if (count++ % group_size == 0) {
        //         cout << " | ";
        //     }
        //     cout << i << " ";
        // }
        // cout << " | " << endl;

        // cout << endl;
    }
};