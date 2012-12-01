#include "opencl.h"

#ifdef HAVE_OPENCL

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <CL/opencl.h>
#include "system.h"

static cl_platform_id _platform;
static cl_device_id _device;
static cl_context _context = NULL;
/* TODO One queue per calling thread ? */
static cl_command_queue _queue = NULL;

static cl_program _noise_program = NULL;
static cl_kernel _noise_kernel_simplex2d = NULL;
static cl_kernel _noise_kernel_simplex3d = NULL;

static const char* _getErrorMessage(cl_int err)
{
    switch (err)
    {
        case CL_SUCCESS: return "Success!";
        case CL_DEVICE_NOT_FOUND: return "Device not found.";
        case CL_DEVICE_NOT_AVAILABLE: return "Device not available";
        case CL_COMPILER_NOT_AVAILABLE: return "Compiler not available";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "Memory object allocation failure";
        case CL_OUT_OF_RESOURCES: return "Out of resources";
        case CL_OUT_OF_HOST_MEMORY: return "Out of host memory";
        case CL_PROFILING_INFO_NOT_AVAILABLE: return "Profiling information not available";
        case CL_MEM_COPY_OVERLAP: return "Memory copy overlap";
        case CL_IMAGE_FORMAT_MISMATCH: return "Image format mismatch";
        case CL_IMAGE_FORMAT_NOT_SUPPORTED: return "Image format not supported";
        case CL_BUILD_PROGRAM_FAILURE: return "Program build failure";
        case CL_MAP_FAILURE: return "Map failure";
        case CL_INVALID_VALUE: return "Invalid value";
        case CL_INVALID_DEVICE_TYPE: return "Invalid device type";
        case CL_INVALID_PLATFORM: return "Invalid platform";
        case CL_INVALID_DEVICE: return "Invalid device";
        case CL_INVALID_CONTEXT: return "Invalid context";
        case CL_INVALID_QUEUE_PROPERTIES: return "Invalid queue properties";
        case CL_INVALID_COMMAND_QUEUE: return "Invalid command queue";
        case CL_INVALID_HOST_PTR: return "Invalid host pointer";
        case CL_INVALID_MEM_OBJECT: return "Invalid memory object";
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "Invalid image format descriptor";
        case CL_INVALID_IMAGE_SIZE: return "Invalid image size";
        case CL_INVALID_SAMPLER: return "Invalid sampler";
        case CL_INVALID_BINARY: return "Invalid binary";
        case CL_INVALID_BUILD_OPTIONS: return "Invalid build options";
        case CL_INVALID_PROGRAM: return "Invalid program";
        case CL_INVALID_PROGRAM_EXECUTABLE: return "Invalid program executable";
        case CL_INVALID_KERNEL_NAME: return "Invalid kernel name";
        case CL_INVALID_KERNEL_DEFINITION: return "Invalid kernel definition";
        case CL_INVALID_KERNEL: return "Invalid kernel";
        case CL_INVALID_ARG_INDEX: return "Invalid argument index";
        case CL_INVALID_ARG_VALUE: return "Invalid argument value";
        case CL_INVALID_ARG_SIZE: return "Invalid argument size";
        case CL_INVALID_KERNEL_ARGS: return "Invalid kernel arguments";
        case CL_INVALID_WORK_DIMENSION: return "Invalid work dimension";
        case CL_INVALID_WORK_GROUP_SIZE: return "Invalid work group size";
        case CL_INVALID_WORK_ITEM_SIZE: return "Invalid work item size";
        case CL_INVALID_GLOBAL_OFFSET: return "Invalid global offset";
        case CL_INVALID_EVENT_WAIT_LIST: return "Invalid event wait list";
        case CL_INVALID_EVENT: return "Invalid event";
        case CL_INVALID_OPERATION: return "Invalid operation";
        case CL_INVALID_GL_OBJECT: return "Invalid OpenGL object";
        case CL_INVALID_BUFFER_SIZE: return "Invalid buffer size";
        case CL_INVALID_MIP_LEVEL: return "Invalid mip-map level";
        default: return "Unknown";
    }
}

static cl_program _loadProgram(const char* path)
{
    cl_int error = 1;
    size_t src_size = systemGetFileSize(path);
    FILE* f = fopen(path, "rb");
    if (!f)
    {
        printf("[OpenCL] Program %s not found !\n", path);
        return NULL;
    }
    else
    {
        cl_program program = NULL;
        char* source = malloc(sizeof (char)* src_size);
        if (fread(source, src_size, 1, f) != 1)
        {
            printf("[OpenCL] Error reading program %s\n", path);
            src_size = 0;
        }
        fclose(f);

        if (src_size > 0)
        {
            program = clCreateProgramWithSource(_context, 1, (const char**)&source, &src_size, &error);
            if (error)
            {
                printf("[OpenCL] Error loading program %s : %s\n", path, _getErrorMessage(error));
                if (program)
                {
                    clReleaseProgram(program);
                    program = NULL;
                }
            }
        }
        free(source);

        if (program)
        {
            error = clBuildProgram(program, 1, &_device, NULL, NULL, NULL);
            if (error)
            {
                char* build_log;
                size_t log_size;

                clGetProgramBuildInfo(program, _device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
                build_log = malloc(sizeof (char)* (log_size + 1));
                clGetProgramBuildInfo(program, _device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
                build_log[log_size] = '\0';
                printf("[OpenCL] Build error for %s :\n%s\n", path, build_log);
                free(build_log);

                clReleaseProgram(program);
                return NULL;
            }
            else
            {
                return program;
            }
        }
        else
        {
            return NULL;
        }
    }
}

static cl_kernel _getkernel(cl_program program, const char* name)
{
    cl_int error;
    cl_kernel result;

    result = clCreateKernel(program, name, &error);
    if (!error)
    {
        return result;
    }
    else
    {
        printf("[OpenCL] Error retrieving kernel %s : %s\n", name, _getErrorMessage(error));
        if (result)
        {
            clReleaseKernel(result);
        }
        return NULL;
    }
}

static size_t _roundUp(int group_size, int global_size)
{
    int r = global_size % group_size;
    if (r == 0)
    {
        return global_size;
    }
    else
    {
        return global_size + group_size - r;
    }
}


void openclInit()
{
    cl_int error;
    cl_uint platform_count;

    /* Get platform */
    error = clGetPlatformIDs(1, &_platform, &platform_count);
    if (error != CL_SUCCESS)
    {
        printf("[OpenCL] Error getting platform id: %s\n", _getErrorMessage(error));
        return;
    }
    else if (platform_count != 1)
    {
        printf("[OpenCL] No platform available\n");
        return;
    }
    /* Get available devices */
    /* TODO Handle several devices */
    error = clGetDeviceIDs(_platform, CL_DEVICE_TYPE_GPU, 1, &_device, NULL);
    if (error != CL_SUCCESS)
    {
        printf("[OpenCL] Error getting devices: %s\n", _getErrorMessage(error));
        return;
    }
    /* Create a context on better device */
    _context = clCreateContext(0, 1, &_device, NULL, NULL, &error);
    if (error != CL_SUCCESS)
    {
        printf("[OpenCL] Error creating context: %s\n", _getErrorMessage(error));
        return;
    }

    /* Preload programs */
    _noise_program = _loadProgram("opencl/noise.cl");
    if (_noise_program)
    {
        _noise_kernel_simplex2d = _getkernel(_noise_program, "simplex_2d");
        _noise_kernel_simplex3d = _getkernel(_noise_program, "simplex_3d");
        if (!_noise_kernel_simplex2d || !_noise_kernel_simplex3d)
        {
            return;
        }
    }
    else
    {
        return;
    }

    /* Create a command queue in this context */
    _queue = clCreateCommandQueue(_context, _device, 0, &error);
    if (error != CL_SUCCESS)
    {
        printf("[OpenCL] Error creating command queue: %s\n", _getErrorMessage(error));
        return;
    }

    printf("OpenCL support is enabled.\n");
}

int openclAvailable()
{
    return _queue != NULL;
}

void openclQuit()
{
    if (_noise_kernel_simplex2d)
    {
        clReleaseKernel(_noise_kernel_simplex2d);
    }
    if (_noise_kernel_simplex3d)
    {
        clReleaseKernel(_noise_kernel_simplex3d);
    }
    if (_noise_program)
    {
        clReleaseProgram(_noise_program);
    }
    if (_queue)
    {
        clReleaseCommandQueue(_queue);
    }
    if (_context)
    {
        clReleaseContext(_context);
    }
}

void openclTest()
{
    cl_int error;
    const int size = 100;
    float src_a_h[size];
    float src_b_h[size];
    int i;

    // Initialize both vectors
    for (i = 0; i < size; i++)
    {
        src_a_h[i] = src_b_h[i] = (float)i;
    }

    const int mem_size = sizeof (float)*size;
    // Allocates a buffer of size mem_size and copies mem_size bytes from src_a_h
    cl_mem src_a_d = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, src_a_h, &error);
    cl_mem src_b_d = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, src_b_h, &error);
    cl_mem res_d = clCreateBuffer(_context, CL_MEM_WRITE_ONLY, mem_size, NULL, &error);

    // Creates the program
    cl_program program = _loadProgram("test.cl");
    if (program)
    {
        // Builds the program
        error = clBuildProgram(program, 1, &_device, NULL, NULL, NULL);
        if (error)
        {
            // Shows the log
            char* build_log;
            size_t log_size;
            // First call to know the proper size
            clGetProgramBuildInfo(program, _device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
            build_log = malloc(sizeof (char)* (log_size + 1));
            // Second call to get the log
            clGetProgramBuildInfo(program, _device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
            build_log[log_size] = '\0';
            printf("[OPENCL] Build log :\n%s\n", build_log);
            free(build_log);
        }
        else
        {
            // Extracting the kernel
            cl_kernel vector_add_k = clCreateKernel(program, "vector_add_gpu", &error);
            if (!error)
            {
                // Enqueuing parameters
                // Note that we inform the size of the cl_mem object, not the size of the memory pointed by it
                error = clSetKernelArg(vector_add_k, 0, sizeof (cl_mem), &src_a_d);
                error |= clSetKernelArg(vector_add_k, 1, sizeof (cl_mem), &src_b_d);
                error |= clSetKernelArg(vector_add_k, 2, sizeof (cl_mem), &res_d);
                error |= clSetKernelArg(vector_add_k, 3, sizeof (size_t), &size);
                assert(error == CL_SUCCESS);

                // Launching kernel
                // TODO Get max number of items in device
                // TODO Lock between the call and the result as we only have one queue
                const size_t local_ws = 64; // Number of work-items per work-group
                const size_t global_ws = _roundUp(local_ws, size); // Total number of work-items
                error = clEnqueueNDRangeKernel(_queue, vector_add_k, 1, NULL, &global_ws, &local_ws, 0, NULL, NULL);
                if (error)
                {
                    printf("[OPENCL] Execution error : %s\n", _getErrorMessage(error));
                }
                else
                {
                    // Reading back
                    float check[size];
                    clEnqueueReadBuffer(_queue, res_d, CL_TRUE, 0, mem_size, check, 0, NULL, NULL);

                    for (i = 0; i < size; i++)
                    {
                        printf("%f %f %f\n", src_a_h[i], src_b_h[i], check[i]);
                    }
                }

                clReleaseKernel(vector_add_k);
            }
        }

        clReleaseProgram(program);
    }

    clReleaseMemObject(src_a_d);
    clReleaseMemObject(src_b_d);
    clReleaseMemObject(res_d);
}
#else
void openclInit()
{
}
int openclAvailable()
{
    return 0;
}
void openclQuit()
{
}
#endif
