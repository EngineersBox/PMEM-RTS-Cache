#include "opencl_defines.cpp"

__kernel void square(__global float* input, __global float* output, int n) {
    int i = get_global_id(0);

    if ((i >= 0) && (i < n)) {
        output[i]=input[i]*input[i];
    }
}