#include "TEE-Capability/distributed_tee.h"
#include <sstream>
#include <string>

#include "../secure/llm.h"
#include "file_stub.h"
#define USED(x) ((void)(x))

std::string construct_prompt(std::string user_prompt)
{
    std::stringstream ss;
    // ss << user_prompt;
        ss << R"(<|im_start|>system
You are a helpful assistant.<|im_end|>
<|im_start|>user
)";
        ss << user_prompt;
        ss << R"(<|im_end|>
<|im_start|>assistant
)";

    return ss.str();
}

#define DEFAULT_PROMPT "Who are you?"

int main(int argc, char **argv)
{
    printf("OK\n");
    USED(read_file);
    std::string prompt = construct_prompt(DEFAULT_PROMPT);
    int n_predict = 10;
    if (argc >= 2) {
        prompt = construct_prompt(argv[1]);
    }
    if (argc >= 3) {
        n_predict = atoi(argv[2]);
    }

    /* char user_input[] = "<user_prompt>who are you<model_prompt>\n"; */
    auto ctx = init_distributed_tee_context({.side = SIDE::Client,
                                             .mode = MODE::Transparent,
                                             .name = "llama",
                                             .version = "1.0"});
    size_t size = 10 * n_predict;
    // char *res = new char[size];
    char res[1024];
    printf("%s%s\n", prompt.c_str(), res);
    llm_inference((char *)prompt.c_str(), prompt.size(), n_predict, res);
    printf("%s\n", res);
    destroy_distributed_tee_context(ctx);
}
