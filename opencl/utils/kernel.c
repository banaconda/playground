#include "kernel.h"

static kernel_code_t *kernel_code_list;
static size_t kernel_code_list_len = 0;

const char *
kernel_code_load(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    kernel_code_list = (kernel_code_t *)realloc(kernel_code_list,
                                                sizeof(kernel_code_t) * (kernel_code_list_len + 1));
    kernel_code_t *kc = &kernel_code_list[kernel_code_list_len];
    kernel_code_list_len++;

    strcpy(kc->filename, filename);
    kc->kernel_code = (char *)malloc(size);
    fread(kc->kernel_code, 1, size, fp);
    return kc->kernel_code;
}
