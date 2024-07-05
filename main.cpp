#include "dmalloc.h"

int main() {
  uint64_t memory_pool_size = 5*1000;
  dmalloc::Dmalloc dmalloc(memory_pool_size);
  
  void* ptr = dmalloc.Malloc(1000);
  dmalloc.Free(ptr);

  return 0;
}