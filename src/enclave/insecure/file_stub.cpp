
#ifdef __cplusplus
extern "C" {
#endif
#include "llama_t.h"
#include "enclave.h"
#ifdef __cplusplus
}
#endif
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

extern "C" size_t read_file(size_t f, size_t size, size_t n, char* out_buffer, int out_buffer_len) {
  size_t retval;

  cc_enclave_result_t __Z_res = __insecure_read_file_impl(&retval , f, size, n, out_buffer, out_buffer_len);
  if (__Z_res != CC_SUCCESS) {
    printf("Ecall enclave error\n");
    exit(-1);
  }

  return retval;
}
extern "C" size_t close_file(size_t f) {
  size_t retval;

  cc_enclave_result_t __Z_res = __insecure_close_file_impl(&retval , f);
  if (__Z_res != CC_SUCCESS) {
    printf("Ecall enclave error\n");
    exit(-1);
  }

  return retval;
}
extern "C" size_t open_file(char* filename, int filename_len) {
  size_t retval;

  cc_enclave_result_t __Z_res = __insecure_open_file_impl(&retval , filename, filename_len);
  if (__Z_res != CC_SUCCESS) {
    printf("Ecall enclave error\n");
    exit(-1);
  }

  return retval;
}
extern "C" long tell_file(size_t f) {
  long retval;

  cc_enclave_result_t __Z_res = __insecure_tell_file_impl(&retval , f);
  if (__Z_res != CC_SUCCESS) {
    printf("Ecall enclave error\n");
    exit(-1);
  }

  return retval;
}
extern "C" int seek_file(size_t f, long offset, int whence) {
  int retval;

  cc_enclave_result_t __Z_res = __insecure_seek_file_impl(&retval , f, offset, whence);
  if (__Z_res != CC_SUCCESS) {
    printf("Ecall enclave error\n");
    exit(-1);
  }

  return retval;
}
