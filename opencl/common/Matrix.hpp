#include <cstdio>
#include <iostream>
#include <vector>
#include <map>

#include <CL/cl2.hpp>
#include <clblast.h>

// ld = leading dimesion(col size)

// StatusCode Gemm(const Layout layout, const Transpose a_transpose, const Transpose b_transpose,
//                 const size_t m, const size_t n, const size_t k,
//                 const T alpha,
//                 const cl_mem a_buffer, const size_t a_offset, const size_t a_ld,
//                 const cl_mem b_buffer, const size_t b_offset, const size_t b_ld,
//                 const T beta,
//                 cl_mem c_buffer, const size_t c_offset, const size_t c_ld,
//                 cl_command_queue* queue, cl_event* event = nullptr,
//                 cl_mem temp_buffer = nullptr);

using namespace std;
class Matrix
{
  public:
    cl::Buffer buffer;
    size_t m, n;

    Matrix(cl::Buffer buffer, size_t m, size_t n)
    {
        this->buffer = buffer;
        this->m = m;
        this->n = n;
    }

    template <typename T>
    static clblast::StatusCode
    Gemm(cl::CommandQueue &queue, const T alpha, Matrix &A, Matrix &B, const T beta, Matrix &C)
    {
        if (A.m != C.m || B.n != C.n || A.n != B.m) {
            return clblast::StatusCode::kInvalidDimension;
        }

        size_t m = C.m;
        size_t n = C.n;
        size_t k = A.n;

        auto event = cl_event{nullptr};
        auto status =
            clblast::Gemm(clblast::Layout::kRowMajor, clblast::Transpose::kNo, clblast::Transpose::kNo, m, n, k,
                          alpha,            //
                          A.buffer(), 0, k, //
                          B.buffer(), 0, n, //
                          beta,             //
                          C.buffer(), 0, n, //
                          &queue(), &event);

        if (status == clblast::StatusCode::kSuccess) {
            clWaitForEvents(1, &event);
            clReleaseEvent(event);
        }

        return status;
    }

    template <typename T>
    static string
    toString(vector<T> data, size_t m, size_t n)
    {
        string str = "";
        size_t len = m * n;
        for (int i = 0; i < len; i++) {
            str += to_string(data[i]) + " ";
            if (i % n == n - 1) {
                str += '\n';
            }
        }
        return str;
    }
};