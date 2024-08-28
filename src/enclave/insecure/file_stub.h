#include <TEE-Capability/common.h>
#include <fcntl.h>

extern "C" size_t read_file(size_t f, size_t size, size_t n, char *out_buffer,
                            int out_buffer_len);
extern "C" size_t open_file(char *filename, int filename_len);
extern "C" size_t close_file(size_t f);
extern "C" long int tell_file(size_t f);
extern "C" int seek_file(size_t f, long int offset, int whence);
