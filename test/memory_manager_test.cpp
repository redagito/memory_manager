#include "memory/memory_manager.h"
#include <gtest/gtest.h>

class MemoryManagerTest : public ::testing::Test {
protected:
  memory::memory_manager *mm;

  void SetUp() override {
    mm = new memory::memory_manager();
  }

  void TearDown() override {
    delete mm;
  }
};

TEST_F(MemoryManagerTest, ConstructorTest) {
  // Test the constructor here
}

TEST_F(MemoryManagerTest, DestructorTest) {
  // Test the destructor here
}

TEST_F(MemoryManagerTest, AllocateTest) {
  // Test the allocate method here
}

TEST_F(MemoryManagerTest, DeallocateTest) {
  // Test the deallocate method here
}

TEST_F(MemoryManagerTest, GarbageCollectTest) {
  // Test the garbage_collect method here
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
