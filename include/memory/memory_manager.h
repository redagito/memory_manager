#pragma once

#include <cstdint>

namespace memory {

/**
 * Simple memory pool implementation.
 */
class memory_manager {
public:
  /**
   * Default maximum size 32 MB.
   */
  memory_manager(uint32_t max_size = 32 * 1024 * 1024);
  ~memory_manager();

  /**
   * Attempts to allocate the requested numbers of bytes and returns a pointer
   * to its memory location.
   */
  void *allocate(uint32_t byte_size);

  /**
   * Deallocates a previously allocated memory block.
   */
  void deallocate(void *memory);

  /**
   * Returns the current pool size in bytes.
   */
  uint32_t current_size() const { return m_current_pool_size; }

private:
  /**
   * Deletes nodes in the free list with usage count lower than min_usage_count.
   * Stops when either more or equal memory than expected_size is freed or the
   * whole free list is traversed.
   */
  void garbage_collect(uint32_t expected_size, uint32_t min_usage_count);

  // Stores information for memory chunks
  struct memory_node {
    memory_node *m_next = nullptr;
    memory_node *m_prev = nullptr;
    uint32_t m_index = 0;
    uint32_t m_size = 0;
    uint32_t m_usage = 0;
    uint32_t m_mem[1]; //< Must stay last
  };

  // Stores list of free nodes
  struct memory_pool {
    memory_node *m_first = nullptr;
    memory_node *m_last = nullptr;
  };

  // Internal memory pool
  memory_pool m_pool[16];

  // The maximum pool size in bytes
  uint32_t m_max_pool_size = 0;

  // The current pool size in bytes
  uint32_t m_current_pool_size = 0;
};
}
