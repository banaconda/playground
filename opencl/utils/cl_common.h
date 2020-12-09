#ifndef __CL_COMMON_H__
#define __CL_COMMON_H__

#include <CL/cl.h>

#define CL_WRAP(func, ...)                                                                                             \
    {                                                                                                                  \
        cl_int err = func(__VA_ARGS__);                                                                                \
        if (err != CL_SUCCESS) {                                                                                       \
            printf("err=%s\n", cl_error_string(err));                                                                  \
            exit(err);                                                                                                 \
        }                                                                                                              \
    }

#define CL_WRAP_DECL(t, ret, func, ...)                                                                                \
    t ret;                                                                                                             \
    {                                                                                                                  \
        cl_int err;                                                                                                    \
        ret = func(__VA_ARGS__, &err);                                                                                 \
        if (err != CL_SUCCESS) {                                                                                       \
            printf("err=%s\n", cl_error_string(err));                                                                  \
            exit(err);                                                                                                 \
        }                                                                                                              \
    }

#define CL_WRAP_ASSIGN(ret, func, ...)                                                                                 \
    {                                                                                                                  \
        cl_int err;                                                                                                    \
        ret = func(__VA_ARGS__, &err);                                                                                 \
        if (err != CL_SUCCESS) {                                                                                       \
            printf("err=%s\n", cl_error_string(err));                                                                  \
            exit(err);                                                                                                 \
        }                                                                                                              \
    }

const char *cl_error_string(cl_int errCode);

#endif // __CL_COMMON_H__