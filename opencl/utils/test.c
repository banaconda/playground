
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <CL/cl_ext.h>

#define PLATFORM_NUM 2
#define DEVICE_NUM 0
#define PARTITION_COUNT 2
#define LOG_BUFFER_SIZE 100

#define CL_WRAPPER(FUNC) \
    { \
        cl_int err = FUNC; \
        if (err != CL_SUCCESS) { \
            fprintf(stderr, "Error %d executing %s on %s:%d (code = %d)\n", \
                err, #FUNC, __FILE__, __LINE__, err); \
            abort(); \
        }; \
    }

/* OpenCL kernel */
const char* source[] = {
	"#pragma OPENCL EXTENSION cl_intel_printf : enable\n",
	"__kernel void empty_kernel() {\n",
	"	unsigned int idx = get_global_id(0);\n",
	"	printf(\"test\\n\");\n",
	"}\n"
};

int example(int argc, char** argv) {
	cl_int error = 0;
	long long val;

	cl_uint platform_count;
	cl_platform_id* platforms_id;

	cl_uint device_count;
	cl_device_id* devices_id;

	cl_context context;

	cl_uint part_count = PARTITION_COUNT;
	cl_device_id* devices_id_part;
	const cl_context_properties part_props[] = { CL_DEVICE_PARTITION_BY_COUNTS_EXT, 4, 4, CL_PARTITION_BY_COUNTS_LIST_END_EXT, CL_PROPERTIES_LIST_END_EXT };

	cl_program program;
	size_t log_size;
	char log_buffer[LOG_BUFFER_SIZE];
	cl_kernel kernel;

	cl_command_queue queue;

	/* ### Platform */
	/* get platform count */
	CL_WRAPPER(clGetPlatformIDs(0, NULL, &platform_count));
	/* allocate array for platform IDs */
	platforms_id = (cl_platform_id*)malloc(platform_count*sizeof(cl_platform_id));
	/* get platform IDs */
	CL_WRAPPER(clGetPlatformIDs(platform_count, platforms_id, NULL));

	/* ### Device */
	/* get device count for the selected platform */
	CL_WRAPPER(clGetDeviceIDs(platforms_id[PLATFORM_NUM], CL_DEVICE_TYPE_ALL, 0, NULL, &device_count));
	/* allocate array for device IDs */
    devices_id = (cl_device_id*)malloc(device_count*sizeof(cl_device_id));
    /* get device IDs for the selected platform */
	CL_WRAPPER(clGetDeviceIDs(platforms_id[PLATFORM_NUM], CL_DEVICE_TYPE_ALL, device_count, devices_id, NULL));

	clGetDeviceInfo(devices_id[DEVICE_NUM], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(val), &val, NULL);
	printf("Device 0:  max compute units = %lld\n", val );

	/* ### Context */
	cl_context_properties context_props[3];
	context_props[0] = (cl_context_properties)CL_CONTEXT_PLATFORM;      // indicates that next element is platform
    context_props[1] = (cl_context_properties)platforms_id[PLATFORM_NUM];  // platform is of type cl_platform_id
    context_props[2] = (cl_context_properties)0;                        // last element must be 0
	context = clCreateContextFromType(context_props, CL_DEVICE_TYPE_CPU, NULL, NULL, NULL);
	CL_WRAPPER(error);

	/* ### Device partitioning using cl_ext_device_fission extension */
	devices_id_part = (cl_device_id*)malloc(PARTITION_COUNT*sizeof(cl_device_id));
	// CL_WRAPPER(clCreateSubDevicesEXT(devices_id[DEVICE_NUM], part_props, PARTITION_COUNT, devices_id_part, &part_count));

	/* get compute units of the partition and print it */
	CL_WRAPPER(clGetDeviceInfo(devices_id_part[0], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(val), &val, NULL));
	printf("Device partition 0: max compute units = %lld\n", val );

	/* ### Program */
	program = clCreateProgramWithSource(context, 5, source, NULL, &error);
	CL_WRAPPER(error);
	CL_WRAPPER(clBuildProgram(program, 0, NULL, NULL, NULL, NULL));
	CL_WRAPPER(clGetProgramBuildInfo(program, devices_id_part[0], CL_PROGRAM_BUILD_LOG, LOG_BUFFER_SIZE, log_buffer, &log_size));
	printf("%s\n", log_buffer);
	kernel = clCreateKernel(program, "empty_kernel", &error);
	CL_WRAPPER(error);
	CL_WRAPPER(clReleaseProgram(program));

	/* ### Command queue */
	queue = clCreateCommandQueue(context, devices_id_part[0], 0, &error);
	CL_WRAPPER(error);
	size_t global_size[1] = { 64 };
	size_t local_size[1] = { 64 };
	CL_WRAPPER(clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global_size, NULL, 0, NULL, NULL));
	CL_WRAPPER(clFinish(queue));

	return 0;
}
