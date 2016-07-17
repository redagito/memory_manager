#include "memory/memory_manager.h"

#include <cstdlib>

// Note that internal allocation/deallocation happens with
// malloc/free to provide the possibility of overloading
// the global new/delete operators

// Invalid index constant
static const int32_t invalid_index = -1;

// Returns the pool index for the provided byte_size and returns the next power
// of two of byte_size in real_size_out.
// TODO Currently implemented for 16 pool sizes
static int32_t byte_size_to_index(uint32_t byte_size, uint32_t &real_size_out) {
  // TODO Improve speed
  if (byte_size <= 32) {
    real_size_out = 32;
    return 0;
  }
  if (byte_size <= 64) {
    real_size_out = 64;
    return 1;
  }
  if (byte_size <= 128) {
    real_size_out = 128;
    return 2;
  }
  if (byte_size <= 256) {
    real_size_out = 256;
    return 3;
  }
  if (byte_size <= 512) {
    real_size_out = 512;
    return 4;
  }
  if (byte_size <= 1024) {
    real_size_out = 1024;
    return 5;
  }
  if (byte_size <= 2 * 1024) {
    real_size_out = 2 * 1024;
    return 6;
  }
  if (byte_size <= 4 * 1024) {
    real_size_out = 4 * 1024;
    return 7;
  }
  if (byte_size <= 8 * 1024) {
    real_size_out = 8 * 1024;
    return 8;
  }
  if (byte_size <= 16 * 1024) {
    real_size_out = 16 * 1024;
    return 9;
  }
  if (byte_size <= 32 * 1024) {
    real_size_out = 32 * 1024;
    return 10;
  }
  if (byte_size <= 64 * 1024) {
    real_size_out = 64 * 1024;
    return 11;
  }
  if (byte_size <= 128 * 1024) {
    real_size_out = 128 * 1024;
    return 12;
  }
  if (byte_size <= 256 * 1024) {
    real_size_out = 256 * 1024;
    return 13;
  }
  if (byte_size <= 512 * 1024) {
    real_size_out = 512 * 1024;
    return 14;
  }
  if (byte_size <= 1024 * 1024) {
    real_size_out = 1024 * 1024;
    return 15;
  }
  real_size_out = byte_size;
  return -1;
}

namespace memory {

memory_manager::memory_manager(uint32_t max_pool_size)
    : m_max_pool_size(max_pool_size) {}

memory_manager::~memory_manager() {
  // Clear pool
  for (int i = 0; i < 16; ++i) {
    while (m_pool[i].m_first != nullptr) {
      memory_node *node = m_pool[i].m_first;
      m_pool[i].m_first = node->m_next;
      free(node);
    }
  }
}

void *memory_manager::allocate(uint32_t byte_size) {
  // Allocation happens by either retrieving an unused
  // node from the pool with sufficient memory size
  // or by creating a new node.

  uint32_t real_size = 0;
  // Calculate index for the provided byte size
  int32_t index = byte_size_to_index(byte_size, real_size);

  // Check index valid
  if (index != invalid_index) {
    // Check pool index available
    if (m_pool[index].m_first != nullptr) {
      // At least a single node exists in the pool
      // Retrieve first node for the index
      memory_node *node = m_pool[index].m_first;

      // Set second node as front node
      // detaching the front node
      m_pool[index].m_first = node->m_next;

      // Still nodes in the pool index?
      if (m_pool[index].m_first != nullptr) {
        // Remove binding to previous node
        m_pool[index].m_first->m_prev = nullptr;
      } else {
        // Removed the only node in the pool
        m_pool[index].m_last = nullptr;
      }

      // Recalculate current pool size
      m_current_pool_size -= real_size;

      // Increment usage
      ++node->m_usage;
      return node->m_mem;
    }
  }

  // No nodes available
  // Allocates a node consisting of the requested size, the size of a
  // memory_node and
  // without the size of the m_mem member.
  // Basically creates memory_node with additional memory which can be accessed
  // with the
  // memory address located at m_mem.
  memory_node *node =
      (memory_node *)malloc(real_size + sizeof(memory_node) - sizeof(uint32_t));

  if (node == nullptr) {
    // Failed to create
    return nullptr;
  }

  // Initialize newly created memory node
  // Note that the index might not be valid at this point
  node->m_index = index;
  node->m_size = real_size;
  node->m_next = nullptr;
  node->m_prev = nullptr;

  // Usage count is zero, increased on each subsequent reuse
  node->m_usage = 0;
  return node->m_mem;
}

void memory_manager::deallocate(void *memory) {
  // memory points to the location of m_mem member in the tag node
  // The real memory location of the memory_node is calculated from there
  memory_node *node = (memory_node *)(static_cast<char *>(memory) -
                                      sizeof(memory_node) + sizeof(uint32_t));

  // Valid index?
  if (node->m_index != invalid_index) {
    // Add back the node to the pool if it does not exceed the maximum pool size
    if (node->m_size + m_current_pool_size <= m_max_pool_size) {
      // Node is added as first node, no predecessor
      node->m_prev = nullptr;

      // Add first node of the pool as successor
      node->m_next = m_pool[node->m_index].m_first;

      // Pool index not not empty?
      if (m_pool[node->m_index].m_first) {
        // Link back with first node in pool
        m_pool[node->m_index].m_first->m_prev = node;
      } else {
        // Pool empty, added node is first and last node
        m_pool[node->m_index].m_last = node;
      }

      // Update first node in the pool
      m_pool[node->m_index].m_first = node;

      // Update pool size
      m_current_pool_size += node->m_size;
      return;
    } else if (node->m_usage > 0) {
      // Node has been reused and adding would exceed the maximum
      // pool storage.
      // Trigger garbage collector with the node usage count to clean up pool
      // and try to collect twice the size of the current node
      garbage_collect(node->m_size * 2, node->m_usage);
    }
  }
  // Delete it
  free(node);
}

void memory_manager::garbage_collect(uint32_t expected_size,
                                     uint32_t usage_count) {
  // Freed memory size
  uint32_t free_size = 0;

  // Loop over the available pools in reverse (most memory first)
  for (int i = 15; i >= 0; --i) {
    if (m_pool[i].m_first == nullptr) {
      // Pool empty at index, nothing to garbage collect
      continue;
    }

    // Found pool with nodes, traverse in reverse
    memory_node *prev_node = m_pool[i].m_last;
    while (prev_node != nullptr) {
      // Store current node
      memory_node *current_node = prev_node;

      // Set to previous node
      prev_node = current_node->m_prev;

      // Check usage of the current node
      if (current_node->m_usage < usage_count) {
        // Usage is less than minimum usage count
        // Previous node is valid?
        if (prev_node != nullptr) {
          // Connect previous node with next node
          prev_node->m_next = current_node->m_next;
        }

        // Current node has a valid next node?
        if (current_node->m_next != nullptr) {
          // Connect next node with previous node
          current_node->m_next->m_prev = prev_node;
        }

        // Update pool pointer
        // Current node is the last node of the pool?
        if (m_pool[i].m_last == current_node) {
          // Update to previous node
          m_pool[i].m_last = prev_node;
        }
        // Current node is the first node of
        if (m_pool[i].m_first == current_node) {
          // Update to node after current node
          m_pool[i].m_first = current_node->m_next;
        }

        // At this point, all pointers to the current_node have
        // been reset, removing the node from the pool
        // Update pool size
        m_current_pool_size -= current_node->m_size;

        // Update freed byte count
        free_size += current_node->m_size;

        // Actually delete node
        free(current_node);
      }

      // Expected size requirement met?
      if (free_size >= expected_size) {
        return;
      }
    }
  }
}
}
