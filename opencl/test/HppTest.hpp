#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS // to disable deprecation warnings
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS // to disable deprecation warnings

#include <CL/cl2.hpp>
#include <cstdio>
#include <iostream>
#include <vector>

#include <clblast.h>
#include <clBLAS.h>

#include "Kernel.hpp"
#include "Module.hpp"
#include "Blas.hpp"

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

        blasTest(gpus[0]);
    }

    void
    blasTest(GPU &gpu)
    {

        // const size_t m = 128;
        // const size_t n = 64;
        // const size_t k = 512;
        // const size_t m = 16;
        // const size_t n = 8;
        // const size_t k = 65535;
        // const float alpha = 0.7f;
        // const float beta = 1.0f;
        // const auto a_ld = k;
        // const auto b_ld = n;
        // const auto c_ld = n;

        // auto host_a = std::vector<float>(m * k);
        // auto host_b = std::vector<float>(n * k);
        // auto host_c = std::vector<float>(m * n);
        // for (auto &item : host_a) {
        //     item = 12.193f;
        // }
        // for (auto &item : host_b) {
        //     item = -8.199f;
        // }
        // for (auto &item : host_c) {
        //     item = 0.0f;
        // }

        // gpu.addBuffer("a", CL_MEM_READ_ONLY, host_a);
        // gpu.addBuffer("b", CL_MEM_READ_ONLY, host_b);
        // gpu.addBuffer("c", CL_MEM_READ_WRITE, host_c);

        // blas::Matrix A(gpu.getBuffer("a"), m, k);
        // blas::Matrix B(gpu.getBuffer("b"), k, n);
        // blas::Matrix C(gpu.getBuffer("c"), m, n);

        // auto status = blas::Gemm(gpu.queue, alpha, A, B, beta, C);

        // gpu.readBuffer("c", CL_TRUE, 0, host_c.size() * sizeof(float), host_c.data());

        // cout << blas::Matrix::toString(host_c, m, n);

        size_t m =  3;
        size_t n =  3;

        auto host_A = vector<float>(m * n);
        auto host_X = vector<float>(m);
        auto host_Y = vector<float>(m);
        for (int i = 0; i < m * n; i++) {
            host_A[i] = i + 1;
        }

        for (int i = 0; i < m; i++) {
            host_X[i] = (i + 1) * 2;
        }

        for (auto &item : host_Y) {
            item = 0.0f;
        }

        gpu.addBuffer("A", CL_MEM_READ_WRITE, host_A);
        gpu.addBuffer("X", CL_MEM_READ_WRITE, host_X);
        gpu.addBuffer("Y", CL_MEM_READ_WRITE, host_Y);

        blas::Matrix A(gpu.getBuffer("A"), m, n);
        blas::Vector X(gpu.getBuffer("X"), m);
        blas::Vector Y(gpu.getBuffer("Y"), m);

        auto status = blas::Gemv(gpu.queue, 1.f, A, X, 0.f, Y);

        gpu.readBuffer("Y", host_Y);

        cout << blas::Vector::toString(host_Y, m) << endl;
    }
};