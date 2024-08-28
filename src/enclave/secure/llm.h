typedef char out_char;
#define OUT_SIZE 1024
int llm_inference(char* in_prompt, int in_prompt_len, int n_predict, out_char out_res[OUT_SIZE]);
