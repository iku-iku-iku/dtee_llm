

#ifdef __cplusplus
extern "C" {
#endif
#include "llama_u.h"
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

extern cc_enclave_t *g_enclave_context;
extern void z_create_enclave(const char*, bool is_proxy);
extern void z_destroy_enclave();

#ifdef __cplusplus
}
#endif

int llm_inference(char* in_prompt, int in_prompt_len, int n_predict, char* out_res) {
  int retval;

  z_create_enclave("enclave.signed.so", false);

  cc_enclave_result_t __Z_res = __secure_llm_inference_impl(g_enclave_context, &retval , in_prompt, in_prompt_len, n_predict, out_res);
  if (__Z_res != CC_SUCCESS) {
    printf("Ecall enclave error\n");
    exit(-1);
  } 

  z_destroy_enclave();

  return retval;
}

