//
// Copyright (c) 2010 Advanced Micro Devices, Inc. All rights reserved.
//

// A minimalist OpenCL program.

#include <CL/cl.h>
#include <stdio.h>
#include <string.h>

#include "kernel.h"

#define NWITEMS 512
// A simple memset kernel

int
main(int argc, char **argv)
{
    // 1. Get a platform.
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    // 2. Find a gpu device.
    cl_device_id device;
    cl_uint num_devices;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices);
    printf("num devices=%d\n", num_devices);

    // 3. Create a context and command queue on that device.
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);

    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, 0, NULL);

    // 4. Perform runtime source compilation, and obtain kernel entry point.
    const char *kernel_code = kernel_code_load("kernels/test.cl");
    if (kernel_code == NULL) {
        exit(1);
    }
    cl_program program = clCreateProgramWithSource(context, 1, &kernel_code, NULL, NULL);

    clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    cl_kernel kernel = clCreateKernel(program, "memset", NULL);

    // 5. Create a data buffer.
    cl_mem buffer =
        clCreateBuffer(context, CL_MEM_WRITE_ONLY, NWITEMS * sizeof(cl_uint), NULL, NULL);

    // 6. Launch the kernel. Let OpenCL pick the local work size.
    size_t global_work_size = NWITEMS;
    clSetKernelArg(kernel, 0, sizeof(buffer), (void *)&buffer);

    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);

    clFinish(queue);

    // 7. Look at the results via synchronous buffer map.
    cl_uint *ptr;
    ptr = (cl_uint *)clEnqueueMapBuffer(queue, buffer, CL_TRUE, CL_MAP_READ, 0,
                                        NWITEMS * sizeof(cl_uint), 0, NULL, NULL, NULL);

    int i;

    for (i = 0; i < NWITEMS; i++)
        printf("%d %d\n", i, ptr[i]);

    return 0;
}
