#include <stdio.h>
#include <stdlib.h>

#ifdef __linux__
#include <CL/cl.h>
#else
#include <OpenCL/cl.h>
#endif

cl_device_id create_device() {
    cl_platform_id platform;
    cl_device_id deviceId;
    int err;

    err = clGetPlatformIDs(1, &platform, NULL);
    if (err < 0) {
        perror("Couldn't identify a platform");
        exit(1);
    }

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &deviceId, NULL);
    if (err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &deviceId, NULL);
    }
    if (err < 0) {
        perror("Couldn't access any devices");
        exit(1);
    }

    return deviceId;
}

cl_program build_program(cl_context ctx, cl_device_id deviceId, const char* filename) {
    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    int err;

    program_handle = fopen(filename, "r");
    if (program_handle == NULL) {
        perror("Couldn't find the program file");
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char*)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(
        program_buffer,
        sizeof(char),
        program_size,
        program_handle
    );
    fclose(program_handle);

    program = clCreateProgramWithSource(
        ctx,
        1,
        (const char**)&program_buffer,
        &program_size,
        &err
    );
    if(err < 0) {
        perror("Couldn't create the program");
        exit(1);
    }
    free(program_buffer);

    err = clBuildProgram(
        program,
        0,
        NULL,
        NULL,
        NULL,
        NULL
    );
    if (err >= 0) {
        return program;
    }

    clGetProgramBuildInfo(
        program,
        deviceId,
        CL_PROGRAM_BUILD_LOG,
        0,
        NULL,
        &log_size
    );
    program_log = (char*) malloc(log_size + 1);
    program_log[log_size] = '\0';
    clGetProgramBuildInfo(
        program,
        deviceId,
        CL_PROGRAM_BUILD_LOG,
        log_size + 1,
        program_log,
        NULL
    );
    printf("%s\n", program_log);
    free(program_log);
    exit(1);

    return program;
}