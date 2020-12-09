kernel void memset(global uint *dst)
{
    dst[get_global_id(0)] = get_global_id(0);
}

kernel void memcpy(global uint *dst, global uint *src)
{
    dst[get_global_id(0)] = src[get_global_id(0)];
}