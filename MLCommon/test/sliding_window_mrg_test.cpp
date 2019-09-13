#include <Tools/SlidingWindowMgr.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

TEST(ToolsTests_slidingWindowMgr, checkAddWindow) {
  const std::string WINDOW_NAME{"Win1"};
  const size_t WINDOW_SIZE{2};

  using MgrType = ml::SlidingWindowMgr<int, int>;

  MgrType mgr;
  mgr.addWindow(WINDOW_NAME, WINDOW_SIZE);
  MgrType::WindowType& window = mgr.getWindow(WINDOW_NAME);
  EXPECT_EQ(WINDOW_SIZE, window.getCleanupPolicy().getHistoryDepth());
}

TEST(ToolsTests_slidingWindowMgr, checkAddData) {
  const std::string WINDOW_NAME{"Win1"};
  const size_t WINDOW_SIZE{2};
  const int EXP_DATA{123};
  const int EXP_META{56};

  using MgrType = ml::SlidingWindowMgr<int, int>;

  MgrType mgr;
  mgr.addWindow(WINDOW_NAME, WINDOW_SIZE);
  mgr.addData(WINDOW_NAME, EXP_META, EXP_DATA);

  MgrType::WindowType& window = mgr.getWindow(WINDOW_NAME);
  window.accessStorage([&](
      const MgrType::WindowType::BaseType::SlidingWindowStorageType& storage) {
    EXPECT_EQ(1, storage.size());
    auto it = storage.begin();
    MgrType::WindowType::BaseType::DataWithMetaShrPtr data = *it;
    EXPECT_EQ(EXP_META, data->first);
    EXPECT_EQ(EXP_DATA, data->second);
  });
}

TEST(ToolsTests_slidingWindowMgr, checkGetWindowFail) {
  const std::string WINDOW_NAME{"Win1"};
  using MgrType = ml::SlidingWindowMgr<int, int>;

  MgrType mgr;
  EXPECT_THROW(mgr.getWindow(WINDOW_NAME), std::runtime_error);
}

TEST(ToolsTests_slidingWindowMgr, checkAddDataFail) {
  const std::string WINDOW_NAME{"Win1"};
  using MgrType = ml::SlidingWindowMgr<int, int>;

  MgrType mgr;
  EXPECT_THROW(mgr.addData(WINDOW_NAME, 10, 10), std::runtime_error);
}
