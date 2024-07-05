#include "dmalloc.h"

#include <iostream>

namespace dmalloc {

Dmalloc::Dmalloc(): total_size_{5*1024*1024}, memory_{nullptr} {
  init();
}

Dmalloc::Dmalloc(uint64_t size): total_size_{size}, memory_{nullptr} {
  init();
}

Dmalloc::~Dmalloc() {
  deinit();
}

void Dmalloc::init() {
  // 从系统申请一块匿名的私有内存
  memory_ = mmap(nullptr, total_size_, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (memory_ == MAP_FAILED) {
    throw std::runtime_error("mmap failed");
  }
  memset(memory_, 0, total_size_);
  head_ = (Block*)memory_;
  head_->next = nullptr;
  head_->is_free = true;
  head_->size = total_size_ - sizeof(Block); // 实际可用内存大小
}

void Dmalloc::deinit() {
  head_ = nullptr;
  if (munmap(memory_, total_size_) == -1) {
    throw std::runtime_error("munmap failed");
  }
  memory_ = nullptr;
  total_size_ = 0;
}

void* Dmalloc::Malloc(uint64_t need_size) {
  Block* cur = head_;
  Block* prev = nullptr;
  void* result_ptr = nullptr;

  do {
    if (cur->is_free && cur->size >= need_size) {
      // 重新计算分配need_size后当前内存块的可用内存size
      cur->size = cur->size - need_size - sizeof(Block);

      // 为新内存块元数据指定起始地址，元数据紧邻返回用户的内存块
      Block* new_block_ptr = (Block*)((void*)cur + sizeof(Block) + cur->size);
      new_block_ptr->is_free = false;
      new_block_ptr->size = need_size;
      Block* tmp = cur->next;
      cur->next = new_block_ptr;
      new_block_ptr->next = tmp;

      // 分配完新内存后，当前内存块的剩余空间，不足以再为用户分配任何可用内存
      // 解除Block，将剩余内存并入前一个Block管理的内存区域
      if (cur->size <= sizeof(Block) && prev != nullptr) {
        prev->next = cur->next;
      }
      result_ptr = (void*)(++new_block_ptr);
      break;
    }

    prev = cur;
    cur = cur->next;
  } while (cur != nullptr);

  return result_ptr;
}

void Dmalloc::Free(void* ptr) {
  Block* wait_free_block = (Block*)(ptr - sizeof(Block));
  Block* prev = head_;
  Block* cur = head_;
  do {
    if (cur == wait_free_block) {
      cur->is_free = true;
      prev->next = cur->next;
      prev->size += cur->size + sizeof(Block);
      break;
    }
    prev = cur;
    cur = cur->next;
  } while (cur != nullptr);
}

void Dmalloc::merge(Block* prev, Block* be_freed_block) {}

} // namespace dmalloc
