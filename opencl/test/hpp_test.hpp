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
    HppTest()
    {
        GPU::initGPUs();
        cl::Context context(GPU::getAllDevices());
        vector<GPU> gpus = GPU::getGPUs(context);
        gpus.pop_back();

        // KERNEL
        cl::Program program =
            Kernel::buildProgram(context, GPU::getAllDevices(), vector<::string>{"kernel/test.cl", "kernel/test2.cl"});
        auto k_memcpy = cl::KernelFunctor<cl::Buffer, cl::Buffer, int>(program, "memcpy");

        // QUEUE (DATA)
        size_t group_size = NUM_WORK_ITEMS / gpus.size();
        gpus[0].addBuffer("dst", CL_MEM_WRITE_ONLY, NUM_WORK_ITEMS * sizeof(cl_uint));
        gpus[0].addBuffer("src", CL_MEM_WRITE_ONLY, NUM_WORK_ITEMS * sizeof(cl_uint));

        cl_uint h_dst[NUM_WORK_ITEMS] = {
            0,
        };
        cl_uint h_src[NUM_WORK_ITEMS];

        for (int i = 0; i < NUM_WORK_ITEMS; i++) {
            h_src[i] = i + 1;
        }

        gpus[0].writeBuffer("dst", CL_TRUE, 0, NUM_WORK_ITEMS * sizeof(cl_uint), &h_dst[0]);
        gpus[0].writeBuffer("src", CL_TRUE, 0, NUM_WORK_ITEMS * sizeof(cl_uint), &h_src[0]);

        // k_memcpy(cl::EnqueueArgs(gpus[0].queue, cl::NDRange(0), cl::NDRange(group_size), cl::NDRange(1)), d_dst,
        // d_src, 0);
        gpus[0].runKernel<>(k_memcpy, 0, NUM_WORK_ITEMS, 1, gpus[0].getBuffer("dst"), gpus[0].getBuffer("src"), 0);
        gpus[0].readBuffer("dst", CL_TRUE, 0, NUM_WORK_ITEMS * sizeof(cl_uint), &h_dst[0]);

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

        int count = 0;
        for (auto i : h_dst) {
            if (count++ % group_size == 0) {
                cout << " | ";
            }
            printf("%02d ", i);
        }
        cout << " | " << endl;

        for (auto i : h_src) {
            if (count++ % group_size == 0) {
                cout << " | ";
            }
            printf("%02d ", i);
        }
        cout << " | " << endl;

        cout << endl;
    }
};