#include "cl_common.h"

const char* cl_error_string(cl_int errCode) {
	const char* err = NULL;
	switch (errCode) {
	case CL_SUCCESS:
		err = "CL_SUCCESS: command is successful, no error!";
		break;
	case CL_DEVICE_NOT_FOUND:
		err = "CL_DEVICE_NOT_FOUND: No match the criteria OpenCL device!";
		break;
	case CL_DEVICE_NOT_AVAILABLE:
		err = "CL_DEVICE_NOT_AVAILABLE: OpenCL device is currently not available!";
		break;
	case CL_COMPILER_NOT_AVAILABLE:
		err = "CL_COMPILER_NOT_AVAILABLE: program created by the source code, but not available for OpenCL C compiler!";
		break;
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		err = "CL_MEM_OBJECT_ALLOCATION_FAILURE: Unable to allocate memory for a memory object or image object!";
		break;
	case CL_OUT_OF_RESOURCES:
		err = "CL_OUT_OF_RESOURCES: There is not enough resources to execute the command!";
		break;
	case CL_OUT_OF_HOST_MEMORY:
		err = "CL_OUT_OF_HOST_MEMORY: There is not enough memory to execute commands on the host!";
		break;
	case CL_PROFILING_INFO_NOT_AVAILABLE:
		err = "CL_PROFILING_INFO_NOT_AVAILABLE: Unable to get information about events or performance evaluation command queue does not support performance evaluation!";
		break;
	case CL_MEM_COPY_OVERLAP:
		err = "CL_MEM_COPY_OVERLAP: two buffers overlap in the same area of ​​memory!";
		break;
	case CL_IMAGE_FORMAT_MISMATCH:
		err = "CL_IMAGE_FORMAT_MISMATCH: images are not the same image format!";
		break;
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
		err = "CL_IMAGE_FORMAT_NOT_SUPPORTED: do not support the specified image format!";
		break;
	case CL_BUILD_PROGRAM_FAILURE:
		err = "CL_BUILD_PROGRAM_FAILURE: you can not build the executable code for the program!";
		break;
	case CL_MAP_FAILURE:
		err = "CL_MAP_FAILURE: memory area can not be mapped to host memory!";
		break;
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:
		err = "CL_MISALIGNED_SUB_BUFFER_OFFSET: sub-buffer offset misalignment!";
		break;
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
		err = "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: waiting list in the event of the implementation status error!";
		break;
	case CL_COMPILE_PROGRAM_FAILURE:
		err = "CL_COMPILE_PROGRAM_FAILURE: compiler failed!";
		break;
	case CL_LINKER_NOT_AVAILABLE:
		err = "CL_LINKER_NOT_AVAILABLE: link not available!";
		break;
	case CL_LINK_PROGRAM_FAILURE:
		err = "CL_LINK_PROGRAM_FAILURE: Link Fails!";
		break;
	case CL_DEVICE_PARTITION_FAILED:
		err = "CL_DEVICE_PARTITION_FAILED: Equipment partition failed!";
		break;
	case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:
		err = "CL_KERNEL_ARG_INFO_NOT_AVAILABLE: kernel parameter information is not available!";
		break;


	case CL_INVALID_VALUE:
		err = "CL_INVALID_VALUE: command of one or more parameters specifying an invalid value!";
		break;
	case CL_INVALID_DEVICE_TYPE:
		err = "CL_INVALID_DEVICE_TYPE: Incoming device type is not a valid value!";
		break;
	case CL_INVALID_PLATFORM:
		err = "CL_INVALID_PLATFORM: Incoming platform is not a valid value!";
		break;
	case CL_INVALID_DEVICE:
		err = "CL_INVALID_DEVICE: Incoming device is not a legal value!";
		break;
	case CL_INVALID_CONTEXT:
		err = "CL_INVALID_CONTEXT: incoming context is not a valid value!";
		break;
	case CL_INVALID_QUEUE_PROPERTIES:
		err = "CL_INVALID_QUEUE_PROPERTIES: device does not support the command queue properties!";
		break;
	case CL_INVALID_COMMAND_QUEUE:
		err = "CL_INVALID_COMMAND_QUEUE: incoming command queue is not a legal value!";
		break;
	case CL_INVALID_HOST_PTR:
		err = "CL_INVALID_HOST_PTR: host pointer illegal!";
		break;
	case CL_INVALID_MEM_OBJECT:
		err = "CL_INVALID_MEM_OBJECT: Incoming memory object is not a valid value!";
		break;
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		err = "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: incoming image format descriptor is not a valid value!";
		break;
	case CL_INVALID_IMAGE_SIZE:
		err = "CL_INVALID_IMAGE_SIZE: device does not support this image size!";
		break;
	case CL_INVALID_SAMPLER:
		err = "CL_INVALID_SAMPLER: Incoming sampling tool is not a valid value!";
		break;
	case CL_INVALID_BINARY:
		err = "CL_INVALID_BINARY: Incoming illegal binaries!";
		break;
	case CL_INVALID_BUILD_OPTIONS:
		err = "CL_INVALID_BUILD_OPTIONS: one or more build options is illegal!";
		break;
	case CL_INVALID_PROGRAM:
		err = "CL_INVALID_PROGRAM: Incoming program is not a valid value!";
		break;
	case CL_INVALID_PROGRAM_EXECUTABLE:
		err = "CL_INVALID_PROGRAM_EXECUTABLE: program fails to successfully build an executable command queue on the associated device!";
		break;
	case CL_INVALID_KERNEL_NAME:
		err = "CL_INVALID_KERNEL_NAME: specified kernel does not exist in the program!";
		break;
	case CL_INVALID_KERNEL_DEFINITION:
		err = "CL_INVALID_KERNEL_DEFINITION: Source code defined in the kernel illegal!";
		break;
	case CL_INVALID_KERNEL:
		err = "CL_INVALID_KERNEL: Incoming kernel is not a legal value!";
		break;
	case CL_INVALID_ARG_INDEX:
		err = "CL_INVALID_ARG_INDEX: Parameter index indicates the kernel for illegal!";
		break;
	case CL_INVALID_ARG_VALUE:
		err = "CL_INVALID_ARG_VALUE: For a non-local parameter kernel parameter value is NULL; or for a partial parameter kernel parameter value is not NULL!";
		break;
	case CL_INVALID_ARG_SIZE:
		err = "CL_INVALID_ARG_SIZE: size parameters and kernel parameters inconsistent!";
		break;
	case CL_INVALID_KERNEL_ARGS:
		err = "CL_INVALID_KERNEL_ARGS: One or more kernel parameters unassigned!";
		break;
	case CL_INVALID_WORK_DIMENSION:
		err = "CL_INVALID_WORK_DIMENSION: Working dimension value is not a value between 1 to 3!";
		break;
	case CL_INVALID_WORK_GROUP_SIZE:
		err = "CL_INVALID_WORK_GROUP_SIZE: local or global work group size is illegal!";
		break;
	case CL_INVALID_WORK_ITEM_SIZE:
		err = "CL_INVALID_WORK_ITEM_SIZE: one or more work item size exceeds the maximum size supported by the device!";
		break;
	case CL_INVALID_GLOBAL_OFFSET:
		err = "CL_INVALID_GLOBAL_OFFSET: Global Offset beyond the boundaries of the supported!";
		break;
	case CL_INVALID_EVENT_WAIT_LIST:
		err = "CL_INVALID_EVENT_WAIT_LIST: waiting list size does not provide legal or contains non-event!";
		break;
	case CL_INVALID_EVENT:
		err = "CL_INVALID_EVENT: incoming event is not a legal value!";
		break;
	case CL_INVALID_OPERATION:
		err = "CL_INVALID_OPERATION: execute command causes an illegal operation appears!";
		break;
	case CL_INVALID_GL_OBJECT:
		err = "CL_INVALID_GL_OBJECT: OpenGL objects referenced problems!";
		break;
	case CL_INVALID_BUFFER_SIZE:
		err = "CL_INVALID_BUFFER_SIZE: buffer size specified bounds!";
		break;
	case CL_INVALID_MIP_LEVEL:
		err = "CL_INVALID_MIP_LEVEL: specified for OpenGL texture mipmap level is not lawful for OpenGL objects!";
		break;
	case CL_INVALID_GLOBAL_WORK_SIZE:
		err = "CL_INVALID_GLOBAL_WORK_SIZE: Incoming global work size is not legitimate, it may be zero or out of the device supports size!";
		break;
	case CL_INVALID_PROPERTY:
		err = "CL_INVALID_PROPERTY: Incoming Invalid property!";
		break;
	case CL_INVALID_IMAGE_DESCRIPTOR:
		err = "CL_INVALID_IMAGE_DESCRIPTOR: Incoming Invalid image descriptor!";
		break;
	case CL_INVALID_COMPILER_OPTIONS:
		err = "CL_INVALID_COMPILER_OPTIONS: Incoming Invalid compiler option!";
		break;
	case CL_INVALID_LINKER_OPTIONS:
		err = "CL_INVALID_LINKER_OPTIONS: Incoming Invalid linker option!";
		break;
	case CL_INVALID_DEVICE_PARTITION_COUNT:
		err = "CL_INVALID_DEVICE_PARTITION_COUNT: Incoming Invalid device partition count!";
		break;
	default:
		err = "unknown mistake";
	}
	return err;
}