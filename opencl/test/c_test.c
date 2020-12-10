
#include "../util/cl_common.h"
#include "../util/kernel.h"

#define NUM_WORK_ITEMS 512

void
test()
{
    cl_int err;

    // 1. PLATFORM
    // Get platform count
    cl_platform_id *platforms;
    cl_uint num_platforms;
    CL_WRAP(clGetPlatformIDs, 0, NULL, &num_platforms);
    printf("platform: num=%d, err=%d\n", num_platforms, err);

    // Alloc count of platforms
    platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id) * num_platforms);

    // Get platforms
    CL_WRAP(clGetPlatformIDs, num_platforms, platforms, NULL);

    // 2. DEVICE
    // Get device count
    cl_device_id *devices;
    cl_uint num_devices;
    CL_WRAP(clGetDeviceIDs, platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
    printf("device: num=%d, err=%d\n", num_devices, err);

    // Alloc count of devices
    devices = (cl_device_id *)malloc(sizeof(cl_device_id) * num_devices);

    // Get devices
    CL_WRAP(clGetDeviceIDs, platforms[0], CL_DEVICE_TYPE_GPU, num_devices, devices, NULL);
    for (int i = 0; i < num_devices; i++) {
        long long compute_unit;
        CL_WRAP(clGetDeviceInfo, devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_unit), &compute_unit, NULL);
        printf("Device[%d] max compute units = %lld\n", i, compute_unit);

        char driver_version[256];
        CL_WRAP(clGetDeviceInfo, devices[i], CL_DRIVER_VERSION, sizeof(driver_version), &driver_version, NULL);
        printf("Driver Version %s\n", driver_version);

        char device_version[256];
        CL_WRAP(clGetDeviceInfo, devices[i], CL_DEVICE_VERSION, sizeof(device_version), &device_version, NULL);
        printf("Device Version %s\n", device_version);
    }

    // 3. CONTEXT
    cl_context_properties context_properties[] = {
        CL_CONTEXT_PLATFORM,                 //
        (cl_context_properties)platforms[0], //
        0                                    //
    };
    void(CL_CALLBACK * pfn_notify)(const char *errinfo, const void *private_info, size_t cb, void *user_data) = NULL;
    void *user_data = NULL;
    cl_context context =
        CL_WRAP_AUTO(clCreateContext, context_properties, num_devices, devices, pfn_notify, user_data);
    // cl_context context =
    //     CL_WRAP_AUTO(clCreateContextFromType, context_properties, CL_DEVICE_TYPE_GPU, pfn_notify, user_data);

    // 4. PROGRAM
    const char *kernel_codes[] = {
        kernel_code_load("kernel/test.cl"), //
        kernel_code_load("kernel/test2.cl") //
    };
    cl_uint count = sizeof(kernel_codes) / sizeof(char *);
    cl_program program = CL_WRAP_AUTO(clCreateProgramWithSource, context, count, kernel_codes, NULL);
    CL_WRAP(clBuildProgram, program, num_devices, devices, NULL, NULL, NULL);
    cl_kernel kernel = CL_WRAP_AUTO(clCreateKernel, program, "memcpy");

    // 5. QUEUE
    cl_queue_properties *queue_properties = NULL;
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, devices[0], queue_properties, &err);

    // 6. SET BUFFER
    // Create buffer
    cl_mem write_buffer = CL_WRAP_AUTO(clCreateBuffer, context, CL_MEM_WRITE_ONLY, NUM_WORK_ITEMS * sizeof(cl_uint), NULL);
    cl_mem read_buffer = CL_WRAP_AUTO(clCreateBuffer, context, CL_MEM_READ_ONLY, NUM_WORK_ITEMS * sizeof(cl_uint), NULL);

    // Copy data to device from host
    cl_uint arr[NUM_WORK_ITEMS];
    for (int i = 0; i < NUM_WORK_ITEMS; i++) {
        arr[i] = i * 2;
    }
    CL_WRAP(clEnqueueWriteBuffer, queue, read_buffer, CL_TRUE, 0, NUM_WORK_ITEMS * sizeof(cl_uint), arr, 0, NULL, NULL);

    // 7. Launch the kernel. Let OpenCL pick the local work size.
    size_t global_work_size = NUM_WORK_ITEMS;
    CL_WRAP(clSetKernelArg, kernel, 0, sizeof(write_buffer), (void *)&write_buffer);
    CL_WRAP(clSetKernelArg, kernel, 1, sizeof(read_buffer), (void *)&read_buffer);
    CL_WRAP(clEnqueueNDRangeKernel, queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);

    // 8. FINISH
    // Wait for finishing
    CL_WRAP(clFinish, queue);
    printf("finish\n");

    // 9. GET BUFFER
    cl_uint *ptr = (cl_uint *)CL_WRAP_AUTO(clEnqueueMapBuffer, queue, write_buffer, CL_TRUE, CL_MAP_READ, 0,
                                           NUM_WORK_ITEMS * sizeof(cl_uint), 0, NULL, NULL);

    int i;
    for (i = NUM_WORK_ITEMS - 10; i < NUM_WORK_ITEMS; i++)
        printf("%d %d\n", i, ptr[i]);
}