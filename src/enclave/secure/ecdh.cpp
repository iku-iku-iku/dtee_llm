


#ifdef __cplusplus
extern "C" {
#endif
#include "llama_t.h"
#ifdef __cplusplus
}
#endif

#define key_exchange __secure_key_exchange_impl

#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include "TEE-Capability/common.h"
#define PRIVATE_KEY_SIZE 32
#define PUBLIC_KEY_SIZE 64
#define HASH_SIZE 32
#define SIGNATURE_SIZE 64

#define NONCE 12345
#define ROUND_TO(x, align) (((x) + ((align)-1)) & ~((align)-1))
extern "C"
{
#include <miracl/miracl.h>
#include <miracl/mirdef.h>
#include <SM4_Enc.h>
    extern std::vector<char> (*get_report_func)(const char *enclave_path);
    extern bool (*is_report_valid_func)(void* report, const char *enclave_path);
    extern std::string (*key_exchange_func)(std::string in_pub_key);
    extern std::pair<std::string, std::string> (*make_key_pair_func)();
    extern std::string (*make_shared_key_func)(std::string pri_key, std::string in_pub_key);
#define SM2_WORDSIZE 8
#define SM2_NUMBITS 256
#define SM2_NUMWORD (SM2_NUMBITS / SM2_WORDSIZE)
}
extern "C"
{
    typedef struct
    {
        unsigned int state[8];
        unsigned int length;
        unsigned int curlen;
        unsigned char buf[64];
    } SM3_STATE;
    void SM3_init(SM3_STATE * md);
    void SM3_process(SM3_STATE * md, unsigned char buf[], int len);
    void SM3_done(SM3_STATE * md, unsigned char* hash);
}

const char SM2_p[32] = {0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff,
                        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                        0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
                        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

const char SM2_a[32] = {0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff,
                        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                        0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
                        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc};

const char SM2_b[32] = {0x28, 0xe9, 0xfa, 0x9e, 0x9d, 0x9f, 0x5e, 0x34,
                        0x4d, 0x5a, 0x9e, 0x4b, 0xcf, 0x65, 0x09, 0xa7,
                        0xf3, 0x97, 0x89, 0xf5, 0x15, 0xab, 0x8f, 0x92,
                        0xdd, 0xbc, 0xbd, 0x41, 0x4d, 0x94, 0x0e, 0x93};

const char SM2_Gx[32] = {0x32, 0xc4, 0xae, 0x2c, 0x1f, 0x19, 0x81, 0x19,
                         0x5f, 0x99, 0x04, 0x46, 0x6a, 0x39, 0xc9, 0x94,
                         0x8f, 0xe3, 0x0b, 0xbf, 0xf2, 0x66, 0x0b, 0xe1,
                         0x71, 0x5a, 0x45, 0x89, 0x33, 0x4c, 0x74, 0xc7};

const char SM2_Gy[32] = {0xbc, 0x37, 0x36, 0xa2, 0xf4, 0xf6, 0x77, 0x9c,
                         0x59, 0xbd, 0xce, 0xe3, 0x6b, 0x69, 0x21, 0x53,
                         0xd0, 0xa9, 0x87, 0x7c, 0xc6, 0x2a, 0x47, 0x40,
                         0x02, 0xdf, 0x32, 0xe5, 0x21, 0x39, 0xf0, 0xa0};

const char SM2_n[32] = {0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff,
                        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                        0x72, 0x03, 0xdf, 0x6b, 0x21, 0xc6, 0x05, 0x2b,
                        0x53, 0xbb, 0xf4, 0x09, 0x39, 0xd5, 0x41, 0x23};

void str_to_point(std::string str, epoint *a)
{
    char mem[MR_BIG_RESERVE(2)] = {0};
    big x, y;
    x = mirvar_mem(mem, 0);
    y = mirvar_mem(mem, 1);
    bytes_to_big(SM2_NUMWORD, str.c_str(), x);
    bytes_to_big(SM2_NUMWORD, str.c_str() + SM2_NUMWORD, y);

    epoint_set(x, y, 0, a);
}

std::string point_to_str(epoint *a)
{
    char mem[MR_BIG_RESERVE(2)] = {0};
    big x, y;
    x = mirvar_mem(mem, 0);
    y = mirvar_mem(mem, 1);

    epoint_get(a, x, y);

    char buffer[2 * SM2_NUMWORD];
    big_to_bytes(SM2_NUMWORD, x, buffer, 1);

    big_to_bytes(SM2_NUMWORD, y, buffer + SM2_NUMWORD, 1);

    return std::string(buffer, buffer + 2 * SM2_NUMWORD);
}

std::string get_key(big b) {
    char buffer[SM2_NUMWORD];
    big_to_bytes(SM2_NUMWORD, b, buffer, 1);
    return std::string(buffer, buffer + SM2_NUMWORD);
};


std::pair<std::string, std::string> make_key_pair()
{
    miracl *mip = mirsys(128, 16);

    char mem[MR_BIG_RESERVE(7)] = {0};
    char mem_point[MR_ECP_RESERVE(2)] = {0};

    epoint *g, *pub;
    big p, a, b, n, gx, gy, pri;
    p = mirvar_mem(mem, 0);
    a = mirvar_mem(mem, 1);
    b = mirvar_mem(mem, 2);
    n = mirvar_mem(mem, 3);
    gx = mirvar_mem(mem, 4);
    gy = mirvar_mem(mem, 5);
    pri = mirvar_mem(mem, 6);

    bytes_to_big(SM2_NUMWORD, SM2_a, a);
    bytes_to_big(SM2_NUMWORD, SM2_b, b);
    bytes_to_big(SM2_NUMWORD, SM2_p, p);
    bytes_to_big(SM2_NUMWORD, SM2_Gx, gx);
    bytes_to_big(SM2_NUMWORD, SM2_Gy, gy);
    bytes_to_big(SM2_NUMWORD, SM2_n, n);

    g = epoint_init_mem(mem_point, 0);

    pub = epoint_init_mem(mem_point, 1);
    ecurve_init(a, b, p, MR_PROJECTIVE);

    if (!epoint_set(gx, gy, 0, g)) {
        {};
    }

    auto make_prikey = [n](big x) {
        bigrand(n, x);  // generate a big random number 0<=x<n
    };

    make_prikey(pri);
    ecurve_mult(pri, g, pub);

    auto res = std::make_pair(get_key(pri), point_to_str(pub));
    mirexit();
    return res;
}

void _Z_encrypt(const unsigned char* key, unsigned char* buf, int buf_len)
{
    unsigned char iv[16] = {0};
    SM4_CBC_Encrypt(key, iv, buf, buf_len, buf, buf_len);
}

void _Z_decrypt(const unsigned char* key, unsigned char* buf, int buf_len)
{
    unsigned char iv[16] = {0};
    SM4_CBC_Decrypt(key, iv, buf, buf_len, buf, buf_len);
}

#ifdef __cplusplus
extern "C" {
#endif
    // ENCRYPTED_DATA | OUT_LEN(4B) | SEALED_KEY | SEALED_KEY_LEN(4B) 
    int __decrypt_in_enclave(char* buf, int buf_len, const char* func_name) {
        if (strcmp(func_name, "ecall___secure_key_exchange_impl") == 0) {
            return buf_len;
        }
        char *p_buf = buf + buf_len;

        p_buf -= 4;
        int sealed_key_len = *(int*)p_buf;
        if (sealed_key_len == 0) {
            return buf_len;
        }

        p_buf -= sealed_key_len;
        char* key_buf = p_buf;
        unseal_data_inplace(key_buf, sealed_key_len);
                        for (int i = 0; i < 64; i++) {
                            eapp_print("%x\n",
                                   (unsigned char)key_buf[i]);
                        }
        
        p_buf -= 4;
        int origin_data_len = *(int*)p_buf;
        

        _Z_decrypt((const unsigned char*)key_buf, (unsigned char*)buf, (origin_data_len + 15) / 16 * 16);
        return origin_data_len;
    }

    // ENCRYPTED_DATA | OUT_LEN(4B) | SEALED_KEY | SEALED_KEY_LEN(4B) | DECRYPTED(1B)
    void __encrypt_in_enclave(char* buf, int buf_len, const char* func_name) {
        if (strcmp(func_name, "ecall___secure_key_exchange_impl") == 0) return;
        char *p_buf = buf + buf_len;

        bool decrypted = *(char*)--p_buf;
        if (decrypted) return;

        p_buf -= 4;
        int sealed_key_len = *(int*)p_buf;

        p_buf -= sealed_key_len;
        char *key_buf = p_buf;
        unseal_data_inplace(key_buf, sealed_key_len);

        p_buf -= 4;
        int out_len = *(int*)p_buf;
        eapp_print("OUT LEN: %d\n", out_len);

        _Z_encrypt((const unsigned char*)key_buf, (unsigned char*)buf, (out_len + 15) / 16 * 16);

        *(int*)(buf + buf_len - 4 - 1) = out_len;
    }
#ifdef __cplusplus
}
#endif

std::string make_shared_key(std::string pri_key, std::string in_pub_key)
{
    miracl *mip = mirsys(128, 16);
    char mem[MR_BIG_RESERVE(4)] __attribute__((aligned(16)));
    char mem_point[MR_ECP_RESERVE(2)] __attribute__((aligned(16)));
    memset(mem, 0, sizeof(mem));
    memset(mem_point, 0, sizeof(mem_point));

    big pri, a, b, p;
    pri = mirvar_mem(mem, 0);
    a = mirvar_mem(mem, 1);
    b = mirvar_mem(mem, 2);
    p = mirvar_mem(mem, 3);
    bytes_to_big(SM2_NUMWORD, SM2_a, a);
    bytes_to_big(SM2_NUMWORD, SM2_b, b);
    bytes_to_big(SM2_NUMWORD, SM2_p, p);
    ecurve_init(a, b, p, MR_PROJECTIVE);

    epoint *pub = epoint_init_mem(mem_point, 0);
    str_to_point(std::move(in_pub_key), pub);

    epoint *shared = epoint_init_mem(mem_point, 1);
    bytes_to_big(SM2_NUMWORD, pri_key.c_str(), pri);
    ecurve_mult(pri, pub, shared);

    auto res = point_to_str(shared);
    mirexit();
    return res;
}

extern "C" {
    extern int GenerateKeyPairAndSignature(uintptr_t pri_key_ptr, uintptr_t pub_key_ptr, uintptr_t sig_ptr);
}

int key_exchange(char* in_key, int in_key_len, char* out_key, int out_key_len, char* out_encrypted_shared_key, int out_encrypted_shared_key_len, char* out_key_signature, int out_key_signature_len) {
    char pri_key[32];
    char pub_key[64];
    char signature[64];
    memset(pri_key, 0, sizeof(pri_key));
    memset(pub_key, 0, sizeof(pub_key));
    memset(signature, 0, sizeof(signature));
    (void)GenerateKeyPairAndSignature((uintptr_t)pri_key, (uintptr_t)pub_key, (uintptr_t)signature);
    memcpy(out_key_signature, signature, out_key_signature_len);
    
    const auto in_pub_key = std::string(in_key, in_key + in_key_len);
    
    const auto pri = std::string(pri_key, pri_key + sizeof(pri_key));
    const auto shared = make_shared_key(pri, in_pub_key);
    eapp_print("shared key: \n");
                        for (int i = 0; i < 64; i++) {
                            eapp_print("%x\n",
                                   (unsigned char)shared[i]);
                        }

    memcpy(out_key, pub_key, out_key_len);

    std::vector<char> shared_key(shared.begin(), shared.end());
    shared_key.resize(shared_key.size() + 200);
    int sealed_len = seal_data_inplace(shared_key.data(), shared_key.size(), shared.size());
    memcpy(out_encrypted_shared_key, shared_key.data(), sealed_len);
    return sealed_len;
}

