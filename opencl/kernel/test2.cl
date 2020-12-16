__kernel void memcpy(__global uint *dst, __global uint *src, int n)
{
    int i = get_global_id(0);
    dst[i] = src[i];
}