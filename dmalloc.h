#ifndef D_MALLOC_H_
#define D_MALLOC_H_

#include <cstdint>
#include <cstring>
#include <sys/mman.h>

namespace dmalloc {

class Dmalloc {
 public:
  Dmalloc();
  explicit Dmalloc(uint64_t total_size);
  ~Dmalloc();

  /// @brief 开辟一块内存
  /// @param size 需要开辟的内存大小
  /// @return 开辟的内存地址
  void* Malloc(uint64_t size);

  /// @brief 释放一块内存
  /// @param ptr 需要释放的内存地址 
  void Free(void* ptr);

 private:
  /// @brief 描述内存块的元数据
  struct Block {
    uint64_t size; // 实际可用内存
    bool is_free; // 是否空闲
    Block* next; // 下一个内存块
  };

  /// @brief 初始化总的内存
  void init();

  /// @brief 释放初始化的内存
  void deinit();

  /// @brief 合并两块相邻的内存
  /// @param prior_ptr 前一块内存
  /// @param next_ptr  后一块内存
  void merge(Block* prior_ptr, Block* next_ptr);

  Block* head_;
  void* memory_;
  uint64_t total_size_;
  
};

} // namespace dmalloc
#endif // D_MALLOC_H_