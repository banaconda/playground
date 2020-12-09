//
// Copyright (c) 2010 Advanced Micro Devices, Inc. All rights reserved.
//

// A minimalist OpenCL program.

#include <CL/cl.h>
#include <clBLAS.h>
#include <stdio.h>
#include <string.h>

#include "kernel.h"
#include "cl_common.h"

#define NWITEMS 512
// A simple memset kernel


void
test()
{
    // 1. Get a platform.
    cl_platform_id platform;
    cl_uint num_platforms;
    cl_uint num_platform_entries = 1;
    cl_int err = clGetPlatformIDs(num_platform_entries, &platform, &num_platforms);
    printf("platform: num=%d, err=%d\n", num_platforms, err);

    // 2. Find a gpu device.
    cl_device_id devices[5];
    cl_uint num_devices;
    cl_uint num_device_entries = 5;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, num_device_entries, devices, &num_devices);
    printf("device: num=%d, err=%d\n", num_devices, err);

    // 3. Create a context and command queue on that device.
    cl_context_properties *context_properties = NULL;
    void(CL_CALLBACK * pfn_notify)(const char *errinfo, const void *private_info, size_t cb,
                                   void *user_data) = NULL;
    void *user_data = NULL;
    cl_context context =
        clCreateContext(context_properties, num_devices, devices, pfn_notify, user_data, &err);

    cl_queue_properties *queue_properties = NULL;
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, devices[0], queue_properties, &err);

    // 4. Perform runtime source compilation, and obtain kernel entry point.
    const char *kernel_code1 = kernel_code_load("kernels/test.cl");
    const char *kernel_code2 = kernel_code_load("kernels/test2.cl");
    cl_uint count = 1;
    const char *kernel_codes[] = {kernel_code1, kernel_code2};

    // const char **kernel_codes = malloc(sizeof(char *) * 2);
    cl_program program = clCreateProgramWithSource(context, count, kernel_codes, NULL, NULL);
    clBuildProgram(program, num_devices, devices, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "memcpy", &err);
    if (err != CL_SUCCESS) {
        printf("err=%s\n", cl_error_string(err));
    }

    // 5. Create a data buffer.
    cl_mem write_buffer =
        clCreateBuffer(context, CL_MEM_WRITE_ONLY, NWITEMS * sizeof(cl_uint), NULL, NULL);

    cl_mem read_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY,
                                        NWITEMS * sizeof(cl_uint), NULL, NULL);

    cl_uint arr[NWITEMS];
    for (int i = 0; i < NWITEMS; i++) {
        arr[i] = i * 2;
    }
    clEnqueueWriteBuffer(queue, read_buffer, CL_TRUE, 0, NWITEMS * sizeof(cl_uint), arr, 0, NULL,
                         NULL);

    // 6. Launch the kernel. Let OpenCL pick the local work size.
    size_t global_work_size = NWITEMS;
    clSetKernelArg(kernel, 0, sizeof(write_buffer), (void *)&write_buffer);
    clSetKernelArg(kernel, 1, sizeof(read_buffer), (void *)&read_buffer);

    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);

    clFinish(queue);

    // 7. Look at the results via synchronous buffer map.
    cl_uint *ptr;
    ptr = (cl_uint *)clEnqueueMapBuffer(queue, write_buffer, CL_TRUE, CL_MAP_READ, 0,
                                        NWITEMS * sizeof(cl_uint), 0, NULL, NULL, NULL);

    int i;

    for (i = NWITEMS - 10; i < NWITEMS; i++)
    // for (i = 0; i < NWITEMS; i++)
        printf("%d %d\n", i, ptr[i]);
}

static const size_t N = 7;
static cl_float X[] = {
    1,
    2,
    -11,
    17,
    5,
    6,
    81,
};
static const int incx = 1;
static cl_float Y[] = {
    1,
    5,
    6,
    4,
    9,
    10,
    4,
};

static cl_float dotProduct;

int
blas_test()
{
    static const int incy = 1;
    cl_int err;
    cl_platform_id platform = 0;
    cl_device_id device = 0;
    cl_context_properties props[3] = {CL_CONTEXT_PLATFORM, 0, 0};
    cl_context ctx = 0;
    cl_command_queue queue = 0;
    cl_mem bufX, bufY, bufDotP, scratchBuff;
    cl_event event = NULL;
    int ret = 0;
    int lenX = 1 + (N - 1) * abs(incx);
    int lenY = 1 + (N - 1) * abs(incy);
    /* Setup OpenCL environment. */
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err != CL_SUCCESS) {
        printf("clGetPlatformIDs() failed with %d\n", err);
        return 1;
    }
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err != CL_SUCCESS) {
        printf("clGetDeviceIDs() failed with %d\n", err);
        return 1;
    }
    props[1] = (cl_context_properties)platform;
    ctx = clCreateContext(props, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("clCreateContext() failed with %d\n", err);
        return 1;
    }
    queue = clCreateCommandQueueWithProperties(ctx, device, 0, &err);
    if (err != CL_SUCCESS) {
        printf("clCreateCommandQueueWithProperties() failed with %d\n", err);
        clReleaseContext(ctx);
        return 1;
    }
    /* Setup clblas. */
    err = clblasSetup();
    if (err != CL_SUCCESS) {
        printf("clblasSetup() failed with %d\n", err);
        clReleaseCommandQueue(queue);
        clReleaseContext(ctx);
        return 1;
    }
    /* Prepare OpenCL memory objects and place matrices inside them. */
    bufX = clCreateBuffer(ctx, CL_MEM_READ_ONLY, (lenX * sizeof(cl_float)), NULL, &err);
    bufY = clCreateBuffer(ctx, CL_MEM_READ_ONLY, (lenY * sizeof(cl_float)), NULL, &err);
    // Allocate 1 element space for dotProduct
    bufDotP = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, (sizeof(cl_float)), NULL, &err);
    // Allocate minimum of N elements
    scratchBuff = clCreateBuffer(ctx, CL_MEM_READ_WRITE, (N * sizeof(cl_float)), NULL, &err);
    err =
        clEnqueueWriteBuffer(queue, bufX, CL_TRUE, 0, (lenX * sizeof(cl_float)), X, 0, NULL, NULL);
    err =
        clEnqueueWriteBuffer(queue, bufY, CL_TRUE, 0, (lenY * sizeof(cl_float)), Y, 0, NULL, NULL);
    /* Call clblas function. */
    err = clblasSdot(N, bufDotP, 0, bufX, 0, incx, bufY, 0, incy, scratchBuff, 1, &queue, 0, NULL,
                     &event);
    if (err != CL_SUCCESS) {
        printf("clblasSdot() failed with %d\n", err);
        ret = 1;
    } else {
        /* Wait for calculations to be finished. */
        err = clWaitForEvents(1, &event);
        /* Fetch results of calculations from GPU memory. */
        err = clEnqueueReadBuffer(queue, bufDotP, CL_TRUE, 0, sizeof(cl_float), &dotProduct, 0,
                                  NULL, NULL);
        printf("Result dot product: %f\n", dotProduct);
    }
    /* Release OpenCL events. */
    clReleaseEvent(event);
    /* Release OpenCL memory objects. */
    clReleaseMemObject(bufY);
    clReleaseMemObject(bufX);
    clReleaseMemObject(bufDotP);
    clReleaseMemObject(scratchBuff);
    /* Finalize work with clblas. */
    clblasTeardown();
    /* Release OpenCL working objects. */
    clReleaseCommandQueue(queue);
    clReleaseContext(ctx);
    return ret;
}

int
main(int argc, char **argv)
{
    test();
    return 0;
}
