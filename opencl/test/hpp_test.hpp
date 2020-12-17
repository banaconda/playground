#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS // to disable deprecation warnings
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS // to disable deprecation warnings

#include <CL/cl2.hpp>
#include <cstdio>
#include <iostream>
#include <vector>

#include <clblast.h>

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
        for (auto &gpu : gpus) {
            gpu.addBuffer("dst", CL_MEM_WRITE_ONLY, group_size * sizeof(cl_uint));
            gpu.addBuffer("src", CL_MEM_WRITE_ONLY, group_size * sizeof(cl_uint));
        }

        cl_uint h_dst[NUM_WORK_ITEMS] = {
            0,
        };
        cl_uint h_src[NUM_WORK_ITEMS];

        for (int i = 0; i < NUM_WORK_ITEMS; i++) {
            h_src[i] = i + 1;
        }

        printf("running\n");
        size_t index = 0;
        for (auto &gpu : gpus) {
            gpu.writeBuffer("dst", CL_TRUE, 0, group_size * sizeof(cl_uint), &h_dst[index]);
            gpu.writeBuffer("src", CL_TRUE, 0, group_size * sizeof(cl_uint), &h_src[index]);
            index += group_size;
        }

        index = 0;
        for (auto &gpu : gpus) {
            gpu.runKernel<>(k_memcpy, 0, group_size, 1, gpu.getBuffer("dst"), gpu.getBuffer("src"), 0);
            index += group_size;
        }

        index = 0;
        for (auto &gpu : gpus) {
            gpu.readBuffer("dst", CL_TRUE, 0, group_size * sizeof(cl_uint), &h_dst[index]);
            index += group_size;
        }

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


        const size_t m = 128;
        const size_t n = 64;
        const size_t k = 512;
        const float alpha = 0.7f;
        const float beta = 1.0f;
        const auto a_ld = k;
        const auto b_ld = n;
        const auto c_ld = n;

        auto host_a = std::vector<float>(m * k);
        auto host_b = std::vector<float>(n * k);
        auto host_c = std::vector<float>(m * n);
        for (auto &item : host_a) {
            item = 12.193f;
        }
        for (auto &item : host_b) {
            item = -8.199f;
        }
        for (auto &item : host_c) {
            item = 0.0f;
        }

        GPU gpu = gpus[0];
        gpu.addBuffer("a", CL_MEM_READ_WRITE, host_a.size() * sizeof(float));
        gpu.addBuffer("b", CL_MEM_READ_WRITE, host_b.size() * sizeof(float));
        gpu.addBuffer("c", CL_MEM_READ_WRITE, host_c.size() * sizeof(float));
        gpu.writeBuffer("a", CL_TRUE, 0, host_a.size() * sizeof(float), host_a.data());
        gpu.writeBuffer("b", CL_TRUE, 0, host_b.size() * sizeof(float), host_b.data());
        gpu.writeBuffer("c", CL_TRUE, 0, host_c.size() * sizeof(float), host_c.data());

        auto event = cl_event{nullptr};

        auto start_time = std::chrono::steady_clock::now();
        auto status = clblast::Gemm(clblast::Layout::kRowMajor, clblast::Transpose::kNo, clblast::Transpose::kNo, m, n,
                                    k, alpha, gpu.getBuffer("a")(), 0, a_ld, gpu.getBuffer("b")(), 0, b_ld, beta,
                                    gpu.getBuffer("c")(), 0, c_ld, &gpu.queue(), &event);
        if (status == clblast::StatusCode::kSuccess) {
            clWaitForEvents(1, &event);
            clReleaseEvent(event);
        }

        auto elapsed_time = std::chrono::steady_clock::now() - start_time;
        auto time_ms = std::chrono::duration<double, std::milli>(elapsed_time).count();

        // Example completed. See "clblast.h" for status codes (0 -> success).
        printf("Completed SGEMM in %.3lf ms with status %d\n", time_ms, static_cast<int>(status));
    }
};