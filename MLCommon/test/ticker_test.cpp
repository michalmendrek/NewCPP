#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Tools/Ticker.h"

static constexpr int ExpectedValue = 5;
static constexpr int ExpectedTimeout = 2;

TEST(ToolsTests_ticker, generalTest) {
  int a = 0;
  Ticker ticker([&a]() { a = ExpectedValue; });
  ticker.SetInterval(std::chrono::seconds(ExpectedTimeout));
  EXPECT_EQ(a, 0);
  ticker.Start();
  const auto &beforeTest = time(nullptr);
  while (a != ExpectedValue) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  const auto &afterTest = time(nullptr);
  EXPECT_EQ(afterTest - beforeTest, ExpectedTimeout);
  EXPECT_TRUE(ticker.IsRunning());
  ticker.Stop();
}

TEST(ToolsTests_ticker, singleShotTest) {
  int a = 0;
  Ticker ticker([&a]() { a = ExpectedValue; }, true);
  ticker.SetInterval(std::chrono::seconds(ExpectedTimeout));
  EXPECT_EQ(a, 0);
  ticker.Start();
  const auto &beforeTest = time(nullptr);
  while (a != ExpectedValue) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  const auto &afterTest = time(nullptr);
  EXPECT_EQ(afterTest - beforeTest, ExpectedTimeout);
  EXPECT_FALSE(ticker.IsRunning());
  a = 0;
  auto now = time(nullptr);
  while (time(nullptr) - now <= ExpectedTimeout) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  EXPECT_EQ(a, 0);
}

TEST(ToolsTests_ticker, misconfiguredTicker) {
  Ticker ticker;
  ticker.Start();
  EXPECT_FALSE(ticker.IsRunning());
}
