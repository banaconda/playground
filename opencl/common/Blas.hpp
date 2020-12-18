#include <cstdio>
#include <iostream>
#include <vector>
#include <map>

#include <CL/cl2.hpp>
#include <clblast.h>

using namespace std;
namespace blas
{

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

class Vector
{
    public:
    cl::Buffer buffer;
    size_t n;

    Vector(cl::Buffer buffer, size_t n)
    {
        this->buffer = buffer;
        this->n = n;
    }

    template <typename T>
    static string
    toString(vector<T> data, size_t n)
    {
        string str = "";
        for (int i = 0; i < n; i++) {
            str += to_string(data[i]) + " ";
        }
        return str;
    }
};

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
    auto status = clblast::Gemm(clblast::Layout::kRowMajor, clblast::Transpose::kNo, clblast::Transpose::kNo, //
                                m, n, k,                                                                      //
                                alpha,                                                                        //
                                A.buffer(), 0, k,                                                             //
                                B.buffer(), 0, n,                                                             //
                                beta,                                                                         //
                                C.buffer(), 0, n,                                                             //
                                &queue(), &event);

    if (status == clblast::StatusCode::kSuccess) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
    }

    return status;
}

template <typename T>
static clblast::StatusCode
Gemv(cl::CommandQueue &queue, const T alpha, Matrix &A, Vector &X, const T beta, Vector &Y)
{
    size_t m = A.m;
    size_t n = A.n;
    auto event = cl_event{nullptr};
    auto status = clblast::Gemv(clblast::Layout::kRowMajor, clblast::Transpose::kNo, //
                                m, n,                                                //
                                alpha,                                               //
                                A.buffer(), 0, n,                                    //
                                X.buffer(), 0, 1,                                    //
                                beta,                                                //
                                Y.buffer(), 0, 1,                                    //
                                &queue(), &event);

    if (status == clblast::StatusCode::kSuccess) {
        clWaitForEvents(1, &event);
        clReleaseEvent(event);
    }

    return status;
}

} // namespace blas