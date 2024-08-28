#include "TEE-Capability/dtee_sdk.h"
#include "file_stub.h"
int main() {
  (void)read_file;
  (void)close_file;
  (void)open_file;
  (void)tell_file;
  (void)seek_file;
  auto ctx = init_distributed_tee_context(
      {.side = SIDE::Server, .mode = MODE::ComputeNode});
  dtee_server_run(ctx);
  destroy_distributed_tee_context(ctx);
}
