#include <MLController/MLController.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ml {

class MockMLController : public MLController {
 public:
  MOCK_METHOD0(printHelloWorld,
      void());
};

}  // namespace ml

TEST(MLControllerTest, testMain) {
  ml::MockMLController ctrl;
  EXPECT_CALL(ctrl, printHelloWorld());
  ctrl.mlControllerMain();
}
