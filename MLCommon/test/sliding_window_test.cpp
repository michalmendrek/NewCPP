#include <Tools/SlidingWindow.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

template <typename DATA_TYPE, typename METADATA_TYPE>
class EmptyCleanupPolicy {
 public:
  using Storage = typename ml::SlidingWindowContainer<
      DATA_TYPE, METADATA_TYPE>::SlidingWindowStorageType;
  void cleanup(Storage& storage, typename Storage::iterator& nextReadElement) {
    (void)storage;
    (void)nextReadElement;
  }
};

TEST(ToolsTests, checkEmptyData) {
  using SlidingWindowType = ml::SlidingWindow<int, int, EmptyCleanupPolicy>;
  EmptyCleanupPolicy<int, int> policy;
  SlidingWindowType sw{policy};
  EXPECT_FALSE(sw.HasNextData());
}

TEST(ToolsTests, checkAddData) {
  using SlidingWindowType = ml::SlidingWindow<int, int, EmptyCleanupPolicy>;
  EmptyCleanupPolicy<int, int> policy;
  SlidingWindowType sw{policy};
  sw.AddData(10, 10);
  EXPECT_TRUE(sw.HasNextData());
}

TEST(ToolsTests, checkPullData) {
  constexpr int EXPECTED_DATA = 898;
  constexpr int EXPECTED_META = 123;

  using SlidingWindowType = ml::SlidingWindow<int, int, EmptyCleanupPolicy>;
  EmptyCleanupPolicy<int, int> policy;
  SlidingWindowType sw{policy};
  sw.AddData(EXPECTED_META, EXPECTED_DATA);
  auto result = sw.GetNextData();

  EXPECT_EQ(EXPECTED_META, result->first);
  EXPECT_EQ(EXPECTED_DATA, result->second);
}

TEST(ToolsTests, checkPullNoData) {
  using SlidingWindowType = ml::SlidingWindow<int, int, EmptyCleanupPolicy>;
  EmptyCleanupPolicy<int, int> policy;
  SlidingWindowType sw{policy};
  auto result = sw.GetNextData();

  EXPECT_FALSE(result);
}

TEST(ToolsTests, checkStorageVisitor) {
  const int EXP_DATA{123};
  const int EXP_META{729};
  using SlidingWindowType = ml::SlidingWindow<int, int, EmptyCleanupPolicy>;
  EmptyCleanupPolicy<int, int> policy;
  SlidingWindowType sw{policy};
  sw.AddData(EXP_META, EXP_DATA);

  sw.accessStorage([=](
      const SlidingWindowType::BaseType::SlidingWindowStorageType& storage) {
    EXPECT_EQ(1, storage.size());
    auto it = storage.begin();
    SlidingWindowType::BaseType::DataWithMetaShrPtr data = *it;
    EXPECT_EQ(EXP_META, data->first);
    EXPECT_EQ(EXP_DATA, data->second);
  });
}

TEST(ToolsTests, checkMaxHistoryCleanupPolicy) {
  using Helper = ml::SlidingWindowContainer<int, int>;
  using StorageType = Helper::SlidingWindowStorageType;
  StorageType storage;
  for (int t = 0; t < 10; t++) {
    storage.push_back(Helper::pack(t, t));
  }
  StorageType::iterator readIter = storage.begin();
  readIter += 5;

  ml::MaxHistoryCleanupPolicy<int, int> policy{3};
  policy.cleanup(storage, readIter);

  EXPECT_EQ(8, storage.size());
}

TEST(ToolsTests, checkMaxHistoryCleanupPolicyNoData) {
  using StorageType =
      ml::SlidingWindowContainer<int, int>::SlidingWindowStorageType;
  StorageType storage;
  StorageType::iterator readIter = storage.end();

  ml::MaxHistoryCleanupPolicy<int, int> policy{3};
  policy.cleanup(storage, readIter);
}
