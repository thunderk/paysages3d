__kernel void simplex_2d(__global const int* level_count,
                    __global const double* levels,
                    __global const double* location,
                    __global double* res)
{
    res[0] = 0.0;
}

__kernel void simplex_3d(__global const int* level_count,
                    __global const double* levels,
                    __global const double* location,
                    __global double* res)
{
    res[0] = 0.0;
}
