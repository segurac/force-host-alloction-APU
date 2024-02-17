#define _GNU_SOURCE
#include <dlfcn.h>
#include <sys/types.h>


enum MyHipError_t {

  succes = 0,
};


enum MyHipError_t hipMalloc(void** ptr, size_t size) {
    enum MyHipError_t (*original_hipMalloc)(void** ptr, size_t size) = dlsym(RTLD_NEXT,"hipHostMalloc");
    return original_hipMalloc(ptr, size);
}


enum MyHipError_t hipFree(void* ptr) {
    enum MyHipError_t (*original_hipFree)(void* ptr) = dlsym(RTLD_NEXT,"hipHostFree");
    return original_hipFree(ptr);
}



