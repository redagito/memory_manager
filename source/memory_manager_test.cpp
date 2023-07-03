#include "gtest/gtest.h"
#include "memory/memory_manager.h"

class MemoryManagerTest : public ::testing::Test {
protected:
  memory::memory_manager* mm;

  void SetUp() override {
    mm = new memory::memory_manager();
  }

  void TearDown() override {
    delete mm;
  }
};

TEST_F(MemoryManagerTest, ConstructorTest) {
  // Test cases for constructor
}

TEST_F(MemoryManagerTest, AllocateTest) {
  // Test cases for allocate method
}

TEST_F(MemoryManagerTest, DeallocateTest) {
  // Test cases for deallocate method
}

TEST_F(MemoryManagerTest, CurrentSizeTest) {
  // Test cases for current_size method
}
